# PSY-Q library code: function index

This page lists every function of Sony's PSY-Q library code in the main
executable, `SLUS_006.28`, by address: its name, the library member it
belongs to, the file that holds it and the recipe markers that file gives it.
A second table lists the library's routines that are still assembly, with
what their code shows that a C compiler does not emit. The program links
release 4.5 of the libraries; [docs/naming.md](../../../../naming.md) says
how that was measured and how each name was proven. What the functions do is
in [psyq-libs.md](../psyq-libs.md) and the pages beside this one, and how
the library code is compiled is in [codegen.md](codegen.md).

The tables are read from today's tree: the files under
`decomp/SLUS_006.28/src/`, the register `config/symbol_addrs.txt` and
`config/asm_only.txt`.

- **Name.** Sony's name, where the register gives one. A function inside a
  member that gives it no public name keeps its address (`func_8008DC4C`)
  unless Sony's own text in the member names it (`sdk-text`) or another
  release exports its bytes under one name (`sdk-earlier`, `sdk-later`); the
  note says which. For an address-named function the note is the register's
  reading of what it does.
- **Member.** The library, the member as release 4.5 names it, and the
  function's offset in the member's text.
- **File.** Each member has its own file, `lib<library>_<member>.c`. A
  member is in several files (`libgpu_sys_1.c` to `libgpu_sys_6.c`) where
  the tree compiles one run of its functions with other flags than the runs
  beside it; `config/cc_flags.txt` and each file's header give those flags.
  One file does not follow the rule: `SpuWrite`, of member `s_w.o`, is in
  `libspu_s_r.c`, whose header names `s_r.o`, the twin member the register
  refutes.
- **Recipe.** The per-function markers in the file. `cc97_lib` is the
  library compiler and `cc272_lib` GCC 2.7.2's library recipe; a function
  with neither is built by the game's compiler, GCC 2.8.1. What each marker
  does is in [codegen.md](codegen.md) and
  [BUILD.md](../../../../../BUILD.md).

529 functions are C, in 273 files; 64 routines are assembly, in 52 files.
The files also define small `static inline` helpers that the compiler
expands where they are called; they are not functions of the image and are
not listed. Sony's start-up object, `2mbyte.obj` (`2mbyte.c`), is not a
library member and is not listed either; [docs/naming.md](../../../../naming.md)
has it.

## C

| Address | Name | Member | File | Recipe | Note |
|---|---|---|---|---|---|
| 0x8008B438 | `InitHeap2` | libapi `i_heap2.o` +0x0 | `libapi_i_heap2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8008B488 | `malloc2` | libapi `malloc2.o` +0x0 | `libapi_malloc2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8008B640 | `_ExpAndAlloc` | libapi `malloc2.o` +0x1B8 | `libapi_malloc2.c` | `cc97_lib` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x8008B6A8 | `free2` | libapi `free2.o` +0x0 | `libapi_free2.c` | `cc97_lib` |  |
| 0x8008B6E8 | `bcopy` | libc2 `bcopy.o` +0x0 | `libc2_bcopy.c` | `cc97_lib` |  |
| 0x8008B728 | `bzero` | libc2 `bzero.o` +0x0 | `libc2_bzero.c` | `cc97_lib` |  |
| 0x8008B758 | `memset` | libc2 `memset.o` +0x0 | `libc2_memset.c` | `cc97_lib` |  |
| 0x8008B788 | `strcmp` | libc2 `strcmp.o` +0x0 | `libc2_strcmp.c` | `cc97_lib` |  |
| 0x8008B7F8 | `strcpy` | libc2 `strcpy.o` +0x0 | `libc2_strcpy.c` | `cc97_lib` |  |
| 0x8008B848 | `strlen` | libc2 `strlen.o` +0x0 | `libc2_strlen.c` | `cc97_lib` |  |
| 0x8008B888 | `ResetGraph` | libgpu `sys.o` +0x0 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` |  |
| 0x8008B9FC | `SetGraphDebug` | libgpu `sys.o` +0x174 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` |  |
| 0x8008BA58 | `SetGraphQueue` | libgpu `sys.o` +0x1D0 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008BAFC | `GetGraphDebug` | libgpu `sys.o` +0x274 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8008BB0C | `DrawSyncCallback` | libgpu `sys.o` +0x284 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008BB6C | `SetDispMask` | libgpu `sys.o` +0x2E4 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008BC04 | `DrawSync` | libgpu `sys.o` +0x37C | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008BC6C | `func_8008BC6C` | libgpu `sys.o` +0x3E4 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` | The image functions' debug check of a rectangle, printing it when it is out of the frame buffer (or always, at level 2). |
| 0x8008BD88 | `ClearImage` | libgpu `sys.o` +0x500 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008BE18 | `ClearImage2` | libgpu `sys.o` +0x590 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008BEB0 | `LoadImage` | libgpu `sys.o` +0x628 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008BF10 | `StoreImage` | libgpu `sys.o` +0x688 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008BF70 | `MoveImage` | libgpu `sys.o` +0x6E8 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C028 | `ClearOTag` | libgpu `sys.o` +0x7A0 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C0F0 | `ClearOTagR` | libgpu `sys.o` +0x868 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C19C | `DrawPrim` | libgpu `sys.o` +0x914 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C1F8 | `DrawOTag` | libgpu `sys.o` +0x970 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C268 | `PutDrawEnv` | libgpu `sys.o` +0x9E0 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C328 | `DrawOTagEnv` | libgpu `sys.o` +0xAA0 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C400 | `GetDrawEnv` | libgpu `sys.o` +0xB78 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C434 | `PutDispEnv` | libgpu `sys.o` +0xBAC | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C92C | `GetDispEnv` | libgpu `sys.o` +0x10A4 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008C960 | `GetODE` | libgpu `sys.o` +0x10D8 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8008C990 | `SetDrawArea` | libgpu `sys.o` +0x1108 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008CA10 | `SetDrawOffset` | libgpu `sys.o` +0x1188 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008CA50 | `SetDrawEnv` | libgpu `sys.o` +0x11C8 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008CC48 | `func_8008CC48` | libgpu `sys.o` +0x13C0 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | `SetDrawEnv` as `PutDrawEnv` and `DrawOTagEnv` use it: the drawing-environment words, with the clear done by a fill or a rectangle. |
| 0x8008CEB8 | `func_8008CEB8` | libgpu `sys.o` +0x1630 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` | Builds the draw-mode setting word. |
| 0x8008CED8 | `func_8008CED8` | libgpu `sys.o` +0x1650 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Builds the drawing area's top-left setting word, clamped to the frame buffer. |
| 0x8008CF70 | `func_8008CF70` | libgpu `sys.o` +0x16E8 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Builds the drawing area's bottom-right setting word, clamped to the frame buffer. |
| 0x8008D008 | `func_8008D008` | libgpu `sys.o` +0x1780 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Builds the drawing-offset setting word. |
| 0x8008D024 | `func_8008D024` | libgpu `sys.o` +0x179C | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Builds the texture-window setting word from a rectangle. |
| 0x8008D0A4 | `func_8008D0A4` | libgpu `sys.o` +0x181C | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` | Returns the GPU status word. |
| 0x8008D0BC | `func_8008D0BC` | libgpu `sys.o` +0x1834 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Builds a reverse ordering table by DMA and waits for it, with the time-out. |
| 0x8008D19C | `func_8008D19C` | libgpu `sys.o` +0x1914 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` | Fills a frame-buffer rectangle with a colour, by a fill command or a rectangle primitive. |
| 0x8008D3CC | `func_8008D3CC` | libgpu `sys.o` +0x1B44 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Writes pixel data into a frame-buffer rectangle, partly by hand and the rest by DMA. |
| 0x8008D608 | `func_8008D608` | libgpu `sys.o` +0x1D80 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Reads a frame-buffer rectangle into memory, partly by hand and the rest by DMA. |
| 0x8008D888 | `func_8008D888` | libgpu `sys.o` +0x2000 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Writes a GPU control word and keeps its low byte in a shadow table. |
| 0x8008D8AC | `func_8008D8AC` | libgpu `sys.o` +0x2024 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` | Reads back the shadow of the last control word with a given command byte. |
| 0x8008D8C0 | `func_8008D8C0` | libgpu `sys.o` +0x2038 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` | Writes a number of command words to the GPU by hand. |
| 0x8008D900 | `func_8008D900` | libgpu `sys.o` +0x2078 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Sends a primitive list to the GPU by linked-list DMA and returns at once. |
| 0x8008D948 | `_param` | libgpu `sys.o` +0x20C0 | `libgpu_sys_1.c` | `cc97_lib` + `gnu_as_loads` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x8008D978 | `_addque` | libgpu `sys.o` +0x20F0 | `libgpu_sys_2.c` | `cc97_lib` + `gnu_as_loads` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x8008D99C | `func_8008D99C` | libgpu `sys.o` +0x2114 | `libgpu_sys_3.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Runs a GPU routine now, or queues it with a copy of its data; returns the queue's length. |
| 0x8008DC4C | `func_8008DC4C` | libgpu `sys.o` +0x23C4 | `libgpu_sys_3.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Runs queued GPU routines until the queue is empty or one starts a DMA transfer; fires the `DrawSync` callback when done. |
| 0x8008DEAC | `func_8008DEAC` | libgpu `sys.o` +0x2624 | `libgpu_sys_3.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Empties the queue and resets the GPU and its DMA by mode; returns the GPU type on a full reset. |
| 0x8008DFFC | `func_8008DFFC` | libgpu `sys.o` +0x2774 | `libgpu_sys_3.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Waits for the queue and the GPU to finish, or reports how many entries are left. |
| 0x8008E138 | `set_alarm` | libgpu `sys.o` +0x28B0 | `libgpu_sys_4.c` | `cc97_lib` + `gnu_as_loads` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x8008E16C | `func_8008E16C` | libgpu `sys.o` +0x28E4 | `libgpu_sys_4.c` | `cc97_lib` + `gnu_as_loads` | Tests the GPU time-out; when it has passed, reports the GPU state, resets the queue and returns -1. |
| 0x8008E2B0 | `func_8008E2B0` | libgpu `sys.o` +0x2A28 | `libgpu_sys_4.c` | `cc97_lib` + `gnu_as_loads` | Asks the GPU for its version and returns the GPU type. |
| 0x8008E350 | `LoadImage2` | libgpu `sys.o` +0x2AC8 | `libgpu_sys_5.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008E43C | `StoreImage2` | libgpu `sys.o` +0x2BB4 | `libgpu_sys_5.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008E528 | `MoveImage2` | libgpu `sys.o` +0x2CA0 | `libgpu_sys_5.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008E66C | `DrawOTag2` | libgpu `sys.o` +0x2DE4 | `libgpu_sys_5.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008E768 | `_GPU_ResetCallback` | libgpu `sys.o` +0x2EE0 | `libgpu_sys_6.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8008E790 | `func_8008E790` | libgpu `sys.o` +0x2F08 | `libgpu_sys_6.c` | `cc97_lib` + `gnu_as_loads` | `sys.o`'s own byte fill (no return value). |
| 0x8008E7C8 | `memcpy` | libc2 `memcpy.o` +0x0 | `libc2_memcpy.c` | `cc97_lib` |  |
| 0x8008E808 | `LoadClut` | libgpu `e01.o` +0x0 | `libgpu_e01.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8008E878 | `SetDefDrawEnv` | libgpu `e03.o` +0x0 | `libgpu_e03.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8008E938 | `SetDefDispEnv` | libgpu `e04.o` +0x0 | `libgpu_e04.c` | `cc97_lib` |  |
| 0x8008E978 | `GetTPage` | libgpu `p00.o` +0x0 | `libgpu_p00.c` | `cc97_lib` |  |
| 0x8008E9B8 | `GetClut` | libgpu `p01.o` +0x0 | `libgpu_p01.c` | `cc97_lib` |  |
| 0x8008E9D8 | `AddPrim` | libgpu `p06.o` +0x0 | `libgpu_p06.c` | `cc97_lib` |  |
| 0x8008EA18 | `SetSemiTrans` | libgpu `p10.o` +0x0 | `libgpu_p10.c` | `cc97_lib` |  |
| 0x8008EA48 | `SetPolyF3` | libgpu `p12.o` +0x0 | `libgpu_p12.c` | `cc97_lib` |  |
| 0x8008EA68 | `SetPolyF4` | libgpu `p16.o` +0x0 | `libgpu_p16.c` | `cc97_lib` |  |
| 0x8008EA88 | `SetTile` | libgpu `p26.o` +0x0 | `libgpu_p26.c` | `cc97_lib` |  |
| 0x8008EAA8 | `SetLineF3` | libgpu `p29.o` +0x0 | `libgpu_p29.c` | `cc97_lib` |  |
| 0x8008EAC8 | `SetDrawTPage` | libgpu `p33.o` +0x0 | `libgpu_p33.c` | `cc97_lib` |  |
| 0x8008EAF8 | `GsInitGraph` | libgs `gs_001.o` +0x0 | `libgs_gs_001_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008EB6C | `func_8008EB6C` | libgs `gs_001.o` +0x74 | `libgs_gs_001_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | `GsInitGraph`'s GPU half: resets the GPU and sends the first drawing and display environments. |
| 0x8008EC88 | `GsInitGraph2` | libgs `gs_001.o` +0x190 | `libgs_gs_001_2.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8008ECF0 | `func_8008ECF0` | libgs `gs_001.o` +0x1F8 | `libgs_gs_001_3.c` | `cc97_lib` + `gnu_as_loads` | Sets the libgs variables a screen size decides (unit matrices, clip, screen-clear primitives). |
| 0x8008EF10 | `GsSortClear` | libgs `gs_001.o` +0x418 | `libgs_gs_001_3.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8008F048 | `GsGetActiveBuff` | libgs `gs_002.o` +0x0 | `libgs_gs_002.c` | `cc97_lib` |  |
| 0x8008F058 | `GsSetDrawBuffOffset` | libgs `gs_002.o` +0x10 | `libgs_gs_002.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008F168 | `GsSetDrawBuffClip` | libgs `gs_003.o` +0x0 | `libgs_gs_003.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8008F1E8 | `GsSetClip2D` | libgs `gs_006.o` +0x0 | `libgs_gs_006.c` | `cc97_lib` + `lib_g0` |  |
| 0x8008F228 | `GsInitVcount` | libgs `gs_007.o` +0x0 | `libgs_gs_007.c` | `cc97_lib` |  |
| 0x8008F268 | `SetRCnt` | libapi `counter.o` +0x0 | `libapi_counter_1.c` | `cc97_lib` |  |
| 0x8008F304 | `GetRCnt` | libapi `counter.o` +0x9C | `libapi_counter_1.c` | `cc97_lib` |  |
| 0x8008F33C | `StartRCnt` | libapi `counter.o` +0xD4 | `libapi_counter_2.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008F36C | `StopRCnt` | libapi `counter.o` +0x104 | `libapi_counter_2.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8008F3A0 | `ResetRCnt` | libapi `counter.o` +0x138 | `libapi_counter_2.c` | `cc97_lib` |  |
| 0x8008F3D8 | `GsGetVcount` | libgs `gs_008.o` +0x0 | `libgs_gs_008.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8008F428 | `GsSwapDispBuff` | libgs `gs_010.o` +0x0 | `libgs_gs_010.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8008F4D8 | `GsDefDispBuff` | libgs `gs_103.o` +0x0 | `libgs_gs_103.c` | `cc97_lib` |  |
| 0x8008F578 | `GsInit3D` | libgs `gs_104.o` +0x0 | `libgs_gs_104.c` | `cc97_lib` |  |
| 0x8008F5F8 | `GsSetFlatLight` | libgs `gs_107.o` +0x0 | `libgs_gs_107_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8008FAB0 | `func_8008FAB0` | libgs `gs_107.o` +0x4B8 | `libgs_gs_107_2.c` | `cc97_lib` | Sets the object's local colour matrix and loads it into the GTE. |
| 0x8008FB14 | `func_8008FB14` | libgs `gs_107.o` +0x51C | `libgs_gs_107_3.c` | `cc97_lib` + `return_slot_fill` | Copies the object's local colour matrix out. |
| 0x8008FB68 | `GsDrawOt` | libgs `gs_111.o` +0x0 | `libgs_gs_111.c` | `cc97_lib` |  |
| 0x8008FB98 | `GsClearOt` | libgs `gs_113.o` +0x0 | `libgs_gs_113.c` | `cc97_lib` |  |
| 0x8008FBF8 | `gte_init` | libgs `gs_121.o` +0x0 | `libgs_gs_121.c` | `cc97_lib` |  |
| 0x8008FC48 | `GsGetTimInfo` | libgs `gs_122.o` +0x0 | `libgs_gs_122.c` | `cc97_lib` |  |
| 0x8008FD38 | `Gssub_make_matrix` | libgs `gs_123.o` +0x0 | `libgs_gs_123.c` | `cc97_lib` |  |
| 0x80090078 | `StSetRing` | libcd `cdrom.o` +0x0 | `libcd_cdrom.c` | none (GCC 2.8.1) |  |
| 0x800900A8 | `CdGetToc` | libcd `toc.o` +0x0 | `libcd_toc_1.c` | `cc97_lib` |  |
| 0x800900CC | `CdGetToc2` | libcd `toc.o` +0x24 | `libcd_toc_2.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800902F8 | `CdInit` | libcd `event.o` +0x0 | `libcd_event_1.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x80090388 | `func_80090388` | libcd `event.o` +0x90 | `libcd_event_2.c` | `cc97_lib` | The reset `CdInit` retries: `CD_init`, then `CD_initvol`; 1 when both succeed. |
| 0x800903C4 | `def_cbsync` | libcd `event.o` +0xCC | `libcd_event_2.c` | `cc97_lib` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x800903EC | `func_800903EC` | libcd `event.o` +0xF4 | `libcd_event_2.c` | `cc97_lib` | `CdInit`'s default ready callback: delivers the CD-ROM "data ready" event. |
| 0x80090414 | `func_80090414` | libcd `event.o` +0x11C | `libcd_event_2.c` | `cc97_lib` | `CdInit`'s default read callback: delivers the same "data ready" event. |
| 0x80090458 | `CdStatus` | libcd `sys.o` +0x0 | `libcd_sys_1.c` | `cc97_lib` |  |
| 0x80090468 | `CdMode` | libcd `sys.o` +0x10 | `libcd_sys_1.c` | `cc97_lib` |  |
| 0x80090478 | `CdLastCom` | libcd `sys.o` +0x20 | `libcd_sys_1.c` | `cc97_lib` |  |
| 0x80090488 | `CdLastPos` | libcd `sys.o` +0x30 | `libcd_sys_2.c` | none (GCC 2.8.1) |  |
| 0x80090494 | `CdReset` | libcd `sys.o` +0x3C | `libcd_sys_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80090500 | `CdFlush` | libcd `sys.o` +0xA8 | `libcd_sys_3.c` | `cc97_lib` |  |
| 0x80090520 | `CdSetDebug` | libcd `sys.o` +0xC8 | `libcd_sys_4.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80090534 | `CdComstr` | libcd `sys.o` +0xDC | `libcd_sys_4.c` | `cc97_lib` |  |
| 0x80090568 | `CdIntstr` | libcd `sys.o` +0x110 | `libcd_sys_4.c` | `cc97_lib` |  |
| 0x8009059C | `CdSync` | libcd `sys.o` +0x144 | `libcd_sys_5.c` | `cc97_lib` |  |
| 0x800905BC | `CdReady` | libcd `sys.o` +0x164 | `libcd_sys_5.c` | `cc97_lib` |  |
| 0x800905DC | `CdSyncCallback` | libcd `sys.o` +0x184 | `libcd_sys_6.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800905F0 | `CdReadyCallback` | libcd `sys.o` +0x198 | `libcd_sys_6.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80090604 | `CdControl` | libcd `sys.o` +0x1AC | `libcd_sys_6.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80090740 | `CdControlF` | libcd `sys.o` +0x2E8 | `libcd_sys_6.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80090874 | `CdControlB` | libcd `sys.o` +0x41C | `libcd_sys_6.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800909C0 | `CdMix` | libcd `sys.o` +0x568 | `libcd_sys_6.c` | `cc97_lib` |  |
| 0x800909E0 | `CdGetSector` | libcd `sys.o` +0x588 | `libcd_sys_6.c` | `cc97_lib` |  |
| 0x80090A00 | `CdGetSector2` | libcd `sys.o` +0x5A8 | `libcd_sys_6.c` | `cc97_lib` |  |
| 0x80090A20 | `CdDataCallback` | libcd `sys.o` +0x5C8 | `libcd_sys_7.c` | `cc97_lib` |  |
| 0x80090A44 | `CdDataSync` | libcd `sys.o` +0x5EC | `libcd_sys_7.c` | `cc97_lib` |  |
| 0x80090A68 | `func_80090A68` | libcd `bios.o` +0x0 | `libcd_bios_1.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` | The CD-ROM interrupt service: reads the cause and result bytes, sets the sync, ready or end state, copies the result and returns the case. |
| 0x80090FCC | `CD_sync` | libcd `bios.o` +0x564 | `libcd_bios_1.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x8009124C | `CD_ready` | libcd `bios.o` +0x7E4 | `libcd_bios_1.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x80091514 | `CD_cw` | libcd `bios.o` +0xAAC | `libcd_bios_1.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` |  |
| 0x80091920 | `CD_vol` | libcd `bios.o` +0xEB8 | `libcd_bios_1.c` | `cc97_lib` |  |
| 0x800919A8 | `CD_flush` | libcd `bios.o` +0xF40 | `libcd_bios_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80091A7C | `CD_initvol` | libcd `bios.o` +0x1014 | `libcd_bios_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80091B6C | `CD_initintr` | libcd `bios.o` +0x1104 | `libcd_bios_2.c` | `cc97_lib` |  |
| 0x80091BB8 | `CD_init` | libcd `bios.o` +0x1150 | `libcd_bios_2.c` | `cc97_lib` + `reorder_fill` |  |
| 0x80091D98 | `CD_datasync` | libcd `bios.o` +0x1330 | `libcd_bios_3.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x80091F00 | `CD_getsector` | libcd `bios.o` +0x1498 | `libcd_bios_3.c` | `cc97_lib` |  |
| 0x80092000 | `CD_getsector2` | libcd `bios.o` +0x1598 | `libcd_bios_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800920EC | `CD_set_test_parmnum` | libcd `bios.o` +0x1684 | `libcd_bios_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800920F8 | `func_800920F8` | libcd `bios.o` +0x1690 | `libcd_bios_3.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | The CD-ROM interrupt handler `CD_init` and `CD_initintr` install: runs `func_80090A68` until it returns 0 and calls the user's ready and sync callbacks. |
| 0x800921D8 | `puts` | libc2 `puts.o` +0x0 | `libc2_puts.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x80092238 | `_putchar` | libc2 `putchar.o` +0x0 | `libc2_putchar.c` | `cc97_lib` + `lib_g0` + `return_slot_fill` |  |
| 0x80092334 | `_putchar_flash` | libc2 `putchar.o` +0xFC | `libc2_putchar.c` | `cc97_lib` |  |
| 0x80092370 | `putchar` | libc2 `putchar.o` +0x138 | `libc2_putchar.c` | `cc97_lib` + `return_slot_fill` + `inline_functions` |  |
| 0x80092498 | `toupper` | libc2 `ctype.o` +0x0 | `libc2_ctype.c` | `cc97_lib` |  |
| 0x800924CC | `tolower` | libc2 `ctype.o` +0x34 | `libc2_ctype.c` | `cc97_lib` |  |
| 0x80092508 | `CdSearchFile` | libcd `iso9660.o` +0x0 | `libcd_iso9660.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` |  |
| 0x800927E0 | `_cmp` | libcd `iso9660.o` +0x2D8 | `libcd_iso9660.c` | none (GCC 2.8.1) | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x80092800 | `CD_newmedia` | libcd `iso9660.o` +0x2F8 | `libcd_iso9660.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` | No public name in 4.5; Sony's text in the member names it (`sdk-text`). |
| 0x80092AC4 | `func_80092AC4` | libcd `iso9660.o` +0x5BC | `libcd_iso9660.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` | Finds a directory by parent number and name in the directory cache; its number, or -1. |
| 0x80092B68 | `CD_cachefile` | libcd `iso9660.o` +0x660 | `libcd_iso9660.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` | No public name in 4.5; Sony's text in the member names it (`sdk-text`). |
| 0x80092E04 | `cd_read` | libcd `iso9660.o` +0x8FC | `libcd_iso9660.c` | `cc97_lib` + `return_slot_fill` | No public name in 4.5; Sony's text in the member names it (`sdk-text`). |
| 0x80092E78 | `strncmp` | libc2 `strncmp.o` +0x0 | `libc2_strncmp.c` | `cc97_lib` |  |
| 0x80092EF8 | `func_80092EF8` | libcd `cdread.o` +0x0 | `libcd_cdread_1.c` | `cc97_lib` + `return_slot_fill` | The sync callback installed with a pause: puts the caller's sync callback back and clears the pause-pending flag. |
| 0x80092F2C | `func_80092F2C` | libcd `cdread.o` +0x34 | `libcd_cdread_1.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | `CdRead`'s ready callback: takes each sector in, checks its position, retries or times out, and on the end calls the user's callback. |
| 0x800931A0 | `func_800931A0` | libcd `cdread.o` +0x2A8 | `libcd_cdread_2.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | `CdRead`'s data callback in DMA mode: advances the write pointer and counts the sector; after the last one restores the callbacks, sends a pause and calls the user's callback. |
| 0x80093270 | `func_80093270` | libcd `cdread.o` +0x378 | `libcd_cdread_2.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | Starts or restarts `CdRead`'s transfer: drops the callbacks, gives up while the shell is open, else sets mode and position again and issues the read with `func_80092F2C` installed. |
| 0x80093458 | `CdReadBreak` | libcd `cdread.o` +0x560 | `libcd_cdread_2.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800935F8 | `CdRead` | libcd `cdread.o` +0x700 | `libcd_cdread_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80093794 | `CdReadSync` | libcd `cdread.o` +0x89C | `libcd_cdread_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80093898 | `CdReadCallback` | libcd `cdread.o` +0x9A0 | `libcd_cdread_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800938AC | `CdReadMode` | libcd `cdread.o` +0x9B4 | `libcd_cdread_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800938C8 | `CdRead2` | libcd `cdread2.o` +0x0 | `libcd_cdread2_1.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x8009394C | `func_8009394C` | libcd `cdread2.o` +0x84 | `libcd_cdread2_2.c` | `cc97_lib` | A bare call of `StCdInterrupt`; `CdRead2` installs it as the ready callback in stream mode. |
| 0x80093978 | `StClearRing` | libcd `c_002.o` +0x0 | `libcd_c_002.c` | `cc97_lib` |  |
| 0x800939D8 | `data_ready_callback` | libcd `c_004.o` +0x0 | `libcd_c_004_1.c` | `cc97_lib` |  |
| 0x80093A64 | `StGetBackloc` | libcd `c_004.o` +0x8C | `libcd_c_004_2.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x80093AC8 | `StSetStream` | libcd `c_005.o` +0x0 | `libcd_c_005.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80093B58 | `StFreeRing` | libcd `c_007.o` +0x0 | `libcd_c_007.c` | `cc97_lib` |  |
| 0x80093C08 | `init_ring_status` | libcd `c_008.o` +0x0 | `libcd_c_008.c` | `cc97_lib` |  |
| 0x80093C48 | `StGetNext` | libcd `c_009.o` +0x0 | `libcd_c_009.c` | `cc97_lib` |  |
| 0x80093D08 | `StSetMask` | libcd `c_010.o` +0x0 | `libcd_c_010.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80093D28 | `StCdInterrupt` | libcd `c_011.o` +0x0 | `libcd_c_011_1.c` | `cc97_lib` |  |
| 0x80094644 | `func_80094644` | libcd `c_011.o` +0x91C | `libcd_c_011_1.c` | `cc97_lib` | Copies a number of words from one address to another (`StCdInterrupt`'s stand-in for the DMA when a stream is emulated). |
| 0x80094670 | `func_80094670` | libcd `c_011.o` +0x948 | `libcd_c_011_2.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` | Starts one DMA transfer on a channel (`StCdInterrupt` uses the CD-ROM channel), waiting for the channel to be free first. |
| 0x80094818 | `CdIntToPos` | libcd `s_002.o` +0x0 | `libcd_s_002.c` | `cc97_lib` |  |
| 0x8009491C | `CdPosToInt` | libcd `s_002.o` +0x104 | `libcd_s_002.c` | `cc97_lib` |  |
| 0x800949A8 | `VSync` | libetc `vsync.o` +0x0 | `libetc_vsync_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80094B20 | `func_80094B20` | libetc `vsync.o` +0x178 | `libetc_vsync_2.c` | `cc97_lib` + `reorder_fill` | Waits until the frame count reaches a value, with a time-out that reports and resets the pad driver's handling. |
| 0x80094BD8 | `ResetCallback` | libetc `intr.o` +0x0 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094C08 | `InterruptCallback` | libetc `intr.o` +0x30 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094C38 | `DMACallback` | libetc `intr.o` +0x60 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094C68 | `VSyncCallback` | libetc `intr.o` +0x90 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094C9C | `VSyncCallbacks` | libetc `intr.o` +0xC4 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094CCC | `StopCallback` | libetc `intr.o` +0xF4 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094CFC | `RestartCallback` | libetc `intr.o` +0x124 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094D2C | `CheckCallback` | libetc `intr.o` +0x154 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094D3C | `GetIntrMask` | libetc `intr.o` +0x164 | `libetc_intr_1.c` | `cc97_lib` |  |
| 0x80094D54 | `SetIntrMask` | libetc `intr.o` +0x17C | `libetc_intr_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80094D6C | `func_80094D6C` | libetc `intr.o` +0x194 | `libetc_intr_2.c` | `cc97_lib` + `return_slot_fill` | Hooks the interrupt entry: clears the interrupt and DMA state, saves a context the dispatcher returns into, installs the VSync and DMA handlers. |
| 0x80094E44 | `func_80094E44` | libetc `intr.o` +0x26C | `libetc_intr_2.c` | `cc97_lib` + `lib_g0` + `reorder_fill` + `return_slot_fill` | The interrupt dispatcher: acknowledges each pending interrupt and calls its handler; reports a stuck interrupt. |
| 0x80095014 | `func_80095014` | libetc `intr.o` +0x43C | `libetc_intr_2.c` | `cc97_lib` + `return_slot_fill` | Installs or removes the handler of one interrupt and returns the previous one. |
| 0x8009515C | `func_8009515C` | libetc `intr.o` +0x584 | `libetc_intr_2.c` | `cc97_lib` + `return_slot_fill` | Unhooks the interrupt entry, saving the interrupt mask and DMA control for `func_800951FC`. |
| 0x800951FC | `func_800951FC` | libetc `intr.o` +0x624 | `libetc_intr_2.c` | `cc97_lib` + `return_slot_fill` | Hooks the interrupt entry again after `func_8009515C` and restores the saved mask and DMA control. |
| 0x80095274 | `func_80095274` | libetc `intr.o` +0x69C | `libetc_intr_2.c` | `cc97_lib` | Zeroes a number of words (`func_80094D6C` clears its environment with it). |
| 0x80095368 | `startIntrVSync` | libetc `intr_vb.o` +0x0 | `libetc_intr_vb_1.c` | `cc97_lib` |  |
| 0x800953C0 | `func_800953C0` | libetc `intr_vb.o` +0x58 | `libetc_intr_vb_2.c` | `cc97_lib` + `return_slot_fill` | The vertical-blank handler: counts the frame and calls each installed callback. |
| 0x8009542C | `setIntrVSync` | libetc `intr_vb.o` +0xC4 | `libetc_intr_vb_2.c` | `cc97_lib` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x80095458 | `func_80095458` | libetc `intr_vb.o` +0xF0 | `libetc_intr_vb_2.c` | `cc97_lib` | Zeroes a number of words (`startIntrVSync` clears the callback slots with it). |
| 0x80095488 | `startIntrDMA` | libetc `intr_dma.o` +0x0 | `libetc_intr_dma_1.c` | `cc97_lib` |  |
| 0x800954D4 | `func_800954D4` | libetc `intr_dma.o` +0x4C | `libetc_intr_dma_2.c` | `cc97_lib` + `return_slot_fill` | The DMA interrupt handler: acknowledges each finished channel and calls its callback; reports a bus error. |
| 0x80095654 | `func_80095654` | libetc `intr_dma.o` +0x1CC | `libetc_intr_dma_2.c` | `cc97_lib` | Installs one DMA channel's completion callback and returns the one it replaces. |
| 0x80095700 | `func_80095700` | libetc `intr_dma.o` +0x278 | `libetc_intr_dma_2.c` | `cc97_lib` | Zeroes a number of words (`startIntrDMA` clears the callback slots with it). |
| 0x80095728 | `SetVideoMode` | libetc `vmode.o` +0x0 | `libetc_vmode.c` | none (GCC 2.8.1) + `return_slot_fill` |  |
| 0x8009573C | `GetVideoMode` | libetc `vmode.o` +0x14 | `libetc_vmode.c` | none (GCC 2.8.1) |  |
| 0x80095758 | `func_80095758` | libsnd `ssclose.o` +0x0 | `libsnd_ssclose_1.c` | `cc97_lib` + `return_slot_fill` | The worker of `SsSeqClose` and `SsSepClose`: silences an access number, frees it and resets its records. |
| 0x800958D4 | `SsSeqClose` | libsnd `ssclose.o` +0x17C | `libsnd_ssclose_2.c` | `cc97_lib` |  |
| 0x800958F8 | `SsSepClose` | libsnd `ssclose.o` +0x1A0 | `libsnd_ssclose_2.c` | `cc97_lib` |  |
| 0x80095928 | `SsEnd` | libsnd `ssend.o` +0x0 | `libsnd_ssend.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800959D8 | `_SsInit` | libsnd `ssinit.o` +0x0 | `libsnd_ssinit.c` | `cc97_lib` |  |
| 0x80095A78 | `SsInit` | libsnd `ssinit_c.o` +0x0 | `libsnd_ssinit_c.c` | `cc97_lib` |  |
| 0x80095AB8 | `SsSetLoop` | libsnd `ssloop.o` +0x0 | `libsnd_ssloop.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x80095AF8 | `SsIsEos` | libsnd `ssloop.o` +0x40 | `libsnd_ssloop.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x80095B38 | `SsSeqOpen` | libsnd `ssopenq.o` +0x0 | `libsnd_ssopenq.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x80095DC8 | `_SsContBankChange` | libsnd `cc_0.o` +0x0 | `libsnd_cc_0.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80095E48 | `_SsContDataEntry` | libsnd `cc_6.o` +0x0 | `libsnd_cc_6.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x800961C8 | `_SsContMainVol` | libsnd `cc_7.o` +0x0 | `libsnd_cc_7.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80096298 | `_SsContPanpot` | libsnd `cc_10.o` +0x0 | `libsnd_cc_10.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80096368 | `_SsContExpression` | libsnd `cc_11.o` +0x0 | `libsnd_cc_11.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80096458 | `_SsContDamper` | libsnd `cc_64.o` +0x0 | `libsnd_cc_64.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80096508 | `_SsContExternal` | libsnd `cc_91.o` +0x0 | `libsnd_cc_91.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80096598 | `_SsContNrpn1` | libsnd `cc_98.o` +0x0 | `libsnd_cc_98.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096698 | `_SsContNrpn2` | libsnd `cc_99.o` +0x0 | `libsnd_cc_99.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800967D8 | `_SsContRpn1` | libsnd `cc_100.o` +0x0 | `libsnd_cc_100.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80096848 | `_SsContRpn2` | libsnd `cc_101.o` +0x0 | `libsnd_cc_101.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x800968B8 | `_SsContResetAll` | libsnd `cc_121.o` +0x0 | `libsnd_cc_121.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80096978 | `_SsSetNrpnVabAttr0` | libsnd `de_0.o` +0x0 | `libsnd_de_0.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096A08 | `_SsSetNrpnVabAttr1` | libsnd `de_1.o` +0x0 | `libsnd_de_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096AC8 | `_SsSetNrpnVabAttr2` | libsnd `de_2.o` +0x0 | `libsnd_de_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096B58 | `_SsSetNrpnVabAttr3` | libsnd `de_3.o` +0x0 | `libsnd_de_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096BE8 | `_SsSetNrpnVabAttr4` | libsnd `de_4.o` +0x0 | `libsnd_de_4.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096C98 | `_SsUtResolveADSR` | libsnd `ccadsr.o` +0x0 | `libsnd_ccadsr.c` | `cc97_lib` |  |
| 0x80096CF4 | `_SsUtBuildADSR` | libsnd `ccadsr.o` +0x5C | `libsnd_ccadsr.c` | `cc97_lib` |  |
| 0x80096D98 | `_SsSetNrpnVabAttr5` | libsnd `de_5.o` +0x0 | `libsnd_de_5.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096E58 | `_SsSetNrpnVabAttr6` | libsnd `de_6.o` +0x0 | `libsnd_de_6.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096F08 | `_SsSetNrpnVabAttr7` | libsnd `de_7.o` +0x0 | `libsnd_de_7.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80096FB8 | `_SsSetNrpnVabAttr8` | libsnd `de_8.o` +0x0 | `libsnd_de_8.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80097068 | `_SsSetNrpnVabAttr9` | libsnd `de_9.o` +0x0 | `libsnd_de_9.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80097128 | `_SsSetNrpnVabAttr10` | libsnd `de_10.o` +0x0 | `libsnd_de_10.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800971D8 | `_SsSetNrpnVabAttr11` | libsnd `de_11.o` +0x0 | `libsnd_de_11.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80097288 | `_SsSetNrpnVabAttr12` | libsnd `de_12.o` +0x0 | `libsnd_de_12.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80097368 | `_SsSetNrpnVabAttr13` | libsnd `de_13.o` +0x0 | `libsnd_de_13.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80097408 | `_SsSetNrpnVabAttr14` | libsnd `de_14.o` +0x0 | `libsnd_de_14.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800974A8 | `_SsSetNrpnVabAttr15` | libsnd `de_15.o` +0x0 | `libsnd_de_15.c` | `cc97_lib` |  |
| 0x800974D8 | `_SsSetNrpnVabAttr16` | libsnd `de_16.o` +0x0 | `libsnd_de_16.c` | `cc97_lib` |  |
| 0x80097508 | `_SsSetNrpnVabAttr17` | libsnd `de_17.o` +0x0 | `libsnd_de_17.c` | `cc97_lib` |  |
| 0x80097538 | `_SsSetNrpnVabAttr18` | libsnd `de_18.o` +0x0 | `libsnd_de_18.c` | `cc97_lib` |  |
| 0x80097568 | `_SsSetNrpnVabAttr19` | libsnd `de_19.o` +0x0 | `libsnd_de_19.c` | `cc97_lib` |  |
| 0x80097598 | `_SsSetPitchBend` | libsnd `midibend.o` +0x0 | `libsnd_midibend.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80097648 | `_SsSetControlChange` | libsnd `midicc.o` +0x0 | `libsnd_midicc.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80097878 | `_SsGetMetaEvent` | libsnd `midimeta.o` +0x0 | `libsnd_midimeta.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80097A48 | `_SsNoteOn` | libsnd `midinote.o` +0x0 | `libsnd_midinote.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x80097B28 | `_SsSetProgramChange` | libsnd `midiprog.o` +0x0 | `libsnd_midiprog.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80097B98 | `_SsReadDeltaValue` | libsnd `miditime.o` +0x0 | `libsnd_miditime.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x80097C48 | `_SsInitSoundSeq` | libsnd `seqinit.o` +0x0 | `libsnd_seqinit.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` |  |
| 0x80097F98 | `SsSeqPlay` | libsnd `ssplay.o` +0x0 | `libsnd_ssplay.c` | `cc97_lib` |  |
| 0x80097FD0 | `SsSepPlay` | libsnd `ssplay.o` +0x38 | `libsnd_ssplay.c` | `cc97_lib` |  |
| 0x80098018 | `Snd_SetPlayMode` | libsnd `playmode.o` +0x0 | `libsnd_playmode.c` | `cc97_lib` |  |
| 0x80098138 | `func_80098138` | libsnd `ssstart.o` +0x0 | `libsnd_ssstart_1.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | Installs the sequencer tick that the tick settings ask for (VSync or root counter). |
| 0x80098368 | `SsStart` | libsnd `ssstart.o` +0x230 | `libsnd_ssstart_2.c` | `cc97_lib` |  |
| 0x80098388 | `SsStart2` | libsnd `ssstart.o` +0x250 | `libsnd_ssstart_2.c` | `cc97_lib` |  |
| 0x800983A8 | `func_800983A8` | libsnd `ssstart.o` +0x270 | `libsnd_ssstart_3.c` | `cc97_lib` + `return_slot_fill` | The handler `func_80098138` installs on the vertical blank with a root counter: runs the displaced VSync handler, then the tick. |
| 0x800983F4 | `func_800983F4` | libsnd `ssstart.o` +0x2BC | `libsnd_ssstart_4.c` | `cc97_lib` | The half-rate tick handler for rates below 70 Hz: runs the tick every second call. |
| 0x80098448 | `SsSeqCalledTbyT` | libsnd `sscall.o` +0x0 | `libsnd_sscall.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800986B8 | `_SsSndCrescendo` | libsnd `cres.o` +0x0 | `libsnd_cres.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x800988C8 | `_SsSndPause` | libsnd `pause.o` +0x0 | `libsnd_pause.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80098968 | `_SsSndPlay` | libsnd `play.o` +0x0 | `libsnd_play.c` | `cc97_lib` |  |
| 0x80098998 | `_SsSeqPlay` | libsnd `midiread.o` +0x0 | `libsnd_midiread.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80098A94 | `_SsSeqGetEof` | libsnd `midiread.o` +0xFC | `libsnd_midiread.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80098CD8 | `_SsGetSeqData` | libsnd `midiread.o` +0x340 | `libsnd_midiread.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80099088 | `_SsSndNextSep` | libsnd `next.o` +0x0 | `libsnd_next.c` | `cc97_lib` |  |
| 0x80099188 | `_SsSndReplay` | libsnd `replay.o` +0x0 | `libsnd_replay.c` | `cc97_lib` |  |
| 0x800991E8 | `_SsSndStop` | libsnd `ssstop.o` +0x0 | `libsnd_ssstop_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80099368 | `SsSeqStop` | libsnd `ssstop.o` +0x180 | `libsnd_ssstop_2.c` | `cc97_lib` |  |
| 0x80099390 | `SsSepStop` | libsnd `ssstop.o` +0x1A8 | `libsnd_ssstop_2.c` | `cc97_lib` |  |
| 0x800993C8 | `SsSetTableSize` | libsnd `sstable.o` +0x0 | `libsnd_sstable.c` | `cc97_lib` |  |
| 0x800995E8 | `SsSetTickMode` | libsnd `sstick.o` +0x0 | `libsnd_sstick.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x80099738 | `_SsSndSetVol` | libsnd `ssvol.o` +0x0 | `libsnd_ssvol.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x800997C8 | `SsSeqSetVol` | libsnd `ssvol.o` +0x90 | `libsnd_ssvol.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x80099830 | `SsSepSetVol` | libsnd `ssvol.o` +0xF8 | `libsnd_ssvol.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x800998C0 | `SsSeqGetVol` | libsnd `ssvol.o` +0x188 | `libsnd_ssvol.c` | `cc97_lib` |  |
| 0x800998F8 | `_SsSndTempo` | libsnd `tempo.o` +0x0 | `libsnd_tempo.c` | `cc97_lib` |  |
| 0x80099B18 | `SsUtGetProgAtr` | libsnd `ut_gpa.o` +0x0 | `libsnd_ut_gpa.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80099C28 | `SsUtGetVagAtr` | libsnd `ut_gva.o` +0x0 | `libsnd_ut_gva.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x80099E68 | `SsUtSetReverbDelay` | libsnd `ut_rdel.o` +0x0 | `libsnd_ut_rdel.c` | `cc97_lib` |  |
| 0x80099EA8 | `SsUtSetReverbDepth` | libsnd `ut_rdep.o` +0x0 | `libsnd_ut_rdep.c` | `cc97_lib` |  |
| 0x80099F38 | `SsUtSetReverbType` | libsnd `ut_rev.o` +0x0 | `libsnd_ut_rev.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x80099FD4 | `SsUtGetReverbType` | libsnd `ut_rev.o` +0x9C | `libsnd_ut_rev.c` | `cc97_lib` |  |
| 0x80099FE8 | `SsUtSetReverbFeedback` | libsnd `ut_rfb.o` +0x0 | `libsnd_ut_rfb.c` | `cc97_lib` |  |
| 0x8009A028 | `SsUtReverbOff` | libsnd `ut_roff.o` +0x0 | `libsnd_ut_roff.c` | `cc97_lib` |  |
| 0x8009A048 | `SsUtReverbOn` | libsnd `ut_ron.o` +0x0 | `libsnd_ut_ron.c` | `cc97_lib` |  |
| 0x8009A068 | `SsUtSetVagAtr` | libsnd `ut_sva.o` +0x0 | `libsnd_ut_sva.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009A238 | `_SsVmDamperOff` | libsnd `vm_doff.o` +0x0 | `libsnd_vm_doff.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009A248 | `_SsVmDamperOn` | libsnd `vm_don.o` +0x0 | `libsnd_vm_don.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009A258 | `_SsVmFlush` | libsnd `vm_f.o` +0x0 | `libsnd_vm_f.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009A728 | `_SsVmInit` | libsnd `vm_init.o` +0x0 | `libsnd_vm_init.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009AA78 | `_SsVmKeyOn` | libsnd `vm_key.o` +0x0 | `libsnd_vm_key_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009AF94 | `_SsVmKeyOff` | libsnd `vm_key.o` +0x51C | `libsnd_vm_key_1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009B0F4 | `_SsVmSeKeyOn` | libsnd `vm_key.o` +0x67C | `libsnd_vm_key_2.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009B1E0 | `_SsVmSeKeyOff` | libsnd `vm_key.o` +0x768 | `libsnd_vm_key_2.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009B214 | `KeyOnCheck` | libsnd `vm_key.o` +0x79C | `libsnd_vm_key_2.c` | none (GCC 2.8.1) |  |
| 0x8009B228 | `_SsVmAlloc` | libsnd `vm_aloc1.o` +0x0 | `libsnd_vm_aloc1.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009B498 | `_SsVmDoAllocate` | libsnd `vm_aloc2.o` +0x0 | `libsnd_vm_aloc2.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009B668 | `note2pitch` | libsnd `vm_n2p.o` +0x0 | `libsnd_vm_n2p.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009B6B4 | `note2pitch2` | libsnd `vm_n2p.o` +0x4C | `libsnd_vm_n2p.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009B71C | `SsPitchFromNote` | libsnd `vm_n2p.o` +0xB4 | `libsnd_vm_n2p.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009B848 | `vmNoiseOn` | libsnd `vm_no1.o` +0x0 | `libsnd_vm_no1.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009BDB8 | `vmNoiseOff` | libsnd `vm_noff.o` +0x0 | `libsnd_vm_noff.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009BDF8 | `_SsVmKeyOffNow` | libsnd `vm_nowof.o` +0x0 | `libsnd_vm_nowof.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009BEB8 | `_SsVmKeyOnNow` | libsnd `vm_nowon.o` +0x0 | `libsnd_vm_nowon.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009C388 | `_SsVmPBVoice` | libsnd `vm_pb.o` +0x0 | `libsnd_vm_pb.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009C574 | `_SsVmPitchBend` | libsnd `vm_pb.o` +0x1EC | `libsnd_vm_pb.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009C658 | `_SsVmSetProgVol` | libsnd `vm_prog.o` +0x0 | `libsnd_vm_prog.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009C6C8 | `_SsVmGetProgVol` | libsnd `vm_prog.o` +0x70 | `libsnd_vm_prog.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009C718 | `_SsVmSetProgPan` | libsnd `vm_prog.o` +0xC0 | `libsnd_vm_prog.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009C788 | `_SsVmGetProgPan` | libsnd `vm_prog.o` +0x130 | `libsnd_vm_prog.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009C7D8 | `_SsVmSetSeqVol` | libsnd `vm_seq.o` +0x0 | `libsnd_vm_seq.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009CD44 | `_SsVmGetSeqVol` | libsnd `vm_seq.o` +0x56C | `libsnd_vm_seq.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009CDA8 | `_SsVmGetSeqLVol` | libsnd `vm_seq.o` +0x5D0 | `libsnd_vm_seq.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009CDF0 | `_SsVmGetSeqRVol` | libsnd `vm_seq.o` +0x618 | `libsnd_vm_seq.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009CE38 | `_SsVmSeqKeyOff` | libsnd `vm_seq.o` +0x660 | `libsnd_vm_seq.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009CEE8 | `_SsVmSelectToneAndVag` | libsnd `vm_stav.o` +0x0 | `libsnd_vm_stav.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009CFB8 | `_SsVmSetVol` | libsnd `vm_vol.o` +0x0 | `libsnd_vm_vol.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009D558 | `_SsVmVSetUp` | libsnd `vm_vsu.o` +0x0 | `libsnd_vm_vsu.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009D618 | `SsSetMono` | libsnd `vs_mono.o` +0x0 | `libsnd_vs_mono.c` | none (GCC 2.8.1) + `return_slot_fill` |  |
| 0x8009D628 | `SsSetStereo` | libsnd `vs_mono.o` +0x10 | `libsnd_vs_mono.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009D638 | `SsVabClose` | libsnd `vs_vab.o` +0x0 | `libsnd_vs_vab.c` | `cc272_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009D6E8 | `SsVabOpenHead` | libsnd `vs_vh2.o` +0x0 | `libsnd_vs_vh2.c` | `cc97_lib` |  |
| 0x8009D71C | `func_8009D71C` | libsnd `vs_vh2.o` +0x34 | `libsnd_vs_vh2.c` | `cc272_lib` + `return_slot_fill` | `SsVabOpenHead`'s allocator: `SpuMalloc` of the size; on failure frees the VAB slot and returns -1. |
| 0x8009D788 | `SsVabOpenHeadSticky` | libsnd `vs_vh.o` +0x0 | `libsnd_vs_vh.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009D7BC | `SsVabFakeHead` | libsnd `vs_vh.o` +0x34 | `libsnd_vs_vh.c` | `cc97_lib` + `gnu_as_loads` |  |
| 0x8009D7F0 | `func_8009D7F0` | libsnd `vs_vh.o` +0x68 | `libsnd_vs_vh.c` | `cc97_lib` + `gnu_as_loads` | The pass-through allocator `SsVabOpenHeadSticky` and `SsVabFakeHead` use: returns the SPU address it is given. |
| 0x8009D7F8 | `_SsVabOpenHeadWithMode` | libsnd `vs_vh.o` +0x70 | `libsnd_vs_vh.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009DBE8 | `SsVabTransBody` | libsnd `vs_vtb.o` +0x0 | `libsnd_vs_vtb.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x8009DCA8 | `SsVabTransCompleted` | libsnd `vs_vtc.o` +0x0 | `libsnd_vs_vtc.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009DCD8 | `SpuInit` | libspu `s_i.o` +0x0 | `libspu_s_i.c` | `cc97_lib` |  |
| 0x8009DCF8 | `_SpuInit` | libspu `s_ini.o` +0x0 | `libspu_s_ini_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009DDE0 | `SpuStart` | libspu `s_ini.o` +0xE8 | `libspu_s_ini_2.c` | `cc97_lib` + `reorder_fill` |  |
| 0x8009DE78 | `_spu_init` | libspu `spu.o` +0x0 | `libspu_spu_1.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x8009E0F8 | `func_8009E0F8` | libspu `spu.o` +0x280 | `libspu_spu_1.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | Writes bytes to SPU RAM through the data port, without DMA, with time-out reports. |
| 0x8009E2B8 | `_spu_FiDMA` | libspu `spu.o` +0x440 | `libspu_spu_2.c` | `cc97_lib` |  |
| 0x8009E374 | `_spu_Fr_` | libspu `spu.o` +0x4FC | `libspu_spu_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009E41C | `_spu_t` | libspu `spu.o` +0x5A4 | `libspu_spu_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009E69C | `_spu_Fw` | libspu `spu.o` +0x824 | `libspu_spu_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009E720 | `_spu_Fr` | libspu `spu.o` +0x8A8 | `libspu_spu_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009E784 | `_spu_FsetRXX` | libspu `spu.o` +0x90C | `libspu_spu_3.c` | `cc97_lib` |  |
| 0x8009E7C8 | `_spu_FsetRXXa` | libspu `spu.o` +0x950 | `libspu_spu_3.c` | `cc97_lib` |  |
| 0x8009E86C | `_spu_FgetRXXa` | libspu `spu.o` +0x9F4 | `libspu_spu_3.c` | `cc97_lib` |  |
| 0x8009E8A8 | `_spu_FsetPCR` | libspu `spu.o` +0xA30 | `libspu_spu_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009E900 | `func_8009E900` | libspu `spu.o` +0xA88 | `libspu_spu_3.c` | none (GCC 2.8.1) | Sets the SPU bus delay for writing (`_spu_t`'s comment: the delay for the direction). |
| 0x8009E928 | `func_8009E928` | libspu `spu.o` +0xAB0 | `libspu_spu_3.c` | none (GCC 2.8.1) | Sets the SPU bus delay for reading (`_spu_t`'s comment: the delay for the direction). |
| 0x8009E950 | `_spu_Fw1ts` | libspu `spu.o` +0xAD8 | `libspu_spu_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009E9B8 | `_SpuDataCallback` | libspu `s_dcb.o` +0x0 | `libspu_s_dcb.c` | `cc97_lib` |  |
| 0x8009E9E8 | `SpuInitMalloc` | libspu `s_m_init.o` +0x0 | `libspu_s_m_init.c` | `cc97_lib` |  |
| 0x8009EA48 | `SpuMalloc` | libspu `s_m_m.o` +0x0 | `libspu_s_m_m.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009ED18 | `_spu_gcSPU` | libspu `s_m_int.o` +0x0 | `libspu_s_m_int.c` | `cc97_lib` |  |
| 0x8009F018 | `SpuFree` | libspu `s_m_f.o` +0x0 | `libspu_s_m_f.c` | `cc97_lib` |  |
| 0x8009F098 | `SpuSetNoiseVoice` | libspu `s_snv.o` +0x0 | `libspu_s_snv.c` | `cc97_lib` |  |
| 0x8009F0C8 | `_SpuSetAnyVoice` | libspu `s_sav.o` +0x0 | `libspu_s_sav.c` | `cc97_lib` |  |
| 0x8009F388 | `SpuGetNoiseVoice` | libspu `s_gnv.o` +0x0 | `libspu_s_gnv.c` | `cc97_lib` |  |
| 0x8009F3B8 | `_SpuGetAnyVoice` | libspu `s_gav.o` +0x0 | `libspu_s_gav.c` | `cc97_lib` |  |
| 0x8009F3E8 | `SpuSetNoiseClock` | libspu `s_snc.o` +0x0 | `libspu_s_snc.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009F438 | `SpuSetReverb` | libspu `s_sr.o` +0x0 | `libspu_s_sr.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009F508 | `_SpuIsInAllocateArea` | libspu `s_m_util.o` +0x0 | `libspu_s_m_util.c` | `cc97_lib` |  |
| 0x8009F588 | `_SpuIsInAllocateArea_` | libspu `s_m_util.o` +0x80 | `libspu_s_m_util.c` | `cc97_lib` |  |
| 0x8009F618 | `SpuSetReverbModeParam` | libspu `s_srmp.o` +0x0 | `libspu_s_srmp.c` | `cc97_lib` + `gnu_as_loads` + `reorder_fill` + `return_slot_fill` |  |
| 0x8009FAF8 | `_spu_setReverbAttr` | libspu `s_sra.o` +0x0 | `libspu_s_sra.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x8009FFC8 | `SpuSetReverbVoice` | libspu `s_srv.o` +0x0 | `libspu_s_srv.c` | `cc97_lib` |  |
| 0x8009FFF8 | `SpuGetReverbVoice` | libspu `s_grv.o` +0x0 | `libspu_s_grv.c` | `cc97_lib` |  |
| 0x800A0028 | `SpuClearReverbWorkArea` | libspu `s_crwa.o` +0x0 | `libspu_s_crwa.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A01D8 | `SpuSetKey` | libspu `s_sk.o` +0x0 | `libspu_s_sk.c` | `cc97_lib` |  |
| 0x800A0398 | `SpuWrite` | libspu `s_w.o` +0x0 | `libspu_s_r.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A03F8 | `SpuSetTransferStartAddr` | libspu `s_stsa.o` +0x0 | `libspu_s_stsa.c` | `cc97_lib` |  |
| 0x800A0458 | `SpuSetTransferMode` | libspu `s_stm.o` +0x0 | `libspu_s_stm.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A0488 | `SpuIsTransferCompleted` | libspu `s_itc.o` +0x0 | `libspu_s_itc.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A0548 | `_spu_setInTransfer` | libspu `s_it.o` +0x0 | `libspu_s_it.c` | `cc97_lib` |  |
| 0x800A0570 | `_spu_getInTransfer` | libspu `s_it.o` +0x28 | `libspu_s_it.c` | `cc97_lib` |  |
| 0x800A0588 | `SpuSetVoiceAttr` | libspu `s_sva.o` +0x0 | `libspu_s_sva.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A0B88 | `_spu_note2pitch` | libspu `s_n2p.o` +0x0 | `libspu_s_n2p.c` | `cc97_lib` |  |
| 0x800A0C58 | `_spu_pitch2note` | libspu `s_n2p.o` +0xD0 | `libspu_s_n2p.c` | `cc97_lib` |  |
| 0x800A0D88 | `SpuGetVoiceEnvelope` | libspu `s_gvex.o` +0x0 | `libspu_s_gvex.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A0DA8 | `SpuSetCommonMasterVolume` | libspu `s_scmv.o` +0x0 | `libspu_s_scmv.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A0DC8 | `SpuSetCommonCDVolume` | libspu `s_sccv.o` +0x0 | `libspu_s_sccv.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A0DE8 | `SpuSetCommonCDReverb` | libspu `s_sccr.o` +0x0 | `libspu_s_sccr.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A0E28 | `SpuSetCommonCDMix` | libspu `s_sccm.o` +0x0 | `libspu_s_sccm.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A0E68 | `SpuSetReverbModeType` | libspu `s_srmt.o` +0x0 | `libspu_s_srmt.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A1038 | `SpuSetReverbModeDepth` | libspu `s_srmd.o` +0x0 | `libspu_s_srmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A1068 | `MemCardInit` | libmcrd `init.o` +0x0 | `libmcrd_init.c` | none (GCC 2.8.1) |  |
| 0x800A1098 | `MemCardEnd` | libmcrd `init.o` +0x30 | `libmcrd_init.c` | none (GCC 2.8.1) |  |
| 0x800A10B8 | `InitCARD` | libcard `init.o` +0x0 | `libcard_init.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A1148 | `StartCARD` | libcard `init.o` +0x90 | `libcard_init.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A1190 | `StopCARD` | libcard `init.o` +0xD8 | `libcard_init.c` | `cc97_lib` |  |
| 0x800A11B8 | `SetInitPadFlag` | libapi `pad.o` +0x0 | `libapi_pad_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A11C4 | `ReadInitPadFlag` | libapi `pad.o` +0xC | `libapi_pad_1.c` | `cc97_lib` |  |
| 0x800A11D4 | `PAD_init` | libapi `pad.o` +0x1C | `libapi_pad_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A1264 | `InitPAD` | libapi `pad.o` +0xAC | `libapi_pad_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A12F4 | `StartPAD` | libapi `pad.o` +0x13C | `libapi_pad_1.c` | `cc97_lib` |  |
| 0x800A1324 | `StopPAD` | libapi `pad.o` +0x16C | `libapi_pad_2.c` | `cc97_lib` |  |
| 0x800A135C | `func_800A135C` | libapi `pad.o` +0x1A4 | `libapi_pad_3.c` | `cc97_lib` + `return_slot_fill` | Fills the pad interrupt entry and puts it at the head of the kernel's interrupt queue 1. |
| 0x800A13D4 | `RemovePatchPad` | libapi `pad.o` +0x21C | `libapi_pad_3.c` | `cc97_lib` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x800A140C | `_Pad1` | libapi `pad.o` +0x254 | `libapi_pad_4.c` | `cc97_lib` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x800A1474 | `_IsVSync` | libapi `pad.o` +0x2BC | `libapi_pad_4.c` | `cc97_lib` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x800A18F8 | `MemCardStart` | libmcrd `libmcrd.o` +0x0 | `libmcrd_libmcrd_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A1968 | `MemCardStop` | libmcrd `libmcrd.o` +0x70 | `libmcrd_libmcrd_2.c` | `cc97_lib` |  |
| 0x800A19A8 | `MemCardExist` | libmcrd `libmcrd.o` +0xB0 | `libmcrd_libmcrd_2.c` | `cc97_lib` + `reorder_fill` |  |
| 0x800A1A10 | `func_800A1A10` | libmcrd `libmcrd.o` +0x118 | `libmcrd_libmcrd_3.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | `MemCardExist`'s step function: queries the card and turns the event into the result. |
| 0x800A1D2C | `MemCardAccept` | libmcrd `libmcrd.o` +0x434 | `libmcrd_libmcrd_4.c` | `cc97_lib` + `reorder_fill` |  |
| 0x800A1D94 | `func_800A1D94` | libmcrd `libmcrd.o` +0x49C | `libmcrd_libmcrd_5.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | `MemCardAccept`'s step function: checks, clears and loads the card's directory, retrying. |
| 0x800A203C | `MemCardOpen` | libmcrd `libmcrd.o` +0x744 | `libmcrd_libmcrd_5.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A21C4 | `MemCardClose` | libmcrd `libmcrd.o` +0x8CC | `libmcrd_libmcrd_5.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A2208 | `MemCardReadData` | libmcrd `libmcrd.o` +0x910 | `libmcrd_libmcrd_6.c` | `cc97_lib` + `reorder_fill` |  |
| 0x800A22C0 | `func_800A22C0` | libmcrd `libmcrd.o` +0x9C8 | `libmcrd_libmcrd_7.c` | `cc97_lib` + `return_slot_fill` | `MemCardReadData`'s step function: seeks, reads, waits for the event, retries up to three times. |
| 0x800A23FC | `MemCardWriteData` | libmcrd `libmcrd.o` +0xB04 | `libmcrd_libmcrd_8.c` | `cc97_lib` + `reorder_fill` |  |
| 0x800A24B4 | `func_800A24B4` | libmcrd `libmcrd.o` +0xBBC | `libmcrd_libmcrd_9.c` | `cc97_lib` + `return_slot_fill` | `MemCardWriteData`'s step function: seeks, writes, waits for the event, retries up to three times. |
| 0x800A25F0 | `MemCardReadFile` | libmcrd `libmcrd.o` +0xCF8 | `libmcrd_libmcrd_9.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A2708 | `func_800A2708` | libmcrd `libmcrd.o` +0xE10 | `libmcrd_libmcrd_9.c` | `cc97_lib` + `return_slot_fill` | `MemCardReadFile`'s step function: checks the card, opens the file and queues the read. |
| 0x800A2848 | `MemCardWriteFile` | libmcrd `libmcrd.o` +0xF50 | `libmcrd_libmcrd_9.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A2960 | `func_800A2960` | libmcrd `libmcrd.o` +0x1068 | `libmcrd_libmcrd_9.c` | `cc97_lib` + `return_slot_fill` | `MemCardWriteFile`'s step function: checks the card, opens the file and queues the write. |
| 0x800A2AA0 | `MemCardGetDirentry` | libmcrd `libmcrd.o` +0x11A8 | `libmcrd_libmcrd_9.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A2CFC | `MemCardCallback` | libmcrd `libmcrd.o` +0x1404 | `libmcrd_libmcrd_9.c` | `cc97_lib` |  |
| 0x800A2D10 | `MemCardSync` | libmcrd `libmcrd.o` +0x1418 | `libmcrd_libmcrd_9.c` | `cc97_lib` |  |
| 0x800A2E2C | `MemCardCreateFile` | libmcrd `libmcrd.o` +0x1534 | `libmcrd_libmcrd_9.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A2F28 | `MemCardDeleteFile` | libmcrd `libmcrd.o` +0x1630 | `libmcrd_libmcrd_9.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A3394 | `MemCardFormat` | libmcrd `libmcrd.o` +0x1A9C | `libmcrd_libmcrd_10.c` | `cc97_lib` + `reorder_fill` |  |
| 0x800A3408 | `MemCardUnformat` | libmcrd `libmcrd.o` +0x1B10 | `libmcrd_libmcrd_11.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A34BC | `func_800A34BC` | libmcrd `libmcrd.o` +0x1BC4 | `libmcrd_libmcrd_11.c` | `cc97_lib` | Turns a memory-card event number into the library's result code. |
| 0x800A3510 | `func_800A3510` | libmcrd `libmcrd.o` +0x1C18 | `libmcrd_libmcrd_12.c` | `cc97_lib` | The VSync callback `MemCardStart` installs: runs one queued step and completes the command when the last one is done. |
| 0x800A35BC | `_make_file_discrpt` | libmcrd `libmcrd.o` +0x1CC4 | `libmcrd_libmcrd_13.c` | `cc97_lib` | No public name in 4.5; an earlier release exports its bytes under this name (`sdk-earlier`). |
| 0x800A3638 | `firstfile` | libapi `first.o` +0x0 | `libapi_first.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A37D4 | `func_800A37D4` | libapi `first.o` +0x19C | `libapi_first.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` | The device hook `firstfile` installs: marks the I/O block, puts the device's own `firstfile` back and calls it with the same arguments. |
| 0x800A38E8 | `strcat` | libc2 `strcat.o` +0x0 | `libc2_strcat.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A39D8 | `_card_clear` | libcard `card.o` +0x0 | `libcard_card.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A3A18 | `UserFuncInit` | libmcrd `userfunc.o` +0x0 | `libmcrd_userfunc_1.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A3A28 | `UserFuncOpen` | libmcrd `userfunc.o` +0x10 | `libmcrd_userfunc_2.c` | `cc97_lib` + `reorder_fill` |  |
| 0x800A3AA4 | `UserFuncExecute` | libmcrd `userfunc.o` +0x8C | `libmcrd_userfunc_2.c` | `cc97_lib` |  |
| 0x800A3B10 | `UserFuncComplete` | libmcrd `userfunc.o` +0xF8 | `libmcrd_userfunc_3.c` | `cc97_lib` |  |
| 0x800A3B28 | `funcEvSpIOE` | libmcrd `bios.o` +0x0 | `libmcrd_bios_1.c` | `cc97_lib` |  |
| 0x800A3B3C | `funcEvSpError` | libmcrd `bios.o` +0x14 | `libmcrd_bios_1.c` | `cc97_lib` |  |
| 0x800A3B50 | `funcEvSpTimeout` | libmcrd `bios.o` +0x28 | `libmcrd_bios_1.c` | `cc97_lib` |  |
| 0x800A3B64 | `funcEvSpNewcard` | libmcrd `bios.o` +0x3C | `libmcrd_bios_1.c` | `cc97_lib` |  |
| 0x800A3B78 | `funcEvSpIOEx` | libmcrd `bios.o` +0x50 | `libmcrd_bios_1.c` | `cc97_lib` |  |
| 0x800A3B8C | `funcEvSpErrorx` | libmcrd `bios.o` +0x64 | `libmcrd_bios_1.c` | `cc97_lib` |  |
| 0x800A3BA0 | `funcEvSpTimeoutx` | libmcrd `bios.o` +0x78 | `libmcrd_bios_1.c` | `cc97_lib` |  |
| 0x800A3BB4 | `funcEvSpNewcardx` | libmcrd `bios.o` +0x8C | `libmcrd_bios_1.c` | `cc97_lib` |  |
| 0x800A3BC8 | `_card_open` | libmcrd `bios.o` +0xA0 | `libmcrd_bios_2.c` | `cc97_lib` |  |
| 0x800A3BF8 | `_card_start` | libmcrd `bios.o` +0xD0 | `libmcrd_bios_3.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A3DD4 | `_card_close` | libmcrd `bios.o` +0x2AC | `libmcrd_bios_4.c` | `cc97_lib` |  |
| 0x800A3DF4 | `_card_stop` | libmcrd `bios.o` +0x2CC | `libmcrd_bios_5.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A3EA8 | `_clr_card_event` | libmcrd `bios.o` +0x380 | `libmcrd_bios_6.c` | `cc97_lib` |  |
| 0x800A3FB0 | `_get_card_event` | libmcrd `bios.o` +0x488 | `libmcrd_bios_7.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A4088 | `_get_card_event_x` | libmcrd `bios.o` +0x560 | `libmcrd_bios_7.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A4160 | `_chk_card_event` | libmcrd `bios.o` +0x638 | `libmcrd_bios_7.c` | `cc97_lib` |  |
| 0x800A419C | `_chk_card_event_x` | libmcrd `bios.o` +0x674 | `libmcrd_bios_7.c` | `cc97_lib` |  |
| 0x800A41E8 | `_card_format2` | libmcrd `low.o` +0x0 | `libmcrd_low.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x800A45B8 | `_card_create2` | libmcrd `low.o` +0x3D0 | `libmcrd_low.c` | `cc97_lib` + `gnu_as_loads` + `return_slot_fill` |  |
| 0x800A4C78 | `strncpy` | libc2 `strncpy.o` +0x0 | `libc2_strncpy.c` | `cc97_lib` |  |
| 0x800A4CF8 | `PadChkVsync` | libpad `padentry.o` +0x0 | `libpad_padentry_1.c` | `cc97_lib` |  |
| 0x800A4D18 | `PadStartCom` | libpad `padentry.o` +0x20 | `libpad_padentry_1.c` | `cc97_lib` |  |
| 0x800A4D38 | `PadStopCom` | libpad `padentry.o` +0x40 | `libpad_padentry_1.c` | `cc97_lib` |  |
| 0x800A4D58 | `PadChkMtap` | libpad `padentry.o` +0x60 | `libpad_padentry_1.c` | `cc97_lib` |  |
| 0x800A4DA4 | `PadGetState` | libpad `padentry.o` +0xAC | `libpad_padentry_1.c` | `cc97_lib` |  |
| 0x800A4E70 | `PadInfoMode` | libpad `padentry.o` +0x178 | `libpad_padentry_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A4F68 | `PadInfoAct` | libpad `padentry.o` +0x270 | `libpad_padentry_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A503C | `PadInfoComb` | libpad `padentry.o` +0x344 | `libpad_padentry_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A50E4 | `PadSetActAlign` | libpad `padentry.o` +0x3EC | `libpad_padentry_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A511C | `PadSetMainMode` | libpad `padentry.o` +0x424 | `libpad_padentry_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A5164 | `PadSetAct` | libpad `padentry.o` +0x46C | `libpad_padentry_2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A51B8 | `PadEnableCom` | libpad `padmain.o` +0x0 | `libpad_padmain.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A52D0 | `_padSetVsyncParam` | libpad `padmain.o` +0x118 | `libpad_padmain.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A52FC | `_padIsVsync` | libpad `padmain.o` +0x144 | `libpad_padmain.c` | `cc97_lib` | No public name in 4.5; releases 4.6 and 4.7 export its bytes under this name (`sdk-later`). |
| 0x800A5364 | `_padIntPad` | libpad `padmain.o` +0x1AC | `libpad_padmain.c` | `cc97_lib` + `return_slot_fill` | No public name in 4.5; releases 4.6 and 4.7 export its bytes under this name (`sdk-later`). |
| 0x800A54F4 | `_padChkVsync` | libpad `padmain.o` +0x33C | `libpad_padmain.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A5508 | `_padStartCom` | libpad `padmain.o` +0x350 | `libpad_padmain.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A55D4 | `_padStopCom` | libpad `padmain.o` +0x41C | `libpad_padmain.c` | `cc97_lib` |  |
| 0x800A5618 | `_padInitSioMode` | libpad `padmain.o` +0x460 | `libpad_padmain.c` | `cc97_lib` + `reorder_fill` + `return_slot_fill` |  |
| 0x800A594C | `func_800A594C` | libpad `padmain.o` +0x794 | `libpad_padmain.c` | `cc97_lib` + `lib_g0` + `return_slot_fill` | Runs one step function of the controller transfer and moves the step on; ends the transfer on an error or time-out. |
| 0x800A5A3C | `_padSioRW` | libpad `padmain.o` +0x884 | `libpad_padmain.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A5C4C | `_padSioRW2` | libpad `padmain.o` +0xA94 | `libpad_padmain.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A5EC4 | `_padClrIntSio0` | libpad `padmain.o` +0xD0C | `libpad_padmain.c` | `cc97_lib` |  |
| 0x800A5F54 | `_padWaitRXready` | libpad `padmain.o` +0xD9C | `libpad_padmain.c` | `cc97_lib` |  |
| 0x800A5F88 | `_padSetAct` | libpad `padcmd.o` +0x0 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A5F94 | `_padSetCmd` | libpad `padcmd.o` +0xC | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A5FA4 | `_padSendAtLoadInfo` | libpad `padcmd.o` +0x1C | `libpad_padcmd.c` | `cc97_lib` |  |
| 0x800A6028 | `_padRecvAtLoadInfo` | libpad `padcmd.o` +0xA0 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A6298 | `_padGetActSize` | libpad `padcmd.o` +0x310 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A62D0 | `_padLoadActInfo` | libpad `padcmd.o` +0x348 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A63A4 | `func_800A63A4` | libpad `padcmd.o` +0x41C | `libpad_padcmd.c` | `cc97_lib` | The send hook `_padLoadActInfo` installs: queues the command of the current step of loading the controller's three tables. |
| 0x800A644C | `func_800A644C` | libpad `padcmd.o` +0x4C4 | `libpad_padcmd.c` | `cc97_lib` + `lib_g0` | The receive hook `_padLoadActInfo` installs: reads the replies and fills the ID, actuator and combination tables. |
| 0x800A6844 | `_padSetActAlign` | libpad `padcmd.o` +0x8BC | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A68AC | `func_800A68AC` | libpad `padcmd.o` +0x924 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` | The send hook `_padSetActAlign` installs: queues command 0x4D with the six alignment bytes. |
| 0x800A68C8 | `func_800A68C8` | libpad `padcmd.o` +0x940 | `libpad_padcmd.c` | `cc97_lib` | The receive hook `_padSetActAlign` installs: builds the record's own copy of the actuator alignment. |
| 0x800A6990 | `_padSetMainMode` | libpad `padcmd.o` +0xA08 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A6A28 | `func_800A6A28` | libpad `padcmd.o` +0xAA0 | `libpad_padcmd.c` | `cc97_lib` | The send hook `_padSetMainMode` installs: queues the mode command, then the alignment. |
| 0x800A6A7C | `func_800A6A7C` | libpad `padcmd.o` +0xAF4 | `libpad_padcmd.c` | `cc97_lib` | The receive hook `_padSetMainMode` installs: moves the sequence on, or clears the record when the mode changed. |
| 0x800A6ADC | `_padCmdParaMode` | libpad `padcmd.o` +0xB54 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A6AFC | `func_800A6AFC` | libpad `padcmd.o` +0xB74 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` | Queues controller command 0x45 with no parameter bytes. |
| 0x800A6B10 | `func_800A6B10` | libpad `padcmd.o` +0xB88 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` | Queues controller command 0x4C with one parameter byte, an ID-table offset. |
| 0x800A6B30 | `func_800A6B30` | libpad `padcmd.o` +0xBA8 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` | Queues controller command 0x46 with one parameter byte, an actuator number. |
| 0x800A6B50 | `func_800A6B50` | libpad `padcmd.o` +0xBC8 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` | Queues controller command 0x47 with one parameter byte, a combination number. |
| 0x800A6B70 | `func_800A6B70` | libpad `padcmd.o` +0xBE8 | `libpad_padcmd.c` | `cc97_lib` + `return_slot_fill` | Queues controller command 0x4B with no parameter bytes. |
| 0x800A6B88 | `func_800A6B88` | libpad `padif.o` +0x0 | `libpad_padif.c` | `cc97_lib` + `return_slot_fill` | Step 0 of a controller transfer: asks what to send and starts the exchange. |
| 0x800A6BD0 | `func_800A6BD0` | libpad `padif.o` +0x48 | `libpad_padif.c` | `cc97_lib` + `return_slot_fill` | Step 1 of a controller transfer: runs the gun hooks and sends the command byte. |
| 0x800A6CA8 | `func_800A6CA8` | libpad `padif.o` +0x120 | `libpad_padif.c` | `cc97_lib` + `return_slot_fill` | Step 2 of a controller transfer: sends the third byte and takes the controller's ID. |
| 0x800A6D54 | `func_800A6D54` | libpad `padif.o` +0x1CC | `libpad_padif.c` | `cc97_lib` + `return_slot_fill` | Step 3 of a controller transfer: decides whether a multi tap answers and checks the reply. |
| 0x800A6E18 | `func_800A6E18` | libpad `padif.o` +0x290 | `libpad_padif.c` | `cc97_lib` + `return_slot_fill` | Step 4 of a controller transfer: moves the payload bytes and ends the transfer. |
| 0x800A71B8 | `PadInitDirect` | libpad `padportd.o` +0x0 | `libpad_padportd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A7318 | `func_800A7318` | libpad `padportd.o` +0x160 | `libpad_padportd.c` | `cc97_lib` | Forgets the controller a port record describes. |
| 0x800A7380 | `func_800A7380` | libpad `padportd.o` +0x1C8 | `libpad_padportd.c` | `cc97_lib` + `return_slot_fill` | Closes the transfer with the port being served and starts the next port's. |
| 0x800A7474 | `func_800A7474` | libpad `padportd.o` +0x2BC | `libpad_padportd.c` | `cc97_lib` | Moves the queued command to the taken one and clears the queue byte. |
| 0x800A7484 | `func_800A7484` | libpad `padportd.o` +0x2CC | `libpad_padportd.c` | `cc97_lib` | Gives the byte to send at the transfer's current position. |
| 0x800A7540 | `func_800A7540` | libpad `padportd.o` +0x388 | `libpad_padportd.c` | `cc97_lib` + `return_slot_fill` | Decides which actuator bytes may be sent this round within the current budget. |
| 0x800A77BC | `func_800A77BC` | libpad `padportd.o` +0x604 | `libpad_padportd.c` | `cc97_lib` | Gives the port number a record belongs to. |
| 0x800A77F4 | `func_800A77F4` | libpad `padportd.o` +0x63C | `libpad_padportd.c` | `cc97_lib` | Gives the record of a port number. |
| 0x800A7818 | `_padInitDirSeq` | libpad `padseqd.o` +0x0 | `libpad_padseqd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A784C | `func_800A784C` | libpad `padseqd.o` +0x34 | `libpad_padseqd.c` | `cc97_lib` + `return_slot_fill` | Decides what a direct port sends next, by the record's sequence step. |
| 0x800A795C | `func_800A795C` | libpad `padseqd.o` +0x144 | `libpad_padseqd.c` | `cc97_lib` + `return_slot_fill` | Takes in a direct port's reply and moves the sequence step on. |
| 0x800A7BC0 | `_dirFailAuto` | libpad `padseqd.o` +0x3A8 | `libpad_padseqd.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A7CA0 | `func_800A7CA0` | libpad `padseqd.o` +0x488 | `libpad_padseqd.c` | `cc97_lib` | 0 when the record has an extended ID and its sequence is finished, else 1. |
| 0x800A7CD8 | `setRC2wait` | libpad `waitrc2.o` +0x0 | `libpad_waitrc2.c` | `cc97_lib` + `return_slot_fill` |  |
| 0x800A7CF8 | `chkRC2wait` | libpad `waitrc2.o` +0x20 | `libpad_waitrc2.c` | `cc97_lib` + `return_slot_fill` |  |

## Still assembly

Each routine below carries evidence in `config/asm_only.txt` that it was
never C, and stays an `INCLUDE_ASM` stub. The evidence column says what it
is: a BIOS call stub (the call's table and function number), an instruction
or register use the compiler never emits, or data that is not code.

| Address | Name | Member | File | Evidence | Note |
|---|---|---|---|---|---|
| 0x8008B388 | `InitHeap` | libapi `c57.o` +0x0 | `libapi_c57.c` | BIOS call stub, table A0, function 0x39 |  |
| 0x8008B398 | `FlushCache` | libapi `c68.o` +0x0 | `libapi_c68.c` | BIOS call stub, table A0, function 0x44 |  |
| 0x8008B3A8 | `_bu_init` | libapi `c112.o` +0x0 | `libapi_c112.c` | BIOS call stub, table A0, function 0x70 |  |
| 0x8008B3B8 | `EnterCriticalSection` | libapi `a36.o` +0x0 | `libapi_a36.c` | a `syscall` instruction |  |
| 0x8008B3C8 | `ExitCriticalSection` | libapi `a37.o` +0x0 | `libapi_a37.c` | a `syscall` instruction |  |
| 0x8008B3D8 | `open` | libapi `a50.o` +0x0 | `libapi_a50.c` | BIOS call stub, table B0, function 0x32 |  |
| 0x8008B3E8 | `lseek` | libapi `a51.o` +0x0 | `libapi_a51.c` | BIOS call stub, table B0, function 0x33 |  |
| 0x8008B3F8 | `read` | libapi `a52.o` +0x0 | `libapi_a52.c` | BIOS call stub, table B0, function 0x34 |  |
| 0x8008B408 | `write` | libapi `a53.o` +0x0 | `libapi_a53.c` | BIOS call stub, table B0, function 0x35 |  |
| 0x8008B418 | `close` | libapi `a54.o` +0x0 | `libapi_a54.c` | BIOS call stub, table B0, function 0x36 |  |
| 0x8008B428 | `Krom2RawAdd` | libapi `a81.o` +0x0 | `libapi_a81.c` | BIOS call stub, table B0, function 0x51 |  |
| 0x8008E7B8 | `GPU_cw` | libapi `c73.o` +0x0 | `libapi_c73.c` | BIOS call stub, table A0, function 0x49 |  |
| 0x8008FE08 | `func_8008FE08` | libgte `msc00.o` +0x0 | `libgte_msc00.c` | data, not code | Not a function: the member's version stamp, ahead of `InitGeom`; nothing reaches it. |
| 0x8008FE10 | `InitGeom` | libgte `msc00.o` +0x8 | `libgte_msc00.c` | `$ra` stored off `$at`; `$ra` loaded off `$ra`; `mfc0`, `mtc0`, `ctc2` |  |
| 0x8008FE98 | `SquareRoot0` | libgte `msc01.o` +0x0 | `libgte_msc01.c` | `mtc2`, `mfc2`, the trapping `sub`, the trapping `addi` |  |
| 0x8008FF28 | `SetColorMatrix` | libgte `mtx_11.o` +0x0 | `libgte_mtx_11.c` | `ctc2` |  |
| 0x8008FF58 | `SetFarColor` | libgte `reg11.o` +0x0 | `libgte_reg11.c` | `ctc2` |  |
| 0x8008FF78 | `SetGeomOffset` | libgte `reg12.o` +0x0 | `libgte_reg12.c` | `ctc2` |  |
| 0x8008FF98 | `_patch_gte` | libgte `patchgte.o` +0x0 | `libgte_patchgte.c` | `$ra` stored off `$at`; `$ra` loaded off `$ra` |  |
| 0x80090044 | `func_80090044` | libgte `patchgte.o` +0xAC | `libgte_patchgte.c` | `$k0` used; `$ra` stored off `$k0`; `mfc0` | Not a function: two fragments of exception-entry code that `_patch_gte` forms the address of; nothing calls them. |
| 0x80090448 | `DeliverEvent` | libapi `a07.o` +0x0 | `libapi_a07.c` | BIOS call stub, table B0, function 0x07 |  |
| 0x80094BB8 | `ChangeClearPAD` | libapi `a91.o` +0x0 | `libapi_a91.c` | BIOS call stub, table B0, function 0x5B |  |
| 0x80094BC8 | `ChangeClearRCnt` | libapi `l10.o` +0x0 | `libapi_l10.c` | BIOS call stub, table C0, function 0x0A |  |
| 0x80095298 | `func_80095298` | libapi `c114.o` +0x0 | `libapi_c114.c` | data, not code | Not a function: the member's version stamp, ahead of `_96_remove`; nothing reaches it. |
| 0x800952A0 | `_96_remove` | libapi `c114.o` +0x8 | `libapi_c114.c` | BIOS call stub, table A0, function 0x72 |  |
| 0x800952B8 | `ReturnFromException` | libapi `a23.o` +0x0 | `libapi_a23.c` | BIOS call stub, table B0, function 0x17 |  |
| 0x800952C8 | `ResetEntryInt` | libapi `a24.o` +0x0 | `libapi_a24.c` | BIOS call stub, table B0, function 0x18 |  |
| 0x800952D8 | `HookEntryInt` | libapi `a25.o` +0x0 | `libapi_a25.c` | BIOS call stub, table B0, function 0x19 |  |
| 0x800952E8 | `setjmp` | libc2 `setjmp.o` +0x0 | `libc2_setjmp.c` | `$ra` stored off `$a0`; `$gp` stored off `$a0`; `$sp` stored through memory |  |
| 0x80095324 | `longjmp` | libc2 `setjmp.o` +0x3C | `libc2_setjmp.c` | `$ra` loaded off `$a0`; `$gp` loaded off `$a0`; `$sp` loaded through memory |  |
| 0x8009DE58 | `OpenEvent` | libapi `a08.o` +0x0 | `libapi_a08.c` | BIOS call stub, table B0, function 0x08 |  |
| 0x8009DE68 | `EnableEvent` | libapi `a12.o` +0x0 | `libapi_a12.c` | BIOS call stub, table B0, function 0x0C |  |
| 0x800A01C8 | `WaitEvent` | libapi `a10.o` +0x0 | `libapi_a10.c` | BIOS call stub, table B0, function 0x0A |  |
| 0x800A0538 | `TestEvent` | libapi `a11.o` +0x0 | `libapi_a11.c` | BIOS call stub, table B0, function 0x0B |  |
| 0x800A14B8 | `InitPAD2` | libapi `a18.o` +0x0 | `libapi_a18.c` | BIOS call stub, table B0, function 0x12 |  |
| 0x800A14C8 | `StartPAD2` | libapi `a19.o` +0x0 | `libapi_a19.c` | BIOS call stub, table B0, function 0x13 |  |
| 0x800A14D8 | `StopPAD2` | libapi `a20.o` +0x0 | `libapi_a20.c` | BIOS call stub, table B0, function 0x14 |  |
| 0x800A14E8 | `PAD_init2` | libapi `a21.o` +0x0 | `libapi_a21.c` | BIOS call stub, table B0, function 0x15 |  |
| 0x800A14F8 | `SysEnqIntRP` | libapi `l02.o` +0x0 | `libapi_l02.c` | BIOS call stub, table C0, function 0x02 |  |
| 0x800A1508 | `SysDeqIntRP` | libapi `l03.o` +0x0 | `libapi_l03.c` | BIOS call stub, table C0, function 0x03 |  |
| 0x800A1518 | `EnablePAD` | libapi `patch.o` +0x0 | `libapi_patch.c` | a tail jump (`jr $t1`) |  |
| 0x800A152C | `DisablePAD` | libapi `patch.o` +0x14 | `libapi_patch.c` | a tail jump (`jr $t1`) |  |
| 0x800A1540 | `_patch_pad` | libapi `patch.o` +0x28 | `libapi_patch.c` | `$ra` stored off `$at`; the trapping `addi`; `$ra` loaded off `$ra` |  |
| 0x800A15B8 | `_remove_ChgclrPAD` | libapi `chclrpad.o` +0x0 | `libapi_chclrpad.c` | `$ra` stored off `$at`; the trapping `addi`; `$ra` loaded off `$ra` |  |
| 0x800A1628 | `InitCARD2` | libcard `a74.o` +0x0 | `libcard_a74.c` | BIOS call stub, table B0, function 0x4A |  |
| 0x800A1638 | `StartCARD2` | libcard `a75.o` +0x0 | `libcard_a75.c` | BIOS call stub, table B0, function 0x4B |  |
| 0x800A1648 | `StopCARD2` | libcard `a76.o` +0x0 | `libcard_a76.c` | BIOS call stub, table B0, function 0x4C |  |
| 0x800A1658 | `_patch_card_info` | libcard `patch.o` +0x0 | `libcard_patch.c` | `$ra` stored off `$at`; the trapping `addi`; `$ra` loaded off `$ra`; a routine of libcard `patch.o`, which holds hand assembly |  |
| 0x800A169C | `func_800A169C` | libcard `patch.o` +0x44 | `libcard_patch.c` | a routine of libcard `patch.o`, which holds hand assembly | Not a function: patch code whose address `_copy_memcard_patch` forms; nothing calls it. |
| 0x800A16C8 | `func_800A16C8` | libcard `patch.o` +0x70 | `libcard_patch.c` | a routine of libcard `patch.o`, which holds hand assembly | Not a function: patch code; nothing calls it or forms its address. |
| 0x800A170C | `func_800A170C` | libcard `patch.o` +0xB4 | `libcard_patch.c` | a tail jump (`jr $v0`); a routine of libcard `patch.o`, which holds hand assembly | Not a function: patch code whose address `_patch_card` and `_copy_memcard_patch` form; nothing calls it. |
| 0x800A1734 | `_patch_card` | libcard `patch.o` +0xDC | `libcard_patch.c` | `$ra` stored off `$at`; `$ra` loaded off `$ra`; a routine of libcard `patch.o`, which holds hand assembly |  |
| 0x800A17C8 | `_patch_card2` | libcard `patch.o` +0x170 | `libcard_patch.c` | `$ra` stored off `$at`; `$ra` loaded off `$ra`; a routine of libcard `patch.o`, which holds hand assembly |  |
| 0x800A1838 | `_copy_memcard_patch` | libcard `patch.o` +0x1E0 | `libcard_patch.c` | a routine of libcard `patch.o`, which holds hand assembly |  |
| 0x800A1878 | `_ExitCard` | libcard `end.o` +0x0 | `libcard_end.c` | `$ra` stored off `$at`; `$ra` loaded off `$ra` |  |
| 0x800A3618 | `nextfile` | libapi `a67.o` +0x0 | `libapi_a67.c` | BIOS call stub, table B0, function 0x43 |  |
| 0x800A3628 | `erase` | libapi `a69.o` +0x0 | `libapi_a69.c` | BIOS call stub, table B0, function 0x45 |  |
| 0x800A38D8 | `firstfile2` | libapi `a66.o` +0x0 | `libapi_a66.c` | BIOS call stub, table B0, function 0x42 |  |
| 0x800A3998 | `_card_info` | libcard `c171.o` +0x0 | `libcard_c171.c` | BIOS call stub, table A0, function 0xAB |  |
| 0x800A39A8 | `_card_load` | libcard `c172.o` +0x0 | `libcard_c172.c` | BIOS call stub, table A0, function 0xAC |  |
| 0x800A39B8 | `_card_write` | libcard `a78.o` +0x0 | `libcard_a78.c` | BIOS call stub, table B0, function 0x4E |  |
| 0x800A39C8 | `_new_card` | libcard `a80.o` +0x0 | `libcard_a80.c` | BIOS call stub, table B0, function 0x50 |  |
| 0x800A41D8 | `CloseEvent` | libapi `a09.o` +0x0 | `libapi_a09.c` | BIOS call stub, table B0, function 0x09 |  |
| 0x800A4CE8 | `_card_read` | libcard `a79.o` +0x0 | `libcard_a79.c` | BIOS call stub, table B0, function 0x4F |  |
