#include "global.h"
#include "strings.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "field_weather.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item.h"
#include "item_menu.h"
#include "item_menu_icons.h"
#include "list_menu.h"
#include "item_icon.h"
#include "item_use.h"
#include "international_string_util.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "party_menu.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text_window.h"
#include "overworld.h"
#include "event_data.h"
#include "constants/items.h"
#include "constants/field_weather.h"
#include "constants/weather.h"
#include "constants/songs.h"
#include "constants/rgb.h"
#include "pokemon_icon.h"
#include "battle_pyramid.h"
#include "region_map.h"
#include "constants/battle_frontier.h"
#include "constants/layouts.h"
#include "rtc.h"
#include "pokedex.h"
#include "trainer_card.h"
#include "pokenav.h"
#include "option_menu.h"
#include "link.h"
#include "frontier_pass.h"
#include "start_menu.h"
#include "move.h"
#include "trainer_pokemon_sprites.h"
#include "pokemon.h"
#include "pokemon_animation.h"
#include "party_menu.h"
#include "pokemon_summary_screen.h"
#include "battle_main.h"
#include "ui_stat_editor.h"
#include "comfy_anim.h"
#include "wild_encounter.h"
#include "follower_scavenge.h"
#include "mining_minigame.h"
#include "field_move.h"
#include "move_relearner.h"
#include "field_screen_effect.h"
#include "field_player_avatar.h"
#include "config/summary_screen.h"
#include "money.h"
#include "dexnav.h"
#include "save.h"
#include "fieldmap.h"
#include "constants/game_stat.h"
#include "start_menu_book.h"



// ==========================================
// BOOK MENU IMPLEMENTATION
// ==========================================

// ------------------------------------------
// Visual Tuning Constants
// Tweak these to adjust the book's look.
// ------------------------------------------

// Type stamps (party page and pokeguide page).
// 0 = invisible, 100 = fully opaque ink stamp.
#define TYPE_STAMP_OPACITY      80

// Sepia strength on item icons (minerals / scavenger tabs).
// 0 = original colors, 100 = fully sepia.
#define ITEM_ICON_SEPIA         40

// Set to 1 to sepia-tint wild held-item icons on the pokeguide page; 0 for full color.
#define PG_WILD_ITEM_SEPIA      1

// ------------------------------------------

struct StartMenuResources
{
    MainCallback savedCallback;     // determines callback to run when we exit. e.g. where do we want to go after closing the menu
    u8 gfxLoadState;
    u16 cursorSpriteId;
    u16 iconBoxSpriteIds[6];
    u16 iconMonSpriteIds[6];
    u16 iconStatusSpriteIds[6];
    u16 selector_x;
    u16 selector_y;
    u16 selectedMenu;
    u16 greyMenuBoxIds[3];
    u16 portraitSpriteId;
    u8 lastSelectedMenu;
    u8 heldItemSpriteId;
    u16 heldItemItemId; // last shown item, for quick no-op updates
    u8 heartSpriteId;
    u8 lastHeartFrame;  // 0xFF = unset
    u8 hpBarSpriteIds[PARTY_SIZE];
    u8 typeIconSpriteIds[2];
    u8 currentTab;
    bool8 isOnWater;    // cached at book-open time; pokeguide uses this to pick land vs water page
    u8 pokeguideIconSpriteIds[20];
    u8 pokeguideIconCount;
    u8 scavengeIconSpriteIds[15];
    u8 scavengeIconCount;
    u8 sharedMatrix;
    u8 badgeSpriteIds[NUM_BADGES];
};





#define SPRITE_NONE 0xFF
#define BOOK_TEXT_BASE  1

// Maximum unique species entries the pokeguide can display (all sections combined).
#define POKEGUIDE_MAX_TOTAL   20
// Maximum unique species shown per type (land or water).
// 9 = clean 3x3 grid; LAND_WILD_COUNT=12 but up to 12 unique species can exist.
#define POKEGUIDE_MAX_LAND    9
#define POKEGUIDE_MAX_WATER   9

static u8 sStartMenuPartyIconSpriteIds[PARTY_SIZE];

// Left page text windows for the book menu
enum WindowIds
{
    WIN_LEFT_NAME,
    WIN_LEFT_MOVES_ALL,
    WIN_RIGHT_LVS,
    WIN_ROUTE,
    WIN_TC_LEFT,   // trainer card: left page text (name, ID, seen, caught)
    WIN_TC_RIGHT,      // trainer card: right page text (journey, miles, money)
    WIN_PG_ROUTE,     // pokeguide: route name on left page top-right
    WIN_PG_INFO,      // pokeguide: right page info panel (name, search level, HA)
    WIN_PG_CATEGORY,  // pokeguide: category text below front sprite
    WIN_LEFT_COUNT,   // sentinel - must stay last (DUMMY_WIN_TEMPLATE uses bg=0xFF to stop InitWindows)
};

enum StartMenuBoxes
{
    START_MENU_POKEDEX,
    START_MENU_PARTY,
    START_MENU_BAG,
    START_MENU_QUESTS,
    START_MENU_DEXNAV,
    START_MENU_CARD,
    START_MENU_MAP,
    START_MENU_OPTIONS,

};

//==========EWRAM==========//
static EWRAM_DATA struct StartMenuResources *sStartMenuDataPtr = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
static EWRAM_DATA u8 *sBg2TilemapBuffer = NULL;
static EWRAM_DATA u8 gSelectedMenu = 0; // holds the position of the menu so that it persists in memory,
                                        // when you go into something like the bag and leave your cursor is still on the bag

static void StartMenuBook_RunSetup(void);
static bool8 StartMenuBook_DoGfxSetup(void);
static bool8 StartMenuBook_InitBgs(void);
void StartBookMenu(MainCallback);
static void StartMenuBook_FadeAndBail(void);
static bool8 StartMenuBook_LoadGraphics(void);
static void StartMenuBook_InitWindows(void);
static void Task_StartMenuBookWaitFadeIn(u8 taskId);
static void Task_StartMenuBookMain(u8 taskId);
//static u32 GetHPEggCyclePercent(u32 partyIndex);
//static void PrintMapNameAndTime(void);
static void CursorCallback(struct Sprite *sprite);
static void DisplaySelectedPokemonDetails(u8 index, bool32 refreshMoves);
static void CreatePartyMonIcons(void);
static void DestroyPartyMonIcons(void);
static void DestroyBookPortraitSprite(void);
static void CreateBookPortraitSprite(u8 partyIndex);
static void SpriteCB_BookPokemon(struct Sprite *sprite);
static void CreateOrUpdateBookHeldItemIcon(u16 itemId);
static void DestroyBookHeldItemIcon(void);
static void CreateOrUpdateBookHeartIcon(u8 friendship);
static void DestroyBookHeartIcon(void);
static void CreatePartyHPBars(void);
static void DestroyPartyHPBars(void);
static void PrintPartyLevels(void);
static void CreateOrUpdateTypeIcons(u16 species);
static void DestroyTypeIconSprites(void);
static void Task_BookMenu_ActionMenu(u8 taskId);
static void Task_BookMenu_ActionMenuOpenAnim(u8 taskId);
static void Task_BookMenu_ActionMenuCloseAnim(u8 taskId);
static void Task_BookMenu_PgSearchExit(u8 taskId);
static void Task_BookMenu_PgRegisterMsg(u8 taskId);
static void CB2_LaunchMiningMinigame(void);
static void Task_BookMenu_MiningExit(u8 taskId);
static void CB2_ReturnToBookTrainer(void);
static void CB2_ReturnToBookParty(void);
static void CB2_BookMenu_OpenPokedexEntry(void);
static void Task_BookMenu_TcOpenPokedex(u8 taskId);
static void Task_BookMenu_TcOpenBag(u8 taskId);
static void Task_BookMenu_TcOpenOptions(u8 taskId);
static void Task_BookMenu_SaveAnim(u8 taskId);
static void Task_BookMenu_SaveMsg(u8 taskId);
static void RefreshFollowerMsgText(void);
static void BuildPokeguideActionList(void);
static void PgSetRightPageSpritesBehindBg(bool8 behind);
static void Task_BookMenu_ItemSubMenu(u8 taskId);
static void Task_BookMenu_ShowFollowerMsg(u8 taskId);
static void Task_BookMenu_SwitchTarget(u8 taskId);
static void Task_StartMenuBookTurnOff(u8 taskId);
static void Task_BookMenuGiveItem(u8 taskId);
static void CB2_BookMenuGiveHoldItem(void);
static bool8 IsDesignatedFollower(u8 selected);
static void BuildBookActionList(u8 selected);
static void BuildBookFieldMoveList(u8 slot);
static u8 FindBookAction(u8 actionId);
static void DrawBookActionMenu(u8 windowId, u8 cursor, bool8 isFollower);
static void DrawBookItemSubMenu(u8 windowId, u8 cursor, bool8 hasTake);
static void DrawBookFollowerSubMenu(u8 windowId, u8 cursor, u8 followerState);
static void DrawBookFieldMovesSubMenu(u8 windowId, u8 cursor);
static void UpdateBookMenuCursorArrow(u8 windowId, u8 oldCursor, u8 newCursor, const u8 *colors);
static void Task_BookMenu_FollowerSubMenu(u8 taskId);
static void Task_BookMenu_FieldMovesSubMenu(u8 taskId);
static void CB2_BookMenu_ExecuteFieldMove(void);
static void CB2_BookMenu_OpenFlyMap(void);
static void Task_BookMenuOpenRelearner(u8 taskId);
static void Task_BookMenu_HeightTransitionAnim(u8 taskId);
static void Task_BookMenu_FieldMoveError(u8 taskId);
static const u32 *BookTab_GetTiles(u8 tabId);
static const u32 *BookTab_GetTilemap(u8 tabId);
static const u16 *BookTab_GetPalette(u8 tabId);
static void InitialTabLoad(void);
static void BookMenu_SetRightPageSpritePriority(u8 priority);
static void CreateSwitchSourceIndicator(u8 taskId, u8 srcSlot);
static void DestroySwitchSourceIndicator(u8 taskId);
static void BeginCloseBookActionMenu(u8 taskId, u8 closeAction);
static void ShowFollowerMsgBox(void);
static void LoadPartyTabContent(void);
static void UnloadPartyTabContent(void);
static void LoadPokeGuideContent(void);
static void UnloadPokeGuideContent(void);
static void PgClearRightPage(void);
static void PgUpdateRightPage(u8 idx);
static void PgMoveCursor(u8 idx);
static void LoadMineralsContent(void);
static void UnloadMineralsContent(void);
static void AddScavengeItemIcon(u16 itemId, s16 x, s16 y);
static void LoadTrainerCardContent(void);
static void UnloadTrainerCardContent(void);
static void LoadScavengerContent(void);
static void UnloadScavengerContent(void);
static void LoadBlankContent(void);
static void UnloadBlankContent(void);
static void LoadEndMapContent(void);
static void UnloadEndMapContent(void);
static void BeginTabSwitch(u8 taskId, s8 direction);
static void Task_BookMenu_TabFlipAnim(u8 taskId);

// Slot preserved across fade callbacks (not in sStartMenuDataPtr since it gets freed)
static EWRAM_DATA u8 sActionMenuSelectedSlot = 0;
// Message string for follower/give-item confirmation (nickname + item name + short suffix)
static EWRAM_DATA u8 sFollowerMsgStr[POKEMON_NAME_LENGTH + ITEM_NAME_LENGTH + 12] = {};
// Pending give-item result: non-zero means show confirmation on book menu re-open
static EWRAM_DATA u16 sPendingGiveItemId = {};
// Comfy anim struct for action menu slide-in/out (persists across task transitions)
static EWRAM_DATA struct ComfyAnim sActionMenuSlideAnim = {};
// Window ID for the follower confirmation message box (set to SPRITE_NONE in StartBookMenu)
static EWRAM_DATA u8 sFollowerMsgWindowId;
static EWRAM_DATA u8 sPgRegisterMsgWindowId;
// Pokeguide: list of unique species on the current map (populated by LoadPokeGuideContent)
static EWRAM_DATA u16 sPokeguideSpeciesList[POKEGUIDE_MAX_TOTAL] = {};
static EWRAM_DATA u8  sPokeguideTotal = 0;
// Badge tiles rearranged from BG format to OBJ sprite format (computed in LoadTrainerCardContent)
static EWRAM_DATA u8 sBadgeTilesOBJ[NUM_BADGES * 4 * TILE_SIZE_4BPP];

// PokéGuide cursor state
static EWRAM_DATA u8  sPgCursorIdx;
static EWRAM_DATA u8  sPgCursorSpriteId;
static EWRAM_DATA u8  sPgNullSpriteIds[9];
static EWRAM_DATA u8  sPgMonPicSpriteId;
static EWRAM_DATA u8  sPgTypeIconSpriteIds[2];
static EWRAM_DATA u8  sPgCaughtSpriteId;
static EWRAM_DATA u8  sPgCaughtAllSpriteId;
static EWRAM_DATA u8  sPgEvSpriteIds[6];
static EWRAM_DATA u8  sPgStarSpriteIds[3];
static EWRAM_DATA u8  sPgWildItemSpriteIds[2];
// 24x24 cursor tiles padded to 32x32 (16 tiles) before LoadSpriteSheet (u32 for alignment)

// Dynamic action menu list (built at open time for each pokemon).
// Size 9 = max actions (BOOK_ACTION_MAX defined below constants section).
static EWRAM_DATA u8  sBookMenuActions[9] = {};
static EWRAM_DATA u8  sBookMenuActionCount = 0;
// Adaptive action menu dimensions (computed from sBookMenuActionCount at open time).
static EWRAM_DATA u8  sActionMenuHeightPx = 0;
static EWRAM_DATA u8  sActionMenuTilemapTopFinal = 0;
// Safe off-screen start row for the slide-in anim: min(20, 32 - heightTiles).
// GBA BG tilemap is 32 rows; a window at row T with height H occupies rows T..T+H-1.
// Row 32+ wraps to row 0 (visible), causing a glitch. For H=13, safe start = 19 not 20.
static EWRAM_DATA u8  sActionMenuTilemapTopStart = 0;
// Field moves available for the currently selected pokemon.
static EWRAM_DATA u8  sBookFieldMoves[FIELD_MOVES_COUNT] = {};
static EWRAM_DATA u8  sBookFieldMoveCount = 0;
// Field move chosen in submenu, preserved across the fade callback.
static EWRAM_DATA u8  sFieldMoveToUse = 0;
// Pending exit callback for field move execution (CB2_ReturnToField or CB2_OpenFlyMap).
static EWRAM_DATA MainCallback sFieldMoveExitCB = NULL;

// Height transition direction for Task_BookMenu_HeightTransitionAnim.
#define HEIGHT_TRANSITION_TO_FIELD_MOVES  0
#define HEIGHT_TRANSITION_TO_ACTION_MENU  1
static EWRAM_DATA u8 sHeightTransitionTarget = 0;

// Fixed action IDs (not indices - the visible list is built dynamically).
#define BOOK_ACTION_POKEDEX      0   // open pokedex entry for selected pokemon
#define BOOK_ACTION_SUMMARY      1
#define BOOK_ACTION_SWITCH       2
#define BOOK_ACTION_FOLLOWER     3
#define BOOK_ACTION_ITEM         4
#define BOOK_ACTION_STAT_EDIT    5   // conditional: only if FLAG_CAN_USE_STAT_EDITOR
#define BOOK_ACTION_FIELD_MOVES  6   // conditional: only if pokemon has usable field moves
#define BOOK_ACTION_RELEARNER    7   // conditional: only if P_PARTY_MOVE_RELEARNER and mon has learnable moves
#define BOOK_ACTION_PG_SEARCH    8   // pokeguide: search for species in field
#define BOOK_ACTION_PG_REGISTER  9   // pokeguide: register species to R button
#define BOOK_ACTION_MAX          11  // max slots in dynamic action list

// Item sub-menu options
#define BOOK_ITEM_GIVE  0
#define BOOK_ITEM_TAKE  1

// Popup window color indices (all into whichever BG palette bank the window uses).
// Adjust these to restyle the action menu, follower msg box, and item/field move submenus.
#define BOOK_MENU_BG_COLOR      2   // window fill / background
#define BOOK_MENU_BORDER_COLOR  10   // 1-pixel border / lining
#define BOOK_MENU_FONT_BG       0   // text background (0 = transparent)
#define BOOK_MENU_FONT_FG       10   // text foreground / ink
#define BOOK_MENU_FONT_SHADOW   0   // text shadow (0 = transparent)

#define BOOK_ACTION_BORDER_COLOR BOOK_MENU_BORDER_COLOR  // legacy alias

// Pending action after the close slide-out animation completes
#define CLOSE_THEN_MAIN        0
#define CLOSE_THEN_SWITCH      1
#define CLOSE_THEN_FOLLOWER    2
#define CLOSE_THEN_PG_SEARCH   3  // pokeguide: fade + start dexnav field search
#define CLOSE_THEN_PG_REGISTER 4  // pokeguide: show register confirmation window

// Task data fields used across action/item/switch modes
#define tActionWindowId      data[1]
#define tActionCursor        data[2]
#define tSwitchSource        data[3]
#define tSwitchIndicatorId   data[4]
#define tItemCursor          data[4]   // reuses data[4]; mutually exclusive with switch mode
#define tItemHasTake         data[5]
#define tFollowerState       data[5]   // reuses data[5]; one of FOLLOWER_STATE_* constants below
#define tCloseAction         data[6]

// Follower submenu states stored in tFollowerState
#define FOLLOWER_STATE_IMPLICIT  0  // followerIndex == NOT_SET, slot 0 implicitly follows
#define FOLLOWER_STATE_EXPLICIT  1  // followerIndex == selected, explicitly set
#define FOLLOWER_STATE_RECALLED  2  // followerIndex == RECALLED, slot 0 can take out
#define tIsFollower          data[7]   // TRUE when selected mon is the designated follower

// Tilemap X for action menu (fixed, not animated).
#define ACTION_MENU_TILEMAP_FINAL   18
// Off-screen Y: menu top starts at tile 20 (just below the 20-tile screen height).
#define ACTION_MENU_TILEMAPTOP_OFF  20
// Width of action menu in pixels (fixed).
#define ACTION_MENU_PX_W            (11 * 8)

#define TAG_SWITCH_INDICATOR   30010

// Derived from tuning constants above - do not edit directly.
#define TYPE_STAMP_BLEND_EVA  ((TYPE_STAMP_OPACITY * 16 + 50) / 100)
#define TYPE_STAMP_BLEND_EVB  (16 - TYPE_STAMP_BLEND_EVA)
#define ITEM_ICON_SEPIA_BLEND16  ((ITEM_ICON_SEPIA * 16 + 50) / 100)

#define TAG_TYPE_STAMPS_TILES  30020
#define TAG_TYPE_STAMPS_PAL    30021
#define TAG_BADGES_TILES       30030
#define TAG_BADGES_PAL         30031
#define TAG_PG_CURSOR_TILES    30040
#define TAG_PG_CURSOR_PAL      30041
#define TAG_PG_NULL_TILES      30042
#define TAG_PG_BLACK_PAL       30043
#define TAG_PG_CAUGHT_TILES    30044
#define TAG_PG_CAUGHT_ALL_TILES 30045
#define TAG_PG_EV_OK_TILES     30046
#define TAG_PG_EV_GOOD_TILES   30047
#define TAG_PG_EV_GREAT_TILES  30048
#define TAG_PG_STAR_TILES      30049
#define TAG_PG_TYPE_UNKNOWN_TILES  30054
#define TAG_PG_ITEM_UNKNOWN_TILES  30055
#define TAG_PG_WILD_ITEM1_TILES  30050
#define TAG_PG_WILD_ITEM1_PAL    30051
#define TAG_PG_WILD_ITEM2_TILES  30052
#define TAG_PG_WILD_ITEM2_PAL    30053
#define TAG_TC_CURSOR_PAL        30061

// Trainer card tab cursor layout
// Three icons (Pokedex, Bag, Options) are stacked at x=203.
// Icon height: 28px. Gap between icons: 8px. Stride: 36px.
#define TC_CURSOR_X    173
#define TC_ICON_0_Y    59
#define TC_ICON_STRIDE 27
#define TC_ICON_COUNT  3

// Search level star thresholds
#define PG_STAR_THRESH_1    0 //5
#define PG_STAR_THRESH_2    1 //30
#define PG_STAR_THRESH_3   3 //100

// EV sprite positions: 8x8 sprites centered inside 10x10 boxes at (213,47), stacked 10px apart
#define PG_EV_CENTER_X      218
#define PG_EV_CENTER_Y0      51
#define PG_EV_STEP           9

// Caught badge and caught-all badge positions (top-left pixel -> center for CreateSprite)
#define PG_CAUGHT_CENTER_X  216   // top-left (206,17) + 8
#define PG_CAUGHT_CENTER_Y   28
#define PG_CAUGHT_ALL_CTR_X  32   // top-left (10,23) + 32
#define PG_CAUGHT_ALL_CTR_Y  42

// Search level star row: 3 stars at top-left (194,111) spaced 11px, centers +4
#define PG_STAR_CENTER_X0   200
#define PG_STAR_CENTER_Y    114
#define PG_STAR_STEP         11

#define TYPE_ICON_X  98
#define TYPE_ICON_X2  110
#define TYPE_ICON_Y1 133
#define TYPE_ICON_Y2 148


static const int baseBlock = 1;


// Action menu popup window.
// tilemapLeft=18 is the resting position; slide anim starts it at ACTION_MENU_TILEMAP_OFF.
// Tile budget: 11*13=143 tiles (85-227). WIN_LEFT_MOVES_ALL starts at 228, so no overlap.
static const struct WindowTemplate sBookActionMenuTemplate =
{
    .bg = 0,
    .tilemapLeft = ACTION_MENU_TILEMAP_FINAL,
    .tilemapTop = 7,
    .width = 11,
    .height = 13,   // fits up to 8 actions at 12px stride; adaptive tilemapTop hides unused rows
    .paletteNum = 1,
    .baseBlock = 85, // tiles 85-227; WIN_LEFT_MOVES_ALL safely above at 228
};

// Follower/save/notification message window (bottom-right, shown on any tab).
// baseBlock is selected dynamically in ShowFollowerMsgBox based on current tab
// so each tab can reuse its idle tile range without exceeding the 512-tile charBase limit.
// Party tab:    85  (action menu freed; 85-159 safe, under WIN_LEFT_NAME 1-84 and WIN_LEFT_MOVES_ALL 206+)
// TC tab:       331 (after WIN_TC_LEFT 85-252 and WIN_TC_RIGHT 253-330)
// PG tab:       236 (after WIN_PG_CATEGORY 206-235; action menu sequential)
// Other tabs:   85  (minimal windows, no conflict)
#define FOLLOWER_MSG_PX_W (15 * 8)
#define FOLLOWER_MSG_PX_H (5 * 8)
static const struct WindowTemplate sBookFollowerMsgTemplate =
{
    .bg = 0,
    .tilemapLeft = 15,
    .tilemapTop = 13,
    .width = 15,
    .height = 5,
    .paletteNum = 1,
    .baseBlock = 85, // overridden dynamically in ShowFollowerMsgBox
};

// PokéGuide-specific action menu: height=5 tiles (fits 2 actions at 16px stride with FONT_NORMAL).
// baseBlock=236: after WIN_PG_CATEGORY (206-235), avoids conflict with WIN_PG_INFO (101-205).
// Sequential with sBookPgRegisterMsgTemplate (280+); never active simultaneously, overlap is safe.
static const struct WindowTemplate sBookPgActionMenuTemplate =
{
    .bg = 0,
    .tilemapLeft = ACTION_MENU_TILEMAP_FINAL,
    .tilemapTop = 20,  // starts off-screen; slide anim moves up to tilemapTop=15
    .width = 11,
    .height = 5,
    .paletteNum = 1,
    .baseBlock = 236,  // tiles 236-290; safe from pokeguide windows (sequential with PgRegisterMsg)
};

// PokéGuide register confirmation window. Shown after PG action menu closes (tiles 236+ freed).
// Shares dimensions with follower msg box for visual consistency.
#define PG_REGISTER_MSG_PX_W FOLLOWER_MSG_PX_W
#define PG_REGISTER_MSG_PX_H FOLLOWER_MSG_PX_H
static const struct WindowTemplate sBookPgRegisterMsgTemplate =
{
    .bg = 0,
    .tilemapLeft = 15,
    .tilemapTop = 13,
    .width = 15,
    .height = 5,
    .paletteNum = 1,
    .baseBlock = 236,  // PG action menu closed before this shows; tiles 236-310 free
};

static const u8 sBookActionText_Arrow[]         = _("▶");
static const u8 sBookActionText_Pokedex[]       = _(" Pokédex");
static const u8 sBookActionText_Summary[]       = _(" Summary");
static const u8 sBookActionText_Switch[]        = _(" Switch");
static const u8 sBookActionText_SetFollower[]   = _(" Set Follower");
static const u8 sBookActionText_IsFollower[]    = _(" Follower");
static const u8 sBookActionText_Item[]          = _(" Item");
static const u8 sBookActionText_StatEdit[]      = _(" Stat Edit");
static const u8 sBookActionText_FieldMoves[]    = _(" Field Moves");
static const u8 sBookActionText_Relearner[]     = _(" Move Tutor");

static const u8 sBookFollowerText_Unset[]    = _(" Unset");
static const u8 sBookFollowerText_Set[]      = _(" Set");
static const u8 sBookFollowerText_TakeOut[]  = _(" Take Out");
static const u8 sBookFollowerText_Return[]   = _(" Return");

static const u8 sBookItemText_Give[]         = _(" Give Item");
static const u8 sBookItemText_Take[]         = _(" Take Item");

static const u8 sBookText_IsNowFollowing[]      = _(" is now\nfollowing!");
static const u8 sBookText_Recalled[]            = _(" recalled.");
static const u8 sBookText_NoLongerFollower[]    = _(" is no longer\nset as follower.");
static const u8 sBookText_GotItem[]          = _(" is now holding\n");
static const u8 sBookText_ExclaimEnd[]       = _("!");
static const u8 sBookText_CantUseHere[]      = _("Can't use that here.");
static const u8 sBookText_RegisteredToL[]    = _(" registered!\nPress L to search!");

static const u8 sBookActionText_PgSearch[]   = _(" Search");
static const u8 sBookActionText_PgRegister[] = _(" Register to L");

// Indexed by BOOK_ACTION_* ID. FOLLOWER uses SetFollower by default;
// DrawBookActionMenu swaps in IsFollower when tIsFollower is TRUE.
static const u8 *const sBookActionTexts[BOOK_ACTION_MAX] =
{
    [BOOK_ACTION_POKEDEX]     = sBookActionText_Pokedex,
    [BOOK_ACTION_SUMMARY]     = sBookActionText_Summary,
    [BOOK_ACTION_SWITCH]      = sBookActionText_Switch,
    [BOOK_ACTION_FOLLOWER]    = sBookActionText_SetFollower,
    [BOOK_ACTION_ITEM]        = sBookActionText_Item,
    [BOOK_ACTION_STAT_EDIT]   = sBookActionText_StatEdit,
    [BOOK_ACTION_FIELD_MOVES] = sBookActionText_FieldMoves,
    [BOOK_ACTION_RELEARNER]   = sBookActionText_Relearner,
    [BOOK_ACTION_PG_SEARCH]   = sBookActionText_PgSearch,
    [BOOK_ACTION_PG_REGISTER] = sBookActionText_PgRegister,
};

/// BG Configuration
static const struct BgTemplate sBookBgTemplates[] =
{
    // Text Layer (Front)
    {
        .bg = 0,
        .charBaseIndex = 1, // Text tiles go here
        .mapBaseIndex = 31,
        .priority = 0,      // High priority (on top)
    },
    // Book Art Layer (Back)
    {
        .bg = 1,
        .charBaseIndex = 0, // Book tiles go here (separate block)
        .mapBaseIndex = 30,
        .paletteMode = 0,
        .priority = 3,      // Furthest back; OBJ priority 2 sits above it, weather OBJ priority 1 above that
    },
    {
        .bg = 2,    // Flip animation layer; charBase 2 is separate from BG1's charBase 0
        .charBaseIndex = 2,
        .mapBaseIndex = 28,
        .priority = 3,
    },
//    {
//        .bg = 3,    // If I get really bored, maybe this will hold backgrounds based on...location?timeOfDay?
//        .charBaseIndex = 3,
//        .mapBaseIndex = 26,
//        .priority = 3
//    }
};

static const u8 moveWindowWidth = 9;
static const u8 moveWindowHeight = 1;

static const struct WindowTemplate sBookWindows[] =
{
    [WIN_LEFT_NAME]   = //Window the currently selected pokemon name is is printed to.
     { .bg = 0, //text prints on bg 0
       .tilemapLeft = 1, //position from left (*8)
       .tilemapTop = 1,  //position from top (8*)
       .width = 14,
       .height = 6,
       .paletteNum = 1,
       .baseBlock = baseBlock },
      [WIN_LEFT_MOVES_ALL] = {
          .bg = 0,
          .tilemapLeft = 1,
          .tilemapTop = 13,
          .width = 14,
          .height = 6,
          .paletteNum = 1,
          .baseBlock = baseBlock + 227}, // starts at 228; above action menu's max reach (85..227)

    [WIN_RIGHT_LVS] = {
        .bg = 0,
        .tilemapLeft = 16,  // x = 128px, just left of left-column HP bars
        .tilemapTop = 6,    // y = 48px, just above row-0 level text
        .width = 12,
        .height = 13,       // 104px, covers all 3 party rows (last textY=87+12=99 < 104)
        .paletteNum = 1,
        .baseBlock = baseBlock + 311,   // WIN_LEFT_MOVES_ALL ends at tile 311 (228 + 14*6 - 1); start here
    },
    [WIN_ROUTE] = {
        .bg = 0,
        .tilemapLeft = 15,   // right page, slight indent
        .tilemapTop = 1,
        .width = 16,
        .height = 3,
        .paletteNum = 1,
        .baseBlock = baseBlock,//586    // can go on it's on level as will never be displayed at same time as party windows
    },
    // Trainer card tab: dedicated windows that don't share tile memory with party tab.
    // Tiles 85-252: WIN_TC_LEFT  (14*12=168 tiles; reuses action-menu range, mutually exclusive)
    // WIN_TC_RIGHT reserved for future right-page button UI; no text drawn to it currently.
    [WIN_TC_LEFT] = {
        .bg         = 0,
        .tilemapLeft = 1,
        .tilemapTop  = 1,
        .width       = 14,
        .height      = 12,
        .paletteNum  = 1,
        .baseBlock   = 85,   // tiles 85-252; action menu never open on trainer card tab
    },
    [WIN_TC_RIGHT] = {
        .bg         = 0,
        .tilemapLeft = 16,
        .tilemapTop  = 2,
        .width       = 13,
        .height      = 6,
        .paletteNum  = 1,
        .baseBlock   = 253,  // reserved; right page used for interactive buttons (future)
    },
    // PokéGuide tab: route name window on right portion of left page.
    // Tiles 85-101: WIN_PG_ROUTE (8*2=16 tiles; reuses action-menu range, mutually exclusive)
    [WIN_PG_ROUTE] = {
        .bg         = 0,
        .tilemapLeft = 7,    // x=56px; right portion of left page (left page ends ~tile 14)
        .tilemapTop  = 1,
        .width       = 8,
        .height      = 2,
        .paletteNum  = 1,
        .baseBlock   = 85,   // tiles 85-100; never conflicts with trainer card or action menu
    },
    // PokéGuide tab: right page info panel (name, search level, HA).
    // Starts at ~pixel (120,16). Tiles 101-205: 15*7=105 tiles.
    [WIN_PG_INFO] = {
        .bg         = 0,
        .tilemapLeft = 15,
        .tilemapTop  = 1,
        .width       = 15,
        .height      = 7,
        .paletteNum  = 1,
        .baseBlock   = 101,
    },
    // PokéGuide tab: category text below front sprite (~pixel 136,104).
    // Tiles 206-235: 10*3=30 tiles.
    [WIN_PG_CATEGORY] = {
        .bg         = 0,
        .tilemapLeft = 14,
        .tilemapTop  = 13,
        .width       = 10,
        .height      = 3,
        .paletteNum  = 1,
        .baseBlock   = 206,
    },
    [WIN_LEFT_COUNT] = DUMMY_WIN_TEMPLATE,
};

//Graphics
static const u8 sFontColorTable[][3] =
{
    {TEXT_COLOR_TRANSPARENT, TEXT_DYNAMIC_COLOR_1, TEXT_COLOR_TRANSPARENT},  // (Dark red d.t. palette)
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_BLUE, TEXT_COLOR_TRANSPARENT},  // Default
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_TRANSPARENT},  // Default
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,      TEXT_COLOR_BLUE},      // Unused
    {TEXT_COLOR_TRANSPARENT, TEXT_DYNAMIC_COLOR_2,  TEXT_DYNAMIC_COLOR_3},  // Gender symbol
    {TEXT_DYNAMIC_COLOR_4,       TEXT_DYNAMIC_COLOR_5,  TEXT_DYNAMIC_COLOR_6}, // Selection actions
    {TEXT_COLOR_WHITE,       TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_BLUE}, // Field moves
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},  // Unused
    {TEXT_COLOR_WHITE,       TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_RED},  // Move relearner
};

// Background: one tileset + tilemap + palette per tab, plus two flip frames.
// Palettes are per-tab; each is loaded when the tab becomes active.
static const u32 sBookTiles_Trainer[]          = INCBIN_U32("graphics/start_menu_book/book_pages/page_1_trainer_tiles.4bpp.lz");
static const u32 sBookTilemap_Trainer[]        = INCBIN_U32("graphics/start_menu_book/book_pages/page_1_trainer_tiles.bin.lz");
static const u16 sBookPalette_Trainer[]        = INCBIN_U16("graphics/start_menu_book/book_pages/page_1_trainer_tiles.gbapal");

static const u32 sBookTiles_Party[]            = INCBIN_U32("graphics/start_menu_book/book_pages/page_2_party_tiles.4bpp.lz");
static const u32 sBookTilemap_Party[]          = INCBIN_U32("graphics/start_menu_book/book_pages/page_2_party_tiles.bin.lz");
static const u16 sBookPalette_Party[]          = INCBIN_U16("graphics/start_menu_book/book_pages/page_2_party_tiles.gbapal");

static const u32 sBookTiles_PokeGuideLand[]    = INCBIN_U32("graphics/start_menu_book/book_pages/page_3_pokeguide_land_tiles.4bpp.lz");
static const u32 sBookTilemap_PokeGuideLand[]  = INCBIN_U32("graphics/start_menu_book/book_pages/page_3_pokeguide_land_tiles.bin.lz");
static const u16 sBookPalette_PokeGuideLand[]  = INCBIN_U16("graphics/start_menu_book/book_pages/page_3_pokeguide_land_tiles.gbapal");

static const u32 sBookTiles_PokeGuideWater[]   = INCBIN_U32("graphics/start_menu_book/book_pages/page_3_pokeguide_water_tiles.4bpp.lz");
static const u32 sBookTilemap_PokeGuideWater[] = INCBIN_U32("graphics/start_menu_book/book_pages/page_3_pokeguide_water_tiles.bin.lz");
static const u16 sBookPalette_PokeGuideWater[] = INCBIN_U16("graphics/start_menu_book/book_pages/page_3_pokeguide_water_tiles.gbapal");

static const u32 sBookTiles_Scavenger[]        = INCBIN_U32("graphics/start_menu_book/book_pages/page_4_scavenge_tiles.4bpp.lz");
static const u32 sBookTilemap_Scavenger[]      = INCBIN_U32("graphics/start_menu_book/book_pages/page_4_scavenge_tiles.bin.lz");
static const u16 sBookPalette_Scavenger[]      = INCBIN_U16("graphics/start_menu_book/book_pages/page_4_scavenge_tiles.gbapal");

static const u32 sBookTiles_Minerals[]         = INCBIN_U32("graphics/start_menu_book/book_pages/page_5_minerals_tiles.4bpp.lz");
static const u32 sBookTilemap_Minerals[]       = INCBIN_U32("graphics/start_menu_book/book_pages/page_5_minerals_tiles.bin.lz");
static const u16 sBookPalette_Minerals[]       = INCBIN_U16("graphics/start_menu_book/book_pages/page_5_minerals_tiles.gbapal");

static const u32 sBookTiles_Blank[]            = INCBIN_U32("graphics/start_menu_book/book_pages/page_blank_tiles.4bpp.lz");
static const u32 sBookTilemap_Blank[]          = INCBIN_U32("graphics/start_menu_book/book_pages/page_blank_tiles.bin.lz");
static const u16 sBookPalette_Blank[]          = INCBIN_U16("graphics/start_menu_book/book_pages/page_blank_tiles.gbapal");

static const u32 sBookTiles_EndMap[]           = INCBIN_U32("graphics/start_menu_book/book_pages/page_10_end_map_tiles.4bpp.lz");
static const u32 sBookTilemap_EndMap[]         = INCBIN_U32("graphics/start_menu_book/book_pages/page_10_end_map_tiles.bin.lz");
static const u16 sBookPalette_EndMap[]         = INCBIN_U16("graphics/start_menu_book/book_pages/page_10_end_map_tiles.gbapal");

static const u32 sBookTiles_FlipRight[]        = INCBIN_U32("graphics/start_menu_book/book_pages/page_turnright_tiles.4bpp.lz");
static const u32 sBookTilemap_FlipRight[]      = INCBIN_U32("graphics/start_menu_book/book_pages/page_turnright_tiles.bin.lz");
static const u16 sBookPalette_FlipRight[]      = INCBIN_U16("graphics/start_menu_book/book_pages/page_turnright_tiles.gbapal");

static const u32 sBookTiles_FlipLeft[]         = INCBIN_U32("graphics/start_menu_book/book_pages/page_turnleft_tiles.4bpp.lz");
static const u32 sBookTilemap_FlipLeft[]       = INCBIN_U32("graphics/start_menu_book/book_pages/page_turnleft_tiles.bin.lz");
static const u16 sBookPalette_FlipLeft[]       = INCBIN_U16("graphics/start_menu_book/book_pages/page_turnleft_tiles.gbapal");

// Consistent text/ink palette loaded into BG slot 1 at startup; never swapped out.
// BG slot 0 holds the current page art palette and changes per tab.
// For dark red to render as text ink, ensure it sits at index 1 in this palette.
static const u16 sBookPalette_Text[]           = INCBIN_U16("graphics/start_menu_book/book_pal.gbapal");

// trainer card tab cursors (64x64 canvas each; raw 4bpp for image-array sprite; all share cursor.gbapal)
static const u8 sTcCursor_Pokedex_Gfx[] = INCBIN_U8("graphics/start_menu_book/cursor/trainer_card_cursor_pokedex.4bpp");
static const u8 sTcCursor_Bag_Gfx[]     = INCBIN_U8("graphics/start_menu_book/cursor/trainer_card_cursor_bag.4bpp");
static const u8 sTcCursor_Options_Gfx[] = INCBIN_U8("graphics/start_menu_book/cursor/trainer_card_cursor_options.4bpp");

//party selection cursor
static const u32 sCursor_Gfx[] = INCBIN_U32("graphics/start_menu_book/cursor/cursor.4bpp.lz");
static const u16 sCursor_Pal[] = INCBIN_U16("graphics/start_menu_book/cursor/cursor.gbapal");

// pokeguide cursor (24x24 canvas, cursor in px 0-18; padded to 32x32 at load time)
static const u32 sPgCursor_Gfx[] = INCBIN_U32("graphics/start_menu_book/cursor/cursor_pokeguide.4bpp");
static const u16 sPgCursor_Pal[] = INCBIN_U16("graphics/start_menu_book/cursor/cursor_pokeguide.gbapal");
// null slot icon (replaces question-mark for absent pokemon; shares pokeguide cursor palette)
static const u8 sPgNull_Gfx[]        = INCBIN_U8("graphics/start_menu_book/cursor/null.4bpp");
// unknown type / item placeholders (both share cursor palette)
static const u8 sPgTypeUnknown_Gfx[] = INCBIN_U8("graphics/start_menu_book/cursor/type_unknown.4bpp");
static const u8 sPgItemUnknown_Gfx[] = INCBIN_U8("graphics/start_menu_book/cursor/item_unknown.4bpp");
// pokeguide overlay sprites (all share cursor palette)
static const u32 sPgCaught_Gfx[]    = INCBIN_U32("graphics/start_menu_book/cursor/pokeguide_caught.4bpp");
static const u32 sPgCaughtAll_Gfx[] = INCBIN_U32("graphics/start_menu_book/cursor/guide_caught_em_all.4bpp");
static const u32 sPgEvOk_Gfx[]      = INCBIN_U32("graphics/start_menu_book/cursor/ev_ok.4bpp");
static const u32 sPgEvGood_Gfx[]    = INCBIN_U32("graphics/start_menu_book/cursor/ev_good.4bpp");
static const u32 sPgEvGreat_Gfx[]   = INCBIN_U32("graphics/start_menu_book/cursor/ev_great.4bpp");
static const u32 sPgStar_Gfx[]      = INCBIN_U32("graphics/start_menu_book/cursor/search_level_star.4bpp");

// trainer card badges (8 badges, each 16x16px = 128 bytes, total 1024 bytes)
static const u8 sBadgeTiles[] = INCBIN_U8("graphics/trainer_card/badges.4bpp");
static const u16 sBadgePal[]  = INCBIN_U16("graphics/trainer_card/badges.gbapal");

// friendship hearts (share cursor palette)
static const u8 sHeart0_Gfx[]    = INCBIN_U8("graphics/start_menu_book/friendship_heart/heart_0.4bpp");
static const u8 sHeart25_Gfx[]   = INCBIN_U8("graphics/start_menu_book/friendship_heart/heart_25.4bpp");
static const u8 sHeart50_Gfx[]   = INCBIN_U8("graphics/start_menu_book/friendship_heart/heart_50.4bpp");
static const u8 sHeart75_Gfx[]   = INCBIN_U8("graphics/start_menu_book/friendship_heart/heart_75.4bpp");
static const u8 sHeart100_Gfx[]  = INCBIN_U8("graphics/start_menu_book/friendship_heart/heart_100.4bpp");
static const u8 sHeartPerf_Gfx[]  = INCBIN_U8("graphics/start_menu_book/friendship_heart/heart_perf.4bpp");
static const u8 sHeartPerf2_Gfx[] = INCBIN_U8("graphics/start_menu_book/friendship_heart/heart_perf2.4bpp");
static const u8 sHeartPerf3_Gfx[] = INCBIN_U8("graphics/start_menu_book/friendship_heart/heart_perf3.4bpp");

// HP bars (32x32, share cursor palette)
static const u8 sHPBar0_Gfx[]   = INCBIN_U8("graphics/start_menu_book/hp_bar/hp0.4bpp");
static const u8 sHPBar1_Gfx[]   = INCBIN_U8("graphics/start_menu_book/hp_bar/hp1.4bpp");
static const u8 sHPBar10_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp10.4bpp");
static const u8 sHPBar20_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp20.4bpp");
static const u8 sHPBar30_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp30.4bpp");
static const u8 sHPBar40_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp40.4bpp");
static const u8 sHPBar50_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp50.4bpp");
static const u8 sHPBar60_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp60.4bpp");
static const u8 sHPBar70_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp70.4bpp");
static const u8 sHPBar80_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp80.4bpp");
static const u8 sHPBar90_Gfx[]  = INCBIN_U8("graphics/start_menu_book/hp_bar/hp90.4bpp");
static const u8 sHPBar100_Gfx[] = INCBIN_U8("graphics/start_menu_book/hp_bar/hp100.4bpp");


//
//  Sprite Data for Cursor, IconBox, GreyedBoxes, and Statuses
//
#define TAG_CURSOR 30004
#define TAG_ICON_BOX 30006
#define TAG_HPBAR   30008
#define TAG_STATUS_ICONS 30009
#define TAG_BOOK_HELD_ITEM_TILES   0xE100
#define TAG_BOOK_HELD_ITEM_PAL     0xE101

/// Scavenger and Minerals tab item icon tags.
// Per-tier display limits: MINING_DISPLAY_MAX_* in constants/mining_minigame.h (6/6/3).
// OBJ palette budget on these tabs: cursor and type stamps only load on party tab; all 16 slots free.
// Theoretical max (8+8+3=19) exceeds the 14-slot OBJ budget; well-designed pools
// keep total icons <= 14. Items beyond the budget silently fail to render.
// Tags 0xE200..0xE212 for tiles, 0xE220..0xE232 for palettes.
#define TAG_SCAVENGE_ITEM_TILES_BASE  0xE200
#define TAG_SCAVENGE_ITEM_PAL_BASE    0xE220
#define SCAVENGE_MAX_ICONS            15  // 6 common + 6 uncommon + 3 rare

// Grid layout: icons are 32x32 px. Left page is px 0-119, right page 120-239.
// Common grid (left page, upper): 3 cols x 2 rows starting at (4, 4)
#define SCAVENGE_COMMON_X0    16
#define SCAVENGE_COMMON_Y0    42
// Uncommon grid (left page, lower): 3 cols x 2 rows starting at (4, 80)
#define SCAVENGE_UNCOMMON_X0  130
#define SCAVENGE_UNCOMMON_Y0  42
// Rare grid (right page, below route name): 3 cols x 2 rows starting at (124, 24)
#define SCAVENGE_RARE_X0      16
#define SCAVENGE_RARE_Y0      124
// Horizontal step: 36px (32px icon + 4px gap)
#define SCAVENGE_ICON_STEP_X  36
// Vertical step: 36px
#define SCAVENGE_ICON_STEP_Y  36

//#define TAG_BOOK_PORTRAIT_PAL  6100

static const struct OamData sOamData_Cursor =
{
    .size = SPRITE_SIZE(64x64),
    .shape = SPRITE_SHAPE(64x64),
    .priority = 2,
};

static const struct CompressedSpriteSheet sSpriteSheet_Cursor =
{
    .data = sCursor_Gfx,
    .size = 64*64*4/2,
    .tag = TAG_CURSOR,
};

static const struct SpritePalette sSpritePal_Cursor =
{
    .data = sCursor_Pal,
    .tag = TAG_CURSOR
};

static const union AnimCmd sSpriteAnim_Cursor0[] =
{
    ANIMCMD_FRAME(0, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sSpriteAnimTable_Cursor[] =
{
    sSpriteAnim_Cursor0,
};

static const struct SpriteTemplate sSpriteTemplate_Cursor =
{
    .tileTag = TAG_CURSOR,
    .paletteTag = TAG_CURSOR,
    .oam = &sOamData_Cursor,
    .anims = sSpriteAnimTable_Cursor,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = CursorCallback
};

// Trainer card tab cursor sprite data (64x64 canvas, cursor visual 64x30)
// Shared palette for all three trainer card cursors (uses the main cursor.gbapal).
static const struct SpritePalette sSpritePal_TcCursor =
{
    .data = sCursor_Pal,  // cursor.gbapal shared across all TC cursor images
    .tag = TAG_TC_CURSOR_PAL,
};

// Image array: one entry per icon. StartSpriteAnim(sprite, idx) picks the right image.
static const struct SpriteFrameImage sTcCursorImages[TC_ICON_COUNT] =
{
    { sTcCursor_Pokedex_Gfx, sizeof(sTcCursor_Pokedex_Gfx) },
    { sTcCursor_Bag_Gfx,     sizeof(sTcCursor_Bag_Gfx)     },
    { sTcCursor_Options_Gfx, sizeof(sTcCursor_Options_Gfx) },
};

static const union AnimCmd sSpriteAnim_TcCursor0[] = { ANIMCMD_FRAME(0, 1), ANIMCMD_JUMP(0) };
static const union AnimCmd sSpriteAnim_TcCursor1[] = { ANIMCMD_FRAME(1, 1), ANIMCMD_JUMP(0) };
static const union AnimCmd sSpriteAnim_TcCursor2[] = { ANIMCMD_FRAME(2, 1), ANIMCMD_JUMP(0) };

static const union AnimCmd *const sSpriteAnimTable_TcCursor[] =
{
    sSpriteAnim_TcCursor0,
    sSpriteAnim_TcCursor1,
    sSpriteAnim_TcCursor2,
};

static const struct SpriteTemplate sSpriteTemplate_TcCursor =
{
    .tileTag     = TAG_NONE,           // image-array sprite; no static tile allocation
    .paletteTag  = TAG_TC_CURSOR_PAL,
    .oam         = &sOamData_Cursor,
    .anims       = sSpriteAnimTable_TcCursor,
    .images      = sTcCursorImages,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCallbackDummy,
};

// Switch source indicator palette: same structure as cursor but greens/blues shifted to red,
// so the frozen source cursor looks visually distinct from the moving cursor.
// Cursor palette indices (from cursor.gbapal, 12 colors):
//  [0]=white(transparent bg) [1]=dark-green-gray [2]=light-warm-gray [3]=med-warm-gray
//  [4]=blue-gray [5]=med-gray [6]=dark-red [7]=very-dark-red [8]=black
//  [9]=bright-red [10]=white [11]=dark-salmon
// Red-tinted version: shift green/blue tones to warm red, keep existing reds intact.
static const u16 sSwitchIndicatorPal_Data[16] =
{
    RGB(31, 31, 31),   // [0]  transparent bg (unchanged)
    RGB( 9,  5,  5),   // [1]  dark reddish-gray (was dark green-gray)
    RGB(24, 17, 16),   // [2]  light reddish (was light warm-gray)
    RGB(17, 11, 10),   // [3]  medium reddish (was med warm-gray)
    RGB(16,  9,  8),   // [4]  warm-gray (was blue-gray)
    RGB(14, 10,  9),   // [5]  medium reddish gray (was med-gray)
    RGB(17,  1,  0),   // [6]  dark red (unchanged)
    RGB(10,  0,  0),   // [7]  very dark red (unchanged)
    RGB( 0,  0,  0),   // [8]  black (unchanged)
    RGB(31,  0,  0),   // [9]  bright red (unchanged)
    RGB(31, 31, 31),   // [10] white (unchanged)
    RGB(25,  6,  6),   // [11] dark salmon (unchanged)
    RGB( 0,  0,  0),   // [12] unused
    RGB( 0,  0,  0),   // [13] unused
    RGB( 0,  0,  0),   // [14] unused
    RGB( 0,  0,  0),   // [15] unused
};

// Switch source indicator: same tiles as cursor, frozen (no CursorCallback).
// Palette is loaded directly into OBJ slot SWITCH_INDICATOR_PAL_SLOT to avoid
// competing with the tag system's icon/item palette allocations.
#define SWITCH_INDICATOR_PAL_SLOT 12

static const struct SpriteTemplate sSpriteTemplate_SwitchIndicator =
{
    .tileTag = TAG_CURSOR,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_Cursor,
    .anims = sSpriteAnimTable_Cursor,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// PokéGuide cursor: 24x24 PNG padded to 32x32 at load time (see LoadPokeGuideContent)
static const struct OamData sOamData_PgCursor =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 3,  // behind pokeguide icon grid (priority 2)
};

static const struct SpriteTemplate sSpriteTemplate_PgCursor =
{
    .tileTag    = TAG_PG_CURSOR_TILES,
    .paletteTag = TAG_PG_CURSOR_PAL,
    .oam        = &sOamData_PgCursor,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

// Null slot icon: 16x16, shares pokeguide cursor palette
static const struct OamData sOamData_PgNull =
{
    .size = SPRITE_SIZE(16x16),
    .shape = SPRITE_SHAPE(16x16),
    .priority = 2,
};

static const struct SpriteTemplate sSpriteTemplate_PgNull =
{
    .tileTag    = TAG_PG_NULL_TILES,
    .paletteTag = TAG_PG_CURSOR_PAL,
    .oam        = &sOamData_PgNull,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

// type_unknown: 16x16, shown in place of type stamps for unseen species
static const struct OamData sOamData_PgTypeUnknown =
{
    .size     = SPRITE_SIZE(16x16),
    .shape    = SPRITE_SHAPE(16x16),
    .priority = 2,
};
static const struct SpriteTemplate sSpriteTemplate_PgTypeUnknown =
{
    .tileTag     = TAG_PG_TYPE_UNKNOWN_TILES,
    .paletteTag  = TAG_PG_CURSOR_PAL,
    .oam         = &sOamData_PgTypeUnknown,
    .anims       = gDummySpriteAnimTable,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCallbackDummy,
};

// item_unknown: 32x32, shown in place of item icons for unseen/uncaught species
static const struct OamData sOamData_PgItemUnknown =
{
    .size     = SPRITE_SIZE(32x32),
    .shape    = SPRITE_SHAPE(32x32),
    .priority = 2,
};
static const struct SpriteTemplate sSpriteTemplate_PgItemUnknown =
{
    .tileTag     = TAG_PG_ITEM_UNKNOWN_TILES,
    .paletteTag  = TAG_PG_CURSOR_PAL,
    .oam         = &sOamData_PgItemUnknown,
    .anims       = gDummySpriteAnimTable,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCallbackDummy,
};

// Sepia-brown silhouette palette for unseen species in the pokeguide grid.
// Index 0 = transparent; 1-15 = dark warm sepia brown (R=7, G=5, B=3).
#define PG_SEPIA_BROWN  ((3 << 10) | (5 << 5) | 7)
static const u16 sPgBlackPalData[16] = {
    0x0000,         PG_SEPIA_BROWN, PG_SEPIA_BROWN, PG_SEPIA_BROWN,
    PG_SEPIA_BROWN, PG_SEPIA_BROWN, PG_SEPIA_BROWN, PG_SEPIA_BROWN,
    PG_SEPIA_BROWN, PG_SEPIA_BROWN, PG_SEPIA_BROWN, PG_SEPIA_BROWN,
    PG_SEPIA_BROWN, PG_SEPIA_BROWN, PG_SEPIA_BROWN, PG_SEPIA_BROWN,
};
static const struct SpritePalette sSpritePal_PgBlack = { sPgBlackPalData, TAG_PG_BLACK_PAL };

// pokeguide_caught badge: 16x16, shares cursor palette
static const struct OamData sOamData_PgCaught =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 2,
};
static const struct SpriteTemplate sSpriteTemplate_PgCaught =
{
    .tileTag    = TAG_PG_CAUGHT_TILES,
    .paletteTag = TAG_PG_CURSOR_PAL,
    .oam        = &sOamData_PgCaught,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

// guide_caught_em_all badge: 64x64, shares cursor palette
static const struct OamData sOamData_PgCaughtAll =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 2,
};
static const struct SpriteTemplate sSpriteTemplate_PgCaughtAll =
{
    .tileTag    = TAG_PG_CAUGHT_ALL_TILES,
    .paletteTag = TAG_PG_CURSOR_PAL,
    .oam        = &sOamData_PgCaughtAll,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

// EV yield sprites and search star: all 8x8, share cursor palette
static const struct OamData sOamData_Pg8x8 =
{
    .size = SPRITE_SIZE(8x8),
    .shape = SPRITE_SHAPE(8x8),
    .priority = 2,
};
static const struct SpriteTemplate sSpriteTemplate_PgEvOk =
{
    .tileTag    = TAG_PG_EV_OK_TILES,
    .paletteTag = TAG_PG_CURSOR_PAL,
    .oam        = &sOamData_Pg8x8,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};
static const struct SpriteTemplate sSpriteTemplate_PgEvGood =
{
    .tileTag    = TAG_PG_EV_GOOD_TILES,
    .paletteTag = TAG_PG_CURSOR_PAL,
    .oam        = &sOamData_Pg8x8,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};
static const struct SpriteTemplate sSpriteTemplate_PgEvGreat =
{
    .tileTag    = TAG_PG_EV_GREAT_TILES,
    .paletteTag = TAG_PG_CURSOR_PAL,
    .oam        = &sOamData_Pg8x8,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};
static const struct SpriteTemplate sSpriteTemplate_PgStar =
{
    .tileTag    = TAG_PG_STAR_TILES,
    .paletteTag = TAG_PG_CURSOR_PAL,
    .oam        = &sOamData_Pg8x8,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

// friendship heart sprite (16x16, shares cursor palette, image-based so no tile tag needed)
static const struct SpriteFrameImage sHeartImages[] =
{
    { sHeart0_Gfx,    128 },  // 0: heart_0
    { sHeart25_Gfx,   128 },  // 1: heart_25
    { sHeart50_Gfx,   128 },  // 2: heart_50
    { sHeart75_Gfx,   128 },  // 3: heart_75
    { sHeart100_Gfx,  128 },  // 4: heart_100
    { sHeartPerf_Gfx,  128 },  // 5: heart_perf
    { sHeartPerf2_Gfx, 128 },  // 6: heart_perf2
    { sHeartPerf3_Gfx, 128 },  // 7: heart_perf3
};

static const struct OamData sOamData_Heart =
{
    .size = SPRITE_SIZE(16x16),
    .shape = SPRITE_SHAPE(16x16),
    .priority = 2,
};

static const union AnimCmd sHeartAnim_0[] = { ANIMCMD_FRAME(0, 1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHeartAnim_1[] = { ANIMCMD_FRAME(1, 1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHeartAnim_2[] = { ANIMCMD_FRAME(2, 1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHeartAnim_3[] = { ANIMCMD_FRAME(3, 1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHeartAnim_4[] = { ANIMCMD_FRAME(4, 1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHeartAnim_5[] = {
    ANIMCMD_FRAME(5, 12),   // perf (full sparkle)
    ANIMCMD_FRAME(6, 8),    // perf2 (medium)
    ANIMCMD_FRAME(7, 6),    // perf3 (smallest)
    ANIMCMD_FRAME(6, 8),    // perf2 (back up)
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sHeartAnimTable[] =
{
    sHeartAnim_0,
    sHeartAnim_1,
    sHeartAnim_2,
    sHeartAnim_3,
    sHeartAnim_4,
    sHeartAnim_5,
};

static const struct SpriteTemplate sSpriteTemplate_Heart =
{
    .tileTag = TAG_NONE,
    .paletteTag = TAG_CURSOR,
    .oam = &sOamData_Heart,
    .anims = sHeartAnimTable,
    .images = sHeartImages,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

#define HEART_CENTER_X 110
#define HEART_CENTER_Y 64

// HP bar sprite data (32x32, shares cursor palette)
static const struct SpriteFrameImage sHPBarImages[] =
{
    { sHPBar0_Gfx,   512 },  //  0: fainted
    { sHPBar1_Gfx,   512 },  //  1: <10%
    { sHPBar10_Gfx,  512 },  //  2: >=10%
    { sHPBar20_Gfx,  512 },  //  3: >=20%
    { sHPBar30_Gfx,  512 },  //  4: >=30%
    { sHPBar40_Gfx,  512 },  //  5: >=40%
    { sHPBar50_Gfx,  512 },  //  6: >=50%
    { sHPBar60_Gfx,  512 },  //  7: >=60%
    { sHPBar70_Gfx,  512 },  //  8: >=70%
    { sHPBar80_Gfx,  512 },  //  9: >=80%
    { sHPBar90_Gfx,  512 },  // 10: >=90%
    { sHPBar100_Gfx, 512 },  // 11: 100%
};

static const struct OamData sOamData_HPBar =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 2,
};

static const union AnimCmd sHPBarAnim_0[]  = { ANIMCMD_FRAME(0,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_1[]  = { ANIMCMD_FRAME(1,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_2[]  = { ANIMCMD_FRAME(2,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_3[]  = { ANIMCMD_FRAME(3,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_4[]  = { ANIMCMD_FRAME(4,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_5[]  = { ANIMCMD_FRAME(5,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_6[]  = { ANIMCMD_FRAME(6,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_7[]  = { ANIMCMD_FRAME(7,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_8[]  = { ANIMCMD_FRAME(8,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_9[]  = { ANIMCMD_FRAME(9,  1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_10[] = { ANIMCMD_FRAME(10, 1), ANIMCMD_JUMP(0) };
static const union AnimCmd sHPBarAnim_11[] = { ANIMCMD_FRAME(11, 1), ANIMCMD_JUMP(0) };

static const union AnimCmd *const sHPBarAnimTable[] =
{
    sHPBarAnim_0,  sHPBarAnim_1,  sHPBarAnim_2,  sHPBarAnim_3,
    sHPBarAnim_4,  sHPBarAnim_5,  sHPBarAnim_6,  sHPBarAnim_7,
    sHPBarAnim_8,  sHPBarAnim_9,  sHPBarAnim_10, sHPBarAnim_11,
};

static const struct SpriteTemplate sSpriteTemplate_HPBar =
{
    .tileTag      = TAG_NONE,
    .paletteTag   = TAG_CURSOR,
    .oam          = &sOamData_HPBar,
    .anims        = sHPBarAnimTable,
    .images       = sHPBarImages,
    .affineAnims  = gDummySpriteAffineAnimTable,
    .callback     = SpriteCallbackDummy,
};

// Badge sprites for trainer card tab
// Each badge is 16x16px = 4 tiles. 8 badges total in the .4bpp sheet.
#define BADGE_TILE_SIZE  (16*16/2)  // bytes per 16x16 4bpp badge
#define BADGE_TILES_PER  4          // 8x8 tiles per 16x16 badge

static const struct OamData sOamData_Badge =
{
    .y          = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode    = ST_OAM_OBJ_NORMAL,
    .mosaic     = FALSE,
    .bpp        = ST_OAM_4BPP,
    .shape      = SPRITE_SHAPE(16x16),
    .x          = 0,
    .matrixNum  = 0,
    .size       = SPRITE_SIZE(16x16),
    .tileNum    = 0,
    .priority   = 2,
    .paletteNum = 0,
};

static const union AnimCmd sSpriteAnim_Badge0[] = { ANIMCMD_FRAME(0,  1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_Badge1[] = { ANIMCMD_FRAME(4,  1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_Badge2[] = { ANIMCMD_FRAME(8,  1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_Badge3[] = { ANIMCMD_FRAME(12, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_Badge4[] = { ANIMCMD_FRAME(16, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_Badge5[] = { ANIMCMD_FRAME(20, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_Badge6[] = { ANIMCMD_FRAME(24, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_Badge7[] = { ANIMCMD_FRAME(28, 1, FALSE, FALSE), ANIMCMD_END };

static const union AnimCmd *const sSpriteAnimTable_Badge[] =
{
    sSpriteAnim_Badge0, sSpriteAnim_Badge1, sSpriteAnim_Badge2, sSpriteAnim_Badge3,
    sSpriteAnim_Badge4, sSpriteAnim_Badge5, sSpriteAnim_Badge6, sSpriteAnim_Badge7,
};

static const struct SpritePalette sSpritePalette_Badges =
{
    .data = sBadgePal,
    .tag  = TAG_BADGES_PAL,
};

static const struct SpriteTemplate sSpriteTemplate_Badge =
{
    .tileTag     = TAG_BADGES_TILES,
    .paletteTag  = TAG_BADGES_PAL,
    .oam         = &sOamData_Badge,
    .anims       = sSpriteAnimTable_Badge,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCallbackDummy,
};

// Badge screen positions: 2 rows of 4, centered in left page (x 0-119)
// Row 0 y=118, Row 1 y=138; x columns at 14, 34, 54, 74 (16px badge + 4px gap)
static const u8 sBadgeX[NUM_BADGES] = { 24, 48, 72, 96,  24, 48, 72, 96};
static const u8 sBadgeY[NUM_BADGES] = { 127, 127,  127, 127,  146, 146, 146, 146};

//
//  Begin Sprite Loading Functions25
//

//
//      Cursor Creation and Callback
//
#define CURSOR_LEFT_COL_X 124+32
#define CURSOR_RIGHT_COL_X 176+32
#define CURSOR_TOP_ROW_Y 19+32
#define CURSOR_MID_ROW_Y 19 + 43+32
#define CURSOR_BTM_ROW_Y 19 + 43*2+32

static void CreateCursor()
{
    if (sStartMenuDataPtr->cursorSpriteId == SPRITE_NONE)
        sStartMenuDataPtr->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, CURSOR_LEFT_COL_X, CURSOR_TOP_ROW_Y, 0);

    CursorCallback(&gSprites[sStartMenuDataPtr->cursorSpriteId]);

    gSprites[sStartMenuDataPtr->cursorSpriteId].invisible = FALSE;
    StartSpriteAnim(&gSprites[sStartMenuDataPtr->cursorSpriteId], 0);
    gSprites[sStartMenuDataPtr->cursorSpriteId].oam.priority = 3;
    gSprites[sStartMenuDataPtr->cursorSpriteId].subpriority = 1; // <--- For some reason keeps curosr alive
    return;
}

static void DestroyCursor()
{
    if (sStartMenuDataPtr->cursorSpriteId != SPRITE_NONE)
        DestroySprite(&gSprites[sStartMenuDataPtr->cursorSpriteId]);
    sStartMenuDataPtr->cursorSpriteId = SPRITE_NONE;
}

struct SpriteCordsStruct {
    u8 x;
    u8 y;
};

static void CursorCallback(struct Sprite *sprite) // Sprite callback for the cursor that updates the position every frame when the input control code updates
{
    struct SpriteCordsStruct spriteCords[3][2] = {
        {{CURSOR_LEFT_COL_X, CURSOR_TOP_ROW_Y}, {CURSOR_RIGHT_COL_X, CURSOR_TOP_ROW_Y}},
        {{CURSOR_LEFT_COL_X, CURSOR_MID_ROW_Y}, {CURSOR_RIGHT_COL_X, CURSOR_MID_ROW_Y}},
        {{CURSOR_LEFT_COL_X, CURSOR_BTM_ROW_Y}, {CURSOR_RIGHT_COL_X, CURSOR_BTM_ROW_Y}},
    };

    gSelectedMenu = sStartMenuDataPtr->selector_x + (sStartMenuDataPtr->selector_y * 2);

    sprite->x = spriteCords[sStartMenuDataPtr->selector_y][sStartMenuDataPtr->selector_x].x;
    sprite->y = spriteCords[sStartMenuDataPtr->selector_y][sStartMenuDataPtr->selector_x].y;


}

static void InitCursorInPlace()
{
    if(gSelectedMenu % 2)
        sStartMenuDataPtr->selector_x = 1;
    else
        sStartMenuDataPtr->selector_x = 0;

    if(gSelectedMenu <= 1)
        sStartMenuDataPtr->selector_y = 0;
    else if (gSelectedMenu > 1 && gSelectedMenu <= 3)
        sStartMenuDataPtr->selector_y = 1;
    else
        sStartMenuDataPtr->selector_y = 2;
}



//==========FUNCTIONS==========//
// These next few functions are from the Ghoulslash UI Shell, they are the basic functions to init a brand new UI
void Task_OpenStartMenuBook(u8 taskId)
{
    //s16 *data = gTasks[taskId].data;
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        StartBookMenu(CB2_ReturnToField);
        DestroyTask(taskId);
    }
}


void StartBookMenuOnTab(MainCallback callback, u8 openTab)
{
    if ((sStartMenuDataPtr = AllocZeroed(sizeof(struct StartMenuResources))) == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    gMain.state = 0;
    sStartMenuDataPtr->gfxLoadState = 0;
    sStartMenuDataPtr->savedCallback = callback;
    sStartMenuDataPtr->cursorSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->portraitSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->heldItemSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->heartSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->lastHeartFrame = 0xFF;
    sStartMenuDataPtr->lastSelectedMenu = 0xFF;
    for (u8 i = 0; i < PARTY_SIZE; i++)
        sStartMenuDataPtr->hpBarSpriteIds[i] = SPRITE_NONE;
    sStartMenuDataPtr->typeIconSpriteIds[0] = SPRITE_NONE;
    sStartMenuDataPtr->typeIconSpriteIds[1] = SPRITE_NONE;
    sStartMenuDataPtr->currentTab = (openTab < BOOK_TAB_COUNT) ? openTab : BOOK_TAB_PARTY;
    sStartMenuDataPtr->isOnWater = (bool8)TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING);
    sStartMenuDataPtr->pokeguideIconCount = 0;
    sStartMenuDataPtr->sharedMatrix = 0xFF;
    for (u8 i = 0; i < 20; i++)
        sStartMenuDataPtr->pokeguideIconSpriteIds[i] = SPRITE_NONE;
    sStartMenuDataPtr->scavengeIconCount = 0;
    for (u8 i = 0; i < SCAVENGE_MAX_ICONS; i++)
        sStartMenuDataPtr->scavengeIconSpriteIds[i] = SPRITE_NONE;
    for (u8 i = 0; i < NUM_BADGES; i++)
        sStartMenuDataPtr->badgeSpriteIds[i] = SPRITE_NONE;
    sFollowerMsgWindowId    = SPRITE_NONE;
    sPgRegisterMsgWindowId  = SPRITE_NONE;

    InitCursorInPlace();

    gFieldCallback = NULL;

    SetMainCallback2(StartMenuBook_RunSetup);
}

void StartBookMenu(MainCallback callback)
{
    StartBookMenuOnTab(callback, BOOK_TAB_PARTY);
}

static void StartMenuBook_RunSetup(void)
{
    while (1)
    {
        if (StartMenuBook_DoGfxSetup() == TRUE)
            break;
    }
}

static void StartMenuBook_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void StartMenuBook_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static bool8 StartMenuBook_DoGfxSetup(void)
{
    switch (gMain.state)
    {
    case 0:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000)
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        ResetVramOamAndBgCntRegs();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        // Reset script context so any completed field-move script (e.g. Cut) cannot be
        // accidentally re-triggered when the debug menu calls ScriptContext_Enable on close.
        ScriptContext_Init();
        gMain.state++;
        break;
    case 2:
        if (StartMenuBook_InitBgs())
        {
            sStartMenuDataPtr->gfxLoadState = 0;
            gMain.state++;
        }
        else
        {
            StartMenuBook_FadeAndBail();
            return TRUE;
        }
        break;
    case 3:
        if (StartMenuBook_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 4:
        StartMenuBook_InitWindows();
        gMain.state++;
        break;
    case 5:
        InitialTabLoad();
        gMain.state++;
        break;
    case 6:
        CreateTask(Task_StartMenuBookWaitFadeIn, 0);
        gMain.state++;
        break;
    case 7:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(StartMenuBook_VBlankCB);
        SetMainCallback2(StartMenuBook_MainCB);
        return TRUE;
    }
    return FALSE;
}

#define try_free(ptr) ({        \
    void ** ptr__ = (void **)&(ptr);   \
    if (*ptr__ != NULL)                \
        Free(*ptr__);                  \
})

static void StartMenuBook_FreeResources(void) // Clear Everything if Leaving
{
    // Unload scavenge/minerals item icon sprites before the struct is freed.
    // Minerals and scavenger both store sprites in scavengeIconSpriteIds and use
    // TAG_SCAVENGE_ITEM_* tags that must be freed by tag for the allocator to stay clean.
    // UnloadMineralsContent is idempotent (scavengeIconCount = 0 guard).
    UnloadMineralsContent();
    UnloadPokeGuideContent();

    // Clean up any open pokeguide confirmation window before destroying all windows
    if (sPgRegisterMsgWindowId != SPRITE_NONE)
    {
        ClearWindowTilemap(sPgRegisterMsgWindowId);
        RemoveWindow(sPgRegisterMsgWindowId);
        sPgRegisterMsgWindowId = SPRITE_NONE;
        PgSetRightPageSpritesBehindBg(FALSE);
    }
    DestroyCursor();
    DestroyPartyMonIcons();
    DestroyBookPortraitSprite();
    DestroyBookHeldItemIcon();
    DestroyBookHeartIcon();
    DestroyPartyHPBars();
    DestroyTypeIconSprites();
    StopCryAndClearCrySongs();
    FreeMonIconPalettes();
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeAllWindowBuffers();
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    // Cancel any BG tilemap copies that were scheduled during cleanup above.
    // They would run in DoScheduledBgTilemapCopiesToVram() AFTER this function returns,
    // but by then the window buffers are freed (NULL source = bad DMA / freeze).
    ClearScheduledBgCopiesToVram();
    // Free heap allocations last, after all code that reads from them.
    try_free(sBg1TilemapBuffer);
    try_free(sBg2TilemapBuffer);
    try_free(sStartMenuDataPtr);
    sStartMenuDataPtr = NULL;
}

static void Task_StartMenuBookWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sStartMenuDataPtr->savedCallback);
        StartMenuBook_FreeResources();
        DestroyTask(taskId);
    }
}
static void StartMenuBook_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_StartMenuBookWaitFadeAndBail, 0);
    SetVBlankCallback(StartMenuBook_VBlankCB);
    SetMainCallback2(StartMenuBook_MainCB);
}


static bool8 StartMenuBook_InitBgs(void) // This function sets the bg tilemap buffers for each bg and initializes them, shows them, and turns sprites on
{
    ResetAllBgsCoordinates();
    sBg1TilemapBuffer = Alloc(0x800);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;
    memset(sBg1TilemapBuffer, 0, 0x800);

    sBg2TilemapBuffer = Alloc(0x800);
    if (sBg2TilemapBuffer == NULL)
        return FALSE;
    memset(sBg2TilemapBuffer, 0, 0x800);
    
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBookBgTemplates, NELEMS(sBookBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    SetBgTilemapBuffer(2, sBg2TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);
    ScheduleBgCopyTilemapToVram(2);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    // Blend semi-transparent OBJ (type stamps) with whatever BG lies beneath.
    // BLDCNT_TGT1_OBJ is intentionally omitted: sprites with objMode=ST_OAM_OBJ_BLEND
    // (type stamps) act as first target automatically. Including TGT1_OBJ would make
    // ALL sprites blend, not just the stamps.
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG_ALL);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(TYPE_STAMP_BLEND_EVA, TYPE_STAMP_BLEND_EVB));
    ShowBg(0);
    ShowBg(1);
    HideBg(2); // BG2 is only shown during the flip animation
    return TRUE;
}

// --- Type stamp sprite system (book menu only) ---
// 18 ink stamp sprites, one per standard type, 16x16, sharing a single palette.
// Rendered with ST_OAM_AFFINE_DOUBLE so 45-degree rotation does not clip.
// TYPE_NONE / TYPE_MYSTERY / TYPE_STELLAR have no stamp; they fall back to stamp 0 (Normal).
// 18 stamps concatenated in type order (normal=0 .. fairy=17) by Makefile rule.
static const u32 sTypeStamps_Gfx[] = INCBIN_U32("graphics/type_stamps/type_stamps_all.4bpp");
static const u16 sTypeStamps_Pal[] = INCBIN_U16("graphics/type_stamps/type_stamp.gbapal");

static const struct OamData sOamData_TypeStamps =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .objMode = ST_OAM_OBJ_BLEND,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 1,
};

// Maps every TYPE_* constant to a stamp animation index (0 = normal, 17 = fairy).
static const u8 sTypeStampAnim[NUMBER_OF_MON_TYPES] =
{
    [TYPE_NONE]     = 0,
    [TYPE_NORMAL]   = 0,
    [TYPE_FIGHTING] = 1,
    [TYPE_FLYING]   = 2,
    [TYPE_POISON]   = 3,
    [TYPE_GROUND]   = 4,
    [TYPE_ROCK]     = 5,
    [TYPE_BUG]      = 6,
    [TYPE_GHOST]    = 7,
    [TYPE_STEEL]    = 8,
    [TYPE_MYSTERY]  = 0,
    [TYPE_FIRE]     = 9,
    [TYPE_WATER]    = 10,
    [TYPE_GRASS]    = 11,
    [TYPE_ELECTRIC] = 12,
    [TYPE_PSYCHIC]  = 13,
    [TYPE_ICE]      = 14,
    [TYPE_DRAGON]   = 15,
    [TYPE_DARK]     = 16,
    [TYPE_FAIRY]    = 17,
    [TYPE_STELLAR]  = 0,
};

// Each 16x16 stamp = 4 tiles; frame tile offset = stampIndex * 4.
static const union AnimCmd sSpriteAnim_TypeStamp0[]  = { ANIMCMD_FRAME(0,  1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp1[]  = { ANIMCMD_FRAME(4,  1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp2[]  = { ANIMCMD_FRAME(8,  1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp3[]  = { ANIMCMD_FRAME(12, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp4[]  = { ANIMCMD_FRAME(16, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp5[]  = { ANIMCMD_FRAME(20, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp6[]  = { ANIMCMD_FRAME(24, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp7[]  = { ANIMCMD_FRAME(28, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp8[]  = { ANIMCMD_FRAME(32, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp9[]  = { ANIMCMD_FRAME(36, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp10[] = { ANIMCMD_FRAME(40, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp11[] = { ANIMCMD_FRAME(44, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp12[] = { ANIMCMD_FRAME(48, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp13[] = { ANIMCMD_FRAME(52, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp14[] = { ANIMCMD_FRAME(56, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp15[] = { ANIMCMD_FRAME(60, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp16[] = { ANIMCMD_FRAME(64, 1, FALSE, FALSE), ANIMCMD_END };
static const union AnimCmd sSpriteAnim_TypeStamp17[] = { ANIMCMD_FRAME(68, 1, FALSE, FALSE), ANIMCMD_END };

static const union AnimCmd *const sSpriteAnimTable_TypeStamps[] =
{
    sSpriteAnim_TypeStamp0,  sSpriteAnim_TypeStamp1,  sSpriteAnim_TypeStamp2,
    sSpriteAnim_TypeStamp3,  sSpriteAnim_TypeStamp4,  sSpriteAnim_TypeStamp5,
    sSpriteAnim_TypeStamp6,  sSpriteAnim_TypeStamp7,  sSpriteAnim_TypeStamp8,
    sSpriteAnim_TypeStamp9,  sSpriteAnim_TypeStamp10, sSpriteAnim_TypeStamp11,
    sSpriteAnim_TypeStamp12, sSpriteAnim_TypeStamp13, sSpriteAnim_TypeStamp14,
    sSpriteAnim_TypeStamp15, sSpriteAnim_TypeStamp16, sSpriteAnim_TypeStamp17,
};

static const struct SpriteSheet sTypeStampsSpriteSheet =
{
    .data = sTypeStamps_Gfx,
    .size = 18 * 4 * 32,  // 18 stamps * 4 tiles * 32 bytes per 4bpp tile
    .tag  = TAG_TYPE_STAMPS_TILES,
};

static const struct SpritePalette sTypeStampsSpritePalette =
{
    .data = sTypeStamps_Pal,
    .tag  = TAG_TYPE_STAMPS_PAL,
};

static const struct SpriteTemplate sSpriteTemplate_TypeStamps =
{
    .tileTag     = TAG_TYPE_STAMPS_TILES,
    .paletteTag  = TAG_TYPE_STAMPS_PAL,
    .oam         = &sOamData_TypeStamps,
    .anims       = sSpriteAnimTable_TypeStamps,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCallbackDummy,
};

// Pokeguide page variant: no affine (stamps displayed at natural 16x16, no scaling or rotation).
// Shares the same tiles/palette/anims; only the OamData differs.
static const struct OamData sOamData_TypeStamps_Pg =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode    = ST_OAM_OBJ_BLEND,
    .bpp        = ST_OAM_4BPP,
    .shape      = SPRITE_SHAPE(16x16),
    .size       = SPRITE_SIZE(16x16),
    .priority   = 2,
};
static const struct SpriteTemplate sSpriteTemplate_TypeStamps_Pg =
{
    .tileTag     = TAG_TYPE_STAMPS_TILES,
    .paletteTag  = TAG_TYPE_STAMPS_PAL,
    .oam         = &sOamData_TypeStamps_Pg,
    .anims       = sSpriteAnimTable_TypeStamps,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCallbackDummy,
};

static bool8 StartMenuBook_LoadGraphics(void) // Load the Tilesets, Tilemaps, Spritesheets, and Palettes for Everything
{
    switch (sStartMenuDataPtr->gfxLoadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, BookTab_GetTiles(sStartMenuDataPtr->currentTab), 0, 0, 0);
        sStartMenuDataPtr->gfxLoadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(BookTab_GetTilemap(sStartMenuDataPtr->currentTab), sBg1TilemapBuffer);
            sStartMenuDataPtr->gfxLoadState++;
        }
        break;
    case 2:
        LoadPalette(BookTab_GetPalette(sStartMenuDataPtr->currentTab), BG_PLTT_ID(0), PLTT_SIZE_4BPP); // initial page art
        LoadPalette(sBookPalette_Text,  BG_PLTT_ID(1), PLTT_SIZE_4BPP); // text/ink, fixed
        sStartMenuDataPtr->gfxLoadState++;
        break;
    default:
        sStartMenuDataPtr->gfxLoadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void StartMenuBook_InitWindows(void)
{
    InitWindows(sBookWindows);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);


    //Left Pokemon Summary Screen Windows
    FillWindowPixelBuffer(WIN_LEFT_NAME, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WIN_LEFT_NAME);
    CopyWindowToVram(WIN_LEFT_NAME, COPYWIN_FULL);


    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WIN_LEFT_MOVES_ALL);
    CopyWindowToVram(WIN_LEFT_MOVES_ALL, COPYWIN_FULL);

    FillWindowPixelBuffer(WIN_RIGHT_LVS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WIN_RIGHT_LVS);
    CopyWindowToVram(WIN_RIGHT_LVS, COPYWIN_FULL);

    ScheduleBgCopyTilemapToVram(2);
}


static void Task_StartMenuBookWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        sStartMenuDataPtr->lastSelectedMenu = 0xFF; // force refresh
        if (sPendingGiveItemId != ITEM_NONE)
        {
            sPendingGiveItemId = ITEM_NONE;
            CreateBookPortraitSprite(gSelectedMenu);
            sStartMenuDataPtr->lastSelectedMenu = gSelectedMenu;
            ShowFollowerMsgBox();
            gTasks[taskId].func = Task_BookMenu_ShowFollowerMsg;
        }
        else
        {
            gTasks[taskId].func = Task_StartMenuBookMain;
        }
    }
}

static void Task_StartMenuBookTurnOff(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sStartMenuDataPtr->savedCallback);
        StartMenuBook_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_BookMenuGiveItem(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        GoToBagMenu(ITEMMENULOCATION_PARTY, POCKETS_COUNT, CB2_BookMenuGiveHoldItem);
        DestroyTask(taskId);
    }
}

static void CB2_BookMenuGiveHoldItem(void)
{
    u8 slot = (u8)gPartyMenu.slotId;
    if (gSpecialVar_ItemId != ITEM_NONE)
    {
        u16 newItem = gSpecialVar_ItemId;
        u16 oldItem = GetMonData(&gPlayerParty[slot], MON_DATA_HELD_ITEM);
        u8 nickStr[POKEMON_NAME_LENGTH + 1];
        if (oldItem != ITEM_NONE)
            AddBagItem(oldItem, 1);
        SetMonData(&gPlayerParty[slot], MON_DATA_HELD_ITEM, &newItem);
        RemoveBagItem(newItem, 1);
        GetMonData(&gPlayerParty[slot], MON_DATA_NICKNAME, nickStr);
        StringCopy(sFollowerMsgStr, nickStr);
        StringAppend(sFollowerMsgStr, sBookText_GotItem);
        CopyItemName(newItem, sFollowerMsgStr + StringLength(sFollowerMsgStr));
        StringAppend(sFollowerMsgStr, sBookText_ExclaimEnd);
        sPendingGiveItemId = newItem;
    }
    else
    {
        sPendingGiveItemId = ITEM_NONE;
    }
    CleanupOverworldWindowsAndTilemaps();
    StartBookMenu(CB2_ReturnToField);
}

void Task_OpenPokedexFromStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        IncrementGameStat(GAME_STAT_CHECKED_POKEDEX);
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_OpenPokedex);
        DestroyTask(taskId);
    }
}

void Task_OpenPokemonPartyFromStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PartyMenuFromStartMenu);
    }
}

void Task_OpenBagFromStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_BagMenuFromStartMenu);
    }
}

void Task_OpenTrainerCardFromStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000) // cleared vram because for some reason I was having issues with this
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();

        if (FlagGet(FLAG_SYS_FRONTIER_PASS))
            ShowFrontierPass(CB2_ReturnToFieldWithOpenMenu);
        else
            ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu);
    }
}

void Task_OpenPokenavStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
		PlayRainStoppingSoundEffect();
		CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitPokeNav);
    }
}

void Task_OpenOptionsMenuStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitOptionMenu); 
        gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;
    }
}

void Task_ReturnToFieldOnSave(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_ReturnToField); 
    }
}

//
//  Handle save Confirmation and then Leave to Overworld for Saving
//
# define sFrameToSecondTimer data[6]
void Task_HandleSaveConfirmation(u8 taskId)
{
    /*if(JOY_NEW(A_BUTTON)) //confirm and leave
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_ReturnToFieldOnSave;
        gFieldCallback = SaveStartCallback_FullStartMenu;
        return;
    }
    if(JOY_NEW(B_BUTTON)) // back to normal Menu Control
    {
        PlaySE(SE_SELECT);
        //FillWindowPixelBuffer(WINDOW_BOTTOM_BAR, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
        //PutWindowTilemap(WINDOW_BOTTOM_BAR);
        //CopyWindowToVram(WINDOW_BOTTOM_BAR, COPYWIN_FULL);
        gTasks[taskId].func = Task_StartMenuFullMain;
        return;
    }
    if(gTasks[taskId].sFrameToSecondTimer >= 60) // every 60 frames update the time
    {
        PrintMapNameAndTime();
        gTasks[taskId].sFrameToSecondTimer = 0;
    }
    gTasks[taskId].sFrameToSecondTimer++;*/
}

/*static void PrintSaveConfirmToWindow()
{
    *//*const u8 *str = sText_ConfirmSave;
    u8 sConfirmTextColors[] = {TEXT_COLOR_TRANSPARENT, 2, 3};
    u8 x = 24;
    u8 y = 0;
*//*
    //FillWindowPixelBuffer(WINDOW_BOTTOM_BAR, PIXEL_FILL(5));
    //BlitBitmapToWindow(WINDOW_BOTTOM_BAR, sA_ButtonGfx, 12, 5, 8, 8);
    //AddTextPrinterParameterized4(WINDOW_BOTTOM_BAR, 1, x, y, 0, 0, sConfirmTextColors, 0xFF, str);
    //PutWindowTilemap(WINDOW_BOTTOM_BAR);
    //CopyWindowToVram(WINDOW_BOTTOM_BAR, COPYWIN_FULL);
}*/


//
//  Main Control Function, Grid UI Control
//
static void Task_StartMenuBookMain(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_BRIDGE_WALK);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_StartMenuBookTurnOff;
    }
    if (sStartMenuDataPtr->currentTab == BOOK_TAB_PARTY)
    {
        if (JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT)) // these change the position of the selector, the actual x/y of the sprite is handled in its callback CursorCallback
        {
            if(sStartMenuDataPtr->selector_x == 0)
                sStartMenuDataPtr->selector_x = 1;
            else
                sStartMenuDataPtr->selector_x = 0;
        }
        if (JOY_NEW(DPAD_UP))
        {
            if (sStartMenuDataPtr->selector_y == 0)
                sStartMenuDataPtr->selector_y = 2;
            else
                sStartMenuDataPtr->selector_y--;
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            if (sStartMenuDataPtr->selector_y == 2)
                sStartMenuDataPtr->selector_y = 0;
            else
                sStartMenuDataPtr->selector_y++;
        }
        u8 selected = sStartMenuDataPtr->selector_x + (sStartMenuDataPtr->selector_y * 2);

        if (selected != sStartMenuDataPtr->lastSelectedMenu)
        {
            sStartMenuDataPtr->lastSelectedMenu = selected;
            DisplaySelectedPokemonDetails(selected, TRUE);
            CreateBookPortraitSprite(selected);
        }

        if (JOY_NEW(A_BUTTON))
        {
            u8 selected = sStartMenuDataPtr->selector_x + (sStartMenuDataPtr->selector_y * 2);
            if (selected < gPlayerPartyCount)
            {
                PlaySE(SE_SELECT);
                BookMenu_SetRightPageSpritePriority(1);
                {
                    bool8 isFollower = IsDesignatedFollower(selected);
                    struct WindowTemplate dynTemplate;
                    struct ComfyAnimEasingConfig animCfg;
                    u8 wid;
                    // Build the action list first so heightTiles is known before AddWindow.
                    BuildBookActionList(selected);
                    // Create window with exactly the needed height so PutWindowTilemap only
                    // places the used tile rows. Prevents BG tilemap wrap artifact at screen top.
                    dynTemplate = sBookActionMenuTemplate;
                    dynTemplate.height = sActionMenuHeightPx / 8;
                    wid = AddWindow(&dynTemplate);
                    gTasks[taskId].tActionWindowId = wid;
                    gTasks[taskId].tActionCursor = 0;
                    gTasks[taskId].tIsFollower = isFollower;
                    SetWindowAttribute(wid, WINDOW_TILEMAP_TOP, sActionMenuTilemapTopStart);
                    DrawBookActionMenu(wid, 0, isFollower);
                    InitComfyAnimConfig_Easing(&animCfg);
                    animCfg.from = Q_24_8((u32)sActionMenuTilemapTopStart);
                    animCfg.to   = Q_24_8((u32)sActionMenuTilemapTopFinal);
                    animCfg.durationFrames = 10;
                    animCfg.easingFunc = ComfyAnimEasing_EaseOutCubic;
                    InitComfyAnim_Easing(&animCfg, &sActionMenuSlideAnim);
                }
                gTasks[taskId].func = Task_BookMenu_ActionMenuOpenAnim;
            }
        } // end A_BUTTON

    } // end BOOK_TAB_PARTY gate

    if (JOY_NEW(START_BUTTON))
    {
        static const u8 sText_Saving[] = _("Saving");
        PlaySE(SE_SELECT);
        StringCopy(sFollowerMsgStr, sText_Saving);
        ShowFollowerMsgBox();
        gTasks[taskId].data[6] = 0; // frame timer
        gTasks[taskId].data[7] = 0; // dot state
        gTasks[taskId].func = Task_BookMenu_SaveAnim;
    }

    if (sStartMenuDataPtr->currentTab == BOOK_TAB_TRAINER)
    {
        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            sStartMenuDataPtr->selector_y = (sStartMenuDataPtr->selector_y == 0)
                ? TC_ICON_COUNT - 1 : sStartMenuDataPtr->selector_y - 1;
            if (sStartMenuDataPtr->cursorSpriteId < MAX_SPRITES)
            {
                gSprites[sStartMenuDataPtr->cursorSpriteId].y =
                    TC_ICON_0_Y + sStartMenuDataPtr->selector_y * TC_ICON_STRIDE;
                StartSpriteAnim(&gSprites[sStartMenuDataPtr->cursorSpriteId],
                                sStartMenuDataPtr->selector_y);
            }
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            sStartMenuDataPtr->selector_y = (sStartMenuDataPtr->selector_y >= TC_ICON_COUNT - 1)
                ? 0 : sStartMenuDataPtr->selector_y + 1;
            if (sStartMenuDataPtr->cursorSpriteId < MAX_SPRITES)
            {
                gSprites[sStartMenuDataPtr->cursorSpriteId].y =
                    TC_ICON_0_Y + sStartMenuDataPtr->selector_y * TC_ICON_STRIDE;
                StartSpriteAnim(&gSprites[sStartMenuDataPtr->cursorSpriteId],
                                sStartMenuDataPtr->selector_y);
            }
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            switch (sStartMenuDataPtr->selector_y)
            {
            case 0: gTasks[taskId].func = Task_BookMenu_TcOpenPokedex; break;
            case 1: gTasks[taskId].func = Task_BookMenu_TcOpenBag;     break;
            case 2: gTasks[taskId].func = Task_BookMenu_TcOpenOptions; break;
            }
        }
    }

    if (sStartMenuDataPtr->currentTab == BOOK_TAB_MINERALS)
    {
        if (JOY_NEW(A_BUTTON))
        {
            if (!TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_ON_FOOT))
            {
                static const u8 sText_CantMineNow[] = _("Can't mine right now.");
                PlaySE(SE_FAILURE);
                StringCopy(sFollowerMsgStr, sText_CantMineNow);
                ShowFollowerMsgBox();
                gTasks[taskId].func = Task_BookMenu_SaveMsg;
            }
            else if (sStartMenuDataPtr->scavengeIconCount == 0)
            {
                static const u8 sText_NothingToMine[] = _("There is nothing\nto mine here.");
                PlaySE(SE_FAILURE);
                StringCopy(sFollowerMsgStr, sText_NothingToMine);
                ShowFollowerMsgBox();
                gTasks[taskId].func = Task_BookMenu_SaveMsg;
            }
            else
            {
                PlaySE(SE_SELECT);
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gTasks[taskId].func = Task_BookMenu_MiningExit;
            }
        }
    }

    if (sStartMenuDataPtr->currentTab == BOOK_TAB_POKEGUIDE && sPokeguideTotal > 0)
    {
        u8 cols = 3;
        u8 rows = (sPokeguideTotal + cols - 1) / cols;   // actual row count
        u8 col = sPgCursorIdx % cols;
        u8 row = sPgCursorIdx / cols;
        u8 maxCount = sStartMenuDataPtr->isOnWater ? POKEGUIDE_MAX_WATER : POKEGUIDE_MAX_LAND;
        bool8 moved = FALSE;

        if (JOY_NEW(DPAD_RIGHT))
        {
            u8 newIdx = sPgCursorIdx + 1;
            if (newIdx < maxCount)
            {
                sPgCursorIdx = newIdx;
                moved = TRUE;
            }
        }
        if (JOY_NEW(DPAD_LEFT))
        {
            if (sPgCursorIdx > 0)
            {
                sPgCursorIdx--;
                moved = TRUE;
            }
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            u8 newIdx = sPgCursorIdx + cols;
            if (newIdx < maxCount)
            {
                sPgCursorIdx = newIdx;
                moved = TRUE;
            }
        }
        if (JOY_NEW(DPAD_UP))
        {
            if (sPgCursorIdx >= cols)
            {
                sPgCursorIdx -= cols;
                moved = TRUE;
            }
        }
        if (moved)
        {
            PlaySE(SE_SELECT);
            PgMoveCursor(sPgCursorIdx);
        }

        if (JOY_NEW(A_BUTTON) && sPgCursorIdx < sPokeguideTotal)
        {
            u16 species = sPokeguideSpeciesList[sPgCursorIdx];
            u32 dexNum  = SpeciesToNationalPokedexNum(species);
            bool8 seen   = GetSetPokedexFlag(dexNum, FLAG_GET_SEEN);
            bool8 caught = GetSetPokedexFlag(dexNum, FLAG_GET_CAUGHT);
            if (seen || caught)
            {
                PlaySE(SE_SELECT);
                {
                    struct WindowTemplate dynPgTemplate;
                    struct ComfyAnimEasingConfig animCfg;
                    u8 wid;
                    BuildPokeguideActionList();
                    dynPgTemplate = sBookPgActionMenuTemplate;
                    dynPgTemplate.height = sActionMenuHeightPx / 8;
                    wid = AddWindow(&dynPgTemplate);
                    gTasks[taskId].tActionWindowId = wid;
                    gTasks[taskId].tActionCursor   = 0;
                    SetWindowAttribute(wid, WINDOW_TILEMAP_TOP, ACTION_MENU_TILEMAPTOP_OFF);
                    DrawBookActionMenu(wid, 0, FALSE);
                    InitComfyAnimConfig_Easing(&animCfg);
                    animCfg.from          = Q_24_8(ACTION_MENU_TILEMAPTOP_OFF);
                    animCfg.to            = Q_24_8((u32)sActionMenuTilemapTopFinal);
                    animCfg.durationFrames = 10;
                    animCfg.easingFunc    = ComfyAnimEasing_EaseOutCubic;
                    InitComfyAnim_Easing(&animCfg, &sActionMenuSlideAnim);
                }
                gTasks[taskId].func = Task_BookMenu_ActionMenuOpenAnim;
            }
        }
        (void)rows; (void)col; (void)row; (void)maxCount;
    }

    if (JOY_NEW(R_BUTTON))
    {
        PlaySE(SE_SWITCH);
        BeginTabSwitch(taskId, +1);
        return;
    }
    if (JOY_NEW(L_BUTTON))
    {
        PlaySE(SE_SWITCH);
        BeginTabSwitch(taskId, -1);
        return;
    }



}

////////////////////////////////////////
//FXNs to display pokemon characteristics in right window
////////////////////////////////////////

static void CreatePartyMonIcons(void)
{
    u8 i;
    s16 x, y;

    for (i = 0; i < PARTY_SIZE; i++)
        sStartMenuPartyIconSpriteIds[i] = SPRITE_NONE;

    for (i = 0; i < gPlayerPartyCount; i++)
    {

        x = 146 + (i % 2) * 54;
        y =  28 + (i / 2) * 44;

        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);
        u32 pid     = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY);

        sStartMenuPartyIconSpriteIds[i] = CreateMonIcon(
            species,
            SpriteCB_MonIcon,
            x, y,
            4,      // match party_menu
            pid
        );

        // Guard against MAX_SPRITES (64) vs SPRITE_NONE (0xFF): treat any failure as NONE
        if (sStartMenuPartyIconSpriteIds[i] >= MAX_SPRITES)
            sStartMenuPartyIconSpriteIds[i] = SPRITE_NONE;

        if (sStartMenuPartyIconSpriteIds[i] != SPRITE_NONE)
        {
            // Put icons in front of BGs
            gSprites[sStartMenuPartyIconSpriteIds[i]].oam.priority = 2;
        }
    }
}

static void DestroyPartyMonIcons(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (sStartMenuPartyIconSpriteIds[i] != SPRITE_NONE)
        {
            FreeAndDestroyMonIconSprite(&gSprites[sStartMenuPartyIconSpriteIds[i]]);
            sStartMenuPartyIconSpriteIds[i] = SPRITE_NONE;
        }
    }
}

static u8 GetHPBarFrame(struct Pokemon *mon)
{
    u32 hp    = GetMonData(mon, MON_DATA_HP);
    u32 maxHp = GetMonData(mon, MON_DATA_MAX_HP);

    if (hp == 0 || maxHp == 0) return 0;   // hp0: fainted

    u32 pct = (hp * 100) / maxHp;

    if (pct < 10)  return 1;   // hp1
    if (pct < 20)  return 2;   // hp10
    if (pct < 30)  return 3;   // hp20
    if (pct < 40)  return 4;   // hp30
    if (pct < 50)  return 5;   // hp40
    if (pct < 60)  return 6;   // hp50
    if (pct < 70)  return 7;   // hp60
    if (pct < 80)  return 8;   // hp70
    if (pct < 90)  return 9;   // hp80
    if (pct < 100) return 10;  // hp90
    return 11;                  // hp100
}

static void CreatePartyHPBars(void)
{
    u8 i;
    for (i = 0; i < gPlayerPartyCount; i++)
    {
        s16 x = 154 + (i % 2) * 53;
        s16 y =  28 + (i / 2) * 43 + 34;  // 28px below icon top

        u8 spriteId = CreateSprite(&sSpriteTemplate_HPBar, x, y, 1);
        if (spriteId == MAX_SPRITES)
            continue;

        sStartMenuDataPtr->hpBarSpriteIds[i] = spriteId;
        gSprites[spriteId].oam.priority = 2;
        StartSpriteAnim(&gSprites[spriteId], GetHPBarFrame(&gPlayerParty[i]));
    }
}

static void DestroyPartyHPBars(void)
{
    u8 i;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (sStartMenuDataPtr->hpBarSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sStartMenuDataPtr->hpBarSpriteIds[i]]);
            sStartMenuDataPtr->hpBarSpriteIds[i] = SPRITE_NONE;
        }
    }
}

static void PrintPartyLevels(void)
{
    u8 i;
    u8 levelStr[8];

    // Window covers x=152..240, y=88..159
    // Text positions within window are relative to that origin.
    FillWindowPixelBuffer(WIN_RIGHT_LVS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    for (i = 0; i < gPlayerPartyCount; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            continue;

        //s16 hpBarX = 154 + (i % 2) * 53;
        //s16 hpBarY = 28 + (i / 2) * 43 + 34;
        s16 textX = (i % 2) * 53+15;       // window tilemapLeft=19 -> x=152
        s16 textY = ((i / 2)) * 43+1; // window tilemapTop=11  -> y=88

        StringCopy(levelStr, gText_Lv);
        ConvertIntToDecimalStringN(
            levelStr + StringLength(levelStr),
            GetMonData(&gPlayerParty[i], MON_DATA_LEVEL),
            STR_CONV_MODE_LEFT_ALIGN,
            3
        );

        AddTextPrinterParameterized3(WIN_RIGHT_LVS, FONT_SMALL_NARROWER,
                                     textX, textY, sFontColorTable[0], 0, levelStr);
    }

    CopyWindowToVram(WIN_RIGHT_LVS, COPYWIN_FULL);
    PutWindowTilemap(WIN_RIGHT_LVS);
}


// ==========================================
// ACTION MENU
// ==========================================

// {background, text, shadow} using BG palette 0 (book palette) indices.
// 4 = parchment, 2 = green accent.
static const u8 sActionMenuFontColors[]  = {BOOK_MENU_FONT_BG, BOOK_MENU_FONT_FG, BOOK_MENU_FONT_SHADOW};
static const u8 sActionMenuArrowColors[] = {BOOK_MENU_FONT_BG, BOOK_MENU_FONT_FG, BOOK_MENU_FONT_SHADOW};

// Fixed pixel width of action menu window (11 tiles wide).
#define ACTION_MENU_PX_W (11 * 8)

static void DrawBookActionMenu(u8 windowId, u8 cursor, bool8 isFollower)
{
    u8 i;
    u8 hPx = sActionMenuHeightPx;
    FillWindowPixelBuffer(windowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));

    // 1-pixel border using book palette index BOOK_ACTION_BORDER_COLOR
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,        ACTION_MENU_PX_W, 1);      // top
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    hPx - 1,  ACTION_MENU_PX_W, 1);      // bottom
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,        1,               hPx);     // left
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), ACTION_MENU_PX_W - 1, 0,        1,               hPx);     // right

    for (i = 0; i < sBookMenuActionCount; i++)
    {
        u8 actionId = sBookMenuActions[i];
        const u8 *text = (actionId == BOOK_ACTION_FOLLOWER && isFollower)
                       ? sBookActionText_IsFollower
                       : sBookActionTexts[actionId];
        if (i == cursor)
            AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                         3, 2 + i * 12,
                                         sActionMenuArrowColors, 0,
                                         sBookActionText_Arrow);
        AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                     11, 2 + i * 12,
                                     sActionMenuFontColors, 0,
                                     text);
    }
    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

static void CloseBookActionMenu(u8 taskId)
{
    u8 windowId = (u8)gTasks[taskId].tActionWindowId;
    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    RemoveWindow(windowId);
    gTasks[taskId].tActionWindowId = SPRITE_NONE;
    if (sStartMenuDataPtr->currentTab == BOOK_TAB_POKEGUIDE)
    {
        PutWindowTilemap(WIN_PG_INFO);
        PutWindowTilemap(WIN_PG_CATEGORY);
    }
    else
    {
        BookMenu_SetRightPageSpritePriority(2);
        PrintPartyLevels();
        DisplaySelectedPokemonDetails(gSelectedMenu, FALSE);
    }
    ScheduleBgCopyTilemapToVram(0);
}

// Advance the slide animation and reposition the action menu window accordingly.
// Animates tilemapTop (Y). Horizontal position stays fixed; no X wrapping hazard.
static void AdvanceActionMenuSlide(u8 windowId)
{
    int posY;
    TryAdvanceComfyAnim(&sActionMenuSlideAnim);
    posY = ReadComfyAnimValueSmooth(&sActionMenuSlideAnim);
    ClearWindowTilemap(windowId);
    // Restore the windows that overlap the action menu area so their content
    // stays correct in any row the action menu has vacated.
    if (sStartMenuDataPtr->currentTab == BOOK_TAB_POKEGUIDE)
    {
        PutWindowTilemap(WIN_PG_INFO);
        PutWindowTilemap(WIN_PG_CATEGORY);
    }
    else
    {
        PutWindowTilemap(WIN_RIGHT_LVS);
    }
    SetWindowAttribute(windowId, WINDOW_TILEMAP_TOP, (u32)posY);
    PutWindowTilemap(windowId);
    ScheduleBgCopyTilemapToVram(0);
}

static void Task_BookMenu_ActionMenuOpenAnim(u8 taskId)
{
    u8 windowId = (u8)gTasks[taskId].tActionWindowId;
    AdvanceActionMenuSlide(windowId);
    if (sActionMenuSlideAnim.completed)
    {
        // Snap to exact final position in case of easing rounding
        ClearWindowTilemap(windowId);
        if (sStartMenuDataPtr->currentTab == BOOK_TAB_POKEGUIDE)
        {
            PutWindowTilemap(WIN_PG_INFO);
            PutWindowTilemap(WIN_PG_CATEGORY);
        }
        else
        {
            PutWindowTilemap(WIN_RIGHT_LVS);
        }
        SetWindowAttribute(windowId, WINDOW_TILEMAP_TOP, (u32)sActionMenuTilemapTopFinal);
        PutWindowTilemap(windowId);
        ScheduleBgCopyTilemapToVram(0);
        gTasks[taskId].func = Task_BookMenu_ActionMenu;
    }
}

static void BeginCloseBookActionMenu(u8 taskId, u8 closeAction)
{
    struct ComfyAnimEasingConfig animCfg;
    InitComfyAnimConfig_Easing(&animCfg);
    animCfg.from = Q_24_8((u32)sActionMenuTilemapTopFinal);
    animCfg.to   = Q_24_8((u32)sActionMenuTilemapTopStart);
    animCfg.durationFrames = 12;
    animCfg.easingFunc = ComfyAnimEasing_EaseInCubic;
    InitComfyAnim_Easing(&animCfg, &sActionMenuSlideAnim);
    gTasks[taskId].tCloseAction = closeAction;
    gTasks[taskId].func = Task_BookMenu_ActionMenuCloseAnim;
}

static void Task_BookMenu_ActionMenuCloseAnim(u8 taskId)
{
    u8 windowId = (u8)gTasks[taskId].tActionWindowId;
    AdvanceActionMenuSlide(windowId);
    if (!sActionMenuSlideAnim.completed)
        return;

    CloseBookActionMenu(taskId);

    switch ((u8)gTasks[taskId].tCloseAction)
    {
    case CLOSE_THEN_SWITCH:
        CreateSwitchSourceIndicator(taskId, (u8)gTasks[taskId].tSwitchSource);
        gTasks[taskId].func = Task_BookMenu_SwitchTarget;
        break;
    case CLOSE_THEN_FOLLOWER:
        ShowFollowerMsgBox();
        gTasks[taskId].func = Task_BookMenu_ShowFollowerMsg;
        break;
    case CLOSE_THEN_PG_SEARCH:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_BookMenu_PgSearchExit;
        break;
    case CLOSE_THEN_PG_REGISTER:
        {
            PgSetRightPageSpritesBehindBg(TRUE);
            sPgRegisterMsgWindowId = AddWindow(&sBookPgRegisterMsgTemplate);
            FillWindowPixelBuffer(sPgRegisterMsgWindowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
            FillWindowPixelRect(sPgRegisterMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                                0, 0, PG_REGISTER_MSG_PX_W, 1);
            FillWindowPixelRect(sPgRegisterMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                                0, PG_REGISTER_MSG_PX_H - 1, PG_REGISTER_MSG_PX_W, 1);
            FillWindowPixelRect(sPgRegisterMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                                0, 0, 1, PG_REGISTER_MSG_PX_H);
            FillWindowPixelRect(sPgRegisterMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                                PG_REGISTER_MSG_PX_W - 1, 0, 1, PG_REGISTER_MSG_PX_H);
            AddTextPrinterParameterized3(sPgRegisterMsgWindowId, FONT_NORMAL,
                                         6, 4, sActionMenuFontColors, 0, sFollowerMsgStr);
            PutWindowTilemap(sPgRegisterMsgWindowId);
            CopyWindowToVram(sPgRegisterMsgWindowId, COPYWIN_FULL);
            ScheduleBgCopyTilemapToVram(0);
        }
        gTasks[taskId].func = Task_BookMenu_PgRegisterMsg;
        break;
    default: // CLOSE_THEN_MAIN
        gTasks[taskId].func = Task_StartMenuBookMain;
        break;
    }
}

static void BookMenu_SetRightPageSpritePriority(u8 priority)
{
    u8 i;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (sStartMenuPartyIconSpriteIds[i] != SPRITE_NONE)
            gSprites[sStartMenuPartyIconSpriteIds[i]].oam.priority = priority;
        if (sStartMenuDataPtr->hpBarSpriteIds[i] != SPRITE_NONE)
            gSprites[sStartMenuDataPtr->hpBarSpriteIds[i]].oam.priority = priority;
    }
}

// Returns TRUE if the party slot qualifies for the "Follower" submenu:
//   - explicitly set as follower (followerIndex == selected)
//   - implicit front-mon follower (followerIndex == NOT_SET && selected == 0)
//   - slot 0 when follower is recalled (followerIndex == RECALLED && selected == 0)
static bool8 IsDesignatedFollower(u8 selected)
{
    u8 idx = (u8)gSaveBlock3Ptr->followerIndex;
    if (idx == selected)
        return TRUE;
    if (selected == 0 && (idx == OW_FOLLOWER_NOT_SET || idx == OW_FOLLOWER_RECALLED))
        return TRUE;
    return FALSE;
}

static void DrawBookFollowerSubMenu(u8 windowId, u8 cursor, u8 followerState)
{
    const u8 *texts[2];
    u8 i;

    switch (followerState)
    {
    case FOLLOWER_STATE_EXPLICIT:
        texts[0] = sBookFollowerText_Unset;
        texts[1] = sBookFollowerText_Return;
        break;
    case FOLLOWER_STATE_RECALLED:
        texts[0] = sBookFollowerText_Set;
        texts[1] = sBookFollowerText_TakeOut;
        break;
    default: // FOLLOWER_STATE_IMPLICIT
        texts[0] = sBookFollowerText_Set;
        texts[1] = sBookFollowerText_Return;
        break;
    }
    {
        u8 hPx = sActionMenuHeightPx;
        FillWindowPixelBuffer(windowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
        FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,       ACTION_MENU_PX_W, 1);
        FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    hPx - 1, ACTION_MENU_PX_W, 1);
        FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,       1,               hPx);
        FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), ACTION_MENU_PX_W - 1, 0,       1,               hPx);
    }
    for (i = 0; i < 2; i++)
    {
        if (i == cursor)
            AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                         3, 2 + i * 12, sActionMenuFontColors, 0,
                                         sBookActionText_Arrow);
        AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                     11, 2 + i * 12, sActionMenuFontColors, 0, texts[i]);
    }
    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

static void DrawBookItemSubMenu(u8 windowId, u8 cursor, bool8 hasTake)
{
    u8 count = hasTake ? 2 : 1;
    u8 i;
    const u8 *texts[2] = { sBookItemText_Give, sBookItemText_Take };

    {
        u8 hPx = sActionMenuHeightPx;
        FillWindowPixelBuffer(windowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
        FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,       ACTION_MENU_PX_W, 1);
        FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    hPx - 1, ACTION_MENU_PX_W, 1);
        FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,       1,               hPx);
        FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), ACTION_MENU_PX_W - 1, 0,       1,               hPx);
    }
    for (i = 0; i < count; i++)
    {
        if (i == cursor)
            AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                         3, 2 + i * 12, sActionMenuFontColors, 0,
                                         sBookActionText_Arrow);
        AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                     11, 2 + i * 12, sActionMenuFontColors, 0, texts[i]);
    }
    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

static void ShowFollowerMsgBox(void)
{
    struct WindowTemplate dynFollowerTemplate = sBookFollowerMsgTemplate;
    switch (sStartMenuDataPtr->currentTab)
    {
    case BOOK_TAB_TRAINER:
        dynFollowerTemplate.baseBlock = 331;
        break;
    case BOOK_TAB_POKEGUIDE:
        dynFollowerTemplate.baseBlock = 236;
        break;
    default:
        dynFollowerTemplate.baseBlock = 85;
        break;
    }
    sFollowerMsgWindowId = AddWindow(&dynFollowerTemplate);
    FillWindowPixelBuffer(sFollowerMsgWindowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
    FillWindowPixelRect(sFollowerMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                        0, 0, FOLLOWER_MSG_PX_W, 1);
    FillWindowPixelRect(sFollowerMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                        0, FOLLOWER_MSG_PX_H - 1, FOLLOWER_MSG_PX_W, 1);
    FillWindowPixelRect(sFollowerMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                        0, 0, 1, FOLLOWER_MSG_PX_H);
    FillWindowPixelRect(sFollowerMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                        FOLLOWER_MSG_PX_W - 1, 0, 1, FOLLOWER_MSG_PX_H);
    AddTextPrinterParameterized3(sFollowerMsgWindowId, FONT_NORMAL,
                                 6, 4, sActionMenuFontColors, 0, sFollowerMsgStr);
    PutWindowTilemap(sFollowerMsgWindowId);
    CopyWindowToVram(sFollowerMsgWindowId, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
    if (sStartMenuDataPtr->currentTab == BOOK_TAB_PARTY)
        BookMenu_SetRightPageSpritePriority(1);
}

static void Task_BookMenu_ShowFollowerMsg(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        BookMenu_SetRightPageSpritePriority(2);
        if (sFollowerMsgWindowId != SPRITE_NONE)
        {
            ClearWindowTilemap(sFollowerMsgWindowId);
            RemoveWindow(sFollowerMsgWindowId);
            sFollowerMsgWindowId = SPRITE_NONE;
            PrintPartyLevels();
            ScheduleBgCopyTilemapToVram(0);
        }
        DisplaySelectedPokemonDetails(gSelectedMenu, FALSE);
        gTasks[taskId].func = Task_StartMenuBookMain;
    }
}

static void Task_BookMenu_ItemSubMenu(u8 taskId)
{
    u8 windowId = (u8)gTasks[taskId].tActionWindowId;
    u8 cursor   = (u8)gTasks[taskId].tItemCursor;
    bool8 hasTake = (bool8)gTasks[taskId].tItemHasTake;
    u8 count    = hasTake ? 2 : 1;
    u8 selected = gSelectedMenu;

    if (JOY_NEW(DPAD_UP))
    {
        u8 newCursor = (cursor == 0) ? count - 1 : cursor - 1;
        PlaySE(SE_SELECT);
        gTasks[taskId].tItemCursor = newCursor;
        UpdateBookMenuCursorArrow(windowId, cursor, newCursor, sActionMenuFontColors);
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        u8 newCursor = (cursor >= count - 1) ? 0 : cursor + 1;
        PlaySE(SE_SELECT);
        gTasks[taskId].tItemCursor = newCursor;
        UpdateBookMenuCursorArrow(windowId, cursor, newCursor, sActionMenuFontColors);
    }
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        // Return to action menu with ITEM highlighted
        gTasks[taskId].tActionCursor = FindBookAction(BOOK_ACTION_ITEM);
        DrawBookActionMenu(windowId, FindBookAction(BOOK_ACTION_ITEM), (bool8)gTasks[taskId].tIsFollower);
        gTasks[taskId].func = Task_BookMenu_ActionMenu;
        return;
    }
    if (!JOY_NEW(A_BUTTON))
        return;

    PlaySE(SE_SELECT);
    switch (cursor)
    {
    case BOOK_ITEM_GIVE:
        CloseBookActionMenu(taskId);
        gPartyMenu.slotId = selected;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_BookMenuGiveItem;
        break;

    case BOOK_ITEM_TAKE:
        {
            u16 itemId = GetMonData(&gPlayerParty[selected], MON_DATA_HELD_ITEM);
            u16 noItem = ITEM_NONE;
            if (AddBagItem(itemId, 1) == TRUE)
            {
                SetMonData(&gPlayerParty[selected], MON_DATA_HELD_ITEM, &noItem);
                BeginCloseBookActionMenu(taskId, CLOSE_THEN_MAIN);
            }
            else
            {
                // Bag full; play error sound, stay in sub-menu
                PlaySE(SE_FAILURE);
            }
        }
        break;

    default:
        BeginCloseBookActionMenu(taskId, CLOSE_THEN_MAIN);
        break;
    }
}

static void Task_BookMenu_FollowerSubMenu(u8 taskId)
{
    u8 windowId      = (u8)gTasks[taskId].tActionWindowId;
    u8 cursor        = (u8)gTasks[taskId].tItemCursor;
    u8 followerState = (u8)gTasks[taskId].tFollowerState;
    u8 selected      = gSelectedMenu;

    if (JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_DOWN))
    {
        u8 newCursor = cursor ^ 1;
        PlaySE(SE_SELECT);
        gTasks[taskId].tItemCursor = newCursor;
        UpdateBookMenuCursorArrow(windowId, cursor, newCursor, sActionMenuFontColors);
    }
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].tActionCursor = FindBookAction(BOOK_ACTION_FOLLOWER);
        DrawBookActionMenu(windowId, FindBookAction(BOOK_ACTION_FOLLOWER), TRUE);
        gTasks[taskId].func = Task_BookMenu_ActionMenu;
        return;
    }
    if (!JOY_NEW(A_BUTTON))
        return;

    PlaySE(SE_SELECT);
    {
        u8 nickStr[POKEMON_NAME_LENGTH + 1];
        GetMonData(&gPlayerParty[selected], MON_DATA_NICKNAME, nickStr);

        if (cursor == 0)
        {
            switch (followerState)
            {
            case FOLLOWER_STATE_EXPLICIT:
                // Unset: clear designation; front mon auto-follows again
                gSaveBlock3Ptr->followerIndex = OW_FOLLOWER_NOT_SET;
                StringCopy(sFollowerMsgStr, nickStr);
                StringAppend(sFollowerMsgStr, sBookText_NoLongerFollower);
                BeginCloseBookActionMenu(taskId, CLOSE_THEN_FOLLOWER);
                break;
            case FOLLOWER_STATE_RECALLED:
                // Set: explicitly set slot 0 as follower from recalled state
                gSaveBlock3Ptr->followerIndex = selected;
                StringCopy(sFollowerMsgStr, nickStr);
                StringAppend(sFollowerMsgStr, sBookText_IsNowFollowing);
                BeginCloseBookActionMenu(taskId, CLOSE_THEN_FOLLOWER);
                break;
            default: // FOLLOWER_STATE_IMPLICIT
                // Set: explicitly designate the implicit front-mon follower
                gSaveBlock3Ptr->followerIndex = selected;
                StringCopy(sFollowerMsgStr, nickStr);
                StringAppend(sFollowerMsgStr, sBookText_IsNowFollowing);
                BeginCloseBookActionMenu(taskId, CLOSE_THEN_FOLLOWER);
                break;
            }
        }
        else
        {
            // cursor == 1
            if (followerState == FOLLOWER_STATE_RECALLED)
            {
                // Take Out: stop recalled state; front mon auto-follows again
                gSaveBlock3Ptr->followerIndex = OW_FOLLOWER_NOT_SET;
                BeginCloseBookActionMenu(taskId, CLOSE_THEN_MAIN);
            }
            else
            {
                // Return: send to pokeball, no follower until explicitly set
                gSaveBlock3Ptr->followerIndex = OW_FOLLOWER_RECALLED;
                StringCopy(sFollowerMsgStr, nickStr);
                StringAppend(sFollowerMsgStr, sBookText_Recalled);
                BeginCloseBookActionMenu(taskId, CLOSE_THEN_FOLLOWER);
            }
        }
    }
}

// Returns the index of actionId in sBookMenuActions[], or 0 if not found.
static u8 FindBookAction(u8 actionId)
{
    u8 i;
    for (i = 0; i < sBookMenuActionCount; i++)
    {
        if (sBookMenuActions[i] == actionId)
            return i;
    }
    return 0;
}

// Fills sBookFieldMoves[] with FIELD_MOVE_* enum values usable by the given party slot.
// A move qualifies if: the badge/unlock check passes AND (pokemon knows the move OR can learn it).
static void BuildBookFieldMoveList(u8 slot)
{
    u8 i, j;
    u16 species;
    sBookFieldMoveCount = 0;

    if (GetMonData(&gPlayerParty[slot], MON_DATA_IS_EGG))
        return;

    species = GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES);
    for (i = 0; i < FIELD_MOVES_COUNT; i++)
    {
        u16 moveId = FieldMove_GetMoveId(i);
        bool8 knowsMove = FALSE;

        if (!IsFieldMoveUnlocked(i))
            continue;

        // Check if pokemon already knows the move
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            if (GetMonData(&gPlayerParty[slot], MON_DATA_MOVE1 + j) == moveId)
            {
                knowsMove = TRUE;
                break;
            }
        }

        if (knowsMove || (CanLearnTeachableMove(species, moveId)
                          && CheckBagHasItem(GetTMHMItemIdFromMoveId(moveId), 1)))
            sBookFieldMoves[sBookFieldMoveCount++] = i;
    }
}

// Returns TRUE if the given slot has any available field moves.
static bool8 BookMonHasFieldMoves(u8 slot)
{
    u8 i, j;
    u16 species;
    if (GetMonData(&gPlayerParty[slot], MON_DATA_IS_EGG))
        return FALSE;
    species = GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES);
    for (i = 0; i < FIELD_MOVES_COUNT; i++)
    {
        u16 moveId = FieldMove_GetMoveId(i);
        if (!IsFieldMoveUnlocked(i))
            continue;
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            if (GetMonData(&gPlayerParty[slot], MON_DATA_MOVE1 + j) == moveId)
                return TRUE;
        }
        if (CanLearnTeachableMove(species, moveId)
            && CheckBagHasItem(GetTMHMItemIdFromMoveId(moveId), 1))
            return TRUE;
    }
    return FALSE;
}

// Builds the visible action list for the selected slot and computes adaptive menu height.
// Must be called before opening the action menu.
static void BuildBookActionList(u8 selected)
{
    u8 count;
    u8 heightTiles;
    sBookMenuActionCount = 0;
    if (!GetMonData(&gPlayerParty[selected], MON_DATA_IS_EGG))
        sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_POKEDEX;
    sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_SUMMARY;
    sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_SWITCH;
    sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_FOLLOWER;
    sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_ITEM;
    if (FlagGet(FLAG_CAN_USE_STAT_EDITOR))
        sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_STAT_EDIT;
    if (BookMonHasFieldMoves(selected))
        sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_FIELD_MOVES;
    if (FlagGet(FLAG_CAN_USE_MOVE_RELEARNER)
        && !GetMonData(&gPlayerParty[selected], MON_DATA_IS_EGG)
        && (GetNumberOfLevelUpMoves(&gPlayerParty[selected])
            || GetNumberOfEggMoves(&gPlayerParty[selected])
            || GetNumberOfTMMoves(&gPlayerParty[selected])
            || GetNumberOfTutorMoves(&gPlayerParty[selected])))
    {
        sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_RELEARNER;
    }
    // Adaptive height: minimum tiles to contain all items at 12px stride plus 2px border.
    // Formula: ceil((count*12 + 2) / 8). Clamped to window max of 13 tiles.
    count = sBookMenuActionCount;
    heightTiles = (count * 12 + 2 + 7) / 8;
    if (heightTiles > 13)
        heightTiles = 13;
    sActionMenuHeightPx        = heightTiles * 8;
    sActionMenuTilemapTopFinal = 20 - heightTiles;
    // GBA tilemap is 32 rows; window at row T with height H wraps if T+H > 32.
    // Start just off-screen at row 20 when safe, otherwise at 32-H to avoid wrap.
    sActionMenuTilemapTopStart = (20 + heightTiles > 32) ? (u8)(32 - heightTiles) : 20;
}

static void BuildPokeguideActionList(void)
{
    u8 heightTiles;
    sBookMenuActionCount = 0;
    sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_PG_SEARCH;
    sBookMenuActions[sBookMenuActionCount++] = BOOK_ACTION_PG_REGISTER;
    heightTiles = (sBookMenuActionCount * 12 + 2 + 7) / 8;
    if (heightTiles > 5)
        heightTiles = 5;
    sActionMenuHeightPx        = heightTiles * 8;
    sActionMenuTilemapTopFinal = 20 - heightTiles;
    sActionMenuTilemapTopStart = 20; // PG menu height=5 always safe at row 20 (5+20=25 < 32)
}

static void Task_BookMenu_PgSearchExit(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        DexNavStartSearchFromBook();
        DestroyTask(taskId);
    }
}

static void CB2_ReturnToBookTrainer(void)
{
    CleanupOverworldWindowsAndTilemaps();
    StartBookMenuOnTab(CB2_ReturnToField, BOOK_TAB_TRAINER);
}

static void CB2_ReturnToBookParty(void)
{
    CleanupOverworldWindowsAndTilemaps();
    StartBookMenuOnTab(CB2_ReturnToField, BOOK_TAB_PARTY);
}

static void CB2_BookMenu_OpenPokedexEntry(void)
{
    u16 species = GetMonData(&gPlayerParty[sActionMenuSelectedSlot], MON_DATA_SPECIES);
    SetPokedexOpenTarget(species);
    IncrementGameStat(GAME_STAT_CHECKED_POKEDEX);
    gMain.savedCallback = CB2_ReturnToBookParty;
    SetMainCallback2(CB2_OpenPokedex);
}

static void Task_BookMenu_TcOpenPokedex(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        IncrementGameStat(GAME_STAT_CHECKED_POKEDEX);
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        gMain.savedCallback = CB2_ReturnToBookTrainer;
        SetMainCallback2(CB2_OpenPokedex);
        DestroyTask(taskId);
    }
}

static void Task_BookMenu_TcOpenBag(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        GoToBagMenu(ITEMMENULOCATION_FIELD, POCKETS_COUNT, CB2_ReturnToBookTrainer);
        DestroyTask(taskId);
    }
}

static void Task_BookMenu_TcOpenOptions(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        gMain.savedCallback = CB2_ReturnToBookTrainer;
        SetMainCallback2(CB2_InitOptionMenu);
        DestroyTask(taskId);
    }
}

static void CB2_ReturnToBookMinerals(void)
{
    CleanupOverworldWindowsAndTilemaps();
    StartBookMenuOnTab(CB2_ReturnToField, BOOK_TAB_MINERALS);
}

static void CB2_LaunchMiningMinigame(void)
{
    StartMiningWithCallback(CB2_ReturnToBookMinerals);
}

static void Task_BookMenu_MiningExit(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        StartMenuBook_FreeResources();
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_LaunchMiningMinigame);
        DestroyTask(taskId);
    }
}

// Redraws the follower message box text in-place without recreating the window.
static void RefreshFollowerMsgText(void)
{
    if (sFollowerMsgWindowId == SPRITE_NONE)
        return;
    FillWindowPixelBuffer(sFollowerMsgWindowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
    FillWindowPixelRect(sFollowerMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                        0, 0, FOLLOWER_MSG_PX_W, 1);
    FillWindowPixelRect(sFollowerMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                        0, FOLLOWER_MSG_PX_H - 1, FOLLOWER_MSG_PX_W, 1);
    FillWindowPixelRect(sFollowerMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                        0, 0, 1, FOLLOWER_MSG_PX_H);
    FillWindowPixelRect(sFollowerMsgWindowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR),
                        FOLLOWER_MSG_PX_W - 1, 0, 1, FOLLOWER_MSG_PX_H);
    AddTextPrinterParameterized3(sFollowerMsgWindowId, FONT_NORMAL,
                                 6, 4, sActionMenuFontColors, 0, sFollowerMsgStr);
    CopyWindowToVram(sFollowerMsgWindowId, COPYWIN_FULL);
}

// Animates "Saving" -> "Saving." -> "Saving.." -> "Saving..." then performs the save.
static void Task_BookMenu_SaveAnim(u8 taskId)
{
    static const u8 sText_Saving1[] = _("Saving.");
    static const u8 sText_Saving2[] = _("Saving..");
    static const u8 sText_Saving3[] = _("Saving...");
    static const u8 sText_Saved[]   = _("Game saved.");
    static const u8 sText_Failed[]  = _("Save failed!");
    static const u8 *const sDotFrames[3] = {sText_Saving1, sText_Saving2, sText_Saving3};
    s16 *timer    = &gTasks[taskId].data[6];
    s16 *dotState = &gTasks[taskId].data[7];

    (*timer)++;
    if (*timer < 10)
        return;
    *timer = 0;
    (*dotState)++;
    if (*dotState <= 3)
    {
        StringCopy(sFollowerMsgStr, sDotFrames[*dotState - 1]);
        RefreshFollowerMsgText();
    }
    else
    {
        SaveMapView();
        IncrementGameStat(GAME_STAT_SAVED_GAME);
        StringCopy(sFollowerMsgStr,
            TrySavingData(SAVE_NORMAL) == SAVE_STATUS_OK ? sText_Saved : sText_Failed);
        RefreshFollowerMsgText();
        gTasks[taskId].func = Task_BookMenu_SaveMsg;
    }
}

// Shown after a save attempt; dismisses on A or B.
static void Task_BookMenu_SaveMsg(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        if (sStartMenuDataPtr->currentTab == BOOK_TAB_PARTY)
            BookMenu_SetRightPageSpritePriority(2);
        if (sFollowerMsgWindowId != SPRITE_NONE)
        {
            ClearWindowTilemap(sFollowerMsgWindowId);
            RemoveWindow(sFollowerMsgWindowId);
            sFollowerMsgWindowId = SPRITE_NONE;
            if (sStartMenuDataPtr->currentTab == BOOK_TAB_PARTY)
            {
                PrintPartyLevels();
                DisplaySelectedPokemonDetails(gSelectedMenu, FALSE);
            }
            ScheduleBgCopyTilemapToVram(0);
        }
        gTasks[taskId].func = Task_StartMenuBookMain;
    }
}

static void Task_BookMenu_PgRegisterMsg(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        if (sPgRegisterMsgWindowId != SPRITE_NONE)
        {
            ClearWindowTilemap(sPgRegisterMsgWindowId);
            CopyWindowToVram(sPgRegisterMsgWindowId, COPYWIN_FULL);
            RemoveWindow(sPgRegisterMsgWindowId);
            sPgRegisterMsgWindowId = SPRITE_NONE;
        }
        PgSetRightPageSpritesBehindBg(FALSE);
        // Restore pokeguide info windows (no party-specific redraws here)
        PutWindowTilemap(WIN_PG_INFO);
        PutWindowTilemap(WIN_PG_CATEGORY);
        ScheduleBgCopyTilemapToVram(0);
        gTasks[taskId].func = Task_StartMenuBookMain;
    }
}

static void DrawBookFieldMovesSubMenu(u8 windowId, u8 cursor)
{
    u8 i;
    u8 hPx = sActionMenuHeightPx;
    FillWindowPixelBuffer(windowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,       ACTION_MENU_PX_W, 1);
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    hPx - 1, ACTION_MENU_PX_W, 1);
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,       1,               hPx);
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), ACTION_MENU_PX_W - 1, 0,       1,               hPx);
    for (i = 0; i < sBookFieldMoveCount; i++)
    {
        const u8 *name = GetMoveName(FieldMove_GetMoveId(sBookFieldMoves[i]));
        if (i == cursor)
            AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                         3, 2 + i * 12, sActionMenuFontColors, 0,
                                         sBookActionText_Arrow);
        AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                     11, 2 + i * 12, sActionMenuFontColors, 0, name);
    }
    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

// Moves the arrow indicator between rows without a full window redraw.
// Erases the old arrow pixel rect and draws the new arrow at newCursor row.
static void UpdateBookMenuCursorArrow(u8 windowId, u8 oldCursor, u8 newCursor, const u8 *colors)
{
    FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_MENU_BG_COLOR), 3, 2 + oldCursor * 12, 8, 12);
    AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                 3, 2 + newCursor * 12,
                                 colors, 0,
                                 sBookActionText_Arrow);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

static void CB2_BookMenu_ExecuteFieldMove(void)
{
    // sFieldMoveExitCB is set before the fade: CB2_ReturnToField or CB2_BookMenu_OpenFlyMap.
    SetMainCallback2(sFieldMoveExitCB != NULL ? sFieldMoveExitCB : CB2_ReturnToField);
}

// CB2_OpenFlyMap requires clean VRAM tile data. The book menu leaves its own tile data in
// charBlock 0 which causes CB2_OpenFlyMap to display corrupted graphics (blue fill).
// This wrapper clears VRAM first, then hands off to the standard fly map callback.
static void CB2_BookMenu_OpenFlyMap(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        gMain.state++;
        break;
    default:
        SetMainCallback2(CB2_OpenFlyMap);
        break;
    }
}

static void Task_BookMenu_FieldMovesSubMenu(u8 taskId)
{
    u8 windowId = (u8)gTasks[taskId].tActionWindowId;
    u8 cursor   = (u8)gTasks[taskId].tItemCursor;
    u8 count    = sBookFieldMoveCount;

    if (JOY_NEW(DPAD_UP))
    {
        u8 newCursor = (cursor == 0) ? count - 1 : cursor - 1;
        PlaySE(SE_SELECT);
        gTasks[taskId].tItemCursor = newCursor;
        UpdateBookMenuCursorArrow(windowId, cursor, newCursor, sActionMenuFontColors);
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        u8 newCursor = (cursor >= count - 1) ? 0 : cursor + 1;
        PlaySE(SE_SELECT);
        gTasks[taskId].tItemCursor = newCursor;
        UpdateBookMenuCursorArrow(windowId, cursor, newCursor, sActionMenuFontColors);
    }
    if (JOY_NEW(B_BUTTON))
    {
        struct ComfyAnimEasingConfig animCfg;
        u8 oldTop = sActionMenuTilemapTopFinal;
        PlaySE(SE_SELECT);
        // Rebuild action list to get its height, then animate back to that height.
        BuildBookActionList(gSelectedMenu);
        gTasks[taskId].tActionCursor = FindBookAction(BOOK_ACTION_FIELD_MOVES);
        FillWindowPixelBuffer(windowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
        CopyWindowToVram(windowId, COPYWIN_FULL);
        InitComfyAnimConfig_Easing(&animCfg);
        animCfg.from = Q_24_8((u32)oldTop);
        animCfg.to   = Q_24_8((u32)sActionMenuTilemapTopFinal);
        animCfg.durationFrames = 8;
        animCfg.easingFunc = ComfyAnimEasing_EaseInOutCubic;
        InitComfyAnim_Easing(&animCfg, &sActionMenuSlideAnim);
        sHeightTransitionTarget = HEIGHT_TRANSITION_TO_ACTION_MENU;
        gTasks[taskId].func = Task_BookMenu_HeightTransitionAnim;
        return;
    }
    if (!JOY_NEW(A_BUTTON))
        return;

    PlaySE(SE_SELECT);
    sFieldMoveToUse = sBookFieldMoves[cursor];
    gPartyMenu.slotId = gSelectedMenu;
    if (SetUpFieldMove(sFieldMoveToUse))
    {
        // Move succeeded - set exit callback and fade to field.
        sFieldMoveExitCB = (sFieldMoveToUse == FIELD_MOVE_FLY) ? CB2_BookMenu_OpenFlyMap : CB2_ReturnToField;
        CloseBookActionMenu(taskId);
        sStartMenuDataPtr->savedCallback = CB2_BookMenu_ExecuteFieldMove;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_StartMenuBookTurnOff;
    }
    else
    {
        // Move failed - show error inline in the action menu window.
        // Cannot use ShowFollowerMsgBox here: it shares baseBlock=85 with the
        // action menu window and would corrupt the tiles still in use.
        {
            u8 hPx = sActionMenuHeightPx;
            FillWindowPixelBuffer(windowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
            FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,        ACTION_MENU_PX_W, 1);
            FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    hPx - 1,  ACTION_MENU_PX_W, 1);
            FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), 0,                    0,        1,               hPx);
            FillWindowPixelRect(windowId, PIXEL_FILL(BOOK_ACTION_BORDER_COLOR), ACTION_MENU_PX_W - 1, 0,        1,               hPx);
            AddTextPrinterParameterized3(windowId, FONT_NORMAL,
                                         4, 4, sActionMenuFontColors, 0, sBookText_CantUseHere);
            CopyWindowToVram(windowId, COPYWIN_FULL);
            ScheduleBgCopyTilemapToVram(0);
        }
        gTasks[taskId].func = Task_BookMenu_FieldMoveError;
    }
}

// Waits for A/B after a "can't use here" error, then restores the field moves submenu.
static void Task_BookMenu_FieldMoveError(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        u8 windowId = (u8)gTasks[taskId].tActionWindowId;
        PlaySE(SE_SELECT);
        DrawBookFieldMovesSubMenu(windowId, (u8)gTasks[taskId].tItemCursor);
        gTasks[taskId].func = Task_BookMenu_FieldMovesSubMenu;
    }
}

static void Task_BookMenuOpenRelearner(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        // Screen is already black from our palette fade. Set up the relearner
        // directly (bypassing ChooseMonForMoveRelearner which is for script context
        // and would start a redundant second fade and require selecting a mon).
        gRelearnMode = RELEARN_MODE_PARTY_MENU;
        gMoveRelearnerState = MOVE_RELEARNER_LEVEL_UP_MOVES;
        gSpecialVar_0x8004 = gSelectedMenu;
        gLastViewedMonIndex = gSelectedMenu;
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        StartMenuBook_FreeResources();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitLearnMove);
        DestroyTask(taskId);
    }
}

// Animates the action menu window to a new tilemapTop stored in sActionMenuTilemapTopFinal,
// then draws content based on sHeightTransitionTarget.
static void Task_BookMenu_HeightTransitionAnim(u8 taskId)
{
    u8 windowId = (u8)gTasks[taskId].tActionWindowId;
    AdvanceActionMenuSlide(windowId);
    if (sActionMenuSlideAnim.completed)
    {
        // Snap to exact final position
        ClearWindowTilemap(windowId);
        if (sStartMenuDataPtr->currentTab == BOOK_TAB_POKEGUIDE)
        {
            PutWindowTilemap(WIN_PG_INFO);
            PutWindowTilemap(WIN_PG_CATEGORY);
        }
        else
        {
            PutWindowTilemap(WIN_RIGHT_LVS);
        }
        SetWindowAttribute(windowId, WINDOW_TILEMAP_TOP, (u32)sActionMenuTilemapTopFinal);
        PutWindowTilemap(windowId);
        ScheduleBgCopyTilemapToVram(0);
        if (sHeightTransitionTarget == HEIGHT_TRANSITION_TO_FIELD_MOVES)
        {
            DrawBookFieldMovesSubMenu(windowId, 0);
            gTasks[taskId].func = Task_BookMenu_FieldMovesSubMenu;
        }
        else
        {
            u8 cursor = (u8)gTasks[taskId].tActionCursor;
            DrawBookActionMenu(windowId, cursor, (bool8)gTasks[taskId].tIsFollower);
            gTasks[taskId].func = Task_BookMenu_ActionMenu;
        }
    }
}

static void CreateSwitchSourceIndicator(u8 taskId, u8 srcSlot)
{
    static const s16 sColX[] = {CURSOR_LEFT_COL_X, CURSOR_RIGHT_COL_X};
    static const s16 sRowY[] = {CURSOR_TOP_ROW_Y, CURSOR_MID_ROW_Y, CURSOR_BTM_ROW_Y};
    u8 spriteId;
    // Load red palette directly into a fixed OBJ slot, bypassing the tag system
    // so no icon/item palette slots are displaced.
    LoadPalette(sSwitchIndicatorPal_Data, OBJ_PLTT_ID(SWITCH_INDICATOR_PAL_SLOT), PLTT_SIZE_4BPP);
    spriteId = CreateSprite(&sSpriteTemplate_SwitchIndicator,
                            sColX[srcSlot % 2], sRowY[srcSlot / 2], 2);
    if (spriteId < MAX_SPRITES)
        gSprites[spriteId].oam.paletteNum = SWITCH_INDICATOR_PAL_SLOT;
    gTasks[taskId].tSwitchIndicatorId = (spriteId >= MAX_SPRITES) ? SPRITE_NONE : spriteId;
}

static void DestroySwitchSourceIndicator(u8 taskId)
{
    u8 spriteId = (u8)gTasks[taskId].tSwitchIndicatorId;
    if (spriteId != SPRITE_NONE)
    {
        DestroySprite(&gSprites[spriteId]);
        gTasks[taskId].tSwitchIndicatorId = SPRITE_NONE;
    }
}

static void CB2_ReturnToBookMenu(void)
{
    StartBookMenu(CB2_ReturnToField);
}

static void CB2_BookMenu_ShowSummary(void)
{
    ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL, gPlayerParty,
                             sActionMenuSelectedSlot,
                             gPlayerPartyCount - 1,
                             CB2_ReturnToBookMenu);
}

static void CB2_BookMenu_ShowStatEditor(void)
{
    StatEditor_Init(CB2_ReturnToBookMenu);
}

static void Task_BookMenu_SwitchTarget(u8 taskId)
{
    u8 source = (u8)gTasks[taskId].tSwitchSource;
    u8 target;

    if (JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
        sStartMenuDataPtr->selector_x ^= 1;
    if (JOY_NEW(DPAD_UP))
    {
        if (sStartMenuDataPtr->selector_y == 0)
            sStartMenuDataPtr->selector_y = 2;
        else
            sStartMenuDataPtr->selector_y--;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        if (sStartMenuDataPtr->selector_y == 2)
            sStartMenuDataPtr->selector_y = 0;
        else
            sStartMenuDataPtr->selector_y++;
    }

    target = sStartMenuDataPtr->selector_x + (sStartMenuDataPtr->selector_y * 2);

    if (JOY_NEW(A_BUTTON))
    {
        DestroySwitchSourceIndicator(taskId);
        if (target != source && target < gPlayerPartyCount)
        {
            struct Pokemon temp;
            PlaySE(SE_SELECT);
            temp = gPlayerParty[source];
            gPlayerParty[source] = gPlayerParty[target];
            gPlayerParty[target] = temp;
            if (gSaveBlock3Ptr->followerIndex == source)
                gSaveBlock3Ptr->followerIndex = target;
            else if (gSaveBlock3Ptr->followerIndex == target)
                gSaveBlock3Ptr->followerIndex = source;
            // Destroy held item first to avoid palette collision from LoadMonIconPalettes
            DestroyBookHeldItemIcon();
            DestroyPartyMonIcons();
            CreatePartyMonIcons();
            DestroyPartyHPBars();
            CreatePartyHPBars();
            PrintPartyLevels();
            DisplaySelectedPokemonDetails(gSelectedMenu, TRUE);
        }
        gTasks[taskId].func = Task_StartMenuBookMain;
    }
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DestroySwitchSourceIndicator(taskId);
        gTasks[taskId].func = Task_StartMenuBookMain;
    }
}

static void Task_BookMenu_ActionMenu(u8 taskId)
{
    u8 windowId = (u8)gTasks[taskId].tActionWindowId;
    u8 cursor   = (u8)gTasks[taskId].tActionCursor;
    u8 selected = gSelectedMenu;

    if (JOY_NEW(DPAD_UP))
    {
        u8 newCursor = (cursor == 0) ? sBookMenuActionCount - 1 : cursor - 1;
        PlaySE(SE_SELECT);
        gTasks[taskId].tActionCursor = newCursor;
        UpdateBookMenuCursorArrow(windowId, cursor, newCursor, sActionMenuArrowColors);
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        u8 newCursor = (cursor >= sBookMenuActionCount - 1) ? 0 : cursor + 1;
        PlaySE(SE_SELECT);
        gTasks[taskId].tActionCursor = newCursor;
        UpdateBookMenuCursorArrow(windowId, cursor, newCursor, sActionMenuArrowColors);
    }
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginCloseBookActionMenu(taskId, CLOSE_THEN_MAIN);
        return;
    }
    if (!JOY_NEW(A_BUTTON))
        return;

    PlaySE(SE_SELECT);

    switch (sBookMenuActions[cursor])
    {
    case BOOK_ACTION_POKEDEX:
        CloseBookActionMenu(taskId);
        sActionMenuSelectedSlot = selected;
        sStartMenuDataPtr->savedCallback = CB2_BookMenu_OpenPokedexEntry;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_StartMenuBookTurnOff;
        break;

    case BOOK_ACTION_SUMMARY:
        // Fades to black immediately; no need for a slide-out.
        CloseBookActionMenu(taskId);
        sActionMenuSelectedSlot = selected;
        sStartMenuDataPtr->savedCallback = CB2_BookMenu_ShowSummary;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_StartMenuBookTurnOff;
        break;

    case BOOK_ACTION_SWITCH:
        gTasks[taskId].tSwitchSource = selected;
        BeginCloseBookActionMenu(taskId, CLOSE_THEN_SWITCH);
        break;

    case BOOK_ACTION_FOLLOWER:
        if (GetMonData(&gPlayerParty[selected], MON_DATA_IS_EGG))
        {
            BeginCloseBookActionMenu(taskId, CLOSE_THEN_MAIN);
            break;
        }
        if ((bool8)gTasks[taskId].tIsFollower)
        {
            // IS the following pokemon: open submenu
            u8 idx = (u8)gSaveBlock3Ptr->followerIndex;
            u8 followerState = (idx == (u8)selected)          ? FOLLOWER_STATE_EXPLICIT
                             : (idx == OW_FOLLOWER_RECALLED)   ? FOLLOWER_STATE_RECALLED
                             :                                   FOLLOWER_STATE_IMPLICIT;
            gTasks[taskId].tItemCursor = 0;
            gTasks[taskId].tFollowerState = followerState;
            DrawBookFollowerSubMenu(windowId, 0, followerState);
            gTasks[taskId].func = Task_BookMenu_FollowerSubMenu;
        }
        else
        {
            // NOT the active follower: immediately set as follower
            u8 nickStr[POKEMON_NAME_LENGTH + 1];
            GetMonData(&gPlayerParty[selected], MON_DATA_NICKNAME, nickStr);
            gSaveBlock3Ptr->followerIndex = selected;
            if (selected != 0)
                gFollowerSteps = 0;
            StringCopy(sFollowerMsgStr, nickStr);
            StringAppend(sFollowerMsgStr, sBookText_IsNowFollowing);
            BeginCloseBookActionMenu(taskId, CLOSE_THEN_FOLLOWER);
        }
        break;

    case BOOK_ACTION_ITEM:
        // Keep the window open and repurpose it as the item sub-menu.
        {
            u16 held = GetMonData(&gPlayerParty[selected], MON_DATA_HELD_ITEM);
            bool8 hasTake = (held != ITEM_NONE);
            gTasks[taskId].tItemCursor = 0;
            gTasks[taskId].tItemHasTake = hasTake;
            DrawBookItemSubMenu(windowId, 0, hasTake);
            gTasks[taskId].func = Task_BookMenu_ItemSubMenu;
        }
        break;

    case BOOK_ACTION_STAT_EDIT:
        // Fades to black immediately; no need for a slide-out.
        CloseBookActionMenu(taskId);
        gSpecialVar_0x8004 = selected;
        sStartMenuDataPtr->savedCallback = CB2_BookMenu_ShowStatEditor;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_StartMenuBookTurnOff;
        break;

    case BOOK_ACTION_FIELD_MOVES:
        BuildBookFieldMoveList(selected);
        {
            // Resize menu for the number of field moves, then animate to new height.
            u8 htiles = (sBookFieldMoveCount * 12 + 2 + 7) / 8;
            u8 oldTop = sActionMenuTilemapTopFinal;
            struct ComfyAnimEasingConfig animCfg;
            if (htiles > 11) htiles = 11;
            sActionMenuHeightPx = htiles * 8;
            sActionMenuTilemapTopFinal = 20 - htiles;
            FillWindowPixelBuffer(windowId, PIXEL_FILL(BOOK_MENU_BG_COLOR));
            CopyWindowToVram(windowId, COPYWIN_FULL);
            InitComfyAnimConfig_Easing(&animCfg);
            animCfg.from = Q_24_8((u32)oldTop);
            animCfg.to   = Q_24_8((u32)sActionMenuTilemapTopFinal);
            animCfg.durationFrames = 8;
            animCfg.easingFunc = ComfyAnimEasing_EaseInOutCubic;
            InitComfyAnim_Easing(&animCfg, &sActionMenuSlideAnim);
        }
        gTasks[taskId].tItemCursor = 0;
        sHeightTransitionTarget = HEIGHT_TRANSITION_TO_FIELD_MOVES;
        gTasks[taskId].func = Task_BookMenu_HeightTransitionAnim;
        break;

    case BOOK_ACTION_RELEARNER:
        CloseBookActionMenu(taskId);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_BookMenuOpenRelearner;
        break;

    case BOOK_ACTION_PG_SEARCH:
        {
            u16 species     = sPokeguideSpeciesList[sPgCursorIdx];
            u8  environment = sStartMenuDataPtr->isOnWater ? 1 : 0;
            // Set search vars for CB1_InitDexNavSearch without touching DN_VAR_SPECIES
            // (so the registered L-button species is not changed by a one-time search).
            gSpecialVar_0x8000 = species;
            gSpecialVar_0x8001 = environment;
            gSpecialVar_0x8002 = FALSE;
            PlaySE(SE_DEX_SEARCH);
            BeginCloseBookActionMenu(taskId, CLOSE_THEN_PG_SEARCH);
        }
        break;

    case BOOK_ACTION_PG_REGISTER:
        {
            u16 species     = sPokeguideSpeciesList[sPgCursorIdx];
            u8  environment = sStartMenuDataPtr->isOnWater ? 1 : 0;
            u8  speciesName[POKEMON_NAME_LENGTH + 1];
            VarSet(DN_VAR_SPECIES, (environment << 14) | species);
            StringCopy(speciesName, GetSpeciesName(species));
            StringCopy(sFollowerMsgStr, speciesName);
            StringAppend(sFollowerMsgStr, sBookText_RegisteredToL);
            BeginCloseBookActionMenu(taskId, CLOSE_THEN_PG_REGISTER);
        }
        break;

    default:
        BeginCloseBookActionMenu(taskId, CLOSE_THEN_MAIN);
        break;
    }
}

static void DestroyTypeIconSprites(void)
{
    u8 i;
    for (i = 0; i < 2; i++)
    {
        if (sStartMenuDataPtr->typeIconSpriteIds[i] != SPRITE_NONE)
        {
            FreeOamMatrix(gSprites[sStartMenuDataPtr->typeIconSpriteIds[i]].oam.matrixNum);
            DestroySprite(&gSprites[sStartMenuDataPtr->typeIconSpriteIds[i]]);
            sStartMenuDataPtr->typeIconSpriteIds[i] = SPRITE_NONE;
        }
    }
}

static void CreateOrUpdateTypeIcons(u16 species)
{
    u8 type1 = GetSpeciesType(species, 0);
    u8 type2 = GetSpeciesType(species, 1);
    bool8 hasTwoTypes = (type1 != type2);
    struct Sprite *sprite;

    // ST_OAM_AFFINE_DOUBLE doubles the rendering area to 32x32 to avoid clipping at 45 degrees.
    // Subtract 8 from x and y so the stamp's visual center stays at the same screen position.
    // 0xE000 = -45 degrees (tilt left), 0x2000 = +45 degrees (tilt right).
    if (sStartMenuDataPtr->typeIconSpriteIds[0] == SPRITE_NONE)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_TypeStamps, TYPE_ICON_X - 8, TYPE_ICON_Y1 - 8, 1);
        if (spriteId == MAX_SPRITES)
            return;
        sStartMenuDataPtr->typeIconSpriteIds[0] = spriteId;
        // CreateSprite already allocated the OAM matrix via InitSpriteAffineAnim.
        // Use it directly; no second AllocOamMatrix call needed.
        SetOamMatrixRotationScaling(gSprites[spriteId].oam.matrixNum, 384, 384, 0);
    }
    sprite = &gSprites[sStartMenuDataPtr->typeIconSpriteIds[0]];
    sprite->x = (hasTwoTypes ? TYPE_ICON_X : (TYPE_ICON_X + TYPE_ICON_X2) / 2) - 8;
    sprite->y = (hasTwoTypes ? TYPE_ICON_Y1 : (TYPE_ICON_Y1 + TYPE_ICON_Y2) / 2) - 8;
    sprite->invisible = FALSE;
    sprite->oam.priority = 2;
    SetOamMatrixRotationScaling(sprite->oam.matrixNum,
        hasTwoTypes ? 384 : 512,
        hasTwoTypes ? 384 : 512,
        hasTwoTypes ? 0 : 0);
    StartSpriteAnim(sprite, sTypeStampAnim[type1]);

    if (sStartMenuDataPtr->typeIconSpriteIds[1] == SPRITE_NONE)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_TypeStamps, TYPE_ICON_X2 - 8, TYPE_ICON_Y2 - 8, 1);
        if (spriteId == MAX_SPRITES)
            return;
        sStartMenuDataPtr->typeIconSpriteIds[1] = spriteId;
        // CreateSprite already allocated the OAM matrix via InitSpriteAffineAnim.
        SetOamMatrixRotationScaling(gSprites[spriteId].oam.matrixNum, 384, 384, 0);
    }
    sprite = &gSprites[sStartMenuDataPtr->typeIconSpriteIds[1]];
    sprite->x = TYPE_ICON_X2 - 8;
    sprite->y = TYPE_ICON_Y2 - 8;
    sprite->invisible = !hasTwoTypes;
    sprite->oam.priority = 2;
    // Refresh rotation each update so it stays correct after sprite reuse across tab switches.
    SetOamMatrixRotationScaling(sprite->oam.matrixNum, 384, 384, 0);
    if (hasTwoTypes)
        StartSpriteAnim(sprite, sTypeStampAnim[type2]);
}

static void DisplaySelectedPokemonDetails(u8 index, bool32 refreshMoves)
{
    struct Pokemon *mon = &gPlayerParty[index];
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    GetMonData(mon, MON_DATA_NICKNAME, nickname);
    //Nickname + LV
    FillWindowPixelBuffer(WIN_LEFT_NAME, 0);
    AddTextPrinterParameterized3(WIN_LEFT_NAME, GetFontIdToFit(gNaturesInfo[GetNature(mon)].name, FONT_NARROW, 0, 20), 9, 6, sFontColorTable[0], 0, nickname);
    u8 levelStr[8];

    StringCopy(levelStr, gText_Lv);
    ConvertIntToDecimalStringN(
        levelStr + StringLength(levelStr),
        GetMonData(mon, MON_DATA_LEVEL),
        STR_CONV_MODE_LEFT_ALIGN,
        3
    );
    AddTextPrinterParameterized3(WIN_LEFT_NAME, GetFontIdToFit(levelStr, FONT_NARROW, 0, 30), 85, 6, sFontColorTable[0], 0, levelStr);
    //Nature
    AddTextPrinterParameterized3(WIN_LEFT_NAME, GetFontIdToFit(gNaturesInfo[GetNature(mon)].name, FONT_SMALL_NARROWER, 0, 13), 79, 29, sFontColorTable[0], 0, gNaturesInfo[GetNature(mon)].name);
    //Ability
    AddTextPrinterParameterized3(WIN_LEFT_NAME, GetFontIdToFit(gAbilitiesInfo[GetMonAbility(mon)].name, FONT_SHORT_NARROWER, 0, 13), 50, 18, sFontColorTable[0], 0, gAbilitiesInfo[GetMonAbility(mon)].name);
    // MOVES - only refresh when the selected slot has changed (cursor move, switch, initial load).
    // Skip on action menu / message close to avoid a flicker frame from clear-then-redraw.
    if (refreshMoves)
    {
        u8 i;
        FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(0));

        for (i = 0; i < 4; i++)
        {
            u16 moveId = GetMonData(mon, MON_DATA_MOVE1 + i);
            const u8 *moveName = (moveId == MOVE_NONE) ? gText_OneDash : GetMoveName(moveId);
            AddTextPrinterParameterized3(
                WIN_LEFT_MOVES_ALL,
                GetFontIdToFit(moveName, FONT_SMALL, 0, 13),
                6,
                6+10*i,
                sFontColorTable[0],
                0,
                moveName
            );
        }
    }

    // HELD ITEM
    u16 held = GetMonData(mon, MON_DATA_HELD_ITEM);
    CreateOrUpdateBookHeldItemIcon(held);

    // FRIENDSHIP HEART
    u8 friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    CreateOrUpdateBookHeartIcon(friendship);

    // TYPE ICONS
    u16 species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
    CreateOrUpdateTypeIcons(species);
}


////FXNS to display pokmeon sprite
static void CreateBookPortraitSprite(u8 partyIndex)
{
   DestroyBookPortraitSprite(); // safe replace

    // Palette slot 13: past tag-allocated range (0-8) and switch indicator (12).
    u16 spriteId = CreateMonPicSprite_Affine(
         GetMonData(&gPlayerParty[partyIndex],
         MON_DATA_SPECIES_OR_EGG),
         IsMonShiny(&gPlayerParty[partyIndex]),
         GetMonData(&gPlayerParty[partyIndex],
         MON_DATA_PERSONALITY),MON_PIC_AFFINE_FRONT,
         50, 72, 13, TAG_NONE
    );

    if (spriteId == 0xFFFF)
        spriteId = SPRITE_NONE;

    sStartMenuDataPtr->portraitSpriteId = spriteId;
    if (spriteId != SPRITE_NONE)
    {
        u16 species = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPECIES_OR_EGG);
        gSprites[spriteId].data[0] = species;
        gSprites[spriteId].oam.priority = 2;
        if (!IsMonSpriteNotFlipped(species))
        {
            gSprites[spriteId].affineAnims = gAffineAnims_BattleSpriteContest;
            StartSpriteAffineAnim(&gSprites[spriteId], BATTLER_AFFINE_NORMAL);
        }
        gSprites[spriteId].callback = SpriteCB_BookPokemon;
    }
}

static void DestroyBookPortraitSprite(void)
{
    u16 id = sStartMenuDataPtr->portraitSpriteId;

    if (id != SPRITE_NONE)
    {
       StopCryAndClearCrySongs();
       // MANUALLY FREE THE MATRIX
       if (gSprites[id].oam.affineMode != ST_OAM_AFFINE_OFF)
           FreeOamMatrix(gSprites[id].oam.matrixNum);

        FreeAndDestroyMonPicSprite(id);
        sStartMenuDataPtr->portraitSpriteId = SPRITE_NONE;
    }
}

static void SpriteCB_BookPokemon(struct Sprite *sprite)
{
    u16 species = sprite->data[0];
    PlayCry_Normal(species, 0);
    if (HasTwoFramesAnimation(species))
        StartSpriteAnim(sprite, 1);
    sprite->data[0] = 0; // animation callbacks expect data[0] = 0 as initial state
    sprite->data[1] = IsMonSpriteNotFlipped(species); // sDontFlip: controls flip direction in summary anim
    StartMonSummaryAnimation(sprite, gSpeciesInfo[species].frontAnimId);
}

/////DISPLAY ITEM FXNs

static void DestroyBookHeldItemIcon(void)
{
    u8 spriteId = sStartMenuDataPtr->heldItemSpriteId;
    if (spriteId == SPRITE_NONE)
        return;

    DestroySprite(&gSprites[spriteId]);
    FreeSpriteTilesByTag(TAG_BOOK_HELD_ITEM_TILES);
    FreeSpritePaletteByTag(TAG_BOOK_HELD_ITEM_PAL);

    sStartMenuDataPtr->heldItemSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->heldItemItemId = ITEM_NONE;

}

static void CreateOrUpdateBookHeldItemIcon(u16 itemId)
{
    // No held item
    if (itemId == ITEM_NONE)
    {
        DestroyBookHeldItemIcon();
        return;
    }

    // Already showing this item
    if (sStartMenuDataPtr->heldItemSpriteId != SPRITE_NONE
        && sStartMenuDataPtr->heldItemItemId == itemId)
    {
        return;
    }

    // Replace whatever was there
    DestroyBookHeldItemIcon();

    // Make room (safe even if already free)
    //FreeSpriteTilesByTag(TAG_BOOK_HELD_ITEM_TILES);
    //FreeSpritePaletteByTag(TAG_BOOK_HELD_ITEM_PAL);

    u8 spriteId = AddItemIconSprite(TAG_BOOK_HELD_ITEM_TILES, TAG_BOOK_HELD_ITEM_PAL, itemId);
    if (spriteId == MAX_SPRITES)
        return;

    sStartMenuDataPtr->heldItemSpriteId = spriteId;
    sStartMenuDataPtr->heldItemItemId = itemId;

    gSprites[spriteId].x = 120;  // <-- set to circle center X
    gSprites[spriteId].y = 114;  // <-- set to circle center Y

    // Item icons are usually 32x32
    gSprites[spriteId].x -= 16;
    gSprites[spriteId].y -= 16;

    gSprites[spriteId].oam.priority = 2; // in front of BG
}

/////DISPLAY HEART FXNs

static u8 GetHeartFrameForFriendship(u8 friendship)
{
    if (friendship == 255)  return 5; // heart_perf
    if (friendship >= 230)  return 4; // heart_100 (>=90% of 255)
    if (friendship >= 192)  return 3; // heart_75  (>=75%)
    if (friendship >= 128)  return 2; // heart_50  (>=50%)
    if (friendship >= 64)   return 1; // heart_25  (>=25%)
    return 0;                          // heart_0
}

static void DestroyBookHeartIcon(void)
{
    if (sStartMenuDataPtr->heartSpriteId == SPRITE_NONE)
        return;

    DestroySprite(&gSprites[sStartMenuDataPtr->heartSpriteId]);
    sStartMenuDataPtr->heartSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->lastHeartFrame = 0xFF;
}

static void CreateOrUpdateBookHeartIcon(u8 friendship)
{
    u8 frame = GetHeartFrameForFriendship(friendship);

    if (sStartMenuDataPtr->heartSpriteId != SPRITE_NONE
        && sStartMenuDataPtr->lastHeartFrame == frame)
        return;

    if (sStartMenuDataPtr->heartSpriteId == SPRITE_NONE)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_Heart,
                                   HEART_CENTER_X - 8,
                                   HEART_CENTER_Y, 0);
        if (spriteId == MAX_SPRITES)
            return;
        sStartMenuDataPtr->heartSpriteId = spriteId;
        gSprites[spriteId].oam.priority = 2;
    }

    StartSpriteAnim(&gSprites[sStartMenuDataPtr->heartSpriteId], frame);
    sStartMenuDataPtr->lastHeartFrame = frame;
}

/*
static void ShowLeftMonTypes(void){
    if (summary->isEgg)
        {
            SetTypeSpritePosAndPal(TYPE_MYSTERY, 120, 48, SPRITE_ARR_ID_TYPE);
            SetSpriteInvisibility(SPRITE_ARR_ID_TYPE + 1, TRUE);
        }
        else
        {
            SetTypeSpritePosAndPal(GetSpeciesType(summary->species, 0), 120, 48, SPRITE_ARR_ID_TYPE);
            if (GetSpeciesType(summary->species, 0) != GetSpeciesType(summary->species, 1))
            {
                SetTypeSpritePosAndPal(GetSpeciesType(summary->species, 1), 160, 48, SPRITE_ARR_ID_TYPE + 1);
                SetSpriteInvisibility(SPRITE_ARR_ID_TYPE + 1, FALSE);
            }
            else
            {
                SetSpriteInvisibility(SPRITE_ARR_ID_TYPE + 1, TRUE);
            }
            if (P_SHOW_TERA_TYPE >= GEN_9)
            {
                SetTypeSpritePosAndPal(summary->teraType, 200, 48, SPRITE_ARR_ID_TYPE + 2);
            }
        }

}

*/

// ==========================================
// TAB SYSTEM
// ==========================================

// Clear party tab text windows and destroy all party sprites.
static void UnloadPartyTabContent(void)
{
    DestroyCursor();
    FreeSpriteTilesByTag(TAG_CURSOR);
    FreeSpritePaletteByTag(TAG_CURSOR);
    DestroyBookHeldItemIcon();
    DestroyBookHeartIcon();
    DestroyPartyMonIcons();
    FreeMonIconPalettes();
    DestroyPartyHPBars();
    DestroyTypeIconSprites();
    FreeSpriteTilesByTag(TAG_TYPE_STAMPS_TILES);
    FreeSpritePaletteByTag(TAG_TYPE_STAMPS_PAL);
    DestroyBookPortraitSprite();
    FillWindowPixelBuffer(WIN_LEFT_NAME,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_RIGHT_LVS,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_LEFT_NAME);
    ClearWindowTilemap(WIN_LEFT_MOVES_ALL);
    ClearWindowTilemap(WIN_RIGHT_LVS);
    CopyWindowToVram(WIN_LEFT_NAME,      COPYWIN_FULL);
    CopyWindowToVram(WIN_LEFT_MOVES_ALL, COPYWIN_FULL);
    CopyWindowToVram(WIN_RIGHT_LVS,      COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

// Populate party tab sprites and text. Called at startup and on return to party tab.
static void LoadPartyTabContent(void)
{
    // Cursor, type stamps, and mon icon palettes are party-tab-only.
    // Load everything here; freed in UnloadPartyTabContent.
    LoadCompressedSpriteSheet(&sSpriteSheet_Cursor);
    LoadSpritePalette(&sSpritePal_Cursor);
    LoadSpriteSheet(&sTypeStampsSpriteSheet);
    LoadSpritePalette(&sTypeStampsSpritePalette);
    LoadMonIconPalettes();
    CreateCursor();
    // Re-register text windows in the BG0 tilemap (ClearWindowTilemap removed them on unload).
    PutWindowTilemap(WIN_LEFT_NAME);
    PutWindowTilemap(WIN_LEFT_MOVES_ALL);
    ScheduleBgCopyTilemapToVram(0);
    CreatePartyMonIcons();
    CreatePartyHPBars();
    PrintPartyLevels();
    DisplaySelectedPokemonDetails(gSelectedMenu, TRUE);
    // Reset lastSelectedMenu so Task_StartMenuBookMain's portrait creation triggers on the next frame.
    sStartMenuDataPtr->lastSelectedMenu = 0xFF;
}

// ==========================================
// POKEGUIDE TAB
// ==========================================

// Icon grid positions on the LEFT page (absolute screen coordinates).
// Three columns at 36px spacing; left page is x=0-119px.
#define PG_COL_X0    28   // left column
#define PG_COL_X1    62   // middle column
#define PG_COL_X2    98   // right column
// First row starts at y=28 (below route name window at y=8-24).
#define PG_LAND_Y0    88
#define PG_WATER_Y0    70   // right column
#define PG_ROW_STEP  24   // 36px between row centers

static const s16 sPgColX[3] = { PG_COL_X0+4, PG_COL_X1+4, PG_COL_X2+4 };
static const u8 sText_CatchFirst[] = _("Catch First!");

// Helper: add a species to the unique list if not already present.
// Returns TRUE if added, FALSE if already present or list is full.
static bool8 AddUniqueSpecies(u16 *list, u8 *count, u8 max, u16 species)
{
    u8 i;
    if (species == SPECIES_NONE || *count >= max)
        return FALSE;
    for (i = 0; i < *count; i++)
    {
        if (list[i] == species)
            return FALSE;
    }
    list[(*count)++] = species;
    return TRUE;
}

// Build the species list from the current map's encounter tables.
// Shows land encounters when on foot, water encounters when surfing.
// Populates sPokeguideSpeciesList and sPokeguideTotal; returns the count added.
static u8 BuildPokeguideSpeciesList(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    u8 count = 0;
    u8 i;

    sPokeguideTotal = 0;

    if (headerId == HEADER_NONE)
        return 0;

    enum TimeOfDay tod = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);

    if (sStartMenuDataPtr->isOnWater)
    {
        const struct WildPokemonInfo *waterInfo = gWildMonHeaders[headerId].encounterTypes[tod].waterMonsInfo;
        if (waterInfo != NULL)
        {
            for (i = 0; i < WATER_WILD_COUNT && count < POKEGUIDE_MAX_WATER; i++)
            {
                if (AddUniqueSpecies(sPokeguideSpeciesList, &sPokeguideTotal, POKEGUIDE_MAX_TOTAL,
                                     waterInfo->wildPokemon[i].species))
                    count++;
            }
        }
    }
    else
    {
        const struct WildPokemonInfo *landInfo = gWildMonHeaders[headerId].encounterTypes[tod].landMonsInfo;
        if (landInfo != NULL)
        {
            for (i = 0; i < LAND_WILD_COUNT && count < POKEGUIDE_MAX_LAND; i++)
            {
                if (AddUniqueSpecies(sPokeguideSpeciesList, &sPokeguideTotal, POKEGUIDE_MAX_TOTAL,
                                     landInfo->wildPokemon[i].species))
                    count++;
            }
        }
    }

    return sPokeguideTotal;
}

static void CreatePokeguideIcon(u16 species, s16 x, s16 y)
{
    u8 idx = sStartMenuDataPtr->pokeguideIconCount;
    if (idx >= POKEGUIDE_MAX_TOTAL)
        return;

    u8 id = CreateMonIconNoPersonality(species, SpriteCallbackDummy, x, y, 0);
    if (id >= MAX_SPRITES)
        return;

    // Free the matrix CreateMonIconNoPersonality allocated (we don't need affine here).
    if (gSprites[id].oam.affineMode != ST_OAM_AFFINE_OFF)
        FreeOamMatrix(gSprites[id].oam.matrixNum);
    gSprites[id].oam.affineMode = ST_OAM_AFFINE_OFF;
    gSprites[id].oam.priority   = 2;
    gSprites[id].animPaused     = TRUE;
    CalcCenterToCornerVec(&gSprites[id], gSprites[id].oam.shape, gSprites[id].oam.size, ST_OAM_AFFINE_OFF);

    sStartMenuDataPtr->pokeguideIconSpriteIds[idx] = id;
    sStartMenuDataPtr->pokeguideIconCount++;
}

// -------- PokéGuide cursor and info panel helpers --------

// Screen positions for the right-page info panel elements:
#define PG_SPRITE_X         155   // top-left x of 64x64 front sprite (right page, centered)
#define PG_SPRITE_Y           72   // top-left y of 64x64 front sprite
#define PG_TYPE1_VIS_X      210   // type stamp 1 visual center x
#define PG_TYPE2_VIS_X      226   // type stamp 2 visual center x
#define PG_TYPE_VIS_Y        148   // type stamp visual center y (below sprite)
#define PG_MON_PIC_PAL_SLOT  14   // OBJ palette slot for the pokeguide front sprite

static void PgDestroyTypeIcons(void)
{
    u8 i;
    for (i = 0; i < 2; i++)
    {
        if (sPgTypeIconSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sPgTypeIconSpriteIds[i]]);
            sPgTypeIconSpriteIds[i] = SPRITE_NONE;
        }
    }
}

// known=FALSE shows the type_unknown placeholder centered; known=TRUE shows real types.
static void PgCreateOrUpdateTypeIcons(u16 species, bool8 known)
{
    struct Sprite *sprite;
    const struct SpriteTemplate *tmpl = known ? &sSpriteTemplate_TypeStamps_Pg
                                               : &sSpriteTemplate_PgTypeUnknown;

    if (sPgTypeIconSpriteIds[0] == SPRITE_NONE)
    {
        u8 spriteId = CreateSprite(tmpl, PG_TYPE1_VIS_X - 8, PG_TYPE_VIS_Y - 8, 1);
        if (spriteId == MAX_SPRITES)
            return;
        sPgTypeIconSpriteIds[0] = spriteId;
    }
    sprite = &gSprites[sPgTypeIconSpriteIds[0]];
    sprite->y = PG_TYPE_VIS_Y - 8;
    sprite->invisible = FALSE;
    sprite->oam.priority = 2;

    if (!known)
    {
        // Single unknown stamp centered between the two type positions
        sprite->x = (PG_TYPE1_VIS_X + PG_TYPE2_VIS_X) / 2 - 8;
        // Hide second slot
        if (sPgTypeIconSpriteIds[1] != SPRITE_NONE)
            gSprites[sPgTypeIconSpriteIds[1]].invisible = TRUE;
        return;
    }

    // known: show real types
    {
        u8 type1 = GetSpeciesType(species, 0);
        u8 type2 = GetSpeciesType(species, 1);
        bool8 hasTwoTypes = (type1 != type2);

        sprite->x = (hasTwoTypes ? PG_TYPE1_VIS_X : (PG_TYPE1_VIS_X + PG_TYPE2_VIS_X) / 2) - 8;
        StartSpriteAnim(sprite, sTypeStampAnim[type1]);

        if (sPgTypeIconSpriteIds[1] == SPRITE_NONE)
        {
            u8 spriteId = CreateSprite(&sSpriteTemplate_TypeStamps_Pg,
                                       PG_TYPE2_VIS_X - 8, PG_TYPE_VIS_Y - 8, 1);
            if (spriteId == MAX_SPRITES)
                return;
            sPgTypeIconSpriteIds[1] = spriteId;
        }
        sprite = &gSprites[sPgTypeIconSpriteIds[1]];
        sprite->x = PG_TYPE2_VIS_X - 8;
        sprite->y = PG_TYPE_VIS_Y - 8;
        sprite->invisible = !hasTwoTypes;
        sprite->oam.priority = 2;
        if (hasTwoTypes)
            StartSpriteAnim(sprite, sTypeStampAnim[type2]);
    }
}

static void PgDestroyCaughtBadge(void)
{
    if (sPgCaughtSpriteId != SPRITE_NONE)
    {
        DestroySprite(&gSprites[sPgCaughtSpriteId]);
        sPgCaughtSpriteId = SPRITE_NONE;
    }
}

static void PgDestroyCaughtAllBadge(void)
{
    if (sPgCaughtAllSpriteId != SPRITE_NONE)
    {
        DestroySprite(&gSprites[sPgCaughtAllSpriteId]);
        sPgCaughtAllSpriteId = SPRITE_NONE;
    }
}

static void PgDestroyEvSprites(void)
{
    u8 i;
    for (i = 0; i < 6; i++)
    {
        if (sPgEvSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sPgEvSpriteIds[i]]);
            sPgEvSpriteIds[i] = SPRITE_NONE;
        }
    }
}

static void PgDestroyStarSprites(void)
{
    u8 i;
    for (i = 0; i < 3; i++)
    {
        if (sPgStarSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sPgStarSpriteIds[i]]);
            sPgStarSpriteIds[i] = SPRITE_NONE;
        }
    }
}

// Sepia levels for the front sprite
#define PG_SEPIA_LIGHT   0   // caught: slight warm tint, mostly full color
#define PG_SEPIA_MEDIUM  1   // seen but not caught: aged photo look
#define PG_SEPIA_HEAVY   2   // unseen: strong faded/dark sepia

// Blend original palette color toward (82,0,0) = GBA(10,0,0) at the given level.
// blend (0-16): how much tint vs original.  dark (0-16): brightness scaler after blend.
static void PgSepiaObjPaletteLevel(u8 paletteNum, u8 level)
{
    static const u8 sBlend[3] = { 0,  5, 16 };  // caught: off; seen: subtle; unseen: full
    static const u8 sDark[3]  = { 16, 16,  6 };  // heavy: ~37% brightness = dark silhouette

    u32 blend = sBlend[level];
    u32 dark  = sDark[level];

    u16 *unfaded = gPlttBufferUnfaded + 256 + paletteNum * 16;
    u16 *faded   = gPlttBufferFaded   + 256 + paletteNum * 16;
    u16 *pal = unfaded + 1;  // skip index 0 (transparent)
    u32 i;

    for (i = 0; i < 15; i++)
    {
        u16 color = pal[i];
        s32 origR = (color      ) & 0x1F;
        s32 origG = (color >>  5) & 0x1F;
        s32 origB = (color >> 10) & 0x1F;

        // Lerp original -> (10,0,0) by blend/16
        s32 r = (origR * (16 - blend) + 10 * blend) >> 4;
        s32 g = (origG * (16 - blend)             ) >> 4;
        s32 b = (origB * (16 - blend)             ) >> 4;

        // Darken for heavy level
        r = (r * dark) >> 4;
        g = (g * dark) >> 4;
        b = (b * dark) >> 4;

        pal[i] = (u16)((b << 10) | (g << 5) | r);
    }

    CpuCopy16(unfaded, faded, 16 * sizeof(u16));
}

// Apply a red tint (82,0,0) to an OBJ palette slot (skips index 0, transparent).
// Lerps each color toward (10,0,0) in GBA 5-bit scale by ITEM_ICON_SEPIA_BLEND16/16.
// Modifies both unfaded and faded buffers so the tint survives UpdatePaletteFade.
static void PgSepiaObjPalette(u8 paletteNum)
{
    u16 *unfaded = gPlttBufferUnfaded + 256 + paletteNum * 16;
    u16 *faded   = gPlttBufferFaded   + 256 + paletteNum * 16;
    u16 *pal = unfaded + 1; // skip index 0 (transparent)
    u8 blend16 = ITEM_ICON_SEPIA_BLEND16;
    u32 i;
    for (i = 0; i < 15; i++)
    {
        u16 color = pal[i];
        s32 origR = (color      ) & 0x1F;
        s32 origG = (color >>  5) & 0x1F;
        s32 origB = (color >> 10) & 0x1F;
        // Target: RGB(82,0,0) = GBA(10,0,0)
        s32 r = (origR * (16 - blend16) + 10 * blend16) >> 4;
        s32 g = (origG * (16 - blend16)               ) >> 4;
        s32 b = (origB * (16 - blend16)               ) >> 4;
        pal[i] = (u16)((b << 10) | (g << 5) | r);
    }
    CpuCopy16(unfaded, faded, 16 * sizeof(u16));
}

// Same but blends only partway toward sepia. blend16: 0 = no change, 16 = full sepia.
static void PgSepiaObjPalettePartial(u8 paletteNum, u8 blend16)
{
    u16 *unfaded = gPlttBufferUnfaded + 256 + paletteNum * 16;
    u16 *faded   = gPlttBufferFaded   + 256 + paletteNum * 16;
    u16 *pal = unfaded + 1;
    u32 i;
    for (i = 0; i < 15; i++)
    {
        u16 color = pal[i];
        s32 origR = (color      ) & 0x1F;
        s32 origG = (color >>  5) & 0x1F;
        s32 origB = (color >> 10) & 0x1F;
        s32 y = (origR * 77 + origG * 151 + origB * 29) >> 8;
        s32 sepR = (307 * y) >> 8; if (sepR > 31) sepR = 31;
        s32 sepG = y;
        s32 sepB = (241 * y) >> 8;
        s32 r = (origR * (16 - blend16) + sepR * blend16) >> 4;
        s32 g = (origG * (16 - blend16) + sepG * blend16) >> 4;
        s32 b = (origB * (16 - blend16) + sepB * blend16) >> 4;
        pal[i] = (u16)((b << 10) | (g << 5) | r);
    }
    CpuCopy16(unfaded, faded, 16 * sizeof(u16));
}

static void PgDestroyWildItemIcons(void)
{
    u8 i;
    for (i = 0; i < 2; i++)
    {
        if (sPgWildItemSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sPgWildItemSpriteIds[i]]);
            sPgWildItemSpriteIds[i] = SPRITE_NONE;
        }
    }
    FreeSpriteTilesByTag(TAG_PG_WILD_ITEM1_TILES);
    FreeSpriteTilesByTag(TAG_PG_WILD_ITEM2_TILES);
    // Palettes are freed separately in PgUpdateRightPage immediately before
    // new palettes are loaded, so hardware sees no gap frame with a zeroed slot.
}

static bool8 PgAllSpeciesCaught(void)
{
    u8 i;
    if (sPokeguideTotal == 0)
        return FALSE;
    for (i = 0; i < sPokeguideTotal; i++)
    {
        u32 dexNum = SpeciesToNationalPokedexNum(sPokeguideSpeciesList[i]);
        if (!GetSetPokedexFlag(dexNum, FLAG_GET_CAUGHT))
            return FALSE;
    }
    return TRUE;
}

static void PgCreateCaughtAllBadge(void)
{
    if (!PgAllSpeciesCaught())
        return;
    u8 id = CreateSprite(&sSpriteTemplate_PgCaughtAll, PG_CAUGHT_ALL_CTR_X, PG_CAUGHT_ALL_CTR_Y, 0);
    if (id < MAX_SPRITES)
        sPgCaughtAllSpriteId = id;
}

static void PgUpdateEvAndStars(u16 species)
{
    static const struct SpriteTemplate *const sEvTemplates[4] = {
        NULL,
        &sSpriteTemplate_PgEvOk,
        &sSpriteTemplate_PgEvGood,
        &sSpriteTemplate_PgEvGreat,
    };
    static const u32 sStarThresholds[3] = { PG_STAR_THRESH_1, PG_STAR_THRESH_2, PG_STAR_THRESH_3 };
    u8 evValues[6];
    u8 i;

    evValues[0] = gSpeciesInfo[species].evYield_HP;
    evValues[1] = gSpeciesInfo[species].evYield_Attack;
    evValues[2] = gSpeciesInfo[species].evYield_Defense;
    evValues[3] = gSpeciesInfo[species].evYield_SpAttack;
    evValues[4] = gSpeciesInfo[species].evYield_SpDefense;
    evValues[5] = gSpeciesInfo[species].evYield_Speed;

    for (i = 0; i < 6; i++)
    {
        u8 ev = evValues[i];
        if (ev > 0 && ev <= 3)
        {
            s16 cy = PG_EV_CENTER_Y0 + i * PG_EV_STEP;
            u8 id = CreateSprite(sEvTemplates[ev], PG_EV_CENTER_X, cy, 0);
            if (id < MAX_SPRITES)
                sPgEvSpriteIds[i] = id;
        }
    }

    {
        u32 searchLvl = gSaveBlock3Ptr->dexNavSearchLevels[species];
        for (i = 0; i < 3; i++)
        {
            if (searchLvl >= sStarThresholds[i])
            {
                s16 sx = PG_STAR_CENTER_X0 + i * PG_STAR_STEP;
                u8 id = CreateSprite(&sSpriteTemplate_PgStar, sx, PG_STAR_CENTER_Y, 0);
                if (id < MAX_SPRITES)
                    sPgStarSpriteIds[i] = id;
            }
        }
    }
}

// Hide or show all right-page pokeguide sprites (mon pic, type icons, badges, EVs, stars, items).
// Used to ensure BG confirmation windows appear in front of sprites.
static void PgSetRightPageSpritesBehindBg(bool8 behind)
{
    u8 i;
    u8 priority = behind ? 1 : 2;
    if (sPgMonPicSpriteId != SPRITE_NONE)
        gSprites[sPgMonPicSpriteId].oam.priority = priority;
    for (i = 0; i < 2; i++)
        if (sPgTypeIconSpriteIds[i] != SPRITE_NONE)
            gSprites[sPgTypeIconSpriteIds[i]].oam.priority = priority;
    if (sPgCaughtSpriteId != SPRITE_NONE)
        gSprites[sPgCaughtSpriteId].oam.priority = priority;
    if (sPgCaughtAllSpriteId != SPRITE_NONE)
        gSprites[sPgCaughtAllSpriteId].oam.priority = priority;
    for (i = 0; i < 6; i++)
        if (sPgEvSpriteIds[i] != SPRITE_NONE)
            gSprites[sPgEvSpriteIds[i]].oam.priority = priority;
    for (i = 0; i < 3; i++)
        if (sPgStarSpriteIds[i] != SPRITE_NONE)
            gSprites[sPgStarSpriteIds[i]].oam.priority = priority;
    for (i = 0; i < 2; i++)
        if (sPgWildItemSpriteIds[i] != SPRITE_NONE)
            gSprites[sPgWildItemSpriteIds[i]].oam.priority = priority;
}

static void PgDestroyMonPic(void)
{
    if (sPgMonPicSpriteId != SPRITE_NONE)
    {
        if (gSprites[sPgMonPicSpriteId].oam.affineMode != ST_OAM_AFFINE_OFF)
            FreeOamMatrix(gSprites[sPgMonPicSpriteId].oam.matrixNum);
        FreeAndDestroyMonPicSprite(sPgMonPicSpriteId);
        sPgMonPicSpriteId = SPRITE_NONE;
    }
}

static void PgClearRightPage(void)
{
    PgDestroyMonPic();
    PgDestroyTypeIcons();
    PgDestroyCaughtBadge();
    PgDestroyEvSprites();
    PgDestroyStarSprites();
    PgDestroyWildItemIcons();
    FillWindowPixelBuffer(WIN_PG_INFO, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_PG_INFO);
    CopyWindowToVram(WIN_PG_INFO, COPYWIN_FULL);
    FillWindowPixelBuffer(WIN_PG_CATEGORY, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_PG_CATEGORY);
    CopyWindowToVram(WIN_PG_CATEGORY, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

static void PgUpdateRightPage(u8 idx)
{
    u8 buf[64];
    u8 *ptr;

    PgClearRightPage();

    if (idx >= sPokeguideTotal)
        return;

    u16 species = sPokeguideSpeciesList[idx];
    u32 dexNum  = SpeciesToNationalPokedexNum(species);
    bool8 seen   = GetSetPokedexFlag(dexNum, FLAG_GET_SEEN);
    bool8 caught = GetSetPokedexFlag(dexNum, FLAG_GET_CAUGHT);

    // Front sprite: always shown, sepia level depends on dex status
    {
        u16 picSpriteId = CreateMonPicSprite_Affine(
            species, FALSE, 0, MON_PIC_AFFINE_FRONT,
            PG_SPRITE_X, PG_SPRITE_Y, PG_MON_PIC_PAL_SLOT, TAG_NONE);
        sPgMonPicSpriteId = (picSpriteId == 0xFFFF) ? SPRITE_NONE : (u8)picSpriteId;
        if (sPgMonPicSpriteId != SPRITE_NONE)
        {
            u8 sepiaLevel = caught ? PG_SEPIA_LIGHT : (seen ? PG_SEPIA_MEDIUM : PG_SEPIA_HEAVY);
            gSprites[sPgMonPicSpriteId].oam.priority = 2;
            gSprites[sPgMonPicSpriteId].oam.mosaic   = (!seen && !caught) ? TRUE : FALSE;
            PgSepiaObjPaletteLevel(PG_MON_PIC_PAL_SLOT, sepiaLevel);
        }
    }

    // Type stamps: always shown; unknown placeholder if unseen
    PgCreateOrUpdateTypeIcons(species, seen || caught);

    // Caught badge: top-right of right page (only if caught)
    if (caught)
    {
        u8 id = CreateSprite(&sSpriteTemplate_PgCaught, PG_CAUGHT_CENTER_X, PG_CAUGHT_CENTER_Y, 0);
        if (id < MAX_SPRITES)
            sPgCaughtSpriteId = id;
    }

    // EV yield sprites and search level stars (shown if seen or caught)
    if (seen || caught)
        PgUpdateEvAndStars(species);

    // Info text window
    FillWindowPixelBuffer(WIN_PG_INFO, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Line 0: species name (real name if seen/caught, else "???")
    if (seen || caught)
        StringCopy(buf, GetSpeciesName(species));
    else
        StringCopy(buf, gText_ThreeQuestionMarks);
    AddTextPrinterParameterized3(WIN_PG_INFO, FONT_SMALL_NARROWER, 6, 6, sFontColorTable[0], 0, buf);

    // Line 1: hidden ability if caught; "Catch First!" if not yet caught
    {
        static const u8 sText_HA[]   = _(" ");
        static const u8 sText_None[] = _("None");
        if (caught)
        {
            enum Ability hiddenAbility = GetSpeciesAbility(species, NUM_NORMAL_ABILITY_SLOTS);
            const u8 *haName = (hiddenAbility != ABILITY_NONE)
                                    ? gAbilitiesInfo[hiddenAbility].name
                                    : sText_None;
            ptr = StringCopy(buf, sText_HA);
            StringCopy(ptr, haName);
        }
        else
        {
            StringCopy(buf, sText_CatchFirst);
        }
        AddTextPrinterParameterized3(WIN_PG_INFO, FONT_SMALL_NARROWER, 16, 18, sFontColorTable[0], 0, buf);
    }

    PutWindowTilemap(WIN_PG_INFO);
    CopyWindowToVram(WIN_PG_INFO, COPYWIN_FULL);

    // Category text: two centered lines below front sprite (only if seen or caught)
    const u8 *catName = (seen || caught) ? gSpeciesInfo[species].categoryName : sText_CatchFirst;
    s32 catX  = (80 - GetStringWidth(FONT_SMALL_NARROWER, catName,       0)) / 2;
    if (catX < 0) catX = 0;
    FillWindowPixelBuffer(WIN_PG_CATEGORY, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    AddTextPrinterParameterized3(WIN_PG_CATEGORY, FONT_SMALL_NARROWER, catX+4, 4,
                                 sFontColorTable[0], 0, catName);
    PutWindowTilemap(WIN_PG_CATEGORY);
    CopyWindowToVram(WIN_PG_CATEGORY, COPYWIN_FULL);


    // Wild held item icons: always show something.
    // If caught: show real item (or nothing if ITEM_NONE). If not caught: show unknown placeholder.
    {
        static const u16 sTileTags[2] = { TAG_PG_WILD_ITEM1_TILES, TAG_PG_WILD_ITEM2_TILES };
        static const u16 sPalTags[2]  = { TAG_PG_WILD_ITEM1_PAL,   TAG_PG_WILD_ITEM2_PAL };
        // Free old palettes here (not in PgDestroyWildItemIcons) so the new palette
        // is loaded in the same callback with no VBlank gap between free and load.
        FreeSpritePaletteByTag(TAG_PG_WILD_ITEM1_PAL);
        FreeSpritePaletteByTag(TAG_PG_WILD_ITEM2_PAL);
        static const s16 sItemX[2]    = { 122, 156 };
        u16 items[2];
        u8 i;
        items[0] = gSpeciesInfo[species].itemCommon;
        items[1] = gSpeciesInfo[species].itemRare;
        if (!caught)
        {
            // Single centered unknown icon, shifted up 8px from normal item position
            u8 id = CreateSprite(&sSpriteTemplate_PgTypeUnknown, 149, 140, 2);
            if (id < MAX_SPRITES)
            {
                gSprites[id].oam.priority = 2;
                sPgWildItemSpriteIds[0] = id;
            }
        }
        if (caught)
        {
            for (i = 0; i < 2; i++)
            {
                u8 id;
                if (items[i] == ITEM_NONE)
                    continue;
                id = AddItemIconSprite(sTileTags[i], sPalTags[i], items[i]);
                if (id >= MAX_SPRITES)
                    continue;
                gSprites[id].x = sItemX[i] + 16;
                gSprites[id].y = 122 + 16 + 8 - 5;
                gSprites[id].oam.priority = 2;
                sPgWildItemSpriteIds[i] = id;
                if (PG_WILD_ITEM_SEPIA)
                    PgSepiaObjPalette(gSprites[id].oam.paletteNum);
            }
        }
    }

    ScheduleBgCopyTilemapToVram(0);
}

// Move the cursor to slot `idx`, update sprite position and right page.
static void PgMoveCursor(u8 idx)
{
    sPgCursorIdx = idx;

    // Position cursor sprite over the selected grid slot
    if (sPgCursorSpriteId != SPRITE_NONE)
    {
        s16 rowY0 = sStartMenuDataPtr->isOnWater ? PG_WATER_Y0 : PG_LAND_Y0;
        gSprites[sPgCursorSpriteId].x = sPgColX[idx % 3]+4;
        gSprites[sPgCursorSpriteId].y = rowY0 + (idx / 3) * PG_ROW_STEP+9;
    }
    PgUpdateRightPage(idx);
}

static void LoadPokeGuideContent(void)
{
    u8 i;
    u8 maxCount = sStartMenuDataPtr->isOnWater ? POKEGUIDE_MAX_WATER : POKEGUIDE_MAX_LAND;

    // Enable 3x3 OBJ mosaic for "seen but not caught" sprites on this tab.
    // OBJ H/V fields are bits 8-11 / 12-15; value 2 = 3-pixel block.
    // BG mosaic bits (0-7) left at 0.
    SetGpuReg(REG_OFFSET_MOSAIC, (1 << 12) | (1 << 8));

    // Route name on right portion of left page
    FillWindowPixelBuffer(WIN_PG_ROUTE, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    {
        u8 nameBuf[32];
        GetMapNameGeneric(nameBuf, gMapHeader.regionMapSectionId);
        AddTextPrinterParameterized3(WIN_PG_ROUTE, FONT_SMALL_NARROWER,
                                     2, 4, sFontColorTable[0], 0, nameBuf);
    }
    PutWindowTilemap(WIN_PG_ROUTE);
    CopyWindowToVram(WIN_PG_ROUTE, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);

    u8 total = BuildPokeguideSpeciesList();

    // Load palettes and sprite sheets for grid icons
    LoadMonIconPalettes();
    LoadSpriteSheet(&sTypeStampsSpriteSheet);
    LoadSpritePalette(&sTypeStampsSpritePalette);
    LoadSpritePalette(&sSpritePal_PgBlack);

    // Pokeguide cursor: 32x32 PNG, 16 tiles, load directly
    {
        struct SpriteSheet pgCursorSheet = { sPgCursor_Gfx, sizeof(sPgCursor_Gfx), TAG_PG_CURSOR_TILES };
        LoadSpriteSheet(&pgCursorSheet);
    }
    {
        struct SpritePalette pgCursorPal = { sPgCursor_Pal, TAG_PG_CURSOR_PAL };
        LoadSpritePalette(&pgCursorPal);
    }

    // Null slot icon sheet
    {
        struct SpriteSheet pgNullSheet = { sPgNull_Gfx, sizeof(sPgNull_Gfx), TAG_PG_NULL_TILES };
        LoadSpriteSheet(&pgNullSheet);
    }

    // New overlay sprite sheets (all share cursor palette already loaded)
    {
        struct SpriteSheet sh;
        sh = (struct SpriteSheet){ sPgCaught_Gfx,    sizeof(sPgCaught_Gfx),    TAG_PG_CAUGHT_TILES };
        LoadSpriteSheet(&sh);
        sh = (struct SpriteSheet){ sPgCaughtAll_Gfx, sizeof(sPgCaughtAll_Gfx), TAG_PG_CAUGHT_ALL_TILES };
        LoadSpriteSheet(&sh);
        sh = (struct SpriteSheet){ sPgEvOk_Gfx,      sizeof(sPgEvOk_Gfx),      TAG_PG_EV_OK_TILES };
        LoadSpriteSheet(&sh);
        sh = (struct SpriteSheet){ sPgEvGood_Gfx,    sizeof(sPgEvGood_Gfx),    TAG_PG_EV_GOOD_TILES };
        LoadSpriteSheet(&sh);
        sh = (struct SpriteSheet){ sPgEvGreat_Gfx,   sizeof(sPgEvGreat_Gfx),   TAG_PG_EV_GREAT_TILES };
        LoadSpriteSheet(&sh);
        sh = (struct SpriteSheet){ sPgStar_Gfx,         sizeof(sPgStar_Gfx),         TAG_PG_STAR_TILES };
        LoadSpriteSheet(&sh);
        sh = (struct SpriteSheet){ sPgTypeUnknown_Gfx, sizeof(sPgTypeUnknown_Gfx), TAG_PG_TYPE_UNKNOWN_TILES };
        LoadSpriteSheet(&sh);
        sh = (struct SpriteSheet){ sPgItemUnknown_Gfx, sizeof(sPgItemUnknown_Gfx), TAG_PG_ITEM_UNKNOWN_TILES };
        LoadSpriteSheet(&sh);
    }

    // Build mon icon grid
    sStartMenuDataPtr->pokeguideIconCount = 0;
    sStartMenuDataPtr->sharedMatrix = 0xFF; // unused (icons use ST_OAM_AFFINE_OFF)

    sPgCursorIdx = 0;
    sPgCursorSpriteId = SPRITE_NONE;
    sPgMonPicSpriteId = SPRITE_NONE;
    sPgTypeIconSpriteIds[0] = SPRITE_NONE;
    sPgTypeIconSpriteIds[1] = SPRITE_NONE;
    sPgCaughtSpriteId = SPRITE_NONE;
    sPgCaughtAllSpriteId = SPRITE_NONE;
    for (i = 0; i < 9; i++)
        sPgNullSpriteIds[i] = SPRITE_NONE;
    for (i = 0; i < 6; i++)
        sPgEvSpriteIds[i] = SPRITE_NONE;
    for (i = 0; i < 3; i++)
        sPgStarSpriteIds[i] = SPRITE_NONE;
    sPgWildItemSpriteIds[0] = SPRITE_NONE;
    sPgWildItemSpriteIds[1] = SPRITE_NONE;

    s16 rowY0 = sStartMenuDataPtr->isOnWater ? PG_WATER_Y0 : PG_LAND_Y0;
    u8 blackPalSlot = IndexOfSpritePaletteTag(TAG_PG_BLACK_PAL);

    for (i = 0; i < maxCount; i++)
    {
        s16 x = sPgColX[i % 3];
        s16 y = rowY0 + (i / 3) * PG_ROW_STEP;

        if (i < total)
        {
            u16 species = sPokeguideSpeciesList[i];
            CreatePokeguideIcon(species, x, y);
            // Apply black silhouette palette if species not yet seen or caught
            {
                u8 id = sStartMenuDataPtr->pokeguideIconSpriteIds[sStartMenuDataPtr->pokeguideIconCount - 1];
                u32 dexNum = SpeciesToNationalPokedexNum(species);
                bool8 seen = GetSetPokedexFlag(dexNum, FLAG_GET_SEEN);
                bool8 caught = GetSetPokedexFlag(dexNum, FLAG_GET_CAUGHT);
                if (!seen && !caught && id < MAX_SPRITES)
                    gSprites[id].oam.paletteNum = blackPalSlot;
            }
        }
        else
        {
            // Empty slot: show null sprite
            u8 id = CreateSprite(&sSpriteTemplate_PgNull, x, y + 4, 0);
            if (id < MAX_SPRITES)
            {
                gSprites[id].oam.priority = 2;
                sPgNullSpriteIds[i] = id;
            }
        }
    }

    // Caught-all badge on left page if every species in the list is caught
    PgCreateCaughtAllBadge();

    // Create cursor at first valid slot (slot 0 always has a real species if total > 0)
    if (total > 0)
    {
        u8 cursorId = CreateSprite(&sSpriteTemplate_PgCursor, sPgColX[0]+3, rowY0+9, 0);
        if (cursorId < MAX_SPRITES)
        {
            gSprites[cursorId].oam.priority = 3;  // behind mon icons (priority 2)
            sPgCursorSpriteId = cursorId;
        }
        PgUpdateRightPage(0);
    }
}

static void UnloadPokeGuideContent(void)
{
    u8 i;
    if (sStartMenuDataPtr == NULL)
        return;

    // Destroy pokeguide mon pic and type icons (right page)
    PgDestroyMonPic();
    PgDestroyTypeIcons();
    PgDestroyCaughtBadge();
    PgDestroyCaughtAllBadge();
    PgDestroyEvSprites();
    PgDestroyStarSprites();
    PgDestroyWildItemIcons();

    // Destroy cursor sprite
    if (sPgCursorSpriteId != SPRITE_NONE)
    {
        DestroySprite(&gSprites[sPgCursorSpriteId]);
        sPgCursorSpriteId = SPRITE_NONE;
    }

    // Destroy null sprites
    for (i = 0; i < 9; i++)
    {
        if (sPgNullSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sPgNullSpriteIds[i]]);
            sPgNullSpriteIds[i] = SPRITE_NONE;
        }
    }

    // Destroy mon icon grid
    FreeMonIconPalettes();
    for (i = 0; i < sStartMenuDataPtr->pokeguideIconCount; i++)
    {
        u8 id = sStartMenuDataPtr->pokeguideIconSpriteIds[i];
        if (id < MAX_SPRITES)
        {
            gSprites[id].oam.affineMode = ST_OAM_AFFINE_OFF;
            FreeAndDestroyMonIconSprite(&gSprites[id]);
        }
        sStartMenuDataPtr->pokeguideIconSpriteIds[i] = SPRITE_NONE;
    }
    sStartMenuDataPtr->pokeguideIconCount = 0;
    sPokeguideTotal = 0;
    sStartMenuDataPtr->sharedMatrix = 0xFF;

    // Free pokeguide-specific sheets and palettes
    FreeSpriteTilesByTag(TAG_PG_CURSOR_TILES);
    FreeSpritePaletteByTag(TAG_PG_CURSOR_PAL);
    FreeSpriteTilesByTag(TAG_PG_NULL_TILES);
    FreeSpritePaletteByTag(TAG_PG_BLACK_PAL);
    FreeSpriteTilesByTag(TAG_PG_CAUGHT_TILES);
    FreeSpriteTilesByTag(TAG_PG_CAUGHT_ALL_TILES);
    FreeSpriteTilesByTag(TAG_PG_EV_OK_TILES);
    FreeSpriteTilesByTag(TAG_PG_EV_GOOD_TILES);
    FreeSpriteTilesByTag(TAG_PG_EV_GREAT_TILES);
    FreeSpriteTilesByTag(TAG_PG_STAR_TILES);
    FreeSpriteTilesByTag(TAG_PG_TYPE_UNKNOWN_TILES);
    FreeSpriteTilesByTag(TAG_PG_ITEM_UNKNOWN_TILES);
    FreeSpriteTilesByTag(TAG_PG_WILD_ITEM1_TILES);
    FreeSpritePaletteByTag(TAG_PG_WILD_ITEM1_PAL);
    FreeSpriteTilesByTag(TAG_PG_WILD_ITEM2_TILES);
    FreeSpritePaletteByTag(TAG_PG_WILD_ITEM2_PAL);
    FreeSpriteTilesByTag(TAG_TYPE_STAMPS_TILES);
    FreeSpritePaletteByTag(TAG_TYPE_STAMPS_PAL);

    // Clear OBJ mosaic set for this tab
    SetGpuReg(REG_OFFSET_MOSAIC, 0);

    // Clear text windows
    FillWindowPixelBuffer(WIN_PG_ROUTE,    PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_PG_INFO,     PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_PG_CATEGORY, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_PG_ROUTE);
    ClearWindowTilemap(WIN_PG_INFO);
    ClearWindowTilemap(WIN_PG_CATEGORY);
    CopyWindowToVram(WIN_PG_ROUTE,    COPYWIN_FULL);
    CopyWindowToVram(WIN_PG_INFO,     COPYWIN_FULL);
    CopyWindowToVram(WIN_PG_CATEGORY, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

// ==========================================
// MINERALS TAB working todo/add button to start mining?
// ==========================================

static void LoadMineralsContent(void)
{
    u16 common[MINING_DISPLAY_MAX_COMMON], uncommon[MINING_DISPLAY_MAX_UNCOMMON], rare[MINING_DISPLAY_MAX_RARE];
    u8 commonCount, uncommonCount, rareCount;
    u8 i;

    // Clear shared text windows
    FillWindowPixelBuffer(WIN_LEFT_NAME,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_RIGHT_LVS,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_LEFT_NAME);
    ClearWindowTilemap(WIN_LEFT_MOVES_ALL);
    ClearWindowTilemap(WIN_RIGHT_LVS);
    CopyWindowToVram(WIN_LEFT_NAME,      COPYWIN_FULL);
    CopyWindowToVram(WIN_LEFT_MOVES_ALL, COPYWIN_FULL);
    CopyWindowToVram(WIN_RIGHT_LVS,      COPYWIN_FULL);

    // Route name in the upper-right corner of the right page
    FillWindowPixelBuffer(WIN_ROUTE, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    {
        u8 nameBuf[32];
        GetMapNameGeneric(nameBuf, gMapHeader.regionMapSectionId);
        AddTextPrinterParameterized3(WIN_ROUTE, FONT_SMALL_NARROWER,
                                     6, 4, sFontColorTable[0], 0, nameBuf);
    }
    PutWindowTilemap(WIN_ROUTE);
    CopyWindowToVram(WIN_ROUTE, COPYWIN_FULL);

    ScheduleBgCopyTilemapToVram(0);

    // Item icon grid - same positions as scavenger tab
    sStartMenuDataPtr->scavengeIconCount = 0;
    GetMiningDisplayItems(
        common,   &commonCount,
        uncommon, &uncommonCount,
        rare,     &rareCount,
        MINING_DISPLAY_MAX_COMMON, MINING_DISPLAY_MAX_UNCOMMON, MINING_DISPLAY_MAX_RARE);

    // Common: left page upper (3 cols x ceil(MINING_DISPLAY_MAX_COMMON/3) rows)
    for (i = 0; i < commonCount && i < MINING_DISPLAY_MAX_COMMON; i++)
    {
        s16 x = SCAVENGE_COMMON_X0 + (i % 3) * SCAVENGE_ICON_STEP_X;
        s16 y = SCAVENGE_COMMON_Y0 + (i / 3) * SCAVENGE_ICON_STEP_Y;
        AddScavengeItemIcon(common[i], x, y);
    }

    // Uncommon: left page lower (3 cols x ceil(MINING_DISPLAY_MAX_UNCOMMON/3) rows)
    for (i = 0; i < uncommonCount && i < MINING_DISPLAY_MAX_UNCOMMON; i++)
    {
        s16 x = SCAVENGE_UNCOMMON_X0 + (i % 3) * SCAVENGE_ICON_STEP_X;
        s16 y = SCAVENGE_UNCOMMON_Y0 + (i / 3) * SCAVENGE_ICON_STEP_Y;
        AddScavengeItemIcon(uncommon[i], x, y);
    }

    // Rare: right page below route name (3 cols x 1 row)
    for (i = 0; i < rareCount && i < MINING_DISPLAY_MAX_RARE; i++)
    {
        s16 x = SCAVENGE_RARE_X0 + (i % 3) * SCAVENGE_ICON_STEP_X;
        s16 y = SCAVENGE_RARE_Y0 + (i / 3) * SCAVENGE_ICON_STEP_Y;
        AddScavengeItemIcon(rare[i], x, y);
    }
}

static void UnloadMineralsContent(void)
{
    u8 i;
    if (sStartMenuDataPtr == NULL)
        return;

    for (i = 0; i < sStartMenuDataPtr->scavengeIconCount; i++)
    {
        u8 id = sStartMenuDataPtr->scavengeIconSpriteIds[i];
        if (id < MAX_SPRITES)
        {
            FreeSpriteTilesByTag(TAG_SCAVENGE_ITEM_TILES_BASE + i);
            FreeSpritePaletteByTag(TAG_SCAVENGE_ITEM_PAL_BASE + i);
            DestroySprite(&gSprites[id]);
        }
        sStartMenuDataPtr->scavengeIconSpriteIds[i] = SPRITE_NONE;
    }
    sStartMenuDataPtr->scavengeIconCount = 0;

    ClearWindowTilemap(WIN_ROUTE);
    ScheduleBgCopyTilemapToVram(0);
}

// ==========================================
// SCAVENGER TAB
// ==========================================

static void AddScavengeItemIcon(u16 itemId, s16 x, s16 y)
{
    if (sStartMenuDataPtr == NULL)
        return;
    u8 idx = sStartMenuDataPtr->scavengeIconCount;
    if (idx >= SCAVENGE_MAX_ICONS)
        return;

    u16 tilesTag = TAG_SCAVENGE_ITEM_TILES_BASE + idx;
    u16 palTag   = TAG_SCAVENGE_ITEM_PAL_BASE   + idx;
    u8 spriteId  = AddItemIconSprite(tilesTag, palTag, itemId);
    if (spriteId >= MAX_SPRITES)
        return;

    gSprites[spriteId].x = x + 16; // AddItemIconSprite origin is top-left; center at +16
    gSprites[spriteId].y = y + 16;
    gSprites[spriteId].oam.priority = 2;
    PgSepiaObjPalettePartial(IndexOfSpritePaletteTag(palTag), ITEM_ICON_SEPIA_BLEND16);
    sStartMenuDataPtr->scavengeIconSpriteIds[idx] = spriteId;
    sStartMenuDataPtr->scavengeIconCount++;
}

static void LoadScavengerContent(void)
{
    u16 common[MINING_DISPLAY_MAX_COMMON], uncommon[MINING_DISPLAY_MAX_UNCOMMON], rare[MINING_DISPLAY_MAX_RARE];
    u8 commonCount, uncommonCount, rareCount;
    u8 i;

    // Clear shared text windows
    FillWindowPixelBuffer(WIN_LEFT_NAME,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_RIGHT_LVS,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_LEFT_NAME);
    ClearWindowTilemap(WIN_LEFT_MOVES_ALL);
    ClearWindowTilemap(WIN_RIGHT_LVS);
    CopyWindowToVram(WIN_LEFT_NAME,      COPYWIN_FULL);
    CopyWindowToVram(WIN_LEFT_MOVES_ALL, COPYWIN_FULL);
    CopyWindowToVram(WIN_RIGHT_LVS,      COPYWIN_FULL);

    // Route name in the upper-right corner of the right page
    FillWindowPixelBuffer(WIN_ROUTE, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    {
        u8 nameBuf[32];
        GetMapNameGeneric(nameBuf, gMapHeader.regionMapSectionId);
        AddTextPrinterParameterized3(WIN_ROUTE, FONT_SMALL_NARROWER,
                                     6, 4, sFontColorTable[0], 0, nameBuf);
    }
    PutWindowTilemap(WIN_ROUTE);
    CopyWindowToVram(WIN_ROUTE, COPYWIN_FULL);

    ScheduleBgCopyTilemapToVram(0);

    // Item icon grid
    sStartMenuDataPtr->scavengeIconCount = 0;
    GetScavengeDisplayItems(
        common,   &commonCount,
        uncommon, &uncommonCount,
        rare,     &rareCount,
        MINING_DISPLAY_MAX_COMMON, MINING_DISPLAY_MAX_UNCOMMON, MINING_DISPLAY_MAX_RARE);

    // Common: left page upper (3 cols x ceil(MINING_DISPLAY_MAX_COMMON/3) rows)
    for (i = 0; i < commonCount && i < MINING_DISPLAY_MAX_COMMON; i++)
    {
        s16 x = SCAVENGE_COMMON_X0 + (i % 3) * SCAVENGE_ICON_STEP_X;
        s16 y = SCAVENGE_COMMON_Y0 + (i / 3) * SCAVENGE_ICON_STEP_Y;
        AddScavengeItemIcon(common[i], x, y);
    }

    // Uncommon: left page lower (3 cols x ceil(MINING_DISPLAY_MAX_UNCOMMON/3) rows)
    for (i = 0; i < uncommonCount && i < MINING_DISPLAY_MAX_UNCOMMON; i++)
    {
        s16 x = SCAVENGE_UNCOMMON_X0 + (i % 3) * SCAVENGE_ICON_STEP_X;
        s16 y = SCAVENGE_UNCOMMON_Y0 + (i / 3) * SCAVENGE_ICON_STEP_Y;
        AddScavengeItemIcon(uncommon[i], x, y);
    }

    // Rare: right page below route name (3 cols x 1 row)
    for (i = 0; i < rareCount && i < MINING_DISPLAY_MAX_RARE; i++)
    {
        s16 x = SCAVENGE_RARE_X0 + (i % 3) * SCAVENGE_ICON_STEP_X;
        s16 y = SCAVENGE_RARE_Y0 + (i / 3) * SCAVENGE_ICON_STEP_Y;
        AddScavengeItemIcon(rare[i], x, y);
    }
}

static void UnloadScavengerContent(void)
{
    u8 i;
    if (sStartMenuDataPtr == NULL)
        return;

    // Destroy all item icon sprites
    for (i = 0; i < sStartMenuDataPtr->scavengeIconCount; i++)
    {
        u8 id = sStartMenuDataPtr->scavengeIconSpriteIds[i];
        if (id < MAX_SPRITES)
        {
            FreeSpriteTilesByTag(TAG_SCAVENGE_ITEM_TILES_BASE + i);
            FreeSpritePaletteByTag(TAG_SCAVENGE_ITEM_PAL_BASE + i);
            DestroySprite(&gSprites[id]);
        }
        sStartMenuDataPtr->scavengeIconSpriteIds[i] = SPRITE_NONE;
    }
    sStartMenuDataPtr->scavengeIconCount = 0;

    // Clear the route name window
    ClearWindowTilemap(WIN_ROUTE);
    ScheduleBgCopyTilemapToVram(0);
}

// ==========================================
// TRAINER CARD TAB
// ==========================================

static void LoadTrainerCardContent(void)
{
    u8 buf[32];
    u8 *ptr;
    u8 i;
    static const u8 sText_Days[]  = _(" D, ");
    static const u8 sText_Hours[] = _(" H");
    static const u8 sText_Money[]      = _("¥");
    static const u8 sFontColors[] = {TEXT_COLOR_TRANSPARENT, TEXT_DYNAMIC_COLOR_1, TEXT_COLOR_TRANSPARENT};

    // Clear the shared party windows (in case we arrived from party tab without a full unload)
    FillWindowPixelBuffer(WIN_LEFT_NAME,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_RIGHT_LVS,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_LEFT_NAME);
    ClearWindowTilemap(WIN_LEFT_MOVES_ALL);
    ClearWindowTilemap(WIN_RIGHT_LVS);
    CopyWindowToVram(WIN_LEFT_NAME,      COPYWIN_FULL);
    CopyWindowToVram(WIN_LEFT_MOVES_ALL, COPYWIN_FULL);
    CopyWindowToVram(WIN_RIGHT_LVS,      COPYWIN_FULL);

    // ---- Left page (WIN_TC_LEFT: 112x96px at y=8) - all trainer info ----
    FillWindowPixelBuffer(WIN_TC_LEFT, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Row 0: Player name (left) + Trainer ID (right)
    StringCopy(buf, gSaveBlock2Ptr->playerName);
    AddTextPrinterParameterized3(WIN_TC_LEFT, FONT_NORMAL, 2, 8, sFontColors, 0, buf);

    ptr = buf;
    ptr = ConvertIntToDecimalStringN(ptr,
        (gSaveBlock2Ptr->playerTrainerId[1] << 8) | gSaveBlock2Ptr->playerTrainerId[0],
        STR_CONV_MODE_LEADING_ZEROS, 5);
    *ptr = EOS;
    AddTextPrinterParameterized3(WIN_TC_LEFT, FONT_NORMAL, 68, 9, sFontColors, 0, buf);

    // Row 1: Seen count
    ptr = buf;
    ptr = ConvertIntToDecimalStringN(ptr, GetNationalPokedexCount(FLAG_GET_SEEN), STR_CONV_MODE_LEFT_ALIGN, 4);
    *ptr = EOS;
    AddTextPrinterParameterized3(WIN_TC_LEFT, FONT_NORMAL, 2, 42, sFontColors, 0, buf);

    // Row 2: Caught count
    ptr = buf;
    ptr = ConvertIntToDecimalStringN(ptr, GetNationalPokedexCount(FLAG_GET_CAUGHT), STR_CONV_MODE_LEFT_ALIGN, 4);
    *ptr = EOS;
    AddTextPrinterParameterized3(WIN_TC_LEFT, FONT_NORMAL, 2, 62, sFontColors, 0, buf);

    // Row 3: Journey length as "X Days, Y Hrs"
    {
        u16 hours    = gSaveBlock2Ptr->playTimeHours;
        u32 days     = hours / 24;
        u16 remHours = hours % 24;
        ptr = buf;
        ptr = ConvertIntToDecimalStringN(ptr, days, STR_CONV_MODE_LEFT_ALIGN, 4);
        ptr = StringCopy(ptr, sText_Days);
        ptr = ConvertIntToDecimalStringN(ptr, remHours, STR_CONV_MODE_LEFT_ALIGN, 2);
        ptr = StringCopy(ptr, sText_Hours);
        *ptr = EOS;
        AddTextPrinterParameterized3(WIN_TC_LEFT, FONT_NORMAL, 54, 42, sFontColors, 0, buf);
    }

    // Row 4: Miles walked (steps / 2100)
    {
        u32 miles = GetGameStat(GAME_STAT_STEPS) / 2100;
        ptr = buf;
        ptr = ConvertIntToDecimalStringN(ptr, miles, STR_CONV_MODE_LEFT_ALIGN, 6);
        *ptr = EOS;
        AddTextPrinterParameterized3(WIN_TC_LEFT, FONT_NORMAL, 54, 60, sFontColors, 0, buf);
    }

    // Row 5: Money
    {
        u32 money = GetMoney(&gSaveBlock1Ptr->money);
        ptr = buf;
        ptr = StringCopy(ptr, sText_Money);
        ptr = ConvertIntToDecimalStringN(ptr, money, STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS);
        *ptr = EOS;
        AddTextPrinterParameterized3(WIN_TC_LEFT, FONT_NORMAL, 54, 82, sFontColors, 0, buf);
    }

    PutWindowTilemap(WIN_TC_LEFT);
    CopyWindowToVram(WIN_TC_LEFT, COPYWIN_FULL);

    // ---- Badge sprites (2 rows of 4, bottom of left page) ----
    // The badge .4bpp file uses BG tileset layout: row0=[TL+TR for each badge],
    // row1=[BL+BR for each badge]. Rearrange to OBJ sprite format: 4 tiles per badge.
    for (i = 0; i < NUM_BADGES; i++)
    {
        u8 *dst = &sBadgeTilesOBJ[i * 4 * TILE_SIZE_4BPP];
        CpuCopy32(&sBadgeTiles[(2 * i)          * TILE_SIZE_4BPP], dst + 0 * TILE_SIZE_4BPP, TILE_SIZE_4BPP); // TL
        CpuCopy32(&sBadgeTiles[(2 * i + 1)      * TILE_SIZE_4BPP], dst + 1 * TILE_SIZE_4BPP, TILE_SIZE_4BPP); // TR
        CpuCopy32(&sBadgeTiles[(16 + 2 * i)     * TILE_SIZE_4BPP], dst + 2 * TILE_SIZE_4BPP, TILE_SIZE_4BPP); // BL
        CpuCopy32(&sBadgeTiles[(16 + 2 * i + 1) * TILE_SIZE_4BPP], dst + 3 * TILE_SIZE_4BPP, TILE_SIZE_4BPP); // BR
    }
    {
        struct SpriteSheet sheet = { sBadgeTilesOBJ, NUM_BADGES * BADGE_TILES_PER * TILE_SIZE_4BPP, TAG_BADGES_TILES };
        LoadSpriteSheet(&sheet);
    }
    LoadSpritePalette(&sSpritePalette_Badges);
    for (i = 0; i < NUM_BADGES; i++)
    {
        if (!FlagGet(FLAG_BADGE01_GET + i))
            continue;
        u8 id = CreateSprite(&sSpriteTemplate_Badge, sBadgeX[i], sBadgeY[i], 0);
        if (id >= MAX_SPRITES)
            continue;
        StartSpriteAnim(&gSprites[id], i);
        gSprites[id].oam.priority = 2;
        sStartMenuDataPtr->badgeSpriteIds[i] = id;
    }

    // ---- Trainer card tab cursor ----
    {
        u8 cursorY = TC_ICON_0_Y + sStartMenuDataPtr->selector_y * TC_ICON_STRIDE;
        u8 cursorId;
        LoadSpritePalette(&sSpritePal_TcCursor);
        cursorId = CreateSprite(&sSpriteTemplate_TcCursor, TC_CURSOR_X, cursorY, 2);
        if (cursorId < MAX_SPRITES)
        {
            gSprites[cursorId].oam.priority = 2;
            StartSpriteAnim(&gSprites[cursorId], sStartMenuDataPtr->selector_y);
            sStartMenuDataPtr->cursorSpriteId = cursorId;
        }
    }

    ScheduleBgCopyTilemapToVram(0);
}

static void UnloadTrainerCardContent(void)
{
    u8 i;
    if (sStartMenuDataPtr->cursorSpriteId < MAX_SPRITES)
        DestroySprite(&gSprites[sStartMenuDataPtr->cursorSpriteId]);
    sStartMenuDataPtr->cursorSpriteId = SPRITE_NONE;
    FreeSpritePaletteByTag(TAG_TC_CURSOR_PAL);
    for (i = 0; i < NUM_BADGES; i++)
    {
        if (sStartMenuDataPtr->badgeSpriteIds[i] < MAX_SPRITES)
            DestroySprite(&gSprites[sStartMenuDataPtr->badgeSpriteIds[i]]);
        sStartMenuDataPtr->badgeSpriteIds[i] = SPRITE_NONE;
    }
    FreeSpriteTilesByTag(TAG_BADGES_TILES);
    FreeSpritePaletteByTag(TAG_BADGES_PAL);
    FillWindowPixelBuffer(WIN_TC_LEFT,  PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_TC_RIGHT, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_TC_LEFT);
    ClearWindowTilemap(WIN_TC_RIGHT);
    CopyWindowToVram(WIN_TC_LEFT,  COPYWIN_FULL);
    CopyWindowToVram(WIN_TC_RIGHT, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

// ==========================================
// BLANK TAB (placeholder)
// ==========================================

static void LoadBlankContent(void)
{
    FillWindowPixelBuffer(WIN_LEFT_NAME,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_RIGHT_LVS,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_ROUTE,          PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_LEFT_NAME);
    ClearWindowTilemap(WIN_LEFT_MOVES_ALL);
    ClearWindowTilemap(WIN_RIGHT_LVS);
    ClearWindowTilemap(WIN_ROUTE);
    CopyWindowToVram(WIN_LEFT_NAME,      COPYWIN_FULL);
    CopyWindowToVram(WIN_LEFT_MOVES_ALL, COPYWIN_FULL);
    CopyWindowToVram(WIN_RIGHT_LVS,      COPYWIN_FULL);
    CopyWindowToVram(WIN_ROUTE,          COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

static void UnloadBlankContent(void)
{
}

// ==========================================
// END MAP TAB (stub)
// ==========================================

static void LoadEndMapContent(void)
{
    FillWindowPixelBuffer(WIN_LEFT_NAME,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_RIGHT_LVS,      PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_ROUTE,          PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ClearWindowTilemap(WIN_LEFT_NAME);
    ClearWindowTilemap(WIN_LEFT_MOVES_ALL);
    ClearWindowTilemap(WIN_RIGHT_LVS);
    ClearWindowTilemap(WIN_ROUTE);
    CopyWindowToVram(WIN_LEFT_NAME,      COPYWIN_FULL);
    CopyWindowToVram(WIN_LEFT_MOVES_ALL, COPYWIN_FULL);
    CopyWindowToVram(WIN_RIGHT_LVS,      COPYWIN_FULL);
    CopyWindowToVram(WIN_ROUTE,          COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0);
}

static void UnloadEndMapContent(void)
{
}

// ==========================================
// TAB DEFINITION TABLE
// ==========================================

struct BookTabDef
{
    const u32 *tiles;
    const u32 *tilemap;
    const u16 *palette;
    void (*Load)(void);
    void (*Unload)(void);
};

static const struct BookTabDef sBookTabs[BOOK_TAB_COUNT] =
{
    [BOOK_TAB_TRAINER] =
    {
        .tiles   = sBookTiles_Trainer,
        .tilemap = sBookTilemap_Trainer,
        .palette = sBookPalette_Trainer,
        .Load    = LoadTrainerCardContent,
        .Unload  = UnloadTrainerCardContent,
    },
    [BOOK_TAB_PARTY] =
    {
        .tiles   = sBookTiles_Party,
        .tilemap = sBookTilemap_Party,
        .palette = sBookPalette_Party,
        .Load    = LoadPartyTabContent,
        .Unload  = UnloadPartyTabContent,
    },
    // Pokeguide uses land assets as the struct default; BookTab_Get* helpers
    // return the water variants instead when sStartMenuDataPtr->isOnWater is set.
    [BOOK_TAB_POKEGUIDE] =
    {
        .tiles   = sBookTiles_PokeGuideLand,
        .tilemap = sBookTilemap_PokeGuideLand,
        .palette = sBookPalette_PokeGuideLand,
        .Load    = LoadPokeGuideContent,
        .Unload  = UnloadPokeGuideContent,
    },
    [BOOK_TAB_SCAVENGER] =
    {
        .tiles   = sBookTiles_Scavenger,
        .tilemap = sBookTilemap_Scavenger,
        .palette = sBookPalette_Scavenger,
        .Load    = LoadScavengerContent,
        .Unload  = UnloadScavengerContent,
    },
    [BOOK_TAB_MINERALS] =
    {
        .tiles   = sBookTiles_Minerals,
        .tilemap = sBookTilemap_Minerals,
        .palette = sBookPalette_Minerals,
        .Load    = LoadMineralsContent,
        .Unload  = UnloadMineralsContent,
    },
    [BOOK_TAB_BLANK] =
    {
        .tiles   = sBookTiles_Blank,
        .tilemap = sBookTilemap_Blank,
        .palette = sBookPalette_Blank,
        .Load    = LoadBlankContent,
        .Unload  = UnloadBlankContent,
    },
    [BOOK_TAB_END_MAP] =
    {
        .tiles   = sBookTiles_EndMap,
        .tilemap = sBookTilemap_EndMap,
        .palette = sBookPalette_EndMap,
        .Load    = LoadEndMapContent,
        .Unload  = UnloadEndMapContent,
    },
};

// Pokeguide asset selectors - check water state cached at book-open time.
static const u32 *BookTab_GetTiles(u8 tabId)
{
    if (tabId == BOOK_TAB_POKEGUIDE)
        return sStartMenuDataPtr->isOnWater ? sBookTiles_PokeGuideWater : sBookTiles_PokeGuideLand;
    return sBookTabs[tabId].tiles;
}

static const u32 *BookTab_GetTilemap(u8 tabId)
{
    if (tabId == BOOK_TAB_POKEGUIDE)
        return sStartMenuDataPtr->isOnWater ? sBookTilemap_PokeGuideWater : sBookTilemap_PokeGuideLand;
    return sBookTabs[tabId].tilemap;
}

static const u16 *BookTab_GetPalette(u8 tabId)
{
    if (tabId == BOOK_TAB_POKEGUIDE)
        return sStartMenuDataPtr->isOnWater ? sBookPalette_PokeGuideWater : sBookPalette_PokeGuideLand;
    return sBookTabs[tabId].palette;
}

// Calls the Load function for the current tab. Used on initial book open.
static void InitialTabLoad(void)
{
    sBookTabs[sStartMenuDataPtr->currentTab].Load();
}

// ==========================================
// TAB FLIP ANIMATION
// ==========================================

// Task data fields used exclusively during tab flip (safe to share with action fields
// since action menus are never active during a tab flip).
#define tNextTab      data[1]
#define tFlipDir      data[2]
#define tTabAnimState data[3]
#define tFlipCounter  data[4]
#define tCurTab       data[5]  // stored so Unload() can fire after the flip covers the screen

static void Task_BookMenu_TabFlipAnim(u8 taskId)
{
    u8 nextTab = (u8)gTasks[taskId].tNextTab;
    bool8 goRight = (gTasks[taskId].tFlipDir > 0);

    switch ((u8)gTasks[taskId].tTabAnimState)
    {
    case 0:
        // Unload old tab sprites/OBJ resources now, before anything is hidden.
        // OBJ sprites are a separate layer from BG1; destroying them does not
        // corrupt any tile data. They will vanish at the next OAM update.
        sBookTabs[(u8)gTasks[taskId].tCurTab].Unload();
        // Begin DMA for flip tiles into BG2 charBase 2 (separate from BG1 charBase 0,
        // so no tile data is written to BG1 while it is still visible).
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(2,
            goRight ? sBookTiles_FlipRight : sBookTiles_FlipLeft, 0, 0, 0);
        gTasks[taskId].tTabAnimState = 1;
        break;

    case 1:
        // Wait for flip tile DMA.
        if (FreeTempTileDataBuffersIfPossible() == TRUE)
            return;
        // Load flip tilemap into BG2 buffer and flip palette. Both transfer at next VBlank.
        DecompressDataWithHeaderWram(
            goRight ? sBookTilemap_FlipRight : sBookTilemap_FlipLeft,
            sBg2TilemapBuffer);
        ScheduleBgCopyTilemapToVram(2);
        LoadPalette(goRight ? sBookPalette_FlipRight : sBookPalette_FlipLeft,
                    BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        gTasks[taskId].tTabAnimState = 2;
        break;

    case 2:
        // Flip tilemap and palette have propagated. Swap BG1 (old page) for BG2 (flip)
        // in the same frame so both take effect at the same VBlank - no gap, no flicker.
        HideBg(1);
        ShowBg(2);
        gTasks[taskId].tTabAnimState = 3;
        break;

    case 3:
        // Flip is now definitely covering the screen. Safe to load new tab tile data
        // into BG1 charBase 0 - BG1 is hidden so VRAM writes will not be visible.
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, BookTab_GetTiles(nextTab), 0, 0, 0);
        gTasks[taskId].tFlipCounter = 14;
        gTasks[taskId].tTabAnimState = 4;
        break;

    case 4:
        // Hold the flip frame while waiting for new tile DMA and the minimum display time.
        {
            bool8 dmaStillBusy = (FreeTempTileDataBuffersIfPossible() == TRUE);
            if (gTasks[taskId].tFlipCounter > 0)
                gTasks[taskId].tFlipCounter--;
            if (dmaStillBusy || gTasks[taskId].tFlipCounter > 0)
                return;
        }
        // New tiles are in VRAM. Load new tab tilemap and palette; both transfer next VBlank.
        DecompressDataWithHeaderWram(BookTab_GetTilemap(nextTab), sBg1TilemapBuffer);
        CopyBgTilemapBufferToVram(1);
        LoadPalette(BookTab_GetPalette(nextTab), BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        gTasks[taskId].tTabAnimState = 5;
        break;

    case 5:
        // New tilemap and palette have propagated. Swap BG2 (flip) for BG1 (new page)
        // atomically, then load the new tab's sprites and windows.
        HideBg(2);
        ShowBg(1);
        memset(sBg2TilemapBuffer, 0, 0x800);
        ScheduleBgCopyTilemapToVram(2);
        sStartMenuDataPtr->currentTab = nextTab;
        sBookTabs[nextTab].Load();
        gTasks[taskId].func = Task_StartMenuBookMain;
        break;
    }
}

// Unload the current tab, then transition to a new one.
// direction: +1 = right (next tab), -1 = left (previous tab).
static void BeginTabSwitch(u8 taskId, s8 direction)
{
    u8 cur = sStartMenuDataPtr->currentTab;
    u8 next = (u8)((cur + BOOK_TAB_COUNT + direction) % BOOK_TAB_COUNT);

    // Do NOT unload here. Unload fires at the start of Task_BookMenu_TabFlipAnim (case 0)
    // so OBJ sprites vanish cleanly before the flip tile DMA begins.
    gTasks[taskId].tCurTab  = cur;
    gTasks[taskId].tNextTab = next;
    gTasks[taskId].tFlipDir = (s16)direction;
    gTasks[taskId].tTabAnimState = 0;
    gTasks[taskId].func = Task_BookMenu_TabFlipAnim;
}

