.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _putchar_flash, 0x3C

glabel _putchar_flash
    .incbin "expected/code.bin", 0x12075C, 0x3C
endlabel _putchar_flash
