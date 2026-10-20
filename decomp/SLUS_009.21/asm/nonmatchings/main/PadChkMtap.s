.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadChkMtap, 0x4C

glabel PadChkMtap
    .incbin "expected/code.bin", 0x11C440, 0x4C
endlabel PadChkMtap
