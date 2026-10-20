/* g_BtlLevelUpGainsMessage: 0x0C bytes (a Msg10 in btl_message.c) carved from the boot_data
   tail into btl_amount.c's .rodata, 2026-10-09 (BtlMenuActionCheck, the first
   data-in-region carve). */
dlabel g_BtlLevelUpGainsMessage
    .incbin "expected/code.bin", 0x14D8, 0xC
enddlabel g_BtlLevelUpGainsMessage
