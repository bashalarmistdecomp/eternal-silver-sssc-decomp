.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching EnablePAD, 0x14

glabel EnablePAD
    .incbin "expected/code.bin", 0x110160, 0x14
endlabel EnablePAD
