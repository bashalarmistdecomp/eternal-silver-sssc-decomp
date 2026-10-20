.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011FD7C, 0xAC

glabel func_8011FD7C
    .incbin "expected/code.bin", 0x10FC7C, 0xAC
endlabel func_8011FD7C
