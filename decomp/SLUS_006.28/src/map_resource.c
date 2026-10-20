/* Lunar SSSC — map_resource: the map's and the script's resources found by
   number and loaded (MapResourceFind, MapNumberFind, ScriptResourceLoad,
   MapResourceLoad). The first split had it in io.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
extern s8 g_MapResourceSw;
s32 resource_load(s32, u32);
extern u8 g_ScriptResourcePairs[];
extern u8 g_MapResourcePairs[];
extern s32 g_PaletteBgCoverCount;
extern volatile unsigned int g_LoadBuffer;
extern volatile unsigned int g_LoadBuffer;
s32 resource_pair_find(s32 *, s32);
s32 MapResourceFind(s32 map_number);
void MapNumberFind(s32 resource);
void palette_bg_cover_count_set(s32 zeroed_col_count);
extern long HWD0, VWD0;
extern long HWD0;
extern long VWD0;
s32 bcopy(u32, s32, s32);
void ScriptResourceLoad(s32 script_number, s32 destination);
extern long HWD0, VWD0;
extern volatile unsigned int g_LoadBuffer;
s32 resource_pair_find(s32 *, s32);
s32 MapResourceFind(s32 map_number);
void MapNumberFind(s32 resource);
void palette_bg_cover_count_set(s32 zeroed_col_count);
s32 resource_pair_find(s32 *, s32);
s32 MapResourceFind(s32 map_number);
void MapNumberFind(s32 resource);
void palette_bg_cover_count_set(s32 zeroed_col_count);
void resource_load_run(s32 resource);  /* extern */
int resource_pair_key_find(short *a, int b);  /* extern */

s32 resource_pair_find(s32 *, s32);                          /* extern */

/* func_80020CD0 */
s32 MapResourceFind(s32 map_number) {
    resource_pair_find((s32 *)g_MapResourcePairs, map_number);
}
/* func_80020CF8 */
void MapNumberFind(s32 resource) {
    resource_pair_key_find(g_MapResourcePairs, resource);
}
s32 resource_load(s32, u32);                               /* extern */
s32 bcopy(u32, s32, s32);                         /* extern */

/* func_80020D20 */
void ScriptResourceLoad(s32 script_number, s32 destination) {
    resource_load(resource_pair_find((s32 *) g_ScriptResourcePairs, script_number), g_LoadBuffer);
    bcopy(g_LoadBuffer, destination, 0x9F80);
}
/* far_syms: D_800B88A8 (target reaches it %hi/%lo) */
/* far_syms: D_800B88A8 (auto: target reaches it %hi/%lo — addressing row 3) */
extern s16 D_800B88A8;

/* func_80020D70 */
void MapResourceLoad(s32 map_number, s32 sw) {
    g_MapResourceSw = sw;
    D_800B88A8 = map_number;
    resource_load_run(MapResourceFind(map_number));
    bg_data_reset();
    bg_attribute_data_reset();
}

/* func_80020DB4 */
void palette_bg_cover_count_set(s32 zeroed_col_count) {
    g_PaletteBgCoverCount = zeroed_col_count;
}
