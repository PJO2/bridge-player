#include "bridge/deal.h"

#include <stddef.h>

bool bridge_deal_is_valid(const bridge_deal *deal)
{
    if (deal == NULL) {
        return false;
    }
    uint64_t seen = 0;
    int points = 0;
    for (int player = 0; player < BRIDGE_PLAYER_COUNT; ++player) {
        const bridge_hand *hand = &deal->hands[player];
        if (bridge_hand_count(hand) != BRIDGE_HAND_SIZE ||
            (seen & hand->cards) != 0) {
            return false;
        }
        seen |= hand->cards;
        points += bridge_hand_hcp(hand);
    }
    return seen == BRIDGE_DECK_MASK && points == 40;
}

bool bridge_board_get_info(uint32_t board_number, bridge_board_info *out)
{
    static const bridge_vulnerability cycle[16] = {
        BRIDGE_VUL_NONE, BRIDGE_VUL_NS, BRIDGE_VUL_EW, BRIDGE_VUL_BOTH,
        BRIDGE_VUL_NS, BRIDGE_VUL_EW, BRIDGE_VUL_BOTH, BRIDGE_VUL_NONE,
        BRIDGE_VUL_EW, BRIDGE_VUL_BOTH, BRIDGE_VUL_NONE, BRIDGE_VUL_NS,
        BRIDGE_VUL_BOTH, BRIDGE_VUL_NONE, BRIDGE_VUL_NS, BRIDGE_VUL_EW
    };
    if (board_number == 0 || out == NULL) {
        return false;
    }
    uint32_t index = board_number - 1u;
    out->dealer = (bridge_player)(index % BRIDGE_PLAYER_COUNT);
    out->vulnerability = cycle[index % 16u];
    return true;
}
