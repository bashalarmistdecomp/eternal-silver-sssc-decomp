/* PSY-Q 4.5 libsnd, member vm_init.o: _SsVmInit. The first split had it in
   psyq_misc_q.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern u16 D_800EA708;
extern u8 D_800EA710[];       /* _svm_vab_used: 1 = slot holds an opened VAB */
extern u16 D_800EA688;
extern u16 D_800EA688;
extern SvmVoice D_800E9EC0[];
extern s8 D_800EA6E4;
extern void (*D_800EA418)(s32);
extern void (*D_800EA5A0)(s32);
extern u16 D_800E9EB0;
extern u16 D_800E9EB2;
extern u16 D_800E9EB4;
extern u16 D_800E9EB6;
extern u16 D_800E9EB8;
extern u16 D_800E9EBA;
extern u8 D_800EA400[];
extern u16 D_800EA420[];
extern u16 D_800EA5E8;
extern s8 D_800EA720;
void SpuSetVoiceAttr(SpuVoiceAttr *);

/* cc97_lib: _SsVmInit */
/* gnu_as_loads: _SsVmInit */
/* return_slot_fill: _SsVmInit */
extern SpuReverbAttr D_800EA5F0; /* _svm_rattr: the reverb attributes libsnd hands to SpuSetReverbModeParam */
extern s16 D_800EA6D0;           /* _svm_stereo_mono */
extern s16 D_800EA6D2;           /* kMaxPrograms */
extern s32 D_800EA6E8;           /* _svm_vab_not_send_size */
extern u16 D_800EA768;           /* _svm_vab_count */
extern u8 D_800EA7B8[];          /* _ss_spu_vm_rec: libspu's allocation table (32 blocks), vm_init.o's own bss (+8) */
void _SsVmKeyOffNow();
void _spu_setInTransfer(s32);

/* libsnd _SsVmInit (VM_INIT.OBJ): reset the voice manager for maxVoice SPU
   voices.  Called once, by _SsInit, with 24.
   1. _spu_setInTransfer(0); _svm_damper = 0; SpuInitMalloc(32,
      the object's own table) starts libspu's sound-memory allocator.
   2. Clear the 192 shadow registers, the 24 dirty bytes, the VAB count and
      the 16 VAB slot states.
   3. _SsVmMaxVoice = maxVoice, or 24 when maxVoice is not in 0..23 (the
      test is unsigned, so a negative count is 24 as well).
   4. For each managed voice: reset its record (age 24, VAG number 0xFF,
      key state idle, score -1, tone 0xFF, pan 64, everything else that is
      touched 0; note, vab_id, prior and +0x28 / +0x34 are left alone), set
      the SPU voice through SpuSetVoiceAttr (volume 0 / 0, pitch 0x1000,
      sample address 0x1000, ADSR1 0x80FF, ADSR2 0x4000), then make it the
      current voice and _SsVmKeyOffNow it.
   5. _svm_rattr: mask 0, depth 0x3FFF / 0x3FFF, mode 0.  Clear the pending
      key-on, reverb and noise words and the first key-off word (the
      second, _svm_okof2, is NOT cleared here), the auto-key-off switch,
      the stereo / mono flag and _svm_vab_not_send_size; kMaxPrograms =
      128; _SsVmFlush(). */
/* func_8009A728 */
void _SsVmInit(s8 maxVoice)
{
    SpuVoiceAttr attr;
    u16 i;
    u32 n;

    _spu_setInTransfer(0);
    D_800EA688 = 0;
    SpuInitMalloc(32, D_800EA7B8);
    for (i = 0; i < 192; i++) {
        D_800EA420[i] = 0;
    }
    for (i = 0; i < 24; i++) {
        D_800EA400[i] = 0;
    }
    D_800EA768 = 0;
    for (i = 0; i < 16; i++) {
        D_800EA710[i] = 0;
    }
    n = maxVoice;
    if (n >= 24) {
        D_800EA6E4 = 24;
    } else {
        D_800EA6E4 = n;
    }
    attr.mask = 0x60093;    /* SPU_VOICE_VOLL | VOLR | PITCH | WDSA | ADSR_ADSR1 | ADSR_ADSR2 */
    attr.pitch = 0x1000;
    attr.addr = 0x1000;
    attr.adsr1 = 0x80FF;
    attr.volume.left = 0;
    attr.volume.right = 0;
    attr.adsr2 = 0x4000;
    for (i = 0; i < D_800EA6E4; i++) {
        D_800E9EC0[i].age = 24;
        D_800E9EC0[i].vag = 0xFF;
        D_800E9EC0[i].key_stat = 0;
        D_800E9EC0[i].pitch = 0;
        D_800E9EC0[i].envx = 0;
        D_800E9EC0[i].seq_sep = -1;
        D_800E9EC0[i].prog = 0;
        D_800E9EC0[i].pg = 0;
        D_800E9EC0[i].tone = 0xFF;
        D_800E9EC0[i].vol = 0;
        D_800E9EC0[i].channel = 0;
        D_800E9EC0[i].pan = 0x40;
        D_800E9EC0[i].note_vol = 0;
        D_800E9EC0[i].auto_vol = 0;
        D_800E9EC0[i].unk_20 = 0;
        D_800E9EC0[i].unk_22 = 0;
        D_800E9EC0[i].unk_24 = 0;
        D_800E9EC0[i].auto_pan = 0;
        D_800E9EC0[i].unk_2c = 0;
        D_800E9EC0[i].unk_2e = 0;
        D_800E9EC0[i].unk_30 = 0;
        D_800E9EC0[i].unk_32 = 0;
        D_800E9EC0[i].unk_26 = 0;
        attr.voice = 1 << i;
        SpuSetVoiceAttr(&attr);
        D_800EA708 = i;
        _SsVmKeyOffNow(1);   /* the callee takes no argument */
    }
    D_800EA5F0.mask = 0;
    D_800EA5F0.depth.left = 0x3FFF;
    D_800EA5F0.depth.right = 0x3FFF;
    D_800EA5F0.mode = 0;
    D_800E9EB0 = 0;
    D_800E9EB2 = 0;
    D_800EA5E8 = 0;
    D_800E9EB4 = 0;
    D_800E9EB6 = 0;
    D_800E9EB8 = 0;
    D_800E9EBA = 0;
    D_800EA720 = 0;
    D_800EA6D0 = 0;
    D_800EA6E8 = 0;
    D_800EA6D2 = 0x80;
    _SsVmFlush();
}
