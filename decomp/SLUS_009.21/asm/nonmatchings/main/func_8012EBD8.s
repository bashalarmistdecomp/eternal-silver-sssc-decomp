.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012EBD8, 0x74

glabel func_8012EBD8
    .incbin "expected/code.bin", 0x11EAD8, 0x74
endlabel func_8012EBD8
