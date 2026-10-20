/* face.h: written from the readings of 2026-10-19 (docs/naming.md,
   "Structs and their fields"). */
#ifndef FACE_H
#define FACE_H

#include "layout.h"

/* FaceDataHead: the first 0x20 bytes of a face data (FACE.C), as it lies in
   a face slot (0x3000 bytes a slot from 0x801A9B00, FaceSlotAddress): its
   mark, the length and sum of its packed pictures, and the face numbers of
   its pictures. The packed pictures follow it at +0x20: FaceDataCheck sums
   them, FaceImageLoad unpacks them (0x1000 bytes a picture). FaceDataLoad
   and FaceSlot6Load read a face data from the disc into a slot
   (resource_load). */
typedef struct {
    /* +0x00 the mark 0x45434146, the letters FACE in the order they lie in
       memory: FaceDataCheck accepts no data without it. Written only by the
       reads from the disc. */
    s32 mark;
    /* +0x04 the length in bytes of the packed pictures from +0x20:
       FaceDataCheck accepts 1 to 0x4E1F and sums length / 4 words. */
    s32 packed_length;
    /* +0x08 the sum of the packed pictures' 32-bit words, which
       FaceDataCheck compares with its own. */
    s32 sum;
    /* +0x0C..+0x0F nothing reads or writes them. */
    u8 pad_0c[4];
    /* +0x10..+0x19 the face numbers of the data's pictures, one byte a
       picture: FaceSlot6Find looks the face asked for up in slot 6's ten
       and answers its place, the number of the picture FaceImageLoad then
       unpacks. */
    u8 faces[10];
    /* +0x1A..+0x1F nothing reads or writes them. */
    u8 pad_1a[6];
} FaceDataHead;

LAYOUT_SIZE(FaceDataHead, 0x20);
LAYOUT_AT(FaceDataHead, mark, mark, 0x00);
LAYOUT_AT(FaceDataHead, packed_length, packed_length, 0x04);
LAYOUT_AT(FaceDataHead, sum, sum, 0x08);
LAYOUT_AT(FaceDataHead, pad_0c, pad_0c, 0x0c);
LAYOUT_AT(FaceDataHead, faces, faces, 0x10);
LAYOUT_AT(FaceDataHead, pad_1a, pad_1a, 0x1a);

#endif
