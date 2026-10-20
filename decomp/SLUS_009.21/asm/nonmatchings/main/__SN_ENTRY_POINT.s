.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching __SN_ENTRY_POINT

dlabel __SN_ENTRY_POINT
    .incbin "expected/code.bin", 0x10EF64, 0x108
enddlabel __SN_ENTRY_POINT
