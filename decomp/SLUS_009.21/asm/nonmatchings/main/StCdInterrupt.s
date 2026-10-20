.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StCdInterrupt, 0x91C

glabel StCdInterrupt
    .incbin "expected/code.bin", 0x118610, 0x91C
endlabel StCdInterrupt
