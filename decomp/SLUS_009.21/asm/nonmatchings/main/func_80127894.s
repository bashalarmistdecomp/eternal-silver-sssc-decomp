.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80127894, 0x274

glabel func_80127894
    .incbin "expected/code.bin", 0x117794, 0x274
endlabel func_80127894
