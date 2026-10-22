# Bridge Player

Programme de bridge en C11. Le périmètre implémenté est **DEV-001 : modèle des
cartes et des donnes**. La génération aléatoire, le moteur Double Dummy et le
calcul du PAR ne sont pas encore implémentés.

## Compiler et tester

Prérequis : compilateur C11, CMake >= 3.16 et un outil de compilation compatible
(Make ou Ninja). Aucune dépendance externe.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

La cible `bridge` produit une bibliothèque statique. GCC et Clang utilisent
`-Wall -Wextra -Werror -Wpedantic`, le standard C11 et aucune extension C.
Les vérifications des tests restent actives en Release.
Le workflow existant `.github/workflows/ci.yml` exécute ces trois étapes.

Pour une vérification avec Clang et les sanitizers :

```sh
cmake -S . -B build/sanitizers -DCMAKE_C_COMPILER=clang \
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON \
  '-DCMAKE_C_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer' \
  '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined'
cmake --build build/sanitizers --parallel
ctest --test-dir build/sanitizers --output-on-failure
```

## Représentation et API

Les interfaces publiques et leurs contrats d'erreur sont documentés dans
`include/bridge/cards.h` et `include/bridge/deal.h`.

- Les cartes ont les identifiants 0 à 51 : trèfle, carreau, cœur, pique,
  avec les rangs 2 à As dans chaque couleur. Les cinq dénominations ont
  leur propre type, incluant sans-atout.
- Une main contient un masque `uint64_t` : le bit i désigne la carte i.
  Les 12 bits supérieurs sont interdits. Une main vide s'initialise par `{0}`.
  Le masque permet des intersections et extractions de couleurs efficaces
  pour un futur moteur DD, sans allocation dynamique. Il n'utilise pas
  l'encodage de DDS.
- Les fonctions de comptage et de points H acceptent les mains partielles.
  L'ajout refuse les doublons et les mains déjà pleines, sans mutation en
  cas d'erreur. La validation d'une donne exige exactement 13 cartes par
  joueur, les 52 cartes distinctes et 40 points H.
- Les quatre mains suivent l'ordre Nord, Est, Sud, Ouest. Les métadonnées
  du numéro de donne sont séparées des cartes : donneur et vulnérabilité
  sont dérivés, sans risque de désynchronisation de champs stockés.
- Les numéros de donne vont de 1 à `UINT32_MAX`. Zéro est rejeté. Le cycle
  international de vulnérabilité est documenté par l'ACBL :
  <https://www.acbl.org/learn/> (glossaire, « Vulnerability »).

Exemple minimal :

```c
#include "bridge/deal.h"

int main(void)
{
    bridge_hand hand = {0};
    bridge_card ace;
    bridge_board_info board;
    if (!bridge_card_make(BRIDGE_SPADES, BRIDGE_ACE, &ace) ||
        !bridge_hand_add(&hand, ace) || !bridge_board_get_info(17, &board)) {
        return 1;
    }
    return bridge_hand_hcp(&hand) == 4 && board.dealer == BRIDGE_NORTH &&
           board.vulnerability == BRIDGE_VUL_NONE ? 0 : 1;
}
```

## Traçabilité DEV-001

| Exigence | Implémentation | Tests CTest / scénarios |
|---|---|---|
| 52 cartes valides et distinctes | `cards.h`, `bridge_card_make` | `cards` : toutes les couleurs/rangs, 256 valeurs possibles de carte |
| Quatre joueurs, cinq dénominations | `bridge_player`, `bridge_denomination` | `cards` : valeurs et cardinalités |
| Quatre mains de 13 cartes | `bridge_deal`, `bridge_hand_count` | `deal` : donne par couleurs et 52 répartitions déterministes |
| Points H : As=4, Roi=3, Dame=2, Valet=1 | `bridge_card_hcp`, `bridge_hand_hcp` | `cards` : chaque carte, mains connues à 0, 10, 17 et 37 H |
| Total de 40 H | `bridge_deal_is_valid` | `cards`, `deal` : paquet complet et répartitions complètes |
| Donneur N/E/S/O | `bridge_board_get_info` | `board` : les 16 donneurs, puis dix cycles |
| Vulnérabilité internationale | `bridge_board_get_info` | `board` : table attendue des 16 donnes |
| Répétition dès la donne 17 | `bridge_board_get_info` | `board` : donnes 1 à 160 et borne `UINT32_MAX` |
| Rejet des données invalides | Contrôles des API | `cards`, `deal`, `board` : NULL, numéro zéro, rang/couleur/carte invalides, doublons, cartes manquantes, 12/14 cartes, bits hors paquet |

Les suites `cards` et `board` vérifient les fonctions élémentaires ; `deal`
vérifie également leur intégration sur des donnes complètes. Aucun test
d'acceptation préexistant n'a été modifié.
