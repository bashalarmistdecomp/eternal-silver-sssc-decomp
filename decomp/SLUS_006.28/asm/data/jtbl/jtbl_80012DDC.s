/* jtbl_80012DDC: MenuCommandsSet's switch table, emitted from menu_run.c's .rodata while MenuCommandsSet is asm (jtbl_carve); apply_match swaps the INCLUDE_RODATA for its alignment pad when MenuCommandsSet converts */
.align 3
dlabel jtbl_80012DDC
    .incbin "expected/code.bin", 0x1DDC, 0x60
enddlabel jtbl_80012DDC
