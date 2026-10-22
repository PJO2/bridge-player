#include "bridge/cards.h"

#include <stddef.h>

bool bridge_card_make(bridge_suit suit, bridge_rank rank, bridge_card *out)
{
    if (out == NULL || (unsigned)suit > BRIDGE_SPADES ||
        rank < BRIDGE_TWO || rank > BRIDGE_ACE) {
        return false;
    }
    *out = (bridge_card)((unsigned)suit * 13u + (unsigned)rank - 2u);
    return true;
}

bool bridge_card_is_valid(bridge_card card)
{
    return card < BRIDGE_CARD_COUNT;
}

bridge_suit bridge_card_suit(bridge_card card)
{
    return bridge_card_is_valid(card) ? (bridge_suit)(card / 13u)
                                      : BRIDGE_SUIT_INVALID;
}

bridge_rank bridge_card_rank(bridge_card card)
{
    return bridge_card_is_valid(card) ? (bridge_rank)(card % 13u + 2u)
                                      : BRIDGE_RANK_INVALID;
}

int bridge_card_hcp(bridge_card card)
{
    if (!bridge_card_is_valid(card)) {
        return -1;
    }
    int rank = (int)bridge_card_rank(card);
    return rank >= BRIDGE_JACK ? rank - 10 : 0;
}

bool bridge_hand_is_valid(const bridge_hand *hand)
{
    return hand != NULL && (hand->cards & ~BRIDGE_DECK_MASK) == 0;
}

int bridge_hand_count(const bridge_hand *hand)
{
    if (!bridge_hand_is_valid(hand)) {
        return -1;
    }
    uint64_t remaining = hand->cards;
    int count = 0;
    while (remaining != 0) {
        remaining &= remaining - UINT64_C(1);
        ++count;
    }
    return count;
}

bool bridge_hand_add(bridge_hand *hand, bridge_card card)
{
    if (!bridge_hand_is_valid(hand) || !bridge_card_is_valid(card) ||
        bridge_hand_count(hand) >= BRIDGE_HAND_SIZE) {
        return false;
    }
    uint64_t bit = UINT64_C(1) << card;
    if ((hand->cards & bit) != 0) {
        return false;
    }
    hand->cards |= bit;
    return true;
}

int bridge_hand_hcp(const bridge_hand *hand)
{
    if (!bridge_hand_is_valid(hand)) {
        return -1;
    }
    int points = 0;
    for (bridge_card card = 0; card < BRIDGE_CARD_COUNT; ++card) {
        if ((hand->cards & (UINT64_C(1) << card)) != 0) {
            points += bridge_card_hcp(card);
        }
    }
    return points;
}
