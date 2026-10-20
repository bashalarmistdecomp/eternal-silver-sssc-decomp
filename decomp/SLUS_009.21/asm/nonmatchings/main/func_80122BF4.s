.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80122BF4, 0xE0

glabel func_80122BF4
    .incbin "expected/code.bin", 0x112AF4, 0xE0
endlabel func_80122BF4
