/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libgpu/sys.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libgpu, member sys.o, part 4 of 6: set_alarm,
   func_8008E16C, func_8008E2B0. The first split had it in psyq_gpu_e.c. The
   tree compiles this run of the member's functions with other flags than
   the runs beside it (config/cc_flags.txt), so the member is in several
   files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern volatile u_long *D_800B60A4;
extern volatile u_long *D_800B60A8;
/* cc97_lib: set_alarm */
/* gnu_as_loads: set_alarm */
/* far_syms: D_800B60DC (target reaches it %hi/%lo) */
extern int D_800B60DC;
extern int D_800B60E0[];
extern int VSync(int);

/* libgpu set_alarm (SYS.OBJ, static): arms the GPU timeout that get_alarm
   (func_8008E16C) tests: the deadline D_800B60DC = VSync(-1) +
   240, the poll counter D_800B60E0 = 0. */
/* func_8008E138 */
void set_alarm(void)
{
    D_800B60DC = VSync(-1) + 0xF0;
    D_800B60E0[0] = 0;
}
/* cc97_lib: func_8008E16C */
/* gnu_as_loads: func_8008E16C */
/* far_syms: D_800B60AC (target reaches it %hi/%lo) */
/* far_syms: D_800B60B4 (target reaches it %hi/%lo) */
/* far_syms: D_800B60C4 (target reaches it %hi/%lo) */
/* far_syms: D_800B60C8 (target reaches it %hi/%lo) */
/* far_syms: D_800B60CC (target reaches it %hi/%lo) */
/* far_syms: D_800B60D8 (target reaches it %hi/%lo) */
extern volatile u_long **D_800B60AC;
extern volatile u_long *D_800B60B4;
extern volatile u_long *D_800B60C4;
extern volatile int D_800B60C8;
extern volatile int D_800B60CC;
extern int D_800B60D8;
extern char D_80013184[];
extern u_char D_800E5610[];
extern volatile GpuQue D_800E5718[];
extern int printf();
extern int SetIntrMask(int);
extern void func_8008E790(u_char *ptr, int value, int num);
int func_8008E2B0(int mode);  /* TU defines this below */

static inline int gpu_reset(int mode)
{
    /* sys.o's _reset (out of line at func_8008DEAC in libgpu_sys_3.c, the same
       statements), inlined by get_alarm, which sys.c defines after it */
    D_800B60D8 = SetIntrMask(0);
    D_800B60CC = 0;
    D_800B60C8 = D_800B60CC;
    switch (mode & 7) {
    case 0:
    case 5:
        *D_800B60B4 = 0x401;
        *D_800B60C4 |= 0x800;
        *D_800B60A8 = 0;
        func_8008E790(D_800E5610, 0, 0x100);
        func_8008E790((u_char *)D_800E5718, 0, 0x1800);
        break;
    case 1:
    case 3:
        *D_800B60B4 = 0x401;
        *D_800B60C4 |= 0x800;
        *D_800B60A8 = 0x02000000;
        *D_800B60A8 = 0x01000000;
        break;
    }
    SetIntrMask(D_800B60D8);
    return !(mode & 7) ? func_8008E2B0(mode) : 0;
}
/* libgpu get_alarm (SYS.OBJ, static): the GPU timeout test that every wait loop
   of the object polls. Returns 0 while VSync(-1) has not passed
   the deadline D_800B60DC and the poll counter D_800B60E0, incremented on each
   call that gets that far, was not above 0xF0000. Otherwise the timeout: it
   reads GPUSTAT once (value dropped), prints D_80013184 ("GPU
   timeout:que=%d,stat=%08x,chcr=%08x,madr=%08x\n") through printf
    with (_qin - _qout) & 0x3F, GPUSTAT, DMA2 CHCR and DMA2 MADR,
   runs _reset(1) (inlined: the queue emptied, DMA2 CHCR = 0x401, DPCR |= 0x800,
   GP1 = 0x02000000 and 0x01000000, under SetIntrMask) and returns -1. */
int func_8008E16C(void)
{
    if (D_800B60DC < VSync(-1) || D_800B60E0[0]++ > 0xF0000) {
        *D_800B60A8;
        printf(D_80013184, (D_800B60C8 - D_800B60CC) & 0x3F, *D_800B60A8, *D_800B60B4, *D_800B60AC);
        gpu_reset(1);
        return -1;
    }
    return 0;
}

/* cc97_lib: func_8008E2B0 */
/* gnu_as_loads: func_8008E2B0 */
/* far_syms: D_800B60A4 (target reaches it %hi/%lo) */
/* far_syms: D_800B60A8 (target reaches it %hi/%lo) */

/* libgpu _version(mode) (SYS.OBJ, static; _reset's result for a full reset): the
   GPU type. It writes GP1 = 0x10000007 (through D_800B60A8) and reads GPUREAD
   (through D_800B60A4). If the low 24 bits are not 2 it writes GP0 = 0xE1001000 |
   (GPUSTAT & 0x3FFF), reads GPUREAD once more (value dropped) and returns 0.
   Otherwise it returns 1, or, when mode & 8 is set, writes GP1 = 0x09000001 and
   returns 2. */
int func_8008E2B0(int mode)
{
    *D_800B60A8 = 0x10000007;
    if ((*D_800B60A4 & 0xFFFFFF) != 2) {
        *D_800B60A4 = 0xE1001000 | (*D_800B60A8 & 0x3FFF);
        *D_800B60A4;
        return 0;
    } else if (!(mode & 8)) {
        return 1;
    } else {
        *D_800B60A8 = 0x09000001;
        return 2;
    }
}
