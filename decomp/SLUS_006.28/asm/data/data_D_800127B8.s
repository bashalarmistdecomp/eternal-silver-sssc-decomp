.include "macro.inc"

.section .data, "wa"

/* g_MusicBankNumbers @ 0x800127B8, 68 bytes: a bank number for each music number. The bytes are your image's
   (expected/code.bin), as for all of the program's data; the register row
   says what they hold. */

.align 2

nonmatching g_MusicBankNumbers

dlabel g_MusicBankNumbers
    .incbin "expected/code.bin", 0x17B8, 0x44
enddlabel g_MusicBankNumbers
