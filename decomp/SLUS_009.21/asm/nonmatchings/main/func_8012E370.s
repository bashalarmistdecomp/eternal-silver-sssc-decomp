.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012E370, 0x48

glabel func_8012E370
    .incbin "expected/code.bin", 0x11E270, 0x48
endlabel func_8012E370
