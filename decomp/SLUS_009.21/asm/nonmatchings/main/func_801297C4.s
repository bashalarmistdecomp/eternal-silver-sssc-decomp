.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801297C4, 0x8C

glabel func_801297C4
    .incbin "expected/code.bin", 0x1196C4, 0x8C
endlabel func_801297C4
