.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching cd_read, 0x68

glabel cd_read
    .incbin "expected/code.bin", 0x1176EC, 0x68
endlabel cd_read
    .incbin "expected/code.bin", 0x117754, 0xC
