#ifndef GUARD_CONFIG_FOLLOWER_SCAVENGE_H
#define GUARD_CONFIG_FOLLOWER_SCAVENGE_H

// Set FALSE to disable the system entirely - zero runtime overhead when off.
// If TRUE, FLAG_FOLLOWER_HAS_ITEM and VAR_FOLLOWER_SCAVENGE below must be non-zero.
#define SCAVENGE_ENABLED         TRUE

#define SCAVENGE_STEP_INTERVAL       10    // steps between scavenge rolls
#define SCAVENGE_FIND_CHANCE          100    // percent chance per roll (0-100)

// Tier selection: roll 0-99. Below COMMON = common, below UNCOMMON = uncommon, else rare.
#define SCAVENGE_COMMON_THRESHOLD     5   // 60% common
#define SCAVENGE_UNCOMMON_THRESHOLD   9    // 30% uncommon, 10% rare

// Map raw constants to semantic names (same pattern as dexnav.h DN_FLAG_* / DN_VAR_* mappings).
// To port to another ROM hack, change the right-hand side to point at unused flag/var slots.
#define FLAG_FOLLOWER_HAS_ITEM   FLAG_FS_HAS_ITEM   // follower found a pending item
#define VAR_FOLLOWER_SCAVENGE    VAR_FS_SCAVENGE     // packed: itemId[9:0] | movement[12:10] | category[14:13]

#endif // GUARD_CONFIG_FOLLOWER_SCAVENGE_H
