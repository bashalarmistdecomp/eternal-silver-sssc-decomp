/* card.h: the memory card's save file, as the card pages build, write, read
   and check it (card.c, menu_card.c). Written 2026-10-19 from the C of
   card_save_header_set, card_save_title_set (outline.c), MenuSaveWrite,
   MenuLoadRead, card_save_records_head_read, card_save_sum_get and
   card_save_sum_check (docs/naming.md, "Structs and their fields"). */
#ifndef CARD_H
#define CARD_H

#include "layout.h"
#include "map.h"

/* CardSave: one save, the 0x2000 bytes of a save file on the memory card
   (card_save_read and card_save_write move all of them; card_save_write
   makes a file of one block for it). MenuSaveWrite builds it in the buffer at
   g_MenuCardBuffer (0x801CFB00, MenuInit) and hands it to card_save_write;
   MenuLoadRead reads one with card_save_read into the fixed buffer at
   0x801A7B00, checks its sum and lays it out for GameEnter. Four parts: the
   head card_save_header_set writes (+0x0000..+0x01FF), the block of the
   game's records (+0x0200..+0x0DFF), the map objects' flags and entries
   (+0x0E00..+0x1FFB) and the sum (+0x1FFC). The file's name is not in it:
   card_save_read, card_save_write and card_save_records_head_read build it
   from g_CardSaveFileName and the save's number.

   The head is the one every memory-card file begins with: two letters, the
   icon's frame count, the file's blocks, a title, a palette and three icon
   frames. The C only writes it (card_save_header_set); the console reads it
   to show the file. Its parts take the card's file format's words, by the
   owner's ruling of 2026-10-19, except the title, which is the C's own word
   (card_save_title_set). */
typedef struct {
    /* +0x0000..+0x0001 the two letters every file's head begins with, "SC"
       (the card's file format's word): card_save_header_set writes them;
       nothing in the program reads them. */
    u8 magic[2];
    /* +0x0002 the icon's frame count, 0x10 + the count (the format's word):
       card_save_header_set writes 0x13, three frames, the three that
       icon_frames holds; nothing in the program reads it. */
    u8 icon_frame_count;
    /* +0x0003 the blocks the file takes (the format's word):
       card_save_header_set writes 1 (card_save_write makes the file one
       block); nothing in the program reads it. */
    u8 block_count;
    /* +0x0004..+0x0043 the save's title: card_save_title_set (outline.c) makes
       the text "LUNAR-%02d %d:%02d L%d %s" of g_CardSaveTitleFormat (the
       save's number + 1, the play time's hours and minutes, the first
       member's level, the place's short name from g_CardSavePlaceNames) and
       widens it here to two-byte characters with two zero bytes behind;
       card_save_header_set hands it the address. Nothing else in the program
       reads it (card_save_title_set reads its own text back as it widens
       it). Signed as card_save_title_set reads it. */
    s8 title[64];
    /* +0x0044..+0x005F card_save_header_set zeroes them (bzero) after the
       title is written, which cuts a title that ran past its 64 bytes;
       nothing else writes them and nothing reads them. A reserved stretch of
       the head (the format's word). */
    u8 reserved[28];
    /* +0x0060..+0x007F the icon's 16-colour palette (the format's word): one
       of the three sets of 32 bytes at g_CardSaveIconPalettes, set (the save's
       number % 3), copied by card_save_header_set; nothing in the program
       reads it. */
    u8 icon_palette[32];
    /* +0x0080..+0x01FF the icon's three frames of 128 bytes (the format's
       word): one of the three sets of 384 bytes at g_CardSaveIconFrames, set
       (the save's number % 3), copied by card_save_header_set; nothing in the
       program reads them. */
    u8 icon_frames[384];
    /* +0x0200..+0x0DFF the block of the game's records, the 0xC00 bytes at
       g_GameSaveBlock (0x801DBE68): GameSaveHead at its +0x000, the character
       records g_ChrRecords at +0x118, GameMapRecord at +0x478, the party's ids
       g_PartyIds at +0x500, GameOptions at +0x50C, the money g_GameMoney at
       +0x514 and the story bits g_StoryBits from +0x518 (GameStart's
       addresses). MenuSaveWrite copies the block here (mem_move_32);
       MenuLoadRead copies it out to the start of the buffer it is given, which
       GameEnter copies back over the block; card_save_records_head_read reads
       its first 0x80 bytes (the records head) from this offset of the file
       for the list of saves. No type of the whole block is defined, so its
       bytes. */
    u8 records[3072];
    /* +0x0E00..+0x0E7F a byte for each of the 0x80 rows of g_MapObjects, the
       row's first byte, its kind (0 for a row not in use): MenuSaveWrite
       writes them; MenuLoadRead reads them, a byte not 0 taking the next entry
       of object_entries for the row and a 0 a zeroed entry. Signed: the
       bytes need MenuLoadRead's test to load them signed. */
    s8 object_flags[128];
    /* +0x0E80..+0x0F7F nothing reads or writes them but the clear of the
       whole save and the sum. */
    u8 pad_0e80[256];
    /* +0x0F80..+0x1FE7 the entries of the map objects' rows in use
       (MapObjectSaveEntry, map.h), packed in the rows' order with no gap:
       MenuSaveWrite writes one for each row whose flag is not 0, MenuLoadRead
       takes them in order back out. 105 is as many as fit before the sum;
       nothing in the C bounds the count (MenuSaveWrite walks all 0x80 rows),
       and how many rows can be in use at once was not read. */
    MapObjectSaveEntry object_entries[105];
    /* +0x1FE8..+0x1FFB nothing writes them while 105 rows or fewer are in
       use: a 106th entry would run over them and over the sum. */
    u8 pad_1fe8[20];
    /* +0x1FFC the save's sum: card_save_sum_get of the 0x7FF words before
       it. MenuSaveWrite stores it last before card_save_write; MenuLoadRead
       compares it (by its label g_MenuLoadSum, the fixed buffer's +0x1FFC)
       with the sum of the save card_save_read brought and takes nothing when
       they differ; card_save_sum_check, which nothing calls, makes the same
       test. */
    s32 sum;
} CardSave;

LAYOUT_SIZE(CardSave, 0x2000);
LAYOUT_AT(CardSave, magic, magic, 0x0000);
LAYOUT_AT(CardSave, icon_frame_count, icon_frame_count, 0x0002);
LAYOUT_AT(CardSave, block_count, block_count, 0x0003);
LAYOUT_AT(CardSave, title, title, 0x0004);
LAYOUT_AT(CardSave, reserved, reserved, 0x0044);
LAYOUT_AT(CardSave, icon_palette, icon_palette, 0x0060);
LAYOUT_AT(CardSave, icon_frames, icon_frames, 0x0080);
LAYOUT_AT(CardSave, records, records, 0x0200);
LAYOUT_AT(CardSave, object_flags, object_flags, 0x0e00);
LAYOUT_AT(CardSave, pad_0e80, pad_0e80, 0x0e80);
LAYOUT_AT(CardSave, object_entries, object_entries, 0x0f80);
LAYOUT_AT(CardSave, object_entries_0__kind, object_entries[0].kind, 0x0f80);
LAYOUT_AT(CardSave, object_entries_0__step_list, object_entries[0].step_list, 0x0fa4);
LAYOUT_AT(CardSave, object_entries_104__step_list, object_entries[104].step_list, 0x1fe4);
LAYOUT_AT(CardSave, pad_1fe8, pad_1fe8, 0x1fe8);
LAYOUT_AT(CardSave, sum, sum, 0x1ffc);

#endif
