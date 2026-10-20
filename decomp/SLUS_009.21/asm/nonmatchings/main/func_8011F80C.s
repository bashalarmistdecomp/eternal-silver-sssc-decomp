.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011F80C, 0x148

glabel func_8011F80C
    .incbin "expected/code.bin", 0x10F70C, 0x148
endlabel func_8011F80C
