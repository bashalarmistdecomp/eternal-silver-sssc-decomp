.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ReturnFromException, 0xC

glabel ReturnFromException
    .incbin "expected/code.bin", 0x10FD90, 0xC
endlabel ReturnFromException
    .incbin "expected/code.bin", 0x10FD9C, 0x4
