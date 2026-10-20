.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching nextfile, 0xC

glabel nextfile
    .incbin "expected/code.bin", 0x92618, 0xC
endlabel nextfile
    .incbin "expected/code.bin", 0x92624, 0x4
