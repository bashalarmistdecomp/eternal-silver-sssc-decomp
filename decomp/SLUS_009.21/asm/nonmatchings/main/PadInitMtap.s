.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadInitMtap, 0x238

glabel PadInitMtap
    .incbin "expected/code.bin", 0x11E8A0, 0x238
endlabel PadInitMtap
