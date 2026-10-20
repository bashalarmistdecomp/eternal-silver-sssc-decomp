/* jtbl_80011ECC: BtlShow9BRun's switch table, emitted from btl_acting.c's .rodata while BtlShow9BRun is asm (jtbl_carve); apply_match swaps the INCLUDE_RODATA for its alignment pad when BtlShow9BRun converts */
.align 3
dlabel jtbl_80011ECC
    .incbin "expected/code.bin", 0xECC, 0x1D8
enddlabel jtbl_80011ECC
