/* Lunar SSSC — map_sprite: the map's records cleared and the sprite data of
   the party and the objects loaded, freed, marked, reloaded, pointed at and
   counted. The first split had it in scripting_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_MapTwinSpriteDataNumbers */
/* far_syms: g_MapObjectsDataResourceNumbers (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"
void MapObjectsSpriteDataReload(s32, s32);                          /* extern */
void MapPartySpriteDataReload();                            /* extern */
extern u8 g_MapObjectsDataResourceNumbers[];

s32 sprite_page_image_add(s32, s32);                              /* extern */
extern u16 g_MapObjectsDataNumber;
extern u8 g_MapTwinSpriteDataNumbers[];
extern u8 g_PartyCount;
s32 MapPartySpriteDataSet();                                  /* extern */
extern s32 g_ResourceReadBuffer;
extern s32 g_MapPartyTwins;
extern u16 g_MapPartyKind;
extern s32 g_ChrRecords;
extern s32 g_PartyIds;
s32 mem_clear(s32, s32);                            /* extern */
extern u16 g_MapPartyWalkStepCount;
extern s32 g_MapTalkObjectDefinition;
extern s16 g_MapPartyAttributeCheckOff;
extern s32 g_MapPartyRecords;
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
extern s32 g_MapSpriteDataEntries;
extern s32 g_MapSpriteDataMarks;
extern s32 g_MapPartyRingEntries;

/* func_80063A8C */
void MapRecordsClear(void) {
    g_MapPartyRingEntries = 0x801EA000;
    g_MapSpriteDataMarks = 0x801DBE80;
    mem_clear(0x801DBE80, 0x100);
    mem_clear(g_MapSpriteDataEntries, 0x1000);
    mem_clear(g_MapObjects, 0x3000);
    mem_clear(g_MapObjectTwins, 0x1E00);
    mem_clear(g_MapPartyRecords, 0x2D0);
    g_MapPartyWalkStepCount = 1;
    g_MapPartyAttributeCheckOff = 0;
    g_MapTalkObjectDefinition = 0xFF;
}

s32 sprite_init();                                  /* extern */

/* Ruling of 2026-10-14 (docs/naming.md): Does what sprite_init does; nothing read says why there are two. */
/* func_80063B10 */
void sprite_init_copy(void) {
    sprite_init();
}

/* far_syms: g_FaceList */
s32 resource_load(s32, s32);
s32 mem_move(s32, s32, s32);
s32 StoryBitSet(u16);  /* TU declares this below */
s32 ChrWornItemSet(s32, s32, u8);
void StoryBit2CAUpdate(void);  /* TU declares this below */
s32 FaceListPartySet();
extern s32 g_ChrItemList0C;
extern s32 *g_GameMoney;
extern s8 g_PartyLevelLeast;
extern s32 g_ChrPlacePresetRecords;
extern s32 g_FaceList;

/* func_80063B30 */
void ChrRecordsReset(void)
{
  s32 i;
  s32 j;
  u8 *p;
  s32 q;
  ((s16 *) g_PartyIds)[0] = 1;
  g_PartyCount = 1;
  g_PartyLevelLeast = 1;
  ((s16 *) g_PartyIds)[1] = 0;
  ((s16 *) g_PartyIds)[2] = 0;
  ((s16 *) g_PartyIds)[3] = 0;
  ((s16 *) g_PartyIds)[4] = 0;
  ((s16 *) g_PartyIds)[5] = 0;
  StoryBitSet(0x2BD);
  mem_clear(g_FaceList, 7);
  resource_load(0xE7, g_ResourceReadBuffer);
  FaceListPartySet();
  p = (u8 *) (g_ResourceReadBuffer + 0x300);
  q = g_ResourceReadBuffer + 0x342;
  mem_move(g_ChrRecords, g_ResourceReadBuffer, 0x300);
  mem_move(g_ChrPlacePresetRecords + 8, q, 0x58);
  for (i = 1; i < 12; i++)
  {
    for (j = 1; j < 7; j++)
    {
      ChrWornItemSet(i & 0xFFFF, j & 0xFFFF, *p++);
    }
  }
  mem_clear(g_ChrItemList0C, 0x68);
  *g_GameMoney = 0;
  StoryBit2CAUpdate();
}

/* far_syms: g_MapPartySpriteDataIds (target reaches it %hi/%lo) */
extern u8 g_MapPartySpriteDataIds;

/* func_80063C34 */
void MapSpriteDataFree(void) {
    u16 i;

    for (i = 1; i < 0x100; i++) {
        *(u8 *)((u8 *)g_MapSpriteDataMarks + i) = 1;
    }
    for (i = 1; i < 0x100; i++) {
        if ((*(s32 *)((i * 0x10) + g_MapSpriteDataEntries)) != 0) {
            if ((*(u8 *)((u8 *)g_MapSpriteDataMarks + i)) != 0) {
                xmalloc_heap_free(*(s32 *)((i * 0x10) + g_MapSpriteDataEntries));
                *(s32 *)((i * 0x10) + g_MapSpriteDataEntries) = 0;
            }
        }
    }
    sprite_page_image_add(-1, 0);
    for (i = 0; i < 6; i++) {
        *(u8 *)((u8 *)(&g_MapPartySpriteDataIds) + i) = 0;
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Counts down an empty loop and returns -1; its one caller ignores the answer and does not say what for. */
s32 func_80063D30(void) {
    s32 i;
    for (i = 0x3F; i >= 0; i--) {
    }
    return i;
}
/* far_syms: g_MapTwinSpriteDataNumbers g_MapPartySpriteDataIds */
s32 xmalloc_heap_alloc(s32);
s32 inflate_buffer(s32, s32);
s32 sprite_page4_image_load(s32, s32);

/* func_80063D48 */
void MapPartySpriteDataLoad(u8 skip_load)
{
    s32 i;
    s32 j;
    s32 changed;
    SpriteDirectoryRow *dir;
    u8 id;
    s32 n;
    s32 src;
    s32 dst;

    for (i = 5; i >= 0; i--) {
    }

    changed = 0;
    for (i = 0; i < 6; i++) {
        if ((&g_MapPartySpriteDataIds)[i] == 0) {
            break;
        }
        for (j = 0; j < 6; j++) {
            if ((&g_MapPartySpriteDataIds)[i] == ((u16 *)g_PartyIds)[j]) {
                break;
            }
        }
        if (j == 6) {
            changed = 1;
        }
    }

    if (changed == 1) {
        func_80063D30();
        for (i = 0; i < 6; i++) {
            if ((&g_MapPartySpriteDataIds)[i] == 0) {
                break;
            }
        }
        sprite_page_image_add(-1, 0);
    }

    for (i = 0; i < 6; i++) {
        (&g_MapPartySpriteDataIds)[i] = ((u16 *)g_PartyIds)[i];
    }

    if (skip_load == 0) {
        dir = (SpriteDirectoryRow *)g_ResourceReadBuffer;
        resource_load(0x1E8, (s32)dir);
        for (i = 0; i < 6; i++) {
            id = ((u16 *)g_PartyIds)[i];
            if (id == 0) {
                break;
            }
            if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data == 0) {
                if (id == 0xCE) {
                    n = 0x1E;
                } else {
                    n = id - 1;
                }
                src = g_ResourceReadBuffer + dir[n].data_place * 4;
                dst = xmalloc_heap_alloc(dir[n].data_length * 4);
                inflate_buffer(src, dst);
                ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data = dst;
                sprite_page_image_add(n, dst);
            }
            if (id < 0x1F) {
                if (g_MapPartyKind == 0) {
                    id += 0xF;
                    if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data == 0) {
                        if (id == 0xCE) {
                            n = 0x1E;
                        } else {
                            n = id - 1;
                        }
                        src = g_ResourceReadBuffer + dir[n].data_place * 4;
                        dst = xmalloc_heap_alloc(dir[n].data_length * 4);
                        inflate_buffer(src, dst);
                        ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data = dst;
                        sprite_page4_image_load(n, dst);
                    }
                }
                id = g_MapTwinSpriteDataNumbers[((u16 *)g_PartyIds)[i] - 1];
                n = id - 1;
                if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data == 0) {
                    src = g_ResourceReadBuffer + dir[n].data_place * 4;
                    dst = xmalloc_heap_alloc(dir[n].data_length * 4);
                    inflate_buffer(src, dst);
                    ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data = dst;
                    sprite_page_image_add(n, dst);
                }
            }
        }
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_80064084 */
void MapPartyImagesSend(void) {
    s32 i;
    s32 image_id;
    u8 c;

    for (i = 5; i >= 0; i--) {
    }

    sprite_page_image_add(-1, 0);
    resource_load(0x1E8, g_ResourceReadBuffer);

    for (i = 0; i < 6; i++) {
        c = ((u8 *)g_PartyIds)[i * 2];
        if (c == 0) {
            break;
        }
        if (c == 0xCE) {
            image_id = 0x1E;
        } else {
            image_id = c - 1;
        }
        sprite_page_image_send(image_id, ((s32 *)g_MapSpriteDataEntries)[c * 4]);
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_80064124() {

}

/* func_8006412C */
void MapSpriteDataLoadedMark(void) {
    s32 sprite_data_number;

    sprite_data_number = 0;
    do {
        if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[sprite_data_number].data != 0) {
            ((u8 *)g_MapSpriteDataMarks)[sprite_data_number] = 1;
        } else {
            ((u8 *)g_MapSpriteDataMarks)[sprite_data_number] = 0;
        }
        sprite_data_number += 1;
    } while (sprite_data_number < 0xFF);
    ((u8 *)g_MapSpriteDataMarks)[0xFF] = 0;
}
/* far_syms: g_MapObjectsDataResourceNumbers */

/* MapObjectsSpriteDataReload(set, skip_low) -- RELOADS A MAP OBJECT SET'S STALE SPRITE
   SHEETS. Its one caller is MapSpriteDataReload (below; the field restore the
   battle entry runs after a fight), which first marks every loaded sheet
   stale -- each non-zero g_MapSpriteDataEntries[1..0xFF].p becomes 1 -- and then calls
   this with (g_MapObjectsDataNumber, its own argument): the current object set, the
   one MapObjectsDataLoad loaded. The party's own sheets are the next call's
   business (MapPartySpriteDataReload), and whatever is still 1 after both is
   zeroed by the caller.

   Resource ((u16 *)g_MapObjectsDataResourceNumbers)[set] is read to the work buffer g_ResourceReadBuffer
   (resource_load; `hdr` is taken before the read and is its second argument,
   the global is read again after every call). hdr[1] is the offset of the
   set's list of 6-byte sheet loads {id, size, off}, ended by an entry
   whose id is 0 -- the list MapObjectsDataLoad walks. For each entry:
     - with skip_low != 0, an id below 0x1F is passed over (the ids the
       party loaders treat as party members, MapPartySpriteDataReload's
       `id < 0x1F`); with skip_low == 0 every id is considered;
     - only a STALE sheet is reloaded: g_MapSpriteDataEntries[id].p == 1. One that was
       never loaded (0) or has been reloaded already (a pointer) is left;
     - the reload is MapObjectsDataLoad's: dst = xmalloc_heap_alloc(size * 4), the
       packed sheet at g_ResourceReadBuffer + hdr[1] + off * 4 (`off` counts words
       from the LIST) is inflated to it (inflate_buffer), and
       g_MapSpriteDataEntries[id].p = dst.
   Unlike MapObjectsDataLoad this does NOT test the set for 0 (set 0 reads
   resource g_MapObjectsDataResourceNumbers[0], which is 0 in the image), does not range-check
   the id (MapObjectsDataLoad's `id < 0x100`: an id of 0x100 or more indexes
   past the 0x100-row table here and may be written through), does not
   inflate the object definitions at hdr[0], does not point entry 0 at
   g_SpriteData1 and does not load the page images (the caller runs
   sprite_page_list_load itself, and only when its argument is 0). No VRAM page
   is registered. No return statement (the caller ignores the value). No
   RNG draw and no frame wait of its own; resource_load (the file load) waits
   frames.

   Source shapes the bytes need. The first parameter is an INT: a u16
   parameter gets an `andi $a0, 0xFFFF` on entry that the target does not
   have (67 words), so the caller's generated guess `s32
   MapObjectsSpriteDataReload(u16, s32);` takes this definition's types -- the caller
   passes the u16 global g_MapObjectsDataNumber, an `lhu` either way. The list is a
   record array indexed by a counter, as in MapObjectsDataLoad: strength
   reduction then leaves ONE walking register ($s1, fields at 0 / 2 / 4)
   and the copy of the list's address into it sits in the entry test's
   delay slot; walked as a pointer (`list++`) it gets a second induction
   register and a 0x30 frame (45/66). `id` is an int local whose
   table offset (id << 4) is kept in $s2 across both calls; src and dst
   share $s0. The packed sheet's address is written out from the buffer
   and hdr[1] again, not from `list`. The id test is spelled as the `||`
   with the `== 0` arm first (the target's `beqz $s4` with the `slti` in
   its delay slot). MapObjectsSpriteDataRow / MapSpriteDataEntry are this function's own typedef names
   (MapObjectsDataLoad's MapObjectsSpriteDataRow is declared further down the file). */
/* func_8006419C */
void MapObjectsSpriteDataReload(s32 set_number, s32 skip_low)
{
    s32 *hdr;
    MapObjectsSpriteDataRow *list;
    s32 id;
    s32 src;
    s32 dst;
    s32 i;

    hdr = (s32 *)g_ResourceReadBuffer;
    resource_load(((u16 *)g_MapObjectsDataResourceNumbers)[set_number], (s32)hdr);
    list = (MapObjectsSpriteDataRow *)(g_ResourceReadBuffer + hdr[1]);
    for (i = 0; list[i].sprite_data != 0; i++) {
        id = list[i].sprite_data;
        if (skip_low == 0 || id >= 0x1F) {
            if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data == 1) {
                src = g_ResourceReadBuffer + hdr[1] + list[i].place * 4;
                dst = xmalloc_heap_alloc(list[i].length * 4);
                inflate_buffer(src, dst);
                ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data = dst;
            }
        }
    }
}

/* MapPartySpriteDataReload -- the name is the splitter's.

   Reloads the party's field sprite sheets. Its only caller is
   MapSpriteDataReload (the field restore the battle entry MapBtlRun runs
   after a fight), which first marks every loaded sheet stale -- each
   non-zero g_MapSpriteDataEntries[1..0xFF].p becomes 1 -- resets the sheet page maps
   (sprite_page_image_add(-1, 0)), calls this, and afterwards zeroes every .p that
   is still 1.

   This is MapPartySpriteDataLoad's loading half with `.p == 1` (stale) where
   that function tests `.p == 0` (not loaded): the directory resource 0x1E8
   is read to g_ResourceReadBuffer (resource_load), then for each party slot 0..5, up to
   the first empty one, with id = the LOW BYTE of ((u16 *)g_PartyIds)[i]:
     - the member's own sheet `id` (directory row id - 1, or 0x1E for id
       0xCE), registered with sprite_page_image_add;
     - for ids below 0x1F, and only when g_MapPartyKind == 0, sheet id + 0xF
       (row id + 0xE), registered with sprite_page4_image_load;
     - for ids below 0x1F, the twin sheet g_MapTwinSpriteDataNumbers[id16 - 1] (row that
       value - 1), registered with sprite_page_image_add.
   Each load is dst = xmalloc_heap_alloc(dir[n].size * 4), inflate_buffer(
   g_ResourceReadBuffer + dir[n].off * 4, dst) (inflate), g_MapSpriteDataEntries[sheet].p = dst,
   then the registration (the VRAM page allocator). A sheet that is not
   marked stale (never loaded, or already reloaded for an earlier slot) is
   skipped. No RNG draw and no frame wait of its own; resource_load (the file
   load) waits frames.

   Shape: as MapPartySpriteDataLoad, with ONE difference -- the third block's
   `n = id - 1` is INSIDE its `if` here (there it is assigned before the
   test). The target's delay-slot `addiu $s2, $a0, -1` is reorg stealing
   that first statement of the fall-through (n is dead on the taken path);
   written before the test, the byte's zero-extension is a block-local
   temporary and takes $v1 (140/143), while inside the `if` it lives across
   the branch, is allocated globally and takes $a0. The constant 1 is
   hoisted into $fp, and 0xCE and &g_MapTwinSpriteDataNumbers are hoisted too but get no
   register (all nine callee-saved are taken), so reload rematerialises
   both in $a2. SpriteDirectoryRow / MapSpriteDataEntry are this function's own typedef names. */
/* func_800642A4 */
void MapPartySpriteDataReload(void)
{
    s32 i;
    SpriteDirectoryRow *dir;
    u8 id;
    s32 n;
    s32 src;
    s32 dst;

    dir = (SpriteDirectoryRow *)g_ResourceReadBuffer;
    resource_load(0x1E8, (s32)dir);
    for (i = 0; i < 6; i++) {
        id = ((u16 *)g_PartyIds)[i];
        if (id == 0) {
            break;
        }
        if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data == 1) {
            if (id == 0xCE) {
                n = 0x1E;
            } else {
                n = id - 1;
            }
            src = g_ResourceReadBuffer + dir[n].data_place * 4;
            dst = xmalloc_heap_alloc(dir[n].data_length * 4);
            inflate_buffer(src, dst);
            ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data = dst;
            sprite_page_image_add(n, dst);
        }
        if (id < 0x1F) {
            if (g_MapPartyKind == 0) {
                id += 0xF;
                if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data == 1) {
                    if (id == 0xCE) {
                        n = 0x1E;
                    } else {
                        n = id - 1;
                    }
                    src = g_ResourceReadBuffer + dir[n].data_place * 4;
                    dst = xmalloc_heap_alloc(dir[n].data_length * 4);
                    inflate_buffer(src, dst);
                    ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data = dst;
                    sprite_page4_image_load(n, dst);
                }
            }
            id = g_MapTwinSpriteDataNumbers[((u16 *)g_PartyIds)[i] - 1];
            if (((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data == 1) {
                n = id - 1;
                src = g_ResourceReadBuffer + dir[n].data_place * 4;
                dst = xmalloc_heap_alloc(dir[n].data_length * 4);
                inflate_buffer(src, dst);
                ((MapSpriteDataEntry *)g_MapSpriteDataEntries)[id].data = dst;
                sprite_page_image_add(n, dst);
            }
        }
    }
}

void sprite_page_list_load();                                /* extern */
void MapObjectsSpriteDataSet(void);  /* TU defines this below */

/* func_800644E0 */
void MapSpriteDataReload(s32 no_map) {
    s32 *reload_entry;
    s32 *clear_entry;
    s32 i;
    s32 j;

    i = 1;
    reload_entry = (s32 *) (g_MapSpriteDataEntries + 0x10);
    do {
        if (*reload_entry != 0) {
            *reload_entry = 1;
        }
        i += 1;
        reload_entry += 4;
    } while (i < 0x100);
    MapObjectsSpriteDataReload(g_MapObjectsDataNumber, no_map);
    if (no_map == 0) {
        sprite_page_list_load();
        MapObjectsSpriteDataSet();
    }
    sprite_page_image_add(-1, 0);
    MapPartySpriteDataReload();
    MapPartySpriteDataSet();
    j = 1;
    clear_entry = (s32 *) (g_MapSpriteDataEntries + 0x10);
    do {
        if (*clear_entry == 1) {
            *clear_entry = 0;
        }
        j += 1;
        clear_entry += 4;
    } while (j < 0x100);
}

/* func_800645A8 */
void MapRecordSpriteDataSet(void *record, s32 sprite_data_number)
{
  SpriteDataHead *sprite_data;
  sprite_data = (SpriteDataHead *)((MapSpriteDataEntry *)g_MapSpriteDataEntries)[sprite_data_number].data;
  ((MapRecord *)record)->animation_table = (u32) ((u8 *)sprite_data + sprite_data->animation_table * 4);
  ((MapRecord *)record)->step_list = (u32) ((u8 *)sprite_data + sprite_data->step_list * 4);
  ((MapRecord *)record)->sprite.pose_table = (u32) ((u8 *)sprite_data + sprite_data->pose_table * 4);
  ((MapRecord *)record)->sprite.piece_table = (u32) ((u8 *)sprite_data + sprite_data->piece_table * 4);
  ((MapRecord *)record)->sprite.texel_records = (u32) ((u8 *)sprite_data + sprite_data->texel_records * 4);
}

/* far_syms: g_MapTwinSpriteDataNumbers (target reaches it %hi/%lo) */

/* func_80064624 */
s32 MapPartySpriteDataSet()
{
  s32 party_id_offset;
  s32 record_offset;
  s32 place;
  s32 sprite_data_number;
  void *record;
  u8 *twin_numbers;
  place = 0;
  if (g_PartyCount != 0)
  {
    twin_numbers = g_MapTwinSpriteDataNumbers;
    record_offset = 0;
    do
    {
      /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
         the block runs once, so behaviour is the same without the `do` / `while (0)`,
         but then 6 of this function's 56 instruction lines differ. */
      do
      {
        party_id_offset = place * 2;
      }
      while (0);
      sprite_data_number = *((u16 *) (party_id_offset + g_PartyIds));
      if (g_MapPartyKind == 0)
      {
        sprite_data_number += 0xF;
      }
      MapRecordSpriteDataSet(g_MapPartyRecords + record_offset, (s32) sprite_data_number);
      record = record_offset + g_MapPartyRecords;
      ((MapRecord *)record)->timer = (u16) (AS(s16, ((MapRecord *)record)->step_length));
      if (((s32) sprite_data_number) < 0x1F)
      {
        MapRecordSpriteDataSet(g_MapPartyTwins + record_offset, (s32) *(u8 *) ((*((u16 *) (party_id_offset + g_PartyIds)) + (s32) twin_numbers) - 1));
      }
      record_offset += 0x3C;
      place += 1;
    }
    while (place < ((s32) g_PartyCount));
  }
}

extern s32 g_MapObjectDefinitions;

/* func_80064704 */
void MapObjectsSpriteDataSet(void)
{
  s32 row_offset;
  s32 twin_numbers;
  s32 twin_offset;
  s32 i;
  u8 *row;
  u8 *twin_record;
  u8 kind;
  s32 sprite_data_number;
  s32 sprite_data_number_address;
  i = 0;
  twin_numbers = g_MapTwinSpriteDataNumbers;
  twin_offset = 0;
  row_offset = 0;
  do
  {
    row = row_offset + g_MapObjects;
    kind = row[0];
    if (kind != 0)
    {
      if (kind != 7)
      {
        sprite_data_number_address = ((u8 *) (g_MapObjectDefinitions + (row[1] * 0x26))) + 0x1;
        sprite_data_number = *((u8 *) sprite_data_number_address);
        MapRecordSpriteDataSet(row, (u8) (sprite_data_number));
        goto block_4;
      }
    }
    else
    {
      block_4:
      twin_record = twin_offset + g_MapObjectTwins;

      if ((*twin_record) != 0)
      {
        sprite_data_number = *((u8 *) (((u8 *) (sprite_data_number + twin_numbers)) - 1));
        MapRecordSpriteDataSet(twin_record, (u8) (sprite_data_number));
      }
    }
    twin_offset += 0x3C;
    i += 1;
    row_offset += 0x60;
  }
  while (i < 0x80);
}

static inline s32 inline_fn(s32 *word)
{
  return *word;
}
/* func_800647E4 */
s32 MapPartySpriteDataMissingCount(void)
{
  u8 *next_twin_number;
  s32 chr_id;
  s32 missing_count;
  s32 entries;
  s32 place;
  u8 *party_id;
  s32 sprite_data_number;
  missing_count = 0;
  entries = g_MapSpriteDataEntries;
  party_id = g_PartyIds;
  place = 0;
  /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source: the
     block runs once, so behaviour is the same without the `do` / `while (0)`, but then 7
     of this function's 45 instruction lines differ. */
  do
  {
    loop_1:
    sprite_data_number = *party_id;

    chr_id = 0xFF;
    chr_id = sprite_data_number & chr_id;
    if (chr_id != 0)
    {
      if (inline_fn((s32 *) ((chr_id * 0x10) + entries)) == 0)
      {
        missing_count += 1;
      }
      sprite_data_number += 0xF;
      if ((*((s32 *) (((sprite_data_number & 0xFF) * 0x10) + entries))) == 0)
      {
        missing_count += 1;
      }
      next_twin_number = ((u16) inline_fn(party_id)) + g_MapTwinSpriteDataNumbers;
      if (inline_fn((s32 *) (((*(((u8 *) next_twin_number) - 0x1)) * 0x10) + entries)) == 0)
      {
        missing_count += 1;
      }
      place += 1;
      party_id += 2;
      if (place < 6)
      {
 goto loop_1; 
      }
    }
    return missing_count;
  }
  while (0);
}
