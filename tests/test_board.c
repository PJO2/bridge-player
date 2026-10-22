#include "bridge/deal.h"
#include "check.h"

int main(void)
{
    /* Independent fixtures for boards 1..16, in international order. */
    static const bridge_board_info expected[16] = {
        {BRIDGE_NORTH, BRIDGE_VUL_NONE}, {BRIDGE_EAST, BRIDGE_VUL_NS},
        {BRIDGE_SOUTH, BRIDGE_VUL_EW}, {BRIDGE_WEST, BRIDGE_VUL_BOTH},
        {BRIDGE_NORTH, BRIDGE_VUL_NS}, {BRIDGE_EAST, BRIDGE_VUL_EW},
        {BRIDGE_SOUTH, BRIDGE_VUL_BOTH}, {BRIDGE_WEST, BRIDGE_VUL_NONE},
        {BRIDGE_NORTH, BRIDGE_VUL_EW}, {BRIDGE_EAST, BRIDGE_VUL_BOTH},
        {BRIDGE_SOUTH, BRIDGE_VUL_NONE}, {BRIDGE_WEST, BRIDGE_VUL_NS},
        {BRIDGE_NORTH, BRIDGE_VUL_BOTH}, {BRIDGE_EAST, BRIDGE_VUL_NONE},
        {BRIDGE_SOUTH, BRIDGE_VUL_NS}, {BRIDGE_WEST, BRIDGE_VUL_EW}
    };
    for (uint32_t board = 1; board <= 160; ++board) {
        bridge_board_info info;
        CHECK(bridge_board_get_info(board, &info));
        const bridge_board_info *fixture = &expected[(board - 1) % 16];
        CHECK(info.dealer == fixture->dealer);
        CHECK(info.vulnerability == fixture->vulnerability);
    }
    /* Boundary: UINT32_MAX is congruent to board 15 modulo 16. */
    bridge_board_info info;
    CHECK(bridge_board_get_info(UINT32_MAX, &info));
    CHECK(info.dealer == BRIDGE_SOUTH);
    CHECK(info.vulnerability == BRIDGE_VUL_NS);
    info.dealer = BRIDGE_WEST;
    info.vulnerability = BRIDGE_VUL_BOTH;
    CHECK(!bridge_board_get_info(0, &info));
    CHECK(info.dealer == BRIDGE_WEST);
    CHECK(info.vulnerability == BRIDGE_VUL_BOTH);
    CHECK(!bridge_board_get_info(1, NULL));
    CHECK(!bridge_board_get_info(0, NULL));
    return EXIT_SUCCESS;
}
