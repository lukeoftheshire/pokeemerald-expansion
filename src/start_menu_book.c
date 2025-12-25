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
#include "party_menu.h"



// ==========================================
// BOOK MENU IMPLEMENTATION
// ==========================================

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
    u8
};





#define SPRITE_NONE 0xFF
#define BOOK_TEXT_BASE  1

static u8 sStartMenuPartyIconSpriteIds[PARTY_SIZE];

// Left page text windows for the book menu
enum WindowIds
{
    WIN_LEFT_NAME,
    WIN_LEFT_MOVES_ALL,
    WIN_LEFT_COUNT,
    WIN_RIGHT_NAMES,
    WIN_RIGHT_HP_BARS,
    WIN_RIGHT_LVS
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
static void DisplaySelectedPokemonDetails(u8 index);
static void CreatePartyMonIcons(void);
static void DestroyPartyMonIcons(void);
static void DestroyBookPortraitSprite(void);
static void CreateBookPortraitSprite(u8 partyIndex);
static void SpriteCB_Pokemon(struct Sprite *sprite);
static void CreateOrUpdateBookHeldItemIcon(u16 itemId);
static void DestroyBookHeldItemIcon(void);

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
        .priority = 1,      // Lower priority (behind text)
    },
    {
        .bg = 2,    // this will (eventually) load the different tabs
        .charBaseIndex = 2,
        .mapBaseIndex = 28,
        .priority = 2
    },
    {
        .bg = 3,    // If I get really bored, maybe this will hold backgrounds based on...location?timeOfDay?
        .charBaseIndex = 3,
        .mapBaseIndex = 26,
        .priority = 3
    }
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
       .paletteNum = 0,
       .baseBlock = 1},
      [WIN_LEFT_MOVES_ALL] = {
          .bg = 0,
          .tilemapLeft = 1,
          .tilemapTop = 13,
          .width = 14,       // Wide enough to cover both left and right columns
          .height = 6,       // Tall enough for both rows
          .paletteNum = 0,
          .baseBlock = 1 + 20*6 + 13*3+14*3},
          // Calculate new baseblock size,

    [WIN_LEFT_COUNT] = DUMMY_WIN_TEMPLATE,
};

//Graphics
static const u8 sFontColorTable[][3] =
{
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_GREEN, TEXT_COLOR_TRANSPARENT},  // (Dark red d.t. palette)
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_BLUE, TEXT_COLOR_TRANSPARENT},  // Default
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_TRANSPARENT},  // Default
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,      TEXT_COLOR_BLUE},      // Unused
    {TEXT_COLOR_TRANSPARENT, TEXT_DYNAMIC_COLOR_2,  TEXT_DYNAMIC_COLOR_3},  // Gender symbol
    {TEXT_DYNAMIC_COLOR_4,       TEXT_DYNAMIC_COLOR_5,  TEXT_DYNAMIC_COLOR_6}, // Selection actions
    {TEXT_COLOR_WHITE,       TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_BLUE}, // Field moves
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},  // Unused
    {TEXT_COLOR_WHITE,       TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_RED},  // Move relearner
};

//background
static const u32 sBookTiles[] = INCBIN_U32("graphics/start_menu_book/book_tiles_v3.4bpp.lz");
static const u32 sBookTilemap[] = INCBIN_U32("graphics/start_menu_book/book_tiles_v3.bin.lz");
static const u16 sBookPalette[] = INCBIN_U16("graphics/start_menu_book/test_pal.gbapal");

//party selection cursor
static const u32 sCursor_Gfx[] = INCBIN_U32("graphics/start_menu_book/cursor.4bpp.lz");
static const u16 sCursor_Pal[] = INCBIN_U16("graphics/start_menu_book/cursor.gbapal");

//HP Bar - todo
/*
static const u8 sHPBar_100_Percent_Gfx[]  = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_100_Percent_Gfx.4bpp");
static const u8 sHPBar_90_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_90_Percent_Gfx.4bpp");
static const u8 sHPBar_80_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_80_Percent_Gfx.4bpp");
static const u8 sHPBar_70_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_70_Percent_Gfx.4bpp");
static const u8 sHPBar_60_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_60_Percent_Gfx.4bpp");
static const u8 sHPBar_50_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_50_Percent_Gfx.4bpp");
static const u8 sHPBar_40_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_40_Percent_Gfx.4bpp");
static const u8 sHPBar_30_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_30_Percent_Gfx.4bpp");
static const u8 sHPBar_20_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_20_Percent_Gfx.4bpp");
static const u8 sHPBar_10_Percent_Gfx[]   = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_10_Percent_Gfx.4bpp");
static const u8 sHPBar_0_Percent_Gfx[]    = INCBIN_U8("graphics/ui_startmenu_full/sHPBar_0_Percent_Gfx.4bpp");
static const u16 sHP_Pal[] = INCBIN_U16("graphics/ui_startmenu_full/hpbar_pal.gbapal");
static const u16 sHP_PalAlt[] = INCBIN_U16("graphics/ui_startmenu_full/hpbar_pal_alt.gbapal");
*/

//
//  Sprite Data for Cursor, IconBox, GreyedBoxes, and Statuses
//
#define TAG_CURSOR 30004
#define TAG_ICON_BOX 30006
#define TAG_HPBAR   30008
#define TAG_STATUS_ICONS 30009
#define TAG_BOOK_HELD_ITEM_TILES   0xE100
#define TAG_BOOK_HELD_ITEM_PAL     0xE101

//#define TAG_BOOK_PORTRAIT_PAL  6100

static const struct OamData sOamData_Cursor =
{
    .size = SPRITE_SIZE(64x64),
    .shape = SPRITE_SHAPE(64x64),
    .priority = 1,
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

//
//  Begin Sprite Loading Functions
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
    gSprites[sStartMenuDataPtr->cursorSpriteId].oam.priority = 1;
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


void StartBookMenu(MainCallback callback)
{
    //u32 i = 0;
    if ((sStartMenuDataPtr = AllocZeroed(sizeof(struct StartMenuResources))) == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    // Make Sure Sprites are Empty on Reload
    sStartMenuDataPtr->gfxLoadState = 0;
    sStartMenuDataPtr->savedCallback = callback;
    sStartMenuDataPtr->cursorSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->portraitSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->heldItemSpriteId = SPRITE_NONE;
    sStartMenuDataPtr->lastSelectedMenu = 0xFF;


/*    for(i= 0; i < 6; i++)
    {
        sStartMenuDataPtr->iconBoxSpriteIds[i] = SPRITE_NONE;
        sStartMenuDataPtr->iconMonSpriteIds[i] = SPRITE_NONE;
    }
    for(i= 0; i < 3; i++)
    {
        sStartMenuDataPtr->greyMenuBoxIds[i] = SPRITE_NONE;
    }*/
    InitCursorInPlace();

    gFieldCallback = NULL;

    SetMainCallback2(StartMenuBook_RunSetup);
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
    ChangeBgY(2, 128, BG_COORD_SUB); // controls the background scrolling
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
        //PrintMapNameAndTime(); // print all sprites
        //CreateGreyedMenuBoxes();
        //CreateIconBox();
        CreateCursor();
        CreatePartyMonIcons();
        DisplaySelectedPokemonDetails(gSelectedMenu);
        //CreateBookPortraitSprite(gSelectedMenu);
        //StartMenu_DisplayHP();
        //CreatePartyMonStatuses();
        gMain.state++;
        break;
    case 6:
        CreateTask(Task_StartMenuBookWaitFadeIn, 0);
        //BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
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
    try_free(sStartMenuDataPtr);
    try_free(sBg1TilemapBuffer);
    try_free(sBg2TilemapBuffer);
    DestroyCursor();
    //DestroyIconBoxs();
    DestroyPartyMonIcons();
    DestroyBookPortraitSprite();
    DestroyBookHeldItemIcon();
    StopCryAndClearCrySongs();
    ResetSpriteData();
    FreeAllSpritePalettes();
    //DestroyStatusSprites();
    //DestroyGreyMenuBoxes();
    FreeAllWindowBuffers();
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
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    return TRUE;
}

static bool8 StartMenuBook_LoadGraphics(void) // Load the Tilesets, Tilemaps, Spritesheets, and Palettes for Everything
{
    switch (sStartMenuDataPtr->gfxLoadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sBookTiles, 0, 0, 0);
        sStartMenuDataPtr->gfxLoadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sBookTilemap, sBg1TilemapBuffer);
            sStartMenuDataPtr->gfxLoadState++;
        }
        break;
    case 2:
    {
        struct SpritePalette cursorPal = {sSpritePal_Cursor.data, sSpritePal_Cursor.tag};
        LoadPalette(sBookPalette, 0, sizeof(sBookPalette));
        //LoadPalette(sHP_Pal, 32, 16);

        //LoadPalette(sScrollBgPalette, 16, 16);

        /*LoadCompressedSpriteSheet(&sSpriteSheet_IconBox);
        LoadSpritePalette(&sSpritePal_IconBox);*/
        LoadCompressedSpriteSheet(&sSpriteSheet_Cursor);
        LoadSpritePalette(&cursorPal);/*
        LoadCompressedSpriteSheet(&sSpriteSheet_StatusIcons);
        LoadCompressedSpritePalette(&sSpritePalette_StatusIcons);*/
/*
        LoadCompressedSpriteSheet(&sSpriteSheet_GreyMenuButtonMap);
        LoadCompressedSpriteSheet(&sSpriteSheet_GreyMenuButtonDex);
        LoadCompressedSpriteSheet(&sSpriteSheet_GreyMenuButtonParty);
        LoadSpritePalette(&sSpritePal_GreyMenuButton);*/
        sStartMenuDataPtr->gfxLoadState++;
        break;
    }
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
    
/*  FillWindowPixelBuffer(WIN_LEFT_ITEM, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WIN_LEFT_ITEM);
    CopyWindowToVram(WIN_LEFT_ITEM, COPYWIN_FULL);*/

    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WIN_LEFT_MOVES_ALL);
    CopyWindowToVram(WIN_LEFT_MOVES_ALL, COPYWIN_FULL);

/*
    FillWindowPixelBuffer(WIN_LEFT_MOVE_LU, PIXEL_FILL(TEXT_COLOR_LIGHT_GREEN));
    PutWindowTilemap(WIN_LEFT_MOVE_LU);
    CopyWindowToVram(WIN_LEFT_MOVE_LU, COPYWIN_FULL);

    FillWindowPixelBuffer(WIN_LEFT_MOVE_RU, PIXEL_FILL(TEXT_COLOR_LIGHT_GREEN));
    PutWindowTilemap(WIN_LEFT_MOVE_RU);
    CopyWindowToVram(WIN_LEFT_MOVE_RU, COPYWIN_FULL);

    FillWindowPixelBuffer(WIN_LEFT_MOVE_LL, PIXEL_FILL(TEXT_COLOR_LIGHT_GREEN));
    PutWindowTilemap(WIN_LEFT_MOVE_LL);
    CopyWindowToVram(WIN_LEFT_MOVE_LL, COPYWIN_FULL);

    FillWindowPixelBuffer(WIN_LEFT_MOVE_RL, PIXEL_FILL(TEXT_COLOR_LIGHT_GREEN));
    PutWindowTilemap(WIN_LEFT_MOVE_RL);
    CopyWindowToVram(WIN_LEFT_MOVE_RL, COPYWIN_FULL);
*/

//END LEFT SUMMARY SCREEN WINDOWS
    
    FillWindowPixelBuffer(WIN_RIGHT_NAMES, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WIN_RIGHT_NAMES);
    CopyWindowToVram(WIN_RIGHT_NAMES, COPYWIN_FULL);
    
    FillWindowPixelBuffer(WIN_RIGHT_HP_BARS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WIN_RIGHT_HP_BARS);
    CopyWindowToVram(WIN_RIGHT_HP_BARS, COPYWIN_FULL);
    
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
        gTasks[taskId].func = Task_StartMenuBookMain;
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
        DisplaySelectedPokemonDetails(selected);
        CreateBookPortraitSprite(selected);
    }


    if (JOY_NEW(A_BUTTON)) //when A is pressed, load the Task for the Menu the cursor is on, for some they require a flag to be set
    {
        /*switch(gSelectedMenu)
        {
            case START_MENU_BAG:
                PlaySE(SE_SELECT);
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gTasks[taskId].func = Task_OpenBagFromStartMenu;
                break;
            case START_MENU_POKEDEX:
                if(FlagGet(FLAG_SYS_POKEDEX_GET))
                {
                    PlaySE(SE_SELECT);
                    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                    gTasks[taskId].func = Task_OpenPokedexFromStartMenu;
                }
                else
                {
                    PlaySE(SE_BOO);
                }
                break;
            case START_MENU_PARTY:
                if(FlagGet(FLAG_SYS_POKEMON_GET))
                {
                    PlaySE(SE_SELECT);
                    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                    gTasks[taskId].func = Task_OpenPokemonPartyFromStartMenu;
                }
                else
                {
                    PlaySE(SE_BOO);
                }
                break;
            case START_MENU_MAP:
                if(FlagGet(FLAG_SYS_POKENAV_GET))
                {
                    PlaySE(SE_SELECT);
                    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                    gTasks[taskId].func = Task_OpenPokenavStartMenu;
                }
                else{
                    PlaySE(SE_BOO);
                }
                break;
            case START_MENU_CARD:
                PlaySE(SE_SELECT);
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gTasks[taskId].func = Task_OpenTrainerCardFromStartMenu;
                break;
            case START_MENU_OPTIONS:
                PlaySE(SE_SELECT);
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gTasks[taskId].func = Task_OpenOptionsMenuStartMenu;
                break;
        }*/
    }

    if(JOY_NEW(START_BUTTON)) // If start button pressed go to Save Confirmation Control Task
    {
        //PrintSaveConfirmToWindow();
        //gTasks[taskId].func = Task_HandleSaveConfirmation;
    }

    if(JOY_NEW(R_BUTTON))
    {
    //shift right
    }
    if(JOY_NEW(L_BUTTON))
    {
    //shift left
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

    LoadMonIconPalettes();

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

        if (sStartMenuPartyIconSpriteIds[i] != SPRITE_NONE)
        {
            // Put icons in front of BGs
            gSprites[sStartMenuPartyIconSpriteIds[i]].oam.priority = 0;
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
            DestroySprite(&gSprites[sStartMenuPartyIconSpriteIds[i]]);
            sStartMenuPartyIconSpriteIds[i] = SPRITE_NONE;
        }
    }
}


static void DisplaySelectedPokemonDetails(u8 index)
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
    // MOVES

    u8 i;
    FillWindowPixelBuffer(WIN_LEFT_MOVES_ALL, PIXEL_FILL(0));

    for (i = 0; i < 4; i++)
    {
            u16 moveId = GetMonData(mon, MON_DATA_MOVE1 + i);
            const u8 *moveName;

            if (moveId == MOVE_NONE)
                moveName = gText_OneDash;   // built-in "-" string in pokeemerald
            else
                moveName = GetMoveName(moveId);
            // Print to the single large window
            AddTextPrinterParameterized3(
                WIN_LEFT_MOVES_ALL,
                GetFontIdToFit(moveName, FONT_SMALL_NARROWER, 0, 13),
                6,
                6+10*i,
                sFontColorTable[0],
                0,
                moveName
            );
        }

    // HELD ITEM
    u16 held = GetMonData(mon, MON_DATA_HELD_ITEM);
    CreateOrUpdateBookHeldItemIcon(held);
}


////FXNS to display pokmeon sprite
static void CreateBookPortraitSprite(u8 partyIndex)
{
   DestroyBookPortraitSprite(); // safe replace

    u16 spriteId = CreateMonPicSprite_Affine(
         GetMonData(&gPlayerParty[partyIndex],
         MON_DATA_SPECIES_OR_EGG),
         IsMonShiny(&gPlayerParty[partyIndex]),
         GetMonData(&gPlayerParty[partyIndex],
         MON_DATA_PERSONALITY),MON_PIC_AFFINE_FRONT,
         50, 72, 8,TAG_NONE
    );

    if (spriteId == 0xFFFF)
        spriteId = SPRITE_NONE;

    sStartMenuDataPtr->portraitSpriteId = spriteId;
    if (spriteId != SPRITE_NONE)
    {
        gSprites[spriteId].data[0] = GetMonData(&gPlayerParty[partyIndex],MON_DATA_SPECIES_OR_EGG);
        gSprites[spriteId].callback = SpriteCB_Pokemon;
        gSprites[spriteId].oam.priority = 0;
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

static void SpriteCB_Pokemon(struct Sprite *sprite)
{
    DoMonFrontSpriteAnimation(sprite, sprite->data[0], FALSE, 1);
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

    gSprites[spriteId].oam.priority = 0; // in front of BG
}

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


