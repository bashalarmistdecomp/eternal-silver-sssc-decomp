/* g_BtlWinRewardMessage: 0x14 bytes carved from the boot_data tail into btl_amount.c's
   .rodata, 2026-10-09 (BtlMenuActionCheck, the first data-in-region carve);
   btl_message.c's converted functions memcpy() 16 of its bytes to a stack buffer. */
dlabel g_BtlWinRewardMessage
    .incbin "expected/code.bin", 0x14CC, 0xC
enddlabel g_BtlWinRewardMessage
