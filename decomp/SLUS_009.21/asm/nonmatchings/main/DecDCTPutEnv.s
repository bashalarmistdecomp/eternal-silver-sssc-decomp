.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DecDCTPutEnv, 0x98

glabel DecDCTPutEnv
    .incbin "expected/code.bin", 0x119350, 0x98
endlabel DecDCTPutEnv
