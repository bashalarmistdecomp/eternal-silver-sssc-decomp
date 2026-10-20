.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching RemovePatchPad, 0x38

glabel RemovePatchPad
    .incbin "expected/code.bin", 0x11001C, 0x38
endlabel RemovePatchPad
