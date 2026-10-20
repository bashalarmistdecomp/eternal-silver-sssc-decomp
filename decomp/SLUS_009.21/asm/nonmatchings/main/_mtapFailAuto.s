.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _mtapFailAuto, 0x34C

glabel _mtapFailAuto
    .incbin "expected/code.bin", 0x11F7AC, 0x34C
endlabel _mtapFailAuto
