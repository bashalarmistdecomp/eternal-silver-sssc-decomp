.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StGetBackloc, 0x58

glabel StGetBackloc
    .incbin "expected/code.bin", 0x11834C, 0x58
endlabel StGetBackloc
    .incbin "expected/code.bin", 0x1183A4, 0xC
