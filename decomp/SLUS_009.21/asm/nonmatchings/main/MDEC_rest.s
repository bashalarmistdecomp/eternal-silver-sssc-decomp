.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching MDEC_rest, 0xF0

glabel MDEC_rest
    .incbin "expected/code.bin", 0x119544, 0xF0
endlabel MDEC_rest
