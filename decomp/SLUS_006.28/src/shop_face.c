/* Lunar SSSC — shop_face: a party member's face put on the text grid for
   the shop. The first split had it in misc_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"

s32 func_800761A4(s32);
s32 MessageFaceSet(s32, s32, s32, s32, s32);

/* MessageFaceDisp's twin (scripting) for a 1-based record number: the id it
   resolves is (chr_id_then_face - 1) * 5 + 1, the first of that record's five, not
   chr_id_then_face itself.  func_800761A4 (an identity on a short today) converts the
   id; a negative 16-bit result falls back to converting old_face, the value
   the caller already holds.  The answer goes to MessageFaceSet with a last
   argument of 0 (the twin passes 1 and masks the id instead of
   sign-extending it).  The three callers (ShopMemberDisp, ShopItemLineUpSet,
   ShopMemberLineUpSet) pass a byte of the table at *g_PartyIds and
   g_MessageFaceFirst as old_face, and store the result back in g_MessageFaceFirst. */
/* func_80054C0C */
s32 ShopFaceSet(s32 chr_id_then_face, s32 places, s32 old_face, s32 unused)
{
  s32 id;
  chr_id_then_face = (chr_id_then_face & 0xFFFF) - 1;
  chr_id_then_face = chr_id_then_face * 5 + 1;
  id = func_800761A4(chr_id_then_face & 0xFFFF);
  if ((s16) id < 0)
  {
    id = func_800761A4(old_face & 0xFFFF);
  }
  return MessageFaceSet((s16) id, places & 0xFFFF, old_face & 0xFFFF, unused & 0xFFFF, 0) & 0xFFFF;
}
