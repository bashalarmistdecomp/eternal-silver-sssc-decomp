.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadSetActAlign, 0x38

glabel PadSetActAlign
    .incbin "expected/code.bin", 0x11C7CC, 0x38
endlabel PadSetActAlign
