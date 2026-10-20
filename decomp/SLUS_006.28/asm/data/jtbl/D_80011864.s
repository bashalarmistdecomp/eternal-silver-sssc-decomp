/* D_80011864: 0x20 bytes carved from boot_data into xa.c's .rodata, 2026-10-09 — "SsVabTransBody : failed !!!\n". It sits between MoviePlay's switch table and the TU's next table (0x800118C8), so it rides in the TU's block as a raw bridge; no alignment before it. */
dlabel D_80011864
    .incbin "expected/code.bin", 0x864, 0x1D
.align 2
enddlabel D_80011864
