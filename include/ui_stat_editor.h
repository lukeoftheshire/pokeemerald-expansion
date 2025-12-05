#ifndef GUARD_UI_STAT_EDIT_H
#define GUARD_UI_STAT_EDIT_H

#include "main.h"

void Task_OpenStatEditorFromStartMenu(u8 taskId);
void StatEditor_Init(MainCallback callback);

const u8 *GetNatureName(u8 nature);
const u8 *GetAbilityName(u16 ability);
extern const struct SpeciesInfo gSpeciesInfo[];


#endif // GUARD_UI_MENU_H
