.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching puts, 0x54

glabel puts
    .incbin "expected/code.bin", 0x1211C0, 0x54
endlabel puts
    .incbin "expected/code.bin", 0x121214, 0xC
