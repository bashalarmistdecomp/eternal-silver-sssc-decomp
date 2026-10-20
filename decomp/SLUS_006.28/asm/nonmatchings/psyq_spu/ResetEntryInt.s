.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ResetEntryInt, 0xC

glabel ResetEntryInt
    .incbin "expected/code.bin", 0x842C8, 0xC
endlabel ResetEntryInt
    .incbin "expected/code.bin", 0x842D4, 0x4
