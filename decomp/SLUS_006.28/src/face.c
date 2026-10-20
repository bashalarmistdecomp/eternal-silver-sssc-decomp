/* Lunar SSSC — face: the faces: the six data slots and slot 6, their loads
   and checks, and the face cells on the grid. The name is the developers'
   own: FACE.C, the source file FaceSlotGet's assertion names ('FACE DATA
   CRASH!', line 0x9E); the file's other functions have it by being of its
   group. The first split had it in ui.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
#include "face.h"
extern u8 g_FaceSlotNumbers;
extern u8 g_FaceSlotLast;
void FaceSlotReset();                                  s32 resource_load(s32, s32);                               /* extern */
s32 FaceSlotAddress(s32);                               /* extern */
void assert_fail();  /* extern */

/* func_8001B15C */
s32 FaceSlotAddress(s32 slot) {
    return (slot * 0x3000) + 0x801A9B00;
}
/* far_syms: g_FaceSlotNumbers */

/* func_8001B178 */
void FaceSlotReset(void)
{
  s32 i;
  u8 *p;
  i = 5;
  p = &g_FaceSlotNumbers + 5;
  do
  {
    *p = 0;
    i--;
    p--;
  } while (i >= 0);
  g_FaceSlotLast = 0;
}

/* Ruling of 2026-10-14 (docs/naming.md): Does what FaceSlotReset does; nothing read says why there are two. */
/* func_8001B1A0 */
void FaceSlotResetCopy(void) {
    FaceSlotReset();
}
/* func_8001B1C0 */
void FaceDataLoad(s32 slot, s32 face_data_number) {
    s32 destination;
    s32 resource;

    destination = FaceSlotAddress(slot);
    resource = face_data_number + 0xA2;
    if (face_data_number >= 0x20) {
        resource = face_data_number + 0x8E;
    }
    resource_load(resource, destination);
}
/* far_syms: D_800110C4 (target reaches it %hi/%lo) */
/* far_syms: D_800B85A0 (target reaches it %hi/%lo) */
/* far_syms: g_FaceSlotNumbers (target reaches it %hi/%lo) */
s32 FaceDataCheck(void *face_data);  /* TU defines this below */
extern u8 D_800110C4[];
extern u8 D_800B85A0[];

/* func_8001B200 */
s32 FaceSlotGet(s32 face_data_number)
{
  void *slot_face_data;
  s32 i;
  for (i = 0; i < 6; i++)
  {
    if ((face_data_number + 1) == (*((s8 *) ((&g_FaceSlotNumbers) + i))))
    {
      if (FaceDataCheck(slot_face_data = (void *) FaceSlotAddress(i)))
      {
        return i;
      }
      *((u8 *) ((&g_FaceSlotNumbers) + i)) = 0;
    }
  }

  g_FaceSlotLast++;
  if (((s8) g_FaceSlotLast) >= 6)
  {
    g_FaceSlotLast = 0;
  }
 FaceDataLoad((s8) g_FaceSlotLast, face_data_number); 
  *((s8 *) ((&g_FaceSlotNumbers) + ((s8) g_FaceSlotLast))) = face_data_number + 1;
  if (!FaceDataCheck((void *) FaceSlotAddress((s8) g_FaceSlotLast)))
  {
    assert_fail(D_800110C4, D_800B85A0, 0x9E);
  }
  return (s8) g_FaceSlotLast;
}
s32 inflate_buffer(s32, s32);
s32 DrawSync(s32);
extern s32 g_LoadBuffer;

/* func_8001B310 */
void FaceImageLoad(s32 slot, s32 picture, s32 page)
{
  RECT rect;
  s32 buf;
  buf = g_LoadBuffer;
  inflate_buffer(FaceSlotAddress(slot) + 0x20, buf);
  rect.x = (page << 5) + 0x100;
  rect.y = 0x1C0;
  rect.w = 0x20;
  rect.h = 0x40;
  buf += picture << 0xC;
  LoadImage(&rect, (u_long *) buf);
  DrawSync(0);
}

/* Loads the portrait file that holds face `face` into the portrait cache
   (FaceSlotGet(file), file = (face - 1) / 5 + 1: the faces come five to a file);
   face 0 does nothing. The preload half of FaceCellSet, which goes on to
   upload the image. No return value. */
/* func_8001B394 */
void FaceLoad(s32 face)
{
  if (face != 0)
  {
    face--;
    FaceSlotGet(face / 5 + 1);
  }
}
extern s32 g_FaceCellPage;
s32 FaceSlot6Find(s32 face);  /* TU defines this below */
void ankprint_cell_set(s32 column, s32 row, s32 cell);

/* Put FACE number `face` (a 64 x 64 portrait) on the 40 x 28 cell grid at
   cell (x, y); face 0 clears the cell.

     face == 0:  ankprint_cell_set(x, y, 0) and return.
     frame = FaceSlot6Find(face)   is this face one of the ten listed in
                                   the special block held in portrait slot
                                   6 (and is that block valid)? its index
                                   there, or -1.
     found:      slot = 6.
     not found:  the ordinary faces come five to a file:
                   file  = (face - 1) / 5 + 1
                   frame = (face - 1) % 5
                   slot  = FaceSlotGet(file)   the portrait cache: the slot that
                                        already holds the file, or the next
                                        slot round-robin after loading it
                                        from the disc (FaceDataLoad ->
                                        resource_load) -- so this path can read
                                        the CD.
     FaceImageLoad(slot, frame, g_FaceCellPage)
                                   unpack the block in that slot
                                   (inflate_buffer into g_LoadBuffer) and
                                   LoadImage its image `frame` (0x1000
                                   bytes: 64 x 64 at 8bpp) to VRAM
                                   (0x100 + 32 * page, 0x1C0), 32 x 64
                                   halfwords; DrawSync(0) inside.
     ankprint_cell_set(x, y, g_FaceCellPage | 0x400)
                                   the cell word: type 4 (bits 8..10) is
                                   FaceCellDraw's 64 x 64 portrait sprite,
                                   bit 0 the VRAM page it reads.
     g_FaceCellPage ^= 1               the page alternates, so two portraits
                                   can be on screen at once and a third
                                   overwrites the older one's pixels.

   State: reads and writes g_FaceCellPage (the portrait page, 0 / 1); the
   portrait cache through FaceSlotGet (g_FaceSlotNumbers[6], g_FaceSlotLast); the cell grid
   through ankprint_cell_set (g_AnkprintGrid, g_AnkprintRowMarks). Callers: BtlMenuActingFaceSet
   (btl_window.c: the current party member's first face, (id - 1) * 5 + 1) and
   CellFacePlacesSet (cell_face.c: the dialogue face slots). No RNG draw. No
   VSync wait of its own (DrawSync in FaceImageLoad, and FaceSlotGet's file read
   on a cache miss, do wait on hardware). No return statement: $v0 is the
   toggled page on the long path and whatever ankprint_cell_set leaves on the
   short one; BtlMenuActingFaceSet passes it on and nothing reads it.

   Shapes the rows depend on (each measured, see the header):
   - MARKED SPELLING: the lookup variable `frame` is the working variable
     of the division (`frame = face / 5; q = frame; frame = face;
     frame %= 5;`): the quotient goes through it into `q`, then it takes
     the face number and is reduced modulo 5. Without it, with quotient
     and remainder written directly (`q = face / 5; frame = face % 5;` or
     `face - q * 5`): 41/53. combine folds all of that into the target's
     ten instructions, but global-alloc orders the saved registers by
     references over lifetime as flow counted them BEFORE combine: `frame`
     then has nine and takes $s0 ahead of the face number; written
     directly it has five, the face number is first, and $s0 / $s1 are
     exchanged. The Japanese build, compiled by GCC 2.7.2, needs the same,
     so the original gave `frame` these references. That it did so with
     these four statements is not something the bytes say: no dead code
     and no test without effect, but not how a quotient and a remainder
     are usually written (tagged on the owner's word);
   - the division is written on `face`, so the decrement stays in place in
     $s1 (with `frame = face` first it is merged into the copy);
   - the image-load call stands in both arms. */
/* func_8001B3DC */
void FaceCellSet(s32 x, s32 y, s32 face)
{
  s32 frame;
  s32 q;

  if (face == 0)
  {
    ankprint_cell_set(x, y, 0);
    return;
  }
  frame = FaceSlot6Find(face);
  if (frame >= 0)
  {
    FaceImageLoad(6, frame, g_FaceCellPage);
  }
  else
  {
    face--;
    frame = face / 5;
    q = frame;
    frame = face;
    frame %= 5;
    FaceImageLoad(FaceSlotGet(q + 1), frame, g_FaceCellPage);
  }
  ankprint_cell_set(x, y, g_FaceCellPage | 0x400);
  g_FaceCellPage ^= 1;
}

s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32); /* extern */

/* func_8001B4B0 */
void FaceCellDraw(s32 column, s32 row, s32 cell) {
    packet_sprite_add(column * 8, row * 8, (cell & 1) << 6, 0xC0, 0x40, 0x40, 0x94, 0x7DC0, 0x10);
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_8001B504() {

}

/* face_data_number stays s32 to agree with face_list.c's declaration; the `< 0x2BU`
   compare is unsigned either way (usual arithmetic conversions), so the
   `sltiu` is unchanged. */
/* func_8001B50C */
void FaceSlot6Load(s32 face_data_number) {
    s32 destination;

    destination = FaceSlotAddress(6);
    if (face_data_number < 0x2BU) {
        resource_load(face_data_number + 0xBA, destination);
    }
}

/* func_8001B54C */
s32 FaceDataCheck(void *face_data)
{
  s32 *packed_data;
  s32 packed_length;
  s32 word;
  s32 sum;
  s32 result;
  s32 biased_length;
  s32 words_done;
  packed_length = ((FaceDataHead *) face_data)->packed_length;
  packed_data = face_data + 0x20;
  if ((((FaceDataHead *) face_data)->mark == 0x45434146) && ((sum = 0, (((u32) (packed_length - 1)) < 0x4E1FU) != 0)))
  {
    words_done = 0;
    loop_3:
    biased_length = packed_length;

    if (biased_length < 0)
    {
      biased_length += 3;
    }
    if (words_done < (biased_length >> 2))
    {
      words_done += 1;
      word = *packed_data;
      packed_data += 1;
      sum += word;
      goto loop_3;
    }
    result = 1;
    if (sum != ((FaceDataHead *) face_data)->sum)
    {
      goto block_8;
    }
    return result;
  }
  else
  {
    block_8:
    result = 0;

    return result;
  }
}
/* The face byte is written *(picture + head->faces), the number first:
   as head->faces[picture] the sum is formed address first and the
   operands of one addu change places. */
/* func_8001B5C4 */
s32 FaceSlot6Find(s32 face) {
    s32 face_data_address;
    s32 picture;

    face_data_address = FaceSlotAddress(6);
    picture = 0;
    while (picture < 0xA) {
        if (face == *(picture + ((FaceDataHead *)face_data_address)->faces)) {
            if (FaceDataCheck((void *)face_data_address)) {
                return picture;
            }
        }
        picture++;
    }
    return -1;
}
