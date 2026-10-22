#ifndef BRIDGE_CARDS_H
#define BRIDGE_CARDS_H

#include <stdbool.h>
#include <stdint.h>

enum { BRIDGE_CARD_COUNT = 52, BRIDGE_HAND_SIZE = 13,
       BRIDGE_PLAYER_COUNT = 4, BRIDGE_DENOMINATION_COUNT = 5 };

typedef enum {
    BRIDGE_CLUBS = 0, BRIDGE_DIAMONDS, BRIDGE_HEARTS, BRIDGE_SPADES,
    BRIDGE_SUIT_INVALID
} bridge_suit;

/* Numeric values equal the printed ranks; ace is high. */
typedef enum {
    BRIDGE_TWO = 2, BRIDGE_THREE, BRIDGE_FOUR, BRIDGE_FIVE, BRIDGE_SIX,
    BRIDGE_SEVEN, BRIDGE_EIGHT, BRIDGE_NINE, BRIDGE_TEN, BRIDGE_JACK,
    BRIDGE_QUEEN, BRIDGE_KING, BRIDGE_ACE, BRIDGE_RANK_INVALID
} bridge_rank;

/* Seats are clockwise, starting at North. */
typedef enum {
    BRIDGE_NORTH = 0, BRIDGE_EAST, BRIDGE_SOUTH, BRIDGE_WEST
} bridge_player;

typedef enum {
    BRIDGE_DENOM_CLUBS = 0, BRIDGE_DENOM_DIAMONDS, BRIDGE_DENOM_HEARTS,
    BRIDGE_DENOM_SPADES, BRIDGE_DENOM_NOTRUMP
} bridge_denomination;

/* Card IDs 0..51: 13 consecutive ranks (2..A) per suit (C,D,H,S).
 * This ordering is internal to Bridge Player, not the DDS encoding. */
typedef uint8_t bridge_card;
#define BRIDGE_CARD_INVALID UINT8_MAX
#define BRIDGE_DECK_MASK UINT64_C(0x000fffffffffffff)

/* Bit i represents card i. Bits 52..63 must be zero. Zero is an empty
 * hand; partial hands are allowed for future DD positions. No ownership
 * or allocation is involved. A complete hand has exactly 13 set bits. */
typedef struct {
    uint64_t cards;
} bridge_hand;

/* Return false for invalid suit/rank or NULL output; leave output unchanged. */
bool bridge_card_make(bridge_suit suit, bridge_rank rank, bridge_card *out);
bool bridge_card_is_valid(bridge_card card);
/* Invalid cards return BRIDGE_SUIT_INVALID / BRIDGE_RANK_INVALID. */
bridge_suit bridge_card_suit(bridge_card card);
bridge_rank bridge_card_rank(bridge_card card);
/* A=4, K=3, Q=2, J=1, others=0; invalid cards return -1. */
int bridge_card_hcp(bridge_card card);

/* False for NULL or out-of-deck bits; does not enforce hand size. */
bool bridge_hand_is_valid(const bridge_hand *hand);
/* Add one card. Reject NULL, malformed hand, invalid/duplicate card or a
 * hand already containing 13 cards. On failure the hand is unchanged. */
bool bridge_hand_add(bridge_hand *hand, bridge_card card);
/* Return -1 for NULL or out-of-deck bits. Partial hands are supported. */
int bridge_hand_count(const bridge_hand *hand);
int bridge_hand_hcp(const bridge_hand *hand);

#endif
