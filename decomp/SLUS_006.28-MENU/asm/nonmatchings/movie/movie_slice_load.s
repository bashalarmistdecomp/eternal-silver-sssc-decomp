.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching movie_slice_load, 0xF4

glabel movie_slice_load
    .incbin "expected/code.bin", 0x7DC, 0xF4
endlabel movie_slice_load
