.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012ED64, 0x44

glabel func_8012ED64
    .incbin "expected/code.bin", 0x11EC64, 0x44
endlabel func_8012ED64
