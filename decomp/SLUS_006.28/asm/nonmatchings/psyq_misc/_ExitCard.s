.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _ExitCard, 0x70

glabel _ExitCard
    .incbin "expected/code.bin", 0x90878, 0x70
endlabel _ExitCard
  alabel D_800A18E8
    .incbin "expected/code.bin", 0x908E8, 0xC
  alabel D_800A18F4
    .incbin "expected/code.bin", 0x908F4, 0x4
