.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching func_80090044, 0x30

glabel func_80090044
    .incbin "expected/code.bin", 0x7F044, 0x18
  alabel D_8009005C
    .incbin "expected/code.bin", 0x7F05C, 0x18
endlabel func_80090044
  alabel D_80090074
    .incbin "expected/code.bin", 0x7F074, 0x4
