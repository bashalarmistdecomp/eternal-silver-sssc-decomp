#include "psyq_types.h"

/* g_SpriteDataAddresses @ 0x80011054, 3 pointers — migrated from boot_data.data_80011020.s */
extern u16 g_SpriteData0[];
extern u8 g_SpriteData1[];
extern u8 g_SpriteData2[];

u8 *g_SpriteDataAddresses[] = {
    g_SpriteData0,
    g_SpriteData1,
    g_SpriteData2,
};
