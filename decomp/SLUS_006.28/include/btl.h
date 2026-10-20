/* btl.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef BTL_H
#define BTL_H

#include "layout.h"
#include "sprite.h"

/* BtlRecord: one of the battle's 0x40-byte records, which g_BtlRecords points
   at (the slots, the shows' records, the walk, the amounts): whether it is in
   use, the sprite entry its animation copies to the sprite list each frame,
   the animation's words, and the figure's size. */
typedef struct {
    /* +0x00 the record's on word: 1 while the record is in use, 0 off.
       BtlAnimationStep (and BtlAmountRecordsStep for the amount records) steps
       a record's animation and copies its sprite entry to the sprite list only
       while it is not 0. Set 1 by BtlRecordInit and by the shows for the
       copies they draw; 0 by BtlRecordSpriteDataSet (made ready, not yet
       drawn), BtlSlotLeave, BtlF7Slot7RecordOff, BtlRecordClear, BtlMenuReset
       and the shows for the slot record an effect's record stands in for;
       BtlShowB6Run flips it (^= 1) to blink a record out and in. */
    u16 on;
    /* +0x02 nothing found reads or writes it on its own; it goes with the
       whole-record copies and clears (BtlStateInit's mem_clear of 0x700 bytes,
       StartRecordsClear and the whole-record copies). */
    u16 pad_02;
    /* +0x04..+0x27 the record's sprite entry: the 0x24 bytes BtlAnimationStep
       copies whole, every frame, to entry first + n of the sprite list
       (g_SpriteList, 0x96 entries of 0x24 bytes), then sets the copy's pose
       from the step list. The same 0x24 bytes are the entry's layout as
       sprite_draw and sprite_piece_draw (sprite_list.c) read it, and the map's
       0x3C-byte records carry the same entry at +4 (MenuRecordsAnimationStep,
       ShopAnimationStep copy it the same way). It is a SpriteEntry (sprite.h),
       embedded by the owner's ruling of 2026-10-14. */
    SpriteEntry sprite;
    /* +0x28 the address of the animation table: one 4-byte row for each
       animation number, a 16-bit count of steps (top bit: hold on the last
       step) and the 16-bit number of the first step in the step list. Written
       by BtlRecordSpriteDataSet, BtlRecordSpriteBlocksSet, BtlEffectLoad,
       BtlPartyLoad, BtlEnemyLoad (from the sprite data's header word 0),
       BtlAmountRecordsInit (D_800B8D6C); zeroed by BtlRecordClear and
       BtlAmountShowCheck. */
    s32 animation_table;
    /* +0x2C the address of the step list: 4-byte steps, a pose and a length in
       frames (AnimStep, btl_animation.c). For the amount records the first word
       of a step picks a y offset from g_BtlAmountYOffsets instead of a pose.
       Written and zeroed by the same functions as animation_table (D_800B8D68
       for the amount records). */
    s32 step_list;
    /* +0x30 the number of the animation the record is on: BtlAnimationSet
       stores a new number here (keeping the old one in animation_before) and
       starts it with BtlAnimationStart; BtlRecordInit sets 0xFFFF so that the
       next BtlAnimationSet starts whatever it names; BtlRecordSpriteDataSet,
       BtlRecordSpriteBlocksSet, BtlAmountRecordStart and BtlShowB0Run store a
       number directly. */
    u16 animation;
    /* +0x32 the animation number before the last change: BtlAnimationSet
       stores the old +0x30 here when it changes the animation;
       BtlRecordSpriteDataSet and BtlRecordSpriteBlocksSet store 1 for
       animation 0 and 0 for any other (a number unlike the new one). No read
       of it was found in the C. */
    u16 animation_before;
    /* +0x34 frames spent on the current step: BtlAnimationStart sets 0 (with
       its flag), BtlAnimationStep, BtlAmountRecordsStep and
       BtlAnimationFramesCount (on its stack copy) count it up to step_length;
       BtlAmountRecordStart sets 0; BtlShowB6Run sets it to step_length so that
       the next frame steps on. */
    u16 timer;
    /* +0x36 the length in frames of the current step, read from the step list
       when a step begins (BtlAnimationStart, BtlAnimationStep,
       BtlAmountRecordStart, BtlAmountRecordsStep, BtlAnimationFramesCount). */
    u16 step_length;
    /* +0x38 the number of the current step in the step list: BtlAnimationStart
       puts it on the animation's first step, BtlAnimationStep moves it on (and
       back by one at the end when the end's top bit holds the last step),
       BtlAmountRecordStart sets 0. */
    u16 step;
    /* +0x3A one past the animation's last step: first step + count as 16 bits,
       so the count's top bit stays in bit 15, which BtlAnimationStep reads as
       'hold the last step' (otherwise the animation starts again). Written by
       BtlAnimationStart (func_8004B164 and BtlAmountRecordStart's own start
       the same way). */
    u16 end;
    /* +0x3C the width in pixels of the record's figure: BtlRecordSizeSet
       measures the pieces of the first pose of an animation (from
       BtlRecordInit with animation 0, BtlRecordSpriteDataSet and
       BtlRecordSpriteBlocksSet with theirs). BtlSpecialInit sets or narrows
       slot 6's for some battles (0x70, 0x50, - 0x14, - 0x30, - 0x10); walk
       targets get 8 (record 14 in BtlWalkEdgeRun, the stack records of
       BtlSlot5Revive, BtlSlot5ReviveTry, BtlShowA3Run); BtlShowAFRun widens
       the acting record by 0x20 and back; BtlTargetActingRecordCopy keeps and
       restores it; an amount record copies its slot record's
       (BtlAmountRecordStart). */
    u16 width;
    /* +0x3E the height in pixels of the record's figure, measured with width
       by BtlRecordSizeSet; 8 for the walk targets; kept and restored by
       BtlTargetActingRecordCopy. The marks and shows put things above the
       figure with it (BtlMenuTargetMark: mark y = y - height; BtlShow17Run,
       BtlShow28Run, BtlShow3DRun, BtlShowRecordTravelRun). For the amount
       records it is never measured: BtlAmountRecordStart sets it to 0 and
       BtlAmountRecordStart and BtlAmountRecordsStep take it off the y they
       compute, so it stays 0 there. */
    u16 height;
} BtlRecord;

LAYOUT_SIZE(BtlRecord, 0x40);
LAYOUT_AT(BtlRecord, on, on, 0x00);
LAYOUT_AT(BtlRecord, pad_02, pad_02, 0x02);
LAYOUT_AT(BtlRecord, sprite, sprite, 0x04);
LAYOUT_AT(BtlRecord, sprite_on, sprite.on, 0x04);
LAYOUT_AT(BtlRecord, sprite_x, sprite.x, 0x06);
LAYOUT_AT(BtlRecord, sprite_depth, sprite.depth, 0x08);
LAYOUT_AT(BtlRecord, sprite_y, sprite.y, 0x0a);
LAYOUT_AT(BtlRecord, sprite_attr, sprite.attr, 0x0c);
LAYOUT_AT(BtlRecord, sprite_flags, sprite.flags, 0x0e);
LAYOUT_AT(BtlRecord, sprite_texel_records, sprite.texel_records, 0x10);
LAYOUT_AT(BtlRecord, sprite_pose_table, sprite.pose_table, 0x14);
LAYOUT_AT(BtlRecord, sprite_piece_table, sprite.piece_table, 0x18);
LAYOUT_AT(BtlRecord, sprite_pose, sprite.pose, 0x1c);
LAYOUT_AT(BtlRecord, sprite_pad_1a, sprite.pad_1a, 0x1e);
LAYOUT_AT(BtlRecord, sprite_scaled, sprite.scaled, 0x20);
LAYOUT_AT(BtlRecord, sprite_pad_1e, sprite.pad_1e, 0x22);
LAYOUT_AT(BtlRecord, sprite_scale_x, sprite.scale_x, 0x24);
LAYOUT_AT(BtlRecord, sprite_scale_y, sprite.scale_y, 0x26);
LAYOUT_AT(BtlRecord, animation_table, animation_table, 0x28);
LAYOUT_AT(BtlRecord, step_list, step_list, 0x2c);
LAYOUT_AT(BtlRecord, animation, animation, 0x30);
LAYOUT_AT(BtlRecord, animation_before, animation_before, 0x32);
LAYOUT_AT(BtlRecord, timer, timer, 0x34);
LAYOUT_AT(BtlRecord, step_length, step_length, 0x36);
LAYOUT_AT(BtlRecord, step, step, 0x38);
LAYOUT_AT(BtlRecord, end, end, 0x3a);
LAYOUT_AT(BtlRecord, width, width, 0x3c);
LAYOUT_AT(BtlRecord, height, height, 0x3e);

/* One battle slot's stats: 14 records of 0x3C bytes at the address in
   CharStats (the word at 0x800B8C20, which BtlStateInit sets to 0x801D2B00
   before it clears 0x348 = 14 * 0x3C bytes), indexed by slot (0 to 4 the
   party, 5 the slot that stands apart, 6 to 13 the enemies). BtlPartyStatsSet
   fills slots 0 to 4 from the characters' records (ChrRecord, g_ChrRecords)
   and BtlEnemyStatsSet fills 6 to 13 from the enemies' 0x26-byte rows at
   D_800A8768; +0x1A to +0x2D hold different things for the two sides (the
   union side). */
typedef struct {
    /* +0x00 the slot's hp; 0 for a slot that has fallen. BtlPartyStatsSet
       copies the character record's hp, BtlEnemyStatsSet the enemy row's word
       +2 (also into hp_max), BtlEnemyLevelSet raises it with hp_max. Hits take
       it down (BtlHitAmountTake leaves 1 and sets the fall mark when the
       amount is not below it; BtlRoundStatusUpdate for status 1;
       BtlEnemy7AActionSet); heals raise it to hp_max at most
       (BtlTargetHpFillRun, BtlTargetHpAddRun, BtlTargetHpShareAddRun and
       BtlPartyHpShareAddRun through g_BtlTargetStats, BtlPartyHpDrainRun,
       BtlTargetHpDrainRun, BtlPartyRestoreRun, BtlTargetReviveRun,
       BtlRoundStatusUpdate's 10 and 5 hundredths a round); BtlWinRun adds a
       level's gain; BtlFallRun, BtlEnemyClearRun, BtlTargetRemoveRun and
       BtlPartyPlaceFallRun set 0; BtlSlot5Revive sets 10 or 1;
       BtlSlot7HpSetRun sets slot 7's to level * 10 + 500 (battle 0xF7's second
       pool, BtlHitAmountTake). BtlPartyStatsStore writes it back to the
       character record, 1 for 0. */
    u16 hp;
    /* +0x02 the most hp the slot can have: heals stop at it,
       BtlRoundStatusUpdate's per-round gain and loss and
       BtlPartyHpShareAddRun's amount are hundredths of it. BtlPartyStatsSet
       copies the character record's hp_max, BtlEnemyStatsSet the enemy row's
       word +2; BtlEnemyLevelSet raises it (held at 0xFFFF); BtlWinRun adds a
       level's gain (ChrLevelGainAdd, held at 999). Nothing else writes it. */
    u16 hp_max;
    /* +0x04 the slot's mp, what a spell's cost (byte +4 of its 12-byte row,
       halved and rounded up for a slot with trait 0x1B) is taken from.
       BtlPartyStatsSet copies the character record's mp; enemies' stay 0
       (BtlEnemyStatsSet does not write it). BtlActionSpellRun takes the cost
       off; BtlTargetMpAddRun and BtlTargetMpFillRun (through g_BtlTargetStats)
       raise it to mp_max at most; BtlTargetReviveRun sets it to mp_max for
       item 0x83; BtlWinRun adds a level's gain. BtlPartyStatsStore writes it
       back to the character record. */
    u16 mp;
    /* +0x06 the most mp the slot can have. BtlPartyStatsSet copies the
       character record's mp_max; BtlWinRun adds a level's gain (held at 999).
       Enemies' stay 0. */
    u16 mp_max;
    /* +0x08 a stat, named by its place (docs/naming.md: the C does not say
       what each stat is): the acting slot's +8 is what a hit's amount starts
       from (BtlHitAmountSet: +8 plus g_BtlActionPower percent of it, a draw
       below +8 / 10 + 2, the target's stat_0a taken off); the sums over the
       sides (BtlPartyStat08Sum, BtlEnemyStat08Sum) and the target finds
       (BtlPartyTargetStat08Find, BtlEnemyStat08HighestFind) use it.
       BtlPartyStatsSet copies the character record's (worn items counted) and
       keeps a copy in side.party.stat_08_start; BtlEnemyStatsSet takes the
       enemy row's word +4; BtlEnemyLevelSet raises it; BtlStatRaise raises it
       by a percentage held to 170% of stat_08_start (BtlActingStat08RaiseRun,
       BtlTargetStat08RaiseRun, BtlTargetStat08Stat0ARaiseRun);
       BtlTargetReviveRun and BtlSlot5Revive put stat_08_start back;
       BtlWornItemStatLower takes a removed worn item's bonus off (selector 1). */
    u16 stat_08;
    /* +0x0A a stat, named by its place: the target's +0xA is taken off a hit's
       amount (BtlHitAmountSet). BtlPartyStatsSet copies the character record's
       and keeps a copy in side.party.stat_0a_start; BtlEnemyStatsSet takes the
       enemy row's word +6; BtlEnemyLevelSet raises it; BtlStatRaise raises it
       held to 170% of stat_0a_start (BtlTargetStat0ARaiseRun,
       BtlTargetStat08Stat0ARaiseRun); BtlTargetReviveRun and BtlSlot5Revive
       put stat_0a_start back; BtlWornItemStatLower (selector 2). */
    u16 stat_0a;
    /* +0x0C a stat, named by its place: the key the round's order is sorted by
       (BtlTurnOrderMake: +0xC plus a draw; 0 for a slot whose status has a bit
       of 0x0C). BtlPartyStatsSet copies the character record's,
       BtlEnemyStatsSet the enemy row's word +8; BtlEnemyLevelSet raises it;
       BtlWornItemStatLower (selector 3). */
    u16 stat_0c;
    /* +0x0E a stat, named by its place: the acting slot's +0xE is what the
       other kind of amount starts from (BtlStat0EHitAmountSet,
       BtlAreaStat0EHitRun: +0xE plus level times the power, a draw below +0xE
       / 10, the target's stat_10 taken off), and the chance that a status
       takes is the acting slot's +0xE plus the power less the target's +0xE
       (and, for a party target, its side.party.stat_1f) (BtlStatusTakeCheck).
       BtlPartyStatsSet copies the character record's, BtlEnemyStatsSet the
       enemy row's word +0xA; BtlEnemyLevelSet raises it; BtlWornItemStatLower
       (selector 4). */
    u16 stat_0e;
    /* +0x10 a stat, named by its place: the target's +0x10 is taken off the
       stat-0E amount (BtlStat0EHitAmountSet, BtlAreaStat0EHitRun).
       BtlPartyStatsSet copies the character record's, BtlEnemyStatsSet the
       enemy row's word +0xC; BtlEnemyLevelSet raises it; BtlWornItemStatLower
       (selector 5). */
    u16 stat_10;
    /* +0x12 how many times an attack (kind 2) hits: BtlActionAttackRun walks
       and hits that many times; for kind 3 BtlHitAmountSet applies the spell's
       percentage that many times less 1. BtlPartyStatsSet copies the character
       record's attack_count (worn items counted), BtlEnemyStatsSet the enemy
       row's byte +0x10; BtlWornItemStatLower takes a removed worn item's bonus
       off (selector 6). */
    u8 attack_count;
    /* +0x13 the walk's reach: the most cells a walk may cover. BtlWalkSet
       copies it to the walk block's +0xF (BtlWalkRun halves it for a party
       slot with status bit 0x10 and holds it at 0x28). BtlPartyStatsSet copies
       the character record's walk_reach, BtlEnemyStatsSet the enemy row's byte
       +0xE; BtlWornItemStatLower (selector 8). */
    u8 walk_reach;
    /* +0x14 the slot's level: the level gap adds to a hit's amount
       (BtlHitAmountSet), the level times the power to the stat-0E amount
       (BtlStat0EHitAmountSet, BtlAreaStat0EHitRun); BtlTargetStatus8ByLevelRun
       picks by it; BtlEnemyLevelSet takes slot 0's when it is higher;
       BtlSlot7HpSetRun makes slot 7's hp from the acting slot's.
       BtlPartyStatsSet copies the character record's level (+0x1C),
       BtlEnemyStatsSet the enemy row's byte +1, BtlEnemyLevelSet sets the
       enemies', BtlWinRun adds 1 at each level-up. */
    u8 level;
    /* +0x15 the slot's status bits, each named by its bit (docs/naming.md):
       0x01 loses a share of hp_max each round (BtlRoundStatusUpdate,
       g_BtlPartyStatus1Shares); 0x04 and 0x08 keep the slot from acting (and
       double the hits it takes, BtlHitAmountSet & 0xC); for a party slot
       BtlRoundStatusUpdate turns 0x10 into 0x20 and 0x20 into 0x30, which is
       off the field (active byte 0); 0x40 has the slot act on its own side;
       0x80 keeps it from casting. BtlStateInit leaves 0; BtlInit sets 0x40 on
       the party for story bit 0x2D5, BtlSpecialInit 4 for battle 0xF9,
       BtlPartyPairStatusSet 8; the BtlTargetStatus*Set functions (btl_status.c)
       and tries (btl_target.c) set their bit, BtlTargetStatusClearRun,
       BtlTargetStatus8Clear and BtlRoundStatusUpdate clear bits;
       BtlTargetReviveRun, BtlPartyRestoreRun, BtlEnemyClearRun, BtlFallRun and
       BtlWinRun set 0. BtlTurnOrderRun copies the acting slot's into
       g_BtlActingStatus. */
    u8 status;
    /* +0x16 the least amount the acting slot's hit takes: BtlHitAmountSet
       raises an amount below it to it. BtlPartyStatsSet takes byte +0xB of the
       first worn item's 0x12-byte row (left 0 when there is none),
       BtlEnemyStatsSet the enemy row's byte +0xF. */
    u8 least_amount;
    /* +0x17 the show of an attack: for an acting slot of 6 or more
       BtlAttackHitShow and BtlAttackMissShow hand it to BtlActingShowRun in
       place of the attack animation when it is not 0, and BtlAttackWalkRun
       (slots of 5 or more) decides by it whether the slot walks (0x98, 0x9A,
       0x9B, 0xA4, 0xA5, 0xAC, 0xAF, 0xB1 do not; 0x9D, 0xA7 ask
       BtlTargetReachCheck). BtlEnemyStatsSet takes the enemy row's byte +0x16;
       BtlPartyStatsSet writes byte +0xE of the first worn item's row, which
       nothing reads for a party slot. */
    u8 attack_show;
    /* +0x18 the speed of the slot's walk: BtlWalkSet copies it to the walk
       block's +0xE; BtlShow97Run, BtlShow99Run and BtlShow9DRun read it for
       their walks. BtlPartyStatsSet sets 3, BtlEnemyStatsSet takes the enemy
       row's byte +0x15. */
    u8 walk_speed;
    /* +0x19 the sound of the slot's attack: BtlAttackAnimationRun plays it
       (SoundPlay(+0x19, 3)), and BtlActingShowRun makes it the action's sound
       g_BtlActionSound for an action of kind 2. BtlPartyStatsSet picks it by
       the first worn item's kind (0x3B, 0x39, 0x43, 0x3C, 0x4B, by id for kind
       5) or 0x44 with none; BtlEnemyStatsSet takes the enemy row's byte +0x17. */
    u8 attack_sound;
    /* +0x1A..+0x2D what the stats hold for the slot's side: side.party for
       slots 0 to 4 (BtlPartyStatsSet), side.enemy for 6 to 13
       (BtlEnemyStatsSet). The readers tell the sides apart by slot (below 5,
       or 5 or more, or 6 or more); slot 5's bytes are written by neither
       setter and stay 0. See unions. */
    union {
        /* party: used by BtlActingStat08RaiseRun, BtlFallRun,
           BtlPartyFlag1SetRun, BtlPartyFlag2SetRun, BtlPartyStatsSet,
           BtlPartyStatsStore, BtlRoundStatusUpdate, BtlSlot5Revive,
           BtlStatusTakeCheck, BtlTargetFallRun, BtlTargetHpAmountTake,
           BtlTargetReviveRun and others */
        struct {
            /* +0x1A stat_08 as BtlPartyStatsSet set it at the battle's start:
               BtlTargetReviveRun and BtlSlot5Revive put stat_08 back from it,
               and BtlStatRaise holds a raise to 170% of it. Written only by
               BtlPartyStatsSet. */
            u16 stat_08_start;
            /* +0x1C stat_0a as BtlPartyStatsSet set it: put back by
               BtlTargetReviveRun and BtlSlot5Revive, the limit of BtlStatRaise
               for stat_0a. Written only by BtlPartyStatsSet. */
            u16 stat_0a_start;
            /* +0x1E a party slot's flag bits: 0x01 gives 5 hundredths of
               hp_max back each round (BtlRoundStatusUpdate; set by
               BtlPartyFlag1SetRun); 0x02 stops one stat-0E amount or status
               and is then cleared (BtlTargetHpAmountTake, BtlStatusTakeCheck,
               BtlTargetFallRun; set by BtlPartyFlag2SetRun); 0x04 goes with
               status 8 (BtlPartyPairStatusSet sets 4 with it; cleared whenever
               the status changes: BtlTargetStatusAllClearRun,
               BtlPartyRestoreRun, BtlTargetStatus1Set/4Set/10Set/30Set,
               BtlTargetStatus8Clear, BtlRoundStatusUpdate, BtlWinRun) and is
               shown on the party's panel (BtlPartyPanelDisp). BtlFallRun sets
               0. Every writer but BtlTargetFallRun tests for a slot below 5 or
               walks slots 0 to 4 only. */
            u8 flags;
            /* +0x1F a stat, named by its place: for a party target
               BtlStatusTakeCheck adds it to the target's stat_0e, so it lowers
               the chance a status takes. BtlPartyStatsSet copies the character
               record's +0x1D (worn items counted; worn items raise it as
               selector 7), BtlWornItemStatLower takes a removed item's bonus
               off. The same stat as ChrRecord.stat_1d. */
            u8 stat_1f;
            /* +0x20 the kind (byte +0xF of its 0x12-byte row) of the party
               slot's first worn item, 0 with none: BtlPartyStatsSet picks the
               slot's attack, stand and move animations and attack_sound by it;
               for 3 and 4 (and 5 for id 2) BtlAttackWalkRun does not walk the
               slot to its target; BtlAttackAnimationRun picks show 0x3A for 3
               (and 5 for id 2). Written only by BtlPartyStatsSet. */
            u8 worn_item_kind;
            /* +0x21 nothing reads or writes it for a party slot
               (BtlPartyStatsSet skips it; it is side.enemy.win_rate_1 for an
               enemy). */
            u8 pad_21;
            /* +0x22..+0x27 the party slot's six worn items, each the number
               (from 1) of a 0x12-byte item row, 0 for none; the first is the
               one whose row gives least_amount, attack_show and
               worn_item_kind. BtlPartyStatsSet copies the character record's
               worn items; BtlWornItemRemove sets one to 0 (place 4 or 5:
               BtlItem83Revive uses up item 0x83 there). The battle's item menu
               offers them with the items (BtlMenuItemOpen cells 0 to 5,
               BtlMenuActionCheck). */
            u8 worn_items[6];
            /* +0x28..+0x2D the party slot's six traits, byte +0xD of each worn
               item's row (BtlPartyStatsSet, with no test for an empty place).
               BtlTargetTraitCheck answers 1 when one of them is a number it is
               asked for (0x1B halves a spell's cost, 0x1D gives 10 hundredths
               of hp back each round, 0x32..0x34 cut an amount, 0x21/0x25/0x26
               keep statuses off); the acting slot's first trait gives the
               element of its hit (BtlHitAmountSet, 0x2A..0x2F), a fall chance
               (BtlHitSlotsUpdate), a status on the target (BtlHitStatusUpdate)
               and a strong-hit chance (BtlHitStrongTry); 0x18 in the fifth or
               sixth makes BtlActionAttackRun hit every target
               (BtlAttackAllRun). */
            u8 traits[6];
        } party;
        /* enemy: used by BtlActingShowRun, BtlEnemyStatsSet, BtlWalkSet,
           BtlWinItemsGive, BtlWinRewardSum */
        struct {
            /* +0x1A which pick makes the enemy's choice of action:
               BtlEnemyActionPick switches on it (1 to 32 pick among
               BtlEnemyAttackPick, BtlEnemyKind7Pick and
               BtlEnemyAttackKind7Pick with a target rule; 0 and above 32 a
               default by draws); BtlActionTargetAdjust sends 2, 7, 0xC, 0x11
               and 0x1A to the nearest party slot. BtlEnemyStatsSet takes the
               enemy row's byte +0. */
            u8 action_pick;
            /* +0x1B the trait of a slot of 5 or more, the one byte
               BtlTargetTraitCheck tests against the numbers it is asked for
               (the party's six are side.party.traits); BtlHitStrongTry and
               BtlHitStatusUpdate read the acting slot's. BtlEnemyStatsSet
               takes the enemy row's byte +0x11. */
            u8 trait;
            /* +0x1C for an enemy's action of kind 2, the number of its row of
               g_BtlAttackRows: not 0 sends the attack to BtlActionRowRun
               (BtlActionKindRun), which takes it as the action's number and
               runs the row as kind 7; 0 is a plain attack
               (BtlActionAttackRun). BtlEnemyStatsSet takes the enemy row's
               byte +0x12. */
            u8 attack_number;
            /* +0x1D the number of the enemy's action of kind 7, a row of
               g_BtlAttackRows: BtlEnemyActionAdjust gives a slot whose planned
               kind is 7 this number; BtlEnemyKind7Pick tests byte 0 of its
               row; for id 0x4F BtlActionRowRun and BtlActingShowRun use it in
               place of the planned number. BtlEnemyStatsSet takes the enemy
               row's byte +0x13. */
            u8 kind7_number;
            /* +0x1E the sound of an enemy's walk: BtlWalkSet puts it in the
               walk block's +0x10 for a walker of 5 or more (0x2A for the
               party), which BtlWalkPathFollow hands to SoundPlay unless it is
               0xFF. BtlEnemyStatsSet takes the enemy row's byte +0x14. */
            u8 walk_sound;
            /* +0x1F the sound of the slot's action of any kind but 2:
               BtlActingShowRun makes it g_BtlActionSound (attack_sound for
               kind 2). BtlEnemyStatsSet takes the enemy row's byte +0x18. */
            u8 action_sound;
            /* +0x20 the first item a won battle may give for this enemy:
               BtlWinItemsGive gives it (numbers 1 to 0xA4 only) when
               BtlWinItemRateCheck(item, win_rate_1) answers 1, and otherwise
               tries win_item_2. BtlEnemyStatsSet takes the enemy row's byte
               +0x1E. */
            u8 win_item_1;
            /* +0x21 the rate BtlWinItemRateCheck weighs win_item_1 by.
               BtlEnemyStatsSet takes the enemy row's byte +0x1F. */
            u8 win_rate_1;
            /* +0x22 the item tried when win_item_1 is not given (no test of
               its range). BtlEnemyStatsSet takes the enemy row's byte +0x20. */
            u8 win_item_2;
            /* +0x23 the rate for win_item_2. BtlEnemyStatsSet takes the enemy
               row's byte +0x21. */
            u8 win_rate_2;
            /* +0x24 the experience a won battle gives for this enemy:
               BtlWinRewardSum adds the enemies' into g_BtlWinExperience (held
               at 99,999), which BtlWinRun sets against each member's character
               record. BtlEnemyStatsSet takes the enemy row's word +0x1A. */
            u16 experience;
            /* +0x26 the money a won battle gives for this enemy:
               BtlWinRewardSum adds the enemies' into g_BtlWinMoney and the
               game's money. BtlEnemyStatsSet takes the enemy row's word +0x1C. */
            u16 money;
            /* +0x28 bits of what doubles an amount on this slot:
               BtlHitAmountSet doubles a party slot's hit whose element bit is
               set here (number kind 3), and BtlTargetTraitCheck answers 2 for
               a mask bit set here (BtlTargetHpAmountTake doubles;
               BtlStatusTakeCheck lets the status take). BtlEnemyStatsSet takes
               the enemy row's word +0x22. */
            u16 weak_mask;
            /* +0x2A bits of what halves an amount on this slot or keeps a
               status off: BtlHitAmountSet halves a hit whose element bit is
               set here (number kind 4); BtlTargetTraitCheck answers 1 for a
               mask bit set here (statuses 0x100 to 0x2000, elements 1 to
               0x40); BtlPartyOtherSideSpellSet tests bit 4. BtlEnemyStatsSet
               takes the enemy row's word +0x24. */
            u16 resist_mask;
            /* +0x2C..+0x2D nothing writes or reads them for an enemy
               (BtlActionAttackRun reads the party's traits[4] and [5] there
               for a slot below 5 only). */
            u8 pad_2c[2];
        } enemy;
    } side;
    /* +0x2E..+0x33 the six items the slot carries, each the number (from 1) of
       a 0x12-byte item row, 0 for none. BtlPartyStatsSet copies the character
       record's items; BtlWinItemsGive puts a won item in the first 0 of a
       party slot's; BtlActionItemRun takes a used item out and closes the gap
       (+0x33 set 0). BtlPartyStatsStore copies them back to the character
       record. Enemies' stay 0. */
    u8 items[6];
    /* +0x34..+0x3B the slot's eight spells, each the number (from 1) of a
       12-byte spell row, 0 for none. BtlPartyStatsSet copies the character
       record's spells; nothing else writes them (enemies' stay 0). */
    u8 spells[8];
} BtlStats;

LAYOUT_SIZE(BtlStats, 0x3c);
LAYOUT_AT(BtlStats, hp, hp, 0x00);
LAYOUT_AT(BtlStats, hp_max, hp_max, 0x02);
LAYOUT_AT(BtlStats, mp, mp, 0x04);
LAYOUT_AT(BtlStats, mp_max, mp_max, 0x06);
LAYOUT_AT(BtlStats, stat_08, stat_08, 0x08);
LAYOUT_AT(BtlStats, stat_0a, stat_0a, 0x0a);
LAYOUT_AT(BtlStats, stat_0c, stat_0c, 0x0c);
LAYOUT_AT(BtlStats, stat_0e, stat_0e, 0x0e);
LAYOUT_AT(BtlStats, stat_10, stat_10, 0x10);
LAYOUT_AT(BtlStats, attack_count, attack_count, 0x12);
LAYOUT_AT(BtlStats, walk_reach, walk_reach, 0x13);
LAYOUT_AT(BtlStats, level, level, 0x14);
LAYOUT_AT(BtlStats, status, status, 0x15);
LAYOUT_AT(BtlStats, least_amount, least_amount, 0x16);
LAYOUT_AT(BtlStats, attack_show, attack_show, 0x17);
LAYOUT_AT(BtlStats, walk_speed, walk_speed, 0x18);
LAYOUT_AT(BtlStats, attack_sound, attack_sound, 0x19);
LAYOUT_AT(BtlStats, side, side, 0x1a);
LAYOUT_AT(BtlStats, side_party, side.party, 0x1a);
LAYOUT_AT(BtlStats, side_party_stat_08_start, side.party.stat_08_start, 0x1a);
LAYOUT_AT(BtlStats, side_party_stat_0a_start, side.party.stat_0a_start, 0x1c);
LAYOUT_AT(BtlStats, side_party_flags, side.party.flags, 0x1e);
LAYOUT_AT(BtlStats, side_party_stat_1f, side.party.stat_1f, 0x1f);
LAYOUT_AT(BtlStats, side_party_worn_item_kind, side.party.worn_item_kind, 0x20);
LAYOUT_AT(BtlStats, side_party_pad_21, side.party.pad_21, 0x21);
LAYOUT_AT(BtlStats, side_party_worn_items, side.party.worn_items, 0x22);
LAYOUT_AT(BtlStats, side_party_traits, side.party.traits, 0x28);
LAYOUT_AT(BtlStats, side_enemy, side.enemy, 0x1a);
LAYOUT_AT(BtlStats, side_enemy_action_pick, side.enemy.action_pick, 0x1a);
LAYOUT_AT(BtlStats, side_enemy_trait, side.enemy.trait, 0x1b);
LAYOUT_AT(BtlStats, side_enemy_attack_number, side.enemy.attack_number, 0x1c);
LAYOUT_AT(BtlStats, side_enemy_kind7_number, side.enemy.kind7_number, 0x1d);
LAYOUT_AT(BtlStats, side_enemy_walk_sound, side.enemy.walk_sound, 0x1e);
LAYOUT_AT(BtlStats, side_enemy_action_sound, side.enemy.action_sound, 0x1f);
LAYOUT_AT(BtlStats, side_enemy_win_item_1, side.enemy.win_item_1, 0x20);
LAYOUT_AT(BtlStats, side_enemy_win_rate_1, side.enemy.win_rate_1, 0x21);
LAYOUT_AT(BtlStats, side_enemy_win_item_2, side.enemy.win_item_2, 0x22);
LAYOUT_AT(BtlStats, side_enemy_win_rate_2, side.enemy.win_rate_2, 0x23);
LAYOUT_AT(BtlStats, side_enemy_experience, side.enemy.experience, 0x24);
LAYOUT_AT(BtlStats, side_enemy_money, side.enemy.money, 0x26);
LAYOUT_AT(BtlStats, side_enemy_weak_mask, side.enemy.weak_mask, 0x28);
LAYOUT_AT(BtlStats, side_enemy_resist_mask, side.enemy.resist_mask, 0x2a);
LAYOUT_AT(BtlStats, side_enemy_pad_2c, side.enemy.pad_2c, 0x2c);
LAYOUT_AT(BtlStats, items, items, 0x2e);
LAYOUT_AT(BtlStats, spells, spells, 0x34);

/* One of the twelve show sound slots at g_BtlShowSounds (0x800E5588, 4 bytes
   each): a sound a show schedules on SoundPlay's sequence slot slot + 3,
   played at once, after a wait, or again every repeat frames. The glossary's
   show sound slot. */
typedef struct {
    /* +0x00 the slot's state: 0 off (BtlShowSoundsStep passes the slot over),
       1 playing (the count runs up to the repeat), above 1 frames still to
       wait, counted down one a show frame by BtlShowSoundsStep.
       BtlShowSoundSet stores its wait here first (a wait of 0 plays the sound
       at once); BtlShowSoundsStep sets 0 after a sound whose repeat is 0;
       BtlShowSoundStop sets 0; the shows store 0 to end a repeat (btl_show.c
       BtlShow01Run, BtlShow06Run, BtlShow08Run, BtlShow0BRun, BtlShow17Run,
       BtlShow1ARun, BtlShow1DRun, BtlShow2ARun, BtlShow2BRun, BtlShow2CRun;
       btl_boss.c BtlPartyRightMove, BtlPartyRightMovePatternsRun; slot 1's
       BtlEffectShotRun after the shot, slot 2's BtlEffectCopyTargetStandRun
       after the copy's animation, slots 1's and 2's BtlShow55Run and
       BtlShow56Run between their stages, slot 4's BtlShow54Run at the end of
       its show) and BtlEffectCopyAboveShotRun stores 1 in slots 0 to 2 to start
       them. A 0 ends the slot's repeat without stopping its sound. */
    u8 state;
    /* +0x01 the sound the slot plays, handed to SoundPlay(sound, slot + 3).
       BtlShowSoundSet stores it for a wait that is not 0; BtlShowSoundsStep
       and BtlShowSoundStop set 0; btl_weapon.c BtlShow3ERun and BtlShow41Run write
       slots 0 to 2's directly. */
    u8 sound;
    /* +0x02 frames played since the slot last sounded: at state 1
       BtlShowSoundsStep adds 1 each show frame and plays the sound when it is
       no longer below the repeat (both compared as signed bytes), then sets 0.
       BtlShowSoundSet stores the repeat here too; BtlShowSoundStop sets 0;
       BtlShow3ERun and BtlShow41Run zero it directly. */
    u8 count;
    /* +0x03 the frames between two plays of a repeating sound, 0 for one play:
       BtlShowSoundsStep compares the count with it and turns the slot off
       after a play when it is 0. Set by BtlShowSoundSet (its last argument),
       zeroed by BtlShowSoundStop; BtlShow2DRun writes slot 1's (0x10) and
       BtlShow3ERun, BtlShow41Run zero them directly. */
    u8 repeat;
} BtlShowSound;

LAYOUT_SIZE(BtlShowSound, 0x04);
LAYOUT_AT(BtlShowSound, state, state, 0x00);
LAYOUT_AT(BtlShowSound, sound, sound, 0x01);
LAYOUT_AT(BtlShowSound, count, count, 0x02);
LAYOUT_AT(BtlShowSound, repeat, repeat, 0x03);

/* One item of the battle menu, 0x20 of them in the 0x180 bytes whose address
   is in g_BtlMenuItems (BtlMenuInit takes them), g_BtlMenuItemCount in use: an
   icon on the text grid that the cursor moves over. The glossary's item (of
   the battle's menu). */
typedef struct {
    /* +0x00 1 for an item in use, 0 for an empty place: BtlMenuIconCellsSet
       sets the icon cells only of items whose byte is not 0, and
       BtlMenuCursorMove passes over cells whose byte is 0. Set 1 by the menu's
       opens (BtlMenuFirstOpen, BtlMenuKindOpen, BtlMenuSpellOpen,
       BtlMenuItemOpen, BtlMenuPresetOpen, BtlMenuTargetOpen, BtlMenuWalkOpen)
       and by BtlWindow1IconDisp (btl_window.c), BtlWinItemMessageDisp and
       BtlLevelUpGainDisp (btl_message.c); 0 by BtlMenuSpellOpen and
       BtlMenuItemOpen for an empty spell or item place; BtlMenuClear empties
       the items. */
    u8 on;
    /* +0x01 not 0 for an item that cannot be chosen: BtlMenuKindStep answers a
       pick of it with sound request 4 and nothing else (as do the spell and
       item steps), and BtlMenuSpellDisp writes its string in colour 4.
       BtlMenuKindOpen sets 1 for the spell kind when the member has no spell,
       cannot cast (status 0x80) or is id 0 or 0xC, for the item kind while
       story bit 0x3D9 is set, and for the four items of the second row;
       BtlMenuSpellOpen and BtlMenuItemOpen set 1 when BtlMenuActionCheck
       refuses the spell or item, 0 otherwise. The glossary's 'item's byte +1'. */
    u8 disabled;
    /* +0x02 the item's column on the text grid, in cells of 8 pixels:
       BtlMenuIconCellsSet puts its icon there (IconCellSet) and puts the
       cursor's mark record (g_BtlMenuMarkRecords + 0x80) at x * 8;
       BtlWindow1IconClose clears the 3 x 3 cells there. Written by the opens. */
    s16 x;
    /* +0x04 the item's row on the text grid, in cells; the mark record's y is
       y * 8. Written by the opens. */
    s16 y;
    /* +0x06 the item's icon number, which IconCellSet draws; the item's string
       is number + 0x100 (BtlMenuTextStringPut). A spell item's number + 0x4A
       is the spell (BtlMenuSpellDisp, BtlMenuSpellStep: 0xB6 + spell number
       when BtlMenuSpellOpen writes it); an item item's is the item's number
       (BtlMenuItemOpen). The kinds' icons are fixed numbers (0xE9 to 0xF5,
       BtlMenuKindOpen, BtlMenuFirstOpen, BtlMenuPresetOpen);
       BtlMenuPresetRowDisp copies a preset cell's icon into items 4 on;
       BtlWinItemMessageDisp and BtlLevelUpGainDisp (btl_message.c) write one item's. */
    u16 icon;
    /* +0x08 the icon cell's attribute word: 0x20200000 for an item shown
       bright, 0x200000 for one shown dim. BtlMenuIconCellsSet hands
       IconCellSet the word exclusive-or 0x20200000 (0 is bright; any other
       sets the cell's bit 7, drawn in 0x404040), and the item under the cursor
       always bright. Written by the opens (0 for an empty spell or item
       place). */
    s32 attr;
} BtlMenuItem;

LAYOUT_SIZE(BtlMenuItem, 0x0c);
LAYOUT_AT(BtlMenuItem, on, on, 0x00);
LAYOUT_AT(BtlMenuItem, disabled, disabled, 0x01);
LAYOUT_AT(BtlMenuItem, x, x, 0x02);
LAYOUT_AT(BtlMenuItem, y, y, 0x04);
LAYOUT_AT(BtlMenuItem, icon, icon, 0x06);
LAYOUT_AT(BtlMenuItem, attr, attr, 0x08);

/* One cell of the preset menu's block, the 0x78 bytes whose address is in
   g_BtlMenuPresetCells: four preset rows of five cells, cell = row * 5 +
   listed member. Rows 0 to 2 hold the members' three presets, row 3 kind 0 for
   everyone. The glossary's preset cell. */
typedef struct {
    /* +0x00 the icon number of the preset's action, as a BtlMenuItem's icon
       (its string is number + 0x100): 0xF0, 0xED, 0xE9, 0xEB, 0xEE for the
       fixed kinds, the spell's or item's own for kinds 3 and 4. Written only
       by BtlMenuPresetOpen; BtlMenuPresetRowDisp writes the cursor row's
       strings from it and copies it into the icon of menu item 4 + member. */
    u16 icon;
    /* +0x02 the colour of the cell's string: 4 when BtlMenuActionCheck refuses
       the preset's spell or item (the cell then has kind 5 and number 0xFF), 0
       otherwise. Written by BtlMenuPresetOpen; read by BtlMenuPresetRowDisp. */
    u16 colour;
    /* +0x04 the kind of the preset's action (0, 1, 2, 5, 6, 3 a spell, 4 an
       item; 5 also for a refused spell or item; row 3 holds 0):
       BtlMenuPresetStep and BtlMenuPresetRowActionSet put it in
       g_BtlSlotPlannedKinds. Written by BtlMenuPresetOpen. */
    u8 kind;
    /* +0x05 the action's number: the spell (icon + 0x4A), the item, 0 for the
       fixed kinds 0 and 1, 0xFF for none. BtlMenuPresetRowActionSet puts it in
       g_BtlSlotPlannedNumbers. Written by BtlMenuPresetOpen. */
    u8 number;
} BtlMenuPresetCell;

LAYOUT_SIZE(BtlMenuPresetCell, 0x06);
LAYOUT_AT(BtlMenuPresetCell, icon, icon, 0x00);
LAYOUT_AT(BtlMenuPresetCell, colour, colour, 0x02);
LAYOUT_AT(BtlMenuPresetCell, kind, kind, 0x04);
LAYOUT_AT(BtlMenuPresetCell, number, number, 0x05);

/* One row of the item table g_BtlItemTable (0x800A9A68, 0xB6 rows of 0x12
   bytes, never written): row number - 1 is item number's. The battle reaches
   it by name, through g_BtlItemRows and through g_BtlItemRowsByNumber + number
   * 0x12; the menus, the shop and the Chr functions through g_ChrItemRows,
   which func_80026AB0 points at the same table. The glossary's item row. */
typedef struct {
    /* +0x00 what the item costs: ShopItemDisp sets the item's flag 0x20 when
       it is more than the money, and the buy takes it off the money (shop_screen.c
       ShopDealRun). */
    u16 price;
    /* +0x02 what giving the item up adds to the money; 0: it cannot be given
       up (ChrItemUseGet mode 2; MenuItemDropStep and the shop's sell refuse
       it). */
    u16 sell_price;
    /* +0x04 the item's kind: below 5 it is worn, in worn place kind + 1 (the
       byte +0x1E + kind of the character record; kind 4 has places 5 and 6); 5
       and up it is carried (6: it cannot be given up). ShopItemDisp,
       ShopItemLineUpSet and the shop's deal, ChrItemListSort and
       ChrItemListPartSort (D_80012864[kind] orders the list), the worn-item
       functions of menu_page.c and BtlWornItemRemove's caller read it. The
       shop's and the Chr functions' word; the glossary's place for it is 'byte
       +4'. */
    u8 kind;
    /* +0x05 a story bit the worn item holds: not 0, ChrWornItemValuesAdd sets
       story bit + 0x2BC when the item is put on and ChrWornItemValuesRemove
       clears it when it comes off. */
    u8 story_bit;
    /* +0x06 two target modes. The low four bits the battle's:
       BtlMenuActionCheck makes g_BtlMenuTargetMode of them (and the mask of
       slots it may be aimed at), BtlActionStatus40Adjust draws a target when
       they are not 0, BtlActionTargetAdjust hands the byte to
       BtlTargetByModeGet or BtlTargetModeAdjust; ChrItemUseGet mode 4 answers
       them for the command set-up page (9: none). The high four bits the field
       menu's (ChrItemUseGet mode 3, MenuItemUseStep: 9 not usable, below 5 no
       target, 5 and up a target to pick). */
    u8 target_modes;
    /* +0x07 two numbers for the field menu's use. The low four bits the entry
       of battle_state_table that MenuUseRun calls for the item; the high four
       bits the picture MenuUsePictureRun plays (movie (bits >> 4) + 0x37). */
    u8 use_picture;
    /* +0x08 the function number of the item used as an action
       (g_BtlActionFunctionNumber: entry number - 1 of scene_state_table,
       BtlActionRowRun); 0 for an item a battle cannot use, which
       BtlMenuActionCheck refuses (the first split's 'usable'). */
    u8 function_number;
    /* +0x09 the show number of the item used as an action: BtlActionRowRun
       puts it in g_BtlActionShowNumber. */
    u8 show_number;
    /* +0x0A a union by what the row's item is. bonus_1: ChrWornItemValuesAdd
       and ChrWornItemValuesRemove add it to and take it off the stat the low
       four bits of +0xC select (ChrWornItemStatAdd), BtlWornItemRemove takes
       it off (BtlWornItemStatLower), ChrWornItemChangeMake shows it. power:
       for an item of number 0x9C or more BtlActionRowRun puts it in
       g_BtlActionPower. */
    union {
        /* bonus_1: used by BtlWornItemRemove, ChrWornItemChangeMake,
           ChrWornItemValuesAdd, ChrWornItemValuesRemove, c, game_state, items,
           scripting, the, worn */
        u8 bonus_1;
        /* power: used by BtlActionRowRun, actions, and, as, c, game_misc_b,
           items, numbers, the, up, used */
        u8 power;
    } bonus_1_power;
    /* +0x0B a worn item's bonus to the stat in the high four bits of
       bonus_stats (ChrWornItemValuesAdd/Remove, BtlWornItemRemove,
       ChrWornItemChangeMake). BtlPartyStatsSet also takes the first worn
       item's as the party slot's least_amount. */
    u8 bonus_2;
    /* +0x0C the two stats a worn item raises, as the selectors of
       ChrWornItemStatAdd and BtlWornItemStatLower (1 stat_08 to 8 walk_reach):
       the low four bits bonus_1's, the high four bits bonus_2's; 0 none. */
    u8 bonus_stats;
    /* +0x0D the trait a worn item gives: BtlPartyStatsSet copies each worn
       item's into the party slot's side.party.traits. */
    u8 trait;
    /* +0x0E BtlPartyStatsSet copies the first worn item's into the party
       slot's attack_show, which nothing reads for a party slot. */
    u8 attack_show;
    /* +0x0F the kind of a worn item of kind 0 (0 to 5; 0 for every other row):
       BtlPartyStatsSet copies the first worn item's into
       side.party.worn_item_kind and picks the slot's attack, stand and move
       animations and attack_sound by it. */
    u8 worn_item_kind;
    /* +0x10 who may wear the item: bit n - 1 for each party number n
       (ChrItemUseGet mode 1, ShopItemDisp, ShopItemLineUpSet, chr_item.c); the
       callers that pass who = 0x10 read bit 15 this way (ChrItemGive's keep,
       MenuPoseWordsSet). */
    u16 wearers;
} BtlItemRow;

LAYOUT_SIZE(BtlItemRow, 0x12);
LAYOUT_AT(BtlItemRow, price, price, 0x00);
LAYOUT_AT(BtlItemRow, sell_price, sell_price, 0x02);
LAYOUT_AT(BtlItemRow, kind, kind, 0x04);
LAYOUT_AT(BtlItemRow, story_bit, story_bit, 0x05);
LAYOUT_AT(BtlItemRow, target_modes, target_modes, 0x06);
LAYOUT_AT(BtlItemRow, use_picture, use_picture, 0x07);
LAYOUT_AT(BtlItemRow, function_number, function_number, 0x08);
LAYOUT_AT(BtlItemRow, show_number, show_number, 0x09);
LAYOUT_AT(BtlItemRow, bonus_1_power, bonus_1_power, 0x0a);
LAYOUT_AT(BtlItemRow, bonus_1_power_bonus_1, bonus_1_power.bonus_1, 0x0a);
LAYOUT_AT(BtlItemRow, bonus_1_power_power, bonus_1_power.power, 0x0a);
LAYOUT_AT(BtlItemRow, bonus_2, bonus_2, 0x0b);
LAYOUT_AT(BtlItemRow, bonus_stats, bonus_stats, 0x0c);
LAYOUT_AT(BtlItemRow, trait, trait, 0x0d);
LAYOUT_AT(BtlItemRow, attack_show, attack_show, 0x0e);
LAYOUT_AT(BtlItemRow, worn_item_kind, worn_item_kind, 0x0f);
LAYOUT_AT(BtlItemRow, wearers, wearers, 0x10);

/* One row of the spell table g_BtlSpellRows (0x800AA734, 0x32 rows of 12
   bytes, never written): row number - 1 is spell number's. The battle reaches
   it by name and through g_BtlSpellRowsByNumber + number * 12; the menus and
   the Chr functions through g_ChrSpellRows, which func_80026AB0 points at it.
   The glossary's spell row. */
typedef struct {
    /* +0x00 the level the spell is learned at: ChrLevelUp learns the rows of
       its id whose level is the new one; BtlActingSpellPick takes a spell only
       when the slot's level is not below it. */
    u8 level;
    /* +0x01 two target modes, as the item row's +6. The low four bits the
       battle's (BtlMenuActionCheck's g_BtlMenuTargetMode;
       BtlActionStatus40Adjust draws a target when they are not 0;
       ChrSpellUseGet mode 4 for the command set-up page, 9 none); the high
       four bits the field menu's (ChrSpellUseGet modes 2 and 3). */
    u8 target_modes;
    /* +0x02 the spell's function number: BtlActionRowRun puts it in
       g_BtlActionFunctionNumber. */
    u8 function_number;
    /* +0x03 two numbers. The low four bits the entry of battle_state_table
       MenuUseRun calls for the spell outside a battle; the high four bits the
       place among the character's eight spells that ScriptCommandChrSpellSet
       writes the spell into. */
    u8 use_place;
    /* +0x04 the mp the spell costs, halved and rounded up for a slot with
       trait 0x1B (in a battle) or with worn item 0x88 in place 5 or 6 (outside
       one): BtlActionSpellRun takes it off; BtlMenuActionCheck,
       BtlMenuSpellDisp, BtlActingSpellPick, BtlActingSpellListMake,
       BtlActionTargetAdjust, ChrSpellUseGet and the field menu compare it with
       the mp. */
    u8 cost;
    /* +0x05 the spell's power: BtlActionRowRun puts it in g_BtlActionPower. */
    u8 power;
    /* +0x06 the spell's show number: BtlActionRowRun puts it in
       g_BtlActionShowNumber. */
    u8 show_number;
    /* +0x07 the id of the character the spell belongs to (1 for id 0xB too):
       ChrLevelUp walks the table for the rows of its id;
       ScriptCommandChrSpellSet stops on an error when the spell asked for is
       not the character's. */
    u8 chr_id;
    /* +0x08 BtlActionRowRun puts it in g_BtlActionBitNumber. */
    u8 bit_number;
    /* +0x09 half the width of the spell's area, in cells of 8 pixels:
       BtlActionRowRun and BtlMenuTargetOpen put it in g_BtlAreaHalfWidth;
       btl_choice.c's area finds take x within area_half_width * 8 of the acting
       record. */
    u8 area_half_width;
    /* +0x0A half the height of the spell's area, in cells:
       g_BtlAreaHalfHeight, as area_half_width. */
    u8 area_half_height;
    /* +0x0B not 0 for a spell that cannot be cast while the caster's status
       has bit 0x20: BtlMenuActionCheck refuses it, BtlActingSpellUsableGet
       (through BtlSpellByte0BGet) passes it over for a status of exactly 0x20,
       BtlActionTargetAdjust replaces it. The name btl_command.c's view gives it. */
    u8 blocked_by_status_20;
} BtlSpellRow;

LAYOUT_SIZE(BtlSpellRow, 0x0c);
LAYOUT_AT(BtlSpellRow, level, level, 0x00);
LAYOUT_AT(BtlSpellRow, target_modes, target_modes, 0x01);
LAYOUT_AT(BtlSpellRow, function_number, function_number, 0x02);
LAYOUT_AT(BtlSpellRow, use_place, use_place, 0x03);
LAYOUT_AT(BtlSpellRow, cost, cost, 0x04);
LAYOUT_AT(BtlSpellRow, power, power, 0x05);
LAYOUT_AT(BtlSpellRow, show_number, show_number, 0x06);
LAYOUT_AT(BtlSpellRow, chr_id, chr_id, 0x07);
LAYOUT_AT(BtlSpellRow, bit_number, bit_number, 0x08);
LAYOUT_AT(BtlSpellRow, area_half_width, area_half_width, 0x09);
LAYOUT_AT(BtlSpellRow, area_half_height, area_half_height, 0x0a);
LAYOUT_AT(BtlSpellRow, blocked_by_status_20, blocked_by_status_20, 0x0b);

/* The walk block g_BtlWalkBlock (0x800E4B10, 0x11 bytes of data): the walk of
   one record to another that BtlWalkSet fills for the acting slot and its
   target, the shows change, and BtlWalkRun measures and, when asked, walks.
   The glossary's walk block. */
typedef struct {
    /* +0x00 the address of the walker's record: BtlWalkSet puts g_BtlRecords +
       acting slot * 0x40; BtlWalkRun copies it to g_BtlFromRecord. */
    s32 walker;
    /* +0x04 the address of the record walked to: BtlWalkSet puts the target
       slot's; BtlShowA3Run, BtlSlot5Revive and BtlActionWalkRun point it at
       records of their own (an x at +6, a y at +0xA and a width at +0x3C) or
       at each party record in turn, and BtlSlot5ReviveTry at one on its stack
       (x from slot 5's record, y 0x60, height and width 8) before BtlWalkRun.
       BtlWalkRun copies it to g_BtlToRecord. */
    s32 target;
    /* +0x08 the walker's slot: BtlWalkRun halves the reach for a party slot
       (below 5) with g_BtlActingStatus bit 0x10; BtlWalkPathFollow takes the
       slot's id and rest animation from it. Written by BtlWalkSet. */
    u8 walker_slot;
    /* +0x09 not 0: the other slots' records block cells of the walk grid
       (BtlWalkGridInit marks their cells 0xFE only then). BtlWalkSet sets 1;
       BtlShowA3Run 0, and BtlWalkEdgeRun 0 for battles 0xFE and 0xF7 after
       BtlWalkSet. */
    u8 slots_block;
    /* +0x0A not 0: the walker's width counts (BtlWalkRun takes its width in
       cells, record +0x3C / 8 made even, into g_BtlWalkerWidth and moves its
       column left by half of it; BtlWalkCellBlockedCheck then also refuses a
       cell from which any of the walker's cells would lie on a blocked one;
       BtlWalkPathMake and BtlWalkDistanceFill read it too); 0 a width of 1.
       BtlWalkSet and BtlShowA1Run set 1; BtlShowA2Run and BtlShowA3Run 0. */
    u8 walker_width_counts;
    /* +0x0B not 0: the target's width counts the same way, and BtlWalkRun
       answers 2 when the two stand side by side (BtlWalkBesideCheck); 0 a
       width of 1. BtlWalkSet sets 1; BtlShowA3Run 0. */
    u8 target_width_counts;
    /* +0x0C not 0: BtlWalkRun walks the walker; 0: it only measures whether
       the target is within reach. BtlWalkSet sets 1; BtlTargetStatus8TryRun
       and BtlTargetReachCheck call with 0 first and then 1; BtlShowA3Run sets
       4; BtlAttackWalkRun sets 0, when the acting status has bit 0x20, before
       it asks BtlWalkRun. */
    u8 walk;
    /* +0x0D the animation the walker walks with (BtlWalkPathFollow:
       BtlAnimationSet): BtlWalkSet puts g_BtlPartyMoveAnimations[slot] for a
       party slot, 1 for any other; BtlShowA3Run 8; BtlTargetReachCheck 0xB
       for enemy ids 0x59, 0x2A, 0x39 and 0x48 before the walk. */
    u8 animation;
    /* +0x0E the walk's speed: BtlWalkPathFollow takes 1, 2 or 4 frames a cell
       for 1, 2 or 3 and 8 for any other. BtlWalkSet copies the walker's stats'
       walk_speed; BtlShowA3Run sets 1; BtlShow99Run 3 for two cases of its
       switch (ids 0x34 and 0x63). */
    u8 speed;
    /* +0x0F the most cells the walk may cover: BtlWalkRun halves it for a
       party walker with status bit 0x10 and holds it at 0x28, and measures the
       goal's distance against it. BtlWalkSet copies the walker's stats'
       walk_reach; BtlShowA3Run sets 0x64. The symbol g_BtlWalkBlockReach
       (0x800E4B1F) names it too: BtlWalkDistanceFill (btl_path.c) starts
       g_BtlWalkNodeCount from it by that name, the count of rings left in a
       pass (read as this member the load's lui moves into a delay slot and
       the bytes change). */
    u8 reach;
    /* +0x10 the walk's sound: BtlWalkPathFollow plays it (SoundPlay(sound, 3))
       at the start and stops it at the end, unless it is 0xFF. BtlWalkSet puts
       0x2A for a party walker and the stats' side.enemy.walk_sound for any
       other; BtlShowA3Run puts g_BtlActionSound. */
    u8 sound;
    /* +0x11..+0x13 the struct's tail padding (the data is 0x11 bytes;
       g_BtlShotBlock begins at 0x800E4B28); nothing reads or writes it. */
    u8 pad_11[3];
} BtlWalkBlock;

LAYOUT_SIZE(BtlWalkBlock, 0x14);
LAYOUT_AT(BtlWalkBlock, walker, walker, 0x00);
LAYOUT_AT(BtlWalkBlock, target, target, 0x04);
LAYOUT_AT(BtlWalkBlock, walker_slot, walker_slot, 0x08);
LAYOUT_AT(BtlWalkBlock, slots_block, slots_block, 0x09);
LAYOUT_AT(BtlWalkBlock, walker_width_counts, walker_width_counts, 0x0a);
LAYOUT_AT(BtlWalkBlock, target_width_counts, target_width_counts, 0x0b);
LAYOUT_AT(BtlWalkBlock, walk, walk, 0x0c);
LAYOUT_AT(BtlWalkBlock, animation, animation, 0x0d);
LAYOUT_AT(BtlWalkBlock, speed, speed, 0x0e);
LAYOUT_AT(BtlWalkBlock, reach, reach, 0x0f);
LAYOUT_AT(BtlWalkBlock, sound, sound, 0x10);
LAYOUT_AT(BtlWalkBlock, pad_11, pad_11, 0x11);

/* The shot block g_BtlShotBlock (0x800E4B28): a shot that flies from one
   record to another as a copy of a third, which BtlShotSet fills with its
   defaults, the shows change, and BtlShotRun runs. The glossary's shot block. */
typedef struct {
    /* +0x00 the animation of the record the shot leaves from, played before
       the shot; 0xFF none (and no from_sound and no wait). BtlShotSet sets 8;
       BtlShowACRun, BtlShowADRun, BtlShowAFRun, BtlShowB0Run, BtlShowB7Run,
       BtlShow9BRun set their own. */
    u8 from_animation;
    /* +0x01 the wait after that animation: 0x63 or more until its last step
       (BtlAnimationEndWait), else that many frames (BtlFrameWait), 0 none.
       BtlShotSet sets 0x63. */
    u8 from_wait;
    /* +0x02 the animation of the shot's record. BtlShotSet sets 9. */
    u8 animation;
    /* +0x03 the flight's length in frames; 0: worked out from the distance and
       speed. BtlShotSet sets 0. */
    u8 length;
    /* +0x04 the pixels a frame for a length worked out from the distance (the
       larger of the two distances divided by it, at least 1); BtlShotRun
       writes 8 back when both length and speed are 0. BtlShotSet sets 8. */
    u8 speed;
    /* +0x05 a sound played before the from record's animation (SoundPlay(n,
       3)); 0 none. BtlShotSet sets 0. */
    u8 from_sound;
    /* +0x06 a sound played as the shot leaves; 0 none. BtlShotSet sets 0x4B. */
    u8 launch_sound;
    /* +0x07 a sound played when the shot arrives; 0 none. BtlShotSet sets 0. */
    u8 arrive_sound;
    /* +0x08 where the shot starts across: pixels beyond the edge of the from
       record (half its width) on the side it faces. BtlShotSet sets 0. */
    s16 dx;
    /* +0x0A where the shot starts down: added to the from record's y.
       BtlShotSet sets 0. */
    s16 dy;
    /* +0x0C the address of the record the shot leaves from (g_BtlFromRecord).
       BtlShotSet puts the acting slot's. */
    s32 from;
    /* +0x10 the address of the record the shot flies to (g_BtlToRecord).
       BtlShotSet puts the target slot's. */
    s32 to;
    /* +0x14 the address of the record whose copy flies (copied whole to the
       stack, then shown as g_BtlExtraRecords). BtlShotSet puts the acting
       slot's. */
    s32 copied;
} BtlShotBlock;

LAYOUT_SIZE(BtlShotBlock, 0x18);
LAYOUT_AT(BtlShotBlock, from_animation, from_animation, 0x00);
LAYOUT_AT(BtlShotBlock, from_wait, from_wait, 0x01);
LAYOUT_AT(BtlShotBlock, animation, animation, 0x02);
LAYOUT_AT(BtlShotBlock, length, length, 0x03);
LAYOUT_AT(BtlShotBlock, speed, speed, 0x04);
LAYOUT_AT(BtlShotBlock, from_sound, from_sound, 0x05);
LAYOUT_AT(BtlShotBlock, launch_sound, launch_sound, 0x06);
LAYOUT_AT(BtlShotBlock, arrive_sound, arrive_sound, 0x07);
LAYOUT_AT(BtlShotBlock, dx, dx, 0x08);
LAYOUT_AT(BtlShotBlock, dy, dy, 0x0a);
LAYOUT_AT(BtlShotBlock, from, from, 0x0c);
LAYOUT_AT(BtlShotBlock, to, to, 0x10);
LAYOUT_AT(BtlShotBlock, copied, copied, 0x14);

/* A travel block on a show's stack: a record that travels to beside another
   along a path BtlTravelPathsMake makes; BtlTravelRun runs it, BtlShow17Run
   follows the paths itself. The glossary's travel (BtlTravelPathsMake's row:
   'the travel block'). */
typedef struct {
    /* +0x00 0: the path runs between the two records' facing edges; not 0: the
       traveller goes to stand beside the other record on its own side (the
       other side near the screen's edges) and the two are turned toward each
       other. Set by the callers (BtlShow01Run, BtlShow20Run 1;
       BtlEffectShotRun, BtlShowRecordTravelRun, BtlEffectCopyAboveShotRun,
       BtlShow17Run 0). */
    u8 mode;
    /* +0x01 the pixels a point: the number of points is the two distances' sum
       divided by it, at least 1. Callers set 8, 0x10 or 2. */
    u8 step;
    /* +0x02..+0x03 nothing reads or writes them. */
    u8 pad_02[2];
    /* +0x04 the address of the record that travels (the effect's record, a
       show record or the acting slot's); BtlTravelRun puts it on the animation
       and moves it. */
    s32 from;
    /* +0x08 an x beside the traveller: its side's reach (half its width, or
       0), added toward the other record. */
    s16 from_dx;
    /* +0x0A a y beside the traveller, added to its y for the path's start (0,
       or minus its height or half of it). */
    s16 from_dy;
    /* +0x0C the address of the record travelled to (the target's record). */
    s32 to;
    /* +0x10 an x beside the other record: half or a quarter of its width, or
       0. */
    s16 to_dx;
    /* +0x12 a y beside the other record, added to its y for the path's end. */
    s16 to_dy;
    /* +0x14 the address of the first path, n PathPoints from (x0, y0) to (x1,
       y1): BtlTravelPathsMake allocates and fills it; BtlTravelRun puts the
       traveller's x and y on each point and frees it; BtlShow17Run does the
       same for its records. */
    s32 path;
    /* +0x18 the address of the second path, n points from the traveller's own
       x and y to the other record's: the traveller's depth is its point's y +
       1. */
    s32 depth_path;
    /* +0x1C nothing reads or writes it (PanReq's path_n); BtlTravelPathsMake
       returns the number of points instead. */
    s32 pad_1c;
} BtlTravelBlock;

LAYOUT_SIZE(BtlTravelBlock, 0x20);
LAYOUT_AT(BtlTravelBlock, mode, mode, 0x00);
LAYOUT_AT(BtlTravelBlock, step, step, 0x01);
LAYOUT_AT(BtlTravelBlock, pad_02, pad_02, 0x02);
LAYOUT_AT(BtlTravelBlock, from, from, 0x04);
LAYOUT_AT(BtlTravelBlock, from_dx, from_dx, 0x08);
LAYOUT_AT(BtlTravelBlock, from_dy, from_dy, 0x0a);
LAYOUT_AT(BtlTravelBlock, to, to, 0x0c);
LAYOUT_AT(BtlTravelBlock, to_dx, to_dx, 0x10);
LAYOUT_AT(BtlTravelBlock, to_dy, to_dy, 0x12);
LAYOUT_AT(BtlTravelBlock, path, path, 0x14);
LAYOUT_AT(BtlTravelBlock, depth_path, depth_path, 0x18);
LAYOUT_AT(BtlTravelBlock, pad_1c, pad_1c, 0x1c);

/* The shows' colour pulse g_BtlShowPulse (0x800E5530): while it is on,
   BtlShowPulseStep fades the layers of mask 0xC from one colour to the other
   and back. The glossary's colour pulse. */
typedef struct {
    /* +0x00 1 while the pulse runs (BtlShowPulseStep starts a fade only while
       it is not 0). The shows store 1 (BtlShow09Run, BtlShow0ARun,
       BtlShow0BRun, BtlShow19Run, BtlShow2CRun, BtlShow54Run,
       BtlPartyRightMovePatternsRun, BtlShow50Run and others); BtlShowPulseSet,
       BtlShowPulseClear and BtlShowOpen set 0. */
    s32 on;
    /* +0x04 which colour the next fade goes to: not 0 colour_1, 0 colour_2;
       BtlShowPulseStep flips it at each fade it starts, BtlShowPulseSet sets
       0. */
    u8 next;
    /* +0x05..+0x07 nothing reads or writes them. */
    u8 pad_05[3];
    /* +0x08 the frames of each fade, given to layer_col_fade_start;
       BtlShowPulseSet sets it. */
    s32 length;
    /* +0x0C..+0x11 the first colour, three signed numbers (BtlShowPulseSet's
       second to fourth arguments); the shows also hand its address to
       layer_col_fade_start for a first fade (btl_show.c BtlShow2CRun, btl_boss.c
       BtlShow54Run), and btl_boss.c's BtlPartyRightMovePatternsRun and
       BtlShow50Run, which fade the layers of mask 0xC to it over 0x10 frames
       (layer_col_fade_start(0, 0xC, 1, 0x10, colour_1)) and then switch the
       pulse on. */
    s16 colour_1[3];
    /* +0x12..+0x17 the second colour (BtlShowPulseSet's last three arguments). */
    s16 colour_2[3];
} BtlShowPulse;

LAYOUT_SIZE(BtlShowPulse, 0x18);
LAYOUT_AT(BtlShowPulse, on, on, 0x00);
LAYOUT_AT(BtlShowPulse, next, next, 0x04);
LAYOUT_AT(BtlShowPulse, pad_05, pad_05, 0x05);
LAYOUT_AT(BtlShowPulse, length, length, 0x08);
LAYOUT_AT(BtlShowPulse, colour_1, colour_1, 0x0c);
LAYOUT_AT(BtlShowPulse, colour_2, colour_2, 0x12);

/* One of the two battle windows' rectangle records at g_BtlWindowRectangles
   (0x800E4B60, 14 bytes each): the size a window is to take, which
   BtlWindowSet or BtlWindow0Set writes and BtlWindowMoveStart reads to start
   the window's move. */
typedef struct {
    /* +0x00 the mode BtlWindowMoveStart copies to the move's mode: 2 from both
       setters (box and outline); BtlWindowsInit sets 0. */
    u8 mode;
    /* +0x01 copied to the move's moving byte by BtlWindowMoveStart; 1 from
       both setters, 0 from BtlWindowsInit. */
    u8 open;
    /* +0x02 the window's left edge in pixels (the setters' cells << 3). */
    u16 x;
    /* +0x04 the window's top edge in pixels. */
    u16 y;
    /* +0x06 the width the window had: the setters copy width here before they
       store the new one; the move starts from it. */
    u16 width_before;
    /* +0x08 the height the window had (q[4] = q[6]). */
    u16 height_before;
    /* +0x0A the new width in pixels; 0 closes the window. */
    u16 width;
    /* +0x0C the new height in pixels. */
    u16 height;
} BtlWindowRectangle;

LAYOUT_SIZE(BtlWindowRectangle, 0x0e);
LAYOUT_AT(BtlWindowRectangle, mode, mode, 0x00);
LAYOUT_AT(BtlWindowRectangle, open, open, 0x01);
LAYOUT_AT(BtlWindowRectangle, x, x, 0x02);
LAYOUT_AT(BtlWindowRectangle, y, y, 0x04);
LAYOUT_AT(BtlWindowRectangle, width_before, width_before, 0x06);
LAYOUT_AT(BtlWindowRectangle, height_before, height_before, 0x08);
LAYOUT_AT(BtlWindowRectangle, width, width, 0x0a);
LAYOUT_AT(BtlWindowRectangle, height, height, 0x0c);

/* One of the two battle windows' moves at g_BtlWindowMoves (0x800E4BC8, 0x1C
   bytes each): the rectangle the window is drawn at, the rectangle it goes to
   and two speeds, which BtlWindowsMoveStep steps every frame of the menu and
   of a show. The glossary's move (of a window). */
typedef struct {
    /* +0x00 the window's mode, from the rectangle record (BtlWindowMoveStart):
       BtlWindowSpritesSet draws the box when it is 2 (and, for window 0 at 1,
       box 1 in the outline's entry); BtlWindowsMoveStep steps the window only
       while it and moving are not 0. BtlWindowsInit sets 0. */
    u8 mode;
    /* +0x01 not 0 while the window moves: BtlWindowMoveStart leaves a moving
       window alone and sets it from the rectangle's open byte;
       BtlWindowsMoveStep sets 0 at the end; BtlWindowsMoveWait (btl_command.c) and
       BtlWindow1IconClose wait for it to be 0. */
    u8 moving;
    /* +0x02 the move's steps so far: BtlWindowsMoveStep adds 1 each frame and
       ends the move at 8 or when both speeds are 0; BtlWindowMoveStart sets 0. */
    s16 step;
    /* +0x04 the left edge now, in pixels (the rectangle's x); put on left_to
       when the right edge arrives. */
    s16 left;
    /* +0x06 the top edge now (the rectangle's y). */
    s16 top;
    /* +0x08 the right edge at the move's start (x + width_before): the edge
       drawn is right + speed_x * step >> 8. */
    s16 right;
    /* +0x0A the bottom edge at the move's start (y + height_before). */
    s16 bottom;
    /* +0x0C the left edge to reach (x). */
    s16 left_to;
    /* +0x0E the top edge to reach (y). */
    s16 top_to;
    /* +0x10 the right edge to reach (x + width). */
    s16 right_to;
    /* +0x12 the bottom edge to reach (y + height). */
    s16 bottom_to;
    /* +0x14 the right edge's speed, 1/256 pixel a step: (width - width_before)
       << 5, the change over eight steps; set 0 when the edge has arrived. */
    s32 speed_x;
    /* +0x18 the bottom edge's speed, (height - height_before) << 5. */
    s32 speed_y;
} BtlWindowMove;

LAYOUT_SIZE(BtlWindowMove, 0x1c);
LAYOUT_AT(BtlWindowMove, mode, mode, 0x00);
LAYOUT_AT(BtlWindowMove, moving, moving, 0x01);
LAYOUT_AT(BtlWindowMove, step, step, 0x02);
LAYOUT_AT(BtlWindowMove, left, left, 0x04);
LAYOUT_AT(BtlWindowMove, top, top, 0x06);
LAYOUT_AT(BtlWindowMove, right, right, 0x08);
LAYOUT_AT(BtlWindowMove, bottom, bottom, 0x0a);
LAYOUT_AT(BtlWindowMove, left_to, left_to, 0x0c);
LAYOUT_AT(BtlWindowMove, top_to, top_to, 0x0e);
LAYOUT_AT(BtlWindowMove, right_to, right_to, 0x10);
LAYOUT_AT(BtlWindowMove, bottom_to, bottom_to, 0x12);
LAYOUT_AT(BtlWindowMove, speed_x, speed_x, 0x14);
LAYOUT_AT(BtlWindowMove, speed_y, speed_y, 0x18);

/* One listed member's record of the party's panel, five of them on
   BtlPartyPanelDisp's stack: what BtlPartyPanelMemberDisp and
   BtlPartyPanelStatusDisp draw in the member's column. */
typedef struct {
    /* +0x00 BtlPartyPanelDisp sets 1 for each member it lists; nothing reads
       it. */
    u8 on;
    /* +0x01 bit 0x04 of the member's side.party.flags (the flag that goes with
       status 8): not 0 shows glyph 0x3C in place of the status and keeps the
       name's colour from 0x3000. */
    u8 flags_04;
    /* +0x02 the member's status byte (BtlStats.status):
       BtlPartyPanelStatusDisp picks the status glyph by it (0 the level), and
       a status of 0 with flags_04 0 gives the name colour 0x3000. */
    u8 status;
    /* +0x03 the member's level (BtlStats.level): BtlPartyPanelStatusDisp
       writes it for status 0, and at 0xFF it and the mp are written as glyphs
       0x5C (BtlPartyPanelNumberDisp's byte). */
    u8 level;
    /* +0x04 the member's hp (BtlStats +0): written on row 0x19, coloured by
       its share of hp_max; 0 shows glyph 0x58 and colour 0. */
    u16 hp;
    /* +0x06 the member's hp_max, against which hp is coloured. */
    u16 hp_max;
    /* +0x08 the member's mp, written on row 0x1A, coloured by its share of
       mp_max. */
    u16 mp;
    /* +0x0A the member's mp_max. */
    u16 mp_max;
} BtlPartyPanelMember;

LAYOUT_SIZE(BtlPartyPanelMember, 0x0c);
LAYOUT_AT(BtlPartyPanelMember, on, on, 0x00);
LAYOUT_AT(BtlPartyPanelMember, flags_04, flags_04, 0x01);
LAYOUT_AT(BtlPartyPanelMember, status, status, 0x02);
LAYOUT_AT(BtlPartyPanelMember, level, level, 0x03);
LAYOUT_AT(BtlPartyPanelMember, hp, hp, 0x04);
LAYOUT_AT(BtlPartyPanelMember, hp_max, hp_max, 0x06);
LAYOUT_AT(BtlPartyPanelMember, mp, mp, 0x08);
LAYOUT_AT(BtlPartyPanelMember, mp_max, mp_max, 0x0a);

/* One pair of the list pair_list_sort sorts in place: a key and a second
   number that breaks ties. Its one caller, BtlShow03Run, sorts the marked
   slots' near edges with it (14 on its stack). */
typedef struct {
    /* +0x00 the number the list is sorted by, from the largest with direction
       1, from the smallest otherwise. */
    s16 key;
    /* +0x02 the number that orders pairs with equal keys, from the smallest in
       both directions. */
    s16 second;
} SortPair;

LAYOUT_SIZE(SortPair, 0x04);
LAYOUT_AT(SortPair, key, key, 0x00);
LAYOUT_AT(SortPair, second, second, 0x02);

/* The battle's mix flash, g_BtlMixFlash (0x800E5500): BtlMixFlashSet starts or
   stops it, BtlMixFlashStep runs it once a battle frame (by the mode: the mix
   level of the mask's layers, the sprites' mix colour, or a shake of the hit
   slots' x); BtlAmountShowCheck and BtlEnemyFallShow (btl_show_frame.c) and
   BtlMenuInit (btl_menu.c) reach it as a byte array. */
typedef struct {
    /* +0x00 the mode, 0 off: BtlMixFlashSet writes it; BtlAmountShowCheck,
       BtlEnemyFallShow and BtlMenuInit write 0. 1 and 3 the layers' mix level
       by turns, 2 the hit slots' shake, 5 the sprites' mix colour. */
    u8 mode;
    /* +0x01 BtlMixFlashSet writes 1, BtlMenuInit 0; BtlMixFlashStep tests it. */
    u8 on;
    /* +0x02 two bytes nothing reads or writes. */
    u8 pad_02[2];
    /* +0x04 the layer mask: BtlMixFlashSet writes it, BtlMixFlashStep hands it
       to layer_mix_set (0x14 shown, 0xFF none). */
    s32 mask;
    /* +0x08 the count: BtlMixFlashSet writes 8, 0xA or 0 by the mode,
       BtlMixFlashStep halves it (1, 3) or counts it (5) and compares it
       signed; BtlEnemyFallShow waits for it to reach 4 (an s8 read);
       BtlMenuInit writes 8. */
    u8 count;
    /* +0x09 the frame: BtlMixFlashSet writes 0, BtlMixFlashStep counts it
       (frame % 6 picks mode 5's colour, mode 2's table entry); BtlMenuInit
       writes 8. */
    u8 frame;
    /* +0x0A two bytes nothing reads or writes. */
    u8 pad_0a[2];
} BtlMixFlash;

LAYOUT_SIZE(BtlMixFlash, 0x0c);
LAYOUT_AT(BtlMixFlash, mode, mode, 0x00);
LAYOUT_AT(BtlMixFlash, on, on, 0x01);
LAYOUT_AT(BtlMixFlash, pad_02, pad_02, 0x02);
LAYOUT_AT(BtlMixFlash, mask, mask, 0x04);
LAYOUT_AT(BtlMixFlash, count, count, 0x08);
LAYOUT_AT(BtlMixFlash, frame, frame, 0x09);
LAYOUT_AT(BtlMixFlash, pad_0a, pad_0a, 0x0a);

/* A show's wave, the 0x18 bytes at the address in D_800B8FEC that BtlShowOpen
   (btl_show.c) takes from the heap: BtlShowWaveSet starts or ends it,
   BtlShowWaveStep runs it each show frame and writes the rows' offsets into
   the block at D_800B9018 and the size and place to the battle's background
   (BtlBgWaveSet) or the tile layer (tile_wave_set). */
typedef struct {
    /* +0x00 BtlShowWaveSet writes 1 (0 for mode 0); BtlShowWaveStep does
       nothing while it is 0. */
    s32 on;
    /* +0x04 BtlShowWaveSet's grow argument: not 0, the wave's size grows a
       step at a time; 0, it falls. */
    s32 grow;
    /* +0x08 the frames gone: BtlShowWaveSet writes 0 when growing, else takes
       it modulo 32; BtlShowWaveStep counts it and takes the place from it
       ((u16) value >> 1 & 0xF). */
    s16 frame_count;
    /* +0x0A BtlShowWaveSet stores its second argument (a byte); nothing reads
       it. */
    s16 unk_0a;
    /* +0x0C the frames a step (BtlShowWaveSet's third argument):
       BtlShowWaveStep works out a new size when frame_count is a multiple of
       it. */
    s16 step_frames;
    /* +0x0E a half-word nothing reads or writes. */
    s16 pad_0e;
    /* +0x10 where the wave goes: 4 the battle's background (BtlBgWaveSet), 8
       the tile layer (tile_wave_set, tile_wave_switch_set); BtlShowWaveSet
       writes it when growing, BtlShowWaveStep reads it before it tests the
       address. */
    s32 mode;
    /* +0x14 the 16 offsets of the wave (D_800B5C3C, D_800B5BFC):
       BtlShowWaveSet writes it when growing; BtlShowWaveStep scales them by
       the size. */
    s32 *table;
} BtlShowWave;

LAYOUT_SIZE(BtlShowWave, 0x18);
LAYOUT_AT(BtlShowWave, on, on, 0x00);
LAYOUT_AT(BtlShowWave, grow, grow, 0x04);
LAYOUT_AT(BtlShowWave, frame_count, frame_count, 0x08);
LAYOUT_AT(BtlShowWave, unk_0a, unk_0a, 0x0a);
LAYOUT_AT(BtlShowWave, step_frames, step_frames, 0x0c);
LAYOUT_AT(BtlShowWave, pad_0e, pad_0e, 0x0e);
LAYOUT_AT(BtlShowWave, mode, mode, 0x10);
LAYOUT_AT(BtlShowWave, table, table, 0x14);

/* BtlAmountDigits: the three digit pieces of one amount the battle shows
   (g_BtlAmountPieces holds one for each amount record): BtlAmountRecordsInit
   copies one digit piece into all three and moves the first 8 pixels left and
   the third 8 right. */
typedef struct {
    /* +0x00 the three digits' pieces, 0x18 bytes each, the left one first.
       Written by BtlAmountRecordsInit (a copy of one digit piece in each, then
       x0 - 8 and + 8). */
    SpritePiece digit[3];
} BtlAmountDigits;

LAYOUT_SIZE(BtlAmountDigits, 0x48);
LAYOUT_AT(BtlAmountDigits, digit, digit, 0x00);
LAYOUT_AT(BtlAmountDigits, digit_0__kind, digit[0].kind, 0x00);
LAYOUT_AT(BtlAmountDigits, digit_0__width, digit[0].width, 0x02);
LAYOUT_AT(BtlAmountDigits, digit_0__height, digit[0].height, 0x04);
LAYOUT_AT(BtlAmountDigits, digit_0__texel_col, digit[0].texel_col, 0x06);
LAYOUT_AT(BtlAmountDigits, digit_0__texel_col_texel, digit[0].texel_col.texel, 0x06);
LAYOUT_AT(BtlAmountDigits, digit_0__texel_col_col, digit[0].texel_col.col, 0x06);
LAYOUT_AT(BtlAmountDigits, digit_0__x0, digit[0].x0, 0x08);
LAYOUT_AT(BtlAmountDigits, digit_0__y0, digit[0].y0, 0x0a);
LAYOUT_AT(BtlAmountDigits, digit_0__x1, digit[0].x1, 0x0c);
LAYOUT_AT(BtlAmountDigits, digit_0__y1, digit[0].y1, 0x0e);
LAYOUT_AT(BtlAmountDigits, digit_0__x2, digit[0].x2, 0x10);
LAYOUT_AT(BtlAmountDigits, digit_0__y2, digit[0].y2, 0x12);
LAYOUT_AT(BtlAmountDigits, digit_0__x3, digit[0].x3, 0x14);
LAYOUT_AT(BtlAmountDigits, digit_0__y3, digit[0].y3, 0x16);

/* BtlWindowSprites: the two sprite list entries of one battle window, at
   g_SpriteList + 0x900 (one pair per window): the outline's, copied from
   g_BtlWindowOutlines, and the box's, copied from g_BtlWindowBoxes
   (BtlWindowSpritesSet). */
typedef struct {
    /* +0x00 the window's outline entry: BtlWindowSpritesSet copies
       g_BtlWindowOutlines[window] here. */
    SpriteEntry outline;
    /* +0x24 the window's box entry: BtlWindowSpritesSet copies
       g_BtlWindowBoxes[window] here. */
    SpriteEntry box;
} BtlWindowSprites;

LAYOUT_SIZE(BtlWindowSprites, 0x48);
LAYOUT_AT(BtlWindowSprites, outline, outline, 0x00);
LAYOUT_AT(BtlWindowSprites, outline_on, outline.on, 0x00);
LAYOUT_AT(BtlWindowSprites, outline_x, outline.x, 0x02);
LAYOUT_AT(BtlWindowSprites, outline_depth, outline.depth, 0x04);
LAYOUT_AT(BtlWindowSprites, outline_y, outline.y, 0x06);
LAYOUT_AT(BtlWindowSprites, outline_attr, outline.attr, 0x08);
LAYOUT_AT(BtlWindowSprites, outline_flags, outline.flags, 0x0a);
LAYOUT_AT(BtlWindowSprites, outline_texel_records, outline.texel_records, 0x0c);
LAYOUT_AT(BtlWindowSprites, outline_pose_table, outline.pose_table, 0x10);
LAYOUT_AT(BtlWindowSprites, outline_piece_table, outline.piece_table, 0x14);
LAYOUT_AT(BtlWindowSprites, outline_pose, outline.pose, 0x18);
LAYOUT_AT(BtlWindowSprites, outline_pad_1a, outline.pad_1a, 0x1a);
LAYOUT_AT(BtlWindowSprites, outline_scaled, outline.scaled, 0x1c);
LAYOUT_AT(BtlWindowSprites, outline_pad_1e, outline.pad_1e, 0x1e);
LAYOUT_AT(BtlWindowSprites, outline_scale_x, outline.scale_x, 0x20);
LAYOUT_AT(BtlWindowSprites, outline_scale_y, outline.scale_y, 0x22);
LAYOUT_AT(BtlWindowSprites, box, box, 0x24);
LAYOUT_AT(BtlWindowSprites, box_on, box.on, 0x24);
LAYOUT_AT(BtlWindowSprites, box_x, box.x, 0x26);
LAYOUT_AT(BtlWindowSprites, box_depth, box.depth, 0x28);
LAYOUT_AT(BtlWindowSprites, box_y, box.y, 0x2a);
LAYOUT_AT(BtlWindowSprites, box_attr, box.attr, 0x2c);
LAYOUT_AT(BtlWindowSprites, box_flags, box.flags, 0x2e);
LAYOUT_AT(BtlWindowSprites, box_texel_records, box.texel_records, 0x30);
LAYOUT_AT(BtlWindowSprites, box_pose_table, box.pose_table, 0x34);
LAYOUT_AT(BtlWindowSprites, box_piece_table, box.piece_table, 0x38);
LAYOUT_AT(BtlWindowSprites, box_pose, box.pose, 0x3c);
LAYOUT_AT(BtlWindowSprites, box_pad_1a, box.pad_1a, 0x3e);
LAYOUT_AT(BtlWindowSprites, box_scaled, box.scaled, 0x40);
LAYOUT_AT(BtlWindowSprites, box_pad_1e, box.pad_1e, 0x42);
LAYOUT_AT(BtlWindowSprites, box_scale_x, box.scale_x, 0x44);
LAYOUT_AT(BtlWindowSprites, box_scale_y, box.scale_y, 0x46);

/* BtlMenuPulse: the battle menu's pulse, a level that swings between a top and
   0, one a frame; the 0x10 bytes BtlMenuInit takes from the heap for
   g_BtlMenuPulse. */
typedef struct {
    /* +0x00 the mode: 0 nothing (BtlMenuPulseStep returns at once); 1 the
       level also goes to layer_mix_set with the mask each frame
       (BtlMenuTargetOpen, BtlMenuWalkOpen); 2 the level only steps
       (BtlMenuKindOpen), BtlMenuKindStep using the direction. BtlMenuPulseSet
       writes it. */
    s32 mode;
    /* +0x04 not 0 while the level goes down, 0 while it goes up:
       BtlMenuPulseSet sets 1, BtlMenuPulseStep flips it at 0 and at the top.
       In mode 2 BtlMenuKindStep, BtlMenuTargetStep and BtlMenuWalkStep copy
       its low 16 bits into +4 of the acting slot's battle record, so the slot
       is drawn while the level goes down. */
    s32 down;
    /* +0x08 the mask BtlMenuPulseStep hands layer_mix_set in mode 1 (0x400
       from every caller); BtlMenuPulseSet writes it. */
    s32 mask;
    /* +0x0C the level, 0 to the top: BtlMenuPulseSet sets the top,
       BtlMenuPulseStep moves it 1 a frame; mode 1 sends it times 2. */
    s16 level;
    /* +0x0E the level's top (6 for the kind menu, 0xC for the target and walk
       menus); BtlMenuPulseSet writes it, BtlMenuPulseStep reads it. */
    s16 top;
} BtlMenuPulse;

LAYOUT_SIZE(BtlMenuPulse, 0x10);
LAYOUT_AT(BtlMenuPulse, mode, mode, 0x00);
LAYOUT_AT(BtlMenuPulse, down, down, 0x04);
LAYOUT_AT(BtlMenuPulse, mask, mask, 0x08);
LAYOUT_AT(BtlMenuPulse, level, level, 0x0c);
LAYOUT_AT(BtlMenuPulse, top, top, 0x0e);

/* BtlEnemyRow: one enemy's row of the table of enemies (D_800A8768, 38 bytes a
   row, row id - 1); BtlEnemyStatsSet copies it field by field into the slot's
   BtlStats, whose fields name these. */
typedef struct {
    /* +0x00 to the stats' side.enemy.action_pick (+0x1A). */
    u8 action_pick;
    /* +0x01 to the stats' level (+0x14). */
    u8 level;
    /* +0x02 to the stats' hp and hp_max (+0x00, +0x02). */
    u16 hp;
    /* +0x04 to the stats' stat_08 (+0x08). */
    u16 stat_08;
    /* +0x06 to the stats' stat_0a (+0x0A). */
    u16 stat_0a;
    /* +0x08 to the stats' stat_0c (+0x0C). */
    u16 stat_0c;
    /* +0x0A to the stats' stat_0e (+0x0E). */
    u16 stat_0e;
    /* +0x0C to the stats' stat_10 (+0x10). */
    u16 stat_10;
    /* +0x0E to the stats' walk_reach (+0x13). */
    u8 walk_reach;
    /* +0x0F to the stats' least_amount (+0x16). */
    u8 least_amount;
    /* +0x10 to the stats' attack_count (+0x12). */
    u8 attack_count;
    /* +0x11 to the stats' side.enemy.trait (+0x1B). */
    u8 trait;
    /* +0x12 to the stats' side.enemy.attack_number (+0x1C). */
    u8 attack_number;
    /* +0x13 to the stats' side.enemy.kind7_number (+0x1D). */
    u8 kind7_number;
    /* +0x14 to the stats' side.enemy.walk_sound (+0x1E). */
    u8 walk_sound;
    /* +0x15 to the stats' walk_speed (+0x18). */
    u8 walk_speed;
    /* +0x16 to the stats' attack_show (+0x17). */
    u8 attack_show;
    /* +0x17 to the stats' attack_sound (+0x19). */
    u8 attack_sound;
    /* +0x18 to the stats' side.enemy.action_sound (+0x1F). */
    u8 action_sound;
    /* +0x19 not copied; nothing reads it. */
    u8 pad_19;
    /* +0x1A to the stats' side.enemy.experience (+0x24). */
    u16 experience;
    /* +0x1C to the stats' side.enemy.money (+0x26). */
    u16 money;
    /* +0x1E to the stats' side.enemy.win_item_1 (+0x20). */
    u8 win_item_1;
    /* +0x1F to the stats' side.enemy.win_rate_1 (+0x21). */
    u8 win_rate_1;
    /* +0x20 to the stats' side.enemy.win_item_2 (+0x22). */
    u8 win_item_2;
    /* +0x21 to the stats' side.enemy.win_rate_2 (+0x23). */
    u8 win_rate_2;
    /* +0x22 to the stats' side.enemy.weak_mask (+0x28). */
    u16 weak_mask;
    /* +0x24 to the stats' side.enemy.resist_mask (+0x2A). */
    u16 resist_mask;
} BtlEnemyRow;

LAYOUT_SIZE(BtlEnemyRow, 0x26);
LAYOUT_AT(BtlEnemyRow, action_pick, action_pick, 0x00);
LAYOUT_AT(BtlEnemyRow, level, level, 0x01);
LAYOUT_AT(BtlEnemyRow, hp, hp, 0x02);
LAYOUT_AT(BtlEnemyRow, stat_08, stat_08, 0x04);
LAYOUT_AT(BtlEnemyRow, stat_0a, stat_0a, 0x06);
LAYOUT_AT(BtlEnemyRow, stat_0c, stat_0c, 0x08);
LAYOUT_AT(BtlEnemyRow, stat_0e, stat_0e, 0x0a);
LAYOUT_AT(BtlEnemyRow, stat_10, stat_10, 0x0c);
LAYOUT_AT(BtlEnemyRow, walk_reach, walk_reach, 0x0e);
LAYOUT_AT(BtlEnemyRow, least_amount, least_amount, 0x0f);
LAYOUT_AT(BtlEnemyRow, attack_count, attack_count, 0x10);
LAYOUT_AT(BtlEnemyRow, trait, trait, 0x11);
LAYOUT_AT(BtlEnemyRow, attack_number, attack_number, 0x12);
LAYOUT_AT(BtlEnemyRow, kind7_number, kind7_number, 0x13);
LAYOUT_AT(BtlEnemyRow, walk_sound, walk_sound, 0x14);
LAYOUT_AT(BtlEnemyRow, walk_speed, walk_speed, 0x15);
LAYOUT_AT(BtlEnemyRow, attack_show, attack_show, 0x16);
LAYOUT_AT(BtlEnemyRow, attack_sound, attack_sound, 0x17);
LAYOUT_AT(BtlEnemyRow, action_sound, action_sound, 0x18);
LAYOUT_AT(BtlEnemyRow, pad_19, pad_19, 0x19);
LAYOUT_AT(BtlEnemyRow, experience, experience, 0x1a);
LAYOUT_AT(BtlEnemyRow, money, money, 0x1c);
LAYOUT_AT(BtlEnemyRow, win_item_1, win_item_1, 0x1e);
LAYOUT_AT(BtlEnemyRow, win_rate_1, win_rate_1, 0x1f);
LAYOUT_AT(BtlEnemyRow, win_item_2, win_item_2, 0x20);
LAYOUT_AT(BtlEnemyRow, win_rate_2, win_rate_2, 0x21);
LAYOUT_AT(BtlEnemyRow, weak_mask, weak_mask, 0x22);
LAYOUT_AT(BtlEnemyRow, resist_mask, resist_mask, 0x24);

/* BtlShowWaveLine: one screen line's record of the show wave's line table
   (BtlShowWaveLines). */
typedef struct {
    /* +0x00 nothing reads or writes it. */
    s32 pad_00;
    /* +0x04 the line's x offset: BtlShowWaveStep writes entry (line & 0xF) of
       its 16 offsets of the frame into every line; nothing reads it. */
    s32 x;
    /* +0x08 nothing reads or writes it. */
    s32 pad_08;
} BtlShowWaveLine;

LAYOUT_SIZE(BtlShowWaveLine, 0x0c);
LAYOUT_AT(BtlShowWaveLine, pad_00, pad_00, 0x00);
LAYOUT_AT(BtlShowWaveLine, x, x, 0x04);
LAYOUT_AT(BtlShowWaveLine, pad_08, pad_08, 0x08);

/* BtlShowWaveLines: the show wave's line table, the 0x1910 bytes BtlShowOpen
   takes from the heap for D_800B9018 (BtlShowClose frees them): a head and a
   record for each of the screen's 0xE0 lines. BtlShowWaveSet and
   BtlShowWaveStep write it and nothing in the program reads it; the wave
   reaches the screen through BtlBgWaveSet and tile_wave_set instead. */
typedef struct {
    /* +0x000 nothing reads or writes them. */
    u8 pad_000[2];
    /* +0x002 BtlShowWaveSet writes 1 when a wave starts to grow; nothing reads
       it. */
    u8 unk_002;
    /* +0x003 nothing reads or writes it. */
    u8 pad_003;
    /* +0x004 BtlShowWaveSet writes 0 when a wave starts to grow; nothing reads
       it. */
    u8 unk_004;
    /* +0x005 nothing reads or writes them. */
    u8 pad_005[7];
    /* +0x00C..+0xA8B the 0xE0 lines' records, 12 bytes each. */
    BtlShowWaveLine line[224];
    /* +0xA8C..+0x190F the rest of the block BtlShowOpen takes: nothing reads
       or writes it. */
    u8 pad_a8c[3716];
} BtlShowWaveLines;

LAYOUT_SIZE(BtlShowWaveLines, 0x1910);
LAYOUT_AT(BtlShowWaveLines, pad_000, pad_000, 0x00);
LAYOUT_AT(BtlShowWaveLines, unk_002, unk_002, 0x02);
LAYOUT_AT(BtlShowWaveLines, pad_003, pad_003, 0x03);
LAYOUT_AT(BtlShowWaveLines, unk_004, unk_004, 0x04);
LAYOUT_AT(BtlShowWaveLines, pad_005, pad_005, 0x05);
LAYOUT_AT(BtlShowWaveLines, line, line, 0x0c);
LAYOUT_AT(BtlShowWaveLines, line_0__pad_00, line[0].pad_00, 0x0c);
LAYOUT_AT(BtlShowWaveLines, line_0__x, line[0].x, 0x10);
LAYOUT_AT(BtlShowWaveLines, line_0__pad_08, line[0].pad_08, 0x14);
LAYOUT_AT(BtlShowWaveLines, pad_a8c, pad_a8c, 0xa8c);

/* BtlEffectTileRow: one effect's tile row, a row of g_BtlEffectTileRows
   (0x800AC5CC, 4 bytes a row, never written): what the tile layer shows for
   an effect with flag 4. BtlEffectLoad points g_BtlEffectTileRow at the row
   byte 1 of the effect's row names. */
typedef struct {
    /* +0x00 the number of the effect's pattern list: BtlEffectLoad takes
       g_BtlTilePatternList from it (BtlTilePatternListGet). */
    u8 pattern_list;
    /* +0x01 the patterns' width: BtlTilePatternPut hands half of it to
       tile_pattern_put as the width in cells; BtlShow0BRun, BtlShow1DRun,
       BtlShow27Run and BtlShow2CRun set the tile layer's scroll x from it
       (BtlTileScrollSet: width * 4 less a record's x, or (width / 2 - 0x14)
       * 8). */
    u8 width;
    /* +0x02 the patterns' height: BtlTilePatternPut hands half of it to
       tile_pattern_put as the height in cells; BtlShow27Run sets the scroll
       y from it (height * 4 plus half the acting record's height less its
       y). */
    u8 height;
    /* +0x03 nothing reads or writes it. */
    u8 pad_03;
} BtlEffectTileRow;

LAYOUT_SIZE(BtlEffectTileRow, 0x04);
LAYOUT_AT(BtlEffectTileRow, pattern_list, pattern_list, 0x00);
LAYOUT_AT(BtlEffectTileRow, width, width, 0x01);
LAYOUT_AT(BtlEffectTileRow, height, height, 0x02);
LAYOUT_AT(BtlEffectTileRow, pad_03, pad_03, 0x03);

/* BtlPartyDataRow: one row of the party's sprite resource, resource 0x197,
   which BtlPartyDataLoad reads to the address in g_ResourceReadBuffer (its
   first 16-bit word is the place of a palette): 8-byte rows from the
   resource's byte 2 on, row kind - 1 for each of the battle's first six
   kinds at g_BtlSlotIds. Where a kind's sprite data and image lie in the
   resource. BtlPartyTallestFind hands a row to BtlPartyImageHeightGet,
   BtlPartyLoad to BtlPartyImageAdd or BtlPartyImagePage4Add (and so to
   BtlPartyImageLoad). */
typedef struct {
    /* +0x00 the place of the kind's packed sprite data, in units of 4 bytes
       from the resource's start: BtlPartyLoad unpacks it from there
       (inflate_buffer). */
    u16 data_place;
    /* +0x02 the unpacked sprite data's length in bytes: BtlPartyLoad takes
       xmalloc_heap_alloc(it) for it. */
    u16 data_length;
    /* +0x04 the place of the kind's packed image, in units of 4 bytes from
       the resource's start: BtlPartyImageLoad unpacks it into the load
       buffer g_LoadBuffer. */
    u16 image_place;
    /* +0x06 the image's height in rows of pixels, in the word's high byte
       (every reader shifts it right by 8): BtlPartyImageLoad sends that many
       rows and marks (height + 7) / 8 units of its column taken;
       BtlPartyImageHeightGet answers it to BtlPartyTallestFind. The low byte
       nothing reads. */
    u16 image_height;
} BtlPartyDataRow;

LAYOUT_SIZE(BtlPartyDataRow, 0x08);
LAYOUT_AT(BtlPartyDataRow, data_place, data_place, 0x00);
LAYOUT_AT(BtlPartyDataRow, data_length, data_length, 0x02);
LAYOUT_AT(BtlPartyDataRow, image_place, image_place, 0x04);
LAYOUT_AT(BtlPartyDataRow, image_height, image_height, 0x06);

#endif
