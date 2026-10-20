.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching data_ready_callback, 0x8C

glabel data_ready_callback
    .incbin "expected/code.bin", 0x1182C0, 0x8C
endlabel data_ready_callback
