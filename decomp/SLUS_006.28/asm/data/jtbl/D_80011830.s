/* D_80011830: 0x18 bytes carved from boot_data into xa.c's .rodata, 2026-10-09 — "sound_buf_add was exhaust!". It sits between MoviePlay's switch table and the TU's next table (0x800118C8), so it rides in the TU's block as a raw bridge; no alignment before it. */
dlabel D_80011830
    .incbin "expected/code.bin", 0x830, 0x17
.align 2
enddlabel D_80011830
