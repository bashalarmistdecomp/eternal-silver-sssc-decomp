.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_note2pitch, 0xD0

glabel _spu_note2pitch
    .incbin "expected/code.bin", 0x11C180, 0xD0
endlabel _spu_note2pitch
