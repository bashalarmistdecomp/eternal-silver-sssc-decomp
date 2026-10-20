/* PSY-Q 4.5 libetc, member intr.o, part 1 of 3: ResetCallback,
   InterruptCallback, DMACallback, VSyncCallback, VSyncCallbacks,
   StopCallback, RestartCallback, CheckCallback. The first split had it in
   main_d.c. The tree compiles this run of the member's functions with other
   flags than the runs beside it (config/cc_flags.txt), so the member is in
   several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;

/* redistributed from the original TU (split decl repair) */
s32 ResetCallback();
s32 DMACallback(s32, s32);
s32 StopCallback();
extern void (*D_801336C8_s)();

extern s32 *D_80132A5C[];

/* func_8011F3D0 */
s32 ResetCallback(void)
{
    return ((s32 (*)(void))D_80132A5C[0][3])();
}

/* func_8011F400 */
s32 InterruptCallback(s32 arg0, void *arg1)
{
    return ((s32 (*)(s32, void *))D_80132A5C[0][2])(arg0, arg1);
}

/* func_8011F430 */
s32 DMACallback(s32 arg0, s32 arg1)
{
    return ((s32 (*)(s32, s32))D_80132A5C[0][1])(arg0, arg1);
}

/* func_8011F460 */
int VSyncCallback(void (*arg0)(void))
{
    return ((s32 (*)(s32, void *))D_80132A5C[0][5])(4, arg0);
}

/* func_8011F494 */
s32 VSyncCallbacks(s32 arg0, void *arg1)
{
    return ((s32 (*)(s32, void *))D_80132A5C[0][5])(arg0, arg1);
}

/* func_8011F4C4 */
s32 StopCallback(void)
{
    return ((s32 (*)(void))D_80132A5C[0][4])();
}

/* func_8011F4F4 */
s32 RestartCallback(void)
{
    return ((s32 (*)(void))D_80132A5C[0][6])();
}

extern u16 D_801319D6[];

/* func_8011F524 */
s32 CheckCallback(void) {
    return D_801319D6[0];
}
