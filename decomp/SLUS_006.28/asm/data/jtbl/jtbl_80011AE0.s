/* jtbl_80011AE0: BtlActionTargetAdjust's switch table, emitted from btl_action.c's .rodata while BtlActionTargetAdjust is asm (jtbl_carve); apply_match swaps the INCLUDE_RODATA for its alignment pad when BtlActionTargetAdjust converts */
.align 3
dlabel jtbl_80011AE0
    .incbin "expected/code.bin", 0xAE0, 0x68
enddlabel jtbl_80011AE0
