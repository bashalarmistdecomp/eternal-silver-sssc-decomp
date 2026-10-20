/* Lunar SSSC — menu_option: the option page: its frame, value boxes,
   values, text, open and close. The first split had it in battle.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "game.h"
#include "chr.h"
#include "menu.h"
#include "map.h"

extern void packet_box_add(s32 left_x, s32 top_y, s32 width, s32 height, s32 col, s32 z);

/* Entry in the g_MenuOptionEntries table (0xA-byte stride). unk0/unk1 are tile
   coordinates (drawn at *8 pixels), unk3 the pixel width of the entry's box
   (MenuOptionValueBoxesDraw draws a rectangle that wide), unk8 a base index added
   before a second lookup into the same table. */

extern u32 g_MenuBackOutLatch;
extern u16 g_MenuPageStage;
/* 4-byte word: read as `lw` for value tests and as `lhu` (low half) where
   it feeds MenuPageDisp — one s32 declaration covers both via a cast. */
extern s32 g_MenuPage;
extern SpritePiece *g_MenuShapes;         /* the field menu's shape pieces */
extern MenuOptionEntry g_MenuOptionEntries[];  /* far array, 0xA-byte stride -> lui/%lo */
extern u8 D_800B8F89[];     
extern void sprite_run_off(s32 first, s32 count);
extern void MenuFrameStep(void);
extern void font_target_clear(u8 *target, s32 block_count);
extern void MenuPageDisp(s32 page);
extern void font_run_show(s32 column, s32 row, s32 block_count, s32 place, u8 *source);
extern void font_target_set(u8 *target);
extern s32 font_col_set(s32 col_bits);
extern void font_string_proportional_put(s32 string_id);
extern void MenuTextPartsLoad(s32 part_bits);
/* gp word holding the address of the seven option bytes. */
extern u8 *g_GameOptions;
/* The option page's working copy of the seven option bytes.  Sized (so GCC
   materialises its address in one register) AND in far_syms.txt (so -G8
   does not make it gp-relative). */
extern u8 g_MenuOptions[7];
extern s32 MusicPlay(u8 id, s32 unused, u8 vol);
extern u16 g_MusicNumber;     

extern s32 g_PartyIds;     extern s32 g_MapPartyRecords;     /* base pointer (gp-relative word) */
extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  extern u16 g_MenuMemberCursor;      /* gp side of the TU wall: 7 gp users vs 1 (MenuUseDrawAreaOpen) %hi/%lo */
extern u16 g_MenuRedrawRequest;
extern void MenuIconBytesDisp(u16 member_cursor, s32 mode, s32 row_count, s32 cursor);
extern void MenuCommandsSet(s32, u16);
extern void MenuCommandsDisp(s32);
extern void MenuIconBytesSet(s32, s32);
/* Script-side movement projection: steps (*px, *py) `dist` tiles along `dir`
   using the actor's probe rect at `probe`, returns the facing actually used. */
extern s16 g_MenuSoundRequest;
extern u16 g_MenuPadRecord[];
extern s32 g_MenuBackOutPattern;
extern s16 g_MenuSoundRequest;
extern u8 g_MenuOptions[7];
extern u16 g_MenuPadRecord[];
extern u16 g_MenuPadRecord[];
extern s32 g_MenuBackOutPattern;
extern s16 g_MenuSoundRequest;
extern u8 g_MenuOptions[7];
extern u16 g_MenuPadRecord[];
extern s32 g_MenuBackOutPattern;
extern s16 g_MenuSoundRequest;

/* la_absolute: MenuOptionStep */
extern u8 D_800B8F89_s;
extern u8 D_800B8F8A;
extern u16 g_MenuOptionRow[1];
extern u8 g_MenuOptions[7];
extern void font_cell_switch_set(s32 on);
void MenuOptionOpen(void);  /* TU defines this below */
s32 MenuOptionValuesPut(u16 row);  /* TU defines this below */
void MenuOptionValueBoxesDraw(u16 cursor);  /* TU defines this below */
void MenuOptionClose(void);  /* TU defines this below */

/* The OPTIONS page of the menu: entries 0x13 and 0x15 of the menu's page
   table at 0x800B59F8 (0x800B5A44 / 0x800B5A4C), one call per frame.

   Stage g_MenuPageStage: 0 = open -- MenuOptionOpen draws the page and copies the
   live 7-byte options block *g_GameOptions into the working copy g_MenuOptions --
   then 1.

   The cursor g_MenuOptionRow is the row, 0..5; row r edits working byte r + 1
   (sel = &D_800B8F89[row]). Pad word g_MenuPadRecord[2]:
     0x8000 (left)   the value steps down: rows other than 3 toggle 0 <-> 1
                     (^ 1); row 3 has three values, 0 -> 2, else - 1.
     0x2000 (right)  steps up: toggle; row 3 goes 0, 1, 2, 0.
     0x1000 (up)     row - 1, wrapping 0 -> 5.
     0x4000 (down)   row + 1, wrapping 5 -> 0.
   Each sets the result g_MenuSoundRequest = 1 and republishes the mode
   (g_MenuRedrawRequest = g_MenuPage).

   Otherwise g_MenuPadRecord[1] & 0x170 leaves the page: g_MenuBackOutLatch = 1 for bit
   0x100, 2 for bit 0x20 (unchanged for 0x40 / 0x10 alone), g_MenuBackOutPattern =
   0x30B30B30, and the working copy is applied:
     byte 1 changed -> MusicPlay(0, 2, 0), one frame (MenuFrameStep),
                       the byte is stored, SoundMonoSet(), and the current
                       track g_MusicNumber is started again
                       (MusicPlay(track, 0, 0));
     byte 2 changed -> stored, then ChrItemListSort(1..12): every character's
                       item list is sorted again (battle.md "Item-list
                       sort" -- byte 2 is its order option);
     then all 7 bytes are copied to *g_GameOptions (so bytes 3..6 -- 5 is the
     vibration option -- take effect without more ado), MenuOptionClose
     closes the page, the stage goes back to 0 and g_MenuSoundRequest = 2.

   Every call ends with MenuOptionValueBoxesDraw(row) (the row highlight); when the
   published mode g_MenuRedrawRequest is 0x13 or 0x15 the row's text column is
   redrawn (MenuTextPartsLoad(MenuOptionValuesPut(row))) and g_MenuRedrawRequest cleared; then
   MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0).

   No RNG draw of its own; the only frame wait is the MenuFrameStep on a
   changed byte 1 (MusicPlay's own frame loop, ScriptFrameStep or
   BtlFrameStep by g_MusicWaitFrameKind, is in an arm the built program never takes).

   Shape. The target loads the cursor's
   ADDRESS once (`lui $a3; addiu $a3,$a3` on one register: the assembler's
   `la` of a small-data symbol), reads the cursor %gp_rel, and in the up /
   down arms COPIES the address ($v1 = $a3, before the test) and stores
   through the copy after the arms join. That is what GCC does for an
   element of an aggregate: every `obj[0]` / `obj.field` access first puts
   the constant address in a fresh register (memory_address, for cse's
   benefit) and cse folds it back into the access only inside one extended
   basic block. A read folds to %gp_rel. A store whose value is a `?:` does
   not: the address is taken BEFORE the conditional and used AFTER the join,
   where cse knows nothing, so the register stays -- and cse turns the arm's
   second `la` into a copy of the first one in the block. Hence:
     - g_MenuOptionRow is declared `u16 [1]` (sized, so it stays small) and
       accessed as [0]; a plain scalar gives %gp_rel stores, the unsized
       far `_blk` alias folds %lo into each store (169/181).
     - each arm's assignment is ONE `?:` expression (two assignments in an
       if/else are folded to %gp_rel stores and cross-jumped).
     - la_absolute expands the `la` into the two-instruction absolute form
       the original assembler wrote.
   D_800B8F89_s is the gp-scalar alias of D_800B8F89[0] (the array itself is
   far in this TU); the g_MenuOptionRow_blk alias is not used here. */
/* func_80075410 */
void MenuOptionStep(void)
{
    u8 *sel;
    u16 *pad;
    s32 i;

    if (g_MenuPageStage == 0) {
        font_cell_switch_set(0);
        MenuOptionOpen();
        font_cell_switch_set(1);
        g_MenuPageStage = 1;
    }
    sel = &D_800B8F89[g_MenuOptionRow[0]];
    pad = g_MenuPadRecord;
    if (pad[2] & 0x8000) {
        *sel = (g_MenuOptionRow[0] != 3) ? (*sel ^ 1) : ((*sel != 0) ? (*sel - 1) : 2);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (pad[2] & 0x2000) {
        *sel = (g_MenuOptionRow[0] != 3) ? (*sel ^ 1) : ((*sel < 2) ? (*sel + 1) : 0);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (pad[2] & 0x1000) {
        g_MenuOptionRow[0] = (g_MenuOptionRow[0] != 0) ? (g_MenuOptionRow[0] - 1) : 5;
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (pad[2] & 0x4000) {
        g_MenuOptionRow[0] = (g_MenuOptionRow[0] < 5) ? (g_MenuOptionRow[0] + 1) : 0;
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (pad[1] & 0x170) {
        if (pad[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (pad[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        if (g_GameOptions[1] != D_800B8F89_s) {
            MusicPlay(0, 2, 0);
            MenuFrameStep();
            g_GameOptions[1] = D_800B8F89_s;
            SoundMonoSet();
            MusicPlay(g_MusicNumber, 0, 0);
        }
        if (g_GameOptions[2] != D_800B8F8A) {
            g_GameOptions[2] = D_800B8F8A;
            for (i = 1; i < 0xD; i++) {
                ChrItemListSort(i & 0xFFFF);
            }
        }
        memcpy(g_GameOptions, g_MenuOptions, 7);
        font_cell_switch_set(0);
        MenuOptionClose();
        font_cell_switch_set(1);
        g_MenuPageStage = 0;
        g_MenuSoundRequest = 2;
        g_MenuRedrawRequest = g_MenuPage;
    }
    MenuOptionValueBoxesDraw(g_MenuOptionRow[0]);
    if (g_MenuRedrawRequest == 0x13 || g_MenuRedrawRequest == 0x15) {
        MenuTextPartsLoad(MenuOptionValuesPut(g_MenuOptionRow[0]) & 0xFFFF);
        g_MenuRedrawRequest = 0;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0);
}

/* The OPTIONS page's value marks: MenuOptionStep calls this once per frame
   with the cursor row (0..5).

   For each of the six option rows i it queues one flat rectangle
   (packet_box_add(x, y, w, h, rgb, ot slot): an opaque POLY_F4) behind the
   label of the row's CURRENT setting. The label's layout entry is
   g_MenuOptionEntries[first + value]: `first` is the unk8 of the row's header
   g_MenuOptionEntries[i + 1] (the header MenuOptionValuesPut reads for the same row),
   `value` the working options byte g_MenuOptions[1 + i]. The rectangle is at
   (unk0 * 8, unk1 * 8), unk3 pixels wide and 0x12 high, in ordering-table
   slot 0x40, coloured 0x404080 (r 0x80: red) on the cursor's row and
   0x408040 (g 0x80: green) on the other five. No state is written.

   Shape. The header index is `i + 1` and the options byte comes through a
   walked pointer: cse turns the loop's `i++` into a copy of that `i + 1`,
   so loop.c sees no induction variable in `i` and the header address is
   recomputed (`* 10`) every pass, as the target does; an early `i++` with
   `g_MenuOptionEntries[i]` gives a second walked pointer instead (31/50).
   `row = cursor` is its own statement after `i = 0`: the u16
   parameter read inside the loop is masked on every pass (loop.c: "life 1,
   savings 1 not desirable"), where the target masks once. The pointer
   starts from the sized block g_MenuOptions, not the unsized D_800B8F89:
   one `la`, expanded on a single register. */
/* func_800756E4 */
void MenuOptionValueBoxesDraw(u16 cursor)
{
    MenuOptionEntry *e;
    u8 *opt;
    s32 color;
    s32 row;
    s32 i;

    opt = &g_MenuOptions[1];
    for (i = 0, row = cursor; i < 6; i++) {
        color = (i == row) ? 0x404080 : 0x408040;
        e = &g_MenuOptionEntries[g_MenuOptionEntries[i + 1].first_value + *opt++];
        packet_box_add(e->column * 8, e->row * 8, e->width, 0x12, color, 0x40);
    }
}

/* Draw the text/glyph column for the menu row given as row.  The row header at
   g_MenuOptionEntries[row+1] gives a first child index (unk8) and a child count
   (unk9); each child is drawn into the VRAM page 0x801CD500 + (unk4<<7),
   highlighted (clip mode 0 vs 4) when its ordinal equals the selected index
   D_800B8F89[row], and the pen advances by the unk2 byte of a *second*
   row of entries at row*2 + 5 + i.  font_col_set's previous mode is saved
   across the whole run and restored at the end.

   `slot = i + 5` must stay its own statement: written inline as
   `g_MenuOptionEntries[row*2 + i + 5]`, GCC folds the 5 into the loop invariant and
   hoists `row*2 + 5`, where the target hoists only `row*2` and adds 5
   inside the loop. */
/* func_800757AC */
s32 MenuOptionValuesPut(u16 row) {
    MenuOptionEntry *e;
    u8 *sel;
    u8 *dst;
    u8 saved;
    u8 cur;
    s32 count;
    s32 glyph;
    s32 i;
    s32 acc;
    s32 slot;

    sel = &D_800B8F89[row];
    saved = font_col_set(0);
    acc = 0;
    cur = g_MenuOptionEntries[row + 1].first_value;
    count = g_MenuOptionEntries[row + 1].value_count;
    for (i = 0; i < count; i++) {
        e = &g_MenuOptionEntries[cur];
        dst = (u8 *)((e->block << 7) + 0x801CD500);
        glyph = e->string;
        font_target_clear(dst, e->block_count);
        cur++;
        font_col_set((i != *sel) * 4);
        font_string_proportional_put(glyph);
        slot = i + 5;
        acc += g_MenuOptionEntries[row * 2 + slot].block_count;
        font_target_set(dst + (acc << 7));
    }
    font_col_set(saved);
    return 0x1F0;
}
/* Redraw all 20 sprite-table entries.  Each entry's unk4 is a VRAM page
   index; the destination is 0x801CBB00 + ((unk4 + 0x34) << 7), i.e. the
   same 0x801CD500 base the rest of the table uses, spelled with the 0x34
   bias folded into the shift.

   Walk the table with a `MenuOptionEntry *`, NOT `g_MenuOptionEntries[i]`: the indexed
   form makes GCC keep a single i*10 induction variable, which is four
   instructions short and drops the 0x801CBB00 hoist.  With the pointer,
   GCC splits it into the two walking IVs the target uses (one at +0 for
   unk0, one at +2 covering unk1/unk2/unk4) and hoists the constant. */
/* func_80075900 */
void MenuOptionTextDisp(void) {
    MenuOptionEntry *e;
    s32 i;
    s32 page;

    e = g_MenuOptionEntries;
    i = 0;
    do {
        page = e->block + 0x34;
        font_run_show(e->column, e->row, e->block_count, page,
                      (u8 *)((page << 7) + 0x801CBB00));
        e++;
        i++;
    } while (i < 0x14);
}
/* Sets the option page up (pages 0x13 and 0x15).  Sizes the g_MenuShapes
   shape (0x3F extents, 0xF row heights), clears byte 0 of the party record
   of every non-empty party slot, then MenuPageDisp(0xFFFF), the page's
   command (MenuCommandsSet / MenuCommandsDisp) and icons (MenuIconBytesSet /
   MenuIconBytesDisp(0xFFFF, 1, 2, 0xC)) for the page g_MenuPage.  It copies
   the seven option bytes at *g_GameOptions into the working copy g_MenuOptions,
   writes string 0x1FD and the six rows' names from g_MenuOptionEntries[1..6] into the
   text at 0x801CD500 + (unk4 << 7), writes all six rows' values via
   MenuOptionValuesPut, and finally loads the text runs (MenuTextPartsLoad(0x55)),
   shows the page and lets one frame pass.

   g_MenuOptions is declared with its real size (7) *and* listed in far_syms.txt:
   at 7 bytes `-G8` would otherwise make it gp-relative, but the target
   addresses it with lui %hi + %lo.  Declaring it as an incomplete array
   `[]` also gives %hi/%lo but splits the address across two registers
   (96/104); the sized-array + far_syms pair is what matches. */
/* func_80075988 */
void MenuOptionOpen(void) {
    s32 i;
    s32 off;

    g_MenuShapes[2].x2 = 0x3F;
    g_MenuShapes[2].x1 = 0x3F;
    g_MenuShapes[2].y3 = 0xF;
    g_MenuShapes[2].y2 = 0xF;
    i = 0;
    off = 0;
    do {
        if (*(u16 *)(g_PartyIds + i * 2) != 0) {
            *(u8 *)(off + g_MapPartyRecords) = 0;
        }
        i++;
        off += 0x3C;
    } while (i < 6);
    sprite_run_off(0x87, 8);
    MenuPageDisp(0xFFFF);
    MenuCommandsSet(0, (u16) g_MenuPage);
    i = 1;
    MenuCommandsDisp(0);
    MenuIconBytesSet(0, (u16) g_MenuPage);
    MenuIconBytesDisp(0xFFFF, i, 2, 0xC);
    memcpy(g_MenuOptions, g_GameOptions, 7);
    font_target_clear((u8 *)0x801CD500, 0x4C);
    font_string_proportional_put(0x1FD);
    do {
        font_target_set((u8 *)((g_MenuOptionEntries[i].block << 7) + 0x801CD500));
        font_string_proportional_put(g_MenuOptionEntries[i].string);
        i++;
    } while (i < 7);
    i = 0;
    do {
        MenuOptionValuesPut(i);
        i++;
    } while (i < 6);
    MenuTextPartsLoad(0x55);
    MenuPageDisp((u16) g_MenuPage);
    MenuFrameStep();
}
/* Puts the option page away: its sprites off, its text cleared and loaded
   again.  Unless the page g_MenuPage is 0x15 (in which case it becomes 0 and
   MenuPageDisp gets 0xFFFF), it copies each non-zero party id from the u16
   list at *g_PartyIds into byte 0 of its record in the 0x3C-stride table at
   *g_MapPartyRecords, sets the page to 7 and shows it.
   `i` must be zeroed before `off` — assignment order sets the copy
   direction of the shared zero ($a0 -> $a1, not the reverse). */
/* func_80075B28 */
void MenuOptionClose(void) {
    s32 i;
    s32 off;
    u16 arg;

    sprite_run_off(0x87, 8);
    font_target_clear((u8 *)0x801CD500, 0x3C);
    MenuTextPartsLoad(0x55);
    if (g_MenuPage != 0x15) {
        i = 0;
        off = 0;
        do {
            if (*(u16 *)(g_PartyIds + i * 2) != 0) {
                *(u8 *)(off + g_MapPartyRecords) = *(u16 *)(g_PartyIds + i * 2);
            }
            i++;
            off += 0x3C;
        } while (i < 6);
        g_MenuPage = 7;
        arg = (u16)g_MenuPage;
    } else {
        g_MenuPage = 0;
        arg = 0xFFFF;
    }
    MenuPageDisp(arg);
}
