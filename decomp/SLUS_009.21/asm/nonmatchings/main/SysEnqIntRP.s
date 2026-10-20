.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SysEnqIntRP, 0xC

glabel SysEnqIntRP
    .incbin "expected/code.bin", 0x110140, 0xC
endlabel SysEnqIntRP
    .incbin "expected/code.bin", 0x11014C, 0x4
