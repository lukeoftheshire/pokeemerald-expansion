#ifndef GUARD_FOLLOWER_SCAVENGE_H
#define GUARD_FOLLOWER_SCAVENGE_H

#include "global.h"
#include "config/follower_scavenge.h"

struct ScriptContext;

// -----------------------------------------------------------------------
// Movement enum
// Append before SCAVENGE_MOVEMENT_COUNT when adding new variants.
// Add the matching label to sScavengeMovements[] in follower_scavenge.c.
// -----------------------------------------------------------------------
enum ScavengeMovement
{
    SCAVENGE_MOVEMENT_DEFAULT = 0,   // face player + hop x2 (FollowerFoundItemMovement)
    SCAVENGE_MOVEMENT_DANCE,         // FollowerDance
    SCAVENGE_MOVEMENT_HOP_FAST,      // FollowerHopFast
    SCAVENGE_MOVEMENT_SKIP,          // FollowerSkippingMovement
    SCAVENGE_MOVEMENT_POKE_GROUND,   // FollowerPokeGround
    SCAVENGE_MOVEMENT_STARTLED,      // FollowerStartled
    SCAVENGE_MOVEMENT_NOSTALGIA,     // FollowerNostalgiaMovement
    SCAVENGE_MOVEMENT_SHIVER_H,      // FollowerShiverHorizontalMovement
    SCAVENGE_MOVEMENT_JUMP_ON,       // directional jump-onto-player (dispatched from C by position)
    SCAVENGE_MOVEMENT_CUDDLE,        // directional cuddle (dispatched from C by position)
    SCAVENGE_MOVEMENT_COUNT
};

// -----------------------------------------------------------------------
// Emote helper
// 0 = use tier-based default (common -> curious, uncommon -> happy, rare -> surprise).
// For a custom emote: SCAVENGE_EMOTE(FOLLOWER_EMOTION_PENSIVE) etc.
// -----------------------------------------------------------------------
#define SCAVENGE_EMOTE_DEFAULT 0
#define SCAVENGE_EMOTE(e)      ((u8)((e) + 1))

// -----------------------------------------------------------------------
// ScavengeItem - define once, reference by pointer in tier pool arrays.
//
// Simple item (no special behaviour):
//   static const struct ScavengeItem sItem_OranBerry = { .itemId = ITEM_ORAN_BERRY };
//
// Item with associated pokemon + custom messages (compound literals keep everything inline):
//   static const struct ScavengeItem sItem_DuskStone = {
//       .itemId            = ITEM_DUSK_STONE,
//       .emote             = SCAVENGE_EMOTE(FOLLOWER_EMOTION_PENSIVE),
//       .movement          = SCAVENGE_MOVEMENT_SHIVER_H,
//       .foundMessage      = (const u8[])_("{STR_VAR_1} gazed into the dark...$"),
//       .associatedPokemon = (const u16[]){ SPECIES_MISDREAVUS, SPECIES_MURKROW, 0xFFFF },
//       .selfMessage       = (const u8[])_("Does {STR_VAR_1} want to evolve?$"),
//       .associatedMessage = (const u8[])_("Was it thinking of {STR_VAR_3}?$"),
//   };
//
// STR_VAR_1 = follower nickname (buffered before script runs)
// STR_VAR_2 = item name        (buffered by bufferitemname in script)
// STR_VAR_3 = matching party member nickname (buffered when ASSOCIATED)
//
// Message fallthrough per category:
//   SELF:       selfMessage -> foundMessage -> default rarity pool
//   ASSOCIATED: associatedMessage -> foundMessage -> default rarity pool
//   FOUND:      foundMessage -> default rarity pool
//   DEFAULT:    default rarity pool (always)
// -----------------------------------------------------------------------
struct ScavengeItem
{
    u16          itemId;
    u8           emote;              // SCAVENGE_EMOTE_DEFAULT or SCAVENGE_EMOTE(FOLLOWER_EMOTION_*)
    u8           movement;           // ScavengeMovement enum value; 0 = SCAVENGE_MOVEMENT_DEFAULT
    const u8    *foundMessage;       // NULL = use default rarity pool message
    const u16   *associatedPokemon;  // NULL-terminated species list; NULL = no association
    const u8    *selfMessage;        // when follower IS in associatedPokemon; NULL = foundMessage
    const u8    *associatedMessage;  // when party member in associatedPokemon; NULL = foundMessage
};

// -----------------------------------------------------------------------
// ScavengePool - three NULL-terminated pointer arrays, one per rarity tier.
// Items shared across locations can appear in multiple pools by pointer.
//
//   static const struct ScavengeItem *sCommon_Forest[] = {
//       &sItem_OranBerry, &sItem_Antidote, NULL
//   };
//   static const struct ScavengePool sPool_Forest = {
//       .common = sCommon_Forest, .uncommon = sUncommon_Forest, .rare = sRare_Forest
//   };
//
// Set a tier field to NULL if nothing is findable at that tier on those maps.
// Tier selection probability: see SCAVENGE_COMMON_THRESHOLD / SCAVENGE_UNCOMMON_THRESHOLD.
// -----------------------------------------------------------------------
struct ScavengePool
{
    const struct ScavengeItem * const * const common;    // NULL-terminated pointer array, or NULL = empty
    const struct ScavengeItem * const * const uncommon;
    const struct ScavengeItem * const * const rare;
};

// Per-map entry. Set pool = NULL to disable scavenging on that map.
struct ScavengeEntry
{
    u8                        mapGroup;
    u8                        mapNum;
    const struct ScavengePool *pool;
};

// Discovery prefix shown as the first message box.
// Exported so EventScript_FollowerFoundItem can reference it directly.
// STR_VAR_1 = follower nickname, STR_VAR_2 = item name.
extern const u8 gText_ScavengeDiscovery[];

// -----------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------

// Called once per player step from field_control_avatar.c.
void TryFollowerScavenge(void);

// Called from GetFollowerAction in event_object_movement.c when FLAG_FOLLOWER_HAS_ITEM is set.
// Picks message, buffers associated nickname if needed, applies movement, loads ctx->data[0].
// Rewrites VAR_FOLLOWER_SCAVENGE to the bare item ID for the script's giveitem/bufferitemname.
void PrepareFollowerScavengeScript(struct ScriptContext *ctx);

// Fills item ID arrays by rarity tier for the current map's scavenge pool.
// Output arrays must have at least maxCommon/maxUncommon/maxRare slots.
void GetScavengeDisplayItems(
    u16 *commonOut,   u8 *commonCount,
    u16 *uncommonOut, u8 *uncommonCount,
    u16 *rareOut,     u8 *rareCount,
    u8 maxCommon, u8 maxUncommon, u8 maxRare);

#endif // GUARD_FOLLOWER_SCAVENGE_H
