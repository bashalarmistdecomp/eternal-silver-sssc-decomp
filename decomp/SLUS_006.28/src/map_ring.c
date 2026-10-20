/* Lunar SSSC — map_ring: the party's ring of past places that the followers
   walk on: rewound, read, written, filled, pushed and followed, and a
   twin's spot. The first split had it in scripting_e.c. Every function is
   C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"

extern u16 g_MapPartyRingHead[1];
extern s32 g_MapPartyTwins;
extern u16 g_MapPartyWalkStepCount;
extern s32 g_MapPartyRecords;
extern s32 g_MapPartyRingEntries;
extern u16 g_MapPartyWalkStepCount;
extern s8 g_MapPartyBoxDx;
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
void MapPartyRingFill(u16, u16, s32, u8);
s32 MapPartyRingFollow();
s32 MapAttributeClassGet(u16 step, u16 top, s32 x, s32 y, u16 ground, MapBox *box, s32 layer);
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
void MapAnimationPoseSet(void *, s16, u16);
void MapPartyTwinSpotSet(s32 place, void *spot);
void MapPartyRingRewind();
s32 MapPartyRingEntryGet(s32, s32 *);
s32 MapPartyRingPush(u16, u16, s32, s32);

/* la_absolute: MapPartyRingRewind */
extern u16 g_MapPartyRingHead[1];

/* MapPartyRingRewind() -- STEPS THE PARTY'S POSITION RING
   BACK AND PUTS THE PARTY THERE. One caller, MapAttributeKind5Run
   (map_change.c): the handler of kind 5 of the cell event word, which
   calls this and then starts map script `arg` (ScriptLabelRun).

   The ring is 0x60 8-byte entries at g_MapPartyRingEntries (0x801EA000) with the head
   index g_MapPartyRingHead[0], 0..0x5F: MapPartyRingPush moves the head on one
   (0x5F wraps to 0) and writes the lead's new place there, and
   MapPartyRingFollow puts live party actor k (the rows of g_MapPartyRecords) at the
   entry 16 * k behind the head. This function moves the head BACK
   g_MapPartyWalkStepCount entries, one at a time, 0 wrapping to 0x5F, and then calls
   MapPartyRingFollow(), so every party actor is taken back along the trail by
   that many entries. No entry is written, only the index. g_MapPartyWalkStepCount is
   the number of steps the lead may take in a frame (MapPartyBoxSet: 1 with
   g_MapPartyKind == 0, else 2; MapPartyStep calls MapPartyWalkStep up to that
   many times a frame, io-hardware.md, and each call pushes at most one
   entry), so g_MapPartyWalkStepCount entries is the most one frame of walking pushes.
   The head goes back that many whether or not that many were pushed this
   frame (what the caller's frame had pushed was not read here). With
   g_MapPartyWalkStepCount == 0 the head is left and MapPartyRingFollow still runs. No RNG
   draw and no frame wait of its own; nothing returned (MapPartyRingFollow has
   no return statement either).

   Source shapes the bytes need:
   - The head index is a MEMBER OF AN AGGREGATE. The target stores it
     through a register that holds the symbol's address (`lui` / `addiu
     $a1` ahead of the loop, `sh $v0, 0($a1)`) and loads it gp-relative.
     For a store to an array element or a struct member expand puts the
     address in a pseudo BEFORE the right-hand side (the first
     divergence from the scalar text is an inserted `(set (reg:SI P)` at
     expand); the store sits after the join of the two arms, so the
     pseudo stays, and the loop pass moves its `la` in front of the loop.
     A scalar is stored gp-relative: 20/24. Outside a loop the same
     statement stores gp-relative under either declaration (probe of
     MapPartyRingPush's `x = x < 0x5F ? x + 1 : 0`, which is what its asm
     has), and the whole TU compiles to the same assembly under both.
     MARKED SPELLING: g_MapPartyRingHead is declared `u16 [1]` for this. The
     bytes say "member of a two-byte aggregate", not which
     one (array, struct, union). As a scalar this function is 20/24.
   - The marker la_absolute: that `la` of a small-data symbol is the
     target's same-register `lui` / `addiu`, as in MapPartyRingFill and
     MapObjectOverlapFind; without it GAS gives `addiu $a1, $gp` (21/24).
   - `i = 0; if (n != 0) do { } while (i < n);`, not a `for`. Under 2.8.1
     `for (i = 0; i < g_MapPartyWalkStepCount; i++)` is the same 24 words. Under GCC
     2.7.2 (the cc272 marker) the `for` gives a 0x20 frame and a `blez`
     entry test, and the Japanese build's twin (0x8006A470 in SLPS_013.97)
     has this function word for word with an 0x18 frame and `beqz`; the
     guarded do-while gives the twin's bytes.
   - One conditional expression for the new index (its arms in either
     order are the same bytes in both builds); an if / else with a store
     in each arm is 17/24.
   The other files' `s32 MapPartyRingRewind();` (map_change.c)
   is stale: nothing is returned and the caller reads nothing. */
/* func_80066F54 */
void MapPartyRingRewind(void)
{
    s32 i;

    i = 0;
    if (g_MapPartyWalkStepCount != 0) {
        do {
            g_MapPartyRingHead[0] = g_MapPartyRingHead[0] != 0 ? g_MapPartyRingHead[0] - 1 : 0x5F;
            i++;
        } while (i < g_MapPartyWalkStepCount);
    }
    MapPartyRingFollow();
}

/* func_80066FB4 */
s32 MapPartyRingEntryGet(s32 place, s32 *spot)
{
    u16 offset;
    s32 src;

    offset = g_MapPartyRingHead[0] < (place & 0xFFFF) * 16
             ? g_MapPartyRingHead[0] + 0x60 - (place & 0xFFFF) * 16
             : g_MapPartyRingHead[0] - (place & 0xFFFF) * 16;

    src = (offset & 0xFFFF) * 8 + g_MapPartyRingEntries;
    memcpy(spot, (void *)src, 8);
    return src;
}

/* func_8006700C */
void MapPartyRingEntrySet(u16 place, void *spot)
{
    u16 offset;

    offset = g_MapPartyRingHead[0] < place * 16 ? g_MapPartyRingHead[0] + 0x60 - place * 16
                                    : g_MapPartyRingHead[0] - place * 16;

    memcpy((void *)(g_MapPartyRingEntries + offset * 8), spot, 8);
}

/* la_absolute: MapPartyRingFill */

/* func_80067064 */
void MapPartyRingFill(u16 x, u16 y, s32 height, u8 facing)
{
  u16 i;
  MapPartyRingEntry entry;
  entry.x = x;
  entry.y = y;
  entry.height = height;
  entry.facing = facing;
  entry.class = MapAttributeClassGet(1, 0x18, x, y - (height & 0xFF), y, (MapBox *) (&g_MapPartyBoxDx), 0);
  for (i = 0; i < 0x60; i++)
  {
    memcpy((void *) (g_MapPartyRingEntries + (i * 8)), &entry, 8);
  }

  g_MapPartyRingHead[0] = 0;

}
/* la_absolute: MapPartyRingPush */
extern u16 g_MapPartyRingHead[1];

/* MapPartyRingPush(x, y, z, dir) -- PUSHES ONE PLACE ONTO THE PARTY'S POSITION
   RING. Two callers: MapPartyWalkStep (this file, the lead's walk step:
   `(x, y, 0, dir & 0xFF)` when MapPartyObjectsFreeCheck(x, y) answered non-zero) and
   MoveChrWalkStep (lns_move.c, script mover slot 1, every pass:
   `((s16)x, (s16)y, z, dir)`, dir being the mover's +0x23 when that is set).

   In this order:
   1. An 8-byte ring entry is built on the stack:
        +0 u16  x
        +2 u16  y      the ground y
        +4 u8   z      the height above the ground (the drawn y is y - z)
        +5 u8   dir    the facing
        +6 u16  flag   MapAttributeClassGet(1, 0x18, x, y - z, y, the lead's box
                       at g_MapPartyBoxDx, 0): 0x2000 or 0x4000, the 0x6000 bits
                       of an actor's +0xE (0x4000 at once when z != 0,
                       because y - z is then not the ground y)
      x and y are 16 bits and z is 8 bits wherever they are used; the
      low byte of dir is stored and dir is not read otherwise.
   2. The head index g_MapPartyRingHead[0] moves on one: below 0x5F it is
      incremented, at 0x5F (or above) it becomes 0. That is after
      MapAttributeClassGet has returned and before the copy.
   3. The entry is copied, bytewise (lwl / lwr / swl / swr), to entry
      [new head] of the ring at g_MapPartyRingEntries (0x60 entries of 8 bytes at
      0x801EA000, MapRecordsClear).
   The ring is read by MapPartyRingFollow (live party actor k is put at the
   entry 16 * k behind the head: x, y, drawn y = y - z, the flag into
   +0xE's 0x6000 bits, dir to MapAnimationPoseSet) and MapPartyRingEntryGet /
   MapPartyRingEntrySet (one spot read / written); MapPartyWalkStep rewrites byte +5
   of the head entry when it pushes nothing and MapPartyTalkCheck reads it as
   the lead's facing; MapPartyRingFill fills all 0x60 entries with one such
   record, and MapPartyRingRewind steps the head back.

   Nothing is returned: there is no return statement, and $v0 is left
   holding the address of the entry just written, which neither caller
   reads. The definition is `s32` because the prototype above
   MapPartyWalkStep says so (lns_move.c declares `s32 MapPartyRingPush();`);
   as `void` it is the same 44 words. No RNG draw and no frame wait in this
   function's own body; the one callee is MapAttributeClassGet (its callee
   MapAttributeFreeCheck, the footprint probe, has no RNG draw and no frame
   wait in its own body either).

   Source shapes the bytes need:
   - No `return`. With `return 0;` after the copy: 32/44 (the original's
     $v0 is the entry address).
   - The head step is ONE conditional expression followed by ONE copy. An
     if / else with a store in each arm is 41/44 (42 with the copy written
     in both arms). Its arms in either order
     (`>= 0x5F ? 0 : + 1`) are the same bytes.
   - The marker la_absolute: the box address is the target's same-register
     `lui` / `addiu $v0` of a small-data symbol, as in MapPartyRingFill above;
     without it GAS gives `addiu $v0, $gp` (42/44).
   Same bytes either way, none of them needed: z as `(u8)height` or as a `u8`
   parameter (unmasked `y - height` on the s32: 42/44), dir as a `u8`
   parameter, the head as a scalar `u16 g_MapPartyRingHead`, the copy's address as
   base + index, and the entry stored with a struct assignment. The text
   below builds the entry as a MapPartyRingEntry local (map.h), member by
   member, as MapPartyRingFill does. */
/* func_80067114 */
s32 MapPartyRingPush(u16 x, u16 y, s32 height, s32 facing)
{
    MapPartyRingEntry entry;

    entry.x = x;
    entry.y = y;
    entry.height = height;
    entry.facing = facing;
    entry.class = MapAttributeClassGet(1, 0x18, x, y - (height & 0xFF), y, (MapBox *)&g_MapPartyBoxDx, 0);
    g_MapPartyRingHead[0] = g_MapPartyRingHead[0] < 0x5F ? g_MapPartyRingHead[0] + 1 : 0;
    memcpy((void *)(g_MapPartyRingHead[0] * 8 + g_MapPartyRingEntries), &entry, 8);
}

s32 bg_party_spot_set(s16, s16);
void MapPartyTwinSpotSet(s32 place, void *spot);  /* TU defines this below */
void MapAnimationPoseSet(void *, s16, u16);

/* func_800671C4 */
s32 MapPartyRingFollow(void)
{
  s32 entries_behind;
  s32 entry_offset;
  unsigned int record_offset;
  s16 entry_number;
  s32 masked_place;
  s32 place;
  unsigned int checked_place;
  void *unused;
  void *entry;
  place = 0;
  do
  {
    checked_place = place & 0xFFFF;
    if ((*((u8 *) ((checked_place * 0x3C) + g_MapPartyRecords))) != 0)
    {
      entries_behind = checked_place * 0x10;
      checked_place = 0 & 0xFFFF;
      if (((s32) g_MapPartyRingHead[0]) < entries_behind)
      {
        entry_number = (g_MapPartyRingHead[0] + 0x60) - entries_behind;
      }
      else
      {
        entry_number = g_MapPartyRingHead[0] - entries_behind;
      }
      masked_place = place & 0xFFFF;
      record_offset = masked_place * 0x3C;
      entry_offset = (entry_number & 0xFFFF) * 8;
      entry = entry_offset + g_MapPartyRingEntries;
      ;
      *((s16 *) (((u8 *) ((masked_place * 0x3C) + g_MapPartyRecords)) + 0x6)) = (u16) (AS(s32, ((MapPartyRingEntry *)entry)->x));
      *((s16 *) (((u8 *) (record_offset + g_MapPartyRecords)) + 0xa)) = (s16) (AS(s16, ((MapPartyRingEntry *)entry)->y) - (*(((u8 *) entry) + 0x4)));
      *((s16 *) (((u8 *) ((masked_place * 0x3C) + g_MapPartyRecords)) + 0x8)) = (u16) (AS(s16, ((MapPartyRingEntry *)entry)->y));
      *((s16 *) (((u8 *) ((masked_place * 0x3C) + g_MapPartyRecords)) + 0xe)) = (u16) (AS(s16, ((MapPartyRingEntry *)entry)->class) | ((*((s16 *) (((u8 *) ((masked_place * 0x3C) + g_MapPartyRecords)) + 0xe))) & 0x9FFF));
      MapAnimationPoseSet((masked_place * 0x3C) + g_MapPartyRecords, 1, ((MapPartyRingEntry *)entry)->facing);
      MapPartyTwinSpotSet(masked_place, g_MapPartyRingEntries + entry_offset);
    }
    place += 1;
  }
  while (((u32) (place & 0xFFFF)) < 6U);
  bg_party_spot_set(((MapRecord *)g_MapPartyRecords)->sprite.x, AS(s32, ((MapRecord *)g_MapPartyRecords)->sprite.depth));
}

void MapAnimationPoseSet(void *, s16, u16);

/* func_800672E8 */
void MapPartyTwinSpotSet(s32 place, void *spot)
{
  void *twin_record;
  twin_record = (void *)((place & 0xFFFF) * 0x3C) + g_MapPartyTwins;
  if ((*(((u8 *) twin_record) + 0x0)) != 0)
  {
    ((MapRecord *)twin_record)->sprite.x = (u16) (AS(s32, ((MapPartyRingEntry *)spot)->x));
    ((MapRecord *)twin_record)->sprite.y = AS(s16, ((MapPartyRingEntry *)spot)->y);
    ((MapRecord *)twin_record)->sprite.depth = (u16) (AS(s16, ((MapPartyRingEntry *)spot)->y));
    ((MapRecord *)twin_record)->sprite.flags = (u16) (AS(s16, ((MapPartyRingEntry *)spot)->class) | (AS(s16, ((MapRecord *)twin_record)->sprite.flags) & 0x9FFF));
    MapAnimationPoseSet(twin_record, 1, ((MapPartyRingEntry *)spot)->facing);
  }
}
