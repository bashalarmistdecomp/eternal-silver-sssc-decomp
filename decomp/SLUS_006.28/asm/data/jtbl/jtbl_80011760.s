/* jtbl_80011760: MoviePlay's switch table, emitted from overlay.c's .rodata while MoviePlay is asm (the jtbl_carve bridge form); apply_match drops the INCLUDE_RODATA when MoviePlay converts */
.align 3
dlabel jtbl_80011760
    .incbin "expected/code.bin", 0x760, 0xBC
enddlabel jtbl_80011760
