.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching chkRC2wait, 0xA0

glabel chkRC2wait
    .incbin "expected/code.bin", 0x11FBD0, 0xA0
endlabel chkRC2wait
