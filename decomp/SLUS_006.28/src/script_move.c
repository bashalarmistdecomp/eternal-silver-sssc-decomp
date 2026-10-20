/* Lunar SSSC — script_move: the script reader's commands that drive the
   rows: their command lists, actors and ends, the party's and the views'
   follows, the attribute check, the mix groups and their fades. The first
   split had it in scripting_d.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "script.h"
#include "map.h"
#include "sprite.h"

/* at the top of the file: MoveChrSecondViewStep's prototype, well above
   its definition, names MoveChrRow */
extern u8 g_MapLayer4Mix;
s32 MapPartyRingEntryGet(s32, s32 *);
extern s32 g_MapObjectDefinitions;
s32 ScriptErrorReport();
extern s32 g_MoveChrLiveBits;
extern s32 g_PartyIds;
void MapAnimationPoseSet(s32, s32, s32);
extern s32 g_ScriptCommandPointer;
extern u8 g_MoveChrPartyFollow;
extern u8 g_MoveChrAttributeCheckOn;
extern u8 g_MoveChrSecondViewFollow;
s32 ScriptPointerAdvance(u8);  /* u8: the callee masks its argument on entry (andi 0xFF); every caller passes 2 or 4 */
s32 ScriptListCount(s32, s32);                              /* extern */
/* far_syms: g_MapPartyBoxDx (target reaches it %hi/%lo) */
extern u8 g_MapTwinSpriteDataNumbers[];
extern s16 g_ScriptErrorNumber;
extern s8 g_MapPartyBoxDx;
extern u16 g_MapPartyKind;
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
extern u16 g_MapEffect;
extern s32 g_MoveChrBusyBits;
extern void *g_MoveChrRows;
void ScriptCommand36Skip(void);  /* TU defines this below */
void ScriptCommand37Skip(void);  /* TU defines this below */
void MoveChrCommandEndWait(s32);
void MoveChrFollowStart(s32);
void ScriptCommandMoveChrPartyFollowStart(void);  /* TU defines this below */
void ScriptCommandMoveChrAttributeCheckOn(void);  /* TU defines this below */
void ScriptCommandMoveChrAttributeCheckOff(void);  /* TU defines this below */
extern u8 g_MoveChrViewFollow;
void ScriptCommandMoveChrViewFollowStart(void);  /* TU defines this below */
void MoveChrRowInit(s32);
void ScriptCommandMoveChrViewFollowEnd(void);  /* TU defines this below */
void ScriptCommandMoveChrCommandEndWait(void);  /* TU defines this below */
void ScriptCommand4ASkip(void);  /* TU defines this below */
void ScriptCommand4BSkip(void);  /* TU defines this below */
void ScriptCommandMoveChrSecondViewFollowStart(void);  /* TU defines this below */
extern s32 g_MapPartyRecords;
extern s32 g_MapPartyTwins;
extern s32 g_MoveChrMixGroup1Bits;
extern s32 g_MoveChrMixGroup2Bits;
extern s32 g_MoveChrOwnActors;
extern s32 g_MoveChrOwnActorTwins;
extern s32 g_MapSpriteDataEntries;
extern u8 g_MapTwinSpriteDataNumbers[];
s32 MapAttributeClassGet();
/* far_syms: g_MapPartyBoxDx */
s16 MapObjectFind(u8 definition_number);
extern s16 g_ScriptErrorNumber;
extern s8 g_MapPartyBoxDx;
extern u16 g_MapPartyKind;
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
extern u16 g_MapEffect;
extern s32 g_MoveChrBusyBits;
extern void *g_MoveChrRows;

void MapPartyRecordSpotSet(s32 i, MapPartyRingEntry *e);
/* func_80061C54 */
void ScriptCommandMoveChrCommandsStart(void)
{
  unsigned short i;
  s32 *row;
  MapStatusClear();
  g_ScriptCommandPointer += 2;
  for (i = 0; i < 0x12; i++)
  {
    if ((((u32) g_MoveChrLiveBits) >> i) & 1)
    {
      row = (s32 *) (((u8 *) g_MoveChrRows) + (i * 48));
      if (((s32 *) (((u8 *) g_MoveChrRows) + (i * 48)))[1] && (((s32 *) (((u8 *) g_MoveChrRows) + (i * 48)))[2] == 0))
      {
        g_MoveChrBusyBits |= 1 << i;
        ((s32 *) (((u8 *) g_MoveChrRows) + (i * 48)))[2] = ((s32 *) (((u8 *) g_MoveChrRows) + (i * 48)))[1];
      }
    }
  }

}

/* func_80061CF8 */
void ScriptCommandMoveChrRowsAllEnd(void)
{
    s32 ptr;
    u16 i;
    s32 val;
    u8 *p;

    ptr = g_ScriptCommandPointer;
    g_ScriptCommandPointer = ptr + 2;
    val = *(u8 *)(ptr + 2);
    g_ScriptCommandPointer = ptr + 4;
    if (val == 0)
    {
        MoveChrCommandEndWait(0x3FFFF);
    }
    else
    {
        g_MoveChrBusyBits = 0;
    }
    MoveChrFollowStart(0x2007F);

    for (i = 7; i < 0x12; i++)
    {
        if ((((u32)g_MoveChrLiveBits >> i) & 1) != 0)
        {
            if (i < 0xB)
            {
                p = (u8 *)(i * 0x30 + (s32)g_MoveChrRows);
                *(u8 *)(g_MapObjects + p[3] * 0x60 + 0x3E) = p[0x21];
                *(u8 *)(g_MapObjects + ((u8 *)g_MoveChrRows + i * 0x30)[3] * 0x60) &= 0x7F;
            }
            else
                *(u8 *)(g_MoveChrOwnActors + i * 0x60 - 0x420) = 0;

            *(u8 *)(g_MoveChrRows + i * 0x30) = 0;
            g_MoveChrLiveBits &= ~(1 << i);
        }
    }

    sprite_run_off(0x6, 0xB);
    sprite_run_off(0x4C, 0xB);
    layer_mix_set(0x4000, 0);
    layer_mix_set(0x8000, 0);

    if (g_MapEffect & 0x80)
    {
        layer_mix_set(0x4, g_MapLayer4Mix);
    }
    else
    {
        layer_mix_set(0x4, 0);
        layer_mix_set(0x4, 0x20);
    }
}

/* far_syms: g_MapTwinSpriteDataNumbers */

/* Script command move_cmd_table[0] (0x800B340C): bind script slot `slot`
   (operand ip[2]; a g_MoveChrRows 0x30 row, live bit in g_MoveChrLiveBits) to an
   actor. It does not dispatch -- the name is the splitter's. A slot that is
   already live reports error 0x10 through g_ScriptErrorNumber / ScriptErrorReport and
   then carries on: none of the three error paths returns.

   slot < 0xB -- a 4-byte command that binds an EXISTING map object:
   ip[3] is matched against the +1 byte of the live g_MapObjects rows
   (MapObjectFind; error 0xA when none matches). The slot points at that
   0x60 row, its 0x3C twin in g_MapObjectTwins and the row's +0x50 word, copies
   the row's +0x3C / +0x3E bytes into +0x20 / +0x21, and sets bit 0x80 of
   the row's first byte.

   slot >= 0xB -- a 16-byte command that SPAWNS a script-owned actor in row
   slot - 0xB of g_MoveChrOwnActors (0x60) / g_MoveChrOwnActorTwins (0x3C). ip[3] is a sprite
   sheet, an index into the 0x10-byte g_MapSpriteDataEntries records (error 9 when its
   pointer is null). ip[4] = MapRecordInit's kind (also stored at +4),
   ip[5] / ip[6] = x / y in 8-pixel units, ip[7] = the height taken off the
   draw y (+0xA; +8 keeps the ground y), ip[8] -> +0x3E (the byte handed to
   MapAnimationPoseSet), ip[9] -> +0x3C, ip[0xA] = MapRecordInit's mode, ip[0xB..0xE] = the four
   unaligned bytes copied to +0x50. MapAttributeClassGet's result is OR'd into
   +0xE once its bits 0x6000 are cleared. Sheets below 0xF also fill the twin row: a copy of the
   first 0x3C bytes, re-rendered with sheet g_MapTwinSpriteDataNumbers[sheet - 1].

   Shape: the two operand bytes are u8 and the row index a s16 -- the
   single `andi` copies ($s0, $s5) are cse temporaries, which is why they
   are recomputed after the 0x3C copy's loop label. The twin sheet is read
   into an int BEFORE the copy (the target loads $a3 ahead of the loop).
   Both arms store pC, p10, p14 in that order; the spawn arm's p14 store
   comes out first only because the scheduler moved it. */
/* func_80061EAC */
void ScriptCommandMoveChrRowActorSet(void)
{
    u8 *ip;
    u8 *next;
    u8 slot;
    u8 sheet;
    u8 kind;
    u8 mode;
    s16 idx;
    s32 twin;

    ip = (u8 *)g_ScriptCommandPointer;
    next = ip + 4;
    g_ScriptCommandPointer = (s32)(ip + 2);
    slot = ip[2];
    sheet = ip[3];
    if (((u32)g_MoveChrLiveBits >> slot) & 1)
    {
        g_ScriptErrorNumber = 0x10;
        ScriptErrorReport();
    }
    if (slot < 0xB)
    {
        idx = MapObjectFind(sheet);
        if (idx < 0)
        {
            g_ScriptErrorNumber = 0xA;
            ScriptErrorReport();
        }
        MoveChrRowInit(slot);
        ((MoveChrRow *)g_MoveChrRows)[slot].actor_number.object = idx;
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[slot].actor) = (MapObject *)g_MapObjects + idx;
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[slot].twin) = (MapRecord *)g_MapObjectTwins + idx;
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[slot].box) = &((MapObject *)g_MapObjects)[idx].box;
        ((MoveChrRow *)g_MoveChrRows)[slot].layer = ((MapObject *)g_MapObjects)[idx].layer;
        ((MoveChrRow *)g_MoveChrRows)[slot].facing = ((MapObject *)g_MapObjects)[idx].facing;
        ((MapObject *)g_MapObjects)[idx].record.on_kind.kind |= 0x80;
    }
    else
    {
        if (((s32 *)g_MapSpriteDataEntries)[sheet * 4] == 0)
        {
            g_ScriptErrorNumber = 9;
            ScriptErrorReport();
        }
        idx = slot - 0xB;
        MoveChrRowInit(slot);
        ((MoveChrRow *)g_MoveChrRows)[slot].actor_number.object = sheet;
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[slot].actor) = (MapObject *)g_MoveChrOwnActors + idx;
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[slot].twin) = (MapRecord *)g_MoveChrOwnActorTwins + idx;
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[slot].box) = &((MapObject *)g_MoveChrOwnActors)[idx].box;
        ((MapObject *)g_MoveChrOwnActors)[idx].record.on_kind.kind = 1;
        kind = ip[4];
        ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.x = ip[5] << 3;
        ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.y = ip[6] << 3;
        ((MoveChrRow *)g_MoveChrRows)[slot].height = ip[7];
        AS(u16, ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.depth) = ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.y;
        ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.y -= ((MoveChrRow *)g_MoveChrRows)[slot].height;
        ((MapObject *)g_MoveChrOwnActors)[idx].facing = ip[8];
        ((MoveChrRow *)g_MoveChrRows)[slot].facing = ((MapObject *)g_MoveChrOwnActors)[idx].facing;
        ((MoveChrRow *)g_MoveChrRows)[slot].layer = ((MapObject *)g_MoveChrOwnActors)[idx].layer = ip[9];
        mode = ip[0xA];
        MapRecordInit((MapObject *)g_MoveChrOwnActors + idx, kind, mode, sheet);
        ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.on = kind;
        MapAnimationPoseSet((s32)AS(void *, ((MoveChrRow *)g_MoveChrRows)[slot].actor), 1, ((MapObject *)g_MoveChrOwnActors)[idx].facing);
        ((MapObject *)g_MoveChrOwnActors)[idx].number = 2;
        ((MapObject *)g_MoveChrOwnActors)[idx].turn_count = 0;
        ((MapObject *)g_MoveChrOwnActors)[idx].box = *(MapBox *)(ip + 0xB);
        ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.flags = MapAttributeClassGet(1, 0x20,
            ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.x, ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.y,
            AS(u16, ((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.depth), &((MapObject *)g_MoveChrOwnActors)[idx].box,
            ((MapObject *)g_MoveChrOwnActors)[idx].layer) | (((MapObject *)g_MoveChrOwnActors)[idx].record.sprite.flags & 0x9FFF);
        next = ip + 0x10;
        if (sheet < 0xF)
        {
            twin = g_MapTwinSpriteDataNumbers[sheet - 1];
            ((MapRecord *)g_MoveChrOwnActorTwins)[idx] = ((MapObject *)g_MoveChrOwnActors)[idx].record;
            MapRecordInit((MapRecord *)g_MoveChrOwnActorTwins + idx, kind, mode, twin);
            ((MapRecord *)g_MoveChrOwnActorTwins)[idx].sprite.on = kind;
            MapAnimationPoseSet((s32)AS(void *, ((MoveChrRow *)g_MoveChrRows)[slot].twin), 1, ((MapObject *)g_MoveChrOwnActors)[idx].facing);
        }
    }
    g_ScriptCommandPointer = (s32)next;
}
/* Script command move_cmd_table[1] (0x800B3410): release script slots --
   the inverse of move_cmd_table[0] (ScriptCommandMoveChrRowActorSet). The operand is
   a zero-terminated list of slot ids (ScriptPointerAdvance(2) = the list,
   ScriptListCount(1, 1) = its length; the cursor ends past it). Per id, in
   list order:

   MoveChrCommandEndWait(1 << id) first waits out the slot's mover. A slot that is
   not live (bit id of g_MoveChrLiveBits clear) reports error 0xF through
   g_ScriptErrorNumber / ScriptErrorReport and RETURNS: the rest of the list is not
   processed and the closing ScriptFrameStep(1) is not made.

   id < 0xB -- a bound map object, row +3 of the slot in g_MapObjects: the
   slot's +0x21 goes back to the row's +0x3E and bit 0x80 of the row's
   first byte is cleared (the bind set it). If +1 of the row's 0x26-byte
   definition (g_MapObjectDefinitions[row +1]) is below 0xF, the row's first byte is
   also copied to its 0x3C twin in g_MapObjectTwins.

   id >= 0xB -- a script-spawned actor: the first byte of row id - 0xB of
   g_MoveChrOwnActors and of its twin in g_MoveChrOwnActorTwins is cleared (actor off).

   Either way the slot's first byte and its bit in g_MoveChrLiveBits are cleared.
   After the whole list: ScriptFrameStep(1), one frame.

   Shape: the id is not held in a local -- *src is re-read after every
   byte store. The row pointer and the definition pointer are both taken
   BEFORE the +0x3E store (the target loads g_MapObjectDefinitions ahead of it and
   reads the row's +1 through the first g_MapObjects load), while the
   first-byte updates index g_MapObjects again. The row pointer is integer
   arithmetic on the s32 global, index first (`addu $a0, $a3, $a0`). */
/* func_80062314 */
void ScriptCommandMoveChrRowsEnd(void)
{
    u8 *src;
    s32 n;
    s32 i;
    u8 idx;
    MapObject *obj;
    u8 *rec;

    src = (u8 *)ScriptPointerAdvance(2);
    n = ScriptListCount(1, 1);
    for (i = 0; i < n; i++)
    {
        MoveChrCommandEndWait(1 << *src);
        if (!(((u32)g_MoveChrLiveBits >> *src) & 1))
        {
            g_ScriptErrorNumber = 0xF;
            ScriptErrorReport();
            return;
        }
        if (*src < 0xB)
        {
            idx = ((MoveChrRow *)g_MoveChrRows)[*src].actor_number.object;
            obj = (MapObject *)(idx * 0x60 + g_MapObjects);
            rec = (u8 *)g_MapObjectDefinitions + obj->record.hold_definition.definition * 0x26;
            obj->facing = ((MoveChrRow *)g_MoveChrRows)[*src].facing;
            ((MapObject *)g_MapObjects)[idx].record.on_kind.kind &= 0x7F;
            if (rec[1] < 0xF)
            {
                ((MapRecord *)g_MapObjectTwins)[idx].on_kind.kind = ((MapObject *)g_MapObjects)[idx].record.on_kind.kind;
            }
        }
        else
        {
            ((MapObject *)g_MoveChrOwnActors)[*src - 0xB].record.on_kind.kind = 0;
            ((MapRecord *)g_MoveChrOwnActorTwins)[*src - 0xB].on_kind.kind = 0;
        }
        ((MoveChrRow *)g_MoveChrRows)[*src].on = 0;
        g_MoveChrLiveBits &= ~(1 << *src);
        src++;
    }
    ScriptFrameStep(1);
}

/* func_800624DC */
void ScriptCommand36Skip(void) {
    ScriptPointerAdvance(2);
    ScriptListCount(1, 1);
}

/* func_80062508 */
void ScriptCommand37Skip(void) {
    ScriptPointerAdvance(2);
    ScriptListCount(1, 1);
}

/* func_80062534 */
void ScriptCommandMoveChrCommandsSet(void)
{
    s32 base;
    u8 idx;
    u8 *ptr;

    MapStatusClear();
    base = g_ScriptCommandPointer;
    g_ScriptCommandPointer = base + 2;
    idx = *(u8 *)(base + 2);
    ptr = (u8 *)(base + 3);
    if (!(((u32)g_MoveChrLiveBits >> idx) & 1))
    {
        g_ScriptErrorNumber = 0xF;
        ScriptErrorReport();
    }
    else
    {
        MoveChrCommandEndWait(1 << idx);
        *(s32 *)((char *)g_MoveChrRows + idx * 48 + 4) = (s32)ptr;
    }
    while (*ptr != 0xF9)
    {
        switch (*ptr - 0xF0)
        {
            case 0:
            case 1:
            case 2:
            case 5:
            case 6:
                ptr += 2;
                break;
            case 3:
            case 4:
            case 7:
                ptr += 5;
                break;
            case 8:
                ptr += 4;
                break;
        }
    }
    g_ScriptCommandPointer = ((s32)ptr & 1) ? ((s32)ptr + 1) : ((s32)ptr + 2);
}

void MoveChrCommandEndWait(s32);                                 /* extern */
void MoveChrFollowStart(s32);                        /* extern */

/* func_80062640 */
void ScriptCommandMoveChrPartyFollowStart(void) {
    g_ScriptCommandPointer += 2;
    if (g_MoveChrPartyFollow == 0) {
        MoveChrCommandEndWait(0x7E);
        MoveChrFollowStart(0x7E);
    }
}

/* func_80062680 */
void ScriptCommandMoveChrPartyFollowEnd(void)
{
  s32 i;
  s32 offset;
  s32 idx;
  s32 base;
  s32 prev;
  s32 addr;
  s32 val2;
  MapPartyRingEntry spot;

  g_ScriptCommandPointer += 2;
  if (g_MoveChrPartyFollow == 0)
    return;

  MoveChrCommandEndWait(0x7E);
  g_MoveChrPartyFollow = 0;

  i = 2;
  offset = 0x3C;
  idx = 2;
  base = 0x60;

  while (i < 7)
  {
    prev = i - 1;

    if (*(u16 *)(idx + g_PartyIds) == 0)
      return;

    MoveChrRowInit(i & 0xFF);

    if (g_MapPartyKind == 2)
    {
      MapPartyRingEntryGet(0, &spot);
      MapPartyRecordSpotSet(prev, &spot);
    }
    else
    {
      MapPartyRingEntryGet(prev & 0xFFFF, &spot);
    }

    i++;

    *(u8 *)(base + (s32)(u8 *)g_MoveChrRows + 3) = *(u8 *)(idx + g_PartyIds);
    idx += 2;

    addr = base + (s32)(u8 *)g_MoveChrRows;
    val2 = g_MapPartyTwins + offset;
    AS(s32, ((MoveChrRow *)addr)->actor) = g_MapPartyRecords + offset;
    AS(s32, ((MoveChrRow *)addr)->twin) = val2;
    AS(s32, ((MoveChrRow *)addr)->box) = (s32)&g_MapPartyBoxDx;
    ((MoveChrRow *)addr)->facing = spot.facing;
    offset += 0x3C;
    *(u8 *)(base + (s32)(u8 *)g_MoveChrRows + 2) = spot.height;
    base += 0x30;
  }
}

extern s32 g_MapPartyRecords;
extern s32 g_MapPartyTwins;

/* func_800627C0 */
void MapPartyRecordSpotSet(s32 i, MapPartyRingEntry *e) {
    s32 offset;
    s32 id;

    id = *(u16 *)(g_PartyIds + i * 2);
    if (id == 0) {
        return;
    }
    offset = i * 0x3C;
    ((MapRecord *)(offset + g_MapPartyRecords))->on_kind.on = id;
    ((MapRecord *)(offset + g_MapPartyRecords))->sprite.x = e->x;
    ((MapRecord *)(offset + g_MapPartyRecords))->sprite.y = e->y - e->height;
    ((MapRecord *)(offset + g_MapPartyRecords))->sprite.depth = e->y;
    MapAnimationPoseSet((void *)(offset + g_MapPartyRecords), 1, e->facing);
    if (id >= 0x1F) {
        return;
    }
    if (g_MapPartyKind == 0) {
        return;
    }
    ((MapRecord *)(offset + g_MapPartyTwins))->on_kind.on = ((MapRecord *)(offset + g_MapPartyRecords))->on_kind.on;
    ((MapRecord *)(offset + g_MapPartyTwins))->sprite.x = e->x;
    ((MapRecord *)(offset + g_MapPartyTwins))->sprite.y = e->y;
    ((MapRecord *)(offset + g_MapPartyTwins))->sprite.depth = e->y;
    MapAnimationPoseSet((void *)(offset + g_MapPartyTwins), 1, e->facing);
}

/* func_800628C4 */
void ScriptCommandMoveChrAttributeCheckOn(void) {
    g_MoveChrAttributeCheckOn = 1;
    g_ScriptCommandPointer += 2;
}

/* func_800628E0 */
void ScriptCommandMoveChrAttributeCheckOff(void) {
    g_MoveChrAttributeCheckOn = 0;
    g_ScriptCommandPointer += 2;
}

extern u8 g_MoveChrViewFollow;

/* func_800628F8 */
void ScriptCommandMoveChrViewFollowStart(void) {
    g_ScriptCommandPointer += 2;
    if (g_MoveChrViewFollow == 0) {
        MoveChrCommandEndWait(0x2007F);
        MoveChrFollowStart(1);
    }
}

void MoveChrRowInit(s32);                                 /* extern */

/* func_8006293C */
void ScriptCommandMoveChrViewFollowEnd(void) {
    g_ScriptCommandPointer += 2;
    if (g_MoveChrViewFollow != 0) {
        MoveChrCommandEndWait(0x2007F);
        g_MoveChrViewFollow = 0;
        MoveChrRowInit(0);
    }
}

/* func_80062984 */
void ScriptCommandMoveChrCommandEndWait(void) {
    MoveChrCommandEndWait(*(s32 *)ScriptPointerAdvance(4));
    g_ScriptCommandPointer += 4;
}

/* func_800629B8 */
void ScriptCommand4ASkip(void) {
    ScriptPointerAdvance(2);
    ScriptListCount(1, 1);
}

/* func_800629E4 */
void ScriptCommand4BSkip(void) {
    ScriptPointerAdvance(2);
    ScriptListCount(1, 1);
}

/* func_80062A10 */
void ScriptCommandMoveChrSecondViewFollowStart(void) {
    g_ScriptCommandPointer += 2;
    if (g_MoveChrSecondViewFollow == 0) {
        MoveChrCommandEndWait(0x2007F);
        MoveChrFollowStart(0x20000);
    }
}

/* func_80062A54 */
void ScriptCommandMoveChrSecondViewFollowEnd(void)
{
    /* MARKED SPELLING (unused local): nothing reads or writes `unused`. The target's
       frame is 32 bytes and this function needs 24: an unused local that keeps its slot
       (this one, or an array such as `u8 pad[4]`) gives the 32, and without it 4 of this
       function's 30 instruction lines differ. What the original kept in the slot the
       bytes do not say. */
    volatile s32 unused;
    g_ScriptCommandPointer += 2;
    if (g_MoveChrSecondViewFollow != 0)
    {
        if (((u32)((g_MapEffect & 3) - 1)) >= 2U)
        {
            g_ScriptErrorNumber = 0x13;
            ScriptErrorReport();
            return;
        }
        MoveChrCommandEndWait(0x2007F);
        g_MoveChrSecondViewFollow = 0;
        MoveChrRowInit(0x11);
    }
}
/* la_absolute: ScriptCommandMixGroupSet */

/* Script command 0x4E -- g_ScriptCommandFunctions[0x4E] (slot 0x800B3474, the splitter's
   event_trigger_table[0]): put mover slots in one of the two colour-ramp
   GROUPS. The command is `[u16 0x4E][group][slot + 1]...[0]`: a byte list,
   its first byte the group (1 or 2), the rest mover slot numbers + 1.

   First the wait: one field frame a pass (ScriptFrameStep(1)) while
   layer_mix_fade_check(0xC00C) answers non-zero. 0xC00C is the four ramp layers
   command 0x50 can start (below); layer_mix_fade_check as it stands looks only
   at the 8 bit, and only in battle (g_BtlRunning != 0: tile_mix_fade_check's
   answer), so in the field this never waits. Then the cursor g_ScriptCommandPointer
   steps over the id and
   ScriptListCount(1, 1) walks the list (its result is the list's length,
   the group byte included; the cursor ends past the zero).
   For every slot byte, with n = byte - 1 and g = group - 1:
     - bit n is set in the group's own mask word and cleared in the other
       one: the two words are g_MoveChrMixGroup1Bits (g = 0) and g_MoveChrMixGroup2Bits (g = 1);
     - unless n is 0 (the camera) or 0x11 (the second scroll), the slot's
       actor row (+0xC of its 0x30-byte g_MoveChrRows row) and its twin
       (+0x10) get bits 10..12 of their draw word +0xE set to g + 6
       (0x1800 for group 1, 0x1C00 for group 2).
   No RNG draw; the only frames are the wait's.

   The other two commands of the set: ScriptCommandMixGroupClear (0x4F) takes slots
   out again -- the bit cleared in both words, the three draw-word bits
   cleared. ScriptCommandMixGroupFadeStart (0x50, below) ramps a group:
   `[u16 0x50][group][level][frames]` calls layer_mix_fade_start(layers, 1,
   frames, level * 2 (0x1F for level 0xF)) with layers = 4 if the group's
   word has bit 0, 8 if it has bit 0x11, and 0x4000 for group 1 or 0x8000
   for group 2 -- that is 0x100 << the value written to the draw word
   here.

   Shape (each measured by changing it alone):
     - The two mask words are indexed through a pointer local set INSIDE
       the loop: loop.c then moves `la`, the index's zero-extension, the
       shift and the add out in that order, the target's. `(&g_MoveChrMixGroup1Bits)
       [g]` written out puts the zero-extension ahead of the `la`
       (90/91); the pointer set before the loop, or in the for's init,
       puts the `la` ahead of the loop's entry test (81/91). The `la` is
       the assembler's same-register lui/addiu of a small-data symbol
       (la_absolute); g_MoveChrMixGroup1Bits keeps this TU's scalar declaration.
     - `bit = 1 << n` is a variable set before the mask stores: the
       constant 1 is then the loop's first invariant and moves at the full
       threshold (58 * 1 * 1 >= 51 insns); written at the stores it comes
       after four other moves and stays in the loop (87/91).
     - The row is `((MoveChrRow *)g_MoveChrRows)[n]` at each of the four stores,
       no row pointer local: the load of g_MoveChrRows is then emitted ahead
       of the n * 0x30 it is added to (lifetime 4), which is what lets it
       move out of the loop; with a row pointer local it is reloaded
       every pass (83/91). The stores are struct member stores, so the
       hoisted load survives them.
     - `&= 0xE3FF` and `|=` are two statements (one expression: 82/91).
     - g is a u8 local, masked at each of its three uses. */
/* func_80062ACC */
void ScriptCommandMixGroupSet(void)
{
    u8 *p;
    u8 *q;
    s32 *m;
    s32 count;
    s32 i;
    s32 n;
    s32 bit;
    u8 g;

    while (layer_mix_fade_check(0xC00C) & 0xFF) {
        ScriptFrameStep(1);
    }
    p = (u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer += 2;
    count = ScriptListCount(1, 1);
    q = p + 3;
    g = p[2] - 1;
    for (i = 0; i < count - 1; i++) {
        n = *q++ - 1;
        bit = 1 << n;
        m = &g_MoveChrMixGroup1Bits;
        m[g] |= bit;
        m[g ^ 1] &= ~bit;
        if (n != 0 && n != 0x11) {
            ((MoveChrRow *)g_MoveChrRows)[n].actor->sprite.flags &= 0xE3FF;
            ((MoveChrRow *)g_MoveChrRows)[n].actor->sprite.flags |= (g + 6) << 10;
            ((MoveChrRow *)g_MoveChrRows)[n].twin->sprite.flags &= 0xE3FF;
            ((MoveChrRow *)g_MoveChrRows)[n].twin->sprite.flags |= (g + 6) << 10;
        }
    }
}

extern u8 g_MapMix;

/* func_80062C38 */
void ScriptCommandMixGroupClear(void)
{
  u8 *ptr;
  s32 x;
  s32 count;
  s32 i;
  s32 idx;
  s32 mask;
  u8 val;
  u8 *base;
  u32 sub;
  u16 flags;
  u8 byte;
  u8 byte_val;
  u32 sub2;
  s32 mode_address;
  ScriptCommandMixFadeWait();
  x = 1;
  ptr = (u8 *) (*((volatile s32 *) (&g_ScriptCommandPointer)));
  count = ScriptListCount(x, x);
  byte = 0;
  i = 0;
  if (count <= byte)
  {
    return;
  }
  while (i < count)
  {
    val = *ptr;
    idx = val - 1;
    mask = ~(1 << idx);
    g_MoveChrMixGroup1Bits &= mask;
    g_MoveChrMixGroup2Bits &= mask;
    ptr++;
    if (idx == 0)
    {
      if (g_MapEffect & 0x80)
      {
        layer_mix_set(4, g_MapLayer4Mix);
      }
      else
      {
        layer_mix_set(4, 0);
        layer_mix_set(4, 0x20);
      }
    }
    else
      if (idx == 0x11)
    {
      layer_mix_set(8, g_MapMix);
      if ((*((u16 *) (&g_MapMix))) == 0)
      {
        layer_mix_set(8, 0x20);
      }
    }
    else
    {
      base = (u8 *) ((idx * 48) + ((s32) g_MoveChrRows));
      sub = AS(u32, ((MoveChrRow *)base)->actor);
      flags = ((MapRecord *)sub)->sprite.flags;
      flags &= 0xE3FF;
      ((MapRecord *)sub)->sprite.flags = flags;
      byte = ((MoveChrRow *)base)->actor_number.object;
      mode_address = (g_MapObjectDefinitions + (byte * 38)) + 3;
      byte_val = *((u8 *) mode_address);
      if (byte_val == 1)
      {
        sub = AS(u32, ((MoveChrRow *)base)->actor);
        flags = ((MapRecord *)sub)->sprite.flags;
        flags |= 0x800;
        ((MapRecord *)sub)->sprite.flags = flags;
      }
      else
        if (byte_val == 2)
      {
        sub = AS(u32, ((MoveChrRow *)base)->actor);
        flags = ((MapRecord *)sub)->sprite.flags;
        flags |= 0xC00;
        ((MapRecord *)sub)->sprite.flags = flags;
      }
      else
      {
        sub2 = *((u32 *) ((((u8 *) g_MoveChrRows) + ((2 + idx) * 16)) + 0x10));
      }
      sub2 = *((u32 *) ((((u8 *) g_MoveChrRows) + (((idx * 2) + idx) * 16)) + 0x10));
      flags = ((MapRecord *)sub2)->sprite.flags;
      flags &= 0xE3FF;
      ((MapRecord *)sub2)->sprite.flags = flags;
    }
    i++;
  }

}
/* la_absolute: ScriptCommandMixGroupFadeStart */

/* ScriptCommandMixGroupFadeStart -- Event-script command 0x50
   (g_ScriptCommandFunctions[0x50], the splitter's event_trigger_table[2]): START A GROUP'S
   COLOUR RAMP. Six bytes, [u16 0x50][group][level][frames][pad]; the group
   is 1 or 2 (command 0x4E, ScriptCommandMixGroupSet, puts mover slots in a group).

   MapStatusClear() first (leave the idle state). g = group - 1. The level
   byte gives the ramp's target: 0xF -> 0x1F, anything else level * 2. The
   group's membership word m[g] (g_MoveChrMixGroup1Bits / g_MoveChrMixGroup2Bits) picks the layers:
   bit 0 (the camera is in the group) -> layer 4, bit 0x11 (the second
   scroll) -> layer 8, and the group's own actor layer, 0x4000 for group 1,
   0x8000 for group 2. Then layer_mix_fade_start(layers, 1, frames, (s8)target) and
   the cursor steps over the command (2 at the top, 4 here).

   What the bytes need:
   - the level byte is READ AGAIN in the arm that doubles it, not kept in a
     local: cse turns the second read into a register copy, which makes that
     arm two instructions and keeps the if / else a real diamond (a local:
     35/47, the arm becomes a delay-slot shift);
   - `!= 0xF` first, so the doubling arm falls through;
   - the frame count read into an int local right after the word (read in
     the argument list: 40/47);
   - the word through a pointer local, m = &g_MoveChrMixGroup1Bits (ScriptCommandMixGroupSet's
     spelling), under la_absolute -- the target's same-register lui / addiu
     is the assembler's `la`. */
/* func_80062E00 */
void ScriptCommandMixGroupFadeStart(void)
{
    u8 *p;
    u8 *q;
    s32 frames;
    s32 *m;
    s32 word;
    s32 layers;
    s8 lv;
    u8 g;

    MapStatusClear();
    p = (u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer += 2;
    g = p[2] - 1;
    q = p + 3;
    if (*q != 0xF) {
        lv = *q * 2;
    } else {
        lv = 0x1F;
    }
    m = &g_MoveChrMixGroup1Bits;
    word = m[g];
    frames = q[1];
    layers = (word & 1) << 2;
    if (word & 0x20000) {
        layers |= 8;
    }
    if (g == 0) {
        layers |= 0x4000;
    } else {
        layers |= 0x8000;
    }
    layer_mix_fade_start(layers, 1, frames, lv);
    g_ScriptCommandPointer += 4;
}
/* Script command 0x51 -- g_ScriptCommandFunctions[0x51] (slot 0x800B3480): the colour-ramp
   wait on its own. The cursor g_ScriptCommandPointer steps over the two-byte command
   (the id word, no operands), then one field frame a pass (ScriptFrameStep(1))
   while layer_mix_fade_check(0xC00C) answers non-zero -- the wait that commands
   0x4E and 0x4F open with (ScriptCommandMixGroupSet above; 0x4F's handler
   ScriptCommandMixGroupClear calls this function for it, so its cursor has already
   stepped over its own id when it reads the list). 0xC00C is the four ramp
   layers command 0x50 can start. No RNG, no other state.

   The load of the cursor is the function's first instruction, ahead of the
   `addiu $sp`: a plain `+= 2` gives that by itself. */
/* func_80062EBC */
void ScriptCommandMixFadeWait(void)
{
    g_ScriptCommandPointer += 2;
    while (layer_mix_fade_check(0xC00C) & 0xFF) {
        ScriptFrameStep(1);
    }
}
