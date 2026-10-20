.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching pad_buffer_decode, 0x270

glabel pad_buffer_decode
    .incbin "expected/code.bin", 0xFF128, 0x270
endlabel pad_buffer_decode
