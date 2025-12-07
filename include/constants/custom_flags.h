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

// Quest Flags
#define FLAG_CAN_SEE_WILD_NATURE                  (CUSTOM_FLAGS_START + 0x10)
#define FLAG_CAN_SEE_WILD_ITEM                    (CUSTOM_FLAGS_START + 0x11)
#define FLAG_CUSTOM_QUEST_1_COMPLETE         (CUSTOM_FLAGS_START + 0x12)

// NPC Hide Flags
#define FLAG_HIDE_CUSTOM_NPC_1               (CUSTOM_FLAGS_START + 0x20)
#define FLAG_HIDE_CUSTOM_NPC_2               (CUSTOM_FLAGS_START + 0x21)

// Item/Reward Flags
#define FLAG_RECEIVED_CUSTOM_ITEM_1          (CUSTOM_FLAGS_START + 0x30)
#define FLAG_RECEIVED_CUSTOM_ITEM_2          (CUSTOM_FLAGS_START + 0x31)

// Add more as needed...

// Define the end of your custom flags block
#define CUSTOM_FLAGS_END                     (CUSTOM_FLAGS_START + 0x1000) // 4096 custom flags

#endif // GUARD_CONSTANTS_CUSTOM_FLAGS_H