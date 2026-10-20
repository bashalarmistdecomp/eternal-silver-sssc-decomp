/* Lunar SSSC — chr_levelup: a character record's level-up: levels added,
   the gain table and the gains. The first split had it in rendering2.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "tile.h"
#include "pad.h"
#include "palette.h"
#include "chr.h"
#include "btl.h"
#include "sprite.h"
extern u8 g_ChrLevelGainTable[];
u8 ChrLevelGainGet(u8 *hdr);
extern u8 g_BtlTargetSlot;
extern u8 g_ChrLevelUpGains[];
extern s32 g_ChrRecords;
extern u8 g_BtlActingSlot;
extern long HWD0;
extern long VWD0;
void ChrLevelGainAdd(u16 *stat, s32 gain);
extern long HWD0, VWD0;
/* far_syms: g_BtlSpellRows (target reaches it %hi/%lo) */
extern u8 g_BtlSpellRows[];
s32 rng_7f_draw(void);
void ChrLevelUp(u8, u8 *);
void mem_clear(void *destination, u32 byte_count);  /* extern */

void ChrLevelUp(u8, u8 *);                       /* extern */

/* func_8002844C */
void ChrLevelAdd(s32 chr_id, s32 level_count)
{
  s32 levels_left;
  u32 total;
  void *chr_record;
  chr_record = g_ChrRecords;
  chr_record += (chr_id & 0xFFFF) << 6;
  levels_left = level_count;
  if ((((u8) (AS(s32, ((ChrRecord *)chr_record)->level))) < 0x63U) && ((mem_clear(g_ChrLevelUpGains, 0xC), g_BtlActingSlot = 0, (levels_left & 0xFF) != 0)))
  {
    {
      do
      {
        total = AS(s32, ((ChrRecord *)chr_record)->experience) + AS(s32, ((ChrRecord *)chr_record)->experience_next);
        ((ChrRecord *)chr_record)->experience = total;
        if (total > 0x98967FU)
        {
          ((ChrRecord *)chr_record)->experience = 0x98967FU;
        }
        ChrLevelUp(chr_id & 0xFF, chr_record);
        levels_left -= 1;
        if (((u8) (AS(s32, ((ChrRecord *)chr_record)->level))) >= 0x63U)
        {
          ((ChrRecord *)chr_record)->experience_next = 0;
          return;
        }
      }
      while (!(!(levels_left & 0xFF)));
    }
  }
}

/* One level-up gain, for ChrLevelUp (its seven calls: hdr + 0, 2, 4 .. 0xC
   = max HP, max MP and the five stats): hdr points at one big-endian u16 of a
   member's header in the gain table g_ChrLevelGainTable, that word is the offset from
   the table's base to the gain's byte row, and the row is indexed by the OLD
   level, which ChrLevelUp parked in g_BtlTargetSlot before its first call —
   returns g_ChrLevelGainTable[be16(*hdr) + g_BtlTargetSlot]. Reads only: no store, no
   callee, no RNG draw, no frame wait. Header words 7 and 8 are read by the
   same expression written out in ChrLevelUp.
   Shape: the table's base in a pointer local and the swapped offset added
   to it BEFORE the level, as ChrLevelUp's own two copies have it (tbl2 +
   swap, then += g_BtlTargetSlot): the same local indexed by (swap + level) is
   10/13 (the level is added to the offset first); that order with the
   symbol in the expression and a u16 cast on the shifted half is 11/13.
   MARKED SPELLING: the pointer local `tbl` — with g_ChrLevelGainTable written in the
   expression the base is formed after the load and in $v1, not in $a1:
   9/13 (as a pointer sum) or 4/13 (in one expression). */
/* func_80028520 */
u8 ChrLevelGainGet(u8 *hdr)
{
    u8 *tbl;
    u16 h;

    tbl = g_ChrLevelGainTable;
    h = *(u16 *)hdr;
    return *(tbl + (((h << 8) & 0xFF00) | (h >> 8)) + g_BtlTargetSlot);
}

/* One level-up of party member `id` (persistent record `rec`, 0x40 bytes):
   the gains come from g_ChrLevelGainTable — per-id headers of ten big-endian u16
   offsets (hdr = g_ChrLevelGainTable + (id - 1) * 0x14), each indexing a byte table
   by the OLD level, which is parked in g_BtlTargetSlot for ChrLevelGainGet.
   Max HP/MP via ChrLevelGainAdd (MP not for ids 7 and 9); the five stats
   +0x08..+0x10 capped at 999 and their u8 copies +0x14..+0x18 saturating
   at 0xFF; words 7/8 add to +0x12/+0x19 and +0x13/+0x1A; the level byte
   +0x1C goes up; +0x3C gets the big-endian EXP-to-next from word 9. The
   gains are left in g_ChrLevelUpGains[1..9] for the win screen. Finally the
   first spell row (g_BtlSpellRows, 50 rows of 0xC) for this id (0xB uses 1's)
   at the NEW level is learned into the slot jtbl_800118DC picks, and its
   row number goes to g_ChrLevelUpGains[0xA] (battle.md "Level-ups"). */
/* load_hazard_nop: ChrLevelUp */
/* func_80028554 */
void ChrLevelUp(u8 id, u8 *rec)
{
    u8 *hdr;
    u8 *gains;
    u8 *out;
    u8 *tbl;
    u8 *tbl2;
    u8 *p;
    u8 *q;
    u16 t;
    u8 g;
    u8 who;
    u8 slot;
    u8 *row;
    u32 v;
    u16 h;

    tbl = g_ChrLevelGainTable;
    hdr = tbl + (id - 1) * 0x14;
    g_BtlTargetSlot = ((ChrRecord *)rec)->level;
    g = ChrLevelGainGet(hdr);
    ChrLevelGainAdd((u16 *)(rec + 2), g);
    gains = g_ChrLevelUpGains;
    gains[1] = g;
    if (id != 7 && id != 9) {
        g = ChrLevelGainGet(hdr + 2);
        ChrLevelGainAdd((u16 *)(rec + 6), g);
        gains[2] = g;
    }

    g = ChrLevelGainGet(hdr + 4);
    t = ((ChrRecord *)rec)->stat_08 + g;
    ((ChrRecord *)rec)->stat_08 = t;
    if (t >= 1000) ((ChrRecord *)rec)->stat_08 = 999;
    if (((ChrRecord *)rec)->stat_08_bare + g < 0x100) ((ChrRecord *)rec)->stat_08_bare += g; else ((ChrRecord *)rec)->stat_08_bare = 0xFF;
    g_ChrLevelUpGains[3] = g;

    g = ChrLevelGainGet(hdr + 6);
    t = ((ChrRecord *)rec)->stat_0a + g;
    ((ChrRecord *)rec)->stat_0a = t;
    if (t >= 1000) ((ChrRecord *)rec)->stat_0a = 999;
    if (((ChrRecord *)rec)->stat_0a_bare + g < 0x100) ((ChrRecord *)rec)->stat_0a_bare += g; else ((ChrRecord *)rec)->stat_0a_bare = 0xFF;
    g_ChrLevelUpGains[4] = g;

    g = ChrLevelGainGet(hdr + 8);
    t = ((ChrRecord *)rec)->stat_0c + g;
    ((ChrRecord *)rec)->stat_0c = t;
    if (t >= 1000) ((ChrRecord *)rec)->stat_0c = 999;
    if (((ChrRecord *)rec)->stat_0c_bare + g < 0x100) ((ChrRecord *)rec)->stat_0c_bare += g; else ((ChrRecord *)rec)->stat_0c_bare = 0xFF;
    g_ChrLevelUpGains[5] = g;

    g = ChrLevelGainGet(hdr + 0xA);
    t = ((ChrRecord *)rec)->stat_0e + g;
    ((ChrRecord *)rec)->stat_0e = t;
    if (t >= 1000) ((ChrRecord *)rec)->stat_0e = 999;
    if (((ChrRecord *)rec)->stat_0e_bare + g < 0x100) ((ChrRecord *)rec)->stat_0e_bare += g; else ((ChrRecord *)rec)->stat_0e_bare = 0xFF;
    g_ChrLevelUpGains[6] = g;

    g = ChrLevelGainGet(hdr + 0xC);
    t = ((ChrRecord *)rec)->stat_10 + g;
    ((ChrRecord *)rec)->stat_10 = t;
    if (t >= 1000) ((ChrRecord *)rec)->stat_10 = 999;
    if (((ChrRecord *)rec)->stat_10_bare + g < 0x100) ((ChrRecord *)rec)->stat_10_bare += g; else ((ChrRecord *)rec)->stat_10_bare = 0xFF;
    out = g_ChrLevelUpGains;
    tbl2 = g_ChrLevelGainTable;
    out[7] = g;

    h = ((ChrLevelGainHead *)hdr)->attack_count_row;
    p = tbl2 + (((h << 8) & 0xFF00) | (h >> 8));
    p += g_BtlTargetSlot;
    ((ChrRecord *)rec)->attack_count += *p;
    ((ChrRecord *)rec)->attack_count_bare += *p;
    out[8] = *p;

    h = ((ChrLevelGainHead *)hdr)->walk_reach_row;
    p = tbl2 + (((h << 8) & 0xFF00) | (h >> 8));
    p += g_BtlTargetSlot;
    ((ChrRecord *)rec)->walk_reach += *p;
    ((ChrRecord *)rec)->walk_reach_bare += *p;
    out[9] = *p;

    ((ChrRecord *)rec)->level++;

    h = ((ChrLevelGainHead *)hdr)->experience_next_row;
    h = ((h << 8) & 0xFF00) | (h >> 8);
    q = tbl2 + h;
    q += g_BtlTargetSlot * 4;
    v = *(u32 *)q;
    ((ChrRecord *)rec)->experience_next = (v << 24) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | (v >> 24);

    g = 0;
    who = id;
    if (id == 0xB) who = 1;
    row = g_BtlSpellRows;
    for (; g < 0x32; g++, row += 0xC) {
        if (row[7] == who && ((ChrRecord *)rec)->level == row[0]) {
            g++;
            g_ChrLevelUpGains[0xA] = g;
            switch (id) {
            case 1:
            case 11:
                slot = g - 1;
                break;
            case 2:
                if (g < 0xD)
                    slot = g - 9;
                else
                    slot = g - 0x27;
                break;
            case 3:
                slot = g - 0xD;
                break;
            case 4:
                slot = g - 0x15;
                break;
            case 5:
                slot = g - 0x1D;
                break;
            case 6:
                slot = g - 0x21;
                break;
            }
            (rec + slot)[0x2A] = g;
            return;
        }
    }
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_800118DC_pad);

/* func_800289E0 */
void ChrLevelGainAdd(u16 *stat, s32 gain) {
    u16 new_value;

    new_value = *stat + (gain & 0xFF);
    *stat = new_value;
    if ((u32) (new_value & 0xFFFF) >= 0x3E8U) {
        *stat = 0x3E7;
    }
}
/* func_80028A0C */
s32 BtlWinItemRateCheck(s32 item, s32 rate)
{
  if (item & 0xFF)
  {
    if ((u32)(rng_7f_draw() & 0xFF) <= (u32)(rate & 0xFF))
      return 1;
  }
  return 0;
}
