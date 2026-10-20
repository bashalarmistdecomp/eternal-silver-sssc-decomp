/* PSY-Q 4.5 libsnd, member midicc.o: _SsSetControlChange. The first split
   had it in psyq_math_c.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: _SsSetPitchBend */
/* gnu_as_loads: _SsSetPitchBend */
/* return_slot_fill: _SsSetPitchBend */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
s32 _SsReadDeltaValue(s16, s16);
/* cc97_lib: _SsSetControlChange */
/* return_slot_fill: _SsSetControlChange */
extern _SsFCALL D_800E9588;

/* libsnd _SsSetControlChange(seq_no, sep_no, control) (midicc.o): the SEQ reader's
   control-change event (MIDI status 0xB0).  The reader (_SsGetSeqData) calls it through
   SsFCALL.control[CC_NUMBER] (D_800E9588 + 0x10) with the controller number.  score =
   &_ss_score[seq_no][sep_no] (D_800E9628, 0xB0-byte records).  It takes the value byte
   at the read pointer and dispatches on the controller number through the other
   SsFCALL.control slots, each handler getting (seq_no, sep_no, data) and reading the next
   delta time itself:
     6 CC_DATAENTRY (+0x18, _SsContDataEntry)      7 CC_MAINVOL (+0x1C, _SsContMainVol)
     10 CC_PANPOT (+0x20, _SsContPanpot)           11 CC_EXPRESSION (+0x24, _SsContExpression)
     64 CC_DAMPER (+0x28, _SsContDamper)           91 CC_EXTERNAL (+0x3C, _SsContExternal)
     98 CC_NRPN1 (+0x2C, _SsContNrpn1)            99 CC_NRPN2 (+0x30, _SsContNrpn2)
     100 CC_RPN1 (+0x34, _SsContRpn1)        101 CC_RPN2 (+0x38, _SsContRpn2)
     121 CC_RESETALL (+0x40, _SsContResetAll; two arguments, the byte is dropped)
   Controller 0 (bank change) is done in line: vab_id (+0x26) = data, then delta_value
   (+0x90) = _SsReadDeltaValue; the CC_BANKCHANGE slot (+0x14,
   _SsContBankChange) is not used here.  Any other controller: the value byte is dropped and
   delta_value = _SsReadDeltaValue.  The handlers are called unprototyped, as libsnd.h
   declares the table.  The recorded size ends in one pad nop, hence INCLUDE_PAD(4). */
/* func_80097648 */
void _SsSetControlChange(s16 seq_no, s16 sep_no, u8 control)
{
    SsScore *score;
    u8 data;

    score = &D_800E9628[seq_no][sep_no];
    data = *score->read_pos++;
    switch (control) {
    case 0:
        score->vab_id = data;
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        break;
    case 6:
        D_800E9588.control[2](seq_no, sep_no, data);
        break;
    case 7:
        D_800E9588.control[3](seq_no, sep_no, data);
        break;
    case 10:
        D_800E9588.control[4](seq_no, sep_no, data);
        break;
    case 11:
        D_800E9588.control[5](seq_no, sep_no, data);
        break;
    case 64:
        D_800E9588.control[6](seq_no, sep_no, data);
        break;
    case 91:
        D_800E9588.control[11](seq_no, sep_no, data);
        break;
    case 98:
        D_800E9588.control[7](seq_no, sep_no, data);
        break;
    case 99:
        D_800E9588.control[8](seq_no, sep_no, data);
        break;
    case 100:
        D_800E9588.control[9](seq_no, sep_no, data);
        break;
    case 101:
        D_800E9588.control[10](seq_no, sep_no, data);
        break;
    case 121:
        D_800E9588.control[12](seq_no, sep_no);
        break;
    default:
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        break;
    }
}

INCLUDE_PAD(4);
