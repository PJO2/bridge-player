#ifndef BRIDGE_DEAL_H
#define BRIDGE_DEAL_H

#include "bridge/cards.h"

/* Hand indices follow bridge_player (North, East, South, West). */
typedef struct {
    bridge_hand hands[BRIDGE_PLAYER_COUNT];
} bridge_deal;

typedef enum {
    BRIDGE_VUL_NONE = 0, BRIDGE_VUL_NS, BRIDGE_VUL_EW, BRIDGE_VUL_BOTH
} bridge_vulnerability;

typedef struct {
    bridge_player dealer;
    bridge_vulnerability vulnerability;
} bridge_board_info;

/* Require 13 cards per seat, no out-of-deck bits, no shared cards,
 * all 52 cards present and 40 total HCP. NULL is invalid. */
bool bridge_deal_is_valid(const bridge_deal *deal);

/* Board numbers are 1-based, through UINT32_MAX. Dealer repeats every
 * four boards (N,E,S,W); vulnerability repeats every 16 boards.
 * Reject board 0 or NULL output, leaving output unchanged. */
bool bridge_board_get_info(uint32_t board_number, bridge_board_info *out);

#endif
