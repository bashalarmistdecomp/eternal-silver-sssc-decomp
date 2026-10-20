.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012DC34, 0x3F8

glabel func_8012DC34
    .incbin "expected/code.bin", 0x11DB34, 0x3F8
endlabel func_8012DC34
