SECTIONS
{
    __romPos = 0;
    _gp = 0x800B8540;
    main_ROM_START = __romPos;
    main_VRAM = ADDR(.main);
    /* Hand-adjusted from splat output, as the boot EXE's was: the image is
       rodata / code / data (measured — see splat.yaml header), so
       head_rodata precedes .text and tail_data follows it. SUBALIGN is 4
       and there is no 16-byte ALIGN between the sections: the text starts
       at 0x8019610C and ends at 0x80197690, neither 16-aligned. The
       boot EXE's symbols the overlay calls and reads are PROVIDEd by
       label_fixups.txt (they are in symbol_addrs.txt, under its header). */
    .main 0x80196000 : AT(main_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        build/SLUS_006.28-MENU/asm/data/head_rodata.data.s.o(.data);
        main_TEXT_START = .;
        build/SLUS_006.28-MENU/src/movie.c.o(.text);
        build/SLUS_006.28-MENU/src/libpress_libpress.c.o(.text);
        build/SLUS_006.28-MENU/src/libpress_vlc.c.o(.text);
        main_TEXT_END = .;
        main_TEXT_SIZE = ABSOLUTE(main_TEXT_END - main_TEXT_START);
        main_DATA_START = .;
        build/SLUS_006.28-MENU/src/movie.c.o(.data);
        build/SLUS_006.28-MENU/src/libpress_libpress.c.o(.data);
        build/SLUS_006.28-MENU/src/libpress_vlc.c.o(.data);
        build/SLUS_006.28-MENU/asm/data/tail_data.data.s.o(.data);
        main_DATA_END = .;
        main_DATA_SIZE = ABSOLUTE(main_DATA_END - main_DATA_START);
        main_RODATA_START = .;
        build/SLUS_006.28-MENU/src/movie.c.o(.rodata);
        build/SLUS_006.28-MENU/src/libpress_libpress.c.o(.rodata);
        build/SLUS_006.28-MENU/src/libpress_vlc.c.o(.rodata);
        main_RODATA_END = .;
        main_RODATA_SIZE = ABSOLUTE(main_RODATA_END - main_RODATA_START);
        main_BSS_START = .;
        build/SLUS_006.28-MENU/src/movie.c.o(.bss);
        build/SLUS_006.28-MENU/src/libpress_libpress.c.o(.bss);
        build/SLUS_006.28-MENU/src/libpress_vlc.c.o(.bss);
        main_BSS_END = .;
        main_BSS_SIZE = ABSOLUTE(main_BSS_END - main_BSS_START);
    }
    __romPos += SIZEOF(.main);
    main_ROM_END = __romPos;
    main_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
