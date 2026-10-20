.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching set_alarm, 0x34

glabel set_alarm
    .incbin "expected/code.bin", 0x113B70, 0x34
endlabel set_alarm
