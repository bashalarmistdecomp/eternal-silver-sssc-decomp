.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DecDCTGetEnv, 0x8C

glabel DecDCTGetEnv
    .incbin "expected/code.bin", 0x1192C4, 0x8C
endlabel DecDCTGetEnv
