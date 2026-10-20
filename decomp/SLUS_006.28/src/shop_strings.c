/* Lunar SSSC — shop_strings: the two strings the shop shows for a number.
   The first split had it in misc_d.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "mem.h"

s32 font_string_put(s32, s32, s32);                         /* extern */
s32 font_target_clear(s32, s32);                              /* extern */
s32 font_target_set(s32);                                 /* extern */
s32 font_col_set(s32);                               /* extern */
s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);         /* extern */

/* func_80054A08 */
void ShopItemStringsDisp(s32 item, u8 col) {
    s32 kept_col;

    font_target_clear(0x801CBB00, 8);
    font_target_set(0x801CBB00);
    kept_col = col;
    font_col_set(kept_col);
    font_string_put((item + 0x100) & 0xFFFF, 0, 1);
    CellTextDisp(0xC, 6, 8, 1, 8, 0xDEAD2001, 0xED00);
    font_target_clear(0x801CBF00, 0xC);
    font_target_set(0x801CBF00);
    font_col_set(kept_col);
    font_string_put((item + 0x200) & 0xFFFF, 0, 1);
    CellTextDisp(0xC, 8, 0xC, 1, 0xC, 0xDEAD2001, 0xED20);
}
