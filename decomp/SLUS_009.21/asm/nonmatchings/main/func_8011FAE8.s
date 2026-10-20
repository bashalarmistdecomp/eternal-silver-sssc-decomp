.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011FAE8, 0x6C

glabel func_8011FAE8
    .incbin "expected/code.bin", 0x10F9E8, 0x6C
endlabel func_8011FAE8
