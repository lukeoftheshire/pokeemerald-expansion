#ifndef GUARD_CONSTANTS_CUSTOM_FLAGS_H
#define GUARD_CONSTANTS_CUSTOM_FLAGS_H

// Custom Flag Block - for your custom story/events
// These flags start after all vanilla flags end
#define CUSTOM_FLAGS_START     (DAILY_FLAGS_END + 1)

// === YOUR CUSTOM FLAGS HERE ===

//Starts at 0x960 hex (2401 in dec)
// Important Story Flags
#define FLAG_CUSTOM_INTRO_COMPLETE           (CUSTOM_FLAGS_START + 0x0)
#define FLAG_CUSTOM_MET_RIVAL                (CUSTOM_FLAGS_START + 0x1)
#define FLAG_CUSTOM_FIRST_GYM_BEATEN         (CUSTOM_FLAGS_START + 0x2)
#define FLAG_CUSTOM_SECOND_GYM_BEATEN         (CUSTOM_FLAGS_START + 0x3)
#define FLAG_CUSTOM_THIRD_GYM_BEATEN         (CUSTOM_FLAGS_START + 0x4)
#define FLAG_CUSTOM_FOURTH_GYM_BEATEN         (CUSTOM_FLAGS_START + 0x5)
#define FLAG_CUSTOM_FIFTH_GYM_BEATEN         (CUSTOM_FLAGS_START + 0x6)
#define FLAG_CUSTOM_SIXTH_GYM_BEATEN         (CUSTOM_FLAGS_START + 0x7)
#define FLAG_CUSTOM_SEVENTH_GYM_BEATEN         (CUSTOM_FLAGS_START + 0x8)
#define FLAG_CUSTOM_EIGHTH_GYM_BEATEN         (CUSTOM_FLAGS_START + 0x9)

// Quest Reward Flags - each unlocks a player ability gained as a quest reward.
// Categories (track here to keep an authoritative list):
//   Wild Nature      -> FLAG_CAN_SEE_WILD_NATURE  (battle intro shows wild mon's nature)
//   Wild Held Item   -> FLAG_CAN_SEE_WILD_ITEM     (battle intro shows wild mon's held item)
//   Move Relearner   -> FLAG_CAN_USE_MOVE_RELEARNER (Move Tutor option in book menu)
//   Stat Editor      -> FLAG_CAN_USE_STAT_EDITOR    (Stat Edit option in book menu)
#define FLAG_CAN_SEE_WILD_NATURE                  (CUSTOM_FLAGS_START + 0x10)
#define FLAG_CAN_SEE_WILD_ITEM                    (CUSTOM_FLAGS_START + 0x11)
#define FLAG_CAN_USE_MOVE_RELEARNER               (CUSTOM_FLAGS_START + 0x12)
#define FLAG_CAN_USE_STAT_EDITOR                  (CUSTOM_FLAGS_START + 0x13)

// Quest Progress Flags
#define FLAG_CUSTOM_QUEST_1_COMPLETE              (CUSTOM_FLAGS_START + 0x14)

// NPC Hide Flags
#define FLAG_HIDE_CUSTOM_NPC_1               (CUSTOM_FLAGS_START + 0x20)
#define FLAG_HIDE_CUSTOM_NPC_2               (CUSTOM_FLAGS_START + 0x21)

// Item/Reward Flags
#define FLAG_RECEIVED_CUSTOM_ITEM_1          (CUSTOM_FLAGS_START + 0x30)
#define FLAG_RECEIVED_CUSTOM_ITEM_2          (CUSTOM_FLAGS_START + 0x31)

// DexNav Flags (used by include/config/dexnav.h DN_FLAG_* settings)
#define FLAG_DN_SEARCHING      (CUSTOM_FLAGS_START + 0x40)
#define FLAG_DN_DEXNAV_GET     (CUSTOM_FLAGS_START + 0x41)
#define FLAG_DN_DETECTOR_MODE  (CUSTOM_FLAGS_START + 0x42)

// Follower Scavenge Flags (referenced by FLAG_FOLLOWER_HAS_ITEM in include/config/follower_scavenge.h)
#define FLAG_FS_HAS_ITEM      (CUSTOM_FLAGS_START + 0x50)  // follower found an item waiting to be collected

// Add more as needed...

// Define the end of your custom flags block
#define CUSTOM_FLAGS_END                     (CUSTOM_FLAGS_START + 0x1000) // 4096 custom flags

#endif // GUARD_CONSTANTS_CUSTOM_FLAGS_H