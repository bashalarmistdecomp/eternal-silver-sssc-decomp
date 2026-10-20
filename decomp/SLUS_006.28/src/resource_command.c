/* Lunar SSSC — resource_command: the handlers of a resource's commands 2 to
   18 (pictures, texture pages, palettes, the bg and its attributes, the
   map's enter table, tile animations and patterns) and the pair lists they
   look things up in. The first split had it in io.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
extern s8 g_MapResourceSw;
s32 inflate_buffer(s32 source, s32 destination);  /* extern */
s32 palette_bg_second_row_set(s32, s32, s32);
s32 palette_glyph_zero_set(s32, s32);
void palette_zero_fix(s16 *, s32, s32);
void palette_bit15_set(u16 *, s32, s32);
extern s32 g_ResourceTimCount;
s32 palette_rows_set(s32, s32, s32);
extern volatile unsigned int g_LoadBuffer;
void resource_tim_reset(void);
void resource_tim_add(s32 command_word, s32 unused, s32 payload);
void resource_tpage_load(s32 command_word, s32 unused, s32 payload);
void resource_palette_load(s32 command_word, s32 unused, s32 payload);
void resource_palette_row_load(s32 command_word, s32 unused, s32 payload);
void resource_bg_set(s32 unused, s32 byte_count, s32 payload);
void resource_bg_attribute_set(s32 unused, s32 byte_count, s32 payload);
void resource_map_enter_table_set(s32 unused, s32 byte_count, s32 payload);
void resource_bg_second_set(s32 unused, s32 unused2, s32 payload);
void resource_tile_animation_set(s32 unused, s32 unused2, s32 payload);
s32 resource_tile_pattern_set(s32 command_word, s32 unused, s32 payload);
s32 tim_image_load(s32);
s32 resource_tim_unpack();
extern volatile unsigned int g_LoadBuffer;
void resource_tim_load(void);
void resource_tim_reset(void);
s32 resource_pair_find(s32 *, s32);
extern long HWD0, VWD0;
extern long HWD0;
extern long VWD0;
s32 palette_bg_cover_rows_set(s32, s32);
void resource_palette_load(s32 command_word, s32 unused, s32 payload);
void resource_palette_row_load(s32 command_word, s32 unused, s32 payload);
extern long HWD0, VWD0;
void resource_words_copy(s32 destination, s32 source, s32 byte_count);  /* extern */
s32 tpage_image_load(s32 tpage, void *pixels);  /* extern */
s32 tpage_small_image_load(s32 tpage, void *pixels);  /* extern */

/* far_syms: D_800E0B90 (target reaches it %hi/%lo) */
/* far_syms: g_ResourceTims (target reaches it %hi/%lo) */
extern u8 D_800E0B90[];
extern u8 g_ResourceTims[];

/* func_800208A0 */
void resource_tim_add(s32 command_word, s32 unused, s32 payload)
{
    if ((((command_word >> 8) & 0xFF) == 0))
    {
        inflate_buffer(payload, g_LoadBuffer);
        tim_image_load(g_LoadBuffer);
    }
    else
    {
        s32 index = g_ResourceTimCount;

        *(s32 *)&g_ResourceTims[index * 4] = payload;
        *(s32 *)&D_800E0B90[index * 4] = command_word;
        g_ResourceTimCount = index + 1;
    }
}

/* func_80020918 */
s32 resource_tim_unpack(picture)
s32 picture;
{
    return inflate_buffer(*(s32 *)((picture * 4) + g_ResourceTims), (*(s32 *)&g_LoadBuffer));
}

s32 tim_image_load(s32);                               /* extern */
s32 resource_tim_unpack();                                  /* extern */
extern volatile unsigned int g_LoadBuffer;

/* func_80020950 */
void resource_tim_load(void) {
    resource_tim_unpack();
    tim_image_load(g_LoadBuffer);
}

/* func_8002097C */
void resource_tpage_load(s32 command_word, s32 unused, s32 payload)
{
  s32 payload_then_sub_number;
  s32 sub_number;
  payload_then_sub_number = payload;
  inflate_buffer(payload_then_sub_number, g_LoadBuffer);
  sub_number = (command_word >> 8) & 0xFF;
  if (sub_number < 3)
  {
    tpage_image_load(((payload_then_sub_number = sub_number) * 2) + 0x19, *(&g_LoadBuffer));
  }
  else
    if (sub_number < 0x80)
    {
      tpage_small_image_load(0x1F, *(&g_LoadBuffer));
    }
    else
      if (sub_number < 0x83)
      {
        tpage_image_load(tile_tpage_get(sub_number - 0x80), *(&g_LoadBuffer));
      }
}

void resource_tim_reset(void);  /* TU defines this below */
s32 resource_pair_find(s32 *, s32);
s32 inflate_buffer(s32 source, s32 destination);  /* extern */
s32 palette_bg_cover_rows_set(s32, s32);

/* func_80020A08 */
void resource_palette_load(s32 command_word, s32 unused, s32 payload)
{
  s32 first_row;
  s32 sub_number;
  sub_number = (command_word >> 8) & ((int) 0xFF);
  first_row = 0;
  inflate_buffer(payload, g_LoadBuffer);
  if (sub_number == first_row)
  {
    palette_zero_fix((s16 *)g_LoadBuffer, 0x200, 0xFF);
    palette_rows_set(first_row, 2, g_LoadBuffer);
    palette_bg_second_row_set(8, 1, g_LoadBuffer);
    palette_bg_cover_rows_set(0, g_LoadBuffer);
    return;
  }
  if (sub_number < 0x80)
  {
    palette_bit15_set((u16 *)g_LoadBuffer, 0x100, 0xFF);
    first_row = 6;
    goto block_6;
  }
  if (sub_number == 0x80)
  {
    palette_bit15_set((u16 *)g_LoadBuffer, 0x100, 0xFF);
    first_row = 1;
    block_6:
    palette_rows_set(first_row, 1, g_LoadBuffer);

  }
}

void resource_tim_reset(void);  /* TU defines this below */
s32 resource_pair_find(s32 *, s32);
s32 inflate_buffer(s32 source, s32 destination);  /* extern */
s32 palette_rows_set(s32, s32, s32);
void palette_bit15_set(u16 *, s32, s32);

/* func_80020ACC */
void resource_palette_row_load(s32 command_word, s32 unused, s32 payload)
{
  s32 row;
  row = (command_word >> 8) & 0xFF;
  inflate_buffer(payload, g_LoadBuffer);
  palette_bit15_set((u16 *)g_LoadBuffer, 0x100, 0xFF);
  if (row == 7)
  {
    palette_glyph_zero_set(g_LoadBuffer, 0x100);
  }
  palette_rows_set(row, 1, g_LoadBuffer);
}
extern s32 g_BgDataBuffer;

/* func_80020B34 */
void resource_bg_set(s32 unused, s32 byte_count, s32 payload) {
    resource_words_copy(g_BgDataBuffer, payload, byte_count);
}

extern s32 g_BgAttributeDataBuffer;

/* func_80020B60 */
void resource_bg_attribute_set(s32 unused, s32 byte_count, s32 payload) {
    resource_words_copy(g_BgAttributeDataBuffer, payload, byte_count);
}

/* Ruling of 2026-10-14 (docs/naming.md): Its purpose was read in this pass; named as the other resource commands are. */
/* func_80020B8C */
void resource_map_enter_table_set(s32 unused, s32 byte_count, s32 payload) {
    if (g_MapResourceSw != 0) {
        resource_words_copy(MapEnterTableAddress(), payload, byte_count);
    }
}

extern s32 g_BgSecondBuffer;

/* func_80020BD8 */
void resource_bg_second_set(s32 unused, s32 unused2, s32 payload) {
    inflate_buffer(payload, g_BgSecondBuffer);
}
/* func_80020BFC */
void resource_tile_animation_set(s32 unused, s32 unused2, s32 payload) {
    inflate_buffer(payload, 0x801C1B00);
}
/* func_80020C24 */
s32 resource_tile_pattern_set(s32 command_word, s32 unused, s32 payload) {
    return tile_pattern_set((command_word >> 8) & 0xFF, payload);
}

/* func_80020C4C */
void resource_tim_reset(void) {
    g_ResourceTimCount = 0;
}
/* func_80020C58 */
s32 resource_pair_find(s32 *pairs, s32 key)
{
    s16 *p = (s16 *)pairs;
    while (p[1] != 0) {
        if (p[0] == key)
            return p[1];
        p += 2;
    }
    return -1;
}
/* func_80020C94 */
int resource_pair_key_find(short *a, int b)
{
    while (a[1] != 0)
    {
        if (a[1] == b)
            return a[0];
        a += 2;
    }
    return -1;
}
