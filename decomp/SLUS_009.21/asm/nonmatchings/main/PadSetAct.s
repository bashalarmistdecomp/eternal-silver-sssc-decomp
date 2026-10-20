.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadSetAct, 0x48

glabel PadSetAct
    .incbin "expected/code.bin", 0x11C84C, 0x48
endlabel PadSetAct
    .incbin "expected/code.bin", 0x11C894, 0xC
