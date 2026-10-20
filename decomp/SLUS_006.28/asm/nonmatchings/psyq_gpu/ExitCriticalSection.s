.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching ExitCriticalSection, 0x10

glabel ExitCriticalSection
    .incbin "expected/code.bin", 0x7A3C8, 0x10
endlabel ExitCriticalSection
