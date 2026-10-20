.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi EnablePAD: PSY-Q 4.5, lib/libapi.lib, member PATCH, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching EnablePAD, 0x14

glabel EnablePAD
    .reloc EnablePAD + 0x0, R_MIPS_HI16, jtbl_800EA8F8_main
    .reloc EnablePAD + 0x4, R_MIPS_LO16, jtbl_800EA8F8_main
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x34CB, 0x2
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x34CF, 0xE
endlabel EnablePAD
