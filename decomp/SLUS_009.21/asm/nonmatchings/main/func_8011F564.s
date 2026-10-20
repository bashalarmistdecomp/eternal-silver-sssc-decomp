.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011F564, 0xD8

glabel func_8011F564
    .incbin "expected/code.bin", 0x10F464, 0xD8
endlabel func_8011F564
