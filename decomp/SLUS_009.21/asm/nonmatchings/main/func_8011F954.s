.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011F954, 0xA0

glabel func_8011F954
    .incbin "expected/code.bin", 0x10F854, 0xA0
endlabel func_8011F954
