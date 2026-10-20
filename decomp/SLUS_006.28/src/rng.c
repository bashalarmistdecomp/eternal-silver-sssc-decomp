/* Lunar SSSC — rng: the random generator: the 55-word table's seed and
   refill, the next number and the draws. The first split had it in ui.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
extern s32 g_RngNumbers[56];
extern s32 g_RngCursor;
s32 rng_draw(void);
s32 rng_draw_copy(void);

/* rng_refill -- the REFILL of the random number generator. The generator
   is the subtractive one docs/spec/code/subsystems/menu-ui.md describes,
   X[n] = (X[n-55] - X[n-24]) mod 10^9; g_RngNumbers is its table ia[56]
   (words 1..55 are the state, word 0 is never touched; the next symbol,
   g_TileWaveRowShifts, starts at +0xE0). This replaces all 55 words, in place, with
   the next 55 numbers of the sequence:

     for i = 1..24:    ia[i] = ia[i] - ia[i + 31]      + 10^9 when negative
     for i = 25..55:   ia[i] = ia[i] - ia[i - 24]      + 10^9 when negative

   The second loop reads words 1..31, which the first loop and its own
   earlier passes have already replaced: that is what makes all 55 steps
   the same recurrence. A word that was in 0 .. 10^9 - 1 stays there.

   Callers, all three in this TU: rng_seed (the seeding: three
   refills to warm the table up), and the two draws -- rng_next, whose
   result rng_draw (the raw draw of the battle RNG) returns, and
   rng_below -- each of which steps the cursor g_RngCursor and calls
   this when it has gone past word 55, so once every 55 draws. It reads
   and writes the table only; the cursor is the callers' business. A leaf:
   no callee, no frame wait, no arguments, no return value (no caller
   reads $v0).

   Shapes the bytes depend on (each measured):
   - the table is an ARRAY of words indexed directly, with no pointer
     local. The target keeps ia[i] as one walking pointer but adds a
     walked byte offset to the table's address inside the loop for the
     other operand (`addu v0, a2, a3` with a2 starting at 0x80, then at
     4), and loads the address and 10^9 afresh before each loop. Under
     `extern u8 g_RngNumbers[]` with `((s32 *) g_RngNumbers)[i]` both
     operands become walking pointers (26/44); an `s32 *` local set
     from the array before the loops is 24/44.
   - the difference goes through the local j and is stored once.
     `ia[i] -= ia[i + 31]; if (ia[i] < 0) ia[i] += 1000000000;` is 34/44.
   Not decided by the bytes: `i <= 24` / `i <= 55` for the bounds, the
   element type's sign (u32 words match too, j stays signed), a sized or
   an incomplete extern, and `s32` with no return statement for `void`. */
/* func_8001C548 */
void rng_refill(void)
{
    s32 i;
    s32 j;

    for (i = 1; i < 25; i++)
    {
        j = g_RngNumbers[i] - g_RngNumbers[i + 31];
        if (j < 0)
        {
            j += 1000000000;
        }
        g_RngNumbers[i] = j;
    }
    for (i = 25; i < 56; i++)
    {
        j = g_RngNumbers[i] - g_RngNumbers[i - 24];
        if (j < 0)
        {
            j += 1000000000;
        }
        g_RngNumbers[i] = j;
    }
}

/* func_8001C5F8 */
void rng_seed(s32 seed)
{
  s32 i;
  s32 ii;
  s32 k;
  g_RngNumbers[55] = seed;
  k = 1;
  for (i = 1; i < 55; i++)
  {
    ii = (i * 21) % 55;
    g_RngNumbers[ii] = k;
    k = seed - k;
    if (k < 0)
    {
      k += 1000000000;
    }
    seed = g_RngNumbers[ii];
  }
  rng_refill();
  rng_refill();
  rng_refill();
  g_RngCursor = 55;
}

/* func_8001C6B0 */
s32 rng_next(void) {
    s32 next_cursor;

    next_cursor = g_RngCursor + 1;
    g_RngCursor = next_cursor;
    if (next_cursor >= 0x38) {
        rng_refill();
        g_RngCursor = 1;
    }
    return g_RngNumbers[g_RngCursor];
}

/* func_8001C704 */
s32 rng_below(s32 bound) {
    s32 next_cursor;

    next_cursor = g_RngCursor + 1;
    g_RngCursor = next_cursor;
    if (next_cursor >= 0x38) {
        rng_refill();
        g_RngCursor = 1;
    }
    return (s32) (bound * (u16) g_RngNumbers[g_RngCursor]) >> 0x10;
}

/* RNG — map_look.c calls this for its result (declared s32 there),
   so the return value is real even though the body is a pure thunk. */
/* func_8001C770 */
s32 rng_draw(void) {
    return rng_next();
}
/* Ruling of 2026-10-14 (docs/naming.md): Does what rng_draw does; nothing read says why there are two. */
/* func_8001C790 */
s32 rng_draw_copy(void) {
    rng_next();
}
