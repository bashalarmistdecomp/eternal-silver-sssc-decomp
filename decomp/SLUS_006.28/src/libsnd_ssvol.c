/* PSY-Q 4.5 libsnd, member ssvol.o: _SsSndSetVol, SsSeqSetVol, SsSepSetVol,
   SsSeqGetVol. The first split had it in psyq_misc_l.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* The slice of a 0xB0-byte score record (_ss_score[a][b]) this object's
   functions use. */
extern SsScore *D_800E9628[];               /* _ss_score */
s16 _SsVmSetSeqVol(s16, u16, u16, s16);
/* cc97_lib: _SsSndSetVol */
/* gnu_as_loads: _SsSndSetVol */

/* libsnd _SsSndSetVol: set the volume of sequence seq_no of SEP sep_no (the
   score _ss_score[sep_no][seq_no]).  A score that is playing (flags == 1)
   goes through _SsVmSetSeqVol(sep_no | seq_no << 8, voll, volr, 1), which
   also re-voices its sounding notes; any other only records the pair. */
/* func_80099738 */
void _SsSndSetVol(s16 sep_no, s16 seq_no, s16 voll, s16 volr)
{
    SsScore *score;

    score = &D_800E9628[sep_no][seq_no];
    if (score->flags != 1) {
        score->voll = voll;
        score->volr = volr;
    } else {
        _SsVmSetSeqVol(sep_no | (seq_no << 8), voll, volr, 1);
    }
}

/* cc97_lib: SsSeqSetVol */
/* gnu_as_loads: SsSeqSetVol */

/* libsnd SsSeqSetVol: set SEQ seq_access_num's left/right volume (the score
   _ss_score[seq_access_num][0]).  A score that is playing (flags == 1) goes
   through _SsVmSetSeqVol(seq_access_num, voll, volr, 1); any other only
   records the pair. */
/* func_800997C8 */
void SsSeqSetVol(s16 seq_access_num, s16 voll, s16 volr)
{
    SsScore *score;

    score = D_800E9628[seq_access_num];
    if (score->flags != 1) {
        score->voll = voll;
        score->volr = volr;
    } else {
        _SsVmSetSeqVol(seq_access_num, voll, volr, 1);
    }
}
/* cc97_lib: SsSepSetVol */
/* gnu_as_loads: SsSepSetVol */

/* libsnd SsSepSetVol: set the volume of sequence seq_no of SEP
   sep_access_num; the public twin of _SsSndSetVol, same
   code.  A score that is playing (flags == 1) goes through _SsVmSetSeqVol;
   any other only records the pair. */
/* func_80099830 */
void SsSepSetVol(s16 sep_access_num, s16 seq_no, s16 voll, s16 volr)
{
    SsScore *score;

    score = &D_800E9628[sep_access_num][seq_no];
    if (score->flags != 1) {
        score->voll = voll;
        score->volr = volr;
    } else {
        _SsVmSetSeqVol(sep_access_num | (seq_no << 8), voll, volr, 1);
    }
}

/* cc97_lib: SsSeqGetVol */
s16 _SsVmGetSeqVol(s16, u16 *, u16 *);

/* libsnd SsSeqGetVol (SSVOL_2.OBJ): read the volume pair of sequence seq_no
   of access number access_num (the score _ss_score[access_num][seq_no]):
   _SsVmGetSeqVol(access_num | seq_no << 8, voll, volr). */
/* func_800998C0 */
void SsSeqGetVol(s16 access_num, s16 seq_no, s16 *voll, s16 *volr)
{
    _SsVmGetSeqVol(access_num | (seq_no << 8), voll, volr);
}

INCLUDE_PAD(4);
