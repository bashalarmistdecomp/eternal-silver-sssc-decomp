/* Lunar SSSC — map_change: what the map changes on request: another map
   (MapChange, GameEnter), the party's place, the bg change, the attributes'
   kinds 4 to 6, the map's colour (with the developers' ColMix) and the
   party's hp taken. The first split had it in scripting_c.c. Every function
   is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_MapBgChange (target reaches it %hi/%lo) */
/* far_syms: g_MapFadeState_s */

#include "include_asm.h"
#include "psyq_types.h"
#include "script.h"
#include "pad.h"
#include "game.h"
#include "chr.h"
#include "map.h"
#include "sprite.h"

extern u8 g_MapBgChangeNumber;
extern u16 g_MapPartyBlinkCount;
s32 ColMix(u8, s32);
extern u8 g_MapBgChange[];
void MapBgChangeStart(s16, u8);
s32 layer_col_fade_start(s32, s32, s32, s32, s32 *);               /* extern */
s32 layer_col_set(s32, s32, s32 *);
s32 MapColFadeStart(s32, s32);
extern long HWD0;
extern long VWD0;
extern s16 g_MapPlaceName;
extern s16 g_PadActuatorListWait;
extern s32 g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
extern u8 g_PartyCount;
/* g_MapCol: declared once, as MapCol, beside that typedef below (an
   unsized array here would make the object far for the whole TU) */
extern s32 g_PartyIds;
extern s32 g_ChrRecords;
extern u16 g_MapPartyKind;
extern s32 g_MapLayersMask;
extern s16 g_MapViewKeptX;
extern s16 g_MapViewKeptY;
/* far_syms: g_MapFadeState_s g_MapBgChange D_800B8E3C */
extern long HWD0, VWD0;		 
extern u8 *g_MapEnterTable;
extern u8 g_MapFadeState_s;
extern s32 g_MapTalkObjectDefinition;
extern s8 D_800B8E0C;
extern s16 g_MapLimitLeft;
extern s16 g_MapLimitTop;
extern s16 g_MapLimitRight;
extern s16 g_MapLimitBottom;
extern s16 D_800B8E3C;
s32 SoundPlay(u16, s32);
s32 MapSeek(s32 map_number, s32 music_number);
void MapFadeOut(s32, s32, s32, s32, s32);
void MapFadeIn(s32, s32, s32, s32);
void MapSet(s32, u8, s32);
void MapBgChangeStoryBitsCopy(void);  /* TU defines this below */
void MessageReset(void);  /* extern */
void MapPadDirectionsSet(s32);
void FaceListPartySet(void);
void MapSpriteDataFree();
void StoryBits3E8Clear(void);
void MapObjectsInit(s32);
s32 MapPartySpotSet(s32, s32);
s32 MapViewStep(s32, s32);

/* func_8005922C */
void MapChange(idx, src, run, view_follow, second_view_follow, music_number)
u16 idx;
MapEnterRow *src;
s32 run;
s32 view_follow;
s32 second_view_follow;
s32 music_number;
{
    MapEnterRow e;
    MapPartyRingEntry pos;
    s32 mode;

    if (idx != 0) {
        e = ((MapEnterRow *)g_MapEnterTable)[idx];
    } else {
        e = *src;
    }
    if (e.sound != 0) {
        SoundPlay(e.sound, 3);
    }
    if (g_MapFadeState_s == 0) {
        mode = -1;
        if (e.fade == 1) {
            mode = -2;
        }
        if (e.fade == 2) {
            g_MapFadeState_s = 2;
        }
        MapSeek(e.map_number, music_number);
        MapFadeOut(g_MapLayersMask | 0x130, 0x19, mode, view_follow, second_view_follow);
    }
    MapSet(e.map_number, music_number, 0);
    MapBgChangeStoryBitsCopy();
    MessageReset();
    MapPadDirectionsSet(0);
    FaceListPartySet();
    MapSpriteDataFree(1);
    StoryBits3E8Clear();
    AS(s16, pos.x) = e.x << 3;
    AS(s16, pos.y) = e.y << 3;
    pos.facing = e.facing + 1;
    MapObjectsInit(e.object_set);
    MapPartySpotSet((s32)&pos, 1);
    g_MapBgChange[1] = 0;
    g_MapTalkObjectDefinition = 0xFF;
    g_MapBgChange[0] = 0;
    g_MapBgChangeNumber = 0;
    D_800B8E0C = 0;
    g_MapPartyBlinkCount = 0;
    g_MapLimitLeft = e.limit_left << 3;
    g_MapLimitRight = (e.limit_right + 1) << 3;
    g_MapLimitTop = e.limit_top << 3;
    g_MapLimitBottom = (e.limit_bottom + 1) << 3;
    MapViewStep(1, 1);
    if (run) {
        MapFadeIn(g_MapLayersMask | 0x130, 0x19, view_follow, second_view_follow);
    }
    D_800B8E3C = 0;
}
/* load_hazard_nop: GameEnter */
/* far_syms: g_PlayTimeMinutes g_MapBgChange */
/* la_absolute: GameEnter MapColFadeStart */
/* g_MapCol (the screen ramps' default colour triple, three s16) is
   declared ONCE in this TU, as the small struct it is, and the functions
   that take its address carry the la_absolute marker above. Why:
   - MapColSet loads and stores its three members %gp_rel, so the
     object is small data.
   - Wherever its ADDRESS is taken (the block copy in GameEnter and
     MapColSet, the ramp calls in MapColFadeStart, MapColFlashRun and
     ScriptCommandMapColSet) the target has a same-register lui / addiu. That is
     the Sony assembler's `la` of a small-data symbol, not a far symbol:
     ASPSX never used $gp for `la`.
   - The object is not declared a second time: an unsized
     `extern s16 g_MapCol[]` near the top would make it far for the
     WHOLE TU (cc1 takes the small-data decision at the first
     declaration), and so would the far alias g_MapCol_blk for the
     block copy. Under either, MapColSet's member accesses cannot be
     gp-relative (34/61 through the alias, 61/61 on this declaration).
   The alias symbol stays in config/ for the TUs that still use it. */
void ScriptLabelRun(s32);                               /* extern */
s32 debug_printf();  /* variadic: callers pass 1..9 args — unprototyped, as the original did */
s32 layer_mix_set(s32, u8);                             /* extern */
void MapObjectsRestore(s32 set_number, u8 *entries);
void ScriptChange(u8 script_number);
extern GameMapRecord *g_GameMapRecord;
extern GameSaveHead *g_GameSaveBlock;
extern u16 g_PlayTimeMinutes;
extern u8 D_800128EC[];
extern s8 g_PartyLevelLeast;
extern MapCol g_MapCol;  /* the TU's one declaration: see the note above MapCol */
extern s16 g_MapMix;
extern s16 g_MapSecondViewKeptX;
extern s16 g_MapSecondViewKeptY;

/* GameEnter -- enter the field from the load screen's result (the mirror
   of the save writer MenuSaveWrite in menu_card.c): three callers pass
   MenuNoWalkRun()'s return value (map_btl.c, map_frame.c, game_start.c),
   and MenuRun (menu_run.c) passes g_MenuRequest.

   id = request & 0xFFFF.
   id 0 or 4: MapFadeOut(g_MapLayersMask | 0x130, 0x19, -2, 0, 1) (the scene
     transition out), MapSpriteDataFree(0), xmalloc_heap_reset(), GameRecordsReset(),
     ScriptChange(0x64), ScriptLabelRun(1) -- and nothing else.
   otherwise: the same transition with -1, MapSpriteDataFree(0), xmalloc_heap_reset(), then
     id < 0x100  -- a save was read: its 0xC00-byte state block is copied
                    from 0x801CFB00 into *g_GameSaveBlock (mem_move_32 is
                    (dest, src, n)), the saved map-object list is the 0x28-byte
                    entries right behind it at 0x801D0700, SoundMonoSet().
     id >= 0x100 -- no save: the map header's b0 = (u8)request (the map), b1E =
                    b1D = 0. THE OBJECT LIST POINTER IS NOT SET ON THIS PATH
                    IN THE ORIGINAL: $s2 is written only in the other arm, so
                    MapObjectsRestore below gets whatever the caller left in $s2
                    (it reads it only when the header's b1C is non-zero).
   Then, from the map header *g_GameMapRecord and the state block:
     MapSet(hdr->b0, hdr->b1E, hdr->b1D) (asm, unread: the scene
     functions above call it with the map id first);
     g_PlayTimeMinutes = blk->hA, g_MapPlaceName = blk->bE;
     g_MapPartyKind = hdr->b1, g_MapMix = hdr->h2, the 6-byte g_MapCol block
     = hdr->v4, g_PartyCount = hdr->b1A, g_PartyLevelLeast = hdr->b1B
     -- the mirror of what MenuSaveWrite writes;
     MapBgChangeStoryBitsCopy(), MessageReset(), MapPadDirectionsSet(0), FaceListLoad(),
     MapPartyInit(hdr + 0xA) (the 8 bytes MapPartyRingEntryGet saved),
     MapObjectsRestore(hdr->b1C, list) spawns the map objects (MapObjectRestore
     per entry), debug_printf("\x81\x9a\x81\x9a mapmix=%d\n", g_MapMix),
     layer_mix_set(8, (u8)g_MapMix), and (8, 0x20) as well when
     g_MapMix is 0;
     g_MapBgChange[1] = [0] = 0, g_MapBgChangeNumber = D_800B8E0C = g_MapPartyBlinkCount = 0,
     g_MapSecondViewKeptY = g_MapSecondViewKeptX = g_MapViewKeptY = g_MapViewKeptX = -1, the scroll
     bounds g_MapLimitLeft / CE2 / CE0 / CE4 = hdr->h12 / h14 / h16 / h18,
     MapViewStep(1, 1) (the camera snapped to MapViewGoalGet's position),
     MapAttributeRun(), and the transition in
     MapFadeIn(g_MapLayersMask | 0x130, 0x19, 1, 1).
   No frame wait and no RNG draw in this function itself.

   Shape: the header and the state block are STRUCT-typed. That is what
   lets the scheduler lift the field loads above the stores to the scalar
   globals (sched.c's true_dependence exempts an in-struct varying load
   against a fixed scalar store) while keeping them below the g_MapBgChange[]
   stores -- the target's order, from plain source order. With raw
   `*(u16 *)(p + N)` reads the loads stay interleaved with the stores
   (122/166). s32 temporaries, a fake two-argument call to
   MapBgChangeStoryBitsCopy(void) and fake third and fourth arguments to
   MapViewStep reach the same rows but stop at 163/166: the last three rows
   (the g_MapBgChange address temp in $a1 instead of $a2) cannot close,
   because MapViewStep takes TWO arguments, and with the real call the
   four halfword temps take $a2, $a3, $t0, $v1 by themselves.
   g_MapMix is read as (u16) -- this TU declares it s16 further down;
   the test goes through a u16 lvalue so it loads with lhu. */
/* func_80059498 */
void GameEnter(s32 request)
{
    u16 id;
    u8 *objs;
    u8 pad[16];     /* MARKED SPELLING (unused local): the target frame (0x38) holds
                       16 bytes at 0x18-0x27 that no instruction touches -- the
                       original's local(s), declaration unknown */

    id = request;
    if (id == 0 || id == 4) {
        MapFadeOut(g_MapLayersMask | 0x130, 0x19, -2, 0, 1);
        MapSpriteDataFree(0);
        xmalloc_heap_reset();
        GameRecordsReset();
        ScriptChange(0x64);
        ScriptLabelRun(1);
        return;
    }
    MapFadeOut(g_MapLayersMask | 0x130, 0x19, -1, 0, 1);
    MapSpriteDataFree(0);
    xmalloc_heap_reset();
    if (id < 0x100) {
        mem_move_32(g_GameSaveBlock, 0x801CFB00, 0xC00);
        objs = (u8 *)0x801D0700;
        SoundMonoSet();
    } else {
        g_GameMapRecord->map = request;
        g_GameMapRecord->music_number = 0;
        g_GameMapRecord->script_number = 0;
    }
    MapSet(g_GameMapRecord->map, g_GameMapRecord->music_number, g_GameMapRecord->script_number);
    g_PlayTimeMinutes = g_GameSaveBlock->play_time_minutes;
    g_MapPlaceName = g_GameSaveBlock->place_name;
    g_MapPartyKind = g_GameMapRecord->party_kind;
    g_MapMix = g_GameMapRecord->mix;
    g_MapCol = g_GameMapRecord->col;
    g_PartyCount = g_GameMapRecord->party_count;
    g_PartyLevelLeast = g_GameMapRecord->party_level_least;
    MapBgChangeStoryBitsCopy();
    MessageReset();
    MapPadDirectionsSet(0);
    FaceListLoad();
    MapPartyInit((u8 *)&g_GameMapRecord->ring_entry);
    MapObjectsRestore(g_GameMapRecord->objects_data_number, objs);
    debug_printf(D_800128EC, (u16)g_MapMix);
    layer_mix_set(8, (u8)g_MapMix);
    if (*(u16 *)&g_MapMix == 0) {
        layer_mix_set(8, 0x20);
    }
    g_MapBgChange[1] = 0;
    g_MapBgChange[0] = 0;
    g_MapBgChangeNumber = 0;
    D_800B8E0C = 0;
    g_MapPartyBlinkCount = 0;
    g_MapSecondViewKeptY = -1;
    g_MapSecondViewKeptX = -1;
    g_MapViewKeptY = -1;
    g_MapViewKeptX = -1;
    g_MapLimitLeft = g_GameMapRecord->limit_left;
    g_MapLimitRight = g_GameMapRecord->limit_right;
    g_MapLimitTop = g_GameMapRecord->limit_top;
    g_MapLimitBottom = g_GameMapRecord->limit_bottom;
    MapViewStep(1, 1);
    MapAttributeRun();
    MapFadeIn(g_MapLayersMask | 0x130, 0x19, 1, 1);
}

extern long HWD0, VWD0;		 
extern u8 *g_MapPartyPlaceRows;
extern s16 g_MapLimitLeft;
extern s16 g_MapLimitTop;
extern s16 g_MapLimitRight;
extern s16 g_MapLimitBottom;
s32 SoundPlay(u16, s32);
s32 MapDrawAreaClose(s32 layer_mask, u32 step_count, s32 unused, s32 view_follow, s32 second_view_follow);
s32 MapDrawAreaOpen(s32 layer_mask, u32 step_count, s32 unused, s32 view_follow, s32 second_view_follow);
s32 MapPartySpotSet(s32, s32);
s32 MapViewStep(s32, s32);

/* func_80059730 */
void MapPartyPlace(u16 idx, MapPartyPlaceRow *src, s32 run, s32 view_follow, s32 second_view_follow)
{
    MapPartyPlaceRow e;
    MapPartyRingEntry pos;

    if (idx != 0) {
        e = ((MapPartyPlaceRow *)g_MapPartyPlaceRows)[idx];
    } else {
        e = *src;
    }
    if (e.sound != 0) {
        SoundPlay(e.sound, 3);
    }
    MapDrawAreaClose(g_MapLayersMask | 0x130, 0x19, e.unk_00, view_follow, second_view_follow);
    AS(s16, pos.x) = e.x << 3;
    AS(s16, pos.y) = e.y << 3;
    pos.facing = e.facing + 1;
    MapPartySpotSet((s32)&pos, 0);
    g_MapBgChangeNumber = 0;
    g_MapPartyBlinkCount = 0;
    g_MapLimitLeft = e.limit_left << 3;
    g_MapLimitRight = (e.limit_right + 1) << 3;
    g_MapLimitTop = e.limit_top << 3;
    g_MapLimitBottom = (e.limit_bottom + 1) << 3;
    MapViewStep(1, 1);
    if (run) {
        MapDrawAreaOpen(g_MapLayersMask | 0x130, 0x19, e.unk_00, view_follow, second_view_follow);
    }
}

/* func_800598AC */
void MapBgChangeRowSet(MapBgChange *record, MapBgChangeRow *row)
{
	u8 *source;

	source = record->source;
	record->source_count = row->source_count;
	record->dx = row->dx;
	record->dy = row->dy;
	record->width = row->width;
	record->height = row->height;
	record->sx = source[0];
	record->sy = source[1];
}

s32 StoryBitSet(u16);
s32 SoundPlay(u16, s32);
extern s32 g_MapBgChangeTable;

/* func_80059900 */
void MapBgChangeStart(s16 change_number, u8 delay)
{
    u16 id;
    u16 res;
    MapBgChangeRow *entry;
    MapBgChange *ptr;
    u8 flag;

    id = change_number;
    if (g_MapBgChangeNumber == id) {
        return;
    }
    res = u16_bytes_swap(*(u16 *)(g_MapBgChangeTable + id * 2));
    if (res == 0) {
        return;
    }
    entry = (MapBgChangeRow *)(g_MapBgChangeTable + res * 2);
    if (entry->story_bit != 0 && StoryBitSet(entry->story_bit + 0x480) == 0) {
        return;
    }
    ptr = (MapBgChange *)g_MapBgChange;
    ptr->source = entry->sources;
    ptr->copies = 0;
    ptr->frames = 0;
    ptr->interval = entry->interval;
    MapBgChangeRowSet(ptr, entry);
    flag = entry->sound;
    ptr->delay = delay;
    ptr->number = id + 1;
    ptr->sound = flag;
    if (delay != 0) {
        return;
    }
    g_MapBgChangeNumber = id;
    ptr->copy_due = 1;
    if (flag != 0) {
        SoundPlay(ptr->sound, 3);
    }
}

/* far_syms: g_MapBgChange[0] (target reaches it %hi/%lo) */

/* func_80059A18 */
void MapBgChangeStep(void)
{
  u8 *ptr;
  MapBgChange *p;
  /* MARKED SPELLING: a second pointer to the record. The target holds the record's
     address in two registers ($a0, and $a1 copied from it at entry) and works through
     the second in the last block, so the source had two pointers to it. Both are
     written as MapBgChange pointers (map.h), which gives the same bytes as two plain
     `u8 *`; what the source's two were is not known. With one pointer for everything
     12 of the 61 instruction lines differ. */
  MapBgChange *q;
  p = (MapBgChange *)g_MapBgChange;
  q = p;
  if (p->number == 0)
  {
    return;
  }
  if (p->copy_due)
  {
    return;
  }
  if (p->delay)
  {
    if (--p->delay)
    {
      return;
    }
    g_MapBgChangeNumber = p->number - 1;
    p->copy_due = 1;
    if (p->sound)
    {
      SoundPlay(p->sound, 3);
    }
    return;
  }
  if (++p->frames < p->interval)
  {
    return;
  }
  p->copies++;
  if (p->source_count == p->copies)
  {
    p->number = 0;
    return;
  }
  ptr = q->source;
  q->frames = 0;
  q->source = ptr + 2;
  q->sx = ptr[2];
  q->sy = ptr[3];
  q->copy_due = 1;
}

/* func_80059B0C */
void MapBgChangeCopy()
{
    MapBgChange *change;

    change = (MapBgChange *)g_MapBgChange;
    if (change->copy_due) {
        bg_both_copy(change->dx, change->dy, change->sx, change->sy, change->width, change->height);
        change->copy_due = 0;
    }
}

s32 StoryBitCheck(s32);

/* func_80059B6C */
void MapBgChangeStoryBitsCopy(void)
{
  u32 i;
  MapBgChange *ptr;
  u16 result;
  MapBgChangeRow *row;
  s32 result2;
  i = 0;
  ptr = (MapBgChange *)g_MapBgChange;
  do
  {
    result = u16_bytes_swap(*((u16 *) (g_MapBgChangeTable + ((i & 0xFFFF) * 2))));
    if (result != 0)
    {
      row = (MapBgChangeRow *) (g_MapBgChangeTable + (result * 2));
      if (row->story_bit != 0)
      {
        result2 = StoryBitCheck(AS(volatile u8, row->story_bit) + 0x480);
        if (result2 != 0)
        {
          ptr->source = (u8 *)row + (row->source_count * 2 + 6);
          ;
          ptr->copies = row->source_count - 1;
          MapBgChangeRowSet(ptr, row);
          ptr->copy_due = 1;
          MapBgChangeCopy();
        }
      }
    }
    i++;
  }
  while ((i & 0xFFFF) < 0x80);
}

s32 MapViewStep(s32, s32);                              /* extern */

/* func_80059C48 */
void MapAttributeKind4Run(s32 label) {
    MapViewStep(1, 1);
    ScriptLabelRun(label & 0xFF);
}

void ScriptLabelRun(s32);                               /* extern */
s32 MapPartyRingRewind();              /* extern */

/* func_80059C7C */
void MapAttributeKind5Run(s32 label) {
    MapPartyRingRewind();
    ScriptLabelRun(label & 0xFF);
}

void ScriptLabelRun(s32);                               /* extern */

/* func_80059CA8 */
void MapAttributeKind6Run(s32 label) {
    ScriptLabelRun(label & 0xFF);
}

void MapColSet(s32, s32);  /* TU defines this below */

/* func_80059CC8 */
s32 MapColFadeStart(s32 row, s32 scale)
{
  if (row != 0)
  {
    MapColSet(row, scale);
    layer_col_fade_start(0, g_MapLayersMask | 0x100, 1, 0x18, (s32 *)&g_MapCol);
  }
}
/* la_absolute: MapColSet */
extern MapCol g_MapColRows[];

/* MapColSet -- SETS THE SCREEN'S COLOUR TRIPLE FROM A PRESET ROW TIMES A
   STRENGTH. Arguments: (row, scale), both full ints -- neither is masked.

     g_MapCol = g_MapColRows[row]     the 6-byte row copied whole
     each of the three s16 channels *= scale, stored back as 16 bits
     each channel then clamped to -0xFF .. 0xFF

   The clamp reads the channel back after the 16-bit store, so it tests the
   truncated product, not the int one.

   g_MapCol (three s16; g_MapColSecond and g_MapColThird are its second and
   third channel) is the default colour triple of the screen ramps: the
   map header's v4 is loaded into it on entering the field (GameEnter);
   layer_col_fade_start(win, flags, a2, len, triple) starts a ramp to a triple
   (battle.md "Window ramps") and layer_col_set(win, flags, triple) takes
   one with no length (its body was not read here); menu_list.c, map_btl.c
   and map_frame.c set all three channels to -100 or -0x38.
   g_MapColRows is ten such rows at 0x800B3300 .. 0x800B333B, right in front
   of the script command table at 0x800B333C. In this build every row is
   grey (three equal channels):
     0 (0)  1 (-14)  2 (16)  3 (-6)  4 (-14)  5 (48)  6 (12)  7, 8, 9 (16)
   With a 4-bit scale (the script dispatch's kind 8, MapEffectSet)
   only row 5 can reach the clamp (48 * 6 = 288). ScriptCommandMapColSet passes
   whole bytes, directly or through MapColFadeStart; a row index past 9
   would read the command table's pointers as a row. 48 * 255 fits 16
   bits, so no in-table row wraps before the clamp.

   Callers. None reads a return value.
     MapColFadeStart(row, scale), above: only when row != 0, then starts the
       0x18-frame ramp layer_col_fade_start(0, g_MapLayersMask | 0x100, 1, 0x18, the
       triple). The script dispatch's kind 8 calls that with (arg >> 4,
       arg & 0xF).
     ScriptCommandMapColSet, a script command (bytes +2, +3 = row, scale; byte +4 !=
       0): this function, then layer_col_set(0, g_MapLayersMask | 0x100, the
       triple) at once when g_MapFadeState and g_MapDrawAreaClosed are both 0. With byte
       +4 == 0 it calls MapColFadeStart instead.
     MapEffectSet (map_load.c), the map-effect record's flag 0x10:
       ((+0xE >> 4) & 7, +0xE & 0xF), skipped when the first is 0.

   It calls nothing: no RNG draw, no frame wait.

   Shape. g_MapCol is ONE small struct: its members are reached
   gp-relative and the block copy takes its address with the assembler's
   `la`, which the Sony assembler wrote as lui / addiu on one register
   (la_absolute; 59/61 without the marker, one gp-relative addiu). Three
   loose s16 scalars plus a far alias for the block (the alias pair
   menu_list.c uses for this symbol) is 34/61: cc1 cannot see that the copy
   feeds the scalars and lifts their loads above it. The parameters are
   ints: as (u8, u8), which this TU's forward declarations said, each gets
   an `andi` the target does not have (60/61). No marked spelling, no
   device. */
/* func_80059D10 */
void MapColSet(s32 row, s32 scale)
{
    g_MapCol = g_MapColRows[row];
    g_MapCol.first *= scale;
    g_MapCol.second *= scale;
    g_MapCol.third *= scale;
    if (g_MapCol.first < -0xFF) {
        g_MapCol.first = -0xFF;
    } else if (g_MapCol.first > 0xFF) {
        g_MapCol.first = 0xFF;
    }
    if (g_MapCol.second < -0xFF) {
        g_MapCol.second = -0xFF;
    } else if (g_MapCol.second > 0xFF) {
        g_MapCol.second = 0xFF;
    }
    if (g_MapCol.third < -0xFF) {
        g_MapCol.third = -0xFF;
    } else if (g_MapCol.third > 0xFF) {
        g_MapCol.third = 0xFF;
    }
}

extern u8 D_800128FC[];

s32 layer_mix_fade_start(s32, s32, s32, s8);                       /* extern */
extern s16 g_MapMix;

/* func_80059E04 */
s32 ColMix(u8 no, s32 sw) {
    u8 masked_no;

    masked_no = no & 0xFF;
    debug_printf(D_800128FC, masked_no, sw);
    if (masked_no == 0xF) {
        g_MapMix = 0x1F;
    } else {
        g_MapMix = masked_no * 2;
    }
    if (sw != 0) {
        layer_mix_fade_start(8, 1, 0x18, (s8) g_MapMix);
        return;
    }
    layer_mix_set(8, (u8) g_MapMix);
    return;
}

/* far_syms: g_MapActuatorOneFrameList (target reaches it %hi/%lo) */
extern u8 g_MapActuatorOneFrameList[];
extern u8 g_MapPartyHpTakeWait;
extern u8 g_MapPartyHpTakeAmount;

/* func_80059E84 */
void MapPartyHpTake(void)
{
  u16 *ptr;
  s32 base;
  u16 val;
  s32 item;
  u16 *item_ptr;
  u16 field;
  int i;
  u16 local[3];

  g_PadActuatorList = (s32)g_MapActuatorOneFrameList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;

  base = g_ChrRecords;
  ptr = (u16 *)g_PartyIds;
  val = g_MapPartyHpTakeAmount;

  for (i = 0; i < 6; i++) {
    item = *ptr;
    if (item == 0)
      break;
    if (item < 0xC) {
      item_ptr = (u16 *)(item * 64 + base);
      field = *item_ptr;
      if (val < field) {
        *item_ptr = field - val;
      } else {
        *item_ptr = 1;
      }
    }
    ptr++;
  }

  local[0] = 0xC8;
  local[2] = 0;
  local[1] = 0;
  layer_col_set(0, g_MapLayersMask | 0x100, (s32 *)local);
  g_MapPartyHpTakeWait = 0x1E;
}
