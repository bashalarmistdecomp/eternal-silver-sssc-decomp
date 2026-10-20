/* D_80011884: 0x14 bytes carved from boot_data into xa.c's .rodata, 2026-10-09 — a format string, two Shift-JIS 0x8190 characters then "PSchk(%x,%x)\n". It sits between MoviePlay's switch table and the TU's next table (0x800118C8), so it rides in the TU's block as a raw bridge; no alignment before it. */
dlabel D_80011884
    .incbin "expected/code.bin", 0x884, 0x14
enddlabel D_80011884
