/* Lunar SSSC — mem_request: the five-word memory request (a copy or a fill)
   and its twin; func_80054B28 (empty) is inside. The first split had it in
   misc_d.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "mem.h"

void mem_request_run();

/* A bare call-through to mem_request_run (the consumer of the g_MemRequest
   block request). Nothing in the EXE image calls it or holds its address,
   so no caller proves its arity; the body proves only the call. */
/* Ruling of 2026-10-14 (docs/naming.md): Does what mem_request_run does; nothing read says why there are two, and nothing reaches it. */
/* func_80054B08 */
void mem_request_run_copy(void)
{
    mem_request_run();
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_80054B28() {
}

extern MemRequest g_MemRequest;

/* The consumer of the block request g_MemRequest (rendering.md "The block
   request"). Its one caller is mem_request_run_copy above, which nothing in the
   EXE image calls, so in the image as shipped this never runs and the
   request func_80049AC4 posts (mode 2) is never served.

   mode (+0) 0: nothing. Otherwise dst = the word at +8, and
     - mode 1: a copy, `rows` (+0x10) rows of `cols` (+0xC) words from the
       address at +4 to dst. Both pointers then skip the rest of a
       0x20-word row, so the row pitch is 0x20 words on both sides; a row
       wider than 0x20 words skips nothing. cols and rows are re-read from
       the request on every pass and compared unsigned;
     - any other mode: 0x400 words at dst are filled with the WORD at +4
       (here a value, not an address);
   then mode = 0: the request is done. No calls, no RNG, no frame wait.

   Shape:
   - the request is the struct, used member by member with no pointer
     local: the address is formed once from the mode read's `lui`, copied
     for the row loop and formed again inside it for the column loop.
   - `j` is the row counter in mode 1 and the fill word otherwise: the
     target keeps both in $a2. With a variable of its own the fill word
     takes $a0 (53/55).
   - the skip is `for (; i < 0x20; i++, dst++, src++)`: the target steps
     i first, then dst, then src (the i step sits in the branch's delay
     slot with a `- 1` after the loop). With the two pointer steps in the
     body the order is dst, src, i (52/55). */
/* Ruling of 2026-10-14 (docs/naming.md): Only code that nothing reaches calls this function; named for what it does. */
/* func_80054B30 */
void mem_request_run(void)
{
    u32 i;
    u32 j;
    s32 *src;
    s32 *dst;

    if (g_MemRequest.kind != 0) {
        dst = (s32 *)g_MemRequest.destination;
        if (g_MemRequest.kind == 1) {
            src = (s32 *)g_MemRequest.source_fill.source;
            for (j = 0; j < g_MemRequest.rows; j++) {
                for (i = 0; i < g_MemRequest.row_words; i++) {
                    *dst++ = *src++;
                }
                for (; i < 0x20; i++, dst++, src++) {
                }
            }
        } else {
            j = g_MemRequest.source_fill.source;
            for (i = 0; i < 0x400; i++) {
                *dst++ = j;
            }
        }
        g_MemRequest.kind = 0;
    }
}
