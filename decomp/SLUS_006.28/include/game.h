/* game.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef GAME_H
#define GAME_H

#include "layout.h"
#include "map.h"

/* The first 16 bytes of the block of the game's records that a save holds (the
   0xC00 bytes at g_GameSaveBlock, 0x801DBE68, GameStart): the program's text,
   the play time, the place and the first member's level. A save carries the
   block at its +0x200, so these are also the first 16 bytes of every save's
   records head; the menu keeps each card slot's 16 in g_MenuCardSlotHeads
   (sixteen entries) to list the saves. */
typedef struct {
    /* +0x00..+0x09 the program's ten bytes of text LUNAR_v010 (the text at
       D_800E4F8C that MenuInit writes), which GameStart copies here with
       mem_move; a save carries them with the block. MenuLoadStep takes a card
       slot as holding a save while the first byte of its head in
       g_MenuCardSlotHeads is not 0 (MenuCardListRead clears the table first).
       Nothing else reads them. */
    u8 text[10];
    /* +0x0A the play time in minutes: MenuSaveWrite stores g_PlayTimeMinutes
       here before the block is saved, GameEnter (map_change.c) takes it back
       into g_PlayTimeMinutes, GameRecordsReset (game_start.c) zeroes it for a new
       game; MenuCardListDisp shows each slot's / 60 and % 60. */
    u16 play_time_minutes;
    /* +0x0C..+0x0D nothing reads or writes them on their own; they go with the
       block's copies. */
    u8 pad_0c[2];
    /* +0x0E the number of the name of the place the party is at: MenuSaveWrite
       stores g_MapPlaceName, GameEnter takes it back; MenuCardListDisp writes
       string 0x400 + it for each slot. */
    u8 place_name;
    /* +0x0F the level of the party's first member (ChrRecord.level of
       g_PartyIds[0]): MenuSaveWrite stores it; MenuCardListDisp shows it for
       each slot. Nothing takes it back. */
    u8 level;
} GameSaveHead;

LAYOUT_SIZE(GameSaveHead, 0x10);
LAYOUT_AT(GameSaveHead, text, text, 0x00);
LAYOUT_AT(GameSaveHead, play_time_minutes, play_time_minutes, 0x0a);
LAYOUT_AT(GameSaveHead, pad_0c, pad_0c, 0x0c);
LAYOUT_AT(GameSaveHead, place_name, place_name, 0x0e);
LAYOUT_AT(GameSaveHead, level, level, 0x0f);

/* The record of the map's state inside the block of the game's records a save
   holds, at the address in g_GameMapRecord (0x801DC2E0, GameStart).
   MenuSaveWrite writes the map's state into it just before a save; GameEnter
   (map_change.c) reads every field back when it enters the field from a
   loaded save. */
typedef struct {
    /* +0x00 the number of the map: MenuSaveWrite stores g_MapNumber; GameEnter
       stores (u8) its request for a request of 0x100 or more (no save) and
       hands it to MapSet. */
    u8 map;
    /* +0x01 the map's party kind: MenuSaveWrite stores g_MapPartyKind,
       GameEnter takes it back. */
    u8 party_kind;
    /* +0x02 the mix level of layer 8: MenuSaveWrite stores g_MapMix, GameEnter
       takes it back (and sets layer 8 from it). */
    u16 mix;
    /* +0x04..+0x09 the map's colour, the three numbers at g_MapCol, copied as
       one 6-byte block: MenuSaveWrite from g_MapCol_blk, GameEnter back into
       g_MapCol (MapCol, map.h, below). */
    MapCol col;
    /* +0x0A..+0x11 the party ring's entry of place 0 (MapPartyRingEntry,
       map.h: x, y, height, facing, class), the spot the party's first record
       stands on and its facing: MenuSaveWrite fills it with
       MapPartyRingEntryGet(0, its address); GameEnter hands its address to
       MapPartyInit, which puts the party on the spot. */
    MapPartyRingEntry ring_entry;
    /* +0x12 the view's left limit: MenuSaveWrite stores g_MapLimitLeft,
       GameEnter takes it back. */
    u16 limit_left;
    /* +0x14 the view's right limit (g_MapLimitRight; MenuSaveWrite,
       GameEnter). */
    u16 limit_right;
    /* +0x16 the view's top limit (g_MapLimitTop; MenuSaveWrite, GameEnter). */
    u16 limit_top;
    /* +0x18 the view's bottom limit (g_MapLimitBottom; MenuSaveWrite,
       GameEnter). */
    u16 limit_bottom;
    /* +0x1A the number of party members: MenuSaveWrite stores g_PartyCount,
       GameEnter takes it back. */
    u8 party_count;
    /* +0x1B the party's least level: MenuSaveWrite stores g_PartyLevelLeast,
       GameEnter takes it back. */
    u8 party_level_least;
    /* +0x1C the number of the set of map objects whose data was loaded:
       MenuSaveWrite stores g_MapObjectsDataNumber; GameEnter hands it to
       MapObjectsRestore with the saved entries (MapObjectSaveEntry, below). */
    u8 objects_data_number;
    /* +0x1D the script's number: MenuSaveWrite stores g_ScriptNumber;
       GameEnter zeroes it for a request of 0x100 or more and hands it to
       MapSet. */
    u8 script_number;
    /* +0x1E the number of the music: MenuSaveWrite stores g_MusicNumber;
       GameEnter zeroes it for a request of 0x100 or more and hands it to
       MapSet. */
    u8 music_number;
    /* +0x1F nothing reads or writes it. */
    u8 pad_1f;
} GameMapRecord;

LAYOUT_SIZE(GameMapRecord, 0x20);
LAYOUT_AT(GameMapRecord, map, map, 0x00);
LAYOUT_AT(GameMapRecord, party_kind, party_kind, 0x01);
LAYOUT_AT(GameMapRecord, mix, mix, 0x02);
LAYOUT_AT(GameMapRecord, col, col, 0x04);
LAYOUT_AT(GameMapRecord, col_first, col.first, 0x04);
LAYOUT_AT(GameMapRecord, col_second, col.second, 0x06);
LAYOUT_AT(GameMapRecord, col_third, col.third, 0x08);
LAYOUT_AT(GameMapRecord, ring_entry, ring_entry, 0x0a);
LAYOUT_AT(GameMapRecord, ring_entry_x, ring_entry.x, 0x0a);
LAYOUT_AT(GameMapRecord, ring_entry_y, ring_entry.y, 0x0c);
LAYOUT_AT(GameMapRecord, ring_entry_height, ring_entry.height, 0x0e);
LAYOUT_AT(GameMapRecord, ring_entry_facing, ring_entry.facing, 0x0f);
LAYOUT_AT(GameMapRecord, ring_entry_class, ring_entry.class, 0x10);
LAYOUT_AT(GameMapRecord, limit_left, limit_left, 0x12);
LAYOUT_AT(GameMapRecord, limit_right, limit_right, 0x14);
LAYOUT_AT(GameMapRecord, limit_top, limit_top, 0x16);
LAYOUT_AT(GameMapRecord, limit_bottom, limit_bottom, 0x18);
LAYOUT_AT(GameMapRecord, party_count, party_count, 0x1a);
LAYOUT_AT(GameMapRecord, party_level_least, party_level_least, 0x1b);
LAYOUT_AT(GameMapRecord, objects_data_number, objects_data_number, 0x1c);
LAYOUT_AT(GameMapRecord, script_number, script_number, 0x1d);
LAYOUT_AT(GameMapRecord, music_number, music_number, 0x1e);
LAYOUT_AT(GameMapRecord, pad_1f, pad_1f, 0x1f);

/* GameOptions: the seven option bytes in the game's records, at the address in
   g_GameOptions (0x801DC374); g_MenuOptions is the option page's working copy. */
typedef struct {
    /* +0x00 GameStart sets 0; read only as one of the seven MenuOptionOpen and
       MenuOptionStep copy. */
    u8 unk_00;
    /* +0x01 sound in one channel when not 0: SoundMonoSet picks SsSetMono or
       SsSetStereo by it. GameStart takes it from D_800B89F5; the option page's
       first row (MenuOptionStep) flips it. */
    u8 mono;
    /* +0x02 the order the item lists are sorted in, 0 to 2 (ChrItemListSort);
       GameStart sets 0, the option page steps it round. */
    u8 sort_order;
    /* +0x03 the message's press wait: ScriptLabelRun copies it to
       g_MessagePressWait before each run. GameStart sets 1, the option page
       flips it. */
    u8 press_wait;
    /* +0x04 the message's speed: ScriptLabelRun copies it to g_MessageSpeed.
       GameStart sets 0, the option page flips it. */
    u8 message_speed;
    /* +0x05 not 0 while the controller's actuators may run: pad_update zeroes
       both levels while it is 0, and ScriptCommandActuatorLevelsSet,
       MapViewScrollSet and the shows that set levels (BtlShow06Run,
       BtlShow08Run, BtlShow14Run, BtlShow2DRun) test it. GameStart sets 1;
       BtlFrameStep and the option page flip it. */
    u8 actuators;
    /* +0x06 not 0 while the battle shows each action's name:
       BtlActionStringDisp does nothing while it is 0. GameStart sets 1;
       BtlFrameStep and the option page flip it. */
    u8 action_strings;
} GameOptions;

LAYOUT_SIZE(GameOptions, 0x07);
LAYOUT_AT(GameOptions, unk_00, unk_00, 0x00);
LAYOUT_AT(GameOptions, mono, mono, 0x01);
LAYOUT_AT(GameOptions, sort_order, sort_order, 0x02);
LAYOUT_AT(GameOptions, press_wait, press_wait, 0x03);
LAYOUT_AT(GameOptions, message_speed, message_speed, 0x04);
LAYOUT_AT(GameOptions, actuators, actuators, 0x05);
LAYOUT_AT(GameOptions, action_strings, action_strings, 0x06);

#endif
