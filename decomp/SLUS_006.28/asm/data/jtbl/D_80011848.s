/* D_80011848: 0x1c bytes carved from boot_data into xa.c's .rodata, 2026-10-09 — "SsVabOpenHead : failed !!!\n". It sits between MoviePlay's switch table and the TU's next table (0x800118C8), so it rides in the TU's block as a raw bridge; no alignment before it. */
dlabel D_80011848
    .incbin "expected/code.bin", 0x848, 0x1C
.align 2
enddlabel D_80011848
