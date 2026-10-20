/* Lunar SSSC — map_party: the party's records and twins made, its box and
   its spot. The first split had it in scripting_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_MapTwinSpriteDataNumbers */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"

extern u8 g_MapTwinSpriteDataNumbers[];
s32 MapPartySpriteDataSet();                                  /* extern */
extern s32 g_MapPartyTwins;
extern u16 g_MapPartyKind;
void MapPartyBoxSet(void);
s32 MapPartyRecordsInit();                                  /* extern */
s32 MapPartySpotSet(s32, s32);                                 /* extern */
extern s32 g_PartyIds;
extern u16 g_MapPartyWalkStepCount;
extern s32 g_MapPartyRecords;
/* far_syms: g_MapTwinSpriteDataNumbers (target reaches it %hi/%lo) */
void MapPartySpriteDataLoad(u8 skip_load);  /* extern */
s32 MapPartySpriteDataMissingCount(void);  /* extern */

/* func_80064898 */
s32 MapPartyRecordsInit() {
    s32 i;
    s32 offset;
    s32 j;

    if (MapPartySpriteDataMissingCount() != 0) {
        MapPartySpriteDataLoad(0);
    }
    i = 0;
    offset = i;
    for (; i < 6; i++) {
        if (*(u16 *)(g_PartyIds + i * 2) == 0) {
            *(u8 *)(offset + g_MapPartyRecords) = 0;
            *(u8 *)(offset + g_MapPartyTwins) = 0;
        }
        else {
            MapPartyRecordInit(i, *(u16 *)(g_PartyIds + i * 2));
        }
        offset += 0x3C;
    }
    MapPartyBoxSet();
    if (g_MapPartyKind == 2) {
        j = 0x3C;
        for (i = 1; i < 6; i++) {
            *(u8 *)(j + g_MapPartyRecords) = 0;
            *(u8 *)(j + g_MapPartyTwins) = 0;
            j += 0x3C;
        }
    }
}

extern u16 g_MapPartyWalkStepCount;
extern s8 g_MapPartyBoxDx;
extern s8 g_MapPartyBoxDy;
extern u8 g_MapPartyBoxHalfWidth;
extern u8 g_MapPartyBoxHalfHeight;

/* func_8006497C */
void MapPartyBoxSet(void) {
    if (g_MapPartyKind == 0) {
        g_MapPartyBoxHalfWidth = 6;
        g_MapPartyBoxHalfHeight = 4;
        g_MapPartyBoxDx = 0;
        g_MapPartyBoxDy = 0;
        g_MapPartyWalkStepCount = 1;
        return;
    }
    g_MapPartyBoxDy = -1;
    g_MapPartyBoxHalfWidth = 8;
    g_MapPartyBoxHalfHeight = 5;
    g_MapPartyBoxDx = 0;
    g_MapPartyWalkStepCount = 2;
}

void MapAnimationPoseSet(void *record, s16 pose, u16 facing);  /* extern */
void MapRecordInit(void *record, s32 kind, s32 mode, s32 sprite_data_number);  /* extern */

/* MapPartyRecordInit -- the name is the splitter's: this SETS UP ONE
   PARTY SLOT'S FIELD ACTOR AND ITS TWIN.
   place is the slot (0..5), chr_id the member id in it. Callers:
   MapPartyRecordsInit (every non-empty slot, with the slot's id from
   g_PartyIds) and MoveChrPartyJoin (lns_move.c: the slot a
   member has just been put in).

   Row place of g_MapPartyRecords (0x3C each): +0 = the LOW BYTE of the slot's id,
   ((u16 *)g_PartyIds)[place] (the row's live byte), +1 = 0, then
   MapRecordInit(row, kind, 0, sheet): kind 1 for an chr_id below 0x1F
   and 2 from 0x1F up (+0xE = 0x2000 / 0x2100 there), sheet = chr_id, plus
   0xF when g_MapPartyKind == 0. The add is not conditioned on the id; the
   loaders MapPartySpriteDataLoad / MapPartySpriteDataReload load an `id + 0xF` sheet
   only for ids below 0x1F. The row is then put at x = 0xA0, +0xA = 0x78,
   y = 0x78 and sent MapAnimationPoseSet(row, 1, 1).
   Row place of g_MapPartyTwins, the twin, gets the row's 0x3C bytes, is rendered
   again as MapRecordInit(twin, 1, 0, twinsheet) -- twinsheet =
   g_MapTwinSpriteDataNumbers[id16 - 1] for an chr_id below 0x1F (id16 = the slot's whole u16
   in g_PartyIds, not chr_id), 0xD otherwise -- and is sent
   MapAnimationPoseSet(twin, 1, 1). Last, the twin's live byte is
   cleared when g_MapPartyKind == 0 or the sheet (chr_id AFTER the + 0xF) is 0x1F
   or more: the twin stays on only with g_MapPartyKind != 0 and an id below
   0x1F. No RNG draw and no frame wait of its own.

   Shape: every access is the row written out as ((T *)g_MapPartyRecords)[place] --
   no pointer local, no offset local. The index * 0x3C is one quantity up
   to the struct copy (cse) and is computed again after it, because the
   copy's loop ends the block: that is the target's second sll / subu /
   sll. The order inside the two arms of the first `if` matters (kind = 1
   first, so the constant 1 is in $a1 when the id's index is scaled -- the
   target's `sllv $v0, $s2, $a1`; twinsheet before kind in the else arm),
   and so does the order of the last test's two halves. The record has to
   be word-aligned (s32 members) for the four-word copy loop with a
   three-word tail. */
/* func_800649DC */
void MapPartyRecordInit(s32 place, s32 chr_id)
{
    s32 kind;
    s32 twinsheet;

    if (chr_id < 0x1F) {
        kind = 1;
        twinsheet = g_MapTwinSpriteDataNumbers[((u16 *)g_PartyIds)[place] - 1];
    } else {
        twinsheet = 0xD;
        kind = 2;
    }
    if (g_MapPartyKind == 0) {
        chr_id += 0xF;
    }
    ((MapRecord *)g_MapPartyRecords)[place].on_kind.on = ((u16 *)g_PartyIds)[place];
    ((MapRecord *)g_MapPartyRecords)[place].hold_definition.hold = 0;
    MapRecordInit(&((MapRecord *)g_MapPartyRecords)[place], kind, 0, chr_id);
    ((MapRecord *)g_MapPartyRecords)[place].sprite.x = 0xA0;
    ((MapRecord *)g_MapPartyRecords)[place].sprite.y = 0x78;
    AS(u16, ((MapRecord *)g_MapPartyRecords)[place].sprite.depth) = 0x78;
    MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[place], 1, 1);
    ((MapRecord *)g_MapPartyTwins)[place] = ((MapRecord *)g_MapPartyRecords)[place];
    MapRecordInit(&((MapRecord *)g_MapPartyTwins)[place], 1, 0, twinsheet);
    MapAnimationPoseSet(&((MapRecord *)g_MapPartyTwins)[place], 1, 1);
    if (g_MapPartyKind == 0 || chr_id >= 0x1F) {
        ((MapRecord *)g_MapPartyTwins)[place].on_kind.on = 0;
    }
}

/* far_syms: g_MapTouchedObject (target reaches it %hi/%lo) */

void MapPartyRingFill(u16, u16, s32, u8);                  /* extern */
s32 MapPartyRingFollow();                                  /* extern */
extern s16 g_MapTouchedObject;

/* func_80064B8C */
s32 MapPartySpotSet(s32 spot, s32 remake_records) {
    if (remake_records != 0) {
        MapPartyRecordsInit();
    }
    MapPartyRingFill(AS(s32, ((MapPartyRingEntry *)spot)->x), AS(s16, ((MapPartyRingEntry *)spot)->y), 0, ((MapPartyRingEntry *)spot)->facing);
    MapPartyRingFollow();
    g_MapTouchedObject = -1;
}

/* func_80064BE0 */
void MapPartyInit(s32 spot) {
    MapPartySpotSet(spot, 1);
    MapPartySpriteDataSet();
}
