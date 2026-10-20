.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching movie_play, 0x560

glabel movie_play
    .incbin "expected/code.bin", 0x10E290, 0x560
endlabel movie_play
