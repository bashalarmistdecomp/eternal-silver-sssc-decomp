.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80122F04, 0x23C

glabel func_80122F04
    .incbin "expected/code.bin", 0x112E04, 0x23C
endlabel func_80122F04
