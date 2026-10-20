/* D_80011898: 0x24 bytes carved from boot_data into xa.c's .rodata, 2026-10-09 — "printmes()'s x,y parameter error". It sits between MoviePlay's switch table and the TU's next table (0x800118C8), so it rides in the TU's block as a raw bridge; no alignment before it. */
dlabel D_80011898
    .incbin "expected/code.bin", 0x898, 0x21
.align 2
enddlabel D_80011898
