/* Lunar SSSC — btl_init: the battle's set-up: BtlInit, the party's and the
   enemies' stats, the special battles, the enemies' level, the pictures and
   animation data loaded, the records and the state. The first split had it
   in rendering2_c.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: CharStats */
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: g_BtlEnemySet (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "chr.h"
#include "btl.h"
extern s32 g_BtlRecords;
extern s32 CharStats;
extern u8 g_BtlSlotIds[];
/* far_syms: CharStats g_BtlNumber g_BtlSlotIds g_BtlSlotActiveFlags */
extern u8 g_BtlNumber;
extern u8 g_BtlPadRecord[];
extern u8 g_BtlSlotActiveFlags[];
void func_8002A130();  /* TU defines this below */
void BtlListClear(void);  /* TU defines this below */
void BtlPadClear(void);  /* TU defines this below */
s32 pad_record_init(u8 *, s32, s32, s32);
void BtlStateInit();
int BtlEnemyLoad();
void BtlEnemyStatsSet(void);  /* TU defines this below */
void BtlPartyStatsSet(void);  /* TU defines this below */
void BtlPartyLoad();
s32 BtlSpecialInit(void);  /* TU defines this below */
s32 StoryBitCheck(u32);
s32 BtlSlotStatusAnimationSet(s32);
s32 StoryBitClear(s32);
void BtlPartyPairStatusSet();
void BtlMenuInit();
void tile_mix_fade_stop();
void BtlBgGreySet(s32);
void sprite_col_set();
void BtlBlackLinesRun();
void BtlPartyChoiceClear();

/* func_80029E68 */
void BtlInit(void)
{
    u8 i;

    func_8002A130();
    BtlListClear();
    BtlPadClear();
    pad_record_init(g_BtlPadRecord, 2, 0, 0);
    BtlStateInit();
    BtlEnemyLoad();
    BtlEnemyStatsSet();
    BtlPartyStatsSet();
    if (g_BtlNumber == 0xE4)
    {
        g_BtlSlotIds[5] = 0;
    }
    BtlPartyLoad();
    if (g_BtlNumber >= 0xEB)
    {
        BtlSpecialInit();
    }
    else if (StoryBitCheck(0x2D5) != 0)
    {
        for (i = 0; i < 5; i++)
        {
            *(u8 *)(i * 0x3C + CharStats + 0x15) = 0x40;
            BtlSlotStatusAnimationSet(i);
        }
        StoryBitClear(0x2D5);
    }
    if (StoryBitCheck(0x2D0) != 0)
    {
        BtlPartyPairStatusSet();
    }
    BtlMenuInit();
    tile_mix_fade_stop();
    BtlBgGreySet(0x80);
    sprite_col_set(0x808080);
    BtlBlackLinesRun();
    for (i = 0; i < 14; i++)
    {
        if (g_BtlSlotIds[i] != 0)
        {
            g_BtlSlotActiveFlags[i] = 1;
        }
        else
        {
            g_BtlSlotActiveFlags[i] = 0;
        }
    }
    g_BtlSlotActiveFlags[5] = 0;
    BtlPartyChoiceClear();
}
typedef struct { u8 b[12]; } Pairs12;
extern u8 g_BtlEnemySet;
extern u32 g_BtlEnemySpriteDataBlocks[];
void BtlRecordInit(s32 slot);  /* TU defines this below */
extern Pairs12 g_BtlPartyPairs;
s32 rng_mask_below(s32, s32);

/* func_8002A010 */
void BtlPartyPairStatusSet(void)
{
    Pairs12 tbl;
    u8 k;
    u8 i;
    u8 id;

    tbl = g_BtlPartyPairs;
    k = rng_mask_below(7, 6);
    k *= 2;
    for (i = 0; i < 2; i++)
    {
        id = tbl.b[k];
        k++;
        *(u8 *)(id * 0x3C + CharStats + 0x15) = 8;
        *(u8 *)(id * 0x3C + CharStats + 0x1E) = 4;
        BtlSlotStatusAnimationSet(id);
    }
}

/* far_syms: g_PadHeldButtons (target reaches it %hi/%lo) */
/* far_syms: g_PadSecondHeldButtons (target reaches it %hi/%lo) */
/* far_syms: g_PadHeldButtons (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PadSecondHeldButtons (auto: target reaches it %hi/%lo — addressing row 3) */
extern s16 g_PadHeldButtons;
extern u16 g_PadSecondHeldButtons;

/* func_8002A0F4 */
void BtlPadClear(void)
{
    g_PadHeldButtons = 0;
    g_PadSecondHeldButtons = 0;
}

s32 sprite_run_off(s32, s32);                              /* extern */

/* func_8002A10C */
void BtlListClear(void) {
    sprite_run_off(0, 0x96);
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its one call passes nothing, and nothing read says what it was for. */
void func_8002A130() {

}
/* far_syms: g_ChrRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlItemRows (target reaches it %hi/%lo) */
extern ChrRecord *g_ChrRecords;
extern BtlItemRow *g_BtlItemRows;
extern u8 g_BtlPartyNotHeldFlags[];
extern u8 g_BtlPartyAttackAnimations[];
extern u8 g_BtlPartyStandAnimations[];
extern u8 g_BtlPartyMoveAnimations[];
extern u8 g_BtlPartyHitAnimations[];

/* func_8002A138 */
void BtlPartyStatsSet(void)
{
    BtlStats *c;
    ChrRecord *p;
    BtlItemRow *item;
    u16 i;
    u16 j;

    c = (BtlStats *)CharStats;
    for (i = 0; i < 5; c++, i++) {
        if (g_BtlSlotIds[i] == 0) {
            break;
        }
        g_BtlPartyNotHeldFlags[i] = 1;
        p = g_ChrRecords;
        p += g_BtlSlotIds[i];
        c->hp = p->hp;
        c->hp_max = p->hp_max;
        c->mp = p->mp;
        c->mp_max = p->mp_max;
        c->side.party.stat_08_start = c->stat_08 = p->stat_08;
        c->side.party.stat_0a_start = c->stat_0a = p->stat_0a;
        c->stat_0c = p->stat_0c;
        c->stat_0e = p->stat_0e;
        c->stat_10 = p->stat_10;
        c->attack_count = p->attack_count;
        c->walk_reach = p->walk_reach;
        c->level = p->level;
        c->walk_speed = 3;
        c->side.party.stat_1f = p->stat_1d;
        for (j = 0; j < 6; j++) {
            c->items[j] = p->items[j];
        }
        for (j = 0; j < 8; j++) {
            c->spells[j] = p->spells[j];
        }
        if (p->worn_items[0] != 0) {
            c->side.party.worn_items[0] = p->worn_items[0];
            item = &g_BtlItemRows[p->worn_items[0] - 1];
            c->least_amount = item->bonus_2;
            c->attack_show = item->attack_show;
            c->side.party.traits[0] = item->trait;
            c->side.party.worn_item_kind = item->worn_item_kind;
            switch (item->worn_item_kind) {
            case 0:
                g_BtlPartyAttackAnimations[i] = 0xB;
                g_BtlPartyStandAnimations[i] = 6;
                g_BtlPartyMoveAnimations[i] = 1;
                c->attack_sound = 0x3B;
                break;
            case 1:
                g_BtlPartyAttackAnimations[i] = 0xD;
                g_BtlPartyStandAnimations[i] = 8;
                g_BtlPartyMoveAnimations[i] = 3;
                c->attack_sound = 0x39;
                break;
            case 2:
                g_BtlPartyAttackAnimations[i] = 0xD;
                g_BtlPartyStandAnimations[i] = 8;
                g_BtlPartyMoveAnimations[i] = 3;
                c->attack_sound = 0x43;
                break;
            case 3:
                if (g_BtlSlotIds[i] == 0xA) {
                    g_BtlPartyAttackAnimations[i] = 0xD;
                    g_BtlPartyStandAnimations[i] = 8;
                    g_BtlPartyMoveAnimations[i] = 3;
                } else {
                    g_BtlPartyAttackAnimations[i] = 0xE;
                    g_BtlPartyStandAnimations[i] = 9;
                    g_BtlPartyMoveAnimations[i] = 4;
                }
                c->attack_sound = 0x3C;
                break;
            case 4:
                g_BtlPartyAttackAnimations[i] = 0xC;
                g_BtlPartyStandAnimations[i] = 7;
                g_BtlPartyMoveAnimations[i] = 2;
                c->attack_sound = 0x4B;
                break;
            case 5:
                g_BtlPartyAttackAnimations[i] = 0xE;
                g_BtlPartyStandAnimations[i] = 9;
                g_BtlPartyMoveAnimations[i] = 4;
                switch (g_BtlSlotIds[i]) {
                case 1:
                case 5:
                case 11:
                    c->attack_sound = 0x39;
                    break;
                case 2:
                    c->attack_sound = 0x3C;
                    break;
                case 3:
                case 4:
                    c->attack_sound = 0x43;
                    break;
                }
                break;
            }
        } else {
            g_BtlPartyAttackAnimations[i] = 0xA;
            g_BtlPartyStandAnimations[i] = 5;
            g_BtlPartyMoveAnimations[i] = 0;
            c->attack_sound = 0x44;
        }
        c->side.party.worn_items[1] = p->worn_items[1];
        item = &g_BtlItemRows[p->worn_items[1] - 1];
        c->side.party.traits[1] = item->trait;
        c->side.party.worn_items[2] = p->worn_items[2];
        item = &g_BtlItemRows[p->worn_items[2] - 1];
        c->side.party.traits[2] = item->trait;
        c->side.party.worn_items[3] = p->worn_items[3];
        item = &g_BtlItemRows[p->worn_items[3] - 1];
        c->side.party.traits[3] = item->trait;
        c->side.party.worn_items[4] = p->worn_items[4];
        item = &g_BtlItemRows[p->worn_items[4] - 1];
        c->side.party.traits[4] = item->trait;
        c->side.party.worn_items[5] = p->worn_items[5];
        item = &g_BtlItemRows[p->worn_items[5] - 1];
        c->side.party.traits[5] = item->trait;
        g_BtlPartyHitAnimations[i] = 0x12;
    }
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_800119F0_pad);
/* far_syms: g_BtlEnemyRows (target reaches it %hi/%lo) */
extern u8 g_BtlEnemyRows[];

/* func_8002A638 */
void BtlEnemyStatsSet(void)
{
  u16 i;
  u8 **p_home;
  u16 idx;
  u8 *src;
  u8 *p;
  u8 *q;
  u16 v;
  q = ((u8 *) (&CharStats)[0]) + 0x168;
  p = ((u8 *) (&CharStats)[0]) + 0x192;
  for (i = 6; i < 0xE; i++)
  {
    if (!g_BtlSlotIds[i])
    {
      break;
    }
    idx = (u16) (g_BtlSlotIds[i] - 1);
    src = ((u8 *) g_BtlEnemyRows) + (idx * 38);
    *((u8 *) (p - 0x10)) = ((BtlEnemyRow *)src)->action_pick;
    *((u8 *) (p - 0x16)) = ((BtlEnemyRow *)src)->level;
    v = ((BtlEnemyRow *)src)->hp;
    *((u16 *) (p - 0x28)) = v;
    *((u16 *) q) = v;
    *((u16 *) (p - 0x22)) = ((BtlEnemyRow *)src)->stat_08;
    *((u16 *) ((*(p_home = &p)) - 0x20)) = ((BtlEnemyRow *)src)->stat_0a;
    *((u16 *) (p - 0x1E)) = ((BtlEnemyRow *)src)->stat_0c;
    *((u16 *) (p - 0x1C)) = ((BtlEnemyRow *)src)->stat_0e;
    *((u16 *) (p - 0x1A)) = ((BtlEnemyRow *)src)->stat_10;
    *((u8 *) (p - 0x17)) = ((BtlEnemyRow *)src)->walk_reach;
    *((u8 *) (p - 0x14)) = ((BtlEnemyRow *)src)->least_amount;
    *((u8 *) (p - 0x18)) = ((BtlEnemyRow *)src)->attack_count;
    *((u8 *) (p - 0xF)) = ((BtlEnemyRow *)src)->trait;
    *((u8 *) (p - 0xE)) = ((BtlEnemyRow *)src)->attack_number;
    *((u8 *) (p - 0xD)) = ((BtlEnemyRow *)src)->kind7_number;
    *((u8 *) (p - 0xC)) = ((BtlEnemyRow *)src)->walk_sound;
    *((u8 *) (p - 0x11)) = ((BtlEnemyRow *)src)->attack_sound;
    *((u8 *) (p - 0xB)) = ((BtlEnemyRow *)src)->action_sound;
    *((u8 *) (p - 0x12)) = ((BtlEnemyRow *)src)->walk_speed;
    *((u8 *) (p - 0x13)) = ((BtlEnemyRow *)src)->attack_show;
    *((u16 *) (p - 0x6)) = ((BtlEnemyRow *)src)->experience;
    *((u16 *) (p - 0x4)) = ((BtlEnemyRow *)src)->money;
    *((u8 *) (p - 0xA)) = ((BtlEnemyRow *)src)->win_item_1;
    *((u8 *) (p - 0x9)) = ((BtlEnemyRow *)src)->win_rate_1;
    *((u8 *) (p - 0x8)) = ((BtlEnemyRow *)src)->win_item_2;
    *((u8 *) (p - 0x7)) = ((BtlEnemyRow *)src)->win_rate_2;
    *((u16 *) (p - 0x2)) = ((BtlEnemyRow *)src)->weak_mask;
    *((u16 *) p) = ((BtlEnemyRow *)src)->resist_mask;
    p += 0x3C;
    q += 0x3C;
  }

}
/* far_syms: g_BtlPartyKeptRecords g_BtlRecords g_BtlNumber g_BtlRoundsLeft CharStats */
extern s32 g_BtlPartyKeptRecords;
extern u8 g_BtlNumber;
extern u8 g_BtlRoundsLeft;
extern u8 g_BtlSpecialBytes[];
s32 xmalloc_heap_alloc(s32);
s32 BtlAnimationSet(void *, s32);
void BtlEnemyLevelSet(u16, u16, u16, u16, u16, u16, u8);
s32 BtlSlotStatusAnimationSet(s32);

/* func_8002A7FC */
s32 BtlSpecialInit(void)
{
    u8 i;

    g_BtlPartyKeptRecords = xmalloc_heap_alloc(0x140);
    for (i = 0; i < 5; i++) {
        *(BtlRecord *)(i * 0x40 + g_BtlPartyKeptRecords) = *(BtlRecord *)(i * 0x40 + g_BtlRecords);
    }
    switch (g_BtlNumber) {
    case 0xEB:
        BtlEnemyLevelSet(0x50, 2, 1, 1, 1, 1, 0xB);
        for (i = 0; i < 4; i++) {
            g_BtlSpecialBytes[i + 5] = g_BtlPartyStandAnimations[i];
        }
        break;
    case 0xEC:
        BtlEnemyLevelSet(0x46, 2, 1, 1, 4, 1, 0xD);
        ((BtlRecord *)g_BtlRecords)[6].width = 0x70;
        break;
    case 0xED:
        BtlEnemyLevelSet(0x5A, 1, 1, 1, 4, 2, 0xF);
        break;
    case 0xEE:
        BtlEnemyLevelSet(0x82, 2, 2, 1, 1, 2, 0x11);
        break;
    case 0xEF:
        BtlEnemyLevelSet(0x78, 2, 2, 1, 3, 2, 0x14);
        break;
    case 0xF0:
        BtlEnemyLevelSet(0x55, 3, 1, 1, 2, 1, 0x1A);
        break;
    case 0xF1:
        BtlEnemyLevelSet(0xFA, 2, 2, 1, 3, 1, 0x20);
        ((BtlRecord *)g_BtlRecords)[6].width = 0x50;
        break;
    case 0xF2:
        BtlEnemyLevelSet(0xCD, 4, 2, 1, 3, 2, 0x24);
        break;
    case 0xF3:
        BtlEnemyLevelSet(0xA0, 1, 2, 1, 3, 2, 0x23);
        break;
    case 0xF4:
        BtlEnemyLevelSet(0xB4, 4, 3, 1, 3, 2, 0x25);
        break;
    case 0xF5:
        BtlEnemyLevelSet(0xB9, 4, 3, 1, 4, 2, 0x28);
        for (i = 0; i < 5; i++) {
            g_BtlSpecialBytes[i + 5] = g_BtlPartyStandAnimations[i];
        }
        ((BtlRecord *)g_BtlRecords)[6].width -= 0x14;
        break;
    case 0xF6:
        BtlAnimationSet((void *)(g_BtlRecords + 0x180), 0x11);
        BtlEnemyLevelSet(0xD2, 4, 3, 1, 4, 2, 0x2B);
        ((BtlRecord *)g_BtlRecords)[6].width -= 0x30;
        break;
    case 0xF7:
        BtlEnemyLevelSet(0x104, 5, 3, 1, 3, 2, 0x2C);
        ((BtlRecord *)g_BtlRecords)[6].width -= 0x10;
        *(BtlRecord *)(g_BtlRecords + 0x1C0) = *(BtlRecord *)(g_BtlRecords + 0x180);
        ((BtlRecord *)g_BtlRecords)[7].on = 0;
        break;
    case 0xF8:
        BtlEnemyLevelSet(0x32, 3, 1, 1, 0, 0, 0x11);
        break;
    case 0xF9:
        BtlEnemyLevelSet(0xF0, 2, 2, 1, 3, 1, 0x20);
        ((BtlRecord *)g_BtlRecords)[6].width = 0x50;
        for (i = 0; i < 4; i++) {
            *(u8 *)(i * 0x3C + CharStats + 0x15) = 4;
            BtlSlotStatusAnimationSet(i);
        }
        g_BtlRoundsLeft = 3;
        break;
    case 0xFA:
        BtlEnemyLevelSet(0, 0, 0, 0, 0, 0, 1);
        g_BtlRoundsLeft = 3;
        break;
    case 0xFC:
        BtlEnemyLevelSet(0x1E, 3, 2, 1, 1, 1, 0xB);
        break;
    case 0xFD:
        BtlEnemyLevelSet(0xA, 2, 2, 1, 2, 1, 0x1B);
        break;
    case 0xFE:
        BtlEnemyLevelSet(0x1C2, 5, 2, 3, 5, 3, 0x2E);
        *(BtlRecord *)(g_BtlRecords + 0x1C0) = *(BtlRecord *)(g_BtlRecords + 0x180);
        ((BtlRecord *)g_BtlRecords)[7].on = 0;
        ((BtlRecord *)g_BtlRecords)[6].width -= 0x10;
        break;
    }
}
/* Encounter level scaling for the enemy slots (called by BtlSpecialInit once
   per encounter id with that formation's per-level increments). The level is
   max(lvl, the lead party member's level CharStats[0] +0x14), capped at 50.
   For each enemy slot 6..13 in order — stopping at the first empty one
   (g_BtlSlotIds == 0) — the 0x3C stat record gets: +0x14 = level; HP (+0) and
   max HP (+2) += level * hp, both pinned at 0xFFFF when max HP would pass it;
   +0x08 / +0x0A / +0x0C / +0x0E / +0x10 += level * the matching increment,
   each pinned at 0xFFFF. */
/* func_8002ADF0 */
void BtlEnemyLevelSet(u16 hp, u16 at, u16 df, u16 sp, u16 ag, u16 wi, u8 lvl)
{
    u8 i;
    u16 dhp;
    u16 dat;
    u16 ddf;
    u16 dsp;
    u16 dag;
    u16 dwi;

    if (lvl < ((BtlStats *)CharStats)->level)
        lvl = ((BtlStats *)CharStats)->level;
    if (lvl >= 0x33)
        lvl = 0x32;
    dhp = lvl * hp;
    dat = lvl * at;
    ddf = lvl * df;
    dsp = lvl * sp;
    dag = lvl * ag;
    dwi = lvl * wi;
    for (i = 6; i < 0xE; i++) {
        if (g_BtlSlotIds[i] == 0)
            return;
        *(u8 *)(i * 0x3C + CharStats + 0x14) = lvl;
        if (*(u16 *)(i * 0x3C + CharStats + 2) + dhp > 0xFFFF)
            *(u16 *)(i * 0x3C + CharStats) = *(u16 *)(i * 0x3C + CharStats + 2) = 0xFFFF;
        else
            *(u16 *)(i * 0x3C + CharStats + 2) = *(u16 *)(i * 0x3C + CharStats) =
                *(u16 *)(i * 0x3C + CharStats) + dhp;
        if (*(u16 *)(i * 0x3C + CharStats + 0x8) + dat > 0xFFFF)
            *(u16 *)(i * 0x3C + CharStats + 0x8) = 0xFFFF;
        else
            *(u16 *)(i * 0x3C + CharStats + 0x8) = *(u16 *)(i * 0x3C + CharStats + 0x8) + dat;
        if (*(u16 *)(i * 0x3C + CharStats + 0xA) + ddf > 0xFFFF)
            *(u16 *)(i * 0x3C + CharStats + 0xA) = 0xFFFF;
        else
            *(u16 *)(i * 0x3C + CharStats + 0xA) = *(u16 *)(i * 0x3C + CharStats + 0xA) + ddf;
        if (*(u16 *)(i * 0x3C + CharStats + 0xC) + dsp > 0xFFFF)
            *(u16 *)(i * 0x3C + CharStats + 0xC) = 0xFFFF;
        else
            *(u16 *)(i * 0x3C + CharStats + 0xC) = *(u16 *)(i * 0x3C + CharStats + 0xC) + dsp;
        if (*(u16 *)(i * 0x3C + CharStats + 0xE) + dag > 0xFFFF)
            *(u16 *)(i * 0x3C + CharStats + 0xE) = 0xFFFF;
        else
            *(u16 *)(i * 0x3C + CharStats + 0xE) = *(u16 *)(i * 0x3C + CharStats + 0xE) + dag;
        if (*(u16 *)(i * 0x3C + CharStats + 0x10) + dwi > 0xFFFF)
            *(u16 *)(i * 0x3C + CharStats + 0x10) = 0xFFFF;
        else
            *(u16 *)(i * 0x3C + CharStats + 0x10) = *(u16 *)(i * 0x3C + CharStats + 0x10) + dwi;
    }
}

/* far_syms: g_BtlSlotIds */
extern s32 g_ResourceReadBuffer;
s32 BtlPartyImageHeightGet(s32);
s32 BtlPage4RoomGet(void);

/* Pick a party slot: over slots 0..5 present in g_BtlSlotIds (the kind
   byte), the one whose BtlPartyImageHeightGet value for its 8-byte entry in the
   g_ResourceReadBuffer block (kind - 1, +2) is largest without exceeding
   BtlPage4RoomGet(); -1 when none qualifies. */
/* func_8002B064 */
s32 BtlPartyTallestFind(void)
{
  s32 best;
  s32 who;
  s32 i;
  s32 v;
  best = 0;
  who = -1;
  for (i = 0; i < 6; i++)
  {
    if (g_BtlSlotIds[i] != 0)
    {
      v = BtlPartyImageHeightGet(g_ResourceReadBuffer + (g_BtlSlotIds[i] - 1) * 8 + 2);
      if ((BtlPage4RoomGet() >= v) && (best < v))
      {
        best = v;
        who = i;
      }
    }
  }
  return who;
}
extern u32 g_BtlPartySpriteDataBlocks[];  /* TU declares this below */
extern u8 g_BtlPartyPlaceXCells[];
extern u8 g_BtlPartyPlaceYCells[];
extern u8 g_BtlSlot5Roaming;
extern u8 g_BtlSlot5RoamAnimationState;
extern u8 g_BtlSlot5RoamXState;
extern u8 g_BtlSlot5RoamYState;
extern u16 g_BtlSlot5RoamAnimationWait;
extern u16 g_BtlSlot5RoamXCount;
extern u16 g_BtlSlot5RoamYCount;
s32 BtlPartyImageAdd(s32, s32);
void BtlPartyImagePage4Add(s32 entry_address, s32 sprite_data_address);
void BtlPartyDataLoad(void);
s32 inflate_buffer(s32, s32);

/* func_8002B114 */
void BtlPartyLoad(void)
{
    u16 i;
    u16 leader;
    s32 base;
    u8 *e;
    s32 src;
    u8 *buf;
    u8 *rec;
    s32 off;
    u8 *hx;
    u8 *e2;
    BtlRecord *q;

    BtlPartyDataLoad();
    i = 0;
    leader = BtlPartyTallestFind();
    do {
        if (g_BtlSlotIds[i] != 0) {
            base = g_ResourceReadBuffer;
            /* e is 2 bytes before the kind's row: the rows begin at the
               resource's byte 2, so e + 2 is its BtlPartyDataRow. */
            e = (u8 *)(base + (g_BtlSlotIds[i] - 1) * 8);
            e2 = e + 2;
            src = base + ((BtlPartyDataRow *)(e + 2))->data_place * 4;
            buf = (u8 *)xmalloc_heap_alloc(((BtlPartyDataRow *)(e + 2))->data_length);
            inflate_buffer(src, (s32)buf);
            g_BtlPartySpriteDataBlocks[i] = (s32)buf;
            rec = (u8 *)((i << 6) + g_BtlRecords);
            ((BtlRecord *)rec)->sprite.texel_records = (s32)(buf + ((SpriteDataHead *)buf)->texel_records * 4);
            ((BtlRecord *)rec)->sprite.pose_table = (s32)(buf + ((SpriteDataHead *)buf)->pose_table * 4);
            ((BtlRecord *)rec)->sprite.piece_table = (s32)(buf + ((SpriteDataHead *)buf)->piece_table * 4);
            ((BtlRecord *)rec)->animation_table = (s32)(buf + ((SpriteDataHead *)buf)->animation_table * 4);
            ((BtlRecord *)rec)->step_list = (s32)(buf + ((SpriteDataHead *)buf)->step_list * 4);
            if (i != leader) {
                BtlPartyImageAdd((s32)e2, (s32)buf);
            } else {
                BtlPartyImagePage4Add((s32)e2, (s32)buf);
            }
            BtlRecordInit((u8)i);
            off = i << 6;
            rec = (u8 *)(off + g_BtlRecords);
            ((BtlRecord *)rec)->sprite.attr = 0x2020;
            ((BtlRecord *)rec)->sprite.flags = 0x2000;
            if (i != 5) {
                hx = &g_BtlPartyPlaceXCells[i];
                ((BtlRecord *)rec)->sprite.x = *hx * 8;
                ((BtlRecord *)rec)->sprite.y = g_BtlPartyPlaceYCells[i] * 8;
                BtlAnimationSet(rec, g_BtlPartyStandAnimations[i]);
                if (*hx < 0x10) {
                    ((BtlRecord *)(off + g_BtlRecords))->sprite.attr = 0x20;
                }
            } else {
                ((BtlRecord *)g_BtlRecords)[5].sprite.x = 0xA0;
                ((BtlRecord *)g_BtlRecords)[5].sprite.depth = 0x68;
                ((BtlRecord *)g_BtlRecords)[5].sprite.y = 0x68;
                BtlAnimationSet((void *)(g_BtlRecords + 0x140), 5);
            }
            q = (BtlRecord *)((i << 6) + g_BtlRecords);
            q->sprite.depth = q->sprite.y;
        }
        i++;
    } while (i < 6);
    if (g_BtlSlotIds[5] == 0xC) {
        g_BtlSlot5Roaming = 1;
        g_BtlSlot5RoamYState = 0;
        g_BtlSlot5RoamXState = 0;
        g_BtlSlot5RoamAnimationState = 0;
        g_BtlSlot5RoamYCount = 0;
        g_BtlSlot5RoamXCount = 0;
        g_BtlSlot5RoamAnimationWait = 0;
    } else {
        g_BtlSlot5Roaming = 0;
    }
}
/* far_syms: D_800119C4 (target reaches it %hi/%lo) */
/* far_syms: g_BtlEnemyGroups (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumberRows (target reaches it %hi/%lo) */
/* far_syms: g_BtlEnemySetIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountYOffsetTable (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountRecordsRunning (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountStepList (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountAnimationTable (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountYOffsets (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountPieces (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountTexelRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountPoseRows (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlEnemySpriteDataBlocks (target reaches it %hi/%lo) */
/* far_syms: g_BtlAmountAnimations (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlAmountRecordsRunning (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlEnemySet (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlNumber (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlAmountStepList (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlAmountAnimationTable (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlAmountYOffsets (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlAmountPieces (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlAmountTexelRecords (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlAmountPoseRows (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlAmountRecords (auto: target reaches it %hi/%lo — addressing row 3) */
extern u8 g_BtlEnemyGroups[];
extern u8 g_BtlNumberRows[];
extern u8 g_BtlEnemySetIds[];
extern u16 g_BtlAmountAnimations[];
extern u8 g_BtlAmountYOffsetTable[];
extern s32 g_BtlAmountRecordsRunning;
extern s32 g_BtlAmountStepList;
extern s32 g_BtlAmountAnimationTable;
extern s32 g_BtlAmountYOffsets;
extern s32 g_BtlAmountPieces;
extern s32 g_BtlAmountTexelRecords;
extern s32 g_BtlAmountPoseRows;
extern s32 g_BtlAmountRecords;
extern u8 D_800119C4[];
void BtlDataLoad(s32);
void BtlAmountRecordsInit(void);

/* func_8002B3AC */
int BtlEnemyLoad(void)
{
    char name[32];
    u8 i;
    u8 m;
    u8 k;
    s32 x;
    s32 y;
    u8 *p;
    u8 *ids;
    u8 *grp;
    u16 *entry;
    s32 *dest;
    s32 src;
    s32 dst;
    s32 tbl;
    s32 rec;
    s32 off;
    s32 v;
    s32 rec2;
    s32 xi;
    u16 *t;

    memcpy(name, D_800119C4, 13);
    p = &g_BtlNumberRows[g_BtlNumber * 2];
    i = 0;
    x = p[0];
    y = p[1];
    g_BtlEnemySet = x;
    ids = g_BtlEnemySetIds + x * 4;
    grp = g_BtlEnemyGroups + y * 24;
    BtlDataLoad(g_BtlEnemySet);
    t = g_BtlAmountAnimations;
    g_BtlAmountAnimationTable = (s32)&t[1];
    g_BtlAmountYOffsets = (s32)g_BtlAmountYOffsetTable;
    g_BtlAmountStepList = (s32)&t[t[0] * 2 + 1];
    g_BtlAmountPoseRows = xmalloc_heap_alloc(0x38);
    g_BtlAmountPieces = xmalloc_heap_alloc(0x3F0);
    g_BtlAmountTexelRecords = xmalloc_heap_alloc(0xFC);
    g_BtlAmountRecords = xmalloc_heap_alloc(0x340);
    g_BtlAmountRecordsRunning = 0;
    BtlAmountRecordsInit();
    while (i < 4 && ids[i] != 0)
        i++;
    tbl = g_ResourceReadBuffer;
    if (i != 0) {
        dest = (s32 *)g_BtlEnemySpriteDataBlocks;
        entry = (u16 *)tbl;
        do {
            i--;
            src = g_ResourceReadBuffer + entry[1] * 4;
            dst = xmalloc_heap_alloc(entry[0] * 4);
            entry += 2;
            inflate_buffer(src, dst);
            *dest++ = dst;
        } while (i != 0);
    }
    i = 0;
    k = 6;
    while (i < 8) {
        if (grp[i] == 0)
            break;
        m = 0;
        while (m < 3 && ids[m] != grp[i])
            m++;
        g_BtlSlotIds[k] = grp[i];
        tbl = g_BtlEnemySpriteDataBlocks[m];
        rec = k * 64 + g_BtlRecords;
        ((BtlRecord *)rec)->sprite.texel_records = tbl + ((SpriteDataHead *)tbl)->texel_records * 4;
        ((BtlRecord *)rec)->sprite.pose_table = tbl + ((SpriteDataHead *)tbl)->pose_table * 4;
        ((BtlRecord *)rec)->sprite.piece_table = tbl + ((SpriteDataHead *)tbl)->piece_table * 4;
        ((BtlRecord *)rec)->animation_table = tbl + ((SpriteDataHead *)tbl)->animation_table * 4;
        ((BtlRecord *)rec)->step_list = tbl + ((SpriteDataHead *)tbl)->step_list * 4;
        BtlRecordInit(k);
        off = k * 64 + g_BtlRecords;
        ((BtlRecord *)off)->sprite.attr = 0x20;
        ((BtlRecord *)off)->sprite.flags = 0x2100;
        ((BtlRecord *)off)->sprite.x = grp[i + 8] * 8;
        xi = grp[i + 16];
        v = ((BtlRecord *)off)->sprite.y = xi * 8;
        if (v < 0x70)
            ((BtlRecord *)off)->sprite.y = 0x70;
        else if (v >= 0xB1)
            ((BtlRecord *)off)->sprite.y = 0xB0;
        rec2 = k * 64;
        rec2 = g_BtlRecords + rec2;
        xi = (u16)((BtlRecord *)rec2)->sprite.y;
        ((BtlRecord *)rec2)->sprite.depth = xi;
        if (((BtlRecord *)rec2)->sprite.x >= 0xC0)
            ((BtlRecord *)rec2)->sprite.attr = 0x2020;
        BtlAnimationSet(g_BtlRecords + k * 64, 0);
        i++;
        k++;
    }
}

/* far_syms: g_BtlRecords */
void BtlRecordSizeSet(u8 *obj, u16 frame);

/* func_8002B770 */
void BtlRecordInit(s32 slot)
{
    s32 off;
    u16 *p;

    off = (slot & 0xFF) << 6;
    p = (u16 *)(off + g_BtlRecords);
    p[0] = 1;
    BtlRecordSizeSet((u8 *)p, 0);
    p = (u16 *)(off += g_BtlRecords);
    p[0x18] = 0xFFFF;
    p[2] = 1;
    p[0xE] = 0;
    p[0x10] = 0;
}
/* far_syms: g_BtlRecords CharStats g_BtlAutoEndAllowed g_BtlPartyMode g_BtlRoundsLeft */
extern s16 D_800B88FC;
extern u32 g_BtlPartySpriteDataBlocks[];
extern u32 D_800E4A20[];
extern u32 D_800E4A00[];
extern u8 g_BtlSlotAmountKinds[];
extern u8 g_BtlSlotStatus4Rounds[];
extern u8 g_BtlSlotStatus8Rounds[];
extern u8 g_BtlSlotStatus40Rounds[];
extern u8 g_BtlSlotStatus80Rounds[];
extern u16 D_800E4B40[];
extern u8 g_BtlPartyStatus1Shares[];
extern u8 g_BtlAutoEndAllowed;
extern u8 g_BtlPartyMode;
void mem_clear(void *destination, u32 byte_count);

/* func_8002B7DC */
void BtlStateInit(void)
{
    u8 i;

    i = 0;
    D_800B88FC = 0x20;
    g_BtlRecords = 0x801D2E80;
    CharStats = 0x801D2B00;
    mem_clear((void *)0x801D2E80, 0x700);
    mem_clear((void *)g_BtlRecords, 0x80);
    mem_clear((void *)CharStats, 0x348);
    do {
        g_BtlPartySpriteDataBlocks[i] = 0;
        D_800E4A20[i] = 0;
        i++;
    } while (i < 6);
    i = 0;
    do {
        g_BtlEnemySpriteDataBlocks[i] = 0;
        D_800E4A00[i] = 0;
        i++;
    } while (i < 8);
    i = 6;
    do {
        g_BtlSlotIds[i] = 0;
        g_BtlSlotAmountKinds[i] = 0;
        i++;
    } while (i < 0xE);
    i = 0;
    do {
        g_BtlSlotStatus4Rounds[i] = g_BtlSlotStatus8Rounds[i] = g_BtlSlotStatus40Rounds[i] = g_BtlSlotStatus80Rounds[i] = 0;
        i++;
    } while (i < 0xE);
    i = 0;
    do {
        g_BtlSpecialBytes[i] = 0;
        i++;
    } while (i < 0xA);
    i = 0;
    do {
        D_800E4B40[i] = 0;
        i++;
    } while (i < 5);
    i = 0;
    do {
        g_BtlPartyAttackAnimations[i] = 0;
        g_BtlPartyStandAnimations[i] = 0;
        g_BtlPartyHitAnimations[i] = 0;
        g_BtlPartyNotHeldFlags[i] = 1;
        g_BtlPartyStatus1Shares[i] = 0;
        i++;
    } while (i < 5);
    g_BtlAutoEndAllowed = 1;
    g_BtlPartyMode = 0;
    g_BtlRoundsLeft = 0xFF;
}
