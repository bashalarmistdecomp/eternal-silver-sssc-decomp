/* Lunar SSSC — btl_acting: the shows the acting slot's own record plays,
   numbers 0x97 and up (table D_800B3070): BtlActingShowRun, the extra
   records they take, and shows 0x97 to 0x9C with BtlActingTargetLeftMove.
   The first split had it in game_state.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "chr.h"
#include "btl.h"
extern u8 g_BtlActuatorC8List[];
s32 BtlAnimationEndWait();
s32 SoundPlay(s32, s32);
extern u8 g_BtlActuatorFFList[];
extern s16 g_PadActuatorListWait;
extern s32 g_PadActuatorList;
extern s8 g_PadActuatorRestFlags;
extern long HWD0;
extern long VWD0;
extern s32 g_BtlRecords;
extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern u8 g_BtlSlotPlannedNumbers[];
extern u8 g_BtlNumber;
extern s32 CharStats; /* far_syms: CharStats */
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotIds[];
extern BtlWalkBlock g_BtlWalkBlock;
extern u8 g_BtlActingSlot;
extern s8 g_BtlExtraRecordCount;
extern s32 g_BtlExtraRecords;
/* far_syms: CharStats g_BtlTargetSlot */
s32 BtlMarkedRestAnimationSet();
void BtlFallRun();
void BtlActingRestAnimationSet();
void BtlFrameWait(s32 frame_count);
s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlFrameStep();
s32 xmalloc_heap_alloc(s32);
s32 mem_clear(s32, s32);
void BtlExtraRecordsAlloc(s32 record_count);
s32 sprite_run_off(s32, s32);
s32 xmalloc_heap_free(s32);
void BtlExtraRecordsFree(s32 record_count);
s32 BtlActingTargetLeftMove(s32);
s32 BtlActingAnimationOutcomeRun(s32, s32);
void BtlShow98Run(void);
s32 BtlMarkedRestAnimationSet();
void BtlFallRun();
void BtlActingRestAnimationSet();
void BtlFrameWait(s32 frame_count);
s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlFrameStep();
s32 xmalloc_heap_alloc(s32);
s32 mem_clear(s32, s32);
void BtlExtraRecordsAlloc(s32 record_count);
s32 sprite_run_off(s32, s32);
s32 xmalloc_heap_free(s32);
void BtlExtraRecordsFree(s32 record_count);
s32 BtlActingTargetLeftMove(s32);
s32 BtlActingAnimationOutcomeRun(s32, s32);
void BtlShow98Run(void);
void BtlFacingBothSet(void);
/* far_syms: g_BtlTargetSlot */
s32 BtlMarkedRestAnimationSet();
void BtlFallRun();
void BtlActingRestAnimationSet();
s32 BtlMarkedRestAnimationSet();
void BtlActingRestAnimationSet();
void BtlFallRun();
s32 BtlAnimationSet();
void BtlFrameWait(s32 frame_count);
s32 BtlWalkSet();
s32 BtlWalkRun();
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: CharStats g_BtlActingSlot g_BtlTargetSlot */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActingSlot */
/* far_syms: g_BtlTargetSlot g_BtlActionNumber g_BtlActingSlot g_BtlRecords CharStats */
/* far_syms: g_BtlActingSlot CharStats g_BtlTargetSlot g_BtlRecords */
extern long HWD0, VWD0;
/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
s32 BtlFrameStep();
extern u8 g_BtlPartyMoveAnimations[];
/* far_syms: g_BtlRecords */
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern void (*g_BtlActingShowFunctions[])(void);
/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: CharStats g_BtlRecords */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber g_BtlNumber */
/* far_syms: g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_BtlRecords */
/* far_syms: g_BtlActingSlot */
/* far_syms: g_BtlActingSlot g_BtlRecords */
void BtlWalkEdgeRun(s32 walk_animation);  /* extern */
void BtlFallRun(void);  /* extern */

/* far_syms: g_BtlActingShowFunctions g_BtlActingSlot g_BtlActionKind g_BtlActionNumber g_BtlNumber */
/* load_hazard_nop: BtlActingShowRun */
extern u8 g_BtlActionSound;
extern void (*g_BtlActingShowFunctions[])(void);
void BtlShotSet(void);
void BtlActionStringDisp(void);
void VoiceSet(s8);
void VoiceStart(void);
s32 VoiceStep(s32);

/* func_8003A158 */
void BtlActingShowRun(s32 show)
{
    s32 snd;

    BtlShotSet();
    if (g_BtlActionKind == 2) {
        g_BtlActionSound = *(u8 *)(g_BtlActingSlot * 0x3C + CharStats + 0x19);
    } else {
        g_BtlActionSound = *(u8 *)(g_BtlActingSlot * 0x3C + CharStats + 0x1F);
    }
    if (g_BtlActionKind == 7 && g_BtlSlotIds[g_BtlActingSlot] == 0x4F) {
        g_BtlActionNumber = *(u8 *)(g_BtlActingSlot * 0x3C + CharStats + 0x1D);
        BtlActionStringDisp();
        g_BtlActionNumber = g_BtlSlotPlannedNumbers[g_BtlActingSlot];
    } else {
        BtlActionStringDisp();
    }
    if (g_BtlActingSlot == 6) {
        switch (g_BtlNumber) {
        case 0xF5:
            snd = 0x3A;
            break;
        case 0xF6:
            snd = 0x3B;
            break;
        case 0xF7:
        case 0xFE:
            snd = 0x38;
            break;
        default:
            goto end;
        }
        VoiceSet(snd);
        VoiceStart();
        while (VoiceStep(1) & 0xFF) {
            BtlFrameStep();
        }
    }
end:
    show += 0x69;
    g_BtlActingShowFunctions[show & 0xFF]();
}

s32 xmalloc_heap_alloc(s32);                             /* extern */
s32 mem_clear(s32, s32);                          /* extern */

/* func_8003A340 */
void BtlExtraRecordsAlloc(s32 record_count) {
    s32 byte_count;
    s32 records;

    byte_count = record_count << 6;
    records = xmalloc_heap_alloc(byte_count);
    g_BtlExtraRecords = records;
    mem_clear(records, byte_count);
    g_BtlExtraRecordCount = record_count;
}

s32 sprite_run_off(s32, s32);                            /* extern */
s32 xmalloc_heap_free(s32);                               /* extern */

/* func_8003A388 */
void BtlExtraRecordsFree(s32 record_count) {
    g_BtlExtraRecordCount = 0;
    sprite_run_off(0x1C, record_count);
    xmalloc_heap_free(g_BtlExtraRecords);
}
extern u8 g_BtlActuatorC8LateList[];
extern u8 D_800AE2C0[];
extern u8 D_800AE2FC[];
extern u8 D_800AE3DC[];

/* Enemy action 0x97 (g_BtlActingShowFunctions[0]): walk to the target on walk anim 1,
   start the caster's vibration script, then strike (anim 8, or 2 for
   enemy 0x72) to the anim's last step and run the hit tail.
   The two g_BtlWalkBlock.speed stores precede BtlWalkSet, which
   writes that byte itself from CharStats +0x18: both are dead, enemy
   0x42's speed 2 included. */
/* func_8003A3BC */
void BtlShow97Run(void) {
    u8 *script;
    s32 anim;
    u8 id;

    script = g_BtlActuatorC8LateList;
    anim = 8;
    g_BtlWalkBlock.speed = *((u8 *) (((u8 *) ((g_BtlActingSlot * 0x3C) + CharStats)) + 0x18));
    id = *(g_BtlActingSlot + g_BtlSlotIds);
    switch (id) {
        case 0x62:
            script = D_800AE2FC;
            break;
        case 0x3C:
            script = D_800AE2C0;
            break;
        case 0x42:
            g_BtlWalkBlock.speed = 2;
            break;
        case 0x6B:
            script = D_800AE3DC;
            break;
        case 0x72:
            anim = 2;
            break;
        default:
            break;
    }
    BtlWalkSet();
    g_BtlWalkBlock.animation = 1;
    BtlWalkRun();
    g_PadActuatorList = (s32)script;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlActingAnimationOutcomeRun(anim, 0xFF);
}
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot */
void line_path_make(s16 *, s16, s16, s16, s16, s32);

/* BtlActingTargetLeftMove -- "the creep": the caster g_BtlActingSlot slides to a
   spot `gap` pixels LEFT of the target g_BtlTargetSlot, on the target's line.
   Callers: BtlShow98Run (0xA0) and field_engine's BtlShowAARun (0x68).

   Goal: x = target.x - gap, y0 = target.y0. Neither the caster's width
   (+0x3C) nor its facing enters, and the gap is always subtracted -- the
   spot is to the target's left whichever side the caster stands on.
   Points: count = max(|goal.x - caster.x|, |target.y0 - caster.y0|) / 2
   (signed halving, toward zero); a distance of 0 or 1 does nothing at all
   -- no path, no anim, no frame.
   line_path_make(path, caster.x, caster.y0, goal.x, target.y0, count)
   fills `count` (x, y0) points -- point k = start + delta * k / count for
   k = 1..count, the last forced to the goal -- into an 80-point buffer ON
   THE STACK (not the shared path buffer g_LoadBuffer). Nothing bounds
   `count`: a distance of 162 pixels or more (count > 80) would write past
   the buffer into the saved registers. The caster then takes anim 1
   (BtlAnimationSet) and the
   loop runs: every frame is one BtlFrameStep; the first FIVE frames show
   nothing new, and from then on a point is taken after every five frames
   (the frame counter is reset when a point is taken and tested `>= 5`
   before the wait: point k is taken after frame 5 * k). A point sets the
   caster's x, y0, and depth (+8) = y0 + 1. The pass that takes the last
   point still runs its frame, so the whole creep is 5 * count + 1 frames. The caster is LEFT at the goal: nothing is saved or
   restored here, facing (+0xC) is not touched and the anim stays 1 -- the
   callers go on from there (BtlActingAnimationOutcomeRun).

   No RNG draw of its own; each frame is the battle frame wait
   (frame-rng.md).

   Shape:
   - The function is NON-void and falls off its end: with v0 live at the
     function's end reorg cannot steal the loop top's `andi $v0` into the
     back branch's delay slot, and the target has a nop there (as `void`:
     118/123).
   - The goal's x lives in a 64-byte local record of which only x is ever
     written (sp+0x1E = record +6): being in an aggregate is what keeps it
     in memory, so it is forwarded inside the first extended block and
     reloaded (`lh $a3, 0x1E($sp)`) for the call after the joins.
   - dy and the point index are ONE s16 variable ($s0); a separate dy
     scores 107.
   - The three loop inits are written AFTER the two calls; sched1 lifts
     them above the first call (their pseudos cross calls), and `p = path`
     becomes a copy of the call's own address argument (`addiu $a0, $sp,
     0x58` -- in the `beqz count` delay slot -- then `move $s2, $a0`).
     Written before the calls: 104.
   - The depth re-reads the y0 just stored (`r->y = r->y0 + 1`), which cse
     forwards: one `lhu`. `p[1] + 1` reloads it and loop.c makes a second
     pointer for p + 1 (96).
   - The loop's record pointer is spelled index first, `(idx << 6) + base`
     (119 with the array form, which puts the base first); the reads above
     are plain array-element reads.
   - line_path_make's prototype is its definition's (layer_mix.c); undeclared,
     or with btl_enemy.c's (u16 *, s16, s16, s16, s32, s32), the bytes
     are the same. */
/* func_8003A4E4 */
s32 BtlActingTargetLeftMove(s32 gap)
{
    BtlRecord obj;
    s16 path[0xA0];
    s16 dx;
    s16 i;
    s32 count;
    u8 j;
    s16 *p;
    BtlRecord *r;

    obj.sprite.x = ((BtlRecord *)g_BtlRecords)[g_BtlTargetSlot].sprite.x - gap;
    if (obj.sprite.x < ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.x) {
        dx = ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.x - obj.sprite.x;
    } else {
        dx = obj.sprite.x - ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.x;
    }
    if (((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.y < ((BtlRecord *)g_BtlRecords)[g_BtlTargetSlot].sprite.y) {
        i = ((BtlRecord *)g_BtlRecords)[g_BtlTargetSlot].sprite.y - ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.y;
    } else {
        i = ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.y - ((BtlRecord *)g_BtlRecords)[g_BtlTargetSlot].sprite.y;
    }
    if (dx < i) {
        dx = i;
    }
    count = dx / 2;
    if (count != 0) {
        line_path_make(path, ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.x, ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.y, obj.sprite.x, ((BtlRecord *)g_BtlRecords)[g_BtlTargetSlot].sprite.y, count);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 1);
        j = 0;
        p = path;
        i = 0;
        while (i < count) {
            if (j >= 5) {
                i++;
                j = 0;
                r = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
                r->sprite.x = p[0];
                r->sprite.y = p[1];
                r->sprite.depth = r->sprite.y + 1;
                p += 2;
            }
            BtlFrameStep();
            j++;
        }
    }
}

s32 BtlActingTargetLeftMove(s32);                                 /* extern */
s32 BtlActingAnimationOutcomeRun(s32, s32);                              /* extern */

/* func_8003A6D0 */
void BtlShow98Run(void) {
    BtlActingTargetLeftMove(0xA0);
    BtlActingAnimationOutcomeRun(2, 0x3C);
}
/* far_syms: D_800AE2D4 (target reaches it %hi/%lo) */
/* far_syms: D_800AE2E8 (target reaches it %hi/%lo) */
/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */
/* far_syms: g_PadActuatorListWait (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PadActuatorList (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PadActuatorRestFlags (auto: target reaches it %hi/%lo — addressing row 3) */
extern u8 D_800AE2D4[];
extern u8 D_800AE2E8[];

/* func_8003A6FC */
void BtlShow99Run(void) {
    u8 var;
    s32 actuator_list;
    actuator_list = (s32)D_800AE2D4;
    g_BtlWalkBlock.animation = 1;
    var = *((u8 *) (((u8 *) ((g_BtlActingSlot * 0x3C) + CharStats)) + 0x18));
    g_BtlWalkBlock.speed = var;
    var = *(g_BtlActingSlot + g_BtlSlotIds);
    switch (var) {
        case 6:
            actuator_list = (s32)D_800AE2E8;
            break;
        case 0x20:
            g_BtlWalkBlock.animation = 9;
            break;
        case 0x34:
        case 0x63:
            g_BtlWalkBlock.speed = 3;
            break;
        default:
            break;
    }
    BtlWalkSet();
    BtlWalkRun();
    g_PadActuatorList = actuator_list;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlActingAnimationOutcomeRun(8, 0xFF);
    BtlFrameWait(0x32);
}
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */
extern s32 g_BtlEffectBlock[];
void BtlEffectLoad(u8 *blk, u8 id);
s32 BtlAmountShowStart(s32);
s32 BtlShowEnd();

/* func_8003A80C */
void BtlShow9ARun(void)
{
    BtlRecord saved;
    BtlRecord effect_record;
    s32 o;
    s32 t;
    s32 r;
    s32 p;
    BtlRecord *q;
    u8 anim;
    s32 dy;
    s32 flag;
    s16 v;
    s32 a, b, c;

    anim = 9;
    flag = 1;
    a = g_BtlActingSlot;
    a <<= 6;
    b = g_BtlRecords;
    o = a + b;
    c = g_BtlTargetSlot;
    c <<= 6;
    dy = 6;
    if (((BtlRecord *)o)->sprite.x < ((BtlRecord *)(c + b))->sprite.x) {
        o += 0xC;
        *(s16 *)o = 0x20;
    } else {
        o += 0xC;
        *(s16 *)o = 0x2020;
    }
    b = g_BtlActingSlot;
    switch (g_BtlSlotIds[b]) {
    case 0x1E:
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
        break;
    case 0x3A:
    case 0x65:
    case 0x66:
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
        anim = 0xB;
        break;
    case 0x3D:
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 2);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
        dy = 0x14;
        flag = 0;
        break;
    case 0x7A:
        saved = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        BtlEffectLoad((u8 *)&effect_record, 0x62);
        *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords) = effect_record;
        r = (g_BtlActingSlot << 6) + g_BtlRecords;
        ((BtlRecord *)r)->sprite.x = saved.sprite.x;
        ((BtlRecord *)r)->sprite.y = saved.sprite.y;
        ((BtlRecord *)r)->sprite.depth = saved.sprite.depth;
        if (saved.sprite.attr & 0x2000) {
            ((BtlRecord *)r)->sprite.attr |= 0x2000;
        } else {
            ((BtlRecord *)r)->sprite.attr &= 0xDFFF;
        }
        p = (g_BtlActingSlot << 6) + g_BtlRecords;
        *(s16 *)p = 1;
        BtlAnimationSet(p, 0);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
        SoundPlay(0x4F, 3);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 1);
        BtlFrameWait(0x3C);
        g_BtlActionSound = 0x5A;
        anim = 3;
        break;
    }
    BtlExtraRecordsAlloc(1);
    *(BtlRecord *)g_BtlExtraRecords = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    t = (g_BtlTargetSlot << 6) + g_BtlRecords;
    q = (BtlRecord *)g_BtlExtraRecords;
    q->sprite.x = ((BtlRecord *)t)->sprite.x;
    v = ((BtlRecord *)t)->sprite.y;
    q->sprite.y = v;
    q->sprite.depth = v + dy;
    SoundPlay(g_BtlActionSound, 3);
    BtlAnimationSet(g_BtlExtraRecords, anim);
    g_PadActuatorList = (s32)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlFrameWait(0x14);
    BtlAmountShowStart(flag);
    if (g_BtlSlotIds[g_BtlActingSlot] == 0x7A) {
        ((BtlRecord *)g_BtlExtraRecords)->sprite.depth = ((BtlRecord *)g_BtlExtraRecords)->sprite.y;
    }
    BtlAnimationEndWait(g_BtlExtraRecords);
    BtlExtraRecordsFree(1);
    if (g_BtlSlotIds[g_BtlActingSlot] == 0x7A) {
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 2);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
        *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords) = saved;
        xmalloc_heap_free(g_BtlEffectBlock[0]);
    }
    BtlShowEnd();
}

extern BtlShotBlock g_BtlShotBlock;
extern u8 g_BtlActuator41List[];
extern u8 g_BtlActuatorFFTo45List[];
extern u8 D_800AE3CC[];
s32 BtlShotRun();
void BtlTargetOutcomeShow(void);

/* Enemy action 0x9B (g_BtlActingShowFunctions[4]): the plain shot. BtlShotSet has
   already filled the shot block g_BtlShotBlock with its defaults; per enemy
   type g_BtlSlotIds[g_BtlActingSlot] the switch edits it — b0 the caster's launch
   anim, b1 the wait after it, b2 the projectile's anim, b4 its pace, b5 /
   b6 / b7 the sounds before / at launch / on arrival, h8 / hA the muzzle
   offset — and picks the two vibration scripts: `before` for the shot
   (none unless 0x16 / 0x60 / 0x64 or 0x2D) and `after` for the hit
   (g_BtlActuatorFFTo45List; g_BtlActuatorC8List for 0x16 / 0x60 / 0x64). Types outside the cases
   keep the defaults. Then the lane walk BtlWalkEdgeRun on walk anim 1 (a
   party actor, g_BtlActingSlot < 5, on its own g_BtlPartyMoveAnimations anim), the face-off
   BtlFacingBothSet, the effect trio with `before` (g_PadActuatorList = script — or 0,
   g_PadActuatorRestFlags = 0x3F, g_PadActuatorListWait = 0), the shot BtlShotRun, the trio again
   with `after`, and the hit tail BtlTargetOutcomeShow. No RNG draw of its own. */
/* func_8003AD08 */
void BtlShow9BRun(void)
{
    u8 *before;
    u8 *after;
    BtlShotBlock *p;
    u8 walk;

    before = 0;
    after = g_BtlActuatorFFTo45List;
    p = &g_BtlShotBlock;
    switch (g_BtlSlotIds[g_BtlActingSlot]) {
    case 0x2:
    case 0x33:
        p->from_sound = 0x5E;
        break;
    case 0x11:
    case 0x6D:
        p->arrive_sound = 0x7C;
        p->dx = -0xA;
        p->from_sound = 0;
        p->dy = -4;
        p->from_wait = 0;
        break;
    case 0x16:
    case 0x60:
    case 0x64:
        before = g_BtlActuator41List;
        after = g_BtlActuatorC8List;
        p->from_sound = 0x64;
        p->from_wait = 0x1B;
        p->launch_sound = 0;
        p->dy = -8;
        break;
    case 0x18:
    case 0x4A:
        p->dx = 0xA;
        p->dy = -0x1B;
        p->from_wait = 0x38;
        break;
    case 0x2D:
        before = D_800AE3CC;
        p->from_sound = 0x52;
        p->launch_sound = 0x36;
        p->dx = 4;
        p->dy = -1;
        p->from_wait = 0x3A;
        p->speed = 2;
        break;
    case 0x2F:
        p->from_animation = 2;
        p->animation = 0xA;
        p->from_wait = 0x35;
        p->dx = 4;
        p->dy = -0xC;
        break;
    case 0x42:
        p->from_sound = 0x4E;
        p->from_animation = 2;
        p->from_wait = 0x27;
        break;
    case 0x46:
        p->from_sound = 0x3C;
        p->from_animation = 2;
        p->from_wait = 0x20;
        p->dx = -8;
        p->dy = -0x14;
        break;
    case 0x49:
        p->from_sound = 0x4E;
        p->arrive_sound = 0x5A;
        p->from_animation = 2;
        p->from_wait = 0x10;
        p->dx = -7;
        p->launch_sound = 0;
        p->dy = -2;
        break;
    case 0x4D:
        p->from_animation = 2;
        p->dx = -0x25;
        p->dy = -0x2D;
        break;
    case 0x76:
        p->from_animation = 2;
        p->animation = 0xC;
        p->from_sound = 0x3C;
        break;
    }
    if (g_BtlActingSlot < 5) {
        walk = g_BtlPartyMoveAnimations[g_BtlActingSlot];
    } else {
        walk = 1;
    }
    BtlWalkEdgeRun(walk);
    BtlFacingBothSet();
    g_PadActuatorList = (s32)before;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlShotRun();
    g_PadActuatorList = (s32)after;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlTargetOutcomeShow();
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80011ECC_pad);
/* far_syms: g_PadActuatorListWait */
/* far_syms: g_PadActuatorList */
/* far_syms: g_PadActuatorRestFlags */
s32 BtlShotRun();
void BtlPartyHitAnimationSet(void);
s32 BtlAmountShowCheck(void);

/* Enemy action 0x9C (g_BtlActingShowFunctions[5]): the actor strikes from ON its target.
   Per enemy type g_BtlSlotIds[g_BtlActingSlot] the switch picks the copy's anim
   (default 0xB), fills the presentation block g_BtlShotBlock (bytes +0..+7,
   offsets +8/+0xA) and clears `impact` for types 0x49, 0x7F and 0x50 when
   g_BtlActionKind != 2; type 0x74 passes 0xE to BtlWalkEdgeRun. Then the effect
   trio (g_PadActuatorList = g_BtlActuatorC8List, g_PadActuatorRestFlags = 0x3F, g_PadActuatorListWait = 0) and a
   64-byte copy of the actor record on the stack becomes the extra object
   g_BtlExtraRecords, placed at the target's x / y0 (y = y0 + 1) playing `anim`;
   16 frames, BtlPartyHitAnimationSet, and with `impact` BtlAmountShowStart(1) runs while
   BtlAmountShowCheck() holds, frame by frame: when the copy reaches its last
   step the actor goes idle (BtlActingRestAnimationSet) — type 0x7A first plays anim
   0x11 on the real actor and waits for ITS last step. The copy is
   released, slot 0x1C cleared, and (impact) BtlFallRun settles KOs, or
   (no impact) 30 more frames. BtlRecord must be 4-aligned for the copy's
   lw/sw loop (its +0x28/+0x2C words). */
/* func_8003AFA0 */
void BtlShow9CRun(void)
{
  BtlRecord obj;
  BtlShotBlock *p;
  BtlRecord *t;
  s32 mode;
  s32 anim;
  u8 impact;
  u8 stage;
  impact = 1;
  anim = 0xB;
  mode = 1;
  p = &g_BtlShotBlock;
  switch (g_BtlSlotIds[g_BtlActingSlot])
  {
    case 0x17:
    case 0x51:
    case 0x5F:
      anim = 0xC;
      p->animation = 0xB;
      p->dx = -6;
      p->dy = -0x16;
      p->from_wait = 0;
      p->arrive_sound = 0x5A;
      break;
    case 0x49:
      impact = 0;
      p->animation = 0xA;
      p->dx = -0xE;
      p->dy = 0x17;
      p->from_wait = 0x17;
      p->length = 0x19;
      p->from_sound = 0x4D;
      p->arrive_sound = 0x5B;
      break;
    case 0x50:
      if (g_BtlActionKind == 2)
      {
        p->from_animation = 2;
      }
      else
      {
        impact = 0;
      }
      anim = 0xA;
      p->from_wait = 0x17;
      p->dx = -0xE;
      p->dy = 0x17;
      p->length = 0x19;
      p->from_sound = 0x4D;
      p->arrive_sound = 0x5A;
      break;
    case 0x3A:
    case 0x65:
    case 0x66:
      anim = 0xA;
      p->from_animation = 2;
      p->length = 0x14;
      p->dx = -8;
      p->dy = -0x28;
      p->arrive_sound = 0x6E;
      break;
    case 0x41:
      anim = 0xA;
      p->dx = -0x18;
      p->dy = 4;
      p->length = 0x2E;
      p->from_wait = 0x28;
      p->launch_sound = 0x72;
      p->arrive_sound = 0x73;
      break;
    case 0x4D:
      p->animation = 0xA;
      p->from_wait = 0x3C;
      p->dx = -0xE;
      p->dy = -0x11;
      p->arrive_sound = 0x7B;
      break;
    case 0x6F:
      p->from_animation = 2;
      p->animation = 0xA;
      p->dx = -0x20;
      p->dy = -0xF;
      p->from_wait = 0x34;
      p->from_sound = 0x50;
      p->arrive_sound = 0x6D;
      break;
    case 0x73:
      anim = 0x11;
      p->from_animation = 0xF;
      p->animation = 0x10;
      p->dx = -0x2C;
      p->dy = -0x40;
      p->length = 0x31;
      p->arrive_sound = 0x6E;
      break;
    case 0x74:
      anim = 0xC;
      mode = 0xE;
      p->animation = 0xB;
      p->dx = -0x24;
      p->dy = 8;
      p->from_sound = 0x52;
      p->launch_sound = 0x36;
      p->arrive_sound = 0x4A;
      break;
    case 0x7A:
      anim = 0x12;
      p->from_animation = 2;
      p->animation = 0x13;
      p->from_wait = 0x37;
      p->dx = -8;
      p->dy = -0x46;
      p->arrive_sound = 0x4A;
      break;
    case 0x7F:
      impact = 0;
      anim = 0xE;
      p->from_animation = 8;
      p->animation = 0xD;
      p->dy = -0x10;
      p->arrive_sound = 0x4A;
      break;
  }
  BtlWalkEdgeRun(mode);
  BtlFacingBothSet();
  BtlShotRun();
  g_PadActuatorList = (s32) g_BtlActuatorC8List;
  g_PadActuatorRestFlags = 0x3F;
  g_BtlExtraRecordCount = 1;
  g_PadActuatorListWait = 0;
  g_BtlExtraRecords = (s32) &obj;
  obj = *(BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords);
  t = (BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords);
  obj.sprite.x = t->sprite.x;
  obj.sprite.y = t->sprite.y;
  obj.sprite.depth = t->sprite.y + 1;
  BtlAnimationSet(&obj, anim);
  BtlFrameWait(0x10);
  BtlPartyHitAnimationSet();
  if (impact)
  {
    stage = 1;
    anim = (obj.end & 0x7FFF) - 1;
    BtlAmountShowStart(1);
    while (BtlAmountShowCheck() != 0)
    {
      BtlFrameStep();
      if (stage && (obj.step == (u16) anim))
      {
        if ((g_BtlSlotIds[g_BtlActingSlot] == 0x7A) && (stage == 1))
        {
          BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0x11);
          stage++;
          anim = (((BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords))->end & 0x7FFF) - 1;
        }
        else
        {
          BtlActingRestAnimationSet();
          stage = 0;
        }
      }
    }
  }
  BtlAnimationEndWait(&obj);
  g_BtlExtraRecordCount = 0;
  sprite_run_off(0x1C, 1);
  if (impact)
  {
    BtlFallRun();
    if (stage)
    {
      if ((g_BtlSlotIds[g_BtlActingSlot] == 0x7A) && (stage == 1))
      {
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0x11);
      }
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    }
  }
  else
  {
    BtlFrameWait(0x1E);
  }
  BtlMarkedRestAnimationSet();
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_800120A4_pad);
