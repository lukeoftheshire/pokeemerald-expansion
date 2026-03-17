#include "global.h"
#include "main.h"
#include "constants/mining_minigame.h"

void StartMining(void);
void StartMiningWithCallback(MainCallback returnCb);
void GetMiningDisplayItems(
    u16 *commonOut,   u8 *commonCount,
    u16 *uncommonOut, u8 *uncommonCount,
    u16 *rareOut,     u8 *rareCount,
    u8 maxCommon, u8 maxUncommon, u8 maxRare);

