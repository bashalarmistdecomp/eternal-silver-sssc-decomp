/* PSY-Q 4.5 libcd, member s_002.o: CdIntToPos, CdPosToInt. The first split
   had it in main_ag.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 temp_a1;
extern u32 temp_v1;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80129200 */
CdlLOC *CdIntToPos(s32 i, CdlLOC *p) {
    s32 m, s, f;

    i += 150;
    s = i / 75;
    f = i - (s * 75);
    m = s / 60;
    s = s - (m * 60);
    p->sector = (u8) (((f / 10) * 16) + (f % 10));
    p->second = (u8) (((s / 10) * 16) + (s % 10));
    p->minute = (u8) (((m / 10) * 16) + (m % 10));
    return p;
}

/* func_80129304 */
int CdPosToInt(CdlLOC *arg0) {
    u8 temp_a1;
    u8 temp_a2;
    u8 temp_v1;

    temp_v1 = (*(s32 *)((u8 *)arg0 + 0x0));
    temp_a2 = (*(u8 *)((u8 *)arg0 + 0x1));
    temp_a1 = (*(s16 *)((u8 *)arg0 + 0x2));
    return (((((((temp_v1 >> 4) * 0xA) + (temp_v1 & 0xF)) * 0x3C) + (((temp_a2 >> 4) * 0xA) + (temp_a2 & 0xF))) * 0x4B) + (((temp_a1 >> 4) * 0xA) + (temp_a1 & 0xF))) - 0x96;
}

INCLUDE_PAD(12);
