.include "macro.inc"

.section .data, "wa"

/* g_BuildStamp @ 0x80011000, 32 bytes: the build stamp text. The bytes are your image's
   (expected/code.bin), as for all of the program's data; the register row
   says what they hold. */

.align 2

nonmatching g_BuildStamp

dlabel g_BuildStamp
    .incbin "expected/code.bin", 0x0, 0x20
enddlabel g_BuildStamp
