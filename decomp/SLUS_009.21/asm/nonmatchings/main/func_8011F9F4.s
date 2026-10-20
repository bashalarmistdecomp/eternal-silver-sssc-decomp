.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011F9F4, 0x78

glabel func_8011F9F4
    .incbin "expected/code.bin", 0x10F8F4, 0x78
endlabel func_8011F9F4
