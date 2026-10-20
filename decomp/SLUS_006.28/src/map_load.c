/* Lunar SSSC — map_load: a map made the one the game is on: its table, set,
   seek, load and reload, its layers and effect (MapEffectSet, the
   developers' name), with the row-2 change and the party's pose and showing
   at its head. The first split had it in scripting_e.c. Every function is
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
s32 StoryBitCheck(s32);
extern s32 D_800B8EDC;

extern long HWD0;
extern long VWD0;
extern s32 g_MapPartyTwins;
extern u16 g_MapPartyKind;
extern u8 g_MapTblNumbers[];
extern u8 g_MapTbls[];
extern s16 D_800B88FC;
extern u8 g_MusicBankNumbers[];
extern u16 g_MapViewShakeCount;
extern u16 g_MapViewShakeWait;
extern u16 g_MapViewShakeX;
extern u16 g_MapViewShakeY;
extern u16 g_MapEffect;
extern s32 g_PartyIds;
void *MapTblGet(s32);  /* TU declares this below */
extern s32 g_ScriptMusicLevelRequest;
extern u8 g_ScriptNumber;
extern s32 g_MapLayersMask;
extern u16 g_MapNumber;
extern s16 g_MapViewKeptX;
extern s16 g_MapViewKeptY;
extern u8 g_MapSoundMapNumber;
extern u16 g_MusicNumber;
extern u16 g_MusicKind;
extern s32 g_ScriptBase;
extern s16 g_MapSecondViewX;
extern s16 g_MapSecondViewY;
extern s32 g_MapPartyRecords;
s32 StoryBitSet(u16);
extern s32 g_MapObjectDefinitions;
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
void MapAnimationPoseSet(void *, s16, u16);
void MapPartyOnPlay(void);
s32 MapLayerOrderSet(void);
void MapLayersInit(void);
s32 SoundMapSet(u8);
u16 MapMusicSet(u16, u16);
void MapLoad(void *tbl, s32 sw);
void MapReload(void);
void MapDataReload(void);
s32 ScriptResourceLoad(s32, s32);
void ScriptChange(u8 script_number);
void func_800692CC();
u8 MapTblByte7Get(unsigned int map_number);
s32 layer_order_set(s32, s32);
s32 MapSeek(s32 map_number, s32 music_number);
s32 bg_effect_flags_set(u8);
s32 MapResourceLoad(u16, s32);
s32 palette_bg_cover_count_set(s32);
s32 palette_animation_set(u8);
s32 tile_animation_set(u8, s32, s32 *);
void MapEffectSet(void *, s32 *, s32);
s32 debug_printf(s32 *, s32, s32, s32, s32, s32, s32, s32, s32);
s32 MapPartyRingEntryGet(s32, s32 *);
void MapAnimationPoseSet(void *, s16, u16);
void MapAnimationPoseSet(void *, s16, u16);
extern s32 (*vtbl_4658_table[])();
void MapPartyOnPlay(void);
s32 MapLayerOrderSet(void);
void MapLayersInit(void);
s32 SoundMapSet(u8);
u16 MapMusicSet(u16, u16);
void MapLoad(void *tbl, s32 sw);
void MapReload(void);
void MapDataReload(void);
s32 ScriptResourceLoad(s32, s32);
void ScriptChange(u8 script_number);
void func_800692CC();
u8 MapTblByte7Get(unsigned int map_number);
s32 layer_order_set(s32, s32);
s32 MapSeek(s32 map_number, s32 music_number);
s32 bg_effect_flags_set(u8);
s32 MapResourceLoad(u16, s32);
s32 palette_bg_cover_count_set(s32);
s32 palette_animation_set(u8);
s32 tile_animation_set(u8, s32, s32 *);
void MapEffectSet(void *, s32 *, s32);
s32 debug_printf(s32 *, s32, s32, s32, s32, s32, s32, s32, s32);
extern long HWD0, VWD0;
s32 SoundPlay(s32, s32);
s32 StoryBitSet(u16);
s32 StoryBitClear(s32);

s32 MapFadeIn(s32, s32, s32, s32);
s32 MapChange(s32, s32, s32, s32, s32, s32);
s32 MapPartyRingEntryGet(s32, s32 *);
void MapPartyPose4Play(void);  /* TU defines this below */

/* func_8006824C */
void MapRow2Change(void)
{
  u8 buf[8];
  s32 record_offset;
  s32 place;
  MapPartyPose4Play();
  MapChange(2, 0, 0, 1, 1, 0xFF);
  place = 0;
  record_offset = 0;
  do
  {
    if ((*((u16 *) ((place * 2) + g_PartyIds))) != 0)
    {
      *((s16 *) (((u8 *) (record_offset + g_MapPartyTwins)) + 0x4)) = (*((s16 *) (((u8 *) (record_offset + g_MapPartyRecords)) + 0x4)) = 0);
      MapPartyRingEntryGet(0, buf);
      MapAnimationPoseSet(g_MapPartyRecords + record_offset, 0xD, buf[5]);
      MapAnimationPoseSet(g_MapPartyTwins + record_offset, 0xD, buf[5]);
    }
    place += 1;
    record_offset += 0x3C;
  }
  while (place < 6);
  MapFadeIn(g_MapLayersMask | 0x130, 0x19, 1, 1);
  MapPartyOnPlay();
}

s32 SoundPlay(s32, s32);
s32 MapFrameStep(s32, s32);
s32 MapPartyRingEntryGet(s32, s32 *);

/* func_80068334 */
void MapPartyPose4Play(void)
{
  u8 buf[8];
  int has_member;
  s32 ring_place;
  s32 place;
  s32 frames_done;
  int twin_record;
  s32 record_offset;
  place = 0;
  record_offset = 0;
  do
  {
    has_member = ((unsigned short) (*((s16 *) ((place * 2) + g_PartyIds)))) != 0;
    if (has_member)
    {
      ring_place = place;
      if (g_MapPartyKind == 2)
      {
        ring_place = 0;
      }
      MapPartyRingEntryGet(ring_place & 0xFFFF, buf);
      MapAnimationPoseSet(g_MapPartyRecords + record_offset, 4, buf[5]);
      MapAnimationPoseSet(twin_record = g_MapPartyTwins + record_offset, 4, buf[5]);
    }
    place += 1;
    record_offset += 0x3C;
  }
  while (place < 6);
  SoundPlay(0xE, 1);
  frames_done = 0;
  do
  {
    MapFrameStep(1, 1);
    frames_done += 1;
  }
  while (frames_done < 0x3C);
}

/* func_80068400 */
void MapPartyOnPlay(void) {
    s32 i;
    s32 offset;
    u8 buf[8];
    MapRecord *p;
    MapRecord *q;
    u16 *r;

    p = (MapRecord *)g_MapPartyTwins;
    q = (MapRecord *)g_MapPartyRecords;
    r = (u16 *)g_PartyIds;
    for (i = 0; i < 6; i++) {
        if (*r != 0) {
            q->sprite.on = 1;
            p->sprite.on = 1;
        }
        p++;
        q++;
        r++;
    }
    SoundPlay(0xE, 1);
    for (i = 0; i < 0x3C; i++) {
        MapFrameStep(1, 1);
    }
    i = 0;
    offset = i;
    for (i = 0; i < 6; i++) {
        if (*(u16 *)(g_PartyIds + i * 2) != 0) {
            MapPartyRingEntryGet(0, buf);
            MapAnimationPoseSet((void *)(g_MapPartyRecords + offset), 1, buf[5]);
            MapAnimationPoseSet((void *)(g_MapPartyTwins + offset), 1, buf[5]);
        }
        offset += 0x3C;
    }
}
/* MapObjectDefinitionSpotFind -- NOTHING CALLS IT: the image holds no
   `jal` or `j` to 0x800684FC, no branch to it, no data word with that value at any byte alignment and
   no lui / addiu (or ori) pair forming it; MENU.BIN has neither the jal
   nor the word; the Japanese build's twin (0x8006BA74) is not called
   either. It is dead code kept by the linker.

   What it would do -- WHERE DOES THE OBJECT WITH SPRITE SHEET sprite_data_number STAND
   ON THIS MAP: the 0x80 object definitions of the loaded object set
   (g_MapObjectDefinitions, 0x26 bytes each; MapObjectsDataLoad) are searched in order for
   the first whose byte +1 (the sprite sheet -- for kinds 1..3 that is the
   character, MoveChrPartyLeave compares it with a party slot's id)
   equals sprite_data_number. For that definition:
     *spot_x = +4 and *spot_y = +5, its x and y in 8-pixel cells, as raw
             bytes (MapObjectInit makes pixels of them with << 3);
     *found_kind = kind = (+0 & 0x3F) + 1;
   and when the definition has an event flag (+2 != 0) and that flag is
   SET (StoryBitCheck) -- flag 0x580 + the byte for kinds 1..3, flag
   i + 0x680 + ((map - 0x65) << 7) (16 bits) for any other kind, i the
   definition's index, map = g_MapNumber: the same two forms and the same
   test as MapObjectsInit's spawn loop -- *spot_x and *spot_y are overwritten
   with -0x80: the object is not on the map. The search stops at the
   first match; with no match nothing is written at all. No RNG draw, no
   frame wait (StoryBitCheck is a bit test).

   Its neighbours below (MapTblGet, the area record of a map id;
   MapReload / MapDataReload / MapSet, the map change) share
   nothing with it but the kind of thing it is, a lookup by key; in body
   it is MapObjectsInit's loop used as a query.

   With no caller, the signature is what the BODY proves, and no more:
   - sprite_data_number is compared as a whole register with the zero-extended byte:
     an int (s32 and u32 give the same bytes; a u8 would be masked on
     entry, 65/71);
   - spot_x / spot_y / found_kind are stored through with `sw`: pointers to 32-bit
     objects (signedness not decided by the body; -0x80 says signed);
   - four arguments, none on the stack;
   - the return type is NOT void: declared void, the `slti` of the kind
     test fills the delay slot of the flag byte's branch to the exit
     (65/71); the target leaves that slot empty, which is what this
     compiler does when the result register is live at the exit -- an
     int function that returns no value. What it was meant to return (a
     found / not-found flag would be the natural reading) cannot be
     recovered: $v0 at the exit is whatever was computed last.

   Other shapes the bytes need: g_MapObjectDefinitions is read again at every access
   (a pointer local: 51/71); kind is a signed int (unsigned: sltiu,
   70/71); a call written out per kind (through one flag-number variable:
   52/71); r tested inside the flag arm. `break` and a bare `return;`
   give the same bytes. */
/* func_800684FC */
s32 MapObjectDefinitionSpotFind(s32 sprite_data_number, s32 *spot_x, s32 *spot_y, s32 *found_kind)
{
    s32 i;
    s32 kind;
    s32 r;

    for (i = 0; i < 0x80; i++) {
        if (((MapObjectDefinition *)g_MapObjectDefinitions)[i].sprite_data == sprite_data_number) {
            *spot_x = ((MapObjectDefinition *)g_MapObjectDefinitions)[i].x;
            *spot_y = ((MapObjectDefinition *)g_MapObjectDefinitions)[i].y;
            kind = (((MapObjectDefinition *)g_MapObjectDefinitions)[i].kind & 0x3F) + 1;
            *found_kind = kind;
            if (((MapObjectDefinition *)g_MapObjectDefinitions)[i].story_bit != 0) {
                if (kind < 4) {
                    r = StoryBitCheck(((MapObjectDefinition *)g_MapObjectDefinitions)[i].story_bit + 0x580);
                } else {
                    r = StoryBitCheck((u16)(i + 0x680 + ((g_MapNumber - 0x65) << 7)));
                }
                if (r != 0) {
                    *spot_x = -0x80;
                    *spot_y = -0x80;
                }
            }
            break;
        }
    }
}

/* func_80068618 */
void *MapTblGet(s32 map_number)
{
  u8 *first_tbl;
  u8 *row_byte_then_tbl;
  int map_number_then_row;
  map_number_then_row = map_number;
  if (map_number < 0xA0)
  {
    row_byte_then_tbl = map_number_then_row + g_MapTblNumbers;
    map_number_then_row = *row_byte_then_tbl;
  }
  else
  {
    map_number_then_row = 0;
  }
  first_tbl = g_MapTbls;
  return row_byte_then_tbl = (map_number_then_row * 0x10) + first_tbl;
}

s32 MapLayerOrderSet(void);  /* TU defines this below */
void MapLayersInit(void);  /* TU defines this below */
s32 SoundMapSet(u8);
u16 MapMusicSet(u16, u16);
void MapLoad(void *tbl, s32 sw);  /* TU defines this below */
void MapReload(void);  /* TU defines this below */
void MapDataReload(void);  /* TU defines this below */
s32 ScriptResourceLoad(s32, s32);
void ScriptChange(u8 script_number);  /* TU defines this below */
void func_800692CC();  
/* func_80068654 */
u8 MapTblByte7Get(unsigned int map_number)
{
  return *((u8 *) (((u8 *) MapTblGet(map_number)) + 0x7));
}

s32 MapLayerOrderSet();                                  /* extern */

/* func_80068678 */
void MapLayersInit(void) {
    MapLayerOrderSet();
    g_MapLayersMask = 4;
}

s32 layer_order_set(s32, s32);                              /* extern */

/* func_8006869C */
s32 MapLayerOrderSet(void) {
    layer_order_set(0x304, 2);
    layer_order_set(0x400, 3);
    layer_order_set(0x800, 4);
    layer_order_set(0x30, 5);
    D_800B88FC = 4;
    g_ScriptMusicLevelRequest = 0;
}

s32 SoundMapSet(u8);                                /* extern */
u16 MapMusicSet(u16, u16);                        /* extern */
void *MapTblGet(s32);                             /* extern */
void MapLoad(void *, s32);                            /* extern */

/* func_800686EC */
void MapReload(void) {
    s32 tbl;

    tbl = MapTblGet(g_MapNumber);
    SoundMapSet(g_MapSoundMapNumber);
    g_MusicKind = MapMusicSet(g_MusicNumber, g_MusicKind);
    MapLoad((void *)tbl, 0);
    g_MapViewKeptY = -1;
    g_MapViewKeptX = -1;
}

void *MapTblGet(s32);                             /* extern */
void MapLoad(void *, s32);                            /* extern */

/* func_80068748 */
void MapDataReload(void) {
    MapLoad((void *)MapTblGet(g_MapNumber), 0);
    g_MapViewKeptY = -1;
    g_MapViewKeptX = -1;
}
extern s16 g_MapPlaceName;
extern u8 *g_MapEnterTable;

/* load_hazard_nop: MapSet */
/* func_80068780 */
void MapSet(u8 map_number, u8 music_number, u8 script_number)
{
    MapTbl rec;

    g_MapNumber = map_number;
    rec = *(MapTbl *)MapTblGet(map_number);
    g_MapPartyKind = rec.party_kind;
    g_MapPlaceName = rec.place_name;
    if (rec.place_name >= 0x51 && rec.place_name < 0x65) {
        StoryBitSet(rec.place_name + 0x28A);
    }
    if (music_number != 0xFF) {
        rec.music = music_number;
    }
    if (script_number != 0) {
        rec.script = script_number;
    }
    g_MusicNumber = rec.music;
    if (g_MapPartyKind == 0 && rec.music == 6) {
        if (StoryBitCheck(0x3C1) != 0) {
            g_MusicNumber = 7;
        }
    }
    /* a halfword store: this TU declares g_MapSoundMapNumber u8 (MapReload
       reads its low byte, as the call below does) */
    *(u16 *)&g_MapSoundMapNumber = g_MapNumber;
    SoundMapSet(g_MapSoundMapNumber);
    g_MusicKind = MapMusicSet(g_MusicNumber, g_MusicKind);
    MapLoad(&rec, 1);
    if (rec.story_bit_2d3 != 0) {
        StoryBitSet(0x2D3);
    } else {
        StoryBitClear(0x2D3);
    }
    if (g_MapEnterTable[0x16] != 0) {
        StoryBitSet(0x2D2);
    } else {
        StoryBitClear(0x2D2);
    }
}

/* far_syms: g_MusicBankNumbers (target reaches it %hi/%lo) */

/* func_8006891C */
s32 MapSeek(s32 map_number, s32 music_number)
{
  u8 local[16];
  u8 *p;
  s32 seek_music_number;
  s32 answer_then_resource;
  u8 b0;

  seek_music_number = music_number;
  answer_then_resource = sound_area_seek();
  if (answer_then_resource)
    return answer_then_resource;

  p = (u8 *)MapTblGet(map_number);
  memcpy(local, p, 16);

  b0 = local[0];

  if (seek_music_number == 0xFF)
    seek_music_number = local[15];

  if (b0 == 0) {
    if (seek_music_number == 6) {
      if (StoryBitCheck(0x3C1))
        seek_music_number = 7;
    }
  }

  if (seek_music_number != 0) {
    answer_then_resource = sound_music_seek(g_MusicBankNumbers[seek_music_number - 1]);
    if (answer_then_resource)
      return answer_then_resource;
  }

  answer_then_resource = MapResourceFind(map_number);
  if (answer_then_resource < 0)
    return answer_then_resource;

  return resource_seek(answer_then_resource);
}

s32 bg_effect_flags_set(u8);
s32 MapResourceLoad(u16, s32);
s32 palette_bg_cover_count_set(s32);
s32 palette_animation_set(u8);
s32 tile_animation_set(u8, s32, s32 *);
void ScriptChange(u8 script_number);  /* TU defines this below */
void MapEffectSet(void *, s32 *, s32);

/* func_80068A10 */
void MapLoad(void *tbl, s32 sw)
{
  s32 unset; /* never set: the original passes this variable to tile_animation_set without giving it a value, so the call gets $s3 as it stood at entry */
  s32 unused;
  s32 zeroed_col_count;
  /* CODEGEN FORCING DEVICE (empty test), not recovered source: the `if` has
     no body, so behaviour is the same without it, but then 2 of this function's 44
     instruction lines differ. */
  if (!g_MapPartyKind)
  {
  }
  /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source: the
     block runs once, so behaviour is the same without the `do` / `while (0)`, but then 3
     of this function's 44 instruction lines differ. */
  do
  {
    zeroed_col_count = 0xA;
    if (g_MapPartyKind == 0)
    {
      zeroed_col_count = 8;
    }
    palette_bg_cover_count_set(zeroed_col_count);
  }
  while (0);
  MapResourceLoad(g_MapNumber, sw);
  bg_effect_flags_set(((MapTbl *)tbl)->effect);
  MapEffectSet(tbl, &unused, sw);
  if (sw != 0)
  {
    ScriptChange(AS(s32, ((MapTbl *)tbl)->script));
  }
  palette_animation_set(((MapTbl *)tbl)->palette_animation);
  tile_animation_set(AS(s32, ((MapTbl *)tbl)->tile_animation), unset, &unused);
}
typedef struct {
    u8 b0;
    u8 b1;
    u8 b2;
} MapFxTri;
extern u8 g_MapSecondViewSpeeds[];
extern MapFxTri D_80012C98[];
extern u8 D_80012CA8[];
extern u8 D_80012CC8[];
extern s8 g_BgMode;
extern u16 g_MapLayer4Mix;
extern s16 g_MapCol;
extern s16 g_MapColSecond;
extern s16 g_MapColThird;
extern u16 g_MapMix;
extern u16 D_800B8D1E;
extern s16 D_800B8EE0;
extern s16 D_800B8EE2;
extern s16 D_800B8EE4;
extern s16 D_800B8EE6;
extern s16 g_MapSecondViewSpeedX;
extern s16 g_MapSecondViewSpeedY;

/* MapEffectSet -- the name is the splitter's: the function
   prints its own, "MapEffectSet(tbl=%x,sw=%d)" (D_80012CA8) -- the map's
   screen effects, set from the map header `tbl` (MapTblGet's record).
   Its only caller is MapLoad(tbl, sw), which passes an unused
   pointer as the second argument. MapSet (asm) calls that with
   sw = 1 and a stack copy of the record; MapReload and MapDataReload
   pass 0.

   sw != 0 first resets the state: the effect flags g_MapEffect = tbl->bB;
   the second scroll g_MapSecondViewX / g_MapSecondViewY, g_MapSecondViewSpeedX / g_MapSecondViewSpeedY
   (MapSecondViewGoalGet reads them), g_MapCol / F2 / F4, g_MapMix and
   g_MapLayer4Mix (the values handed to layer_mix_set(8, ..) and (4, ..)
   below) are zeroed; bit 3 (0x8) of g_MapLayersMask is cleared;
   layer_mix_set(8, 0), (8, 0x20), (4, 0), (4, 0x20); D_800B8EDC = 0.
   Always: g_MapViewShakeWait, g_MapViewShakeCount, g_MapViewShakeY, g_MapViewShakeX = 0 and
   layer_order_set(8, 4). With no flags that is all.

   sw != 0, by flag ("mapeffect=0x%x" is printed first, D_80012CC8):
     & 3 == 3   g_MapSecondViewSpeedX = (s8)g_MapSecondViewSpeeds[bC >> 4], g_MapSecondViewSpeedY =
                (s8)g_MapSecondViewSpeeds[bC & 0xF]; the table is -8 -4 -2 -1 0 1 2
                4 8. Values 1 and 2 do nothing here.
     & 0xC      D_800B8D1E = bD & 0x1F, a row of the 3-byte table
                D_80012C98: D_800B8EE4 = its byte 1, D_800B8EE2 = its byte
                2, D_800B8EE6 = D_800B8EE0 = 0. Otherwise func_800692CC(0)
                -- an empty function.
     & 0x30     0x10: MapColSet((bE >> 4) & 7, bE & 0xF) unless the
                first is 0; 0x20 / 0x30: ColMix(bE & 0xF, 0).
     & 0x80     g_MapLayer4Mix = (bD >> 5) * 4, and layer_mix_set(4, that).
   sw == 0 re-applies the stored values instead: layer_mix_set(8,
   g_MapMix), then (8, 0x20) when g_MapMix is 0, and (4, g_MapLayer4Mix)
   when g_MapLayer4Mix is not 0.
   Last, either way: flag 0x40 sets g_BgMode = 1 and calls
   layer_order_set(8, 2), layer_mix_switch_on(8); without it func_8001C4E4(5) when
   g_MapMix is set, else func_8001C4E4(3).

   No frame wait and no RNG draw anywhere in its call tree (raw-image
   call graph).

   Shape:
   - `g_MapEffect = tbl->bB` is the FIRST statement of the reset block. A
     byte load may alias any store (alias.c's QImode rule), so the lbu
     cannot be lifted above the nine zero stores by the scheduler: it has
     to precede them in the source, and sched1 is what drops the `sh`
     behind them. Written after the zero stores it scores 174.
   - For the same reason both D_80012C98 bytes are read into locals before
     the four stores (reading the second after the first store: 174).
   - D_80012C98 is an array of 3-byte structs: the index is `(n << 1) + n`
     off the re-read u16 D_800B8D1E.
   - tbl is `void *` (the TU's prototype) and cast at each use; a base
     pointer local is not needed.
   - Plain if / else: the two `layer_mix_set(4, (u8)g_MapLayer4Mix)` calls are
     written out in both arms and cross-jumping merges them, reload
     included -- no goto.
   - debug_printf is called through a cast because the TU's prototype for
     it carries nine parameters; the callees the TU does not declare are
     called undeclared. */
/* func_80068AC0 */
void MapEffectSet(void *tbl, s32 *unused, s32 sw)
{
    s32 m;
    u8 b;
    s32 lo;
    s32 hi;
    s32 w;
    s32 h;

    ((s32 (*)())debug_printf)(D_80012CA8, tbl, sw);
    g_BgMode = 0;
    if (sw != 0) {
        g_MapEffect = ((MapTbl *)tbl)->effect;
        g_MapSecondViewY = 0;
        g_MapSecondViewX = 0;
        g_MapSecondViewSpeedY = 0;
        g_MapSecondViewSpeedX = 0;
        g_MapColThird = 0;
        g_MapColSecond = 0;
        g_MapCol = 0;
        g_MapMix = 0;
        g_MapLayer4Mix = 0;
        g_MapLayersMask &= 0xFFF7;
        layer_mix_set(8, 0);
        layer_mix_set(8, 0x20);
        layer_mix_set(4, 0);
        layer_mix_set(4, 0x20);
        D_800B8EDC = 0;
    }
    g_MapViewShakeWait = 0;
    g_MapViewShakeCount = 0;
    g_MapViewShakeY = 0;
    g_MapViewShakeX = 0;
    layer_order_set(8, 4);
    if (g_MapEffect != 0) {
        if (sw != 0) {
            ((s32 (*)())debug_printf)(D_80012CC8, g_MapEffect);
            switch (g_MapEffect & 3) {
            case 0:
                break;
            case 3:
                g_MapSecondViewSpeedX = (s8)g_MapSecondViewSpeeds[((MapTbl *)tbl)->second_view_speeds >> 4];
                g_MapSecondViewSpeedY = (s8)g_MapSecondViewSpeeds[((MapTbl *)tbl)->second_view_speeds & 0xF];
                break;
            }
            if (g_MapEffect & 0xC) {
                D_800B8D1E = ((MapTbl *)tbl)->effect_row_mix & 0x1F;
                w = D_80012C98[D_800B8D1E].b1;
                h = D_80012C98[D_800B8D1E].b2;
                D_800B8EE6 = 0;
                D_800B8EE0 = 0;
                D_800B8EE4 = w;
                D_800B8EE2 = h;
            } else {
                func_800692CC(0);
            }
            m = g_MapEffect & 0x30;
            if (m != 0) {
                b = ((MapTbl *)tbl)->col;
                lo = b & 0xF;
                if (m == 0x10) {
                    hi = (b >> 4) & 7;
                    if (hi != 0) {
                        MapColSet(hi, lo);
                    }
                } else {
                    ColMix(lo, 0);
                }
            }
            if (g_MapEffect & 0x80) {
                g_MapLayer4Mix = (((MapTbl *)tbl)->effect_row_mix >> 5) << 2;
                layer_mix_set(4, (u8)g_MapLayer4Mix);
            }
        } else {
            layer_mix_set(8, (u8)g_MapMix);
            if (g_MapMix == 0) {
                layer_mix_set(8, 0x20);
            }
            if (g_MapLayer4Mix != 0) {
                layer_mix_set(4, (u8)g_MapLayer4Mix);
            }
        }
        if (g_MapEffect & 0x40) {
            g_BgMode = 1;
            layer_order_set(8, 2);
            layer_mix_switch_on(8);
        } else {
            func_8001C4E4(g_MapMix != 0 ? 5 : 3);
        }
    }
}

s32 ScriptResourceLoad(s32, s32);                          /* extern */

/* func_80068D7C */
void ScriptChange(u8 script_number) {
    s32 masked_script_number;

    masked_script_number = script_number & 0xFF;
    if (g_ScriptNumber != masked_script_number) {
        ScriptResourceLoad(masked_script_number, g_ScriptBase);
        g_ScriptNumber = script_number;
    }
}
