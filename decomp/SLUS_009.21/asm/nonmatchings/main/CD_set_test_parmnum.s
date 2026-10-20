.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_set_test_parmnum, 0xC

glabel CD_set_test_parmnum
    .incbin "expected/code.bin", 0x116D04, 0xC
endlabel CD_set_test_parmnum
