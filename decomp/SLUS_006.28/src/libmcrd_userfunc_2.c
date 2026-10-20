/* PSY-Q 4.5 libmcrd, member userfunc.o, part 2 of 3: UserFuncOpen,
   UserFuncExecute. The first split had it in psyq_misc_aac.c. The tree
   compiles this run of the member's functions with other flags than the
   runs beside it (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* libmcrd USERFUNC.OBJ's state: a stack of up to four step functions, each with four
   work words. D_800B8470 (.data + 0) is the index of the top entry, -1 when the stack
   is empty; D_800EA9F0 (.bss + 0) the entries' work words; D_800EAA30 (.bss + 0x40)
   the entries' functions. */
/* far_syms: D_800B8470 (target reaches it %hi/%lo) */
/* far_syms: D_800EA9F0 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA30 (target reaches it %hi/%lo) */
extern s32 D_800B8470;
extern s32 D_800EA9F0[][4];
extern s32 (*D_800EAA30[])(s32 *step);
/* cc97_lib: UserFuncOpen */
/* reorder_fill: UserFuncOpen */
extern char D_80013BF4[];
int printf();

/* libmcrd UserFuncOpen(func) (USERFUNC.OBJ): push the step function func on the stack
   UserFuncExecute runs from: the top index D_800B8470 goes up by one,
   the function goes into D_800EAA30 at that index and the entry's four work words in
   D_800EA9F0 are zeroed (last word first). With four entries already on the stack it
   pushes nothing and prints "libmcrd: event overflow" (D_80013BF4) through printf
   . */
/* func_800A3A28 */
void UserFuncOpen(s32 (*func)(s32 *step))
{
    s32 n;
    s32 i;

    n = D_800B8470 + 1;
    if (n >= 4) {
        printf(D_80013BF4);
        return;
    }
    D_800B8470 = n;
    D_800EAA30[n] = func;
    for (i = 3; i >= 0; i--) {
        D_800EA9F0[n][i] = 0;
    }
}

/* cc97_lib: UserFuncExecute */
/* libmcrd UserFuncExecute (USERFUNC.OBJ): run the step function on top of the stack
   once, with its entry's work words, and pop the entry when the function returns
   non-zero. Nothing happens while the stack is empty. libmcrd's VSync callback
   (_callback_handler, func_800A3510) calls it once a frame while UserFuncComplete
    says steps are left. */
/* func_800A3AA4 */
void UserFuncExecute(void)
{
    if (D_800B8470 >= 0) {
        if (D_800EAA30[D_800B8470](D_800EA9F0[D_800B8470]) != 0) {
            D_800B8470--;
        }
    }
}
