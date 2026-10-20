.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8011FB80, 0x24

glabel func_8011FB80
    .incbin "expected/code.bin", 0x10FA80, 0x24
endlabel func_8011FB80
    .incbin "expected/code.bin", 0x10FAA4, 0xC
