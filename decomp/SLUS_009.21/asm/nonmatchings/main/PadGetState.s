.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadGetState, 0xCC

glabel PadGetState
    .incbin "expected/code.bin", 0x11C48C, 0xCC
endlabel PadGetState
