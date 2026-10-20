.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_pitch2note, 0x130

glabel _spu_pitch2note
    .incbin "expected/code.bin", 0x11C250, 0x130
endlabel _spu_pitch2note
