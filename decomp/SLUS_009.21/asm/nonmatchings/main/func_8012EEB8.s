.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012EEB8, 0x27C

glabel func_8012EEB8
    .incbin "expected/code.bin", 0x11EDB8, 0x27C
endlabel func_8012EEB8
