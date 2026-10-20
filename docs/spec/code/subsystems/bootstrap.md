# Bootstrap: the loader, the start-up and the run loop

What runs from the moment the console loads the boot file to the end of the
game: the disc's loader, which unpacks the game's image and jumps into it;
Sony's start-up code at the image's entry; `main`; and `GameRun`, which
runs the game and shows what follows its end. How the boot file is packed
is in [../../code-decompilation.md](../../code-decompilation.md), and the
unpacker's format in
[../../formats/compression.md](../../formats/compression.md).

## The boot file's loader

`SYSTEM.CNF` boots `cdrom:\SLUS_006.28` (on disc 2, `SLUS_008.99`, the same
file under another name). Its header loads the 0x55000 bytes after it at
`0x80100000` and starts them at `0x801001EC`. The first 0x800 bytes are the
loader (940 of them used, `0x80100000` to `0x801003AB`, and 12 bytes of BSS
after them); the packed stream follows at `0x80100800`, file offset
`0x1000`.

The loader is in no decomp tree, so it has no names in the register. What
follows was read by disassembling the disc's boot file
([../../../../BUILD.md](../../../../BUILD.md) §"What is still assembly"
shows the command that disassembles a range of a file).

| Address | What it is |
|---|---|
| `0x80100000` | The unpacker: arguments the stream and the destination, returns the number of bytes written. Its 88 words are those of the image's own `lz_unpack` (`0x80022BF8`, `inflate.c`) one for one; the only four that differ are `j` instructions, whose absolute targets follow where the code lies. So the tree's C for `lz_unpack`, built at `0x80100000`, is the loader's unpacker too. |
| `0x80100160` | The loader's `main`: the run below. |
| `0x801001EC` | The entry, in the shape of Sony's start-up code: clears the BSS, sets the stack and `$gp`, calls `InitHeap` and then `main`, and ends in `break`. |
| `0x80100294` | Runs a table of constructors, once (a flag word at `0x8010037C` keeps it from running twice). `main` calls it first; the table is empty. |
| `0x80100304` | The same loop for the destructors. Nothing in the loader calls it. |
| `0x8010036C` | `InitHeap`, the kernel call A0:39. |
| `0x80100378`–`0x801003AB` | Data: the flag word, the heap's start and size the entry stores, the stack size (0x8000), the RAM size (`0x00800000`), and the inner header's address, `0x80010800`, where `$gp` points. |

The run:

```
entry (0x801001EC)
  zero 0x801003AC..0x801003B7                  the loader's BSS
  sp = (RAM size - 8) | 0x80000000              0x807FFFF8, not the header's 0x801FFFF0
  $gp = 0x801003A8
  InitHeap(from the end of the BSS to the stack's space, 0x8000 bytes below the top)
  main (0x80100160)
    constructors (0x80100294)                   none
    zero the 2 KB at 0x80010000; byte 0x80010004 = 1, byte 0x80010008 = 0
    unpack(0x80100800, 0x80010800)              inner header at 0x80010800,
                                                image at 0x80011000
    jump to the word at +0x10 of the inner header, its entry: 0x80013C4C
```

**The loader was linked for 8 MB.** Its RAM size word is `0x00800000`,
where the image's own start-up object, `2mbyte.obj`, carries `0x00200000`
(2 MB). The console's 2 MB repeat through the first 8 MB of the address
space, so the loader's stack at `0x807FFFF8` lies at the top of the same
RAM.

**The block at `0x80010000`.** The 2 KB the loader zeroes is the block the
game reads through `g_FileHostBlock` (`0x800B8928`), which `system_init`
(`0x8001EEB8`) sets to `0x80010000`. The C reads three of its bytes: `+0xA`,
above 0 when files are read from the developers' host PC (`file_load`
(`0x80021034`) then calls `pc_load` with `sim:\LUNA\MASTER\` and the name);
`+0xB`, which `system_init` and `func_800217AC` (`fmt.c`) compare with 5
(at 5, `func_800217AC` writes nothing to the host's console); and `+7`,
which `cd_dir_init` tests with `+0xA` (`FileHostBlock` in `include/cd.h`).
The loader leaves all three 0, so the game on the disc reads its files from
the CD. The byte it sets to 1, `+4`, is one the C neither reads nor writes.
The game's table of the disc's files, `g_CdDirRows`, is at `0x80010100`,
inside the same block.

## The image's start-up: `__SN_ENTRY_POINT` (`0x80013C4C`)

The inner header's entry is Sony's start-up object `2mbyte.obj` of PSY-Q
4.5, which lies whole at `0x80013C44` (`2mbyte.c`, a byte range: it was
assembly in the original, [../hand-assembly.md](../hand-assembly.md)). It
clears `0x800B8820` to `0x800EAFE8`, sets the stack and `$gp`
(`0x800B8540`), hands `InitHeap` (`0x8008B388`) the memory from the end of
that area to the stack's space below the top of RAM, whose size is the word
`_stacksize` (`0x800B8714`, still `D_800B8714`), and calls `main`
(`0x80013D2C`). The game's own allocator starts at the same place:
`xmalloc_init` takes `D_800EAFE8`, rounded up to 16, as
`g_XmallocHeapStart`. The object's other routine, `__main`, is the eight
bytes before the entry ([../../../naming.md](../../../naming.md) §"Open").

## `main` (`0x80013D2C`)

`main` (`main.c`) is an endless loop, and its first pass never ends,
because the last call in it does not return. In order: `g_GameEnded` = 0;
`system_init` (`frame.c`: Sony's libraries' start-up, the memory card, the
pad, the disc's directory, the allocator, the screen, the random generator,
the sound and the voices' XA file `_AUDIO.XA`); `main_buffer_init`, the
fixed buffers (resources are read to `0x80156000` and unpacked to
`0x80196000`);
`screen_mode_set(1)`; `vram_strip_clear`; `sprite_reset(0)`, which reads
resource 2; `resource_load_run(1)`, SYSTEM.DAT; `sound_common_load`, the
sound-effect bank (resource `0xE8`); `sound_bank_empty_fill(1)`;
`func_80025FE0(1)`; `font_string_table_set`; `func_80026AB0`; `GameStart`;
and `GameRun`. The compiler puts a call to `__main` at the top of any
function named `main`, and the image has it there.

## The run loop and the end of the game: `GameRun` (`0x800559F4`)

`GameRun` (`map_frame.c`) is the last call of `main`'s pass, and it never
returns.

1. The end flag `g_GameEnded` = 0, then one frame per pass,
   `MapFrameUpdate()` (`0x80055840`) (the frame's work),
   `sprite_list_draw()` (`0x8001A798`), `MapFrameEnd()` (`0x8005596C`) (the
   frame's tail), until the flag is not 0. The one store of a value other
   than 0 to the flag is in `ScriptCommandStoryBitsItemsReset`
   (`0x8005EF58`, `lns_intp.c`): event-script command 0x5C stores 1 there
   when story bit 1 is set ([npc-scripting.md](npc-scripting.md)
   §"Commands 0x23 and 0x5C").
2. `overlay_disc_wait(0x33)` (`0x80023184`): the disc check for movie 0x33,
   that is disc 2 ([../../video.md](../../video.md)). If the drive does not
   hold that disc, it loads resource `0x257` and shows one of its pictures 0
   to 3, the disc-change screens (`cd_change_wait` (`0x80024D2C`)), until
   the player has changed discs.
3. `resource_load_run(0x257)` (`0x800202CC`) loads resource 599, and
   `resource_tim_unpack(4)` (`0x80020918`) unpacks its kept picture 4 into
   the load buffer at `g_LoadBuffer`; `screen_picture_put()` (`0x80024C04`)
   puts that 320 x 224 picture on the screen at once
   ([io-hardware.md](io-hardware.md) §"The full-screen picture put"),
   then `screen_disp_mask_set(1)` (`0x8001EFEC`) and `SetDispMask(1)`.
4. `xa_file_set("_SECOND.XA")` (`0x8002347C`) looks up that file, which only
   disc 2 carries; then for channel i = 0 to 8, `xa_play_start(0, i)`
   (`0x800235C0`) starts the channel from the file's first sector, and
   `VSync(0)` is waited out that channel's count of times. The nine counts
   are the 16-bit table `g_GameSecondXaChannelFrames` (`0x8001280C`): 10972,
   9016, 9557, 8275, 9584, 9501, 8776, 8661, 4223, which is 78,565 frames,
   21.8 minutes at 60 Hz.
5. Resource 599 again, its picture 5, the same three calls, and `VSync(0)`
   for ever.

Not established: what pictures 4 and 5 of resource 599 show, and what the
nine channels hold.
