/* Lunar SSSC — btl_draw: the battle's picture: its background's grey and
   wave, BtlFrameDraw (a battle frame from the drawing to the swap), its
   data load and the party's images in the sprite pages. The first split had
   it in rendering.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_SpritePageCorners (target reaches it %hi/%lo) */

#include "include_asm.h"
/* Sony's libgpu.h declares GetClut as returning u_short, and a caller compiled under that
   masks the result to 16 bits before using it as a word. sprite_draw uses it unmasked
   (`move a2,v0`), so the original of this file called GetClut with no such prototype in
   scope. The header's declaration is renamed out of the way for this file, and the function
   is declared below as it is called here. */
#define GetClut GetClut_sdk_prototype
#include "psyq_types.h"
#include "bg.h"
#include "icon.h"
#include "sprite.h"
#include "packet.h"
#include "btl.h"
#undef GetClut
s32 GetClut(s32, s32);
extern u32 g_SpriteCols[];
extern u8 g_SpritePageCorners[];
extern s32 g_LoadBuffer;
extern long HWD0;
extern long VWD0;
extern u8 g_BtlBgWaveTable[];
extern unsigned char *g_ResourceReadBuffer;
extern s32 g_BtlBgGrey;
extern s32 g_BtlBgWave;
extern s32 g_BtlPage4ImageRows;
s32 ankprint_draw(s32);
void frame_start();
void BtlBgGreySet(s32 grey);
extern u16 g_SpritePageTPages[];
s32 sprite_grey_draw(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
s32 BtlBgWaveDraw(s32, s32, s32, s32);
s32 sprite_list_draw();
void layer_col_fade_step();
s32 frame_end();
void BtlFrameDraw(void);
u32 BtlPartyImageHeightGet(void *entry);
s32 BtlPage4RoomGet(void);
void BtlBgWaveSet(s32 amplitude, s32 phase);
s32 resource_load(s32, u16 *);
s32 BtlPartyImageAdd(s32 entry_address, s32 sprite_data_address);
void BtlPartyDataLoad(void);
void BtlPartyImagePage4Add(s32 entry_address, s32 sprite_data_address);
void frame_start();
void BtlBgGreySet(s32 grey);
extern u16 g_SpritePageTPages[];
s32 sprite_grey_draw(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
s32 BtlBgWaveDraw(s32, s32, s32, s32);
s32 sprite_list_draw();
void layer_col_fade_step();
s32 frame_end();
void BtlFrameDraw(void);
u32 BtlPartyImageHeightGet(void *entry);
s32 BtlPage4RoomGet(void);
extern long HWD0, VWD0;
s32 inflate_buffer(u8 *, u8 *);
extern long HWD0, VWD0;
void frame_start();
void BtlBgWaveSet(s32 amplitude, s32 phase);
s32 resource_load(s32, u16 *);
s32 BtlPartyImageAdd(s32, s32);
void BtlPartyDataLoad(void);
extern long HWD0, VWD0;
/* far_syms: g_SpriteCols (target reaches it %hi/%lo) */
void BtlPartyImagePage4Add(s32 entry_address, s32 sprite_data_address);
void sprite_quad_draw(s16, s16, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, u16, u16, s32, s32, u16, s32, s32, s32, s32);

/* func_8001835C */
void BtlBgGreySet(s32 grey) {
    g_BtlBgGrey = grey;
}

/* func_80018368 */
void BtlBgWaveSet(s32 amplitude, s32 phase) {
    s32 wave_times_8;

    wave_times_8 = amplitude * *(s8 *)(phase + g_BtlBgWaveTable);
    if (wave_times_8 < 0) {
        wave_times_8 += 7;
    }
    g_BtlBgWave = wave_times_8 >> 3;
}
/* The battle arena's SHEAR draw: BtlFrameDraw calls it every frame, in place
   of the two flat sprite_grey_draw sprites, while the shake amount g_BtlBgWave is
   non-zero -- (g_BtlBgWave, 0, 0x100, 0) for the left texture page and
   (g_BtlBgWave, 0x100, 0x40, 1) for the right one.  arg0 is the shear amount a,
   screen_x the screen x, width the width w, page the page index into g_SpritePageTPages.

   Colour-table entry 0 (g_SpriteCols[0], sprite_quad_draw's col 0) is swapped for
   the arena colour g_BtlBgGrey for the duration and restored at the end.  The
   224 lines are drawn as 14 free quads of 16 lines through sprite_quad_draw, two
   per pass of i = 0, 0x20 .. 0xC0: the strip at y = i has its top edge moved
   right by a and its bottom edge left by a -- corners (a, 0) (w + a, 0) above,
   (-a, 0x10) (w - a, 0x10) below -- and the strip at y = i + 0x10 is the
   mirror, so the edges meet and the picture zig-zags.  Each strip takes the
   w x 16 texels at (0, its y) of the page, CLUT GetClut(0, 0x1F4),
   opaque, ordering-table slot 0x300, scale 1:1 (0x100, 0x100), no flips.

   The u8 copy of the page index is taken before the CLUT call; a, x and w
   after it. */
/* func_800183A0 */
s32 BtlBgWaveDraw(s32 wave, s32 screen_x, s32 width, s32 page)
{
    s32 a;
    s32 neg;
    s32 sum;
    s32 diff;
    s16 x;
    s16 w;
    u8 tp;
    u16 clut;
    u32 old;
    s32 i;

    tp = page;
    clut = GetClut(0, 0x1F4);
    a = wave;
    neg = -a;
    sum = width + a;
    diff = width - a;
    x = screen_x;
    w = width;
    old = g_SpriteCols[0];
    g_SpriteCols[0] = g_BtlBgGrey;
    for (i = 0; i < 0xE0; i += 0x20) {
        sprite_quad_draw(x, i, a, 0, sum, 0, diff, 0x10, neg, 0x10, 0, i, w, 0x10, tp, clut, 0, 0, 0x300, 0, 0, 0x100, 0x100);
        sprite_quad_draw(x, i + 0x10, neg, 0, diff, 0, sum, 0x10, a, 0x10, 0, i + 0x10, w, 0x10, tp, clut, 0, 0, 0x300, 0, 0, 0x100, 0x100);
    }
    g_SpriteCols[0] = old;
}

extern u16 g_SpritePageTPages[];  /* field group base (target width u16; reabsorbed: pointer offsets) */

s32 sprite_grey_draw(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32); /* extern */
s32 BtlBgWaveDraw(s32, s32, s32, s32);                      /* extern */
s32 sprite_list_draw();                                  /* extern */
void layer_col_fade_step();                                  /* extern */
s32 frame_end();                                  /* extern */

/* func_80018564 */
void BtlFrameDraw(void) {
    sprite_list_draw();
    layer_col_fade_step();
    if (g_BtlBgWave == 0) {
        sprite_grey_draw(0, 0, 0, 0, 0x100, 0xF0, (s32) g_SpritePageTPages[0], GetClut(0, 0x1F4) & 0xFFFF, 0, g_BtlBgGrey, 0x300);
        sprite_grey_draw(0x100, 0, 0, 0, 0x50, 0xF0, (s32) g_SpritePageTPages[1], GetClut(0, 0x1F4) & 0xFFFF, 0, g_BtlBgGrey, 0x300);
    } else {
        BtlBgWaveDraw(g_BtlBgWave, 0, 0x100, 0);
        BtlBgWaveDraw(g_BtlBgWave, 0x100, 0x40, 1);
    }
    ankprint_draw(0x30);
    frame_end();
    frame_start();
}
extern u16 g_BtlDataResources[];

/* LOADS ONE BATTLE GRAPHICS SET (battle.md "The set bundle"). arg0 is the
   set number; the one caller is BtlEnemyLoad (btl_init.c), with
   g_BtlEnemySet. The set's resource number is g_BtlDataResources[enemy_set], a u16 a set,
   and 0 means the set has no bundle: nothing is loaded and g_BtlPage4ImageRows
   keeps the value it had. Otherwise the bundle is read (resource_load) to the
   work buffer g_ResourceReadBuffer and its header, u16 words, is used in place:
     hdr[8], hdr[9]   word offsets of the two CLUTs, sent to VRAM at
                      (0, 500) and (0, 505) (LoadClut);
     hdr[10 + 2n], hdr[11 + 2n], n = 0..4
                      page n's code and word offset; a code of 0 ends the
                      list. The page is inflated (inflate_buffer) to
                      g_LoadBuffer and sent (LoadImage) to the slot
                      g_SpritePageCorners[n], 0x80 wide and (code >> 8) * 4 lines
                      high.
   Last, g_BtlPage4ImageRows = (hdr[18] >> 8) * 4: page 4's lines, read from the
   header whether or not the loop got as far as that page. No RNG draw and
   no frame wait of its own (resource_load reads the disc). Every address handed
   to a callee is formed from g_ResourceReadBuffer / g_LoadBuffer as read again from
   memory at that call; the header is read through the copy taken on entry.

   Source shapes the bytes need (each scored; the mechanisms were read from
   the compiler's own pass dumps):
   - The page loop is a `for` over the counter, with the header and the
     slot table INDEXED by it. Strength reduction then makes the two
     walking registers of the target ($s1 = hdr with the fields at 0x14 /
     0x16, $s0 = the slot table), puts the table's address right before
     the loop, and leaves `i` used only by its own increment and the count
     test, so the scheduler lifts `i++` to the top of the body and it
     lands in the delay slot of the page-code test. Written with pointer
     locals that are stepped by hand the same loop is 61-72/74 and reached 74 only through an if / else with
     identical arms plus a do { } while (0).
   - The table entry is READ AT BOTH USES, not held in a local. The second
     read is a load straight into $a0 until after reload, where the
     compiler finds the value still in $v0 and makes it `move $a0, $v0`;
     an int local for the entry is loaded into $a0 at once (66/74).
   - LoadClut's x is the literal 0 and `i = 0` is the loop's own: the
     scheduler lifts `i = 0` (and the copy of hdr into the walker) above
     the resource_load call, and the post-reload pass then turns both `li $a1,
     0` into `move $a1, $s2`. `i = 0` written before the call and `i`
     passed is 72-73/74.
   - hdr is taken BEFORE the entry test (the target's `lw $s3` sits above
     the `beqz`); taken after it, 70/74.
   Byte-neutral here: an `if (entry != 0) { }` around the body instead of
   the early return; `i < 5 && hdr[10 + i * 2] != 0` as the loop condition
   instead of the break; the RECT declared in the loop body; `* 4` for
   `<< 2`; a SpritePageCorner pointer local for the table. */
/* func_80018694 */
void BtlDataLoad(s32 enemy_set)
{
    RECT rect;
    u16 *hdr;
    s32 i;

    hdr = (u16 *)g_ResourceReadBuffer;
    if (g_BtlDataResources[enemy_set] == 0) {
        return;
    }
    resource_load(g_BtlDataResources[enemy_set], hdr);
    LoadClut(g_ResourceReadBuffer + hdr[8] * 4, 0, 0x1F4);
    LoadClut(g_ResourceReadBuffer + hdr[9] * 4, 0, 0x1F9);
    for (i = 0; i < 5; i++) {
        if (hdr[10 + i * 2] == 0) {
            break;
        }
        inflate_buffer(g_ResourceReadBuffer + hdr[11 + i * 2] * 4, (u8 *)g_LoadBuffer);
        rect.x = ((SpritePageCorner *)g_SpritePageCorners)[i].x;
        rect.y = ((SpritePageCorner *)g_SpritePageCorners)[i].y;
        rect.w = 0x80;
        rect.h = (hdr[10 + i * 2] >> 8) << 2;
        LoadImage(&rect, (u_long *)g_LoadBuffer);
    }
    g_BtlPage4ImageRows = (hdr[18] >> 8) << 2;
}

/* func_800187BC */
u32 BtlPartyImageHeightGet(void *entry) {
    return ((BtlPartyDataRow *)entry)->image_height >> 8;
}

/* func_800187C8 */
s32 BtlPage4RoomGet(void) {
    return 0x100 - g_BtlPage4ImageRows;
}
/* far_syms: g_SpritePageCorners */

extern u8 g_BtlSpritePageUseMap[];

/* One g_SpritePageCorners entry: where a page slot sits in VRAM. */

/* One 12-byte descriptor of an actor block (battle.md "The actor
   renderer"): +0x02 the page slot, +0x0A v. */

/* The party pages' VRAM allocator (battle.md "The party bundle"; callers
   BtlPartyImageAdd = (entry, blk, 0) and BtlPartyImagePage4Add = (entry, blk, 1),
   from BtlPartyLoad per party slot, and BtlPartyDataLoad = (0, 0, 0)).

   g_BtlSpritePageUseMap is the occupancy map: three columns of 0x20 units, one unit
   = 8 lines, column c being page slot 4 + c (g_SpritePageCorners[4 + c] = its VRAM
   x, y; each 0x80 wide).

   entry == 0 resets it: all three columns cleared, then the first
   g_BtlPage4ImageRows >> 3 units of column 0 marked 0xFF (the lines the enemy set's
   page 4 already uses).

   Otherwise entry is a row of the party bundle (BtlPartyDataRow, btl.h:
   image_place in words, image_height in its high byte) and blk the
   inflated actor block. The page needs
   n = ((height >> 8) + 7) >> 3 units. A column fits when its FIRST free
   unit `row` has row + n <= 0x20 (nothing checks the units after it:
   pages are only ever appended). With `first` set column 0 is tried
   first; then columns 2, 1, 0 in that order; when none fits the page goes
   to column 1, row 0, over whatever is there. The page is inflated
   (inflate_buffer) from g_ResourceReadBuffer + image_place * 4 into g_LoadBuffer and
   LoadImage'd at (x, y + row * 8), 0x80 x (height >> 8) -- the height is
   the word's high byte, not a multiple of 8. The n units are marked 1,
   and every descriptor of the block -- the 12-byte records from
   blk + texel_records * 4, (texel_records_end - texel_records) / 3 of
   them (SpriteDataHead) -- gets slot = 4 + column and v += row * 8.

   Nothing is returned ($v0 is whatever LoadImage or the loops left);
   BtlPartyImageAdd passes that through as its own result. The parameters
   are s32 because both wrappers and their caller pass s32; they are
   pointers.

   Shape:
   - `i` is ONE variable for the reset loops' unit index and the column,
     and `k` one for the mark loop and the descriptor loop: a separate
     local for either scores 110-111/152 (register assignment across the
     whole function moves).
   - The map is indexed flat with an explicit shift, `row + (i << 5)`:
     `i * 0x20` puts the multiply first in the sum (150/152 in the mark
     loop).
   - The reset loop goes through a pointer to unit i (`m = g_BtlSpritePageUseMap +
     i`): the target forms &map + i once and stores at +0x40 / +0x20 / +0;
     three direct `g_BtlSpritePageUseMap[i + 0x40]` stores get three address
     registers.
   - The table address is formed BEFORE the index (`tbl = g_SpritePageCorners`
     first), and the index is (i + 4) * 4 on a byte pointer: an array
     reference folds the 4 into the symbol (&tbl + 0x10) and computes the
     index first.
   - The descriptors are INDEXED (`desc[k]`), not walked: loop.c then
     reduces &desc[k] to one pointer stepped by 12 with offsets +2 / +0xA,
     which is the target; a walked `p++` makes p + 0xA the pointer.
   - The count is an unsigned divide (multu by 0xAAAAAAAB, srl 1) kept in
     a signed variable (slt in the loop, beqz at its entry). */
/* func_800187D8 */
s32 BtlPartyImageLoad(s32 entry_address, s32 sprite_data_address, s32 first)
{
    BtlPartyDataRow *entry = (BtlPartyDataRow *)entry_address;
    SpriteDataHead *blk = (SpriteDataHead *)sprite_data_address;
    RECT rect;
    s32 i;
    s32 row;
    s32 n;
    s32 k;
    s32 count;
    SpriteTexelRecord *desc;
    SpritePageCorner *pos;
    u8 *tbl;
    u8 *m;

    if (entry == 0) {
        for (i = 0; i < 0x20; i++) {
            m = g_BtlSpritePageUseMap + i;
            m[0x40] = 0;
            m[0x20] = 0;
            m[0] = 0;
        }
        for (i = 0; i < g_BtlPage4ImageRows >> 3; i++) {
            g_BtlSpritePageUseMap[i] = 0xFF;
        }
    } else {
        n = ((entry->image_height >> 8) + 7) >> 3;
        if (first != 0) {
            for (row = 0; row < 0x20; row++) {
                if (g_BtlSpritePageUseMap[row] == 0) {
                    break;
                }
            }
            if (row + n <= 0x20) {
                i = 0;
                goto found;
            }
        }
        for (i = 2; i >= 0; i--) {
            for (row = 0; row < 0x20; row++) {
                if (g_BtlSpritePageUseMap[row + (i << 5)] == 0) {
                    break;
                }
            }
            if (row + n <= 0x20) {
                goto found;
            }
        }
        i = 1;
        row = 0;
    found:
        tbl = g_SpritePageCorners;
        pos = (SpritePageCorner *)(tbl + (i + 4) * 4);
        rect.x = pos->x;
        rect.y = pos->y + row * 8;
        rect.w = 0x80;
        rect.h = entry->image_height >> 8;
        inflate_buffer(g_ResourceReadBuffer + entry->image_place * 4, (u8 *)g_LoadBuffer);
        LoadImage(&rect, (u_long *)g_LoadBuffer);
        for (k = row; k < row + n; k++) {
            g_BtlSpritePageUseMap[k + (i << 5)] = 1;
        }
        desc = (SpriteTexelRecord *)((u8 *)blk + blk->texel_records * 4);
        count = (u32)(blk->texel_records_end - blk->texel_records) / 3;
        for (k = 0; k < count; k++) {
            desc[k].page = i + 4;
            desc[k].v += row * 8;
        }
    }
}

/* func_80018A38 */
s32 BtlPartyImageAdd(s32 entry_address, s32 sprite_data_address) {
    return BtlPartyImageLoad(entry_address, sprite_data_address, 0);
}
/* func_80018A58 */
void BtlPartyImagePage4Add(s32 entry_address, s32 sprite_data_address) {
    BtlPartyImageLoad(entry_address, sprite_data_address, 1);
}

s32 resource_load(s32, u16 *);
s32 BtlPartyImageAdd(s32, s32);

/* func_80018A78 */
void BtlPartyDataLoad(void)
{
  u16 *first_col;
  u16 *header;
  volatile int pad;
  header = g_ResourceReadBuffer;
  resource_load(0x197, header);
  first_col = g_ResourceReadBuffer + ((*header) & 0xFFFE);
  *first_col = 0;
  LoadClut(g_ResourceReadBuffer + ((*header) & 0xFFFC), 0, 0x1F8);
  BtlPartyImageAdd(0, 0);
}
