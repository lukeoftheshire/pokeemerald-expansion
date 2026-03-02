# CLAUDE.md

This file provides persistent context for Claude Code. Read it at the start of every session.

Do not use em dashes anywhere in this project - in explanations, documentation, comments, or code.

---

## What This Project Is

A custom Pokemon Emerald ROM hack built on **pokeemerald-expansion** (rh-hideout fork), which is
itself built on pret's pokeemerald decompilation. The output is `pokeemerald.gba`.

The hack has a strong regional identity: **Maine / New England**. Think weathered wood, nautical
charts, general store signage, whaling supply shops in Portland. Earthy and muted. UI should feel
like something the trainer physically carries - a field journal or worn notebook. Nothing loud or
neon unless explicitly requested.

Development is on **macOS**. Primary language is **C** with some ARM Assembly.

---

## Build Commands

```bash
# Standard build
make

# Parallel build (recommended)
make -j$(sysctl -n hw.ncpu)

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
- Earthy, muted palette. Calm legibility over visual excitement.
- Semantic color roles matter. Plan palette slots deliberately.
- Brick red is acceptable but use it sparingly - it reads loud.
- Pine green is a key accent color.
- Parchment tones for text backgrounds where possible.
- Ocean blues and slate grays for cool accents.
- Ask: "could this text box appear on a hand-painted sign outside a whaling supply shop in
  Portland, Maine?" If yes, you are on the right track.

### GBA Palette Constraints
- GBA uses 16-color palettes (15 colors + transparent) per palette slot.
- UI screens share palette budgets. Always plan how many palette slots a screen uses.
- When suggesting UI changes, account for remaining palette slots and suggest uses for them
  (highlight edges, parchment gradients, tab shading, icon contrast) rather than adding decorative colors.

### Motifs
- Pine sprig accents. Subtle placement, not overwhelming. Upper right is an established position.
- Nature motifs generally. Field journal / naturalist notebook cues.
- Avoid loud decorative elements.

### UI Paradigm
- The flagship start menu is a **notebook / field journal** with tabbed pages and bookmark-style party display.
- Menus should feel diegetic - like physical objects the trainer carries.
- Selected states should be subtle, not loud.
- Always design for **240x160** (GBA resolution). Test readability at that size.

### Sprite and Pixel Art Rules (Strict)
Do not violate these without explicit instruction:
- Trainer and Pokemon sprites: exactly **64x64 pixels**
- **4bpp, 16-color palette** (15 colors + transparent)
- No anti-aliasing or subpixel smoothing
- Crisp pixel edges only, transparent background
- When proposing pipelines or tooling, verify they preserve 4bpp palette correctness and do not accidentally add colors

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

### Pending (active backlog)
- Vanilla summary screen expansion
- Game Corner / gacha expansion
- Field mugshot system
- New Birch briefcase with custom starters
- Full screen start menu
- Color variants system
- Music expansion
- Additional custom weather effects (see weather section below)

### Back Burner
- Overworld encounters

### Abandoned for Now
- Rogue-style battle speed-up (may implement manually later)

### Reference Only, Not Planned
- Level scaling

---

## Feature Sources

| Feature | Source |
|---|---|
| Preferred follower toggle | https://github.com/Kasenn/pokeemerald-expansion-kasen/tree/toggle_follower_from_party_menu |
| Overworld encounters | https://github.com/HashtagMarky/pokeemerald/tree/ikigai/ow-encounters |
| FRLG Map previews | https://github.com/Bivurnum/decomps-resources/wiki/FRLG-Map-Previews |
| Vanilla summary screen expanded | https://github.com/TeamAquasHideout/Team-Aquas-Asset-Repo/wiki/Vanilla-Summary-Screen-Expanded |
| Wild encounter message | https://github.com/KaixerRealNewAcc/dynastic-emerald/tree/Wild-Encounter-Message-Changed |
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

---

## Major Initiative: Notebook Start Menu

The flagship UI feature. Stabilize before adding more UI-heavy features.
Active branch: `feature_main_menu`

### Design
- Open notebook / field journal shown as a two-page spread
- Tabs for: Pokedex, Pokemon, Bag, Quest Log, DexNav, Save, Options
- Selected tab highlighted subtly
- Party Pokemon shown as bookmark or inserted card - present but secondary
- Calm bottom panel for contextual text

### Key Files and Symbols
- `src/start_menu_book.c` - main implementation
- `include/start_menu_book.h` - header
- `DisplaySelectedPokemonDetails` - selected Pokemon info handler
- `VBlankCB_BookMenu` - VBlank callback (LoadOam / ProcessSpriteCopyRequests / TransferPlttBuffer)
- `Task_BookMenu_HandleInput(u8 taskId)` - input loop (B/Start closes, SE_SELECT, palette fade)
- `sBookBgTemplates` - BG template array

### Known Bugs

**Double audio and animation on open**
Symptoms: animation and sound play twice on open only.
Check: StartBookMenu called once only; task not created twice; VBlank callback not installed twice;
sound not triggered on both init and first-update state; script-based and UI-based open paths not both firing.

**Fast scrolling breaks animations (cries unaffected)**
Check: StartSpriteAnim only called on real selection change not every input; sprites not
re-initialized on every input; anim IDs not overwritten by task data reuse; OAM updates running
reliably during heavy input.

**StartBookMenu undefined reference at link time**
Check: not declared static; declared in header included by debug.c; src/start_menu_book.c in build
system; declaration signature matches definition; symbol name exact with no macro mangling.

**Horizontal text clipping**
Check: window width in tiles matches tilemap width; text buffer sufficient; GetFontIdToFit receives
a string pointer not a u32 (prior bug: passing level integer caused pointer-from-integer warning).

**Tilemap width mismatch**
Check: tilemap dimensions match BG template screenSize; window template width/height and offsets
consistent; gBgTilemapBuffer copy ranges correct; decompression/blitting writes correct size.

---

## Major Initiative: Quest System and Quest Menu UI

Reference: https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu

Quest icons integrated. Assets pulled from PokemonSanFran:pokeemerald:unbound-quest-menu.
Quest UI serves as a visual standard other menus should match.

Established visual direction:
- Pine sprig in upper right, brown stick plus green needle colors
- Brick red accent used sparingly
- Pine green as key accent
- ~12 colors in current palette, room remains
- Text aims for parchment-like appearance

---

## Major Initiative: Field Weather Effects

Weather effects must render as a sprite/particle layer on top of the scene, or use GBA alpha
blending. No tile-level changes are viable for weather.

### Implemented
**Autumn Leaves** - particle layer. num_leaf_sprites set to 25. If leaves stop rendering: verify
SetWeather called correctly, callback/task invoking leaf spawns is running, palette and tile tags
loaded, debug menu sets correct weather type.

**Fireflies** - stateful animation via FireflyState. Random delay counter prevents sync across
sprites (preserve this pattern always). "Come up from dim" blend values. Animation variants 0-3
selected randomly: `StartSpriteAnim(sprite, rand & 3)`. Do not cause repeated anim restarts.

### Planned
- **Light Snow / Flurries** - slow gentle white particles, calm feel
- **Blizzard** - dense fast horizontal white particles (leaves system reused, high velocity)
- **Nor'easter** - blizzard particles layered over standard rain weather
- **Aurora** - slow semi-transparent color-shifting band in sky region via alpha blend
- **Sea Fog** - alpha-blended white/gray overlay with slow drift particles
- **Wildfire Smoke** - faint sepia/gray semi-transparent overlay, sparse dark particles
- **Embers / Ash** - small rising orange/red particles (firefly system reused, rising direction)
- **Heat Haze** - TBD, distortion effects may not be feasible on GBA

---

## Major Initiative: FRLG-Style Map Previews

Reference: https://github.com/Bivurnum/decomps-resources/wiki/FRLG-Map-Previews

Integrated and working, palettes resolved. Art direction targets Gen 3 authenticity.
Sensitive to overly crunchy pixel edges after downscaling.
Pipeline: start high-res, controlled downscale, palette reduction with manual cleanup, light
dithering on rough geometry (roofs, shorelines). Target 32 colors after reduction.

---

## Recommended Next Steps

1. Stabilize notebook start menu (double audio, fast-scroll anim breakage, text rendering, linkage)
2. Vanilla summary screen expansion (high UI conflict risk)
3. Field mugshot system (watch for overlay and palette conflicts)
4. Music expansion (lower conflict risk)
5. Game Corner gacha expansion
6. Color variants system
7. New Birch briefcase / full-screen start menu (evaluate against notebook paradigm)
