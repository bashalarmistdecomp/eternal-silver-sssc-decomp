/* chr.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef CHR_H
#define CHR_H

#include "layout.h"

/* One character's record, 0x40 bytes, which lasts from battle to battle:
   twelve of them (ids 0 to 11) at the address in g_ChrRecords (0x801DBF80, set
   by GameStart), filled from resource 0xE7 by ChrRecordsReset.
   BtlPartyStatsSet makes a party slot's BtlStats from it at a battle's start
   and BtlPartyStatsStore writes hp, mp and the items back. */
typedef struct {
    /* +0x00 the character's hp. Written by the resource copies
       (ChrRecordsReset, ChrLevelSet, ScriptCommandChrLevelItemSet, the whole-
       record copy of ScriptCommandChr1ChrBChange), set to hp_max by
       ChrLevelSet, ScriptCommandChrLevelItemSet, ScriptCommandPartyJoin and
       ScriptCommandPartyHpMpFill; MapPartyHpTake takes from it (1 at least);
       BtlPartyStatsStore writes the battle's hp back (1 for 0); outside a
       battle the heal rows write it through g_BtlTargetStats
       (BtlPartyHpShareAddRun, BtlTargetHpFillRun, BtlTargetHpAddRun,
       BtlTargetHpShareAddRun). */
    u16 hp;
    /* +0x02 the most hp the character can have: ChrLevelUp adds a level's gain
       (ChrLevelGainAdd, held at 999); otherwise only the resource copies write
       it. */
    u16 hp_max;
    /* +0x04 the character's mp, what a spell's cost is taken from outside a
       battle (MenuUseRun; halved and rounded up when worn item 0x88 is in
       place 5 or 6). Set to mp_max by ChrLevelSet,
       ScriptCommandChrLevelItemSet, ScriptCommandPartyJoin,
       ScriptCommandPartyHpMpFill; halved by ScriptCommandPartyMpHalve, cleared
       by ScriptCommandPartyMpClear; BtlPartyStatsStore writes the battle's mp
       back; BtlTargetMpAddRun and BtlTargetMpFillRun write it through
       g_BtlTargetStats outside a battle. */
    u16 mp;
    /* +0x06 the most mp the character can have: ChrLevelUp adds a level's gain
       (not for ids 7 and 9, held at 999). */
    u16 mp_max;
    /* +0x08 the stat BtlPartyStatsSet copies to the battle's stat_08, worn
       items counted: ChrLevelUp adds a level's gain (held at 999, with
       stat_08_bare), ChrWornItemStatAdd adds and takes off a worn item's bonus
       (selector 1), BtlWornItemStatLower takes it off when a worn item is used
       up in a battle. Named by its place, as BtlStats' is. */
    u16 stat_08;
    /* +0x0A the battle's stat_0a before the battle (selector 2): ChrLevelUp,
       ChrWornItemStatAdd, BtlWornItemStatLower. */
    u16 stat_0a;
    /* +0x0C the battle's stat_0c before the battle (selector 3): ChrLevelUp,
       ChrWornItemStatAdd, BtlWornItemStatLower. */
    u16 stat_0c;
    /* +0x0E the battle's stat_0e before the battle (selector 4): ChrLevelUp,
       ChrWornItemStatAdd, BtlWornItemStatLower. */
    u16 stat_0e;
    /* +0x10 the battle's stat_10 before the battle (selector 5): ChrLevelUp,
       ChrWornItemStatAdd, BtlWornItemStatLower. */
    u16 stat_10;
    /* +0x12 the battle's attack_count before the battle (selector 6):
       ChrLevelUp adds the gain of header word 7 (with attack_count_bare),
       ChrWornItemStatAdd, BtlWornItemStatLower. */
    u8 attack_count;
    /* +0x13 the battle's walk_reach before the battle (selector 8): ChrLevelUp
       adds the gain of header word 8 (with walk_reach_bare),
       ChrWornItemStatAdd, BtlWornItemStatLower. */
    u8 walk_reach;
    /* +0x14 stat_08 without what worn items add: ChrLevelUp adds the same gain
       to it (held at 0xFF), ChrWornItemStatAdd does not touch it.
       MenuChrNumbersDisp shows it beside stat_08, coloured by which is
       greater. */
    u8 stat_08_bare;
    /* +0x15 stat_0a without worn items: ChrLevelUp (held at 0xFF); shown by
       MenuChrNumbersDisp beside stat_0a. */
    u8 stat_0a_bare;
    /* +0x16 stat_0c without worn items: ChrLevelUp (held at 0xFF); shown by
       MenuChrNumbersDisp beside stat_0c. */
    u8 stat_0c_bare;
    /* +0x17 stat_0e without worn items: ChrLevelUp (held at 0xFF); shown by
       MenuChrNumbersDisp beside stat_0e. */
    u8 stat_0e_bare;
    /* +0x18 stat_10 without worn items: ChrLevelUp (held at 0xFF); shown by
       MenuChrNumbersDisp beside stat_10. */
    u8 stat_10_bare;
    /* +0x19 attack_count without worn items: ChrLevelUp adds header word 7's
       gain to both; shown by MenuChrNumbersDisp beside attack_count. */
    u8 attack_count_bare;
    /* +0x1A walk_reach without worn items: ChrLevelUp adds header word 8's
       gain to both; shown by MenuChrNumbersDisp beside walk_reach. */
    u8 walk_reach_bare;
    /* +0x1B stat_1d without worn items, shown by MenuChrNumbersDisp beside
       stat_1d. No level raises it: only the resource and whole-record copies
       write it. */
    u8 stat_1d_bare;
    /* +0x1C the character's level, 99 at most: ChrLevelUp adds 1;
       BtlPartyStatsSet copies it to the battle's level; BtlWinRun and
       ChrLevelAdd stop at 99; ChrLevelSet and ScriptCommandChrLevelItemSet
       reload the record from resource 0xE7 when asked for a lower level. */
    u8 level;
    /* +0x1D the stat BtlPartyStatsSet copies to the battle's
       side.party.stat_1f, worn items counted (selector 7): ChrWornItemStatAdd
       and BtlWornItemStatLower change it; no level raises it. Named by its own
       place; it is the same stat as BtlStats' stat_1f. */
    u8 stat_1d;
    /* +0x1E..+0x23 the six worn items, places 1 to 6, each the number (from 1)
       of a 0x12-byte item row, 0 for none; an item's kind picks its place
       (kind 4 has places 5 and 6). ChrWornItemSet puts one on and adds its
       values (ChrWornItemValuesAdd), ChrWornItemRemove takes one off;
       MenuWornItemChange and ShopDealRun move place 6 into 5;
       BtlWornItemRemove clears place 5 or 6 when a battle uses the item up.
       BtlPartyStatsSet copies them to the battle's worn_items and takes
       least_amount, attack_show, attack_sound and worn_item_kind from the
       first's row (the specification reads the first as the weapon; the C does
       not say). */
    u8 worn_items[6];
    /* +0x24..+0x29 the six items the character carries, 0 for none.
       ChrItemGive puts one in the first 0, ChrItemRemove takes one out and
       ChrItemListGapClose closes the gap, ChrItemListSort orders them,
       BtlPartyStatsStore writes the battle's items back,
       ScriptCommandStoryBitsItemsReset copies them from resource 0xE7,
       ScriptCommandChrLevelItemSet clears them. BtlPartyStatsSet copies them
       to the battle's items. */
    u8 items[6];
    /* +0x2A..+0x31 the character's eight spells, each the number (from 1) of a
       12-byte spell row, 0 for none: ChrLevelUp learns the row of the new
       level into the place jtbl_800118DC gives, ScriptCommandChrSpellSet sets
       one. BtlPartyStatsSet copies them to the battle's spells. */
    u8 spells[8];
    /* +0x32..+0x37 nothing reads or writes them on their own; only the
       resource copies and whole-record copies carry them. */
    u8 pad_32[6];
    /* +0x38 the character's total experience, 9,999,999 at most: BtlWinRun
       adds the battle's g_BtlWinExperience, ChrLevelAdd adds experience_next
       for each level it gives. MenuChrRecordDisp shows it. */
    u32 experience;
    /* +0x3C the experience the next level still costs: ChrLevelUp sets it from
       the gain table (header word 9, a big-endian word by level); BtlWinRun
       levels the character up while what is left of the battle's experience is
       not below it and then takes the rest off it; BtlWinRun and ChrLevelAdd
       set 0 at level 99. MenuChrRecordDisp shows it. */
    u32 experience_next;
} ChrRecord;

LAYOUT_SIZE(ChrRecord, 0x40);
LAYOUT_AT(ChrRecord, hp, hp, 0x00);
LAYOUT_AT(ChrRecord, hp_max, hp_max, 0x02);
LAYOUT_AT(ChrRecord, mp, mp, 0x04);
LAYOUT_AT(ChrRecord, mp_max, mp_max, 0x06);
LAYOUT_AT(ChrRecord, stat_08, stat_08, 0x08);
LAYOUT_AT(ChrRecord, stat_0a, stat_0a, 0x0a);
LAYOUT_AT(ChrRecord, stat_0c, stat_0c, 0x0c);
LAYOUT_AT(ChrRecord, stat_0e, stat_0e, 0x0e);
LAYOUT_AT(ChrRecord, stat_10, stat_10, 0x10);
LAYOUT_AT(ChrRecord, attack_count, attack_count, 0x12);
LAYOUT_AT(ChrRecord, walk_reach, walk_reach, 0x13);
LAYOUT_AT(ChrRecord, stat_08_bare, stat_08_bare, 0x14);
LAYOUT_AT(ChrRecord, stat_0a_bare, stat_0a_bare, 0x15);
LAYOUT_AT(ChrRecord, stat_0c_bare, stat_0c_bare, 0x16);
LAYOUT_AT(ChrRecord, stat_0e_bare, stat_0e_bare, 0x17);
LAYOUT_AT(ChrRecord, stat_10_bare, stat_10_bare, 0x18);
LAYOUT_AT(ChrRecord, attack_count_bare, attack_count_bare, 0x19);
LAYOUT_AT(ChrRecord, walk_reach_bare, walk_reach_bare, 0x1a);
LAYOUT_AT(ChrRecord, stat_1d_bare, stat_1d_bare, 0x1b);
LAYOUT_AT(ChrRecord, level, level, 0x1c);
LAYOUT_AT(ChrRecord, stat_1d, stat_1d, 0x1d);
LAYOUT_AT(ChrRecord, worn_items, worn_items, 0x1e);
LAYOUT_AT(ChrRecord, items, items, 0x24);
LAYOUT_AT(ChrRecord, spells, spells, 0x2a);
LAYOUT_AT(ChrRecord, pad_32, pad_32, 0x32);
LAYOUT_AT(ChrRecord, experience, experience, 0x38);
LAYOUT_AT(ChrRecord, experience_next, experience_next, 0x3c);

/* One character's 8-byte record of battle place and presets, one per character
   id at the address in g_ChrPlacePresetRecords (0x801DC280, GameStart): the
   place x and y in cells and three presets of (kind, number). ChrRecordsReset
   fills ids 1 to 11 for a new game; the menu's battle-places and preset pages
   read and write it; MapBtlRun copies all eight bytes of each member into the
   battle's byte tables. */
typedef struct {
    /* +0x00 the character's battle place, x in cells of 8 pixels:
       MenuBtlPlacesOpen copies it to g_MenuBtlPlacesXCells and puts the member
       at view x + (place_x - 3) * 8; MenuBtlPlacesStep (menu_page.c) writes it
       back; MapBtlRun (map_btl.c) copies it less the battle's x offset to
       D_800B8E88; ChrRecordsReset (map_sprite.c) and
       ScriptCommandChr1ChrBChange (lns_intp.c, the whole record) write it. */
    u8 place_x;
    /* +0x01 the battle place's y in cells (MenuBtlPlacesOpen: view y +
       (place_y - 6) * 8; MenuBtlPlacesStep writes it back; MapBtlRun copies it
       to D_800B8E90). */
    u8 place_y;
    /* +0x02..+0x07 the character's three presets, each a kind byte and a
       number byte (preset n at +2 + n * 2 and +3 + n * 2). MenuPresetSet
       writes one (kind, number); MenuPresetStep, MenuPresetDisp and
       MenuPresetMemberDisp (menu_page.c) read them by g_MenuPresetCursor;
       MapBtlRun copies the six bytes to D_800B8E98, D_800B8EB4, D_800B8EA0,
       D_800B8EBC, D_800B8EA8 and D_800B8EC4. */
    u8 presets[6];
} ChrPlacePresetRecord;

LAYOUT_SIZE(ChrPlacePresetRecord, 0x08);
LAYOUT_AT(ChrPlacePresetRecord, place_x, place_x, 0x00);
LAYOUT_AT(ChrPlacePresetRecord, place_y, place_y, 0x01);
LAYOUT_AT(ChrPlacePresetRecord, presets, presets, 0x02);

/* ChrLevelGainHead: one character's head in the level-up gain table
   g_ChrLevelGainTable, 0x14 bytes for each id from 1 (the head of id at
   (id - 1) * 0x14): ten 16-bit words, each stored high byte first, each the
   offset from the table's start of a row indexed by the old level. Rows 0 to
   8 are byte rows of the gains a level brings, row 9 a row of 32-bit words
   (high byte first) of the experience the next level costs. ChrLevelUp reads
   them: words 0 to 6 through ChrLevelGainGet (passing each word's address),
   words 7 to 9 itself. Named by the ChrRecord fields the gains go to. */
typedef struct {
    /* +0x00 word 0: the row of hp_max's gains (ChrLevelGainAdd adds it). */
    u16 hp_max_row;
    /* +0x02 word 1: the row of mp_max's gains (not read for ids 7 and 9). */
    u16 mp_max_row;
    /* +0x04 word 2: the row of stat_08's gains (with stat_08_bare). */
    u16 stat_08_row;
    /* +0x06 word 3: the row of stat_0a's gains. */
    u16 stat_0a_row;
    /* +0x08 word 4: the row of stat_0c's gains. */
    u16 stat_0c_row;
    /* +0x0A word 5: the row of stat_0e's gains. */
    u16 stat_0e_row;
    /* +0x0C word 6: the row of stat_10's gains. */
    u16 stat_10_row;
    /* +0x0E word 7: the row of attack_count's gains (with
       attack_count_bare); ChrLevelUp swaps its bytes itself. */
    u16 attack_count_row;
    /* +0x10 word 8: the row of walk_reach's gains (with walk_reach_bare). */
    u16 walk_reach_row;
    /* +0x12 word 9: the row of the 32-bit words ChrLevelUp puts in
       experience_next, four bytes a level. */
    u16 experience_next_row;
} ChrLevelGainHead;

LAYOUT_SIZE(ChrLevelGainHead, 0x14);
LAYOUT_AT(ChrLevelGainHead, hp_max_row, hp_max_row, 0x00);
LAYOUT_AT(ChrLevelGainHead, mp_max_row, mp_max_row, 0x02);
LAYOUT_AT(ChrLevelGainHead, stat_08_row, stat_08_row, 0x04);
LAYOUT_AT(ChrLevelGainHead, stat_0a_row, stat_0a_row, 0x06);
LAYOUT_AT(ChrLevelGainHead, stat_0c_row, stat_0c_row, 0x08);
LAYOUT_AT(ChrLevelGainHead, stat_0e_row, stat_0e_row, 0x0a);
LAYOUT_AT(ChrLevelGainHead, stat_10_row, stat_10_row, 0x0c);
LAYOUT_AT(ChrLevelGainHead, attack_count_row, attack_count_row, 0x0e);
LAYOUT_AT(ChrLevelGainHead, walk_reach_row, walk_reach_row, 0x10);
LAYOUT_AT(ChrLevelGainHead, experience_next_row, experience_next_row, 0x12);

#endif
