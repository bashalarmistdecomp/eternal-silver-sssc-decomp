/* PSY-Q 4.5 libcd, member sys.o: CdStatus, CdMode, CdLastCom, CdLastPos,
   CdReset, CdFlush, CdSetDebug, CdComstr, CdIntstr, CdSync, CdReady,
   CdSyncCallback, CdReadyCallback, CdControl, CdControlF, CdControlB,
   CdMix, CdGetSector, CdGetSector2, CdDataCallback, CdDataSync. The first
   split had it in main_aa.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern unsigned short new_var;

/* the .s slot is 0x30; the code is 0x2C (the rest is trailing padding) */

/* redistributed from the original TU (split decl repair) */
extern u8 D_80133A5C[];
s32 DMACallback(s32, s32);
void CdFlush(void);  /* TU defines this below */
extern void (*D_801336C8_s)();
extern u8 D_80133A50[];

/* func_80125170 */
s32 CdStatus(void) {
    return D_80133A50[0];
}

extern u8 D_80133A60[];

/* func_80125180 */
s32 CdMode(void) {
    return D_80133A60[0];
}

extern u8 D_80133A61[];

/* func_80125190 */
s32 CdLastCom(void) {
    return D_80133A61[0];
}

/* func_801251A0 */
CdlLOC *CdLastPos(void) {
    return (CdlLOC *)D_80133A5C;
}

/* func_801251AC */
INCLUDE_ASM("asm/nonmatchings/main", CdReset);

/* epilogue_load_slot: CdFlush */
/* func_80125218 */
void CdFlush(void) {
    CD_flush();
}

extern s32 D_80133A4C;

/* func_80125238 */
s32 CdSetDebug(s32 arg0) {
    s32 old = D_80133A4C;
    D_80133A4C = arg0;
    return old;
}

extern u8 *D_80133A68[];
extern u8 D_8010D8C0[];

/* func_8012524C */
char *CdComstr(u_char com) {
    u8 *r;
    if (com < 0x1C) {
        r = D_80133A68[com];
    } else {
        r = D_8010D8C0;
    }
    return (char *)r;
}

extern u8 *D_80133AE8[];

/* func_80125280 */
char *CdIntstr(u_char intr) {
    u8 *r;
    if (intr < 0x7) {
        r = D_80133AE8[intr];
    } else {
        r = D_8010D8C0;
    }
    return (char *)r;
}

/* epilogue_load_slot: CdSync */
/* func_801252B4 */
int CdSync(int mode, u_char *result) {
    return CD_sync(mode, result);
}

/* epilogue_load_slot: CdReady */
/* func_801252D4 */
int CdReady(int mode, u_char *result) {
    return CD_ready(mode, result);
}

extern CdlCB D_80133A44;

/* func_801252F4 */
CdlCB CdSyncCallback(CdlCB func) {
    CdlCB old = D_80133A44;
    D_80133A44 = func;
    return old;
}

extern CdlCB D_80133A48;

/* func_80125308 */
CdlCB CdReadyCallback(CdlCB func) {
    CdlCB old = D_80133A48;
    D_80133A48 = func;
    return old;
}

/* func_8012531C */
INCLUDE_ASM("asm/nonmatchings/main", CdControl);

/* func_80125458 */
INCLUDE_ASM("asm/nonmatchings/main", CdControlF);

/* func_8012558C */
INCLUDE_ASM("asm/nonmatchings/main", CdControlB);

s32 CD_vol();                                  /* extern */

/* func_801256D8 */
int CdMix(CdlATV *vol) {
    CD_vol(vol);
    return 1;
}

s32 CD_getsector();                                /* extern */

/* func_801256F8 */
int CdGetSector(void *madr, int size) {
    return CD_getsector(madr, size) == 0;
}

s32 CD_getsector2();                                /* extern */

/* func_80125718 */
int CdGetSector2(void *madr, int size) {
    return CD_getsector2(madr, size) == 0;
}

/* func_80125738 */
void *CdDataCallback(void (*func)())
{
  int new_var;
  if (new_var)
  {
    new_var = 3;
    return (void *)DMACallback(new_var, (s32)func);
  }
  else
  {
    new_var = 3;
    return (void *)DMACallback(new_var, (s32)func);
  }
}

/* func_8012575C */
int CdDataSync(int mode) {
    return CD_datasync(mode);
}

INCLUDE_PAD(4);
