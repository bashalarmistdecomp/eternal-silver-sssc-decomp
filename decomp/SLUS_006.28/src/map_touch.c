/* Lunar SSSC — map_touch: the touched map object: its approach, its wait,
   its removal, and the cursor over the objects of kinds 4 to 6. The first
   split had it in menu.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "path.h"
#include "mth.h"
#include "map.h"
#include "btl.h"
/* The 0x60-byte field entities (g_MapObjects records; g_MapPartyRecords points at
   one). Read as a struct, an entity field load is a struct access, which
   GCC 2.8's alias rule lets pass a store to a fixed-address global. */
 

extern s16 g_MapTouchedObject;
extern s32 g_MapObjects;
extern s16 g_MapTouchedObjectKeptX;
extern s16 g_MapTouchedObjectKeptY;
extern u8 g_MapTouchedObjectKeptFacing;
extern s16 g_MapTouchedObjectKeptClass;
extern s32 g_MapPartyRecords;
extern u8 g_MapPartyBoxHalfWidth;
extern u8 g_MapPartyBoxHalfHeight;
extern s8 g_MapPartyBoxDx;
extern s8 g_MapPartyBoxDy;
extern s32 g_MapProbeBoxHalfWidth;
extern s32 g_MapProbeBoxHalfHeight;
extern s32 g_MapProbeBoxX;
extern s32 g_MapProbeBoxY;
void MapAnimationPoseSet(void *, s32, s32);
s32 MapSpotMoveTry();
u16 MapBoxTouchGet(s16, s16, MapBox *);
void *memcpy(void *, const void *, u32);  /* TU declares this below */
s32 MapFacingTowardGet(s16 x0, s16 y0, s16 x1, s16 y1, s32 facing);
s32 MapFrameStep(s32, s32);  /* TU declares this below */
void MapBgChangeStep(void);/* load_hazard_nop: MapObjectTouchApproach */

/* Field step: save field entity g_MapTouchedObject (position, facing, +0xE &
   0x6000 to g_MapTouchedObjectKeptX/DA/DD/DE), take its +0x50 probe grown by one in
   w/h, dispatch its facing, set the origin rect g_MapProbeBoxX..E0 from
   g_MapPartyRecords (position + g_MapPartyBoxDx/29, size g_MapPartyBoxHalfWidth/2B), then for 32
   frames: a blocked probe (MapBoxTouchGet) stops it; while on its first
   facing it re-aims toward g_MapPartyRecords (MapFacingTowardGet); it steps
   (MapSpotMoveTry) and writes facing and position back. */
/* Two loads of e are pointer accesses through their members' addresses,
   the ground y (*(u16 *)&...record.sprite.depth) and the flags
   (*(s16 *)&...record.sprite.flags): as member loads (or the flags as
   AS(s16, ...) or through a u16 pointer) each is
   scheduled above the store before it (g_MapTouchedObjectKeptX,
   g_MapTouchedObjectKeptFacing), GCC taking a member at a varying address
   as unable to touch a fixed scalar, and the code no longer matches. The
   probe's copy takes e + 0x50, not &box: from the member the head is
   scheduled otherwise. The rest of e is members. */
/* func_8007B310 */
void MapObjectTouchApproach(void)
{
  u8 *e;
  MapBox probe;
  s16 pos[2];
  u16 dir;
  u16 want;
  s32 i;
  s32 ox;
  s32 oy;
  s16 y;
  MapRecord *ent;
  e = (u8 *) (g_MapObjects + (g_MapTouchedObject * 0x60));
  g_MapTouchedObjectKeptX = ((MapObject *)e)->record.sprite.x;
  g_MapTouchedObjectKeptY = *(u16 *)&((MapObject *)e)->record.sprite.depth;
  pos[0] = g_MapTouchedObjectKeptX;
  pos[1] = g_MapTouchedObjectKeptY;
  dir = g_MapTouchedObjectKeptFacing = ((MapObject *)e)->facing;
  g_MapTouchedObjectKeptClass = *(s16 *)&((MapObject *)e)->record.sprite.flags & 0x6000;
  memcpy(&probe, e + 0x50, 4);
  probe.half_width++;
  probe.half_height++;
  MapAnimationPoseSet(e, 2, ((MapObject *)e)->facing);
  ent = (MapRecord *) g_MapPartyRecords;
  g_MapProbeBoxHalfWidth = g_MapPartyBoxHalfWidth;
  g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfHeight;
  ox = ent->sprite.x + g_MapPartyBoxDx;
  oy = ent->sprite.depth + g_MapPartyBoxDy;
  g_MapProbeBoxX = ox;
  g_MapProbeBoxY = oy;
  want = dir;
  for (i = 0; i < 0x20; i++)
  {
    if (MapBoxTouchGet(pos[0], pos[1], &probe))
    {
      dir = 0;
    }
    if (dir != 0)
    {
      if (want == dir)
      {
        want = MapFacingTowardGet(pos[0], pos[1], ((MapRecord *)g_MapPartyRecords)->sprite.x,
                             ((MapRecord *)g_MapPartyRecords)->sprite.depth, want);
      }
      dir = MapSpotMoveTry(want, 1, &pos[0], &pos[1], &((MapObject *)e)->box, ((MapObject *)e)->layer, 0);
      if (dir != 0)
      {
        ((MapObject *)e)->facing = dir;
        MapAnimationPoseSet(e, 2, dir);
        y = pos[1];
        ((MapObject *)e)->record.sprite.x = pos[0];
        ((MapObject *)e)->record.sprite.y = y;
        ((MapObject *)e)->record.sprite.depth = y;
      }
    }
    MapFrameStep(1, 1);
    MapBgChangeStep();
  }
}
/* load_hazard_nop: MapObjectTouchWait */

/* The field-save prologue of MapObjectTouchApproach without its walk: save field
   entity g_MapTouchedObject's position (g_MapTouchedObjectKeptX/DA), facing (g_MapTouchedObjectKeptFacing) and
   +0xE & 0x6000 (g_MapTouchedObjectKeptClass), take the +0x50 probe (grown by one in w/h,
   unused here), dispatch its facing, set the origin rect g_MapProbeBoxX..E0
   from g_MapPartyRecords's position plus (g_MapPartyBoxDx, C29) and size (C2A, C2B),
   then wait 32 frames. The g_MapPartyRecords entity is read through a MapRecord
   pointer: as a struct access its loads may pass the rect's gp stores. */
/* Two loads of e are pointer accesses through their members' addresses,
   as in MapObjectTouchApproach: the ground y (+8) and the flags (+0xE).
   Written as members of MapObject each is scheduled above the store before
   it (g_MapTouchedObjectKeptX, g_MapTouchedObjectKeptFacing) and the code
   no longer matches. The probe's copy takes e + 0x50, as there. */
/* func_8007B4D4 */
void MapObjectTouchWait(void)
{
  u8 *e;
  MapBox probe;
  s32 ox;
  s32 oy;
  s32 i;
  MapRecord *ent;
  e = (u8 *) (g_MapObjects + (g_MapTouchedObject * 0x60));
  g_MapTouchedObjectKeptX = ((MapObject *)e)->record.sprite.x;
  g_MapTouchedObjectKeptY = *(u16 *)&((MapObject *)e)->record.sprite.depth;
  g_MapTouchedObjectKeptFacing = ((MapObject *)e)->facing;
  g_MapTouchedObjectKeptClass = *(s16 *)&((MapObject *)e)->record.sprite.flags & 0x6000;
  memcpy(&probe, e + 0x50, 4);
  probe.half_width++;
  probe.half_height++;
  MapAnimationPoseSet(e, 2, ((MapObject *)e)->facing);
  ent = (MapRecord *) g_MapPartyRecords;
  g_MapProbeBoxHalfWidth = g_MapPartyBoxHalfWidth;
  g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfHeight;
  ox = ent->sprite.x + g_MapPartyBoxDx;
  oy = ent->sprite.depth + g_MapPartyBoxDy;
  g_MapProbeBoxX = ox;
  g_MapProbeBoxY = oy;
  for (i = 0; i < 0x20; i++)
  {
    MapFrameStep(1, 1);
    MapBgChangeStep();
  }
}

void func_8007B7C0();  /* TU defines this below */
s32 MapFrameStep(s32, s32);
s32 StoryBitSet(s32);
s32 MapObjectOverlappedRemove(void *);
void MapObjectCursorAdvance(s32);
extern s32 g_MapObjects;
extern u16 g_MapNumber;
extern s32 g_MapObjectDefinitions;
extern s32 g_MapObjectCursor;
extern s16 g_MapTouchedObject;

/* func_8007B5CC */
void MapObjectTouchRemove(s32 view_follow, s32 second_view_follow)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `bit_base` only holds the constant 0x2C00; behaviour is the same with the constant
     written where it is read, but then 2 of this function's 86 instruction lines differ. */
  int bit_base;
  s32 scale;
  u8 definition_number;
  void *row;
  s32 i;
  void *definition;
  scale = 0x100;
  i = 0;
  row = g_MapObjects + (g_MapTouchedObject * 0x60);
  bit_base = 0x2C00;
  ((MapObject *)row)->record.sprite.scaled = 1;
  do
  {
    ((MapObject *)row)->record.sprite.scale_x = scale;
    ((MapObject *)row)->record.sprite.scale_y = scale;
    MapFrameStep(view_follow, second_view_follow);
    i += 1;
    scale -= 0xC;
  }
  while (i < 0x14);
  ((MapObject *)row)->record.sprite.scaled = 0;
  ((MapObject *)row)->record.on_kind.kind = 0;
  definition_number = ((MapObject *)row)->record.hold_definition.definition;
  definition = g_MapObjectDefinitions + (definition_number * 0x26);
  if (((MapObjectDefinition *)definition)->story_bit != 0)
  {
    StoryBitSet((((MapObject *)row)->record.hold_definition.definition + ((g_MapNumber << 7) - bit_base)) & 0xFFFF);
  }
  if ((((MapObjectDefinition *)definition)->seen & 0x7F) == 3)
  {
    if (((((MapObjectDefinition *)definition)->seen_target.overlap.action != 0) && (((MapObject *)row)->target.overlapped != 0)) && (AS(s8, ((MapObject *)row)->state) >= 0))
    {
      MapObjectOverlappedRemove(row);
    }
  }
  MapFrameStep(view_follow, second_view_follow);
  if (g_MapTouchedObject == g_MapObjectCursor)
  {
    MapObjectCursorAdvance(g_MapTouchedObject);
  }
}
/* far_syms: g_MapObjectCursor_blk */
/* no_schedule_insns: MapObjectCursorAdvance */
extern s32 g_MapObjectCursor_blk;
void sprite_run_off(s32 first, s32 count);

/* func_8007B724 */
void MapObjectCursorAdvance(s32 row_number)
{
  s32 *cursor;
  u8 *rec;
  cursor = &g_MapObjectCursor_blk;
loop:
  *cursor = (g_MapObjectCursor < 0x7F) ? g_MapObjectCursor + 1 : 0;
  if (g_MapObjectCursor == row_number)
  {
    sprite_run_off(0x6C, 2);
    return;
  }
  rec = (u8 *) ((g_MapObjectCursor * 0x60) + g_MapObjects);
  if ((u32) (rec[0] - 4) >= 3U)
  {
    goto loop;
  }
  if ((rec[0x3D] & 0x3F) == 3)
  {
    goto loop;
  }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its one call, among the battle's frees, passes nothing and does not say what for. */
void func_8007B7C0() {

}
