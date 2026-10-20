/* jtbl_80012488: BtlAmountRecordSet's switch table, second in the original
   0x8001246C..0x800124E4 region (BtlAmountRecordSet, C in btl_amount.c since,
   reaches it %hi/%lo). Carved from the boot_data tail into the first split's misc.c's
   .rodata, 2026-10-09 (BtlMenuActionCheck, the first data-in-region carve).
   No .align 3: the original
   layout has zero gap after jtbl_8001246C (section offset 0x1c is 4 mod 8 -
   aligning would pad 4 bytes). */
dlabel jtbl_80012488
    .incbin "expected/code.bin", 0x1488, 0x24
enddlabel jtbl_80012488
