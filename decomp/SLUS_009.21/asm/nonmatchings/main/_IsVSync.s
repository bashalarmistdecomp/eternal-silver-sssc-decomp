.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _IsVSync, 0x40

glabel _IsVSync
    .incbin "expected/code.bin", 0x1100BC, 0x40
endlabel _IsVSync
    .incbin "expected/code.bin", 0x1100FC, 0x4
