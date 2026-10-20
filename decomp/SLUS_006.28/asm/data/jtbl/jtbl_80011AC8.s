/* jtbl_80011AC8: BtlActionTargetAdjust's switch table, emitted from btl_action.c's .rodata while BtlActionTargetAdjust is asm (jtbl_carve); apply_match swaps the INCLUDE_RODATA for its alignment pad when BtlActionTargetAdjust converts */
.align 3
dlabel jtbl_80011AC8
    .incbin "expected/code.bin", 0xAC8, 0x18
enddlabel jtbl_80011AC8
