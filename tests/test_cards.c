#include "bridge/cards.h"
#include "check.h"

static void test_card_universe(void)
{
    uint64_t seen = 0;
    int total_hcp = 0;
    const int expected_hcp[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4};
    for (int suit = 0; suit < 4; ++suit) {
        for (int rank = 2; rank <= 14; ++rank) {
            bridge_card card = BRIDGE_CARD_INVALID;
            CHECK(bridge_card_make((bridge_suit)suit, (bridge_rank)rank, &card));
            CHECK(bridge_card_is_valid(card));
            CHECK(card == suit * 13 + rank - 2);
            CHECK(bridge_card_suit(card) == (bridge_suit)suit);
            CHECK(bridge_card_rank(card) == (bridge_rank)rank);
            CHECK(bridge_card_hcp(card) == expected_hcp[rank - 2]);
            CHECK((seen & (UINT64_C(1) << card)) == 0);
            seen |= UINT64_C(1) << card;
            total_hcp += bridge_card_hcp(card);
        }
    }
    CHECK(seen == BRIDGE_DECK_MASK);
    CHECK(total_hcp == 40);
    for (unsigned value = 0; value <= UINT8_MAX; ++value) {
        bridge_card card = (bridge_card)value;
        CHECK(bridge_card_is_valid(card) == (value < 52));
        if (value >= 52) {
            CHECK(bridge_card_suit(card) == BRIDGE_SUIT_INVALID);
            CHECK(bridge_card_rank(card) == BRIDGE_RANK_INVALID);
            CHECK(bridge_card_hcp(card) == -1);
        }
    }
}

static void test_invalid_construction(void)
{
    bridge_card card = 7;
    CHECK(!bridge_card_make((bridge_suit)-1, BRIDGE_ACE, &card));
    CHECK(card == 7);
    CHECK(!bridge_card_make(BRIDGE_SUIT_INVALID, BRIDGE_ACE, &card));
    CHECK(card == 7);
    CHECK(!bridge_card_make(BRIDGE_CLUBS, (bridge_rank)-1, &card));
    CHECK(!bridge_card_make(BRIDGE_CLUBS, (bridge_rank)1, &card));
    CHECK(!bridge_card_make(BRIDGE_CLUBS, BRIDGE_RANK_INVALID, &card));
    CHECK(card == 7);
    CHECK(!bridge_card_make(BRIDGE_CLUBS, BRIDGE_ACE, NULL));
    CHECK(BRIDGE_PLAYER_COUNT == 4);
    CHECK(BRIDGE_NORTH == 0 && BRIDGE_EAST == 1 &&
          BRIDGE_SOUTH == 2 && BRIDGE_WEST == 3);
    CHECK(BRIDGE_DENOMINATION_COUNT == 5);
    CHECK(BRIDGE_DENOM_CLUBS == 0 && BRIDGE_DENOM_DIAMONDS == 1 &&
          BRIDGE_DENOM_HEARTS == 2 && BRIDGE_DENOM_SPADES == 3 &&
          BRIDGE_DENOM_NOTRUMP == 4);
}

static void check_known_hand(const bridge_card cards[13], int expected)
{
    bridge_hand hand = {0};
    for (int i = 0; i < 13; ++i) {
        CHECK(bridge_hand_add(&hand, cards[i]));
    }
    CHECK(bridge_hand_count(&hand) == 13);
    CHECK(bridge_hand_hcp(&hand) == expected);
}

static void test_hands(void)
{
    /* Thirteen spades; four AKQ plus CJ; only spot cards; mixed honors. */
    const bridge_card spades[13] = {
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };
    const bridge_card maximum[13] = {
        12, 25, 38, 51, 11, 24, 37, 50, 10, 23, 36, 49, 9
    };
    const bridge_card spots[13] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 13, 14, 15, 16
    };
    /* CA CK CQ CJ; DA DK; H2 H3 H4; S2 S3 S4 S5 => 17. */
    const bridge_card mixed[13] = {
        12, 11, 10, 9, 25, 24, 26, 27, 28, 39, 40, 41, 42
    };
    check_known_hand(spades, 10);
    check_known_hand(maximum, 37);
    check_known_hand(spots, 0);
    check_known_hand(mixed, 17);

    bridge_hand hand = {0};
    CHECK(bridge_hand_is_valid(&hand));
    CHECK(bridge_hand_count(&hand) == 0);
    CHECK(bridge_hand_hcp(&hand) == 0);
    CHECK(bridge_hand_add(&hand, 51));
    CHECK(hand.cards == (UINT64_C(1) << 51));
    CHECK(bridge_hand_count(&hand) == 1);
    CHECK(bridge_hand_hcp(&hand) == 4);
    CHECK(!bridge_hand_add(&hand, 51));
    for (unsigned invalid = 52; invalid <= UINT8_MAX; ++invalid) {
        CHECK(!bridge_hand_add(&hand, (bridge_card)invalid));
        CHECK(hand.cards == (UINT64_C(1) << 51));
    }
    for (bridge_card card = 0; card < 12; ++card) {
        CHECK(bridge_hand_add(&hand, card));
    }
    uint64_t before = hand.cards;
    CHECK(!bridge_hand_add(&hand, 12));
    CHECK(hand.cards == before);
    CHECK(!bridge_hand_is_valid(NULL));
    CHECK(bridge_hand_count(NULL) == -1);
    CHECK(bridge_hand_hcp(NULL) == -1);
    CHECK(!bridge_hand_add(NULL, 0));
    for (unsigned bit = 52; bit < 64; ++bit) {
        bridge_hand invalid = {UINT64_C(1) << bit};
        CHECK(!bridge_hand_is_valid(&invalid));
        CHECK(bridge_hand_count(&invalid) == -1);
        CHECK(bridge_hand_hcp(&invalid) == -1);
        CHECK(!bridge_hand_add(&invalid, 0));
        CHECK(invalid.cards == (UINT64_C(1) << bit));
    }
}

int main(void)
{
    test_card_universe();
    test_invalid_construction();
    test_hands();
    return EXIT_SUCCESS;
}
