/* D_800118BC: 0xc bytes carved from boot_data into xa.c's .rodata, 2026-10-09 — "ANKPRINT.C". It sits between MoviePlay's switch table and the TU's next table (0x800118C8), so it rides in the TU's block as a raw bridge; no alignment before it. */
dlabel D_800118BC
    .incbin "expected/code.bin", 0x8BC, 0xB
.align 2
enddlabel D_800118BC
