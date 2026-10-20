.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011F338, 0x98

glabel func_8011F338
    .incbin "expected/code.bin", 0x10F238, 0x98
endlabel func_8011F338
