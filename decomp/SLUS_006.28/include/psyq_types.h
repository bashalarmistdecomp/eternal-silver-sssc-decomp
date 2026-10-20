/* Types for the matching build.
 *
 * Sony's PSY-Q headers supply the SDK's types and prototypes: this file
 * includes eleven of them by name, below. They are Sony's and this repository
 * does not carry them; recipe/toolchain.sh fetches them, or PSYQ_INCLUDE
 * names a copy of your own (BUILD.md, "Sony's headers").
 *
 * The real headers, not a reconstruction, because a reconstruction cannot be
 * trusted for struct layout: the hand-written copies this file once held had
 * seven wrong aggregates (DRAWENV was 36 bytes where the real struct is 92,
 * its dr_env declared RECT, 8 bytes, instead of DR_ENV, 64).
 *
 * 4.4 is the version on the include path: PSY-Q 4.3/4.4 ship the CC1PSX
 * that identifies as "GNU C 2.8.1 SN32 BUILD 4.0.0010 (PSX)", the original
 * of the compiler this project rebuilds from source.
 */

#ifndef PSYQ_TYPES_H
#define PSYQ_TYPES_H

/* u_char/u_short/u_int/u_long. The library headers use these without
 * declaring them, so they must come first.
 *
 * They are written out literally rather than pulled from <sys/types.h>
 * because the attempt harness (station-tools, to be released later) inlines
 * THIS FILE with every `#` line stripped, to produce the standalone,
 * directive-free .c that m2c and the permuter require. A typedef reachable
 * only through an #include vanishes in that copy, and every attempt then
 * fails to compile — which is exactly how the first version of this shim
 * broke the check of converted functions (571/571 SKIP, "cc1 failed").
 *
 * The `#ifndef _UCHAR_T` guards are the SDK's own, so <sys/types.h> —
 * which libsnd.h pulls in — sees them already defined and does not
 * redefine them. C89 has no tolerance for a duplicate typedef. */
#ifndef _UCHAR_T
#define _UCHAR_T
typedef unsigned char u_char;
#endif
#ifndef _USHORT_T
#define _USHORT_T
typedef unsigned short u_short;
#endif
#ifndef _UINT_T
#define _UINT_T
typedef unsigned int u_int;
#endif
#ifndef _ULONG_T
#define _ULONG_T
typedef unsigned long u_long;
#endif

/* The headers declare their C structures and prototypes only for C, under
 * LANGUAGE_C. Defined, it gives kernel.h's struct DIRENTRY and struct EXEC,
 * which the C needs complete; it changes no byte of any program. */
#define LANGUAGE_C

/* The SDK proper. ORDER MATTERS: libgpu.h uses SVECTOR, which libgte.h
 * defines, and does not include it itself. */
#include <libgte.h>
#include <libgpu.h>
#include <libspu.h>
#include <libetc.h>
#include <libpad.h>
#include <libsnd.h>
#include <libmath.h>
#include <libgs.h>   /* 2026-08-10: GsIMAGE for the SLUS_009.21 TIM
                      * loader (func_8011E1C8). Vetted per the rule:
                      * psyqhdr --conflicts lists libgs.h NO-conflict,
                      * its 68 #defines contain no short lowercase name,
                      * and it #includes nothing. MUST come BEFORE
                      * libapi/kernel: their asm.h register macros
                      * (#define sp $29) expand libgs.h's own PARAMETER
                      * NAMES (`GsSPRITE *sp`) into `$29` — the register
                      * trap runs in BOTH directions, and the #undef
                      * block below only cures what comes after it. */
#include <libapi.h>
#include <kernel.h>


/* libapi.h and kernel.h drag in asm.h + r3000.h, which #define the MIPS
 * register names as lowercase macros (`#define ra $31`). That is assembler
 * scaffolding leaking into C identifier space, and it collides with real
 * m2c-generated locals — the first split's battle_b.c alone had s0, s1, t0
 * and zero as variables; btl_choice.c, cd_change.c, sound.c, sprite_draw.c
 * and start_screen.c have zero as one now. Undo it here rather than leaving a
 * landmine for whoever adds an SDK header later and cannot explain why one of
 * them stopped compiling.
 * The uppercase constants (SR_*, EXC_*, R_*, C0_*) are left alone. */
#undef zero
#undef AT
#undef v0
#undef v1
#undef a0
#undef a1
#undef a2
#undef a3
#undef t0
#undef t1
#undef t2
#undef t3
#undef t4
#undef t5
#undef t6
#undef t7
#undef t8
#undef t9
#undef s0
#undef s1
#undef s2
#undef s3
#undef s4
#undef s5
#undef s6
#undef s7
#undef k0
#undef k1
#undef gp
#undef sp
#undef fp
#undef ra
#undef flush
#undef inval
#undef lcache
#undef scache

/* libcd.h, like the other libraries' headers, but after the #undefs above:
 * its prototypes name a parameter fp, which asm.h's macros would turn into
 * $30. The files that call libcd once declared what they call themselves, in
 * this project's types, and the libcd types they needed were copied here; on
 * 2026-10-20 each of those lines gave way to libcd.h's own, the build proving
 * every program unchanged. */
#include <libcd.h>

/* Project scalar types. These are OURS, not the SDK's — every converted
 * function is written in them, and no PSY-Q header declares them. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

/* PS1 memory-mapped I/O the SDK does not expose. */
#define GPU_REG_DATA  (*(volatile u_long *)0x1F801810)
#define GPU_REG_STAT  (*(volatile u_long *)0x1F801814)
#define DMA_DPCR      (*(volatile u_long *)0x1F8010F0)
#define DMA_DICR      (*(volatile u_long *)0x1F8010F4)

#endif /* PSYQ_TYPES_H */
