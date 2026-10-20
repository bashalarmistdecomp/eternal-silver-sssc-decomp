/* Lunar SSSC — map_object: the map's objects made: restored from a save,
   their data and pages loaded, made from their definitions, their records,
   their moves set, found and a free row found. The first split had it in
   scripting_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"
extern u8 g_SpriteData1[];
extern u8 g_MapObjectsDataResourceNumbers[];
s32 StoryBitCheck(s32);

extern u16 g_MapObjectsDataNumber;
extern u8 g_MapTwinSpriteDataNumbers[];
extern s32 g_ResourceReadBuffer;
extern u16 g_MapNumber;
s32 mem_clear(s32, s32);                            /* extern */
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
extern s32 g_MapSpriteDataEntries;
s32 resource_load(s32, s32);
/* far_syms: g_MapTwinSpriteDataNumbers */
s32 xmalloc_heap_alloc(s32);
s32 inflate_buffer(s32, s32);
/* far_syms: g_MapObjectsDataResourceNumbers */
void sprite_page_list_load();
/* far_syms: g_MapTwinSpriteDataNumbers (target reaches it %hi/%lo) */
extern s32 g_MapObjectDefinitions;
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
void MapRecordInit(void *record, s32 kind, s32 mode, s32 sprite_data_number);
void MapRecordSpriteDataSet(void *record, s32 sprite_data_number);  /* extern */

void MapObjectsDataLoad(u8);                               /* extern */
void MapObjectRestore(u8 *, s32, s32);                /* extern */
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;

/* func_80065098 */
void MapObjectsRestore(s32 set_number, u8 *entries) {
    s32 set;
    s32 row_offset;
    s32 twin_offset;
    s32 i;
    u8 *entry;

    entry = entries;
    set = set_number & 0xFF;
    MapObjectsDataLoad(set);
    i = 0;
    if (set != 0) {
        twin_offset = 0;
        row_offset = 0;
        do {
            if (*entry != 0) {
                MapObjectRestore(entry, g_MapObjects + row_offset, g_MapObjectTwins + twin_offset);
            }
            twin_offset += 0x3C;
            row_offset += 0x60;
            i += 1;
            entry += 0x28;
        } while (i < 0x80);
    }
}
/* MapAttributeClassGet's box: the +0x50 bytes of a 0x60 map-object row. The
   typedef and the prototype sit here, above the function's FIRST caller
   in this file (MapObjectRestore; then MapObjectInit and
   MapPartyRingFill); the function itself is further down. So no caller
   binds an implicit `int MapAttributeClassGet()` and the TU has no
   prototype after first use; the bytes are the same either way. */
s32 MapAttributeClassGet(u16 step, u16 top, s32 x, s32 y, u16 ground, MapBox *box, s32 layer);  /* extern */
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);  /* extern */
void MapRecordInit(void *record, s32 kind, s32 mode, s32 sprite_data_number);  /* TU defines this below */

/* MapObjectRestore -- the name is the splitter's: this SPAWNS ONE MAP
   OBJECT from a map's object list. MapObjectsRestore (called by GameEnter
   with the list) runs it for each of up to 0x80 0x28-byte entries whose
   first byte is non-zero, with row i of g_MapObjects (0x60) and of
   g_MapObjectTwins (its 0x3C twin).

   src (0x28): +0 the row's live/kind byte, +1 an index into the 0x26-byte
   definitions at g_MapObjectDefinitions, +2 x, +4 ground y, +6 draw y, +8 a facing
   count, +0xA.. the bytes copied to the row's +0x3D.. (see the stores),
   +0x18.. six u16 and +0x24 a word copied to +0x4C.. / +0x5C.
   info (g_MapObjectDefinitions[src +1], 0x26): +0 bit 0x80 = MapAttributeClassGet's layer
   (row +0x3C), +1 the sprite sheet (0 is used instead when the kind is 7),
   +3 MapRecordInit's mode, +6 the twin's dispatch byte less one,
   +7 the four unaligned footprint bytes copied to the row's +0x50.

   Order: live and +1 copied, MapRecordInit(row, kind, mode, sheet),
   the position and the +0x3C / +0x3D / +0x3E / +0x42 bytes, the footprint
   box, MapObjectMoveSet(row, info, 0), the rest of the copies, then
   n = (src +8 - 1) / 4 + 1, the footprint class
   (MapAttributeClassGet(1, 0x20, x, drawY, y, &row+0x50, row+0x3C) OR'd into
   +0xE's bits 0x6000) and MapAnimationPoseSet(row, n, row +0x3E).
   A sheet below 0xF also fills the twin: the row's first 0x3C bytes,
   rendered again with sheet g_MapTwinSpriteDataNumbers[sheet - 1], and
   MapAnimationPoseSet(twin, n, info +6 + 1). The same steps as
   ScriptCommandMoveChrRowActorSet's spawn arm (script_move.c), fed from the map's
   list instead of a script command. No RNG draw and no frame wait in its
   call tree.

   Shape: n is an int. With it, expand computes src +8 - 1 into n, which
   makes expand_divmod drop n as the quotient's target (the operand
   mentions it), so the quotient lands in a temporary and only the `+ 1`
   sets n -- a global pseudo that local-alloc cannot tie to the
   temporaries. As a s16 the whole sum is one local quantity that crosses
   the calls, local-alloc gives it $s0 before global-alloc runs, and obj
   and src come out swapped ($s1 / $s0). The sll/sra at each call are the
   s16 parameter of MapAnimationPoseSet's prototype, which is therefore
   in scope here. The twin's sheet is read into a local BEFORE the struct
   copy (the target loads $a3 ahead of the copy loop), as in
   ScriptCommandMoveChrRowActorSet. The `+ 1` adding the register that holds
   MapAttributeClassGet's first argument (addu $s1, $v0, $a0) is the compiler's
   own; nothing in the source asks for it.

   The parameters keep the types of the TU's forward declaration and of
   the converted caller MapObjectsRestore (u8 *, s32, s32); src / obj / twin
   are casts of them, as in-body macros. Typed pointer LOCALS copied from
   the parameters do not match (the copies change the allocation, TU
   .text -4). */
/* func_80065128 */
void MapObjectRestore(u8 *entry, s32 row, s32 twin_record)
{
#define src ((MapObjectSaveEntry *)entry)
#define obj ((MapObject *)row)
#define twin ((MapRecord *)twin_record)
    MapObjectDefinition *info;
    u8 kind;
    s32 sheet;
    s32 n;
    s32 twinsheet;

    kind = src->kind;
    info = (MapObjectDefinition *)g_MapObjectDefinitions + src->definition;
    obj->record.on_kind.kind = kind;
    obj->record.hold_definition.definition = src->definition;
    sheet = 0;
    if (kind != 7) {
        sheet = info->sprite_data;
    }
    MapRecordInit(obj, obj->record.on_kind.kind, info->mode, sheet);
    obj->record.sprite.x = src->x;
    obj->record.sprite.y = src->y;
    AS(u16, obj->record.sprite.depth) = src->depth;
    obj->layer = info->kind & 0x80;
    obj->number = src->number;
    obj->facing = src->facing;
    obj->turn_count = src->turn_count;
    obj->box = info->box;
    MapObjectMoveSet(obj, info, 0);
    obj->speed = src->speed;
    obj->interval = src->interval;
    obj->look = src->look;
    obj->sight_width = src->sight_width;
    obj->sight_length = src->sight_length;
    obj->length = src->length;
    obj->count = src->count;
    obj->state = src->state;
    obj->thinks_left = src->thinks_left;
    obj->facing_kept = src->facing_kept;
    obj->turn_plan = src->turn_plan;
    AS(u16, obj->target.spot.x) = src->target[0];
    AS(u16, obj->target.spot.y) = src->target[1];
    AS(u16, obj->move.bounds.left) = src->move[0];
    AS(u16, obj->move.bounds.right) = src->move[1];
    AS(u16, obj->move.bounds.top) = src->move[2];
    AS(u16, obj->move.bounds.bottom) = src->move[3];
    obj->step_list = src->step_list;
    n = (src->animation - 1) / 4 + 1;
    obj->record.sprite.flags = MapAttributeClassGet(1, 0x20, obj->record.sprite.x, obj->record.sprite.y, AS(u16, obj->record.sprite.depth), &obj->box, obj->layer) | (obj->record.sprite.flags & 0x9FFF);
    MapAnimationPoseSet(obj, n, obj->facing);
    if (info->sprite_data < 0xF) {
        twinsheet = g_MapTwinSpriteDataNumbers[info->sprite_data - 1];
        *twin = obj->record;
        MapRecordInit(twin, obj->record.on_kind.kind, info->mode, twinsheet);
        MapAnimationPoseSet(twin, n, info->facing + 1);
    }
#undef src
#undef obj
#undef twin
}

/* func_80065418 */
void MapObjectsDataLoad(u8 set_number)
{
    s32 *hdr;
    MapObjectsSpriteDataRow *list;
    u16 *p;
    s32 id;
    s32 src;
    s32 dst;
    s32 i;

    g_MapObjectsDataNumber = set_number;
    mem_clear(g_MapObjects, 0x3000);
    mem_clear(g_MapObjectTwins, 0x1E00);
    if (g_MapObjectsDataNumber != 0) {
        p = (u16 *)g_MapObjectsDataResourceNumbers;
        hdr = (s32 *)g_ResourceReadBuffer;
        resource_load(p[set_number], (s32)hdr);
        inflate_buffer(g_ResourceReadBuffer + hdr[0], g_MapObjectDefinitions);
        list = (MapObjectsSpriteDataRow *)(g_ResourceReadBuffer + hdr[1]);
        for (i = 0; list[i].sprite_data != 0; i++) {
            id = list[i].sprite_data;
            if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data == 0 && id < 0x100) {
                src = g_ResourceReadBuffer + hdr[1] + list[i].place * 4;
                dst = xmalloc_heap_alloc(list[i].length * 4);
                inflate_buffer(src, dst);
                ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data = dst;
            }
        }
        ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[0].data = (s32)g_SpriteData1;
        sprite_page_list_load();
        for (i = 0xFF; i >= 0; i--) {
        }
    }
}

/* far_syms: g_MapObjectsDataResourceNumbers (target reaches it %hi/%lo) */

/* func_8006557C */
void MapObjectsPagesLoad(s32 set_number) {
    u16 *p;
    if (g_MapObjectsDataNumber) {
        p = (u16 *)g_MapObjectsDataResourceNumbers;
        resource_load(p[set_number & 0xFF], g_ResourceReadBuffer);
        sprite_page_list_load();
    }
}
void MapObjectInit(void *row, void *twin_record, void *definition, u8 definition_number);  /* TU defines this below */

/* MapObjectsInit -- SPAWNS A MAP'S OBJECTS FROM ITS OBJECT SET. The one caller is
   MapChange (the scene change, map_change.c) with the scene entry's
   byte +1, the object set.

   MapObjectsDataLoad(set) first: it records the set, zeroes the two map-object
   tables and, for a set other than 0, loads the set's resource and
   inflates its 0x80 definitions (0x26 bytes each) to g_MapObjectDefinitions. Set 0
   stops there -- no objects.

   Otherwise the 0x80 definitions are walked in order, i = 0..0x7F:
   - +1 (the sprite sheet) == 0: no object, skipped;
   - kind = (+0 & 0x3F) + 1;
   - +2 != 0 -- the object has an EVENT FLAG: flag 0x580 + that byte for
     kinds 1..3, flag i + 0x680 + ((map - 0x65) << 7) (16 bits) for any
     other kind (map = g_MapNumber); when the flag is SET (StoryBitCheck)
     the object is not spawned. These are the two flag forms
     ScriptCommandMapObjectsAdd / MoveChrPartyLeave clear and MapObjectOverlappedRemove sets;
   - MapObjectInit(row n of g_MapObjects, row n of g_MapObjectTwins, the
     definition, i) spawns it, and n counts up.
   n is the number of objects spawned so far, not i: the rows are PACKED,
   a skipped definition leaves no hole, and the definition's index only
   survives as the row's byte +1 (MapObjectInit stores it).

   No RNG draw. MapObjectsDataLoad's file load (resource_load) waits frames; the
   loop itself calls nothing that does.

   Source shapes the bytes need:
   - The definitions are one struct pointer that is WALKED (info++), not
     indexed by i. The target's second pointer -- $s0 = info + 2, read at
     -1 for the sheet and at 0 for the flag byte -- is the compiler's own:
     with info a loop variable, the three accesses to +1 / +2 / +2 are
     address givs with constant offsets, which the loop pass folds into
     one new register based at the last of them; +0 is no giv and stays on
     info. Indexed (info[i].b1 ...) everything hangs off one register and
     the frame loses $s4: 42/63. A walked u8 pointer (p[1], p[0], p[2])
     gives the same bytes as the struct.
   - The row offsets are n * 0x60 and n * 0x3C of ONE counter; the two
     registers stepped by 0x60 and 0x3C are its strength-reduced forms
     (two offset variables stepped by hand: 56/63).
   - The no-flag arm comes FIRST and is not empty: it sets r, the flag
     test's result, which nothing reads on that path. That assignment
     emits no instruction; what it leaves is the `else` label between the
     flag byte's test and its second read. Without a label there the
     compiler's post-reload pass (reload_cse_regs, which forgets register
     contents at every label) finds the byte still in $v1 and turns the
     second `lbu` into `move $a0, $v1`: 55/63 with the arm empty, with the
     arms the other way round, or with `if (flag != 0) { ... continue; }`.
     Testing r after the if / else instead is 56/63 -- this compiler does
     not thread `r = 0` to the test (jump.c has that only for cc0
     machines), so `j` + `move $v0, $zero` stay. A second copy of the
     spawn in that arm gives the label too, but the extra uses of i and
     info reorder the saved registers (43 or 47/63, pure renaming).
   - Each kind's call is written out whole (the argument is built in $a0
     in both arms; through one flag-number variable it goes through a
     temporary, 56/63). kind is unsigned (u8 here as in MapObjectInit;
     an int compares with slti, 62/63). */
/* func_800655C8 */
void MapObjectsInit(s32 set_number)
{
    MapObjectDefinition *info;
    s32 set;
    s32 i;
    s32 n;
    u8 kind;
    s32 r;

    set = set_number & 0xFF;
    MapObjectsDataLoad(set);
    if (set != 0) {
        info = (MapObjectDefinition *)g_MapObjectDefinitions;
        n = 0;
        for (i = 0; i < 0x80; i++, info++) {
            if (info->sprite_data != 0) {
                kind = (info->kind & 0x3F) + 1;
                if (info->story_bit == 0) {
                    /* MARKED SPELLING: a DEAD store, standing for a statement
                       the bytes show was here but do not identify. (By the
                       owner's ruling, evidence-backed stand-ins take this tag;
                       the device tag is for shapes with no such evidence.)
                       Nothing reads r on this path. Without it: 55/63.

                       What the bytes say. The target
                       reads the flag byte TWICE (`lbu $v1` for the test,
                       `lbu $a0` again in the kind < 4 arm) although $v1 still
                       holds it. reload_cse turns such a second load into a
                       `move` unless it has forgotten its register contents,
                       which it does at a CODE_LABEL. With a statement in this
                       arm the RTL after flow is `if (b2 != 0) goto L; goto
                       join; L:` -- the store is deleted as dead, the label L
                       stays until the jump pass after reload (read in the
                       .flow / .greg dumps: code_label 69 between insns 59 and
                       78). With the arm empty the first jump pass has already
                       folded that to `if (b2 == 0) goto join` and there is no
                       label. So the original had a statement here that left
                       no code; any dead local store reproduces it (`kind =
                       0;` too) and WHICH one it was cannot be recovered.

                       About forty other spellings do not match: the arm
                       empty, arms swapped, a goto, a while, a switch on the
                       kind or on the flag, the flag through u8 / u16 / s32
                       locals, one flag-number variable (51; 56 with its own
                       dead store), r tested AFTER the if / else (56: a real
                       `move $v0, $zero; j` appears). The spawn call written
                       in both arms -- the one device-free form that keeps the
                       label -- gives these exact instructions with $s0 / $s1
                       / $s2 permuted (47/63), and cannot do better: the
                       second call adds two references each to i and info and
                       none to the walked pointer, whose allocation priority
                       falls from first (0.846 against i's 0.786) to last
                       (0.717 against 0.796 and 0.720). The sibling
                       MapObjectDefinitionSpotFind makes the same test and needs nothing:
                       there the first load's register is reused by the
                       compare, so the second read is natural. */
                    r = 0;
                } else {
                    if (kind < 4) {
                        r = StoryBitCheck(info->story_bit + 0x580);
                    } else {
                        r = StoryBitCheck((u16)(i + 0x680 + ((g_MapNumber - 0x65) << 7)));
                    }
                    if (r != 0) {
                        continue;
                    }
                }
                MapObjectInit((u8 *)g_MapObjects + n * 0x60, (u8 *)g_MapObjectTwins + n * 0x3C, info, i);
                n++;
            }
        }
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_800656C4() {

}

/* MapObjectInit -- the name is the splitter's: this SPAWNS
   ONE MAP OBJECT FROM A DEFINITION ALONE -- MapObjectRestore's steps
   (above) without a map-list entry. Callers: MoveChrPartyLeave
   (lns_move.c), ScriptCommandMapObjectsAdd and MapObjectsInit (asm), each with row i
   of g_MapObjects (0x60), its 0x3C twin in g_MapObjectTwins, a 0x26-byte
   definition of g_MapObjectDefinitions and that definition's index `id`.

   info (0x26): +0 bits 0..5 = the row's kind less one, bit 0x40 goes into
   the row's +0x3D, bit 0x80 = MapAttributeClassGet's layer (+0x3C); +1 the sprite
   sheet (0 is used instead when the kind is 7); +3 MapRecordInit's
   mode; +4 / +5 x / y in 8-pixel units (ground y and draw y are the same);
   +6 the dispatch byte less one (+0x3E); +7 the four unaligned footprint
   bytes for +0x50; +0xC the low bits of +0x3D.

   Order: +1 = id and +0 = kind, MapRecordInit(row, kind, mode,
   sheet), the position and the +0x3C / +0x3D / +0x3E / +0x42 bytes, the
   box, n = MapObjectMoveSet(row, info, 0) -- ITS RESULT IS THE DISPATCH GROUP
   here (MapObjectRestore ignores it and derives the group from the list
   entry's facing count) -- then the footprint class OR'd into +0xE's bits
   0x6000.

   Kind 7 with a dispatch byte of 5 or more is a CHEST: when event flag
   id + 0x2620 + ((map > 100 ? map - 0x35 : map - 0x15) << 5) is set
   (StoryBitCheck; map = g_MapNumber), +0x3E drops by 4 -- the opened
   state. MapObjectKind7Run sets that same flag (StoryBitSet) and makes the
   same -4 when the chest is opened.

   Then MapAnimationPoseSet(row, n, +0x3E), and a sheet below 0xF also
   fills the twin: the row's first 0x3C bytes, rendered again with sheet
   g_MapTwinSpriteDataNumbers[sheet - 1], and MapAnimationPoseSet(twin, n, info +6 + 1)
   -- the twin takes the definition's byte, not the row's, so an opened
   chest's twin is dispatched with the CLOSED value.
   No RNG draw and no frame wait anywhere in its call tree.

   Shape: kind is a u8 (the sum is computed in $v0, copied to $s3 and
   stored from $v0; as an int it is computed straight into $s3). `sheet` is
   ONE variable for both render calls: the twin's sheet reuses it, which is
   why the target loads it into $v1 ahead of the copy loop and moves it to
   $a3 in the jal's delay slot (a separate local is loaded straight into
   $a3 and `move a0` takes the slot). n is a u8: the raw result is kept in
   $s0 and masked `andi 0xFF` at each MapAnimationPoseSet call.
   MapObjectMoveSet is called undeclared; MapAttributeClassGet and StoryBitCheck
   have their prototypes above (byte-neutral -- the mask on the flag is
   the (u16) cast either way). */
/* func_800656CC */
void MapObjectInit(void *row, void *twin_record, void *definition, u8 definition_number)
{
#define obj ((MapObject *)row)
#define twin ((MapRecord *)twin_record)
#define info ((MapObjectDefinition *)definition)
    u8 kind;
    s32 sheet;
    u8 n;

    sheet = 0;
    kind = (info->kind & 0x3F) + 1;
    obj->record.hold_definition.definition = definition_number;
    obj->record.on_kind.kind = kind;
    if (kind != 7) {
        sheet = info->sprite_data;
    }
    MapRecordInit(obj, obj->record.on_kind.kind, info->mode, sheet);
    obj->record.sprite.x = info->x << 3;
    obj->record.sprite.y = info->y << 3;
    AS(u16, obj->record.sprite.depth) = info->y << 3;
    obj->layer = info->kind & 0x80;
    obj->number = (info->kind & 0x40) | info->number;
    obj->facing = info->facing + 1;
    obj->turn_count = 0;
    obj->box = info->box;
    n = MapObjectMoveSet(obj, info, 0);
    obj->record.sprite.flags = MapAttributeClassGet(1, 0x20, obj->record.sprite.x, obj->record.sprite.y, AS(u16, obj->record.sprite.depth), &obj->box, obj->layer) | (obj->record.sprite.flags & 0x9FFF);
    if (kind == 7) {
        if (obj->facing >= 5) {
            if (StoryBitCheck((u16)(definition_number + 0x2620 + ((g_MapNumber > 100 ? g_MapNumber - 0x35 : g_MapNumber - 0x15) << 5))) != 0) {
                obj->facing -= 4;
            }
        }
    }
    MapAnimationPoseSet(obj, n, obj->facing);
    if (info->sprite_data < 0xF) {
        sheet = g_MapTwinSpriteDataNumbers[info->sprite_data - 1];
        *twin = obj->record;
        MapRecordInit(twin, obj->record.on_kind.kind, info->mode, sheet);
        MapAnimationPoseSet(twin, n, info->facing + 1);
    }
#undef obj
#undef twin
#undef info
}

s32 MapObjectFind(u8 definition_number);  /* TU defines this below */
s32 MapObjectFreeFind(void);  /* TU defines this below */
void MapAnimationPoseSet(void *, s16, u16);

/* func_80065940 */
void MapRecordInit(void *record, s32 kind, s32 mode, s32 sprite_data_number)
{
  u16 old_attr;
  MapRecordSpriteDataSet(record, sprite_data_number);
  ((MapRecord *)record)->animation = 0xFFFF;
  ((MapRecord *)record)->sprite.on = 0x0001;
  ((MapRecord *)record)->sprite.attr = 0x0020;
  if (kind == 1)
  {
    ((MapRecord *)record)->sprite.flags = 0x2000;
  }
  else
    if (kind == 2)
  {
    ((MapRecord *)record)->sprite.flags = 0x2100;
  }
  else
    if (kind == 7)
  {
    ((MapRecord *)record)->sprite.attr = 0x0000;
    ((MapRecord *)record)->sprite.flags = 0x2370;
  }
  else
  {
    ((MapRecord *)record)->sprite.flags = 0x2200;
  }
  if (mode == 1)
  {
    ((MapRecord *)record)->sprite.flags |= 0x0800;
  }
  else
    if (mode == 2)
  {
    ((MapRecord *)record)->sprite.flags |= 0x0C00;
  }
  else
    if (mode == 3)
  {
    old_attr = ((MapRecord *)record)->sprite.attr;
    ((MapRecord *)record)->sprite.flags = 0x2000;
    ((MapRecord *)record)->sprite.attr = old_attr | 0x8000;
  }
  ((MapRecord *)record)->sprite.pose = 0x0000;
  ((MapRecord *)record)->sprite.scaled = (float) 0x0000;
}
extern u8 g_MoveSpeedPixels[];
extern u8 g_MoveSpeedIntervals[];
extern s32 g_MapObjectStepLists;

/* MapObjectMoveSet -- sets a map object's BEHAVIOUR STATE from its definition and
   returns the animation group its caller dispatches. `row` is a 0x60 row
   of g_MapObjects, `info` its 0x26-byte definition (g_MapObjectDefinitions). Callers:
   the two spawners MapObjectRestore / MapObjectInit (above; both
   pass second_set = 0, and only MapObjectInit uses the result) and
   MapObjectNumberReset (map_look.c), which first puts the definition's +0xC into
   the row's mode bits and passes second_set = 1. A leaf: no frame wait, no RNG.

   mode = row +0x3D & 0x3F (the think pass MapObjectsThink calls
   vtbl_4658_table[mode] every `+0x41` frames, counting `+0x40` down):

   mode 0 or 6 -- a roamer.
     second_set == 0: the roaming rectangle is set around the row's position,
       in pixels, from four definition bytes in 8-px cells:
         +0x54 = x + (s8)info[0xE] * 8, not below 0
         +0x56 = x + (info[0xF] + 1) * 8, not above 0x7FF
         +0x58 = drawY (+0xA) + (s8)info[0x10] * 8, not below 0
         +0x5A = drawY + (info[0x11] + 1) * 8, not above 0x7FF
       and the five parameter bytes come from info +0x14..+0x18.
     second_set != 0: the rectangle is KEPT and the five bytes come from info
       +0x19..+0x1D (the definition's second set).
     Speed class c = info[0xD]: row kinds 4, 5 and 6 (+0) take entry c + 2
     of the two speed tables, every other kind entry c + 3 --
       +0x3F = g_MoveSpeedPixels[...] (01 01 01 01 02 04 08 10: the step, px)
       +0x41 = g_MoveSpeedIntervals[...] (08 04 02 01 01 01 01 01: the think period)
     The five bytes go to +0x43..+0x47; +0x48, +0x4A and +0x4B are zeroed.
     Returns 1.
   mode 1 or 2 -- a scripted mover. +0x3F = +0x41 = 1, +0x4A = 0, +0x56 =
     0, and the row's move script is entry info[0x12] of the table at
     g_MapObjectStepLists (u16 offsets from the table's own start): +0x5C = the
     script's address, +0x5A (the step dispatcher's cursor) = 2. Returns
     the script's byte 1 -- its animation group.
   any other mode -- +0x41 = 1. Returns 1.
   Last, the think timer +0x40 = the period +0x41.

   Shape:
   - `mode` is an int tested through a (u8) cast in the first test only:
     the target masks it there (andi $v1, $a3, 0xFF) and uses the raw
     register for the 1..2 range test (addiu $v0, $a3, -1). The cast is
     redundant by value (mode <= 0x3F); a u8 `mode` gives a three-insn
     chain that combine reduces to a copy (129/130), and a bit-field at
     +0x3D scores 114-125.
   - The parameter-byte pointer is set in BOTH arms of `if (second_set != 0)`,
     first thing in the else arm: it then shares second_set's register ($a2),
     reorg takes the then-arm's set into the test's delay slot and the
     else-arm's into the first clamp's. One set before the test and one at
     the end of the arm scores 108.
   - In the mode 1/2 arm the script pointer is stored FIRST in the source;
     the scheduler lifts its loads above the four byte/half stores and
     leaves the `sw` behind them (stores first in the source: 108 -> 113
     for this alone).
   - The table reads are plain `g_MoveSpeedPixels[info->bD + 2]`: combine splits
     the address into index + base with the 2 left as the load's offset. */
/* func_80065A2C */
s32 MapObjectMoveSet(MapObject *row, MapObjectDefinition *info, s32 second_set)
{
    s32 mode;
    s32 ret;
    u8 *p;

    mode = row->number & 0x3F;
    ret = 1;
    if ((u8)mode == 0 || (u8)mode == 6) {
        if (second_set != 0) {
            p = info->second_look_set;
        } else {
            p = info->look_set;
            row->move.bounds.left = row->record.sprite.x + info->bound_left * 8;
            if (row->move.bounds.left < 0) {
                row->move.bounds.left = 0;
            }
            row->move.bounds.right = row->record.sprite.x + (info->bound_right + 1) * 8;
            if (row->move.bounds.right >= 0x800) {
                row->move.bounds.right = 0x7FF;
            }
            row->move.bounds.top = row->record.sprite.y + info->bound_top * 8;
            if (row->move.bounds.top < 0) {
                row->move.bounds.top = 0;
            }
            row->move.bounds.bottom = row->record.sprite.y + (info->bound_bottom + 1) * 8;
            if (row->move.bounds.bottom >= 0x800) {
                row->move.bounds.bottom = 0x7FF;
            }
        }
        if (row->record.on_kind.kind >= 4 && row->record.on_kind.kind < 7) {
            row->speed = g_MoveSpeedPixels[info->speed + 2];
            row->interval = g_MoveSpeedIntervals[info->speed + 2];
        } else {
            row->speed = g_MoveSpeedPixels[info->speed + 3];
            row->interval = g_MoveSpeedIntervals[info->speed + 3];
        }
        row->look = *p++;
        row->sight_width = *p++;
        row->sight_length = *p++;
        row->length = *p++;
        row->count = *p;
        row->state = 0;
        row->facing_kept = 0;
        row->turn_plan = 0;
    } else if (mode == 1 || mode == 2) {
        AS(u8 *, row->step_list) = (u8 *)(g_MapObjectStepLists + *(u16 *)(info->step_list * 2 + g_MapObjectStepLists));
        row->speed = 1;
        row->interval = 1;
        row->facing_kept = 0;
        row->move.bounds.right = 0;
        row->move.bounds.bottom = 2;
        ret = AS(u8 *, row->step_list)[1];
    } else {
        row->interval = 1;
    }
    row->interval_left = row->interval;
    return ret;
}

/* func_80065C34 */
s32 MapObjectFind(u8 definition_number)
{
    short i;
    u8 *p;

    i = 0;
    p = (u8 *)g_MapObjects;

    do
    {
        if (p[i * 0x60] != 0 && p[i * 0x60 + 1] == definition_number)
            return i;
        i++;
    } while (i < 0x80);

    return -1;
}

/* func_80065CA0 */
s32 MapObjectFreeFind(void)
{
    short i;
    u8 *p;

    i = 0;
    p = (u8 *)g_MapObjects;

    while (1)
    {
        if (p[i * 0x60] != 0)
        {
            i++;
            if (i < 0x80)
                continue;
            return -1;
        }
        return i;
    }
}
