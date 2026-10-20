.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching lseek, 0xC

glabel lseek
    .incbin "expected/code.bin", 0x7A3E8, 0xC
endlabel lseek
    .incbin "expected/code.bin", 0x7A3F4, 0x4
