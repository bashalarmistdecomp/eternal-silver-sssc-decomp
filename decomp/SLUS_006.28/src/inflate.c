/* Lunar SSSC — inflate: the unpackers: gzip 1.2.4's inflate
   (inflate_buffer, the dynamic block, huft_build, the pool, inflate_codes)
   and lz_unpack, the other unpacker, lone at its end. The first split had
   it in io.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
s32 inflate_buffer(s32 source, s32 destination);  /* TU defines this below */
extern s32 *g_HuftPoolNext;
extern u8 g_HuftPool[];
extern s32 g_Hufts;
extern s32 g_Wp;
extern s32 g_Bb;
extern s32 g_Bk;
extern s32 g_InflateWindow;
extern u16 *g_InflateInput;
s32 inflate_block_dynamic();
s32 huft_pool_reset(void *t);
extern long HWD0, VWD0;
extern long HWD0;
extern long VWD0;
extern long HWD0, VWD0;
s32 inflate_block_dynamic();
s32 inflate_buffer(s32 source, s32 destination);
s32 huft_pool_reset(void *t);
s32 inflate_block_dynamic();
s32 inflate_buffer(s32 source, s32 destination);
s32 huft_pool_reset(void *t);

/* One inflate decode-table entry (the 8-byte struct huft of Mark Adler's
 * inflate): e = extra bits or operation (16 + bits = sub-table link,
 * 15/16 = literal/end-of-block, 99 = invalid), b = bits in this code or
 * sub-code, v = the base value or the sub-table pointer. */

s32 inflate_block_dynamic();                                /* extern */

/* func_80021D6C */
s32 inflate_buffer(s32 source, s32 destination) {
    g_Bb = 0;
    g_Bk = 0;
    g_Wp = 0;
    g_HuftPoolNext = (s32 *) g_HuftPool;
    g_InflateInput = (u16 *) source;
    g_InflateWindow = destination;
loop_1:
    if (inflate_block_dynamic() == 0) {
        g_Hufts = 0;
        goto loop_1;
    }
    return (g_InflateWindow - destination) + g_Wp;
}
extern s32 g_Lbits;
extern s32 g_Dbits;
extern u8 g_Border[];
extern u16 g_MaskBits[];
extern u16 g_Cplens[];
extern u16 g_Cplext[];
extern u16 g_Cpdist[];
extern u16 g_Cpdext[];
s32 huft_build(u32 *b, u32 n, u32 s, u16 *d, u16 *e, Huft **t, s32 *m);  /* TU defines this below */
s32 inflate_codes(Huft *tl, Huft *td, s32 bl, s32 bd);

/* One deflate block of the 16-bit-word inflate: gzip inflate.c's
 * inflate_block() and inflate_dynamic() in one function. b/k are the bit
 * buffer and its count (bb/bk = g_Bb/g_Bk); input comes 16 bits
 * at a time from the u16 pointer g_InflateInput, so each refill is a single
 * `if` (gzip's NEEDBITS loop) and the refill/drop pairs below are its
 * NEEDBITS(n)/DUMPBITS(n) macros written out. It reads the last-block bit,
 * DROPS the two block-type bits unread (every block is taken as dynamic
 * Huffman: there is no stored or fixed case), then the 5/5/4-bit table
 * sizes, the bit-length code lengths in border[] order (g_Border), and
 * the literal/length + distance code lengths through that 7-bit table
 * (16 = repeat last 3..6, 17 = 3..10 zeros, 18 = 11..138 zeros). gzip's
 * range checks (nl > 286, i + j > n) and every huft_build / inflate_codes
 * result test are absent. It builds the two tables with lbits/dbits
 * (g_Lbits/g_Dbits) lookup bits and runs inflate_codes
 * (inflate_codes). Returns the last-block bit. */
/* func_80021DD8 */
s32 inflate_block_dynamic(void)
{
    s32 i;
    u32 j;
    u32 l;
    u32 m;
    u32 n;
    Huft *tl;
    Huft *td;
    s32 bl;
    s32 bd;
    u32 nb;
    u32 nl;
    u32 nd;
    u32 ll[286 + 30];
    u32 b;
    u32 k;
    s32 e;

    b = g_Bb;
    k = g_Bk;
    if (k < 1) {
        b |= (u32) *g_InflateInput++ << k;
        k += 16;
    }
    e = (s32)b & 1;
    b >>= 1;
    k -= 1;
    if (k < 2) {
        b |= (u32) *g_InflateInput++ << k;
        k += 16;
    }
    b >>= 2;
    k -= 2;
    if (k < 5) {
        b |= (u32) *g_InflateInput++ << k;
        k += 16;
    }
    nl = 257 + (b & 0x1f);
    b >>= 5;
    k -= 5;
    if (k < 5) {
        b |= (u32) *g_InflateInput++ << k;
        k += 16;
    }
    nd = 1 + (b & 0x1f);
    b >>= 5;
    k -= 5;
    if (k < 4) {
        b |= (u32) *g_InflateInput++ << k;
        k += 16;
    }
    nb = 4 + (b & 0xf);
    b >>= 4;
    k -= 4;
    for (j = 0; j < nb; j++) {
        if (k < 3) {
            b |= (u32) *g_InflateInput++ << k;
            k += 16;
        }
        ll[g_Border[j]] = b & 7;
        b >>= 3;
        k -= 3;
    }
    for (; j < 19; j++)
        ll[g_Border[j]] = 0;
    bl = 7;
    huft_build(ll, 19, 19, 0, 0, &tl, &bl);
    n = nl + nd;
    m = g_MaskBits[bl];
    i = l = 0;
    while ((u32)i < n) {
        if (k < (u32)bl) {
            b |= (u32) *g_InflateInput++ << k;
            k += 16;
        }
        j = (td = tl + (b & m))->b;
        b >>= j;
        k -= j;
        j = td->v.n;
        if (j < 16)
            ll[i++] = l = j;
        else if (j == 16) {
            if (k < 2) {
                b |= (u32) *g_InflateInput++ << k;
                k += 16;
            }
            j = 3 + (b & 3);
            b >>= 2;
            k -= 2;
            while (j--)
                ll[i++] = l;
        } else if (j == 17) {
            if (k < 3) {
                b |= (u32) *g_InflateInput++ << k;
                k += 16;
            }
            j = 3 + (b & 7);
            b >>= 3;
            k -= 3;
            while (j--)
                ll[i++] = 0;
            l = 0;
        } else {
            if (k < 7) {
                b |= (u32) *g_InflateInput++ << k;
                k += 16;
            }
            j = 11 + (b & 0x7f);
            b >>= 7;
            k -= 7;
            while (j--)
                ll[i++] = 0;
            l = 0;
        }
    }
    huft_pool_reset(tl);
    g_Bb = b;
    g_Bk = k;
    bl = g_Lbits;
    huft_build(ll, nl, 257, g_Cplens, g_Cplext, &tl, &bl);
    bd = g_Dbits;
    huft_build(ll + nl, nd, 0, g_Cpdist, g_Cpdext, &td, &bd);
    inflate_codes(tl, td, bl, bd);
    huft_pool_reset(tl);
    huft_pool_reset(td);
    return e;
}

/* huft_build — the inflate Huffman table builder (gzip/unzip
 * inflate.c), called by the 16-bit-word inflate inflate_block_dynamic. From the
 * code lengths b[0..n-1] it builds a multi-level decode table: codes
 * 0..s-1 are simple values (<256 literal, 256 end of block), the rest map
 * through the base list d[] and extra-bit list e[]. Tables come from a
 * bump allocator instead of malloc — g_HuftPoolNext advances by z + 1 entries
 * per table and g_Hufts counts the entries (gzip's `hufts`). Each table
 * is preceded by a link entry chained through *t. *m is the requested
 * first-level bits on entry and the actual bits on exit. Returns 1 for an
 * incomplete code set whose longest code is over one bit, else 0; an
 * all-zero length set returns 0 with *t = 0 and *m = 0. The table arena
 * base is reset to g_HuftPool by inflate_buffer / huft_pool_reset. */
/* func_800222C4 */
s32 huft_build(u32 *b, u32 n, u32 s, u16 *d, u16 *e, Huft **t, s32 *m)
{
  u32 x[17];
  u32 c[17];
  u32 v[288];
  Huft r;
  Huft *u[16];
  u32 a;
  u32 f;
  s32 g;
  s32 h;
  u32 i;
  u32 j;
  s32 k;
  s32 l;
  u32 *p;
  Huft *q;
  s32 w;
  u32 *xp;
  s32 y;
  u32 z;

  {
    u32 *cp = c;
    s32 cn = 17;
    do
    {
      *cp++ = 0;
    }
    while (--cn);
  }
  p = b;
  i = n;
  do
  {
    c[*p]++;
    p++;
  }
  while (--i);
  if (c[0] == n)
  {
    *t = 0;
    *m = 0;
    return 0;
  }
  l = *m;
  for (j = 1; j <= 16; j++)
  {
    if (c[j])
    {
      break;
    }
  }
  k = j;
  if ((u32) l < j)
  {
    l = j;
  }
  for (i = 16; i; i--)
  {
    if (c[i])
    {
      break;
    }
  }
  g = i;
  if ((u32) l > i)
  {
    l = i;
  }
  *m = l;
  for (y = 1 << j; j < i; j++, y <<= 1)
  {
    y -= c[j];
  }
  y -= c[i];
  c[i] += y;
  x[1] = j = 0;
  p = c + 1;
  xp = x + 2;
  while (--i)
  {
    *xp++ = (j += *p++);
  }
  p = b;
  i = 0;
  do
  {
    if ((j = *p++) != 0)
    {
      v[x[j]++] = i;
    }
  }
  while (++i < n);
  x[0] = i = 0;
  p = v;
  h = -1;
  w = -l;
  u[0] = 0;
  q = 0;
  z = 0;
  for (; k <= g; k++)
  {
    a = c[k];
    while (a--)
    {
      while (k > w + l)
      {
        h++;
        w += l;
        z = (z = g - w) > (u32) l ? l : z;
        if ((f = 1 << (j = k - w)) > a + 1)
        {
          f -= a + 1;
          xp = c + k;
          while (++j < z)
          {
            if ((f <<= 1) <= *++xp)
            {
              break;
            }
            f -= *xp;
          }
        }
        z = 1 << j;
        {
          Huft *hp = (Huft *) g_HuftPoolNext;
          g_HuftPoolNext = (s32 *) (hp + (s32) (z + 1));
          q = hp;
        }
        g_Hufts += z + 1;
        *t = q + 1;
        *(t = &q->v.t) = 0;
        u[h] = ++q;
        if (h)
        {
          x[h] = i;
          r.b = l;
          r.e = 16 + j;
          r.v.t = q;
          j = i >> (w - l);
          u[h - 1][j] = r;
        }
      }
      r.b = k - w;
      if (p >= v + n)
      {
        r.e = 99;
      }
      else if (*p < s)
      {
        r.e = (*p < 256) ? 16 : 15;
        r.v.n = *p;
        p++;
      }
      else
      {
        r.e = e[*p - s];
        r.v.n = d[*p++ - s];
      }
      f = 1 << (k - w);
      for (j = i >> w; j < z; j += f)
      {
        q[j] = r;
      }
      for (j = 1 << (k - 1); i & j; j >>= 1)
      {
        i ^= j;
      }
      i ^= j;
      while ((i & ((1 << w) - 1)) != x[h])
      {
        h--;
        w -= l;
      }
    }
  }
  return (y != 0) && (g != 1);
}

/* func_8002282C */
s32 huft_pool_reset(void *t) {
    g_HuftPoolNext = (s32 *)g_HuftPool;
    return 0;
}
static inline void inflate_copy(u8 *dst, u8 *src, u32 n)
{
  /* The inflate's memcpy(): gzip's inflate_codes calls the C library
   * here; this build carries its own byte loop, expanded inline at both
   * call sites in inflate_codes. */
  while (n != 0) {
    *dst++ = *src++;
    n--;
  }
}
/* inflate_codes — the Huffman-coded body of one deflate block (gzip/unzip
 * inflate.c), called by the 16-bit-word inflate inflate_block_dynamic with the
 * literal/length table tl (bl lookup bits) and the distance table td (bd
 * bits). b/k are the bit buffer and its count (bb/bk = g_Bb /
 * g_Bk), refilled 16 bits at a time from the u16 pointer
 * g_InflateInput; w is the write index (wp = g_Wp) into the current
 * 32 KiB output block g_InflateWindow (gzip's `slide`).
 *
 * Per symbol: look up b & mask_bits[bl] in tl and follow sub-table links
 * while e > 16; e == 16 is a literal byte, e == 15 the end of the block,
 * anything else a length with e extra bits, followed by a distance
 * decoded the same way through td. gzip's `e == 99` invalid-code exits
 * are absent, so the function always returns 0.
 *
 * What differs from gzip is the window. There is none: the output goes
 * straight into the destination buffer, and "flushing" at w == 0x8000
 * only records wp and moves g_InflateWindow on by 0x8000. A match is copied in
 * runs of e = min(n, 0x8000 - max(d, w)) with d masked to 15 bits, so a
 * source index ABOVE w lies in the previous 32 KiB block and is read from
 * g_InflateWindow + d - 0x8000 (gzip reads the same index from its circular
 * window). Overlapping matches (w - d < e) go byte by byte. */
/* func_80022840 */
s32 inflate_codes(Huft *tl, Huft *td, s32 bl, s32 bd)
{
  u32 e;
  u32 n, d;
  u32 w;
  Huft *t;
  u32 ml, md;
  u32 b;
  u32 k;

#define WSIZE 0x8000
#define NEEDBITS(n) if (k < (n)) { b |= (u32) *g_InflateInput++ << k; k += 16; }
#define DUMPBITS(n) { b >>= (n); k -= (n); }
#define FLUSH(w) { g_Wp = (w); g_InflateWindow += g_Wp; }

  b = g_Bb;
  k = g_Bk;
  w = g_Wp;

  ml = g_MaskBits[bl];
  md = g_MaskBits[bd];
  for (;;) {
    NEEDBITS((u32) bl)
    if ((e = (t = tl + (b & ml))->e) > 16)
      do {
        DUMPBITS(t->b)
        e -= 16;
        NEEDBITS(e)
      } while ((e = (t = t->v.t + (b & g_MaskBits[e]))->e) > 16);
    DUMPBITS(t->b)
    if (e == 16) {
      ((u8 *) g_InflateWindow)[w++] = (u8) t->v.n;
      if (w == WSIZE) {
        FLUSH(w)
        w = 0;
      }
    } else {
      if (e == 15)
        break;
      NEEDBITS(e)
      n = t->v.n + (b & g_MaskBits[e]);
      DUMPBITS(e)
      NEEDBITS((u32) bd)
      if ((e = (t = td + (b & md))->e) > 16)
        do {
          DUMPBITS(t->b)
          e -= 16;
          NEEDBITS(e)
        } while ((e = (t = t->v.t + (b & g_MaskBits[e]))->e) > 16);
      DUMPBITS(t->b)
      NEEDBITS(e)
      d = w - t->v.n - (b & g_MaskBits[e]);
      DUMPBITS(e)
      do {
        n -= (e = (e = WSIZE - ((d &= WSIZE - 1) > w ? d : w)) > n ? n : e);
        if (w - d >= e) {
          if ((s32) (w - d) >= 0)
            inflate_copy((u8 *) g_InflateWindow + w, (u8 *) g_InflateWindow + d, e);
          else
            inflate_copy((u8 *) g_InflateWindow + w, (u8 *) g_InflateWindow + d - WSIZE, e);
          w += e;
          d += e;
        } else
          do {
            ((u8 *) g_InflateWindow)[w++] = ((u8 *) g_InflateWindow)[d++];
          } while (--e);
        if (w == WSIZE) {
          FLUSH(w)
          w = 0;
        }
      } while (n);
    }
  }

  g_Wp = w;
  g_Bb = b;
  g_Bk = k;
  return 0;

#undef WSIZE
#undef NEEDBITS
#undef DUMPBITS
#undef FLUSH
}
/* lz_unpack -- the LZSS unpacker of the tile maps' blocks and the menu
   icons: unpacks the stream at src to dst and returns the number of bytes
   written. Five callers in the image: bg_block_load, bg_tile_set,
   bg_attribute_block_load, bg_attribute_set (a map block: 64 or 256 u16)
   and IconImageLoad (an icon); none reads the result.

   The stream is a bit stream of flags, LSB first, refilled one byte at a
   time from the same pointer the data bytes come from:
     1      literal: one byte copied.
     0 1    short match: one byte b. length = (b >> 6) + 2 (2..5),
            distance = b | 0xFFFFFFC0, i.e. -64..-1.
     0 0    long match: two bytes, big-endian w. distance = w | 0xFFFFF000
            (-4096..-1). length = (w >> 12) + 2 (3..17) when the top
            nibble is not 0; when it is 0 the next byte n gives n + 2
            (3..257), and n == 0 ENDS the stream.
   A match copies length bytes from dst + written + distance, a byte at a
   time and forwards, so a source that overlaps the bytes being written
   repeats them. There is no window buffer: references go straight into
   the output, and one that reaches before dst reads whatever memory lies
   there (nothing zero-fills). Nothing bounds the input or the output --
   the end marker is the only stop -- so the output size is the stream's
   own (0x80 bytes for a primary map block, 0x200 for an attribute block).
   A file reader of the same format (one was written for station-tools)
   differs only where it must: it stops at the end of its input as well,
   and a reference before the start yields zeros.

   Shape. The three copy loops are written out, one per kind of match: the
   target has three (cross-jumping cannot merge them -- each ends on a
   branch to its own head). `off` is UNSIGNED: both shifts are srl (as an
   int they are sra, 86/88). `flags` is a signed int (sra). The match
   source is spelled dst + (pos + off), not out + off: the target adds pos
   and off first and then the base. The bit read is spelled out twice;
   reorg copies the `count--` at each join into the branch's delay slot,
   which is why `li 8` is followed by its own decrement. */
/* func_80022BF8 */
s32 lz_unpack(u8 *src, u8 *dst)
{
    s32 count;
    s32 flags;
    s32 pos;
    u8 *out;
    u8 *p;
    s32 len;
    u32 off;
    s32 bit;

    count = 0;
    flags = 0;
    pos = 0;
    out = dst;
    for (;;) {
        if (count == 0) {
            flags = *src++;
            count = 8;
        }
        count--;
        bit = flags & 1;
        flags >>= 1;
        if (bit) {
            *out++ = *src++;
            pos++;
            continue;
        }
        if (count == 0) {
            flags = *src++;
            count = 8;
        }
        count--;
        bit = flags & 1;
        flags >>= 1;
        if (bit) {
            off = *src++;
            len = (off >> 6) + 2;
            off |= -0x40;
            p = dst + (pos + off);
            while (len != 0) {
                *out++ = *p++;
                pos++;
                len--;
            }
            continue;
        }
        off = *src++ << 8;
        off |= *src++;
        len = off >> 12;
        off |= -0x1000;
        if (len != 0) {
            len += 2;
            p = dst + (pos + off);
            while (len != 0) {
                *out++ = *p++;
                pos++;
                len--;
            }
            continue;
        }
        len = *src++;
        if (len == 0) {
            return pos;
        }
        len += 2;
        p = dst + (pos + off);
        while (len != 0) {
            *out++ = *p++;
            pos++;
            len--;
        }
    }
}
