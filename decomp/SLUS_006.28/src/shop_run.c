/* Lunar SSSC — shop_run: ShopRun, one shop from its opening to its end. The
   first split had it in misc_b.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
extern s32 D_800B8B28;
extern s32 D_800B8B2C;
extern u8 g_ShopStockRow;
s32 ShopOpen();
s32 ShopClose();
s32 ShopStepsRun(void);
void ShopRun(s32 shop_number);
void func_800506DC(void);

s32 ShopOpen();                                  /* extern */
s32 ShopClose();                                  /* extern */
s32 ShopStepsRun();                                  /* extern */

/* func_80050694 */
void ShopRun(s32 shop_number) {
    s32 shop_byte;

    shop_byte = shop_number & 0xFF;
    if ((shop_byte != 0) && (shop_byte != 8)) {
        g_ShopStockRow = shop_number - 1;
        ShopOpen();
        ShopStepsRun();
        ShopClose();
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Zeroes two words that are read only to be passed where nothing reads them; its callers do not say what for. */
void func_800506DC(void) {
    D_800B8B2C = 0;
    D_800B8B28 = 0;
}
