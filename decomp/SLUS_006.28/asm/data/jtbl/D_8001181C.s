/* D_8001181C: 0x14 bytes carved from boot_data into xa.c's .rodata, 2026-10-09 — "xa_play: DiskError\n". It sits between MoviePlay's switch table and the TU's next table (0x800118C8), so it rides in the TU's block as a raw bridge; no alignment before it. */
dlabel D_8001181C
    .incbin "expected/code.bin", 0x81C, 0x14
.align 2
enddlabel D_8001181C
