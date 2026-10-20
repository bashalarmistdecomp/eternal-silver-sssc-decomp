.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdReadMode, 0x14

glabel CdReadMode
    .incbin "expected/code.bin", 0x118114, 0x14
endlabel CdReadMode
    .incbin "expected/code.bin", 0x118128, 0x8
