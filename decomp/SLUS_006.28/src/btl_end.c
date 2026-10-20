/* Lunar SSSC — btl_end: the battle's end: the items and the money a win
   gives and BtlClose (the end on screen and in memory). The first split had
   it in rendering2_b.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
extern s32 g_BtlRecords;
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotActiveFlags[];
/* far_syms: g_BtlWinItems */
typedef struct {
    u8 a[8];
} Drops;
extern u8 g_BtlSlotIds[];
extern s32 CharStats;
extern u8 g_BtlWinItems[8];
extern s32 g_ChrItemList0C;
s32 BtlWinItemRateCheck(s32 item, s32 rate);
void ChrItemListSort(u16 who);

/* Battle rewards. Each enemy slot 6..13 that is present (g_BtlSlotIds)
   rolls its first drop (record +0x20 item, +0x21 rate) through
   BtlWinItemRateCheck and, failing that, its second (+0x22, +0x23); an item id
   must be 1..0xA4. Hits go to the 8-entry drop list g_BtlWinItems, which is
   then appended to the 0x60-entry bag g_ChrItemList0C. If the bag fills, the
   rest goes to party members 0..4 (not ids 8..10): one item into each
   member's first empty slot (+0x2E, six per member) per pass, until the
   list runs out. The unused tail of the list is zeroed, then
   ChrItemListSort runs for each nonzero party[0..4] and once with 0xC.
   The drop list is read through a struct view: indexing through a cast
   pointer emits the address before the index, which is the order the
   bag loop's exit test and the overflow test both need. */
/* func_80028A54 */
void BtlWinItemsGive(u8 *party)
{
    u8 i;
    u8 n;
    u8 it;

    n = 0;
    for (i = 6; i < 14; i++) {
        if (g_BtlSlotIds[i] != 0) {
            it = ((BtlStats *)CharStats)[i].side.enemy.win_item_1;
            if (it != 0 && it <= 0xA4) {
                if ((u8)BtlWinItemRateCheck(it, ((BtlStats *)CharStats)[i].side.enemy.win_rate_1) != 0) {
                    ((Drops *)g_BtlWinItems)->a[n] = it;
                    n++;
                } else {
                    it = ((BtlStats *)CharStats)[i].side.enemy.win_item_2;
                    if ((u8)BtlWinItemRateCheck(it, ((BtlStats *)CharStats)[i].side.enemy.win_rate_2) != 0) {
                        ((Drops *)g_BtlWinItems)->a[n] = it;
                        n++;
                    }
                }
            }
        }
    }
    n = 0;
    i = 0;
    while (((u8 *)g_ChrItemList0C)[n] != 0) {
        if (n >= 0x60) break;
        n++;
    }
    while (((Drops *)g_BtlWinItems)->a[i] != 0 && i < 8 && n < 0x60) {
        ((u8 *)g_ChrItemList0C)[n] = ((Drops *)g_BtlWinItems)->a[i];
        i++;
        n++;
    }
    if (((Drops *)g_BtlWinItems)->a[i] != 0 && i < 8) {
        for (n = 0; n < 5; n++) {
            if (g_BtlSlotIds[n] != 0 && (g_BtlSlotIds[n] < 8 || g_BtlSlotIds[n] > 10)) {
                for (it = 0; it < 6; it++) {
                    if (((BtlStats *)CharStats)[n].items[it] == 0) {
                        ((BtlStats *)CharStats)[n].items[it] = ((Drops *)g_BtlWinItems)->a[i];
                        i++;
                        if (i == 8) goto done;
                        if (((Drops *)g_BtlWinItems)->a[i] == 0) i = 8;
                        break;
                    }
                }
                if (i == 8) break;
            }
        }
    }
done:
    for (; i < 8; i++) {
        ((Drops *)g_BtlWinItems)->a[i] = 0;
    }
    for (i = 0; i < 5; i++) {
        if (party[i] != 0) {
            ChrItemListSort(party[i]);
        }
    }
    ChrItemListSort(0xC);
}

s32 screen_disp_mask_set(s32);
extern u8 g_BtlSlotIds[];
extern u32 g_BtlWinExperience;
extern u32 *g_GameMoney;
extern s32 CharStats;
extern u16 g_BtlWinMoney;

/* func_80028D78 */
void BtlWinRewardSum(void)
{
  s32 slot;
  s32 slot_byte;
  unsigned long money_sum;
  u32 experience_sum;
  void *stats;
  int ra_arm;
  money_sum = 0;
  experience_sum = 0;
  slot = 6;
  slot_byte = 6 & 0xFF;
  do
  {
    slot_byte = slot & 0xFF;
    if ((*(slot_byte + g_BtlSlotIds)) != 0)
    {
      stats = (slot_byte * 0x3C) + CharStats;
      money_sum += ((BtlStats *)stats)->side.enemy.money;
      experience_sum += ((BtlStats *)stats)->side.enemy.experience;
    }
    slot += 1;
  }
  while (((u32) ((char) (slot & 0xFF))) < 0xEU);
  if (experience_sum > 0x1869FU)
  {
    g_BtlWinExperience = 0x1869F;
  }
  else
  {
    g_BtlWinExperience = experience_sum;
  }
  ra_arm = 0xFFFF;
  if (money_sum > 0xFFFFU)
  {
    g_BtlWinMoney = ra_arm;
  }
  else
  {
    g_BtlWinMoney = money_sum;
  }
  *g_GameMoney += money_sum;
  if (((u32) (*g_GameMoney)) > 0x98967FU)
  {
    *g_GameMoney = 0x98967F;
  }
}
extern s8 g_BtlSlot5Roaming;
extern u8 g_BtlPartyStandAnimations[];
extern BtlWalkBlock g_BtlWalkBlock;
void VoiceSet();
void VoiceStart();
s32 VoiceStep();
s32 StoryBitCheck();
void BtlAnimationSet(void *record, s32 animation);
void BtlAnimationEndWait(void *record);
s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlFrameStep();
s32 BtlPartyPanelDisp();

/* func_80028E5C */
void BtlSlot5Revive(s32 voice_on)
{
    BtlRecord rec;
    u8 i;
    BtlWalkBlock *blk;
    u8 *r;
    u16 attr;

    if (voice_on & 0xFF)
        VoiceSet(0x3D);
    g_BtlSlot5Roaming = 0;
    switch (((BtlRecord *)(g_BtlRecords + 0x140))->animation)
    {
    case 2:
        BtlAnimationEndWait((void *)(g_BtlRecords + 0x140));
    case 1:
    case 5:
        BtlAnimationSet((void *)(g_BtlRecords + 0x140), 3);
    case 3:
        BtlAnimationEndWait((void *)(g_BtlRecords + 0x140));
    case 4:
        BtlAnimationSet((void *)(g_BtlRecords + 0x140), 0);
    }
    if (((BtlRecord *)(g_BtlRecords + 0x140))->sprite.x < ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x)
        rec.sprite.x = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x - 8;
    else
        rec.sprite.x = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x + 8;
    AS(u16, rec.sprite.y) = AS(u16, ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.y);
    rec.width = rec.height = 8;
    BtlWalkSet();
    blk = &g_BtlWalkBlock;
    blk->walker_slot = 5;
    blk->speed = 3;
    blk->reach = 0x28;
    blk->target = (s32)&rec;
    blk->animation = 6;
    blk->sound = 0x2C;
    blk->walker = g_BtlRecords + 0x140;
    BtlWalkRun();
    if (((BtlRecord *)(g_BtlRecords + 0x140))->sprite.x < ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x)
        ((BtlRecord *)(g_BtlRecords + 0x140))->sprite.attr = 0x20;
    else
        ((BtlRecord *)(g_BtlRecords + 0x140))->sprite.attr = 0x2020;
    AS(u16, ((BtlRecord *)(g_BtlRecords + 0x140))->sprite.depth)++;
    BtlAnimationSet((void *)(g_BtlRecords + 0x140), 7);
    if (voice_on & 0xFF)
        VoiceStart();
    i = 0;
    do
    {
        if ((voice_on & 0xFF) && !(VoiceStep(1) & 0xFF))
            voice_on = 0;
        i++;
        BtlFrameStep();
    } while (i < 0x3C);
    if (voice_on & 0xFF)
    {
        while (VoiceStep(1) & 0xFF)
            BtlFrameStep();
    }
    i = 0;
    do
    {
        switch (i)
        {
        case 1:
            ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x - 1;
            break;
        case 21:
            ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x - 1;
            break;
        case 31:
            ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x - 1;
            break;
        case 2:
        case 22:
        case 32:
            ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x + 1;
            break;
        }
        i++;
        BtlFrameStep();
    } while (i < 0x21);
    AS(u16, ((BtlRecord *)(g_BtlRecords + 0x140))->sprite.depth)--;
    BtlAnimationSet((void *)(g_BtlRecords + (g_BtlTargetSlot << 6)), g_BtlPartyStandAnimations[g_BtlTargetSlot]);
    if (StoryBitCheck(0x2AB))
        *(u16 *)(g_BtlTargetSlot * 0x3C + CharStats) = 10;
    else
        *(u16 *)(g_BtlTargetSlot * 0x3C + CharStats) = 1;
    r = (u8 *)(g_BtlTargetSlot * 0x3C + CharStats);
    ((BtlStats *)r)->stat_08 = ((BtlStats *)r)->side.party.stat_08_start;
    ((BtlStats *)r)->stat_0a = ((BtlStats *)r)->side.party.stat_0a_start;
    g_BtlSlotActiveFlags[g_BtlTargetSlot] = 1;
    BtlPartyPanelDisp();
    BtlAnimationSet((void *)(g_BtlRecords + 0x140), 0);
}

/* far_syms: g_BtlPartyKeptRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlEnemySpriteDataBlocks (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartySpriteDataBlocks (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyKeptRecords (auto: target reaches it %hi/%lo — addressing row 3) */
extern u8 g_BtlNumber;
extern s32 g_BtlPartyKeptRecords;
extern u8 g_BtlSlotAmountKinds[];
extern u8 g_BtlEnemySpriteDataBlocks[];
extern u8 g_BtlPartySpriteDataBlocks[];

/* func_80029214 */
void BtlClose(void)
{
  s32 grey_then_i;
  s32 *entry;
  s32 sprite_data;

  grey_then_i = 0;
  do
  {
    screen_dark_draw(grey_then_i);
    grey_then_i += 0xA;
    BtlFrameStep();
  } while (grey_then_i < 0xFF);

  screen_disp_mask_set(0);
  sprite_mix_col_set(0);
  BtlFrameStep();
  BtlMenuFree();
  BtlAmountRecordsFree();
  func_8007B7C0();

  if (g_BtlNumber >= 0xEB)
  {
    xmalloc_heap_free(g_BtlPartyKeptRecords);
  }

  BtlCellRowsClear(0x17, 4, 0x20);

  grey_then_i = 0;
  entry = (s32 *) g_BtlPartySpriteDataBlocks;
  do
  {
    sprite_data = *entry;
    if (sprite_data != 0)
    {
      xmalloc_heap_free(sprite_data);
    }
    grey_then_i += 1;
    entry += 1;
  } while (((u32) grey_then_i) < 6U);

  grey_then_i = 0;
  entry = (s32 *) g_BtlEnemySpriteDataBlocks;
  do
  {
    sprite_data = *entry;
    if (sprite_data != 0)
    {
      xmalloc_heap_free(sprite_data);
    }
    grey_then_i += 1;
    entry += 1;
  } while (((u32) grey_then_i) < 8U);

  grey_then_i = 0;
  do
  {
    g_BtlSlotAmountKinds[grey_then_i] = 0;
    grey_then_i += 1;
  } while (((u32) grey_then_i) < 0xEU);
}
