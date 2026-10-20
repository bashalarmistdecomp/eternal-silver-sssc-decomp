.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CastleOptionsRun, 0x5224

glabel CastleOptionsRun
    .incbin "expected/code.bin", 0xFF45C, 0x5224
endlabel CastleOptionsRun
