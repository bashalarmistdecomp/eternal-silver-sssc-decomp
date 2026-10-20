.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_getsector, 0x100

glabel CD_getsector
    .incbin "expected/code.bin", 0x116B18, 0x100
endlabel CD_getsector
