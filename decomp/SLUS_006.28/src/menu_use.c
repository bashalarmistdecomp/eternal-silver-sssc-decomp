/* Lunar SSSC — menu_use: an item or a spell used from the menu: the run,
   the battle's words set for it, its hang, and its run on the target, on
   the party or as a picture. The first split had it in battle.c. Every
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

extern void MenuFrameStep(void);
extern void MenuPageDisp(s32 page);
extern void mem_clear(void *destination, u32 byte_count);

extern void MenuUseMoviePlay(s32 movie);
extern void MenuUseDrawAreaOpen(void);
extern s16 g_MovieSkipAllowed;
extern s32 g_MenuUsedMark;
/* The use's entry: MenuUseRun masks the item's or spell's row byte to 0xF
   and uses it to select the function out of battle_state_table (the table
   at 0x800B5B1C).  MenuUseNumberHang prints it. */
extern s32 g_MenuUseEntry;
/* 0x18-byte debug-report template in the boot data segment; byte array on
   purpose so the copy in MenuUseNumberHang keeps its unaligned expansion. */
extern u8 g_MenuUseNumberMessage[];
extern void MessageDisp(void *message, s32 open_allowed, s32 unused, s32 unused2);
extern s32 g_MenuUseNumber;
/* 4-byte word: `lw` in MenuUseMoviePlay, low byte (`lbu`) in MenuUseActionSet. */
extern s32 g_MenuUseUserId;
extern u8 g_BtlActingSlot;       /* far_syms.txt: lui $at + sb */
extern u8 g_BtlActionKind;       /* far_syms.txt: lui $at + sb */
extern u8 g_BtlActionNumber;       /* far_syms.txt: lui $at + sb */
extern s32 g_ChrItemRows[];   /* far: holds a pointer read via lui/%lo lw */
extern void MenuUseActionSet(s32 show_place_count);
extern s32 PartyPlaceFind(s32 chr_id);
/* K&R / unprototyped on purpose: MenuUseTargetRun calls this with one
   argument, MenuUsePartyRun with none (that call sets no argument register
   at all — $a0..$a3 are live-out leftovers from its loop). An ANSI
   prototype would force one arity on both and break whichever it isn't. */
extern void BtlActionRowRun();
extern s32 g_PartyIds;     /* base of the u16 id list scanned by MenuUsePartyRun */
extern u8 g_BtlSlotIds[];    /* per-slot id byte array */
extern u8 g_BtlSlotShowPlaceFlags[];    /* per-slot "present" flag array */
extern u8 g_MenuUseTargetId;
extern u8 g_BtlTargetSlot;      /* far scalar (far_syms.txt) -> lui $at + sb */
extern s32 g_MapPartyRecords;     /* base pointer (gp-relative word) */
extern s16 g_BtlSlotShowPlaceXs[];   /* far halfword (lui/%lo sh) */
extern s16 g_BtlSlotShowPlaceYs[];   /* far halfword (lui/%lo sh) */
extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  extern s32 g_ChrRecords;

/* far_syms: g_ChrItemRows (target reaches it %hi/%lo) */
/* far_syms: g_ChrSpellRows (target reaches it %hi/%lo) */
/* far_syms: battle_state_table (target reaches it %hi/%lo) */
extern s32 g_ChrSpellRows[];
extern u8 battle_state_table[];

/* func_80076444 */
void MenuUseRun(s32 user_id, s32 number, s32 target_id)
{
  u8 *p;
  s32 state;
  g_MenuUseUserId = user_id;
  g_MenuUseNumber = number;
  *((s32 *) (&g_MenuUseTargetId)) = target_id;
  if (number < 0xB7)
  {
    p = (u8 *) ((number * 0x12) + g_ChrItemRows[0]);
    state = p[-0xB] & 0xF;
    *((volatile s32 *) (&g_MenuUseEntry)) = state;
  }
  else
  {
    number -= 0xB6;
    p = (u8 *) ((number * 0xC) + g_ChrSpellRows[0]);
    state = p[-0x9] & 0xF;
    *((volatile s32 *) (&g_MenuUseEntry)) = state;
  }
  ((void (*)(void)) ((s32 *) battle_state_table)[*((volatile s32 *) (&g_MenuUseEntry))])();
  if (g_MenuUsedMark)
  {
    number = g_MenuUseNumber;
    if (number < 0xB7)
    {
      ChrItemRemove(*((u16 *) (&g_MenuUseUserId)), *((u8 *) (&g_MenuUseNumber)));
    }
    else
    {
      number -= 0xB6;
      p = (u8 *) ((number * 0xC) + g_ChrSpellRows[0]);
      number = p[-0x8];
      if (((*((u8 *) (((g_MenuUseUserId * 0x40) + g_ChrRecords) + 0x22))) == 0x88) || ((*((u8 *) (((g_MenuUseUserId * 0x40) + g_ChrRecords) + 0x23))) == 0x88))
      {
        number = (number + 1) >> 1;
      }
      *((u16 *) (((g_MenuUseUserId * 0x40) + g_ChrRecords) + 4)) -= number;
    }
  }
}

/* Sets the battle's words for the action of a use from the menu, with
   `show_place_count` show places on: clears the id/x/y tables of the show places, marks
   places [0,show_place_count) present and the rest absent, then sets the acting slot
   g_BtlActingSlot to the user's id and the action's kind and number
   (g_BtlActionKind / g_BtlActionNumber: 4 and the item, or 3 and the spell).
   g_MenuUseNumber is the used number (an item, or a spell + 0xB6) — read as a
   word for the range test and as its low byte for the number. */
/* func_80076590 */
void MenuUseActionSet(s32 show_place_count) {
    s32 i;

    mem_clear(g_BtlSlotIds, 6);
    mem_clear((u8 *)g_BtlSlotShowPlaceXs, 0xC);
    mem_clear((u8 *)g_BtlSlotShowPlaceYs, 0xC);
    i = 0;
    while (i < show_place_count) {
        g_BtlSlotShowPlaceFlags[i] = 1;
        i++;
    }
    while (i < 6) {
        g_BtlSlotShowPlaceFlags[i] = 0;
        i++;
    }
    g_BtlActingSlot = (u8)g_MenuUseUserId;
    if (g_MenuUseNumber < 0xB7) {
        g_BtlActionKind = 4;
        g_BtlActionNumber = (u8)g_MenuUseNumber;
    } else {
        g_BtlActionKind = 3;
        g_BtlActionNumber = (u8)g_MenuUseNumber + 0x4A;
    }
}
/* Debug/assert screen record: a 0x18-byte template is copied out of
   g_MenuUseNumberMessage and six s16 fields are overwritten with a message id and the
   decimal digits of the two values being reported.  The copy is an inline
   `memcpy` — g_MenuUseNumberMessage is a `u8[]`, so GCC only knows byte alignment and
   expands the block move as lwl/lwr + swl/swr pairs (a struct assignment
   through a typed pointer would assume word alignment and emit lw/sw). */
typedef struct DebugCodeBox {
    /* 0x00 */ s16 unk00;
    /* 0x02 */ s16 unk02;
    /* 0x04 */ s16 msg;      /* text id: used number biased by -0xF00 */
    /* 0x06 */ s16 unk06;
    /* 0x08 */ s16 hundreds; /* used number, digit 100s */
    /* 0x0A */ s16 tens;     /*                    10s  */
    /* 0x0C */ s16 ones;     /*                     1s  */
    /* 0x0E */ s16 unk0E;
    /* 0x10 */ s16 stateTens; /* the use's entry g_MenuUseEntry, digit 10s */
    /* 0x12 */ s16 stateOnes; /*                          1s  */
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
} DebugCodeBox;
/* Entry 0 of MenuUseRun's table — the "this should never run" slot: an
   item or spell whose row byte has 0 in its low four bits lands here.  It
   clears the menu's page (MenuPageDisp(0), one frame), renders the used
   number g_MenuUseNumber (3 digits) and the entry g_MenuUseEntry (2 digits) into the
   report box, and then hangs in an infinite loop with no epilogue.  A
   development-time assert screen, not real game flow. */
/* func_80076688 */
void MenuUseNumberHang(void) {
    DebugCodeBox buf;

    memcpy(&buf, g_MenuUseNumberMessage, sizeof(buf));
    MenuPageDisp(0);
    MenuFrameStep();
    buf.msg = (u16) g_MenuUseNumber - 0xF00;
    buf.hundreds = g_MenuUseNumber / 100;
    buf.tens = (g_MenuUseNumber % 100) / 10;
    buf.ones = g_MenuUseNumber % 10;
    buf.stateTens = g_MenuUseEntry / 10;
    buf.stateOnes = g_MenuUseEntry % 10;
    MessageDisp(&buf, 0, g_MenuUseNumber % 10, g_MenuUseEntry % 10);
    for (;;) {
    }
}
/* func_800767D8 */
void MenuUseTargetRun(void) {
    u8 *base;
    s32 a;
    s32 idx;
    MenuUseActionSet(1);
    g_BtlTargetSlot = g_MenuUseTargetId;
    idx = PartyPlaceFind(g_MenuUseTargetId);
    base = (u8 *)(idx * 0x3C + g_MapPartyRecords);
    a = AS(u16, ((MapRecord *)base)->sprite.x);
    g_BtlSlotShowPlaceXs[0] = a;
    g_BtlSlotShowPlaceYs[0] = AS(u16, ((MapRecord *)base)->sprite.y);
    BtlActionRowRun(a);
    g_MenuUsedMark = 1;
}
/* Entry 2 of MenuUseRun's table, the use on the party: walks up to 6 ids at
   *g_PartyIds, stopping at the first 0, skipping id 0xC, and publishes each
   member's id, present flag and x/y (its party record's +6 / +0xA) into the
   four parallel arrays of the show places indexed by `out`, then
   BtlActionRowRun.
   NOTE: the walking pointers and the i*0x3C offset visible in the target asm
   are GCC strength-reduction IVs, not source variables — writing them as
   explicit locals emits their init BEFORE the hoisted loop invariants
   instead of after, and does not match. Keep the plain array indexing. */
/* func_80076844 */
void MenuUsePartyRun(void) {
    s32 i;
    s32 out;
    s32 id;
    u8 *e;

    MenuUseActionSet(0);
    out = 0;
    for (i = 0; i < 6; i++) {
        id = *(u16 *)(g_PartyIds + i * 2);
        if (id == 0) {
            break;
        }
        if (id != 0xC) {
            g_BtlSlotIds[out] = id;
            g_BtlSlotShowPlaceFlags[out] = 1;
            e = (u8 *)(i * 0x3C + g_MapPartyRecords);
            g_BtlSlotShowPlaceXs[out] = AS(u16, ((MapRecord *)e)->sprite.x);
            g_BtlSlotShowPlaceYs[out] = AS(u16, ((MapRecord *)e)->sprite.y);
            out++;
        }
    }
    BtlActionRowRun();
    g_MenuUsedMark = 1;
}
/* func_8007690C */
void MenuUsePictureRun(void) {
    u8 *p;
    u32 hi;
    g_MovieSkipAllowed = 0x8D0;
    p = (u8 *)(g_MenuUseNumber * 0x12 + g_ChrItemRows[0]);
    hi = p[-0xB];
    MenuUseMoviePlay((hi >> 4) + 0x37);
    MenuUseDrawAreaOpen();
    g_MovieSkipAllowed = 0;
    g_MenuUsedMark = 0;
}
