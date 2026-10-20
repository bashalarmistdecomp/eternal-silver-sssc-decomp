.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ReturnFromException, 0xC

glabel ReturnFromException
    .incbin "expected/code.bin", 0x842B8, 0xC
endlabel ReturnFromException
    .incbin "expected/code.bin", 0x842C4, 0x4
