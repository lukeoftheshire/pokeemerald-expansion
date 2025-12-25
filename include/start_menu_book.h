#ifndef GUARD_UI_START_MENU_H
#define GUARD_UI_START_MENU_H

#include "main.h"

void Task_OpenStartMenuFullScreen(u8 taskId);
void StartMenuFull_Init(MainCallback callback);

enum
{
    PARTY_BOX_LEFT_COLUMN,
    PARTY_BOX_RIGHT_COLUMN,
};



extern const u32 gStartMenuBookTiles[];
extern const u16 gStartMenuBookPal[];
extern const u16 gStartMenuBookTilemap[];




#endif // GUARD_UI_MENU_H