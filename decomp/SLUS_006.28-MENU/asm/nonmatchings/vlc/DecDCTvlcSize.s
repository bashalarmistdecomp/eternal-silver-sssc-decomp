.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten (Sony libpress VLC.OBJ): the size word DecDCTvlcSize keeps in
   .text and reads and writes through $t0, then DecDCTvlcSize itself from
   0x80197320, with the tail at 0x80197340 that the blez at 0x8019732C
   reaches after the jr. */
dlabel D_8019731C
    .incbin "expected/code.bin", 0x131C, 0x4
enddlabel D_8019731C

nonmatching DecDCTvlcSize, 0x30

glabel DecDCTvlcSize
    .incbin "expected/code.bin", 0x1320, 0x30
endlabel DecDCTvlcSize
