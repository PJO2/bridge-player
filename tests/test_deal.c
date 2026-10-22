#include "bridge/deal.h"
#include "check.h"

static bridge_deal suit_deal(void)
{
    bridge_deal deal = {0};
    for (unsigned card = 0; card < 52; ++card) {
        CHECK(bridge_hand_add(&deal.hands[card / 13], (bridge_card)card));
    }
    return deal;
}

static void test_complete_deals(void)
{
    bridge_deal deal = suit_deal();
    CHECK(bridge_deal_is_valid(&deal));
    for (int player = 0; player < 4; ++player) {
        CHECK(bridge_hand_count(&deal.hands[player]) == 13);
        CHECK(bridge_hand_hcp(&deal.hands[player]) == 10);
    }
    /* 17 is coprime to 52: each permutation deals every card once. */
    for (unsigned offset = 0; offset < 52; ++offset) {
        bridge_deal varied = {0};
        for (unsigned i = 0; i < 52; ++i) {
            bridge_card card = (bridge_card)((i * 17 + offset) % 52);
            CHECK(bridge_hand_add(&varied.hands[i % 4], card));
        }
        int total = 0;
        for (int player = 0; player < 4; ++player) {
            CHECK(bridge_hand_count(&varied.hands[player]) == 13);
            total += bridge_hand_hcp(&varied.hands[player]);
        }
        CHECK(total == 40);
        CHECK(bridge_deal_is_valid(&varied));
    }
}

static void test_invalid_deals(void)
{
    CHECK(!bridge_deal_is_valid(NULL));
    bridge_deal empty = {0};
    CHECK(!bridge_deal_is_valid(&empty));
    bridge_deal valid = suit_deal();
    for (int player = 0; player < 4; ++player) {
        bridge_deal invalid = valid;
        invalid.hands[player].cards = 0;
        CHECK(!bridge_deal_is_valid(&invalid));
        for (unsigned bit = 52; bit < 64; ++bit) {
            invalid = valid;
            invalid.hands[player].cards |= UINT64_C(1) << bit;
            CHECK(!bridge_deal_is_valid(&invalid));
        }
    }
    /* Duplicate C2, missing D2: still 13 each and 40 HCP. */
    bridge_deal invalid = valid;
    invalid.hands[BRIDGE_EAST].cards ^= (UINT64_C(1) << 13) | UINT64_C(1);
    CHECK(bridge_hand_count(&invalid.hands[BRIDGE_EAST]) == 13);
    CHECK(bridge_hand_hcp(&invalid.hands[BRIDGE_EAST]) == 10);
    CHECK(!bridge_deal_is_valid(&invalid));
    /* Move C2 from N to E: still 52 distinct cards and 40 HCP, but 12/14. */
    invalid = valid;
    invalid.hands[BRIDGE_NORTH].cards &= ~UINT64_C(1);
    invalid.hands[BRIDGE_EAST].cards |= UINT64_C(1);
    CHECK(bridge_hand_count(&invalid.hands[BRIDGE_NORTH]) == 12);
    CHECK(bridge_hand_count(&invalid.hands[BRIDGE_EAST]) == 14);
    CHECK(!bridge_deal_is_valid(&invalid));
    invalid = valid;
    invalid.hands[BRIDGE_NORTH].cards &= ~(UINT64_C(1) << 12);
    CHECK(!bridge_deal_is_valid(&invalid));
    invalid = valid;
    invalid.hands[BRIDGE_WEST] = invalid.hands[BRIDGE_NORTH];
    CHECK(!bridge_deal_is_valid(&invalid));
}

int main(void)
{
    test_complete_deals();
    test_invalid_deals();
    return EXIT_SUCCESS;
}
