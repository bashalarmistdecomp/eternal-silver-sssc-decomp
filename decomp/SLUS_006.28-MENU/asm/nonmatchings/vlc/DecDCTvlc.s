.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten (Sony libpress vlc object), with the state-save exit at
   0x8019765C that the bgez at 0x80197518 reaches after the jr. */
nonmatching DecDCTvlc, 0x340

glabel DecDCTvlc
    .incbin "expected/code.bin", 0x1350, 0x340
endlabel DecDCTvlc
