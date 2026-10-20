/* Lunar SSSC — pad_start: the pad's two functions: its start (the ports'
   buffers, PadInitDirect, the actuators) and the buttons pad_update found.
   The first split had it in io.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
extern s32 g_PadButtons;

/* func_80020030 */
s32 pad_buttons_get(void) {
    return g_PadButtons;
}
extern u8 g_PadActuatorLevels[];
extern u8 g_PadPort00Buffer[];
extern u8 g_PadPort10Buffer[];
s32 bzero(s32 *, s32);                            /* extern */

/* func_8002003C */
void pad_start(void) {
    bzero((s32 *)g_PadPort00Buffer, 0x40);
    bzero((s32 *)g_PadPort10Buffer, 0x40);
    PadInitDirect((s32 *)g_PadPort00Buffer, (s32 *)g_PadPort10Buffer);
    PadSetAct(0, (s32 *)g_PadActuatorLevels, 2);
    PadStartCom();
}
