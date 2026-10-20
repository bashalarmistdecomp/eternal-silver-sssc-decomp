.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80127B08, 0xD0

glabel func_80127B08
    .incbin "expected/code.bin", 0x117A08, 0xD0
endlabel func_80127B08
