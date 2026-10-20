.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching setRC2wait, 0x20

glabel setRC2wait
    .incbin "expected/code.bin", 0x11FBB0, 0x20
endlabel setRC2wait
