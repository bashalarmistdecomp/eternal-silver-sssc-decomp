.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padSetMainMode, 0x98

glabel _padSetMainMode
    .incbin "expected/code.bin", 0x11E078, 0x98
endlabel _padSetMainMode
