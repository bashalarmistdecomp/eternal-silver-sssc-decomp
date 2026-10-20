/* Lunar SSSC — menu_places: the battle-places page's display, open and
   close (its step ends menu_page.c). The first split had it in battle.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "game.h"
#include "chr.h"
#include "menu.h"
#include "map.h"

/* 4-byte word: read as `lw` for value tests and as `lhu` (low half) where
   it feeds MenuPageDisp — one s32 declaration covers both via a cast. */
extern s32 g_MenuPage;
extern SpriteEntry *g_MenuRenderEntries;   /* the field menu's sprite entries */
extern SpritePiece *g_MenuShapes;         
extern void sprite_run_off(s32 first, s32 count);
extern void MenuFrameStep(void);
extern void font_target_clear(u8 *target, s32 block_count);
extern void MenuPageDisp(s32 page);
extern void font_target_set(u8 *target);
extern void font_string_proportional_put(s32 string_id);
extern void MenuTextPartsLoad(s32 part_bits);

extern void MapAnimationPoseSet(MapObject *record, s32 pose, s32 facing);

extern s32 g_PartyIds;     extern s32 g_MapPartyRecords;     /* base pointer (gp-relative word) */
extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  extern s16 g_MapViewX;
extern s16 g_MapViewY;
extern void MenuIconBytesDisp(u16 member_cursor, s32 mode, s32 row_count, s32 cursor);
extern s32 g_SpriteList;
extern s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
/* GCC 2.8.1 emits its 4-word block-move loop + tail word for a 0x24-byte
   struct assignment; used by MenuBtlPlacesDisp to copy BattleEntity+0x48. */
extern void *g_MenuRecords;    /* gp side of the TU wall */
extern void MenuIconBytesSet(s32, s32);
extern void MenuLineUpSpotsMove(void);
/* Puts the text of the menu's page 0xA (the battle places, MenuBtlPlacesStep)
   on the screen: MenuPageDisp of the page and three strips of text.  In mode
   3 or more (the prompt) it adds the prompt's three strips, stores the bar's
   row Y (g_MapViewY + cursor * 0x10 + 0x98) into BattleEntity+0x4E (render
   entry 2) and then copies that 0x24-byte entry at +0x48 into sprite-list
   entry 0x87 at g_SpriteList+0x12FC.

   The copy is a plain struct assignment — the lw/lw/lw/lw + sw/sw/sw/sw
   loop with a tail word in the target is GCC's block-move for 0x24 bytes,
   not a hand-written loop (m2c renders it as one, which does not match).
   `dst`/`src` must be hoisted into temps *before* the 0x4E store so their
   address arithmetic is emitted ahead of it, as the target schedules it. */
/* func_800722BC */
void MenuBtlPlacesDisp(s32 mode, u32 cursor) {
    SpriteEntry *dst;
    SpriteEntry *src;

    sprite_run_off(0x87, 8);
    MenuPageDisp((u16) g_MenuPage);
    CellTextDisp(0xB, 5, 2, 1, 3, 0xDEAD2001, 0xEE00);
    CellTextDisp(0x18, 5, 3, 1, 3, 0xDEAD2001, 0xEE08);
    CellTextDisp(0xF, 2, 6, 1, 6, 0xDEAD2001, 0xEE14);
    if ((u32) (mode & 0xFFFF) >= 3U) {
        CellTextDisp(0x15, 0x13, 2, 1, 3, 0xDEAD2001, 0xEDD0);
        CellTextDisp(0x15, 0x15, 3, 1, 3, 0xDEAD2001, 0xEDD8);
        CellTextDisp(0x15, 0x17, 4, 1, 3, 0xDEAD2001, 0xEDE4);
        dst = (SpriteEntry *)((u8 *)g_SpriteList + 0x12FC);
        src = (SpriteEntry *)((u8 *)g_MenuRenderEntries + 0x48);
        g_MenuRenderEntries[2].y =
            (s16) (g_MapViewY + (((cursor & 0xFFFF) * 0x10) + 0x98));
        *dst = *src;
    }
}
/* la_absolute: MenuBtlPlacesOpen */
extern s32 g_ChrPlacePresetRecords;
extern const u8 g_MenuMemberPlaces[];
extern u8 g_MenuBtlPlacesXCells[6];
extern u8 g_MenuBtlPlacesYCells[6];
extern void MenuLineUpMove(u16 *xs, u16 *ys);

/* Opens the party formation editor (mode 0 of MenuBtlPlacesStep).  Seats every
   member: display order i picks slot k = g_MenuMemberPlaces[(i + 1) % 6]; a real
   member (id neither 0 nor 0xC) gets its formation cell copied from its
   g_ChrPlacePresetRecords record into g_MenuBtlPlacesXCells[k] / g_MenuBtlPlacesYCells[k] and its screen
   position derived from it (x = g_MapViewX + (cell x - 3) * 8, y =
   g_MapViewY + (cell y - 6) * 8); the guest (0xC) keeps the position its
   display record already has and has that record's +4 cleared.  The two
   coordinate lists go to MenuLineUpMove.  Then every real member's sprite
   record is copied into the list at g_SpriteList + 0xD8, moved 0x70 left,
   with +0x18 taken from the current row of the slot's frame table; the
   two records at g_MenuRenderEntries + 0x24 and + 0 close the list as entries 6
   and 7.

   Codegen notes:
   - `mode` is read before the five stores: the target's lhu is the first
     instruction after the sprite_run_off call, and no store through a
     pointer can be passed by a later load.
   - g_MenuBtlPlacesXCells / g_MenuBtlPlacesYCells are sized small arrays reached through one
     `la` each (the la_absolute marker): the target keeps both lui/addiu
     pairs INSIDE the loop on one register, where a split address is
     hoisted (as g_MenuMemberPlaces's is).
   - `dst->unk2` / `dst->unk18` on the walking pointer are what make the
     second register: loop.c reduces the two address givs of the biv
     `dst`, combining them on the LAST one found (+0x18), hence
     g_SpriteList + 0xF0 with the x at -0x16.  Indexing the list by `k`
     instead leaves one giv at +0 (209/215).
   - `((MapRecord *) g_MapPartyRecords)[k]` by index, not a running offset: k * 0x3C
     is the reduced giv, and the target's `move s2, s0` is its start. */
/* func_80072464 */
void MenuBtlPlacesOpen(void)
{
  u16 xs[6];
  u16 ys[6];
  s32 i;
  s32 k;
  s32 id;
  s32 mode;
  SpriteEntry *dst;

  sprite_run_off(0x87, 8);
  mode = (u16) g_MenuPage;
  g_MenuRenderEntries[2].x = g_MapViewX + 0xA8;
  g_MenuShapes->x2 = 0x3F;
  g_MenuShapes->x1 = 0x3F;
  g_MenuShapes->y3 = 0xF;
  g_MenuShapes->y2 = 0xF;
  MenuPageDisp(mode);
  MenuFrameStep();
  for (i = 0; i < 6; i++)
  {
    k = g_MenuMemberPlaces[(i + 1) % 6];
    id = ((u16 *) g_PartyIds)[k];
    if (id != 0)
    {
      if (id != 0xC)
      {
        g_MenuBtlPlacesXCells[k] = ((ChrPlacePresetRecord *) g_ChrPlacePresetRecords)[id].place_x;
        g_MenuBtlPlacesYCells[k] = ((ChrPlacePresetRecord *) g_ChrPlacePresetRecords)[id].place_y;
        xs[k] = g_MapViewX + ((g_MenuBtlPlacesXCells[k] - 3) * 8);
        ys[k] = g_MapViewY + ((g_MenuBtlPlacesYCells[k] - 6) * 8);
      }
      else
      {
        xs[k] = AS(u16, ((MapRecord *) g_MapPartyRecords)[k].sprite.x);
        ys[k] = AS(u16, ((MapRecord *) g_MapPartyRecords)[k].sprite.y);
        ((MapRecord *) g_MapPartyRecords)[k].sprite.on = 0;
      }
    }
  }
  MenuLineUpMove(xs, ys);
  dst = (SpriteEntry *) (((u8 *) g_SpriteList) + 0xD8);
  for (k = 0; k < 6; k++)
  {
    id = ((u16 *) g_PartyIds)[k];
    if ((id != 0) && (id != 0xC))
    {
      MapAnimationPoseSet((MapObject *) &((MapRecord *) g_MapPartyRecords)[k], 2, 7);
      *((SpriteEntry *) dst) = *((SpriteEntry *) &((MapRecord *) g_MapPartyRecords)[k].sprite.on);
      AS(u16, dst->x) -= 0x70;
      dst->pose = *((u16 *) (((MapRecord *) g_MapPartyRecords)[k].step_list + (((MapRecord *) g_MapPartyRecords)[k].step * 4)));
    }
    dst++;
  }
  *((SpriteEntry *) dst) = *((SpriteEntry *) (((u8 *) g_MenuRenderEntries) + 0x24));
  *((SpriteEntry *) (dst + 1)) = *((SpriteEntry *) g_MenuRenderEntries);
  font_target_clear((u8 *) 0x801CD500, 0x14);
  font_string_proportional_put(0x660);
  font_target_set((u8 *) 0x801CDB00);
  font_target_clear((u8 *) 0x801CDB00, 0xC);
  font_string_proportional_put(0x647);
  font_string_proportional_put(0x1F1);
  MenuTextPartsLoad(3);
}

/* Puts the menu's page 0xA (the battle places) away: switches its records
   and sprites off, clears its text and redraws the icons and the line-up,
   marks the party slot whose id is 0xC (the guest/NPC member) by writing
   that id into its record's +4 field, then sets the page g_MenuPage to 2
   and shows it.

   `k` holds the 0xC literal on purpose: the target keeps it in a register
   hoisted above the loop, and GCC will not do that loop-invariant motion
   for an inline constant in this goto-shaped loop (44/52).  Rewriting the
   loop as a structured do/while so the loop optimizer sees it is worse
   still (38/52) — the goto form is what the original compiled from.
   `i = 0` must also precede `k = 0xC`: the assignment order is what
   puts the counter in $a1 and the record pointer in $a0. */
/* func_800727C0 */
void MenuBtlPlacesClose(void) {
    s32 i;
    s32 record;
    s32 party_id;
    u16 chr_id;
    s32 k;

    ((MapRecord *)g_MenuRecords)[1].sprite.on = 0;
    sprite_run_off(0x87, 8);
    sprite_run_off(6, 8);
    font_target_clear((u8 *)0x801CD500, 0xC);
    MenuTextPartsLoad(1);
    MenuIconBytesSet(0, 0);
    MenuIconBytesDisp(0xFFFFU, 1, 2, 0);
    MenuPageDisp(0xFFFF);
    MenuLineUpSpotsMove();
    i = 0;
    k = 0xC;
    record = g_MapPartyRecords;
    party_id = g_PartyIds;
loop_1:
    chr_id = (*(u16 *)party_id);
    i += 1;
    if (chr_id == k) {
        ((MapRecord *)record)->sprite.on = chr_id;
    } else {
        record += 0x3C;
        party_id += 2;
        if (i < 6) {
            goto loop_1;
        }
    }
    g_MenuPage = 2;
    MenuPageDisp((s32) (u16) g_MenuPage);
}
