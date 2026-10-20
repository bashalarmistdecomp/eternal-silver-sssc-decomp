.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011F63C, 0x1D0

glabel func_8011F63C
    .incbin "expected/code.bin", 0x10F53C, 0x1D0
endlabel func_8011F63C
