/* PSY-Q 4.5 libsnd, member midinote.o: _SsNoteOn. The first split had it in
   psyq_math_d.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _SsNoteOn */
/* gnu_as_loads: _SsNoteOn */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
s32 _SsVmKeyOn(s16, s16, s16, u16, u16, u16);
s32 _SsVmKeyOff(s16, s16, s16, u16);

/* libsnd _SsNoteOn(seq_no, sep_no, note, vol): the SEQ reader's note event (MIDI status
   0x90; midinote.o). score = &_ss_score[seq_no][sep_no] (D_800E9628, 0xB0-byte records),
   ch = the current channel. vol != 0: unless bit ch of unk80 (+0x80, the channel mute
   mask SsChannelMute stores) is set, _SsVmKeyOn(seq_no | sep_no << 8,
   vab_id, programs[ch], note, vol, panpot[ch]).
   vol == 0 is a note off: _SsVmKeyOff(seq_no | sep_no << 8, vab id,
   programs[ch], note), muted or not. Unlike the other SEQ handlers it does not read the
   next delta time (in sotn's _SsGetSeqData the caller reads it before the call). The
   recorded size ends in three pad nops, hence INCLUDE_PAD(12).
   MARKED SPELLING: `voll = vol;`, the u8 parameter copied into a u16 local (the type of
   _SsVmKeyOn's parameter) that is the call's argument, while the test reads the
   parameter itself. The target masks $a3 twice, once into the register kept for the
   call and once for the test; with `vol` used for both, or a u8 or s32 local, it is
   masked once (45/53). The original construct is not known. */
/* func_80097A48 */
void _SsNoteOn(s16 seq_no, s16 sep_no, u8 note, u8 vol)
{
    SsScore *score;
    u8 ch;
    u16 voll;
    u8 pan;

    score = &D_800E9628[seq_no][sep_no];
    ch = score->channel;
    voll = vol;
    pan = score->panpot[ch];
    if (vol != 0) {
        if (((score->mute_mask >> ch) & 1) == 0) {
            _SsVmKeyOn(seq_no | (sep_no << 8), score->vab_id, score->programs[ch], note, voll, pan);
        }
    } else {
        _SsVmKeyOff(seq_no | (sep_no << 8), score->vab_id, score->programs[ch], note);
    }
}
INCLUDE_PAD(12);
