/* Lunar SSSC — main: the program's start: main and main_buffer_init (the
   fixed buffers), with the build stamp's compare and its copy around main
   (overlay_stamp_check in overlay.c calls them). The first split had it in
   boot_init.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern u8 g_BuildStamp[];     
extern s32 g_ResourceReadBuffer;      /* heap block base: 0x80156000 */
extern s32 g_BgDataBuffer;
extern s32 g_BgAttributeDataBuffer;
extern s32 g_ResourceCommand01Buffer;      /* main work buffer (0x15828 bytes) */
extern s32 g_BgSecondBuffer;
extern s32 g_LoadBuffer;      /* MENU.BIN overlay base: 0x80196000 */

extern s32 strcmp(s32 s1, u8 *s2);
extern s32 xmalloc_top_alloc(s32 size);   /* allocator */
extern void func_800506DC(void);
extern void IconInit(s32 icon_data);
extern void bg_data_init(void);
extern void bg_attribute_data_init(void);

/* func_80013D08 */
s32 build_stamp_compare(s32 stamp) {
    return strcmp(stamp, g_BuildStamp);
}

extern s8 g_GameEnded;
extern s32 D_800B885C;
extern s32 g_StartFirstStep;
extern void system_init(void);
extern void screen_mode_set(s32);
extern void vram_strip_clear(void);
extern void sprite_reset(s32);
extern void resource_load_run(s32);
extern void sound_common_load(s32);
extern s32 sound_bank_empty_fill(s32);
extern void func_80025FE0(s32);
extern void font_string_table_set(void);
extern void func_80026AB0(void);
extern void GameStart(void);
extern void GameRun(void);

/* Main loop: one pass of subsystem init + frame work, forever.
   The image's first instruction here is `jal __main`. No call is written for it: the
   compiler puts one at the top of any function named main, and __main is the empty
   function Sony's start-up object (2mbyte.obj) supplies for it. */
/* func_80013D2C */
void main(void) {
    while (1) {
        g_GameEnded = 0;
        g_StartFirstStep = 0;
        system_init();
        main_buffer_init();
        screen_mode_set(1);
        vram_strip_clear();
        sprite_reset(0);
        resource_load_run(1);
        sound_common_load(0);
        sound_bank_empty_fill(1);
        g_StartFirstStep = 0;
        func_80025FE0(1);
        font_string_table_set();
        func_80026AB0();
        if (g_StartFirstStep == 4) {
            g_StartFirstStep = 3;
        }
        GameStart();
        D_800B885C = 1;
        GameRun();
    }
}

/* second entry laid after the loop: hands its argument to build_stamp_compare and
   returns that call's result (overlay.c calls it with the overlay base and
   uses the result) */
/* Ruling of 2026-10-14 (docs/naming.md): Does what build_stamp_compare does; nothing read says why there are two. */
/* func_80013DE4 */
s32 build_stamp_compare_copy(s32 stamp) {
    return build_stamp_compare(stamp);
}
/* Boot-time memory setup: carves the fixed heap blocks and overlay slots,
   then hands off to the subsystem initialisers. */
/* func_80013E04 */
void main_buffer_init(void) {
    g_ResourceCommand01Buffer = xmalloc_top_alloc(0x15828);
    func_800506DC();
    g_BgDataBuffer = xmalloc_top_alloc(0x8000);
    g_BgSecondBuffer = xmalloc_top_alloc(0x4000);
    g_BgAttributeDataBuffer = xmalloc_top_alloc(0xA000);
    g_LoadBuffer = 0x80196000;
    g_ResourceReadBuffer = 0x80156000;
    xmalloc_top_alloc(0);
    IconInit(g_ResourceCommand01Buffer + 0x899C);
    bg_data_init();
    bg_attribute_data_init();
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; named from what its callers pass: the script reader's state, before each command and where it stops. */
/* func_80013E98 */
void ScriptStateReport() {
}

/* byteswap16 */
/* func_80013EA0 */
s32 u16_bytes_swap(s32 value) {
    return ((u32) (value & 0xFF00) >> 8) | ((value & 0xFF) << 8);
}
