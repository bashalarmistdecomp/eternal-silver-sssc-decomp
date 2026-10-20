.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StoreImage2, 0xEC

glabel StoreImage2
    .incbin "expected/code.bin", 0x113E74, 0xEC
endlabel StoreImage2
