#ifndef GUARD_CONSTANTS_CUSTOM_VARS_H
#define GUARD_CONSTANTS_CUSTOM_VARS_H

// Custom vars - reuses the 9 unused vanilla slots 0x40F7 through 0x40FF.
// Stored in SaveBlock1.vars[] alongside all other vars. No struct changes needed.
#define CUSTOM_VARS_START  0x40F7

// DexNav Vars (values referenced by DN_VAR_* in include/config/dexnav.h)
#define VAR_DN_SPECIES         (CUSTOM_VARS_START + 0x0)  // 0x40F7 - registered DexNav species + environment packed into u16
#define VAR_DN_STEP_COUNTER    (CUSTOM_VARS_START + 0x1)  // 0x40F8 - steps toward hidden mon

// Follower Scavenge Vars (referenced by VAR_FOLLOWER_SCAVENGE in include/config/follower_scavenge.h)
#define VAR_FS_SCAVENGE          (CUSTOM_VARS_START + 0x2)  // 0x40F9 - packed: itemId[9:0] | movement[12:10] | category[14:13]

// Add more custom vars here as needed (slots 0x40FA through 0x40FF are free)

#define CUSTOM_VARS_END  0x40FF  // 9 slots total: 0x40F7 - 0x40FF

#endif // GUARD_CONSTANTS_CUSTOM_VARS_H
