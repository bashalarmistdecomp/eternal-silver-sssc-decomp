.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padWaitRXready, 0x28

glabel _padWaitRXready
    .incbin "expected/code.bin", 0x11D63C, 0x28
endlabel _padWaitRXready
    .incbin "expected/code.bin", 0x11D664, 0xC
