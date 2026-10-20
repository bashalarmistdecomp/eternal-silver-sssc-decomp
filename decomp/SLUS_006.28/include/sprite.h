/* sprite.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef SPRITE_H
#define SPRITE_H

#include "layout.h"

/* SpriteEntry: an entry of the sprite list (g_SpriteList, 0x96 entries of 0x24
   bytes), which sprite_list_draw draws; the battle's and the map's records
   carry one at +4 and copy it into the list each frame. */
typedef struct {
    /* +0x00 (in a BtlRecord +0x04) the sprite entry's on word:
       sprite_list_draw draws an entry of the list whose first word is not 0,
       so this switches the record's picture on and off while its animation
       still runs. BtlRecordInit and BtlRecordSpriteDataSet set 1;
       BtlShowAnimationPlay, BtlShow19Run and BtlEffectCopyEnterRun write
       (frame & 1) to blink it; BtlMenuKindStep writes the menu pulse's
       direction word; BtlMenuKindClose sets 1 again; BtlRecordClear,
       BtlMenuClose and BtlAmountShowCheck set 0. */
    u16 on;
    /* +0x02 (in a BtlRecord +0x06) x on the screen in pixels, of the figure's
       origin (its feet, the middle of its width: the shows and target marks
       take half the width either side). sprite_draw draws at it (no scroll is
       taken off in a battle). Written by the loads (BtlPartyLoad,
       BtlEnemyLoad: column * 8), the walk (BtlWalkPathFollow), the moves
       (BtlTargetBackMove/ForwardMove, BtlRecordSpotMove, BtlTravelRun,
       BtlRecordPlaceCopy) and the shows. */
    s16 x;
    /* +0x04 (in a BtlRecord +0x08) the depth: the sort key sprite_draw turns
       into the ordering-table entry (0xFF less depth + 0x20, held to 0..0xFF)
       for an entry of class 0x2000 or 0x4000 (flags bits 0x6000), so a larger
       depth is drawn in front. Kept equal to y while a record stands on the
       ground (BtlEnemyLoad sets it from y, BtlPartyLoad q[4] = q[5],
       BtlActingPlaceAnimationRun, the walk moves the two together); moved off
       y only to order records: y + 1 or the other's depth + 1 to stand in
       front of another record (BtlRecordPlaceCopy, BtlRecordSpotMove,
       BtlMenuWalkOpen, the shows' copies), + 1 / - 1 around an attack
       (BtlAttackAnimationRun, BtlActingAnimationOutcomeRun,
       BtlAttackMissShow), + 2 / + 6 / + 0xB4 for a show's records
       (BtlShow9FRun, BtlShowABRun, BtlShow1ARun), y | 0x1000 for the menu's
       copies (BtlMenuPresetOpen/Close) and 0xFFF for the menu's marks
       (BtlMenuInit). */
    s16 depth;
    /* +0x06 (in a BtlRecord +0x0A) y on the screen in pixels of the figure's
       origin (its feet): what sprite_draw draws at. Moved alone for a fall, a
       jump, a lift or a shake (BtlEnemyFallShow, BtlShakesRun, BtlShow2DRun,
       BtlEffectCopyTargetPlay), with depth for a walk. BtlEnemyLoad holds an
       enemy's to 0x70..0xB0. The marks and amounts are put relative to it (y -
       height above the figure: BtlMenuTargetMark; BtlAmountRecordStart/Step). */
    s16 y;
    /* +0x08 (in a BtlRecord +0x0C) the sprite's attribute word. Bit 0x2000
       mirrors the picture across: the record faces smaller x (the glossary's
       facing: 0x2020 faces left, 0x20 right; BtlFacingTowardSet,
       BtlFacingBothSet, BtlFacingNearestSet, BtlFacingPartySet, the loads and
       the shows set it); bit 0x4000 mirrors it down (BtlShow2ERun sets and
       clears it); bit 0x8000 makes it semi-transparent with colour table 2
       (sprite_draw). Bit 0x20 is written with every facing and
       BtlAmountShowStart clears bits 0x60; nothing read in the C tests them. */
    u16 attr;
    /* +0x0A (in a BtlRecord +0x0E) the sprite's draw word: the low ten bits
       the palette (GetClut(low byte, 0x1F8 | bits 0x300)), bit 0x400 semi-
       transparency, bits 0xC00 the mix group (the battle sets and clears bits
       0x1C00 together: 0x800 marks a target, BtlMenuTargetMark; 0x4800 the
       amount show, BtlAmountShowStart; 0xE3FF / 0xA3FF clear them), bits
       0x6000 the class that picks the ordering-table entry (0x2000 and 0x4000
       by depth, 0x6000 by y, 0 a fixed entry), bit 0x8000 colour table 1. Set
       to (palette & 0x3FF) | 0x2000 by BtlRecordSpriteDataSet, 0x2000 by
       BtlPartyLoad, 0x2100 by BtlEnemyLoad, 0x4300 | (style & 0xF0) by
       BtlAmountRecordStart. */
    u16 flags;
    /* +0x0C (in a BtlRecord +0x10) the address of the sprite data's 12-byte
       texel records (sprite page at +2, u and v at +8 and +0xA); the sprite
       data's fifth header word. Written by BtlRecordSpriteDataSet,
       BtlEffectLoad, BtlPartyLoad, BtlEnemyLoad and BtlAmountRecordsInit
       (g_BtlAmountTexelRecords); BtlRecordSpriteBlocksSet does not write it. */
    s32 texel_records;
    /* +0x10 (in a BtlRecord +0x14) the address of the pose table: 4-byte rows,
       one for each pose, a 16-bit count of pieces and the 16-bit number of the
       first piece. Written by the same loaders (BtlRecordSpriteBlocksSet too,
       BtlAmountRecordStart and BtlAmountRecordsInit point it at the record's
       own row of g_BtlAmountPoseRows), zeroed by BtlRecordClear and
       BtlAmountShowCheck. */
    s32 pose_table;
    /* +0x14 (in a BtlRecord +0x18) the address of the piece table: 0x18-byte
       pieces (a kind word, width and height at +2 and +4, a texel record's
       number at +6, x and y at +8 and +0xA). Written and zeroed by the same
       functions as pose_table; BtlAmountRecordStart reads it to write the
       digits' texel numbers. */
    s32 piece_table;
    /* +0x18 (in a BtlRecord +0x1C) the pose drawn. The record's own word is
       only zeroed (BtlRecordInit, BtlRecordSpriteDataSet,
       BtlRecordSpriteBlocksSet, BtlAmountRecordsInit): BtlAnimationStep writes
       the step's pose into the list's copy after it copies the entry,
       BtlAmountRecordsStep writes 0 there. */
    u16 pose;
    /* +0x1A (in a BtlRecord +0x1E) nothing reads or writes it. */
    u16 pad_1a;
    /* +0x1C (in a BtlRecord +0x20) 1: the sprite is drawn scaled by scale_x
       and scale_y (sprite_piece_draw goes through sprite_scale_draw); 0 plain.
       Set 1 / 0 by BtlRecordScaleSet, BtlEnemyFallShow, BtlShow08Run,
       BtlShow2BRun, BtlShow2CRun; 0 by BtlRecordInit, BtlRecordSpriteDataSet,
       BtlRecordSpriteBlocksSet, BtlEffectLoad, BtlAmountRecordsInit. */
    u16 scaled;
    /* +0x1E (in a BtlRecord +0x22) nothing reads or writes it. */
    u16 pad_1e;
    /* +0x20 (in a BtlRecord +0x24) the scale across, 0x100 = 1
       (sprite_scale_draw multiplies the piece's offset and size by it >> 8).
       BtlRecordScaleSet, BtlEnemyFallShow (the fall's table D_800B5F1C),
       BtlShow08Run, BtlShow2BRun, BtlShow2CRun write it;
       BtlRecordSpriteDataSet, BtlRecordSpriteBlocksSet, BtlEffectLoad,
       BtlAmountRecordsInit and BtlEnemyFallShow write both scales at once as
       one 32-bit word (0, or 0x01000100 for 1 and 1). */
    s16 scale_x;
    /* +0x22 (in a BtlRecord +0x26) the scale down, 0x100 = 1; written with
       scale_x by the same functions (and through the 32-bit word at +0x24). */
    s16 scale_y;
} SpriteEntry;

LAYOUT_SIZE(SpriteEntry, 0x24);
LAYOUT_AT(SpriteEntry, on, on, 0x00);
LAYOUT_AT(SpriteEntry, x, x, 0x02);
LAYOUT_AT(SpriteEntry, depth, depth, 0x04);
LAYOUT_AT(SpriteEntry, y, y, 0x06);
LAYOUT_AT(SpriteEntry, attr, attr, 0x08);
LAYOUT_AT(SpriteEntry, flags, flags, 0x0a);
LAYOUT_AT(SpriteEntry, texel_records, texel_records, 0x0c);
LAYOUT_AT(SpriteEntry, pose_table, pose_table, 0x10);
LAYOUT_AT(SpriteEntry, piece_table, piece_table, 0x14);
LAYOUT_AT(SpriteEntry, pose, pose, 0x18);
LAYOUT_AT(SpriteEntry, pad_1a, pad_1a, 0x1a);
LAYOUT_AT(SpriteEntry, scaled, scaled, 0x1c);
LAYOUT_AT(SpriteEntry, pad_1e, pad_1e, 0x1e);
LAYOUT_AT(SpriteEntry, scale_x, scale_x, 0x20);
LAYOUT_AT(SpriteEntry, scale_y, scale_y, 0x22);

/* One piece of a sprite, a 0x18-byte row of the piece table a sprite entry's
   piece_table points at: sprite_piece_draw draws it by its kind. The battle
   builds its own pieces: the windows' boxes and outlines
   (g_BtlWindowBoxPieces, g_BtlWindowOutlinePieces) and the amounts' digits
   (g_BtlAmountPieces). */
typedef struct {
    /* +0x00 the piece's kind word: bits 0x7000 the kind (0 a textured
       rectangle; 0x1000 and 0x2000 a textured rectangle anchored by the modes
       in bits 0xF00 (sprite_piece_edges_get); 0x3000 a textured quad on four
       corners; 0x4000 a box in one colour; 0x5000 an outline, in a battle one
       LINE_F3 through three points; 0x6000 nothing), bits 0x30 the piece's
       mirror bits (exclusive-or the entry's attr bits 0x6000 >> 9).
       BtlWindowsInit writes 0x4000 for the boxes and 0x5000 for the outlines,
       BtlAmountRecordsInit 0 for the digits. */
    u16 kind;
    /* +0x02 the texture's width in pixels (kinds 0 to 0x3000:
       sprite_piece_draw, sprite_scale_draw); BtlRecordSizeSet adds it to x to
       measure a figure. 8 for the digits; 0 for the windows' pieces. */
    s16 width;
    /* +0x04 the texture's height in pixels, as width. */
    s16 height;
    /* +0x06 a union by kind. texel: sprite_piece_draw takes the texel record
       entry.texel_records + texel * 12; BtlAmountRecordStart writes the
       digits' (base + digit, base 1 or 0xB, 0 for a digit not shown). col: the
       windows' 0x3E4 and 0x3E6 (BtlWindowsInit). */
    union {
        /* texel: used by BtlAmountRecordStart, c, digits, kinds, misc, of,
           pieces, rendering, sprite_piece_draw, the, to */
        u16 texel;
        /* col: used by BtlWindowsInit, MenuRecordsInit, MessageRecordsInit,
           ShopOpen, and, c, cursor, g_MenuShapes, g_ShopShapes, kinds, menu,
           message and others */
        u16 col;
    } texel_col;
    /* +0x08 the first point's x: for kind 0 the piece's x offset from the
       sprite's place (mirrored with it), for the anchored kinds the anchor's
       x, for a quad, a box and an outline a corner (the box from (x0, y0) to
       (x2, y2); the outline's line runs (x1, y1), (x0, y0), (x3, y3)).
       BtlRecordSizeSet measures with it; BtlAmountRecordsInit puts the digits
       at -11, -3, +5; BtlWindowSpritesSet writes the windows' corners. */
    s16 x0;
    /* +0x0A the first point's y (kind 0 the y offset). */
    s16 y0;
    /* +0x0C the second point's x (for the anchored kinds the rectangle's
       width, sprite_piece_edges_get); nothing reads it for kind 0. */
    s16 x1;
    /* +0x0E the second point's y (the anchored kinds' height). */
    s16 y1;
    /* +0x10 the third point's x (the box's right edge; outside a battle a
       box's or outline's width). */
    s16 x2;
    /* +0x12 the third point's y. */
    s16 y2;
    /* +0x14 the fourth point's x (a quad's last corner, the outline's line's
       end). */
    s16 x3;
    /* +0x16 the fourth point's y. */
    s16 y3;
} SpritePiece;

LAYOUT_SIZE(SpritePiece, 0x18);
LAYOUT_AT(SpritePiece, kind, kind, 0x00);
LAYOUT_AT(SpritePiece, width, width, 0x02);
LAYOUT_AT(SpritePiece, height, height, 0x04);
LAYOUT_AT(SpritePiece, texel_col, texel_col, 0x06);
LAYOUT_AT(SpritePiece, texel_col_texel, texel_col.texel, 0x06);
LAYOUT_AT(SpritePiece, texel_col_col, texel_col.col, 0x06);
LAYOUT_AT(SpritePiece, x0, x0, 0x08);
LAYOUT_AT(SpritePiece, y0, y0, 0x0a);
LAYOUT_AT(SpritePiece, x1, x1, 0x0c);
LAYOUT_AT(SpritePiece, y1, y1, 0x0e);
LAYOUT_AT(SpritePiece, x2, x2, 0x10);
LAYOUT_AT(SpritePiece, y2, y2, 0x12);
LAYOUT_AT(SpritePiece, x3, x3, 0x14);
LAYOUT_AT(SpritePiece, y3, y3, 0x16);

/* One row of a sprite's pose table (the address in a sprite entry's
   pose_table), one row a pose: how many pieces it has and where they begin in
   the piece table. */
typedef struct {
    /* +0x00 the number of pieces the pose draws (sprite_draw draws none above
       0x33). BtlWindowsInit gives each window's outline 2,
       BtlAmountRecordsInit each amount 3. */
    u16 count;
    /* +0x02 the number of the pose's first piece in the piece table. 0 for the
       windows and the amounts. */
    u16 first;
} SpritePoseRow;

LAYOUT_SIZE(SpritePoseRow, 0x04);
LAYOUT_AT(SpritePoseRow, count, count, 0x00);
LAYOUT_AT(SpritePoseRow, first, first, 0x02);

/* One step of a sprite's step list (the address in a BtlRecord's or
   MapRecord's step_list, the sprite data's header word 1): a pose and the
   frames it is shown. */
typedef struct {
    /* +0x00 the pose BtlAnimationStep (and the map's steppers) write into the
       sprite list's copy of the entry. For the amount records it picks a y
       offset from g_BtlAmountYOffsets instead (BtlAmountRecordsStep). */
    u16 pose;
    /* +0x02 the frames the step lasts: BtlAnimationStart and BtlAnimationStep
       copy it to the record's step_length. */
    u16 length;
} SpriteStep;

LAYOUT_SIZE(SpriteStep, 0x04);
LAYOUT_AT(SpriteStep, pose, pose, 0x00);
LAYOUT_AT(SpriteStep, length, length, 0x02);

/* A row of the directory of sprite data, resource 0x1E8 as resource_load reads
   it to g_ResourceReadBuffer (the glossary's directory): for one sprite data
   number (row number less 1, row 0x1E for 0xCE), where its packed sprite data
   and its packed picture lie in the resource and how long they are, in units
   of 4 bytes. MapPartySpriteDataLoad and MapPartySpriteDataReload unpack the
   data; sprite_page_image_add and sprite_page4_image_load (sprite_draw.c) send
   the picture to a sprite page. */
typedef struct {
    /* +0x00 the unpacked sprite data's length in units of 4 bytes:
       MapPartySpriteDataLoad and MapPartySpriteDataReload take
       xmalloc_heap_alloc(it * 4) for it. */
    u16 data_length;
    /* +0x02 the packed sprite data's place in units of 4 bytes from the
       directory's start: inflate_buffer(g_ResourceReadBuffer + it * 4, block)
       (the same two functions). */
    u16 data_place;
    /* +0x04 the picture's length in units of 4 bytes, 64 to a line of the
       0x80-word page: sprite_page_image_add and sprite_page4_image_load size
       the run of page lines they take by it. */
    u16 image_length;
    /* +0x06 the packed picture's place in units of 4 bytes from the
       directory's start (sprite_page_image_add, sprite_page4_image_load unpack
       it from there). */
    u16 image_place;
} SpriteDirectoryRow;

LAYOUT_SIZE(SpriteDirectoryRow, 0x08);
LAYOUT_AT(SpriteDirectoryRow, data_length, data_length, 0x00);
LAYOUT_AT(SpriteDirectoryRow, data_place, data_place, 0x02);
LAYOUT_AT(SpriteDirectoryRow, image_length, image_length, 0x04);
LAYOUT_AT(SpriteDirectoryRow, image_place, image_place, 0x06);

/* A row of a sprite data's animation table (the sprite data's header word 0, a
   record's animation_table): one for each animation number, the animation's
   count of steps with its hold bit, and its first step in the step list
   (SpriteStep, the 4-byte steps of the header word 1). The same shape as the
   pose table's row, SpritePoseRow (pass_battle.json), and named beside it, but
   other memory. */
typedef struct {
    /* +0x00 the animation's count of steps; bit 0x8000 holds the last step: at
       the end the steppers (MapPartyAnimationStep, MapObjectsAnimationStep,
       MoveChrAction, BtlAnimationStep) step back by one when it is set and
       start the animation again otherwise. MapAnimationStart and
       BtlAnimationStart make the record's end first + count. */
    u16 count;
    /* +0x02 the number of the animation's first step in the step list
       (MapAnimationStart, BtlAnimationStart, ShopAnimationSet put the record's
       step there). */
    u16 first;
} SpriteAnimationRow;

LAYOUT_SIZE(SpriteAnimationRow, 0x04);
LAYOUT_AT(SpriteAnimationRow, count, count, 0x00);
LAYOUT_AT(SpriteAnimationRow, first, first, 0x02);

/* SpriteTexelRecord: one 12-byte texel record of a sprite data block (the
   records lie from the block's header word 4 to word 5, places counted in 4
   bytes from the block's start): which sprite page one picture of the sprite
   is in, and where in it. A piece (the 0x18-byte records of the piece table)
   names one by its number at +6; SpriteEntry.texel_records holds the address
   of the first. The page loaders point a block's records at the page slot and
   the rows they gave the block's image; the battle's amount records have 21 of
   their own at the address in g_BtlAmountTexelRecords. */
typedef struct {
    /* +0x00 nothing reads it: BtlAmountRecordsInit writes 0 in its 21 records;
       in a sprite data block it holds what the file holds. */
    u16 unk_00;
    /* +0x02 the number of the sprite page (0 to 11) the picture is in:
       sprite_piece_draw gives the plain and the scaled sprite the texture page
       word g_SpritePageTPages[page] and hands the number itself to
       sprite_quad_draw and sprite_mirror_draw. Written by the loaders when
       they place a block's image: sprite_page4_image_load 4,
       sprite_page_image_add 5 + the column, BtlPartyImageLoad 4 + the column,
       sprite_reset 10 for resource 2's three blocks; BtlAmountRecordsInit 10. */
    u16 page;
    /* +0x04 nothing reads it: BtlAmountRecordsInit writes 8 in its records,
       whose pictures are 8 by 8 (the register's reading of
       g_BtlAmountTexelRecords: 'page 10, 8 by 8'), so it is named the
       picture's width; the drawers take a piece's size from the piece (+2,
       +4), not from here. */
    u16 width;
    /* +0x06 nothing reads it: BtlAmountRecordsInit writes 8 (see width), named
       the picture's height. */
    u16 height;
    /* +0x08 the picture's u in its page, in pixels: sprite_piece_draw hands it
       to sprite_quad_draw, sprite_scale_draw and sprite_mirror_draw as 16 bits
       and to sprite_plain_draw as its low byte. BtlAmountRecordsInit writes 8
       * n for digit picture n. */
    u16 u;
    /* +0x0A the picture's v in its page, read as u is. The loaders add the
       line their image was placed at (v += row * 8: sprite_page4_image_load,
       sprite_page_image_add, BtlPartyImageLoad); BtlAmountRecordsInit writes
       0. */
    u16 v;
} SpriteTexelRecord;

LAYOUT_SIZE(SpriteTexelRecord, 0x0c);
LAYOUT_AT(SpriteTexelRecord, unk_00, unk_00, 0x00);
LAYOUT_AT(SpriteTexelRecord, page, page, 0x02);
LAYOUT_AT(SpriteTexelRecord, width, width, 0x04);
LAYOUT_AT(SpriteTexelRecord, height, height, 0x06);
LAYOUT_AT(SpriteTexelRecord, u, u, 0x08);
LAYOUT_AT(SpriteTexelRecord, v, v, 0x0a);

/* SpritePageCorner: one row of g_SpritePageCorners (0x800A7F88, twelve rows,
   never written): where a sprite page (0x80 VRAM words wide, 256 lines) begins
   in video memory: slots 0 to 4 at x = 0x140 + 0x80 * k, y = 0; 5 to 9 the
   same at y = 0x100; 10 and 11 at x = 0x3C0, y = 0 and 0x100. sprite_init
   makes each page's texture page word (g_SpritePageTPages) from it; the page
   loaders send their images there. */
typedef struct {
    /* +0x00 the page's x in video memory, in 16-bit words: sprite_init's
       GetTPage (and x + 0x40 for D_800DDD50) and the loaders' rect.x
       (sprite_page_image_send, sprite_page4_image_load, sprite_page_image_add,
       sprite_page_list_load, BtlDataLoad, BtlPartyImageLoad,
       sprite_page_effect_load). Nothing writes it. */
    u16 x;
    /* +0x02 the page's y in video memory, 0 or 0x100; the loaders that place
       an image in units of 8 lines add row * 8 (rect.y). Nothing writes it. */
    u16 y;
} SpritePageCorner;

LAYOUT_SIZE(SpritePageCorner, 0x04);
LAYOUT_AT(SpritePageCorner, x, x, 0x00);
LAYOUT_AT(SpritePageCorner, y, y, 0x02);

/* SpriteDataList: the 12-byte list at D_80011054 (data_D_80011054.c) of where
   sprite_reset unpacks resource 2's three sprite data blocks: g_SpriteData0,
   g_SpriteData1 and g_SpriteData2. sprite_reset copies the whole list to its
   stack (dst = D_80011054) and takes entry i for the block at the resource's
   word 1 + 2i. */
typedef struct {
    /* +0x00..+0x0B the three blocks' addresses: g_SpriteData0 (resource 2's
       word 1), g_SpriteData1 (word 3) and g_SpriteData2 (word 5). sprite_reset
       inflates each block there (inflate_buffer) and sets the page of each of
       its texel records to 10. Only the definition writes them. */
    u8 * data[3];
} SpriteDataList;

LAYOUT_SIZE(SpriteDataList, 0x0c);
LAYOUT_AT(SpriteDataList, data, data, 0x00);

/* SpriteDataHead: the head of a sprite data block (g_SpriteData0 to 2, the
   blocks of resource 2 sprite_reset unpacks; the battle's and the map's
   sprite data): six 16-bit words, each the place of one of the block's
   tables counted in units of 4 bytes from the block's start. The loaders
   make a record's addresses from them (block + word * 4):
   MapRecordSpriteDataSet (map_sprite.c), BtlPartyLoad and BtlEnemyLoad
   (btl_init.c), BtlRecordSpriteDataSet (btl_show_frame.c), ShopObjectInit,
   MenuRecordsInit, MessageRecordsInit and MenuUseShowPlacesPlay read
   words 0 to 4;
   the page loaders (sprite_page4_image_load, sprite_page_image_add,
   BtlPartyImageLoad) and sprite_reset read words 4 and 5. All read them
   as this type; ShopObjectInit, MenuRecordsInit, MessageRecordsInit and
   MenuUseShowPlacesPlay's word 0 through the members' addresses
   (*(u16 *)&bank->texel_records), which their matches need. */
typedef struct {
    /* +0x00 word 0: the place of the animation table (SpriteAnimationRow),
       the record's animation_table. */
    u16 animation_table;
    /* +0x02 word 1: the place of the step list (SpriteStep), the record's
       step_list. */
    u16 step_list;
    /* +0x04 word 2: the place of the pose table (SpritePoseRow), the sprite
       entry's pose_table. */
    u16 pose_table;
    /* +0x06 word 3: the place of the piece table (SpritePiece), the sprite
       entry's piece_table. */
    u16 piece_table;
    /* +0x08 word 4: the place of the first 12-byte texel record
       (SpriteTexelRecord), the sprite entry's texel_records. sprite_reset
       and the page loaders walk the records from it. */
    u16 texel_records;
    /* +0x0A word 5: the place where the texel records end: the loaders count
       (texel_records_end - texel_records) / 3 records (sprite_reset,
       sprite_page4_image_load, sprite_page_image_add, BtlPartyImageLoad). */
    u16 texel_records_end;
} SpriteDataHead;

LAYOUT_SIZE(SpriteDataHead, 0x0c);
LAYOUT_AT(SpriteDataHead, animation_table, animation_table, 0x00);
LAYOUT_AT(SpriteDataHead, step_list, step_list, 0x02);
LAYOUT_AT(SpriteDataHead, pose_table, pose_table, 0x04);
LAYOUT_AT(SpriteDataHead, piece_table, piece_table, 0x06);
LAYOUT_AT(SpriteDataHead, texel_records, texel_records, 0x08);
LAYOUT_AT(SpriteDataHead, texel_records_end, texel_records_end, 0x0a);

#endif
