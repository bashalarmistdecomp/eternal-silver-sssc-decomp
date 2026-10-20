/* PSY-Q 4.5 libsnd, member ssopenq.o: SsSeqOpen. The first split had it in
   psyq_spu_o.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: SsSeqOpen */
/* return_slot_fill: SsSeqOpen */
/* reorder_fill: SsSeqOpen */
extern u32 D_800E9620;          /* _snd_openflag: bit a = access number a is open */
extern _SsFCALL D_800E9588;     /* SsFCALL */
extern char D_80013844[];       /* "Can't Open Sequence data any more\n\n" */
s32 printf();
s16 _SsInitSoundSeq(s16 arg0, s16 vab_id, u8 *addr);
void _SsNoteOn(s16 seq_no, s16 sep_no, u8 note, u8 vol);
void _SsSetProgramChange(s16 seq_no, s16 sep_no, u8 prog);
void _SsGetMetaEvent(s16 arg0, s16 arg1, u8 arg2);
void _SsSetPitchBend(s16 seq_no, s16 sep_no);
void _SsSetControlChange(s16 seq_no, s16 sep_no, u8 control);
void _SsContBankChange(s16 seq_no, s16 sep_no);  /* TU defines this below */
void _SsContMainVol(s16 seq_no, s16 sep_no, u8 data);
void _SsContPanpot(s16 seq_no, s16 sep_no, u8 data);
void _SsContExpression(s16 seq_no, s16 sep_no, u8 data);
void _SsContDamper(s16 seq_no, s16 sep_no, u8 data);
void _SsContNrpn1(s16 seq_no, s16 sep_no, u8 data);
void _SsContNrpn2(s16 seq_no, s16 sep_no, u8 data);
void _SsContRpn1(s16 seq_no, s16 sep_no, u8 data);
void _SsContRpn2(s16 seq_no, s16 sep_no, u8 data);
void _SsContExternal(s16 seq_no, s16 sep_no, u8 data);
void _SsContResetAll(s16 seq_no, s16 sep_no);
void _SsContDataEntry(s16 seq_no, s16 sep_no, u8 data);  /* TU defines this below */
void _SsSetNrpnVabAttr0(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);
void _SsSetNrpnVabAttr1(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);      /* ...1 */
void _SsSetNrpnVabAttr2(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);      /* ...2 */
void _SsSetNrpnVabAttr3(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...3 */
void _SsSetNrpnVabAttr4(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...4 */
void _SsSetNrpnVabAttr5(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...5 */
void _SsSetNrpnVabAttr6(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...6 */
void _SsSetNrpnVabAttr7(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...7 */
void _SsSetNrpnVabAttr8(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...8 */
void _SsSetNrpnVabAttr9(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...9 */
void _SsSetNrpnVabAttr10(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...10 */
void _SsSetNrpnVabAttr11(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...11 */
void _SsSetNrpnVabAttr12(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...12 */
void _SsSetNrpnVabAttr13(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...13 */
void _SsSetNrpnVabAttr14(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...14 */
void _SsSetNrpnVabAttr15(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...15 */
void _SsSetNrpnVabAttr16(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...16 */
void _SsSetNrpnVabAttr17(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...17 */
void _SsSetNrpnVabAttr18(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...18 */
void _SsSetNrpnVabAttr19(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data);   /* ...19 */

/* libsnd SsSeqOpen(addr, vab_id) (ssopenq.o): open a SEQ for playing and return its
   access number.  _snd_openflag (D_800E9620) has one bit per access number; when all 32
   are taken it prints "Can't Open Sequence data any more\n\n" (D_80013844, through
   printf) and returns -1.  Otherwise it takes the lowest clear bit (0 if
   the scan finds none), sets it, and has _SsInitSoundSeq set up record
   0 of _ss_score[access number] from the SEQ header at addr for VAB vab_id.  Then it
   fills the whole SEQ handler table SsFCALL (D_800E9588), every time it is called: the
   four event handlers, the thirteen control-change slots and the twenty NRPN
   tone-attribute slots, in the order of libsnd.h's own jt_SsInit block.  Returns -1 when
   _SsInitSoundSeq did (not SEQ data; the access number's bit stays set), else the
   access number.  MARKED SPELLING: `seq = (u8 *)addr;` copies the parameter addr into a
   typed local that is then the one thing used; it is semantically inert.  Without it the
   function scores 162/163: the target clears the access number before it copies $a0, and
   this compiler emits that order only when the parameter's one use is an assignment in
   the entry block (its scheduler never moves a parameter's own entry copy).
   The original construct is not known (the SDK's twin SsSeqOpenJ shows the same order).
   The recorded size is followed by one pad nop, hence INCLUDE_PAD(4). */
/* func_80095B38 */
short SsSeqOpen(unsigned long *addr, short vab_id)
{
    s32 i;
    s16 seq_no;
    s16 ret;
    u8 *seq;

    seq = (u8 *)addr;
    seq_no = 0;
    if (D_800E9620 == -1) {
        printf(D_80013844);
        return -1;
    }
    for (i = 0; i < 32; i++) {
        if ((D_800E9620 & (1 << i)) == 0) {
            seq_no = i;
            break;
        }
    }
    D_800E9620 |= 1 << seq_no;
    ret = _SsInitSoundSeq(seq_no, vab_id, seq);
    D_800E9588.noteon = (void (*)())_SsNoteOn;
    D_800E9588.programchange = (void (*)())_SsSetProgramChange;
    D_800E9588.metaevent = (void (*)())_SsGetMetaEvent;
    D_800E9588.pitchbend = (void (*)())_SsSetPitchBend;
    D_800E9588.control[0] = (void (*)())_SsSetControlChange;      /* CC_NUMBER */
    D_800E9588.control[1] = (void (*)())_SsContBankChange;         /* CC_BANKCHANGE */
    D_800E9588.control[3] = (void (*)())_SsContMainVol;         /* CC_MAINVOL */
    D_800E9588.control[4] = (void (*)())_SsContPanpot;             /* CC_PANPOT */
    D_800E9588.control[5] = (void (*)())_SsContExpression;             /* CC_EXPRESSION */
    D_800E9588.control[6] = (void (*)())_SsContDamper;             /* CC_DAMPER */
    D_800E9588.control[7] = (void (*)())_SsContNrpn1;             /* CC_NRPN1 */
    D_800E9588.control[8] = (void (*)())_SsContNrpn2;         /* CC_NRPN2 */
    D_800E9588.control[9] = (void (*)())_SsContRpn1;         /* CC_RPN1 */
    D_800E9588.control[10] = (void (*)())_SsContRpn2;        /* CC_RPN2 */
    D_800E9588.control[11] = (void (*)())_SsContExternal;            /* CC_EXTERNAL */
    D_800E9588.control[12] = (void (*)())_SsContResetAll;        /* CC_RESETALL */
    D_800E9588.control[2] = (void (*)())_SsContDataEntry;          /* CC_DATAENTRY */
    D_800E9588.ccentry[0] = (void (*)())_SsSetNrpnVabAttr0;         /* DE_PRIORITY */
    D_800E9588.ccentry[1] = (void (*)())_SsSetNrpnVabAttr1;         /* DE_MODE */
    D_800E9588.ccentry[2] = (void (*)())_SsSetNrpnVabAttr2;         /* DE_LIMITL */
    D_800E9588.ccentry[3] = (void (*)())_SsSetNrpnVabAttr3;      /* DE_LIMITH */
    D_800E9588.ccentry[4] = (void (*)())_SsSetNrpnVabAttr4;      /* DE_ADSR_AR_L */
    D_800E9588.ccentry[5] = (void (*)())_SsSetNrpnVabAttr5;      /* DE_ADSR_AR_E */
    D_800E9588.ccentry[6] = (void (*)())_SsSetNrpnVabAttr6;      /* DE_ADSR_DR */
    D_800E9588.ccentry[7] = (void (*)())_SsSetNrpnVabAttr7;      /* DE_ADSR_SL */
    D_800E9588.ccentry[8] = (void (*)())_SsSetNrpnVabAttr8;      /* DE_ADSR_SR_L */
    D_800E9588.ccentry[9] = (void (*)())_SsSetNrpnVabAttr9;      /* DE_ADSR_SR_E */
    D_800E9588.ccentry[10] = (void (*)())_SsSetNrpnVabAttr10;     /* DE_ADSR_RR_L */
    D_800E9588.ccentry[11] = (void (*)())_SsSetNrpnVabAttr11;     /* DE_ADSR_RR_E */
    D_800E9588.ccentry[12] = (void (*)())_SsSetNrpnVabAttr12;     /* DE_ADSR_SR */
    D_800E9588.ccentry[13] = (void (*)())_SsSetNrpnVabAttr13;     /* DE_VIB_TIME */
    D_800E9588.ccentry[14] = (void (*)())_SsSetNrpnVabAttr14;     /* DE_PORTA_DEPTH */
    D_800E9588.ccentry[15] = (void (*)())_SsSetNrpnVabAttr15;     /* DE_REV_TYPE */
    D_800E9588.ccentry[16] = (void (*)())_SsSetNrpnVabAttr16;     /* DE_REV_DEPTH */
    D_800E9588.ccentry[17] = (void (*)())_SsSetNrpnVabAttr17;     /* DE_ECHO_FB */
    D_800E9588.ccentry[18] = (void (*)())_SsSetNrpnVabAttr18;     /* DE_ECHO_DELAY */
    D_800E9588.ccentry[19] = (void (*)())_SsSetNrpnVabAttr19;     /* DE_DELAY */
    if (ret == -1) {
        return -1;
    }
    return seq_no;
}

INCLUDE_PAD(4);
