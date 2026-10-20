/* PSY-Q 4.5 libspu, member s_n2p.o: _spu_note2pitch, _spu_pitch2note. The
   first split had it in psyq_misc_aj.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
unsigned short _spu_note2pitch(long, long, long, long);

INCLUDE_RODATA("asm/data/jtbl", jtbl_80013944_pad);
/* cc97_lib: _spu_note2pitch */
extern u16 D_800B8020[];   /* S_N2P.OBJ .data+0: 12 semitone ratios, 0x8000 * 2^(k/12) */
extern u16 D_800B8038[];   /* S_N2P.OBJ .data+0x18: 128 fine ratios, 0x8000 * 2^(f/1536) */

/* libspu _spu_note2pitch (S_N2P.OBJ) */
/* func_800A0B88 */
unsigned short _spu_note2pitch(long cen_note_high, long cen_note_low, long note_high, long note_low) {
    s16 note;
    s16 oct;
    s16 key;
    u16 fine;
    unsigned long pitch;

    fine = note_low + cen_note_low;
    note = note_high + fine / 128 - cen_note_high;
    fine %= 128;
    oct = note / 12 - 2;
    key = note % 12;
    if (key < 0) {
        key += 12;
        oct--;
    }
    pitch = (D_800B8020[key] * D_800B8038[fine]) >> 16;
    if (oct >= 0) {
        pitch = 0x3FFF;
    } else {
        pitch += 1 << (-oct - 1);
        pitch >>= -oct;
    }
    return pitch;
}
/* cc97_lib: _spu_pitch2note */

/* libspu _spu_pitch2note (S_N2P.OBJ) */
/* func_800A0C58 */
long _spu_pitch2note(long cen_note_high, long cen_note_low, u16 pitch) {
    long i;
    long bit;
    u16 key;
    long fine;

    bit = 0;
    if (pitch > 0x3FFF) {
        pitch = 0x3FFF;
    }
    for (i = 0; i < 14; i++) {
        if ((pitch >> i) & 1) {
            bit = i;
        }
    }
    pitch <<= 15 - bit;
    for (i = 11; i >= 0; i--) {
        if (pitch >= D_800B8020[i]) {
            key = i;
            break;
        }
    }
    pitch = ((unsigned long) pitch << 15) / D_800B8020[key];
    for (i = 127; i >= 0; i--) {
        if (pitch >= D_800B8038[i]) {
            fine = i;
            break;
        }
    }
    fine = cen_note_low + 1 + fine;
    key += cen_note_high + (bit - 12) * 12;
    key += (u16) fine / 128;
    fine = (u16) fine % 128;
    return (key << 8) | (fine & 0xFFFE);
}
