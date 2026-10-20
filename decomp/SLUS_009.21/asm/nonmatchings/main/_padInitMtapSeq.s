.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padInitMtapSeq, 0x34

glabel _padInitMtapSeq
    .incbin "expected/code.bin", 0x11F120, 0x34
endlabel _padInitMtapSeq
