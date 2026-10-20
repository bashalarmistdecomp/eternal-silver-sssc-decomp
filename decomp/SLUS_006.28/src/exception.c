/* Lunar SSSC — exception: where the program goes wrong: assert_fail (the
   empty assertion report) and the hook on the exception vector (patch,
   handler, word copy, hook). The first split had it in ui.c. Every function
   is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
s32 exception_address_patch(s32 *, s32 *);                          /* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_8001B01C() {

}
/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_8001B024() {

}
/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its one call passes nothing, and nothing read says what it was for. */
void func_8001B02C() {

}
/* Ruling of 2026-10-14 (docs/naming.md): Writes to an address with no memory, and nothing reaches it: nothing supports a name. */
void func_8001B034(void) {
    *(s8 *)0x03445566 = 1;
}
/* Ruling of 2026-10-14 (docs/naming.md): Loops for ever with nothing in the loop, and nothing reaches it: nothing supports a name. */
void func_8001B048(void) {
    for (;;) {
    }
}

/* func_8001B050 */
void assert_fail() {

}

/* func_8001B058 */
s32 exception_address_patch(s32 *a, s32 *b)
{
  unsigned int v;
  v = ((unsigned int) b) >> 16;
  *((unsigned short *) (a + 1)) = (unsigned short) b;
  *((unsigned short *) a) = (unsigned short) v;
  if (((unsigned int) b) & 0x8000)
  {

    v++;
    *((unsigned short *) a) = (unsigned short) v;
  }
  return (s32) v;
}

extern u8 g_ExceptionHandlerWords[];
extern u8 g_ExceptionRegisters[];

/* func_8001B07C */
void exception_handler_set(s32 *handler) {
    exception_address_patch((s32 *)g_ExceptionHandlerWords, (s32 *)g_ExceptionRegisters);
    exception_address_patch((s32 *)(g_ExceptionHandlerWords + 0xB0), handler);
}
/* func_8001B0C8 */
void exception_words_copy(s32 destination, s32 source, s32 word_count) {
    s32 word;
    s32 write_address;
    s32 read_address;
    s32 words_left;

    write_address = destination;
    read_address = source;
    words_left = word_count;
    if (words_left != 0) {
        do {
            word = (*(s32 *)read_address);
            read_address += 4;
            words_left -= 1;
            (*(s32 *)write_address) = word;
            write_address += 4;
        } while (words_left != 0);
    }
}
/* far_syms: g_ExceptionHookWords (target reaches it %hi/%lo) */
/* far_syms: g_ExceptionKeptVectorWords (target reaches it %hi/%lo) */
extern u8 g_ExceptionHookWords[];
extern u8 g_ExceptionKeptVectorWords[];

/* func_8001B0F0 */
void exception_hook_set(s32 *handler) {
    exception_handler_set(handler);
    exception_words_copy((s32 *)g_ExceptionKeptVectorWords, (s32 *)0x80000080, 4);
    exception_address_patch((s32 *)g_ExceptionHookWords, (s32 *)(g_ExceptionKeptVectorWords - 0x28));
    exception_words_copy((s32 *)0x80000080, (s32 *)g_ExceptionHookWords, 4);
}
