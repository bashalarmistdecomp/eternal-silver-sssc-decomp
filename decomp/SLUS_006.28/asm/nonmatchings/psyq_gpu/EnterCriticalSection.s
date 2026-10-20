.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching EnterCriticalSection, 0x10

glabel EnterCriticalSection
    .incbin "expected/code.bin", 0x7A3B8, 0x10
endlabel EnterCriticalSection
