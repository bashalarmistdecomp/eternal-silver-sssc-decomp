/* sound.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef SOUND_H
#define SOUND_H

#include "layout.h"

/* SoundBank: one of the three 12-byte bank records of the game's sound code
   (SOUND.C) at g_SoundBanks (0x800E2CC8; g_SoundBankCount counts the ones in
   use): one VAB and the sequences loaded after it from one resource. Bank 0 is
   the common one (resource 0xE8, or sound_bank_empty_fill's empty stand-in), 1
   the area's, 2 the music's. */
typedef struct {
    /* +0x00 the sound buffer's free address (g_SoundBufNext) as it was when
       the bank was opened, where the bank's VAB header and sequences begin:
       sound_bank_close puts g_SoundBufNext back to it, which gives the bank's
       part of the buffer back. Written by sound_vab_head_open and
       sound_bank_empty_fill (g_SoundBufNext), sound_init (0). */
    s32 buf_start;
    /* +0x04 the VAB id SsVabOpenHead returned for the bank's header, -1 for
       none (an id of -1 goes to assert_fail, 'SOUND.C' line 0x363).
       sound_seq_play opens the bank's sequences with it (SsSeqOpen),
       sound_vab_body_transfer hands it to SsVabTransBody and checks the answer
       against it, sound_bank_close closes it (SsVabClose) when it is not
       negative and sets -1. Written by sound_vab_head_open; -1 by sound_init,
       sound_bank_empty_fill and sound_bank_close. */
    s16 vab_id;
    /* +0x06 the LUNADATA resource number the bank was loaded from (D_800B873C,
       which sound_bank_load sets before it runs the resource), -1 for none.
       sound_music_load and sound_music_seek compare bank 2's with the music's
       resource and sound_area_load and sound_area_seek bank 1's with the
       area's: an equal number means the bank is there already. Written by
       sound_vab_head_open; -1 by sound_init, sound_bank_empty_fill and
       sound_bank_close. */
    s16 resource;
    /* +0x08 the bank's first entry in the table of sequence addresses
       D_800E2CF8 (g_SoundSeqCount when the bank was opened): sequence k of the
       bank is entry first_seq + k (sound_seq_play), and sound_bank_close puts
       g_SoundSeqCount back to it. Written by sound_vab_head_open and
       sound_bank_empty_fill (g_SoundSeqCount), sound_init (0). */
    s8 first_seq;
    /* +0x09 how many sequences the bank holds: sound_seq_add counts each one
       it adds into the newest bank (g_SoundBanks[g_SoundBankCount - 1]);
       sound_seq_play starts nothing for a sequence number not below it. Set 0
       by sound_vab_head_open, sound_bank_empty_fill and sound_init. */
    s8 seq_count;
    /* +0x0A..+0x0B nothing reads or writes them (sound_init's own note: bytes
       +0xA and +0xB of each record are not written); they round the record to
       12 bytes. */
    u8 pad_0a[2];
} SoundBank;

LAYOUT_SIZE(SoundBank, 0x0c);
LAYOUT_AT(SoundBank, buf_start, buf_start, 0x00);
LAYOUT_AT(SoundBank, vab_id, vab_id, 0x04);
LAYOUT_AT(SoundBank, resource, resource, 0x06);
LAYOUT_AT(SoundBank, first_seq, first_seq, 0x08);
LAYOUT_AT(SoundBank, seq_count, seq_count, 0x09);
LAYOUT_AT(SoundBank, pad_0a, pad_0a, 0x0a);

#endif
