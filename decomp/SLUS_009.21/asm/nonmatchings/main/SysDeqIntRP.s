.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SysDeqIntRP, 0xC

glabel SysDeqIntRP
    .incbin "expected/code.bin", 0x110150, 0xC
endlabel SysDeqIntRP
    .incbin "expected/code.bin", 0x11015C, 0x4
