/* Lunar SSSC — xmalloc: the memory: the top that moves down and the malloc2
   heap under it, their checks and the usage print. The name is the
   developers' own: XMALLOC.C, the source file the assertions of
   xmalloc_top_alloc (line 146) and xmalloc_heap_check_start (line 211)
   name; the file's other functions have it by being of its group. The first
   split had it in io.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
extern u8 D_800EAFE8[];
extern u8 D_800116A8[];
extern u8 D_800116E0[];
extern u8 D_80011674[];
extern u8 D_80011680[];
extern u8 D_800B8718[];
extern s32 D_800E55B8;
extern s32 g_XmallocTop;
extern s32 g_XmallocTopStart;
extern s32 g_XmallocHeapStart;
extern s32 g_XmallocLast;
extern u32 g_XmallocMax;
extern s32 D_800E55C0;
s32 xmalloc_top_set(s32);                               /* extern */
void xmalloc_top_reset(void);
s32 InitHeap2(s32, s32);
void xmalloc_heap_reset(void);
void xmalloc_heap_update_max(u32 block_end);
s32 malloc2();
s32 xmalloc_heap_alloc(s32 byte_count);
s32 free2();
s32 xmalloc_heap_free(s32 block);
void xmalloc_heap_set_limit(s32 limit);
extern long HWD0, VWD0;
extern long HWD0;
extern long VWD0;
s32 *xmalloc_heap_find_end(void);
s32 debug_printf();
s32 assert_fail(s32 *, s32 *, s32);
void xmalloc_heap_check_start(void);
extern long HWD0, VWD0;
void xmalloc_top_reset(void);
s32 InitHeap2(s32, s32);
void xmalloc_heap_reset(void);
void xmalloc_heap_update_max(u32 block_end);
s32 malloc2();
s32 xmalloc_heap_alloc(s32 byte_count);
s32 free2();
s32 xmalloc_heap_free(s32 block);
void xmalloc_heap_set_limit(s32 limit);
void xmalloc_top_reset(void);
s32 InitHeap2(s32, s32);
void xmalloc_heap_reset(void);
void xmalloc_heap_update_max(u32 block_end);
s32 malloc2();
s32 xmalloc_heap_alloc(s32 byte_count);
s32 free2();
s32 xmalloc_heap_free(s32 block);
void xmalloc_heap_set_limit(s32 limit);
void xmalloc_init(void);
void xmalloc_print_usage(void);
s32 *xmalloc_heap_find_end(void);

/* func_80022D58 */
void xmalloc_clear_ram(void) {
    u32 *p;
    u32 *end;

    p = (u32 *)g_XmallocHeapStart;
    end = (u32 *)0x801FF000;

    while (p < end) {
        *p = 0;
        p++;
    }
}

/* func_80022D8C */
s32 xmalloc_top_set(s32 top) {
    g_XmallocTop = top;
}

/* func_80022D98 */
void xmalloc_top_reset(void) {
    xmalloc_top_set(g_XmallocTopStart);
}
/* far_syms: D_800EAFE8 (target reaches it %hi/%lo) */

/* func_80022DBC */
void xmalloc_init(void) {
    u32 var;
    var = (u32)D_800EAFE8;
    var += 0xF;
    var &= 0xFFFFFFF0;
    g_XmallocTopStart = 0x80156000;
    g_XmallocTop = 0x80156000;
    g_XmallocHeapStart = var;
    g_XmallocLast = var + 0x40000;
    g_XmallocMax = var;
    xmalloc_clear_ram();
    xmalloc_heap_reset();
}
extern u8 D_80011664[];

/* xmalloc_top_alloc(size) -- XMALLOC.C's permanent allocator: takes size bytes
   (rounded up to a multiple of 4) off the TOP of the free gap and returns
   the new top, which is the block's address. Nothing gives a block back;
   xmalloc_top_set(top) / xmalloc_top_reset() move the top itself (the latter back
   to g_XmallocTopStart = 0x80156000, xmalloc_init's start value). Its five calls
   in the image are all in main_buffer_init (main.c): 0x15828, 0x8000,
   0x4000, 0xA000 and a last one with size 0, which allocates nothing and
   only runs the limit step below.

   The gap is shared with the malloc2 heap, which grows UP from g_XmallocHeapStart
   (xmalloc_heap_reset = InitHeap2(g_XmallocHeapStart, g_XmallocLast - g_XmallocHeapStart); the
   game's allocate / free are xmalloc_heap_alloc / xmalloc_heap_free):

     g_XmallocTop  the top (an address; it moves down)       read, written
     g_XmallocLast  the heap's nominal end, g_XmallocHeapStart + 0x40000    read

   top -= size. Only when the new top is BELOW g_XmallocLast (unsigned):
     - xmalloc_heap_find_end() walks the heap's block headers to its end marker
       and returns that address; a top below it means the block just taken
       overlaps blocks malloc2 has handed out, and the assert hook is
       called: assert_fail("MEMORY STOP!", "XMALLOC.C", 146). The hook is
       empty in this build, so the function goes on.
     - xmalloc_heap_set_limit(top - 0x100) stores D_800E55C0 = malloc2's _max_heap:
       the heap may from now on grow to 0x100 bytes short of the top.
   A top at or above g_XmallocLast leaves _max_heap alone. No RNG draw, no
   frame wait.

   Shape, on the TU's declarations: the top is re-read from the global at
   each of its four later uses (the target has four gp loads of g_XmallocTop
   after the store), and both comparisons are unsigned (sltu) on the TU's
   s32 globals, as xmalloc_heap_update_max compares g_XmallocMax. */
/* func_80022E1C */
s32 xmalloc_top_alloc(s32 size) {
    size = (size + 3) & ~3;
    g_XmallocTop -= size;
    if ((u32)g_XmallocTop < (u32)g_XmallocLast) {
        if ((u32)g_XmallocTop < (u32)xmalloc_heap_find_end()) {
            assert_fail(D_80011664, D_80011674, 0x92);
        }
        xmalloc_heap_set_limit(g_XmallocTop - 0x100);
    }
    return g_XmallocTop;
}

s32 InitHeap2(s32, s32);                          /* extern */

/* func_80022E9C */
void xmalloc_heap_reset(void) {
    InitHeap2(g_XmallocHeapStart, g_XmallocLast - g_XmallocHeapStart);
}

/* func_80022EC4 */
void xmalloc_heap_update_max(u32 block_end) {
    if ((u32) g_XmallocMax < block_end) {
        g_XmallocMax = block_end;
    }
}
s32 debug_printf();  /* variadic: callers pass 1..9 args — unprototyped, as the original did */
s32 assert_fail(s32 *, s32 *, s32);

/* func_80022EE4 */
void xmalloc_heap_check_start(void)
{
  s32 top_alloc_area;
  if ((top_alloc_area = D_800E55B8) < 0)
  {
    if (((u32) top_alloc_area) <= 0x801FFFFFU)
    {
      if (top_alloc_area & 3)
      {
        goto block_3;
      }
    }
    else
    {
      goto block_4;
    }
  }
  else
  {
    block_3:
    block_4:
    debug_printf(D_80011680, top_alloc_area);

    assert_fail(D_800B8718, D_80011674, 0xD3);
  }
}

s32 malloc2();                                /* extern */

/* func_80022F4C */
s32 xmalloc_heap_alloc(s32 byte_count) {
    s32 block;

    block = malloc2();
    xmalloc_heap_update_max(block + byte_count);
    return block;
}

s32 free2();                                  /* extern */

/* Thunk: $a0 reaches free2 untouched and $v0 comes back untouched
   (its code is jal + jr, no argument setup), so this takes an argument
   and returns a value. 43 of 54 clean callsites prove >= 1 arg, and
   btl_amount.c and btl_window.c declare it `s32 f(s32)` with their callers
   byte-matching. */
/* func_80022F88 */
s32 xmalloc_heap_free(s32 block) {
    return free2(block);
}

/* func_80022FA8 */
void xmalloc_heap_set_limit(s32 limit) {
    D_800E55C0 = limit;
}
/* func_80022FB8 */
s32 *xmalloc_heap_find_end(void)
{
  s32 header_word;
  s32 header;
  s32 lit_4;
  header = g_XmallocHeapStart;
  loop_1:
  header_word = *((s32 *) header);

  if (header_word >= 0)
  {
    lit_4 = 4;
    header += ((header_word >> 2) * lit_4) + lit_4;
    goto loop_1;
  }
  return (s32 *) header;
}

/* func_80022FE8 */
void xmalloc_print_usage(void)
{
  s32 used_end;
  s32 measured_end;

  used_end = (s32)xmalloc_heap_find_end();
  measured_end = (s32)xmalloc_heap_find_end();
  debug_printf(D_800116A8, g_XmallocHeapStart, used_end, measured_end - g_XmallocHeapStart, g_XmallocMax, g_XmallocLast);
  debug_printf(D_800116E0, g_XmallocTop, g_XmallocTopStart, g_XmallocTopStart - g_XmallocTop, g_XmallocTop - (s32)xmalloc_heap_find_end(), g_XmallocTop - (s32)xmalloc_heap_find_end());
}
