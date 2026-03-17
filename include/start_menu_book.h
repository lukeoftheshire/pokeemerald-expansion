#ifndef GUARD_UI_START_MENU_H
#define GUARD_UI_START_MENU_H

#include "main.h"

void Task_OpenStartMenuFullScreen(u8 taskId);
void StartMenuFull_Init(MainCallback callback);
void StartBookMenu(MainCallback callback);
void StartBookMenuOnTab(MainCallback callback, u8 openTab);

enum BookTab
{
    BOOK_TAB_TRAINER   = 0,
    BOOK_TAB_PARTY     = 1,
    BOOK_TAB_POKEGUIDE = 2,
    BOOK_TAB_SCAVENGER = 3,
    BOOK_TAB_MINERALS  = 4,
    BOOK_TAB_BLANK     = 5,
    BOOK_TAB_END_MAP   = 6,
    BOOK_TAB_COUNT,
};

enum
{
    PARTY_BOX_LEFT_COLUMN,
    PARTY_BOX_RIGHT_COLUMN,
};



extern const u32 gStartMenuBookTiles[];
extern const u16 gStartMenuBookPal[];
extern const u16 gStartMenuBookTilemap[];




#endif // GUARD_UI_MENU_H