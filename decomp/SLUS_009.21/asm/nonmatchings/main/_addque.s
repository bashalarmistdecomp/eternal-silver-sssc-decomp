.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _addque, 0x24

glabel _addque
    .incbin "expected/code.bin", 0x1133B0, 0x24
endlabel _addque
