.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SysEnqIntRP, 0xC

glabel SysEnqIntRP
    .incbin "expected/code.bin", 0x904F8, 0xC
endlabel SysEnqIntRP
    .incbin "expected/code.bin", 0x90504, 0x4
