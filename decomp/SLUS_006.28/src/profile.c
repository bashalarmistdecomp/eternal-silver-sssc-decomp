/* Lunar SSSC — profile: the frame's time profile: marks added and read, the
   start stamp and the draw. The first split had it in cdrom_load.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"

extern u8 g_ProfileMarkTimes[];
extern s32 D_800B8AE8;
extern s32 g_ProfileStart;
void profile_start_set(void);  /* TU defines this below */
s32 profile_mark_get(s32 mark);
void frame_start();
s32 xmalloc_heap_reset();
s32 screen_on_wait();
s32 screen_off_wait();
s32 func_80026280();
s32 MapFrameEndCopy();
void func_80025FE0(void);
s32 profile_mark_get(s32 mark);
void frame_start();
s32 xmalloc_heap_reset();
s32 screen_on_wait();
s32 screen_off_wait();
s32 func_80026280();
s32 MapFrameEndCopy();
void func_80025FE0(void);
/* far_syms: g_PacketWritePointer g_PacketOtBase (target reaches them %hi/%lo) */
extern u8 *g_PacketWritePointer;
extern u32 *g_PacketOtBase;
/* far_syms: g_PacketWritePointer */
/* far_syms: g_PacketWritePointer g_PacketOtBase */
void AddPrim(void *ot, void *p);

/* far_syms: g_ProfileMarkTimes g_ProfileMarkCols */
extern s32 g_ProfileMarkCount;
extern u8 g_ProfileMarkCols[];

/* Record one profiler mark.

   g_ProfileMarkCount counts the marks taken this frame (at most 0x20).  Mark n
   stores, in two parallel word tables, the time since the frame's start
   (GsGetVcount() - g_ProfileStart - 10; profile_start_set stamps g_ProfileStart)
   in g_ProfileMarkTimes[n] and the caller's word in g_ProfileMarkCols[n] — every caller
   passes a 0x21BBGGRR colour word, the bar's colour.  Returns the stored
   time; when the table is full it records nothing and returns no value. */
/* func_80025DF4 */
s32 profile_mark_add(s32 col)
{
    s32 n;

    n = g_ProfileMarkCount;
    if (n < 0x20) {
        ((s32 *)g_ProfileMarkTimes)[n] = GsGetVcount() - g_ProfileStart - 0xA;
        ((s32 *)g_ProfileMarkCols)[n] = col;
        g_ProfileMarkCount++;
        return ((s32 *)g_ProfileMarkTimes)[n];
    }
}

/* far_syms: g_ProfileMarkTimes (target reaches it %hi/%lo) */

/* func_80025E70 */
s32 profile_mark_get(s32 mark)
{
    return ((s32 *)g_ProfileMarkTimes)[mark];
}

/* func_80025E8C */
void profile_start_set(void) {
    g_ProfileStart = GsGetVcount();
}
extern s32 g_ScreenWidth;
void SetPolyF4(POLY_F4 *p);

/* func_80025EAC */
void profile_draw(void)
{
    POLY_F4 *p;
    s32 x;
    s32 top;
    s32 y;
    s32 i;

    p = (POLY_F4 *)g_PacketWritePointer;
    x = g_ScreenWidth - 8;
    top = 0;
    for (i = 0; i < g_ProfileMarkCount; i++) {
        y = ((s32 *)g_ProfileMarkTimes)[i] % 240;
        *(s32 *)&p->r0 = ((s32 *)g_ProfileMarkCols)[i];
        SetPolyF4(p);
        p->x0 = x;
        p->y0 = top;
        p->x1 = x + 8;
        p->y1 = top;
        p->x2 = x;
        p->y2 = y;
        p->x3 = x + 8;
        p->y3 = y;
        top = y;
        x -= 8;
        AddPrim(g_PacketOtBase, p);
        p++;
    }
    g_PacketWritePointer = (u8 *)p;
}

/* far_syms: D_800B8AE8 (target reaches it %hi/%lo) */

void frame_start();                                  /* extern */
s32 xmalloc_heap_reset();                                  /* extern */
s32 screen_on_wait();                                  /* extern */
s32 screen_off_wait();                                  /* extern */
s32 func_80026280();                                  /* extern */
s32 MapFrameEndCopy();                                  /* extern */

/* Ruling of 2026-10-14 (docs/naming.md): Reached, and does work: eight statements with no one purpose found. */
void func_80025FE0(void) {
    func_80026280();
    xmalloc_heap_reset();
    *(s8 *)0x800B89FA = 0;
    D_800B8AE8 = 1;
    screen_off_wait();
    frame_start();
    MapFrameEndCopy();
    screen_on_wait();
}
