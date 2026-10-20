.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching __SN_ENTRY_POINT

glabel __SN_ENTRY_POINT
    .incbin "expected/code.bin", 0x2C4C, 0xBC
enddlabel __SN_ENTRY_POINT
