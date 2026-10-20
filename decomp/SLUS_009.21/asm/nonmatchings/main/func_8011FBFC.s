.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011FBFC, 0x180

glabel func_8011FBFC
    .incbin "expected/code.bin", 0x10FAFC, 0x180
endlabel func_8011FBFC
