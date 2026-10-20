/* Lunar SSSC — map_btl: the battle the map starts: the touched object found
   and what it has run, and MapBtlRun (the map left, BtlRun, the map back).
   The first split had it in battle_b.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "pad.h"
#include "menu.h"
#include "map.h"
#include "sprite.h"

extern u16 g_MapIdleCount;
/* 4-byte word: read as `lw` for value tests and as `lhu` (low half) where
   it feeds MenuPageDisp — one s32 declaration covers both via a cast. */
/* far_syms @map_btl.c: MenuUseEnd stores it with a WORD store via
   lui $at, while TU-A treats the same object as a u16 reached %gp_rel. */
extern s32 g_MenuPage;

extern void sprite_run_off(s32 first, s32 count);
extern void mem_clear(void *destination, u32 byte_count);
extern s32 MusicPlay(u8 id, s32 unused, u8 vol);

extern void MapAnimationPoseSet(MapObject *record, s32 pose, s32 facing);
extern void ScriptLabelRun(s32 label);

extern s32 g_PartyIds;     /* base of the u16 id list scanned by MenuUsePartyRun */
extern u8 g_BtlSlotIds[];    /* per-slot id byte array */
extern s32 g_MapPartyRecords;     /* base pointer (gp-relative word) */
extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table 2 */
extern s32 g_MapObjectDefinitions;     /* base of a 0x26-stride record table (gp word) */
extern s32 g_MapObjects;     extern s32 g_MenuOnePageRunning;
/* GCC 2.8.1 emits its 4-word block-move loop + tail word for a 0x24-byte
   struct assignment; used by MenuBtlPlacesDisp to copy BattleEntity+0x48. */
extern s32 g_MapLayersMask;
extern s16 g_MapCol;
extern s16 g_MapColSecond;
extern s16 g_MapColThird;
extern s16 g_MapTouchedObject;
extern u16 g_MapPartyBlinkCount;
extern void MapReload(void);
extern void MapBgChangeStoryBitsCopy(void);
extern void MapViewStep(s32, s32);
/* Script-side movement projection: steps (*px, *py) `dist` tiles along `dir`
   using the actor's probe rect at `probe`, returns the facing actually used. */
extern s32 g_PadActuatorList;      /* far_syms.txt: init guard / table pointer slot */
extern u8 g_PadActuatorRestFlags;       /* far_syms.txt: lui $at + sb */
extern s16 g_PadActuatorListWait;      /* far_syms.txt: lui $at + sh */
extern s32 layer_col_set(s32, s32, s32);
/* far_syms: g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */
extern s8 g_MapPartyBoxDx;
extern s8 g_MapPartyBoxDy;
extern s32 g_MapProbeBoxX;
extern s32 g_MapProbeBoxY;
extern s32 g_MapProbeBoxHalfWidth;
extern s32 g_MapProbeBoxHalfHeight;
s32 MapBoxTouchGet(s32, s32, s8 *);
s32 MapAttributeLineCheck();

extern u8 g_MapPartyBoxHalfWidth;
extern u8 g_MapPartyBoxHalfHeight;

/* The field's touch scan: MapObjectsThink calls it once per field frame, after
   the map objects' think pass. It does nothing while a touch is pending
   (g_MapTouchedObject >= 0) or the grace counter g_MapPartyBlinkCount is running (0x12C
   frames after MapObjectTouchRun's fight returns 2; the party blinks meanwhile).

   Otherwise the lead actor's probe rectangle is published for MapBoxTouchGet
   -- centre g_MapProbeBoxX / g_MapProbeBoxY = record 0 of g_MapPartyRecords's x / ground y
   plus the s8 offsets g_MapPartyBoxDx / g_MapPartyBoxDy, half sizes g_MapProbeBoxHalfWidth /
   g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfWidth / g_MapPartyBoxHalfHeight -- and the 0x80 map objects (0x60-byte
   rows of g_MapObjects) are scanned in order. A row counts when its kind (+0)
   is 4, 5 or 6 and its behaviour (+0x3D & 0x3F) is not 3. Its box (+0x50) is
   copied and both half sizes grown by 8 x byte +0x13 of its 0x26-byte
   definition (g_MapObjectDefinitions[row +1]); a non-zero MapBoxTouchGet(x + box.dx,
   y + box.dy, &box) is a touch, which MapAttributeLineCheck(the
   row's position, the lead's position, row +0x3C) must confirm. The first
   confirmed row's index goes to g_MapTouchedObject and the scan stops;
   MapObjectTouchRun acts on it (definition byte +0xB is run as an event for
   kind 4 and as an encounter for the others).

   Shape: one walked row pointer;
   the second register at row + 0x3C is the loop optimiser's. `pos` is
   taken in the first call's block, before the call: that is what puts it
   in a call-saved register although the scheduler sinks its `addiu` below
   the call. The step is `p++, i++`; `lead` is read once. */
/* func_8007AB64 */
void MapObjectTouchFind(void)
{
    MapObject *p;
    MapObject *lead;
    u8 *rec;
    s32 i;
    MapBox box;
    s16 *pos;

    if (g_MapTouchedObject >= 0) {
        return;
    }
    if (g_MapPartyBlinkCount != 0) {
        return;
    }
    p = (MapObject *)g_MapObjects;
    lead = (MapObject *)g_MapPartyRecords;
    g_MapProbeBoxHalfWidth = g_MapPartyBoxHalfWidth;
    g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfHeight;
    g_MapProbeBoxX = lead->record.sprite.x + g_MapPartyBoxDx;
    g_MapProbeBoxY = lead->record.sprite.depth + g_MapPartyBoxDy;
    for (i = 0; i < 0x80; p++, i++) {
        if (p->record.on_kind.kind < 4 || p->record.on_kind.kind >= 7) {
            continue;
        }
        if ((p->number & 0x3F) == 3) {
            continue;
        }
        pos = &p->record.sprite.x;
        rec = (u8 *)(g_MapObjectDefinitions + p->record.hold_definition.definition * 0x26);
        box = p->box;
        box.half_width += rec[0x13] * 8;
        box.half_height += rec[0x13] * 8;
        if ((MapBoxTouchGet(pos[0] + box.dx, pos[1] + box.dy, (s8 *)&box) & 0xFFFF) == 0) {
            continue;
        }
        if (MapAttributeLineCheck(pos, g_MapPartyRecords + 6, p->layer)) {
            g_MapTouchedObject = i;
            return;
        }
    }
}

extern s8 g_MapBtlWinForced;
extern void MapObjectTouchApproach(void);
extern s32 MapBtlRun(s32, s32, u16, s32, s32, s32, s32);
extern void MapObjectTouchRemove(s32 view_follow, s32 second_view_follow);

/* func_8007ACD8 */
void MapObjectTouchRun(void)
{
  u8 *slot;
  u8 *rec;
  s32 i;
  s32 out;
  s32 id;
  u8 guest;
  s32 ret;
  if (g_MapTouchedObject >= 0)
  {
    slot = (u8 *) (g_MapObjects + (g_MapTouchedObject * 0x60));
    rec = (u8 *) (g_MapObjectDefinitions + (slot[1] * 0x26));
    if (rec[0xB] == 0)
    {
      g_MapTouchedObject = -1;
      return;
    }
    MapObjectTouchApproach();
    if (slot[0] == 4)
    {
      ScriptLabelRun(rec[0xB]);
      g_MapIdleCount = 0;
      g_MapTouchedObject = -1;
      return;
    }
    mem_clear(g_BtlSlotIds, 6);
    out = 0;
    guest = 0xC;
    for (i = 0; i < 6; i++)
    {
      id = *((u16 *) (g_PartyIds + (i * 2)));
      if (id == 0)
      {
        break;
      }
      if (id < 0x1F)
      {
        if (id < 0xC)
        {
          g_BtlSlotIds[out] = id;
          out++;
        }
        else
          if (id == guest)
        {
          g_BtlSlotIds[5] = id;
        }
      }
    }

    ret = MapBtlRun(rec[0xB], 0, 0, 0, 1, 1, 1);
    if (g_MapBtlWinForced != 0)
    {
      ret = 0;
    }
    if (ret == 0)
    {
      MapObjectTouchRemove(1, 1);
      g_MapIdleCount = 0;
      g_MapTouchedObject = -1;
      return;
    }
    if (ret == 2)
    {
      g_MapPartyBlinkCount = 0x12C;
    }
    g_MapIdleCount = 0;
    g_MapTouchedObject = -1;
  }
}
/* far_syms: g_MapActuatorC8List g_MoveChrLiveBits g_BtlPartyPlaceXCells g_BtlPartyPlaceYCells g_BtlPartyPreset0Kinds g_BtlPartyPreset1Kinds g_BtlPartyPreset2Kinds g_BtlPartyPreset0Numbers g_BtlPartyPreset1Numbers g_BtlPartyPreset2Numbers g_BtlSlotIds g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_MenuPage g_MenuOnePageRunning */
extern u8 g_MapActuatorC8List[];
extern u8 g_BtlPartyPlaceXCells[];
extern u8 g_BtlPartyPlaceYCells[];
extern u8 g_BtlPartyPreset0Kinds[];
extern u8 g_BtlPartyPreset1Kinds[];
extern u8 g_BtlPartyPreset2Kinds[];
extern u8 g_BtlPartyPreset0Numbers[];
extern u8 g_BtlPartyPreset1Numbers[];
extern u8 g_BtlPartyPreset2Numbers[];
extern s32 g_ChrPlacePresetRecords;
extern s32 g_MoveChrLiveBits;
extern s32 g_MapPartyTwins;
extern s32 g_MapObjectTwins;
extern u16 g_MapTouchedObjectKeptX[2];
extern u8 g_MapTouchedObjectKeptFacing;
extern u16 g_MapTouchedObjectKeptClass;
extern void sound_btl_seek(void);
extern void MapFadeOut(s32, s32, s32, s32, s32);
extern void xmalloc_heap_reset(void);
extern void bg_slot_reset(void);
extern void SoundMapSet(s8 map_number);
extern s32 BtlRun();
extern void PartyLevelLeastSet(void);
extern s32 layer_col_set(s32, s32, s32);
extern void sprite_init_copy(void);
extern s32 MapLayerOrderSet(void);
extern void MessageReset(void);
extern void MapDataReload(void);
extern void FaceListIdMove(s32 a, s32 b);
extern void MapFadeIn(s32, s32, s32, s32);
extern u16 MenuNoWalkRun(void);
extern void GameEnter(s32);
extern void MapSpriteDataReload(s32 a);
extern void MoveChrSpriteDataSet(void);

/* Runs field encounter `btl_number` and settles its outcome (called for a
   touched map enemy by MapObjectTouchRun and from the script by ScriptCommandMapBtlRun).

   Before the fight, the eight bytes of each party slot's record
   (g_ChrPlacePresetRecords[id * 8], ids from g_BtlSlotIds, ids >= 0x1F skipped, the list
   ends at the first 0) are copied out to eight parallel byte tables at
   g_BtlPartyPlaceXCells..g_BtlPartyPreset2Numbers, the first byte lowered by `place_x_offset`. Then
   MapFadeOut(g_MapLayersMask | 0x130, 0x19, -1, view_follow, second_view_follow), g_MoveChrLiveBits
   saved and cleared, the music request MusicPlay(music_number, 0, 0), and
   ret = BtlRun(btl_number). Encounters 0xF9 / 0xFE, and any result other
   than 2 while g_MapBtlWinForced is set, count as result 0.

   Result 1 with `label` == 0: music 0x17, the lead bytes of the six party
   records (g_MapPartyRecords, g_MapPartyTwins), the id list g_PartyIds and the 0x80
   object records (g_MapObjects, g_MapObjectTwins) cleared, g_MapCol/2/4 set to
   -0x38, then MenuNoWalkRun under g_MenuOnePageRunning = 1 with its result handed
   to GameEnter; returns 1. Otherwise g_MoveChrLiveBits is restored and,
   unless `return_mode` is 2, MapSpriteDataReload(0) / MapBgChangeStoryBitsCopy /
   MapViewStep(view_follow, second_view_follow) run; on result 2 the touched object
   g_MapTouchedObject gets its saved x, y (both copies), facing and the 0x6000
   bits of +0xE back (g_MapTouchedObjectKeptX, +2, g_MapTouchedObjectKeptFacing, g_MapTouchedObjectKeptClass) and is
   re-dispatched. `return_mode` == 0 ends with MapFadeIn(g_MapLayersMask | 0x130,
   0x19, view_follow, second_view_follow). Returns the result.

   Source shapes the match needs: `music_number` is an int read once, so it stays
   in its stack home and the u8 conversion at the call is the target's
   `lbu 0x50($sp)`; the clear loops count in their own variable (the first
   loop's counter is in $a2, theirs in $a0); the saved x / y are one
   two-element array, so the y is loaded again after the store between;
   and the dispatch argument re-reads the facing byte just stored, which
   CSE forwards as the `andi` after the store. */
/* Two stores to e are pointer accesses through their members' addresses,
   the y (*(u16 *)&...record.sprite.y) and the ground y (record.sprite.depth):
   written as members of MapObject (plain, AS(u16, ...), or through s16
   pointers), the second no longer loads g_MapTouchedObjectKeptX[1] again
   after the first (GCC takes a member store as unable to touch the fixed
   array) and the code no longer matches. The rest of e is members. */
/* func_8007AE58 */
s32 MapBtlRun(s32 btl_number, s32 place_x_offset, u16 label, s32 return_mode, s32 view_follow, s32 second_view_follow, s32 music_number)
{
    s32 i;
    s32 j;
    s32 id;
    s32 saved;
    s32 ret;
    u8 *e;
    u16 r;

    g_PadActuatorList = (s32)g_MapActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    for (i = 0; i < 5; i++) {
        id = g_BtlSlotIds[i];
        if (id == 0) {
            break;
        }
        if (id < 0x1F) {
            g_BtlPartyPlaceXCells[i] = ((u8 *)(id * 8 + g_ChrPlacePresetRecords))[0] - place_x_offset;
            g_BtlPartyPlaceYCells[i] = ((u8 *)(id * 8 + g_ChrPlacePresetRecords))[1];
            g_BtlPartyPreset0Kinds[i] = ((u8 *)(id * 8 + g_ChrPlacePresetRecords))[2];
            g_BtlPartyPreset0Numbers[i] = ((u8 *)(id * 8 + g_ChrPlacePresetRecords))[3];
            g_BtlPartyPreset1Kinds[i] = ((u8 *)(id * 8 + g_ChrPlacePresetRecords))[4];
            g_BtlPartyPreset1Numbers[i] = ((u8 *)(id * 8 + g_ChrPlacePresetRecords))[5];
            g_BtlPartyPreset2Kinds[i] = ((u8 *)(id * 8 + g_ChrPlacePresetRecords))[6];
            g_BtlPartyPreset2Numbers[i] = ((u8 *)(id * 8 + g_ChrPlacePresetRecords))[7];
        }
    }
    sound_btl_seek();
    MapFadeOut(g_MapLayersMask | 0x130, 0x19, -1, view_follow, second_view_follow);
    saved = g_MoveChrLiveBits;
    g_MoveChrLiveBits = 0;
    xmalloc_heap_reset();
    bg_slot_reset();
    SoundMapSet(0);
    MusicPlay(music_number, 0, 0);
    ret = BtlRun(btl_number);
    bg_slot_reset();
    PartyLevelLeastSet();
    if (btl_number == 0xF9 || btl_number == 0xFE || (ret != 2 && g_MapBtlWinForced != 0)) {
        ret = 0;
    }
    layer_col_set(0, g_MapLayersMask | 0x80130, -1);
    sprite_init_copy();
    MapLayerOrderSet();
    if (return_mode != 2 && (label != 0 || ret != 1)) {
        MapReload();
    }
    MessageReset();
    sprite_run_off(0, 0x96);
    if (ret == 1 && label == 0) {
        MapDataReload();
        MusicPlay(0x17, 0, 1);
        FaceListIdMove(0xC, 0);
        for (j = 0; j < 6; j++) {
            ((MapRecord *)g_MapPartyRecords)[j].on_kind.on = 0;
            ((MapRecord *)g_MapPartyTwins)[j].on_kind.on = 0;
            ((u16 *)g_PartyIds)[j] = 0;
        }
        for (j = 0; j < 0x80; j++) {
            ((MapObject *)g_MapObjects)[j].record.on_kind.on = 0;
            ((MapRecord *)g_MapObjectTwins)[j].on_kind.on = 0;
        }
        g_MoveChrLiveBits = 0;
        g_MapCol = g_MapColSecond = g_MapColThird = -0x38;
        MapFadeIn(0x30, 5, view_follow, second_view_follow);
        g_MenuPage = 0x14;
        g_MenuOnePageRunning = 1;
        r = MenuNoWalkRun();
        g_MenuOnePageRunning = 0;
        GameEnter(r);
        return ret;
    }
    xmalloc_heap_reset();
    g_MoveChrLiveBits = saved;
    if (return_mode != 2) {
        MapSpriteDataReload(0);
        MapBgChangeStoryBitsCopy();
        MapViewStep(view_follow, second_view_follow);
        if (ret == 2 && g_MapTouchedObject >= 0) {
            e = (u8 *)(g_MapObjects + g_MapTouchedObject * 0x60);
            ((MapObject *)e)->record.sprite.x = g_MapTouchedObjectKeptX[0];
            *(u16 *)&((MapObject *)e)->record.sprite.y = g_MapTouchedObjectKeptX[1];
            *(u16 *)&((MapObject *)e)->record.sprite.depth = g_MapTouchedObjectKeptX[1];
            ((MapObject *)e)->facing = g_MapTouchedObjectKeptFacing;
            ((MapObject *)e)->record.sprite.flags = g_MapTouchedObjectKeptClass | (((MapObject *)e)->record.sprite.flags & 0x9FFF);
            MapAnimationPoseSet((MapObject *)e, 2, ((MapObject *)e)->facing);
        }
    } else {
        MapSpriteDataReload(1);
    }
    MoveChrSpriteDataSet();
    if (return_mode == 0) {
        MapFadeIn(g_MapLayersMask | 0x130, 0x19, view_follow, second_view_follow);
    }
    return ret;
}
