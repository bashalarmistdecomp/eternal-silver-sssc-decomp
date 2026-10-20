.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8008FE08

dlabel func_8008FE08
    .incbin "expected/code.bin", 0x7EE08, 0x8
enddlabel func_8008FE08
