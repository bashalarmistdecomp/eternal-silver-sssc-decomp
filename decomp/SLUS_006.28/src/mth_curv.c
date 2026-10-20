/* Lunar SSSC — mth_curv: the math: the square root and hypotenuse, the
   splines, the curve's points and the 16.16 fixed-point helpers. The name
   is the developers' own: MTH_CURV.C, the source file MTH_Div's assertion
   names (and its message names MTH_Div itself); the file's other functions
   have it by being of its group. The first split had it in ui.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
extern s32 g_MthH;
extern s32 g_MthD;
extern s32 g_MthX;
extern s32 g_MthY;
extern s32 D_800B8990;
extern s32 g_MthP;
extern s32 g_MthA;
extern s32 g_MthB;
extern s32 D_800B89A0;
u32 MTH_CurveCountsCheck(u32 n, u32 count);
s32 MTH_FixedFromInteger(s32 whole);
s32 MTH_FixedToInteger(s32 fixed);
void assert_fail();  /* extern */

u32 MTH_CurveCountsCheck(u32 n, u32 count);  /* TU defines this below */
s32 MTH_FixedFromInteger(s32 whole);  /* TU defines this below */
s32 MTH_FixedToInteger(s32 fixed);  /* TU defines this below */
s32 MTH_Div(s32, s32);

/* MTH_mysqrt -- Haruhiko Okumura's mysqrt(x), from sqrt.c of his dictionary
   of algorithms in C, in 16.16 fixed point: the square root by Newton's
   iteration. What this copy changes: the division is MTH_Div, and a
   negative argument returns -1.0 where the book prints an error and
   returns 0.
   The name is the book's behind the developers' MTH_; docs/naming.md,
   "Haruhiko Okumura's routines", has the comparison. */
/* func_8001C7B0 */
s32 MTH_mysqrt(s32 x)
{
  s32 last_root;
  s32 root;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `one` only holds the constant 0x10000; behaviour is the same with the constant
     written where it is read, but then 3 of this function's 34 instruction lines differ. */
  int one;
  s32 result;
  if (x > 0)
  {
    one = 0x10000;
    root = x;
    if (x < one)
    {
      root = 0x10000;
    }
    do
    {
      last_root = root;
      root = ((s32) (MTH_Div(x, root) + last_root)) / 2;
    }
    while (root < last_root);
    return last_root;
  }
  result = -0x10000;
  if (x == 0)
  {
    result = 0;
  }
  return result;
}
s32 MTH_Mul(s32, s32);

/* func_8001C838 */
s32 MTH_Hypot(s32 x, s32 y)
{
  s32 kept_scaled_y;
  s32 one;
  s32 scale_then_x_squared;
  s32 x_squared;
  s32 kept_scaled_x;
  s32 scaled_y;
  s32 reciprocal;
  s32 scale;
  s32 larger_magnitude;
  s32 divisor;
  s32 x_magnitude;
  s32 y_magnitude;
  s32 x_larger;
  x_magnitude = x;
  y_magnitude = y;
  if (x_magnitude < 0)
  {
    x_magnitude = -x_magnitude;
  }
  if (y_magnitude < 0)
  {
    y_magnitude = -y_magnitude;
  }
  one = 1;
  x_larger = y_magnitude <= (x_magnitude - one);
  if ((x_magnitude > 0x10000) || ((x_larger, (y_magnitude > 0x10000) != 0)))
  {
    larger_magnitude = y_magnitude;
    if ((y_magnitude < x_magnitude) != 0)
    {
      larger_magnitude = x_magnitude;
    }
    scale = MTH_FixedToInteger(larger_magnitude);
    scaled_y = y_magnitude / scale;
    x_magnitude = x_magnitude / scale;
    kept_scaled_y = scaled_y;
    x_squared = MTH_Mul(x_magnitude, x_magnitude);
    return MTH_mysqrt(x_squared + MTH_Mul(kept_scaled_y, kept_scaled_y)) * scale;
  }
  if ((y_magnitude < x_magnitude) != 0)
  {
    divisor = x_magnitude;
    reciprocal = MTH_Div(0x10000, divisor);
  }
  else
  {
    divisor = y_magnitude;
    reciprocal = MTH_Div(0x10000, divisor);
  }
  scale = reciprocal;
  scale_then_x_squared = reciprocal;
  x_magnitude = MTH_Mul(x_magnitude, scale);
  kept_scaled_x = x_magnitude;
  scaled_y = MTH_Mul(y_magnitude, scale_then_x_squared);
  scale_then_x_squared = MTH_Mul(kept_scaled_x, kept_scaled_x);
  return MTH_Div(MTH_mysqrt(scale_then_x_squared + MTH_Mul(scaled_y, scaled_y)), scale);
}

/* MTH_maketable -- Haruhiko Okumura's maketable(x, y, z), from spline.c of
   his dictionary of algorithms in C, in 16.16 fixed point: it makes the
   table z of a cubic spline through the n points (x[i], y[i]). What this
   copy changes: the book's constant N is the fourth parameter, its two
   static work arrays are the arrays at g_MthH and g_MthD, and each
   multiplication or division of two doubles is MTH_Mul or MTH_Div.
   The name is the book's behind the developers' MTH_; docs/naming.md,
   "Haruhiko Okumura's routines", has the comparison. */
/* func_8001C9BC */
void MTH_maketable(s32 *t, s32 *y, s32 *m, u32 n)
{
  u32 i;
  s32 r;
  m[0] = 0;
  m[n - 1] = 0;
  for (i = 0; i < n - 1; i++)
  {
    ((s32 *) g_MthH)[i] = t[i + 1] - t[i];
    ((s32 *) g_MthD)[i + 1] = MTH_Div(y[i + 1] - y[i], ((s32 *) g_MthH)[i]);
  }
  i = 1;
  m[1] = (((s32 *) g_MthD)[2] - ((s32 *) g_MthD)[1]) - MTH_Mul(((s32 *) g_MthH)[0], m[0]);
  ((s32 *) g_MthD)[1] = (t[2] - t[0]) * 2;
  for (; i < n - 2; i++)
  {
    r = MTH_Div(((s32 *) g_MthH)[i], ((s32 *) g_MthD)[i]);
    m[i + 1] = (((s32 *) g_MthD)[i + 2] - ((s32 *) g_MthD)[i + 1]) - MTH_Mul(m[i], r);
    ((s32 *) g_MthD)[i + 1] = ((t[i + 2] - t[i]) * 2) - MTH_Mul(((s32 *) g_MthH)[i], r);
  }
  m[n - 2] -= MTH_Mul(((s32 *) g_MthH)[n - 2], m[n - 1]);
  for (i = n - 2; i != 0; i--)
  {
    m[i] = MTH_Div(m[i] - MTH_Mul(((s32 *) g_MthH)[i], m[i + 1]), ((s32 *) g_MthD)[i]);
  }
}

s32 MTH_Mul(s32, s32);

/* MTH_spline -- Haruhiko Okumura's spline(t, x, y, z), from spline.c of his
   dictionary of algorithms in C, in 16.16 fixed point: the value at t of
   the cubic spline whose table MTH_maketable made. What this copy changes:
   the book's constant N is the fifth parameter, and each multiplication or
   division of two doubles is MTH_Mul or MTH_Div.
   The name is the book's behind the developers' MTH_; docs/naming.md,
   "Haruhiko Okumura's routines", has the comparison. */
/* func_8001CC10 */
s32 MTH_spline(s32 t, s32 knots, s32 values, s32 table, s32 n)
{
  s32 slope;
  s32 interval_length;
  s32 word_offset;
  s32 table_term;
  s32 interval_start;
  u32 middle;
  u32 high;
  u32 low;
  s32 offset_in_interval;
  u32 low_plus_high;
  s32 *interval_knots;
  s32 *interval_values;
  s32 table_product;
  s32 *interval_table;
  low = 0;
  high = n - 1;
  if (high != 0)
  {
    low_plus_high = high;
    do
    {
      low_plus_high = low + high;
      middle = low_plus_high >> 1;
      if ((*((s32 *) (((2 * middle) * 2) + knots))) < t)
      {
        low = middle + 1;
      }
      else
      {
        high = middle;
      }
    }
    while (low < high);
  }
  if (low != 0)
  {
    low -= 1;
  }
  word_offset = low * 4;
  interval_knots = (void *)word_offset + knots;
  interval_table = (void *)word_offset + table;
  interval_start = interval_knots[0];
  offset_in_interval = t - interval_start;
  interval_length = interval_knots[1] - interval_start;
  table_product = MTH_Mul(MTH_Div(MTH_Mul(interval_table[1] - interval_table[0], offset_in_interval), interval_length) + (interval_table[0] * 3), offset_in_interval);
  interval_values = (void *)word_offset + values;
  table_term = table_product;
  slope = MTH_Div(interval_values[1] - interval_values[0], interval_length);
  return MTH_Mul(table_term + (slope - MTH_Mul((interval_table[0] * 2) + interval_table[1], interval_length)), offset_in_interval) + interval_values[0];
}
unsigned int MTH_CurveWorkSet(unsigned int *work, unsigned int n);  
/* MTH_maketable2 -- Haruhiko Okumura's maketable2(p, x, y, a, b), from
   spline2.c of his dictionary of algorithms in C, in 16.16 fixed point: for
   a curve through the n points (x[i], y[i]) it makes the parameter p (the
   length along the points, divided by the whole length) and the spline
   tables a and b of x and of y against p. What this copy changes: the
   book's constant N is the sixth parameter, sqrt(t1 * t1 + t2 * t2) is the
   developers' MTH_Hypot(t1, t2), and the division is MTH_Div.
   The name is the book's behind the developers' MTH_; docs/naming.md,
   "Haruhiko Okumura's routines", has the comparison. */
/* func_8001CD50 */
void MTH_maketable2(s32 knots, s32 xs, s32 ys, s32 x_table, s32 y_table, s32 n)
{
  s32 *dst;
  s32 *src1;
  s32 *src2;
  s32 *end;
  s32 *p;
  u32 i;
  dst = (s32 *) knots;
  src1 = (s32 *) xs;
  src2 = (s32 *) ys;
  i = 1;
  *dst = 0;
  if (i < ((u32) n))
  {
    dst++;
    src2++;
    src1++;
    while (i < ((u32) n))
    {
      *dst = dst[-1] + MTH_Hypot((*src1) - src1[-1], (*src2) - src2[-1]);
      dst++;
      src2++;
      src1++;
      i++;
    }

  }
  i = 1;
  if (i < ((u32) n))
  {
    end = ((s32 *) knots) - (-n);
    p = ((s32 *) knots) + 1;
    while (i < ((u32) n))
    {
      *p = MTH_Div(*p, end[-1]);
      p++;
      i++;
    }

  }
  MTH_maketable(knots, xs, x_table, n);
  MTH_maketable(knots, ys, y_table, n);
}
/* MTH_spline2 -- Haruhiko Okumura's spline2(t, px, py, p, x, y, a, b), from
   spline2.c of his dictionary of algorithms in C: the point of the curve at
   parameter t, x through px and y through py, by two calls of MTH_spline.
   What this copy changes: the book's constant N is the ninth parameter.
   The name is the book's behind the developers' MTH_; docs/naming.md,
   "Haruhiko Okumura's routines", has the comparison. */
/* func_8001CE7C */
void MTH_spline2(s32 t, s32 *outx, s32 *outy, s32 knots, s32 xs, s32 ys, s32 x2, s32 y2, s32 n)
{
  *outx = MTH_spline(t, knots, xs, x2, n);
  *outy = MTH_spline(t, knots, ys, y2, n);
}

/* func_8001CF1C */
u32 MTH_CurveCountsCheck(u32 n, u32 count) {
    if ((n < 3U) || (count < 2U)) {
        return 0U;
    }
    return count;
}

/* func_8001CF40 */
unsigned int MTH_CurveWorkSet(unsigned int *work, unsigned int n)
{
    unsigned int *p;

    p = work;

    if (p[0] < n)
        return 0;

    g_MthH = p[1];
    g_MthD = p[2];
    g_MthX = p[3];
    g_MthY = p[4];
    D_800B8990 = p[5];
    g_MthP = p[6];
    g_MthA = p[7];
    g_MthB = p[8];
    D_800B89A0 = p[9];

    return 1;
}

/* func_8001CFB0 */
s32 MTH_CurvePointsMake(unsigned int *work, s32 *pts, u32 n, u32 count, s32 *out)
{
  u32 i;
  s32 r;
  s32 *p;
  if ((!MTH_CurveCountsCheck(n, count)) || (!MTH_CurveWorkSet(work, n)))
  {
    return 0;
  }
  i = 0;
  if (n)
  {
    do
    {
      ((s32 *) g_MthX)[i] = pts[i * 2];
      ((s32 *) g_MthY)[i] = pts[(i * 2) + 1];
      i++;
    }
    while (i < n);
  }
  MTH_maketable2(g_MthP, g_MthX, g_MthY, g_MthA, g_MthB, n);
  i = 0;
  if (count)
  {
    p = out;
    do
    {
      r = MTH_Div(MTH_FixedFromInteger(i), MTH_FixedFromInteger(count - 1));
      MTH_spline2(r, p, p + 1, g_MthP, g_MthX, g_MthY, g_MthA, g_MthB, n);
      p += 2;
      i++;
    }
    while (i < count);
  }
  return count;
}

/* func_8001D100 */
s32 MTH_FixedFromInteger(s32 whole) {
    return whole << 0x10;
}
/* func_8001D108 */
s32 MTH_FixedToInteger(s32 fixed) {
    return fixed >> 0x10;
}

/* func_8001D110 */
s32 MTH_Mul(s32 a, s32 b)
{
  long long x = a;
  long long y = b;
  return (x * y) >> 16;
}
extern u8 D_800114E8[];
extern u8 D_800114FC[];

/* MTH_Div -- the signed 16.16 fixed-point divide of MTH_CURV.C (the
   curve / spline maths: MTH_mysqrt's Newton square root, the spline set-up
   MTH_maketable / MTH_spline / MTH_maketable2, MTH_CurvePointsMake's
   t = i / (count - 1)): returns a / b as 16.16, i.e. (a << 16) / b, done
   in 32 bits without a 64-bit intermediate.

     neg = (a < 0) ^ (b < 0);  a = |a|;  b = |b|        (plain negations:
                                   0x80000000 stays 0x80000000)
     b == 0: assert_fail("MTH_Div() by zero", "MTH_CURV.C", 565) -- the
             assert hook, an EMPTY function in this image, so execution
             goes on into the divide below
     i = 0
     while (i < 16 && a <= 0x3FFFFFFF) { a <<= 1; i++; }   the 16 bits are
             taken from the dividend first, for as long as it has headroom
     while (i < 16 && b >= 2)          { b >>= 1; i++; }   the rest come
             out of the divisor, whose low bits are DROPPED (not rounded)
     r = a / b                  one signed `div`; both are non-negative
                                here, so it truncates toward zero
     if (i < 16) r <<= 16 - i   only when the divisor ran down to 1 (or
                                was 0) first; nothing checks overflow
     if (neg) r = -r            the sign goes on last: the magnitude is
                                truncated, never rounded

   So the quotient is exact (floor of |a| * 65536 / |b|) whenever the
   dividend takes all 16 shifts, i.e. |a| < 0x8000 (0.5 in 16.16); a
   larger dividend costs the divisor one low bit per missing shift, which
   can only make the result too LARGE in magnitude. a == 0 gives 0
   (sixteen shifts of zero).

   The `div` carries the assembler's two trap checks, both in the target:
   `break 7` for a zero divisor and `break 6` for 0x80000000 / -1. A zero
   divisor therefore reaches `break 7` after the empty assert hook returns.

   No RNG draw, no frame wait; reads and writes no global. The only call
   is the assert hook.

   Shapes: `i = 0` belongs to the first `for`, AFTER the assert (the
   target clears $v1 both in the delay slot of the branch round the call
   and again after the call); both loops are `for (; i < 16; i++)` with
   the other condition as a `break` inside. */
/* func_8001D168 */
s32 MTH_Div(s32 a, s32 b)
{
    s32 neg;
    s32 i;
    s32 r;

    neg = 0;
    if (a < 0)
    {
        a = -a;
        neg = 1;
    }
    if (b < 0)
    {
        b = -b;
        neg ^= 1;
    }
    if (b == 0)
    {
        assert_fail(D_800114E8, D_800114FC, 0x235);
    }
    for (i = 0; i < 16; i++)
    {
        if (a > 0x3FFFFFFF)
        {
            break;
        }
        a <<= 1;
    }
    for (; i < 16; i++)
    {
        if (b < 2)
        {
            break;
        }
        b >>= 1;
    }
    r = a / b;
    if (i < 16)
    {
        r <<= 16 - i;
    }
    if (neg)
    {
        r = -r;
    }
    return r;
}
