/* D_800124AC: 0x20 bytes carved from the boot_data tail into btl_amount.c's
   .rodata, 2026-10-09 (BtlMenuActionCheck, the first data-in-region carve). The
   end decodes as the debug format string "...MessageDisp(%d)\n" (btl_message.c's
   converted functions feed it to debug_printf); the six lead bytes
   (81 9a x3) are an opaque marker - kept raw. */
dlabel D_800124AC
    .incbin "expected/code.bin", 0x14AC, 0x20
enddlabel D_800124AC
