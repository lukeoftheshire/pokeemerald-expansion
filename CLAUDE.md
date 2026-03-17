# CLAUDE.md

This file provides persistent context for Claude Code. Read it at the start of every session.

Do not use em dashes anywhere.

---

## What This Project Is

A custom Pokemon Emerald ROM hack built on **pokeemerald-expansion** (rh-hideout fork), which is
itself built on pret's pokeemerald decompilation. The output is `pokeemerald.gba`.

The hack has a strong regional identity: **Maine / New England**. Think weathered wood, nautical
charts, general store signage, whaling supply shops in Portland. Earthy and muted. UI should feel
like something the trainer physically carries - a field journal or worn notebook.
Development is on **macOS**. Primary language is **C** with some ARM Assembly.

---

## Build Commands

```bash
# Standard parallel build (preferred)
make -j9
# Debug build (-Og -g flags)
make debug
# Release build (LTO, NDEBUG)
make release
# Run test suite via mgba-rom-test
make check
# Clean build artifacts (keeps tools)
make tidy
# Full clean including tools and generated files
make clean
```

Prerequisites (macOS): devkitARM, libpng, pkg-config. See `docs/install/mac/MAC_OS.md`.

---

## Repository Structure

```
src/           C source files (~365 files)
include/       Headers
  config/      Feature toggle headers (battle.h, general.h, pokemon.h, etc.)
  constants/   Game constants (.h files)
  gba/         Low-level GBA hardware headers
data/          Game data in .s, .inc, and JSON formats
  maps/        Per-map event/layout data
graphics/      Source art (.png) and compiled assets (.4bpp, .gbapal, .bin)
test/          Test suite C files (run via `make check`)
tools/         Build tools (gbagfx, poryscript, jsonproc, mgba, etc.)
asm/           Hand-written ARM assembly
constants/     Assembly-format constants (.inc files)
```

---

## Key Architecture Concepts

**GBA graphics pipeline**: `.png` files in `graphics/` are converted by `gbagfx` to `.4bpp` (tile
data) and `.gbapal` (palette). Tilemaps are `.bin` files. Compressed variants use `.smol`/`.smolTM`
suffixes. Graphics data is `INCBIN`'d into C arrays in `src/graphics.c`.

**Background layers**: The GBA has 4 BG layers. Party menu and other full-screen UIs define
`BgTemplate` arrays in `src/data/<screen>.h` that assign tile/map base indices to each layer.

**Task system**: Cooperative task scheduler. Screen-level logic splits into `Task_*` callback
chains. See `include/task.h`.

**Config system**: Feature flags live in `include/config/*.h` as `#define`s (not runtime flags),
so changes require a rebuild. Use `if (CONFIG_FLAG)` inside functions rather than `#ifdef` guards
around entire functions (exception: data structures with `#if`).

**Poryscript**: Map scripts in `data/maps/` are written in `.pory` format and compiled to `.inc`
by the `poryscript` tool during `make`. Edit `.pory` files, not the generated `.inc` files.

**Data files**: External data (species, moves, items, etc.) uses JSON, processed by `jsonproc`.
Learnsets and similar data are generated during build.

**Memory regions**: EWRAM (256KB, for large data structs) and IWRAM (32KB, for hot code/data).
Variables in EWRAM/global structs should use the smallest data type possible to conserve space.

---

## Coding Style

- **Functions/structs**: `PascalCase`
- **Variables/fields**: `camelCase`
- **Globals**: `g` prefix (`gMyVar`); **statics**: `s` prefix (`sMyVar`)
- **Constants/macros**: `CAPS_WITH_UNDERSCORES`
- **Indentation**: 4 spaces for `.c`/`.h`; tabs for `.s` and `.inc`
- **Braces**: Opening brace on its own line for control structures
- **`switch` cases**: Left-aligned with the `switch` block (not indented)
- **Loop iterators**: Declare inside the `for` statement
- **Default int size**: `u32`/`s32` for local variables; smallest type for saveblock/EWRAM/globals
- **No magic numbers**: Use constants or enums; use enum types in function signatures
- **Config checks**: `if (CONFIG_FLAG)` inside normal control flow, not `#ifdef` wrapping whole functions

---

## Aesthetic and Style Guide

### Color Philosophy
- Earthy, muted palette..
- Semantic color roles matter. Plan palette slots deliberately.
- Brick red is acceptable
- Parchment tones for text backgrounds where possible.
- Ocean blues and slate grays for cool accents.

### GBA Palette Constraints
- GBA uses 16-color palettes (15 colors + transparent) per palette slot.
- UI screens share palette budgets. Always plan how many palette slots a screen uses.

### Motifs
- Nature motifs generally. Field journal / naturalist notebook cues.

### Type Stamps
Custom 16x16 ink stamp sprites exist at `graphics/type_stamps/` - one PNG per type, named after the type (e.g. `normal.png`, `fire.png`). All 18 share a single palette: `graphics/type_stamps/type_stamp.gbapal` (from `type_stamp.pal`). Each stamp is one ink color on a transparent background.

These are used on the book menu in place of the standard 32x16 type icons. Use them anywhere a compact, journal-appropriate type indicator is needed. Do NOT use the standard `gSpriteTemplate_MoveTypes` on the book menu - use `sSpriteTemplate_TypeStamps` instead (defined in `src/start_menu_book.c`). The summary screen and other screens keep the original 32x16 icons.

### UI Paradigm
- The flagship start menu is a **notebook / field journal** with tabbed pages and bookmark-style party display.
- Menus should feel diegetic - like physical objects the trainer carries.
- Selected states should be subtle, not loud.
- **240x160** (GBA resolution).

---

## Git and Workflow

### Remotes
- `origin` - rh-hideout/pokeemerald-expansion (upstream base)
- `myCoolROM` - lukeoftheshire/pokeemerald-expansion (user's fork)
- `Biv-expansion` - Bivurnum/pokeemerald-expansion (fishing-minigame branch)
- `mudskip` - mudskipper13/pokeemerald (feature/new-shop-ui-rhh)
- `team_aqua` - TeamAquasHideout/pokeemerald (main_menu branch)

### Integration Philosophy
Comfortable resolving git conflicts but prefers minimal diffs. When recommending how to integrate
a feature from an external fork, prioritize in this order:

1. Copy only new files, then manually port modifications into existing files
2. Cherry-pick specific commits
3. Manual porting of logic with stepwise compilation after each change
4. Full merge only if unavoidable

When conflicts are likely (UI, menus, palette systems), warn upfront and propose a controlled
step-by-step plan. Always compile and test after each meaningful integration step.

---

## Custom Features in This Fork

### Done
- Preferred follower toggle from party menu
- Gen 5-ish party menu with new background graphics (`src/party_menu.c`, `graphics/party_menu/`)
- QoL field moves
- Battle mode toggle (options menu updated)
- Quest menu system
- Overworld fireflies
- Overworld autumn leaves weather
- FRLG-style map previews (integrated, palettes resolved)
- Poryscript-based field scripts
- Comfy Anims library (`src/comfy_anim.c`, `include/comfy_anim.h`)
- Field mugshot system (`mudskipper/feature/field-mugshot`)
- Mining minigame (`mining/mining_minigame`)
- Fishing minigame (`Biv-expansion/fishing-minigame`)
- Wild encounter message with nature and held item (custom implementation, see below)

### Pending (active backlog)
- Notebook start menu tab system (Pokedex, Bag, PokéNav, Trainer Card, Save, Options tabs - stubs exist)
- DexNav enablement (see DexNav section below)
- Follower scavenge system (see Follower Scavenge section below)
- New Birch briefcase with custom starters
- Color variants system
- Music expansion
- Additional custom weather effects (see weather section below)

### Evaluated but Not Yet Decided
These were surfaced during a feature audit (Team Aqua Asset Repo, pret wiki, codebase). Review before planning:

**Book menu tab candidates:**
- Town Map as a dedicated tab (separate from PokéNav)
- Start Menu Clock (in-game time on the menu itself; fits notebook aesthetic)
- Outfits / character customization screen

**Party popup action candidates (additions to the 5-action menu):**
- Nickname from party menu (rename without a PC)
- Ability Changer (item-based ability swap)

**New feature screens:**
- Craft Menu (4-item crafting with discovery mechanics; Team Aqua Asset Repo)
- Multipage Options Menu (more settings pages; pret wiki)

**World / progression systems:**
- Upgradable Fishing Rod
- Apricorn Trees (resource gathering; `gSaveBlock3Ptr->apricornTrees` save slot already exists)
- Path Finder (navigation aid)

**Wilderness / exploration systems (thematic, lower priority):**
- Campfire / rest sites - interact with overworld fire rings to heal party, roast berries

### Back Burner
- Overworld encounters

### Abandoned for Now
- Rogue-style battle speed-up (may implement manually later)
---

## Feature Sources

| Feature | Source |
|---|---|
| Preferred follower toggle | https://github.com/Kasenn/pokeemerald-expansion-kasen/tree/toggle_follower_from_party_menu |
| Overworld encounters | https://github.com/HashtagMarky/pokeemerald/tree/ikigai/ow-encounters |
| FRLG Map previews | https://github.com/Bivurnum/decomps-resources/wiki/FRLG-Map-Previews |
| Vanilla summary screen expanded | https://github.com/TeamAquasHideout/Team-Aquas-Asset-Repo/wiki/Vanilla-Summary-Screen-Expanded |
| Game corner / gacha | https://github.com/agsmgmaster64/worldlinkdeluxe-ame/tree/gacha-expansion |
| Rogue battle speed-up | https://github.com/AlexOn1ine/pokeemerald-expansion-1/tree/Rogue_BattleSpeedUp |
| Field mugshot system | https://github.com/mudskipper13/pokeemerald/tree/feature/field-mugshot |
| Field mugshot how-to | https://github.com/mudskipper13/pokeemerald/wiki/How-to-add-new-Field-Mugshots |
| New Birch briefcase / custom starters | https://github.com/pret/pokeemerald/wiki/New-Birch's-Briefcase-With-Fully-Custom-Starters-by-Archie-and-Mudskip |
| Full screen start menu | https://github.com/pret/pokeemerald/wiki/Full-Screen-Start-Menu-by-Archie-and-Mudskip |
| Gen 5-ish party menu | https://github.com/TeamAquasHideout/pokeemerald/tree/gen5ish_party_menu |
| Color variants | https://github.com/SpaceOtter99/pokeemerald-expansion/tree/colour-variants |
| Quest menu | https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu |
| QoL field moves | https://github.com/fisham-org/pokeemerald-expansion-features/wiki/Modern-QoL-Field-Moves |
| Battle mode toggle | https://github.com/fisham-org/pokeemerald-expansion-features/wiki/Battle-Mode-Toggle |
| Music expansion | https://github.com/grunt-lucas/pokeemerald-expansion/tree/merge-music-expansion |
| Level scaling (reference only) | https://github.com/fisham-org/pokeemerald-expansion-features/wiki/Level-Scaling |
| Pathfinder | https://github.com/estellarc/pokeemerald/tree/feature/pathfinder |
| Comfy Anims | https://github.com/huderlem/pokeemerald/tree/comfy_anims |

---

## Major Initiative: Notebook Start Menu

The flagship UI feature. Stabilize before adding more UI-heavy features.
Active branch: `feature_main_menu`

### Design
- Open notebook / field journal shown as a two-page spread
- Tabs for: Pokedex, Pokemon, Bag, Quest Log, DexNav (reskinned as "Pokeguide"), Save, Options, Minerals (for Mining Minigame), Scavenger Guide
- Selected tab highlighted subtly

### Key Files and Symbols
- `src/start_menu_book.c` - main implementation
- `include/start_menu_book.h` - header
- `DisplaySelectedPokemonDetails` - selected Pokemon info handler
- `VBlankCB_BookMenu` - VBlank callback (LoadOam / ProcessSpriteCopyRequests / TransferPlttBuffer)
- `Task_BookMenu_HandleInput(u8 taskId)` - input loop (B/Start closes, SE_SELECT, palette fade)
- `sBookBgTemplates` - BG template array

### Known Bugs

None currently known.

---

## Major Initiative: Quest System and Quest Menu UI

Reference: https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu

Quest icons integrated. Assets pulled from PokemonSanFran:pokeemerald:unbound-quest-menu.
Quest UI serves as a visual standard other menus should match.

Established visual direction:
- Brick red accent used sparingly
- Pine green as key accent
- ~12 colors in current palette, room remains
- Text aims for parchment-like appearance

### How to Add a Quest

There are 30 quest slots (`QUEST_1` through `QUEST_30` in `include/constants/quests.h`). To add a new quest:

**Step 1 - Text strings in `src/strings.c`:**
- `gText_SideQuestName_N` - short name shown in quest log list
- `gText_SideQuestDesc_N` - active description (one or two lines)
- `gText_SideQuestDoneDesc_N` - completion description shown when done
- `gText_SideQuestMapN` - location name shown in quest details

**Step 2 - Quest data in `src/quests.c`:**
Edit the `sSideQuests[QUEST_N]` entry in the `side_quest(...)` array:
- `sprite`: use a `SPECIES_*` constant with `PKMN` type, or `OBJ_EVENT_GFX_*` with `OBJECT` type
- Set `subquests = NULL, numSubquests = 0` for a simple quest with no sub-objectives

**Step 3 - NPC script in the relevant map's `.pory` file:**
The script should check quest state and branch accordingly. Standard pattern:
```
questmenu QUEST_MENU_CHECK_COMPLETE, QUEST_N  @ done?
goto_if_eq VAR_RESULT, TRUE, Script_Done
questmenu QUEST_MENU_CHECK_ACTIVE, QUEST_N    @ in progress?
goto_if_eq VAR_RESULT, TRUE, Script_Active
@ First encounter: give quest
questmenu QUEST_MENU_SET_ACTIVE, QUEST_N
...

Script_Active:
@ condition check (e.g. getcaughtmon SPECIES_X -> VAR_RESULT)
goto_if_eq VAR_RESULT, TRUE, Script_Reward
@ not done yet
...

Script_Reward:
@ give reward (setflag, giveitem, etc.)
questmenu QUEST_MENU_COMPLETE_QUEST, QUEST_N
...

Script_Done:
@ already completed text
...
```

**Quest scripting commands:**
- `questmenu QUEST_MENU_SET_ACTIVE, QUEST_N` - unlock and mark active
- `questmenu QUEST_MENU_COMPLETE_QUEST, QUEST_N` - mark complete (clears active/reward flags)
- `questmenu QUEST_MENU_CHECK_ACTIVE, QUEST_N` - sets VAR_RESULT to TRUE/FALSE
- `questmenu QUEST_MENU_CHECK_COMPLETE, QUEST_N` - sets VAR_RESULT to TRUE/FALSE
- `getcaughtmon SPECIES_X` - sets VAR_RESULT to TRUE if species is caught in Pokedex
- `getseenmon SPECIES_X` - sets VAR_RESULT to TRUE if seen

**Implemented quests:**
| Slot | Name | NPC | Condition | Reward |
|---|---|---|---|---|
| `QUEST_1` | Jeremy's Field Notes | Jeremy, Littleroot Town | Catch a Zigzagoon on Route 101 (`getcaughtmon SPECIES_ZIGZAGOON`) | `FLAG_CAN_SEE_WILD_NATURE` |

### Quest Icon System

Quest givers automatically display a floating icon above their head when a quest is available or has a reward pending. The system is fully data-driven - no C code changes required to enable it for an NPC.

**How it works:**
- `RefreshQuestIcons()` runs each time the overworld loads or object events are refreshed (called from `overworld.c` and `event_object_movement.c`)
- For each active object event, it checks `objectEvent->trainerType == TRAINER_TYPE_QUEST_GIVER`
- If so, it checks the quest state for `objectEvent->questId` and spawns FLDEFF_QUEST_ICON unless the quest is already completed
- The icon sprite tracks the NPC and renders 16px above it each frame
- The icon is auto-removed when the quest completes or the NPC is spoken to and the quest resolves
- Icons share the OBJ_EVENT_PAL_TAG_EMOTES palette; sprite sheet is `graphics/misc/quests_icons.png` (2-frame animation)

**Step 4 - Register NPC as quest giver in their map's `map.json`:**
Change the NPC's `trainer_type` and add a `quest_id` field:
```json
"trainer_type": "TRAINER_TYPE_QUEST_GIVER",
"trainer_sight_or_berry_tree_id": "0",
"script": "MapName_EventScript_NpcName",
"flag": "0",
"quest_id": "0"
```
`TRAINER_TYPE_QUEST_GIVER` is value 4 (defined in `include/constants/trainer_types.h`). Use the numeric value for `quest_id` (QUEST_1=0, QUEST_2=1, etc.) - the symbol names are not available in the assembly constants. The `quest_id` field is optional; if absent, no icon is shown even if `trainer_type` is set.

**Key source files:**
- `src/quests.c` - `RefreshQuestIcons`, `HandleQuestIconForSingleObjectEvent`, `SpawnQuestIconForObject`
- `src/trainer_see.c` - `FldEff_QuestIcon`, `SpriteCB_QuestIcon` (sprite creation and per-frame tracking)
- `include/global.fieldmap.h` - `ObjectEventTemplate.trainerType`, `ObjectEventTemplate.questId`, `ObjectEvent.hasQuestIcon`
- `tools/mapjson/mapjson.cpp` - JSON parser reads `quest_id` field and emits it as an optional trailing argument to `object_event`

---

## Save System Architecture

The save system uses three separate SaveBlocks distributed across flash sectors.

**SaveBlock1** (~14KB, 4 sectors): Primary game state.
- `flags[300]` (2400-bit bitfield): event flags 0x000 - 0x95F. Custom flags start at `CUSTOM_FLAGS_START = 0x960`. The bitfield auto-sizes via `FLAGS_COUNT = DAILY_FLAGS_END + 1`; custom flags in `custom_flags.h` automatically extend it.
- `vars[256]` (u16 array, 512 bytes): event variables 0x4000 - 0x40FF. Custom vars reuse unused vanilla slots 0x40F7-0x40FF. Defined in `custom_vars.h`, included via `vars.h`.
- Accessed via `FlagGet/FlagSet(id)` and `VarGet/VarSet(id)`.

**SaveBlock2** (~3.4KB, 1 sector): Player identity (name, rival name), Pokedex struct, options, play time, quest data.

**SaveBlock3** (max 1624 bytes = 116 bytes x 14 sectors): Distributed as 116-byte trailing chunks per sector. Reconstructed at load. Contains: `dexNavSearchLevels[NUM_SPECIES]` (1550 bytes, conditional), `dexNavChain`, `followerIndex`, apricorn trees, etc. A `STATIC_ASSERT` in `save.c` enforces the size limit at compile time - it will fail if struct grows too large.

**Adding custom data:**
- New flag: add to `custom_flags.h`. No struct change needed.
- New var: use a free slot in `custom_vars.h` (0x40F9-0x40FF remain free). No struct change needed.
- Larger persistent data: add a field to `SaveBlock3` in `include/global.h`, then verify the static assert still passes.

**Include ordering note:** `config/dexnav.h` is included from `global.h` AFTER `constants/flags.h` and `constants/vars.h`. This allows dexnav.h to reference `FLAG_DN_*` and `VAR_DN_*` constants. Do not move this include back into `constants/global.h`.

---

## DexNav System

Source code: `src/dexnav.c` (~2700 lines, fully implemented). Config: `include/config/dexnav.h`.

### Features Present in the Codebase
- **Species selection GUI**: grid of Pokemon icons from current map encounter tables (land top row, land bottom row, water row, hidden row - up to 20 slots). Cursor selects species to register.
- **Active search overlay**: small window showing icon, name (or "???" if unseen), level, proximity arrows, egg move name, ability name, held item name, IV star potential (1-3 stars), chain counter, owned badge.
- **Hidden Pokemon detection mode**: auto-triggers every ~100 steps (25% chance), shows exclamation mark, species cry plays, player sneaks toward hidden mon.
- **Search levels** (`USE_DEXNAV_SEARCH_LEVELS`): per-species counter (0-255) stored in `gSaveBlock3Ptr->dexNavSearchLevels[NUM_SPECIES]`. Affects egg move chance, hidden ability chance, held item chance, IV potential. WARNING: ~850 bytes in SaveBlock3 - check space.
- **Chain counter**: `gSaveBlock3Ptr->dexNavChain` (0-100). Adds level bonus each chain.
- **Encounter generation**: applies IVs, ability, held item, egg moves, chain level bonus on top of normal wild mon.

### Porting as a Book Menu Page (Inline, No Separate Window)

The DexNav's species selection GUI is the part that would become a book page. The active search itself is always a field interaction (R button), not a menu screen.

A DexNav book page would show:
- Current map's encounter table as a list of Pokemon icons (land / water / hidden groups)
- Caught/seen badge per species
- Search level per species (number or star icons)
- Currently registered species highlighted
- Chain counter
- Selecting a species registers it for field search (sets DN_VAR_SPECIES)

Implementation path:
- Read current map wild data (`gWildMonHeaders`) to populate the species list - same data `dexnav.c` already reads
- Draw mon icons using `CreateMonIcon` / `LoadMonIconPalettes` (same system as party page)
- Read `gSaveBlock3Ptr->dexNavSearchLevels[species]` for search level display
- On selection, call `VarSet(DN_VAR_SPECIES, (environment << 14) | species)` to register
- No need to replicate the active search window - that stays in the field (R button)

---

## Major Initiative: Field Weather Effects

Weather effects must render as a sprite/particle layer on top of the scene, or use GBA alpha
blending. No tile-level changes are viable for weather.

### Implemented
**Autumn Leaves**
**Fireflies** 

### Planned
- **Sea Fog** - alpha-blended white/gray overlay with slow drift particles
- **Light Snow / Flurries** - slow gentle white particles, calm feel
- **Blizzard** - dense fast horizontal white particles (leaves system reused, high velocity)
- **Nor'easter** - blizzard particles layered over standard rain weather
- **Aurora** - slow semi-transparent color-shifting band in sky region via alpha blend
- **Wildfire Smoke** - faint sepia/gray semi-transparent overlay, sparse dark particles
- **Embers / Ash** - small rising orange/red particles (firefly system reused, rising direction)


---

## Major Initiative: FRLG-Style Map Previews

Reference: https://github.com/Bivurnum/decomps-resources/wiki/FRLG-Map-Previews

---

## Wild Encounter Message

Custom implementation (not from an external fork). The battle intro message shows the wild
Pokemon's nature and held item if the player has unlocked those abilities.

Message format: `"You encountered a wild {nature} {name}{held item}!"`

- Nature is shown only if `FLAG_CAN_SEE_WILD_NATURE` is set; otherwise the nature token resolves to empty string
- Held item is shown only if the Pokemon actually holds an item AND `FLAG_CAN_SEE_WILD_ITEM` is set; resolves to `, holding a/an <item>` with correct a/an vowel detection
- Legendary encounters use a separate string (`sText_LegendaryPkmnAppeared`) that omits held item

**Key locations:** `src/battle_message.c` lines ~84-88 (strings), ~3177-3209 (token handlers)
**Flags:** `FLAG_CAN_SEE_WILD_NATURE` and `FLAG_CAN_SEE_WILD_ITEM` defined in `include/constants/custom_flags.h`

---

## Comfy Anims Library

Source: https://github.com/huderlem/pokeemerald/tree/comfy_anims
Files: `src/comfy_anim.c`, `include/comfy_anim.h`
Integration: purely additive, zero conflicts, no merge needed.

Provides smooth physics-based and eased UI animations. Uses `Q_24_8` fixed-point math and
the existing `MathUtil_Mul32` / `MathUtil_Div32` from `math_util.h`. The build auto-discovers
the `.c` file via wildcard - no Makefile changes needed.

### Two animation types

**Easing** - fixed duration, predetermined curve. Use when timing must be predictable (UI
transitions, menu slide-ins).

**Spring** - physics-based, variable duration. Use when you want follow/chase behavior or
don't need exact timing (cursor tracking, value chasing).

### Method 1: Global array (like Tasks)

Best for sprite callbacks or multiple concurrent animations.

```c
// In your main callback - call after RunTasks()
void MainCB(void) {
    RunTasks();
    AdvanceComfyAnimations();
}

// On screen exit - call alongside ResetTasks()
void ExitMyScreen(void) {
    ResetTasks();
    ReleaseComfyAnims();
}

// Start an easing animation
u8 animId = 0;
void StartSlide(void) {
    struct ComfyAnimEasingConfig config;
    InitComfyAnimConfig_Easing(&config);
    config.from = Q_24_8(0);
    config.to   = Q_24_8(128);
    config.durationFrames = 20;
    config.easingFunc = ComfyAnimEasing_EaseOutCubic;
    animId = CreateComfyAnim_Easing(&config);
}

// In a sprite callback, read the current value
void SpriteCallback(struct Sprite *sprite) {
    u8 id = sprite->data[0];
    sprite->x = ReadComfyAnimValueSmooth(&gComfyAnims[id]);
    if (gComfyAnims[id].completed) {
        ReleaseComfyAnim(id);
        sprite->callback = SpriteCallbackDummy;
    }
}
```

### Method 2: Manual struct in EWRAM

Best for a single persistent animation tied to a screen (e.g. scroll offset).

```c
EWRAM_DATA struct ComfyAnim sMyAnim = {0};

void InitMyAnim(void) {
    struct ComfyAnimSpringConfig config;
    InitComfyAnimConfig_Spring(&config);
    config.from = Q_24_8(0);
    config.to   = Q_24_8(64);
    // tension/friction/mass default to sensible values via InitComfyAnimConfig_Spring
    InitComfyAnim_Spring(&config, &sMyAnim);
}

// Call each frame in your Task
void Task_Update(u8 taskId) {
    TryAdvanceComfyAnim(&sMyAnim);
    s32 x = ReadComfyAnimValueSmooth(&sMyAnim);
    // use x to position something
}
```

### Easing functions

| Function | Character |
|---|---|
| `ComfyAnimEasing_Linear` | No easing |
| `ComfyAnimEasing_EaseOutQuad` | Quick start, smooth stop |
| `ComfyAnimEasing_EaseOutCubic` | Stronger version of above |
| `ComfyAnimEasing_EaseInOutCubic` | Smooth on both ends |
| `ComfyAnimEasing_EaseInOutBack` | Slight overshoot / bounce |

### Spring tuning

Defaults (`mass=50, tension=175, friction=1000`) are fast, snappy, low-overshoot.

- Higher `tension` = faster / snappier
- Higher `friction` = less wobble, settles sooner
- Higher `mass` = slower, floatier
- `clampAfter = 1` = stop at first overshoot (no wobble)
- `clampAfter = 0` (default) = run until physics settles naturally

### Key rules

- Always wrap values with `Q_24_8()`. Bare integers produce wrong results.
- `ReadComfyAnimValueSmooth` rounds to nearest integer with +0.5 bias - use this for screen coords.
- `NUM_COMFY_ANIMS` is 8 (in `comfy_anim.h`). Increase if more concurrent global animations are needed.
- For diagonal motion, use two separate `ComfyAnim` instances (one X, one Y).
- Springs can be retargeted mid-animation by updating `.config.data.spring.to`.

---

## Mining Minigame System

Source: `src/mining_minigame.c` (~3400 lines). Config/constants: `include/constants/mining_minigame.h`.
Graphics: `graphics/mining_minigame/` (UI) and `graphics/mining_minigame/items/` (25 custom item sprites).
Launched via `special StartMining` in Poryscript or `StartMining()` in C. Takes no parameters; returns to `CB2_ReturnToField`.

### Adding a New Mining Item

Every item needs five things: a sprite, a palette, registration in the tool's JSON, an enum entry, and a slot in the item data table.

**Sprite spec:** 64x64 pixels, 4bpp, 16-color palette (15 + transparent), indexed PNG. Crisp pixel edges, no anti-aliasing. The item should fit naturally within the 64x64 canvas; transparent padding fills the rest. The build system auto-converts PNG to `.4bpp` and compresses to `.4bpp.smol` - no manual conversion needed.

**Step 1 - Add sprite and palette files:**
```
graphics/mining_minigame/items/my_item.png     # 64x64, 4bpp indexed PNG
graphics/mining_minigame/items/my_item.pal     # Jasc-PAL format, 16 colors
```
Fossils can share `fossil.pal` / `fossil.gbapal` instead of providing their own.

**Step 2 - Register in `tools/mining_minigame/table.json`:**
```json
{"id": "MININGID_MY_ITEM", "path": "./graphics/mining_minigame/items/my_item.4bpp"}
```
Keep entries in a consistent order (matching enum order helps).

**Step 3 - Run the sprite analysis tool to regenerate the tile table:**
```bash
python3 tools/mining_minigame/analyze_sprites.py
```
This reads all `.4bpp` files listed in `table.json`, detects which 8x8 tiles contain non-transparent pixels, and rewrites `src/data/mining_minigame.h` (the `sSpriteTileTable` array). Do not edit that file manually.

**Step 4 - Add enum values in `include/constants/mining_minigame.h`:**
```c
// In the MININGID_* enum:
MININGID_MY_ITEM,

// In the MINING_TAG_* enum (tile and palette tag for OBJ system):
MINING_TAG_MY_ITEM_TILES = <next_unused_value>,
MINING_TAG_MY_ITEM_PAL   = <next_unused_value>,
```

**Step 5 - Add INCBIN declarations in `src/graphics.c`:**
```c
const u32 gMiningItemTiles_MyItem[]  = INCBIN_U32("graphics/mining_minigame/items/my_item.4bpp.smol");
const u16 gMiningItemPal_MyItem[]    = INCBIN_U16("graphics/mining_minigame/items/my_item.gbapal");
```

**Step 6 - Add SpriteSheet and SpritePalette entries in `src/mining_minigame.c`:**
```c
// In the sSpriteSheets[] array:
{gMiningItemTiles_MyItem, sizeof(gMiningItemTiles_MyItem), MINING_TAG_MY_ITEM_TILES},

// In the sSpritePalettes[] array:
{gMiningItemPal_MyItem, MINING_TAG_MY_ITEM_PAL},
```

**Step 7 - Add an entry to `sMiningItems[]` in `src/mining_minigame.c`:**
```c
[MININGID_MY_ITEM] = {
    .itemId     = ITEM_MY_ITEM,        // the actual item granted to player
    .top        = 4,                   // footprint height in 8x8 tiles (1-4)
    .left       = 4,                   // footprint width in 8x8 tiles (1-4)
    .totalTiles = 12,                  // count of occupied cells from sSpriteTileTable (run tool to get this)
    .tilesTag   = MINING_TAG_MY_ITEM_TILES,
    .palTag     = MINING_TAG_MY_ITEM_PAL,
},
```
`.top` and `.left` are the bounding box of the item for wall/edge collision checks. `.totalTiles` is the actual occupied cell count; the analyze_sprites.py tool prints this per-item for reference.

**Step 8 - Add to the appropriate rarity table:**
```c
// In src/mining_minigame.c, pick one:
static const u32 ItemRarityTable_Common[]   = { ..., MININGID_MY_ITEM };
static const u32 ItemRarityTable_Uncommon[] = { ..., MININGID_MY_ITEM };
static const u32 ItemRarityTable_Rare[]     = { ..., MININGID_MY_ITEM };
```
Remember to update the corresponding `_Count` constant or array size if the tables use a fixed-length sentinel.

**Step 9 - Build and test:**
```bash
make -j9
```
Verify the item appears in the minigame and the sprite loads correctly.

---

## Follower Scavenge System

**Not yet implemented.** Scope assessed and ready to build when notebook menu is stable.

Every 100 steps, if a follower Pokemon is out (not on a bike, follower not recalled), roll against a location-specific item pool. On a hit: play an exclamation animation on the follower sprite, set a flag and var to store the pending item. The next time the player talks to the follower, show "Oh? POKEMON found a ITEM!" and add it to the bag (with yes/no choice or direct give). If no pool exists for the current map, the check always fails silently.

### State Storage (two free slots, no struct changes needed)
```c
// include/constants/custom_vars.h
VAR_FOLLOWER_FOUND_ITEM   // stores pending item ID (ITEM_NONE = nothing pending)

// include/constants/custom_flags.h
FLAG_FOLLOWER_HAS_ITEM    // set when item is waiting to be collected
```

### Files to Touch

| File | Change |
|---|---|
| `src/field_control_avatar.c` | ~15 lines in `tookStep` block: check follower out, increment step var, roll, trigger animation |
| `src/follower_scavenge.c` | NEW: pool table struct, per-location lookup, roll logic (~150 lines + data) |
| `include/follower_scavenge.h` | NEW: header (~10 lines) |
| `src/event_object_movement.c` | ~10 lines at top of `GetFollowerAction`: check FLAG_FOLLOWER_HAS_ITEM, branch to item script |
| `data/scripts/follower.inc` | NEW `EventScript_FollowerFoundItem`: bufferitemname, giveitem, clear flag/var |
| `include/constants/custom_vars.h` | +1 VAR |
| `include/constants/custom_flags.h` | +1 FLAG |

### Implementation Notes

**Step hook** - add inside the `if (input->tookStep)` block in `src/field_control_avatar.c`. Guard conditions: follower must be active (`gSaveBlock3Ptr->followerIndex != OW_FOLLOWER_NOT_SET && != OW_FOLLOWER_RECALLED`), player not on bike.

**Animation trigger** - get the follower OBJ event via `GetFollowerNPCData(FNPC_DATA_OBJ_ID)` (party follower path may differ slightly), then call `ObjectEventEmote()` or `FieldEffectStart(FLDEFF_EXCLAMATION_MARK_ICON)` with the follower's local ID/map coords. Animation plays once at moment of discovery; the FLAG persists until the player talks to the follower.

**All configuration lives in `src/follower_scavenge.c`.** This is the single file to edit when tuning behavior or adding routes. Nothing scavenge-related should be scattered across other files.

Top of file defines all tuneable constants:
```c
#define SCAVENGE_STEP_INTERVAL  100   // steps between rolls
#define SCAVENGE_FIND_CHANCE    20    // percent chance per roll (0-100)
#define SCAVENGE_MAX_ITEMS      8     // max items per pool slot
```

The table uses a hybrid approach: named environment presets for common terrain types, with per-route rows that either reference a preset or inline their own items. All of this lives in `src/follower_scavenge.c` - one file to edit for everything.

### Do This After
Notebook tab system should be stable first - both features touch `event_object_movement.c` and the follower interaction path. No conflicts, but fewer moving parts to juggle at once.

---

## Pokedex Reskin (Field Journal Aesthetic)

**Not yet started.** This is primarily an art job - the code is the smaller half.

The **HGSS-style Pokedex** is active (`src/pokedex_plus_hgss.c`, ~8800 lines). It has 8 distinct screens (list, info, cry, area, size, search, search results, new entry). Each screen has its own BG tileset, pre-compiled tilemap `.bin`, window templates, and palette files. 100+ files under `graphics/pokedex/`.

### Three Levels of Ambition

**Level 1 - Palette + texture swap** (recommended starting point)
Keep all panel shapes and layouts as-is. Replace the HGSS blue-grey chrome with parchment/aged paper tones and earthy outlines. Edit `.gbapal` files and redraw the UI chrome tileset to have a paper texture. Immediately shifts the feel without touching code layouts.
Effort: 1-2 days art, half day integration.

**Level 2 - Notebook chrome** (right long-term target)
Redraw panels to look like an open field journal - ruled lines, tab markers, ink-sketch framing. Same information in approximately the same positions, wrapped in notebook visual language. Requires new tileset art, tilemap regeneration, and minor window template tweaks in `pokedex_plus_hgss.c`.
Effort: 3-5 days art, 1 day code.

**Level 3 - Full two-page spread redesign**
Rethink the layout as a genuine journal: left page has sprite + basic stats, right page has written description, type, habitat. Requires new window positions, new tilemaps, possibly new BG layer assignments. High effort, high conflict risk.
Effort: 1-2 weeks combined. Not recommended until Level 2 is assessed.

### Key Files
- Main impl: `src/pokedex_plus_hgss.c`
- Graphics: `graphics/pokedex/hgss/` (tilesets, palettes, tilemap .bin files)
- Palettes: `hgss/palette_default.gbapal`, `palette_national.gbapal`, etc. (multiple star-rating variants)
- BG templates and window templates: lines 812-1504 in `src/pokedex.c`
- Launched from: `src/start_menu_book.c` via `SetMainCallback2(CB2_OpenPokedex)`

### Do After
Notebook start menu should be stable first - same aesthetic language, same graphics pipeline familiarity.

---

## Trainer Card Notebook Page

**Not yet started.** A notebook page showing trainer identity and exploration stats in place of the vanilla trainer card screen.

### What the Vanilla Card Shows

**Front (keep most of this):**
- Trainer name, ID number, money, play time, Pokedex caught count, trainer mugshot, 8 gym badges, 0-4 stars

**Back (mostly discard - multiplayer stats):**
- Link battle wins/losses, trades, contests, Pokeblocks with friends, Battle Frontier BP
- None of this fits the solo wilderness tone; replace with exploration data

### What a Notebook Page Should Show

**Already in save data, just not surfaced:**
- `GAME_STAT_STEPS` - total steps walked; natural fit for an explorer's journal
- Seen vs caught counts separately (card only shows caught)
- `GAME_STAT_POKEMON_CAPTURES` - total captures all time
- Rival name (`gSaveBlock2Ptr->rivalName`)
- Follower Pokemon species / nickname (derive from `gSaveBlock3Ptr->followerIndex`)
- Quest completion count (quest system data is in SaveBlock2, not currently shown on card)

**Would need to be added:**
- Custom badge names and art for the Maine region (necessary regardless for a custom region)
- Play time formatted as "X days, Y hours" rather than raw HHH:MM - more journal-like

**Things to drop:**
- Multiplayer/link stats (back side of vanilla card)
- Stars system (or repurpose stars as exploration milestones rather than competitive achievements)
- Profile phrase (link battles only, not relevant)

### Implementation Notes
- Entry point: `ShowPlayerTrainerCard()` in `src/trainer_card.c` (1903 lines)
- Called from `src/start_menu_book.c` - already wired into the book menu
- Data populated in `TrainerCard_GenerateCardForPlayer()` / `SetPlayerCardData()`
- Adding new fields: extend `struct TrainerCard` and populate from the GAME_STAT / SaveBlock sources listed above
- Badge art: `graphics/trainer_card/badges.png` - will need custom artwork for the Maine region's 8 gyms
- The back-side screens and their graphics can be gutted and replaced with the exploration stat layout

---

## Recommended Next Steps

1. Notebook start menu tab system - wire stubs for Bag, Pokedex, Trainer Card, Options, Save, PokéNav; implement tab navigation (L/R or dedicated button)
2. DexNav book menu page - inline species list using gWildMonHeaders, mon icons, search levels from SaveBlock3 (DexNav is now enabled - see DexNav section)
3. Music expansion (lower conflict risk)
4. New Birch briefcase with custom starters
