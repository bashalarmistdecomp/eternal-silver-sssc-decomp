.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching HookEntryInt, 0xC

glabel HookEntryInt
    .incbin "expected/code.bin", 0x842D8, 0xC
endlabel HookEntryInt
    .incbin "expected/code.bin", 0x842E4, 0x4
