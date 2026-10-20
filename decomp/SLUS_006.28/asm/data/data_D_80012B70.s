.include "macro.inc"

.section .data, "wa"

/* g_MoveChrScales @ 0x80012B70, 20 bytes: the move commands' ten scales. The bytes are your image's
   (expected/code.bin), as for all of the program's data; the register row
   says what they hold. */

.align 2

nonmatching g_MoveChrScales

dlabel g_MoveChrScales
    .incbin "expected/code.bin", 0x1B70, 0x14
enddlabel g_MoveChrScales
