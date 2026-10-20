/* Lunar SSSC — btl_animation: the battle records' animation: one set,
   started and stepped. The first split had it in rendering2.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "tile.h"
#include "pad.h"
#include "palette.h"
#include "chr.h"
#include "btl.h"
#include "sprite.h"
extern long HWD0;
extern long VWD0;
void BtlAnimationStart(void *record, s32 restart);
extern long HWD0, VWD0;

/* func_80027208 */
void BtlAnimationSet(void *record, s32 animation)
{
    u16 old = ((BtlRecord *)record)->animation;

    if (old != (u16)animation)
    {
        ((BtlRecord *)record)->animation = (u16)animation;
        ((BtlRecord *)record)->animation_before = old;
        BtlAnimationStart(record, 1);
    }
}

/* func_80027240 */
void BtlAnimationStart(void *record, s32 restart) {
    u16 first_step;

    first_step = (*(s16 *)((u8 *)((((BtlRecord *)record)->animation * 4) + ((BtlRecord *)record)->animation_table) + 0x2));
    ((BtlRecord *)record)->step = first_step;
    ((BtlRecord *)record)->end = (s16) (first_step + (*(s32 *)((((BtlRecord *)record)->animation * 4) + ((BtlRecord *)record)->animation_table)));
    if (restart != 0) {
        ((BtlRecord *)record)->timer = 0;
        ((BtlRecord *)record)->step_length = (u16) (*(s16 *)((u8 *)((((BtlRecord *)record)->step * 4) + ((BtlRecord *)record)->step_list) + 0x2));
    }
}
extern s32 g_SpriteList;
void sprite_run_off(s32, s32);

/* The per-frame animation tick for n records from `first`: advance each
   active record's timer; at the end of a step, step on (HOLD keeps the last
   step, otherwise BtlAnimationStart restarts the animation) and reload the
   step's duration; then copy the record's render block into its
   g_SpriteList entry with the current pose. */
/* func_800272A8 */
void BtlAnimationStep(BtlRecord *actor, s32 first, s32 n)
{
    SpriteEntry *entry;
    u8 i;
    u16 old;

    entry = (SpriteEntry *)g_SpriteList;
    entry += first;
    sprite_run_off(first, n);
    for (i = 0; i < n; i++, actor++, entry++) {
        if (actor->on != 0) {
            actor->timer++;
            if (actor->timer >= actor->step_length) {
                old = actor->step;
                actor->step = old + 1;
                if (!(actor->end & 0x8000)) {
                    if (actor->step >= actor->end) {
                        BtlAnimationStart(actor, 1);
                    }
                } else if (actor->step >= (actor->end & 0x7FFF)) {
                    actor->step = old;
                }
                actor->timer = 0;
                actor->step_length = AS(SpriteStep *, actor->step_list)[actor->step].length;
            }
            *entry = actor->sprite;
            entry->pose = AS(SpriteStep *, actor->step_list)[actor->step].pose;
        }
    }
}
