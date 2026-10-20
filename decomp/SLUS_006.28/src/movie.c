/* Lunar SSSC — movie: what MoviePlay calls around a movie: MovieScreenClose
   (screen and sound shut before it) and the empty call after it. The first
   split had it in scripting.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
#include "chr.h"

void func_800590B8();  
/* far_syms: D_800B8AE4 (target reaches it %hi/%lo) */
/* far_syms: g_MoveChrSecondViewFollow (target reaches it %hi/%lo) */
/* far_syms: D_800B8AE4 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MoveChrSecondViewFollow (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MoveChrViewFollow (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 D_800B8AE4;
extern s32 g_MapLayersMask;
extern u8 g_MapFadeState;
extern u8 g_MapDrawAreaClosed;
extern u8 g_MoveChrSecondViewFollow;
extern u8 g_MoveChrViewFollow;

/* func_8005901C */
s32 MovieScreenClose(void) {
    s32 var;

    if (g_MapFadeState == 0 && g_MapDrawAreaClosed == 0) {
        var = MapDrawAreaClose(g_MapLayersMask | 0x130, 0x19, 0xFF, g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
    }

    layer_mix_set(8, 0x20);
    VoiceStep(0);
    sprite_run_off(0, 0x96);
    sprite_list_draw();
    MapFrameEndCopy();
    MusicStop();
    D_800B8AE4 += 0xA;
    return D_800B8AE4;
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; reached once, after a movie or picture is shown, which does not say what for. */
void func_800590B8() {

}
