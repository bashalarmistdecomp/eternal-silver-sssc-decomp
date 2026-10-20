.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching movie_run, 0xA0

glabel movie_run
    .incbin "expected/code.bin", 0x10E1F0, 0xA0
endlabel movie_run
