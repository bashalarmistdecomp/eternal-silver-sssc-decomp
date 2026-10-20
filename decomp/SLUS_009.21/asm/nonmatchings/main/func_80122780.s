.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80122780, 0x270

glabel func_80122780
    .incbin "expected/code.bin", 0x112680, 0x270
endlabel func_80122780
