/* Lunar SSSC — sound: the sound library's side: sequences and their slots,
   banks loaded, opened and closed, the music's load, level and fade, the
   area's bank, the reverb and the sound buffer, and resource commands 13 to
   17. The name is the developers' own: SOUND.C, the source file the
   assertions of sound_buf_add (line 0x334), sound_vab_head_open (0x363) and
   sound_vab_body_transfer (0x37C) name; the file's other functions have it
   by being of its group. The first split had it in cdrom_load.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"

extern u8 D_80011848[];
extern u8 g_SoundSeqs[];
extern u8 D_80011864[];
extern u8 g_SoundIds[];
extern u8 g_SoundMapAreas[];
extern s32 g_SoundSeqCount;
extern u8 g_SoundAreaBankPairs[];
extern s32 g_SoundBankCount;
extern s32 g_SoundBankLoadResource;
extern u8 D_80011830[];
extern u8 D_800B8744[];
extern s32 g_SoundBufNext;
extern u16 g_SoundMusicLevel;
extern s16 g_SoundMusicLevelFadeLength;
extern s16 g_SoundMusicLevelFadeCount;
extern s16 g_SoundMusicFadeTarget;
extern s16 g_SoundMusicFadeStart;
s32 sound_seq_play(s32, s32, s32);                           /* extern */
void sound_seq_stop(s32);
s32 sound_words_copy(s32, s32, s32);                   /* extern */
s32 sound_music_volume_set(s32);                               /* extern */
extern s32 g_SoundSeqNextSlot;
void sound_seq_slot_step(void);  /* TU defines this below */
s32 VSync(s32);
void sound_two_frame_wait(void);  /* TU defines this below */
extern u8 g_SoundSeqSlots[];
void sound_seq_rotate_play(s32 bank, s32 seq_number);  /* TU defines this below */
void sound_music_play(void);  /* TU defines this below */
void sound_music_stop(void);  /* TU defines this below */
s32 sound_bank_load(s32 resource);  /* TU defines this below */
void sound_common_load(void);  /* TU defines this below */
s32 sound_pair_find(s32 *, s32);
s32 sound_bank_close_from(s32 first_bank);  /* TU defines this below */
s32 sound_area_get();
void sound_area_load(void);  /* TU defines this below */
void sound_btl_seek(void);  /* TU defines this below */
s32 resource_load_run();
s32 assert_fail(s32 *, s32 *, s32);
s32 sound_buf_add(s32 block, s32 byte_count);  /* TU defines this below */
void sound_seq_stop_copy(s32 slot);  /* TU defines this below */
void sound_area_load_copy(void);  /* TU defines this below */
void sound_music_level_reset(void);  /* TU defines this below */
void sound_music_level_apply(void);  /* TU defines this below */
u16 sound_music_level_get(void);  /* TU defines this below */
void func_80024734();  /* TU defines this below */
void func_8002473C();  /* TU defines this below */
void sound_bank_close_all(void);  /* TU defines this below */
void sound_end(void);  /* TU defines this below */
void sound_seq_init(void);
s16 sound_music_play_check(void);
s32 resource_seek(s32);
s32 sound_area_seek(void);
void sound_bank_seq_stop();
void SsVabClose(s16);
void sound_bank_close(s32 bank);
void sound_reverb_set(s32 reverb_type, s32 depth);
s32 sound_vab_body_transfer(s32 unused, s32 unused2, u32 *payload);
void sound_id_table_set(s32 unused, s32 unused2, s16 *payload);
void sound_music_level_step(void);
s32 sound_music_level_set(s32 which, s32 level, s32 time);
s32 assert_fail(s32 *, s32 *, s32);

/* func_80023978 */
void sound_seq_init(void)
{
  int step;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `no_seq` only holds the constant -1; behaviour is the same with the constant
     written where it is read, but then 1 of this function's 22 instruction lines
     differs. */
  int no_seq;
  s32 *word;
  s32 i;
  s32 slot;
  /* CODEGEN FORCING DEVICE (address of a global in a local), not recovered source:
     `seq_table` only holds the address of `g_SoundSeqs`, through the type
     `u8 (*)[]`; behaviour is the same with the address written where it is used, but
     then 3 of this function's 22 instruction lines differ. */
  u8 (*seq_table)[];
  u8 *slot_seq;
  i = 0x7F;
  seq_table = &g_SoundSeqs;
  word = (*seq_table) + 0x1FC;
  step = 4;
  do
  {
    step = 1;
    *word = 0;
    i -= step;
    word -= step;
  }
  while (i >= 0);
  g_SoundSeqCount = 0;
  no_seq = -1;
  slot = 0xF;
  ;
  slot_seq = g_SoundSeqSlots;
  slot_seq = slot_seq + 0x3C;
  do
  {
    word = slot_seq;
    *word = no_seq;
    slot -= 1;
    slot_seq -= 4;
  }
  while (0 <= slot);
  g_SoundSeqNextSlot = 1;
}

extern s32 g_SoundSeqNextSlot;

/* func_800239D0 */
void sound_seq_slot_step(void) {
    s32 next_slot;

    next_slot = g_SoundSeqNextSlot + 1;
    g_SoundSeqNextSlot = next_slot;
    if (next_slot >= 0x10) {
        g_SoundSeqNextSlot = 1;
    }
}
s32 VSync(s32);                                 /* extern */

/* func_800239F8 */
void sound_two_frame_wait(void) {
    VSync(0);
    VSync(0);
}
extern u8 g_SoundSeqSlots[];

/* Close sequence slot slot_number: if g_SoundSeqSlots[slot_number] holds a SEQ access number, stop it, close
   it and mark the slot free (-1).
   A void function: SsSeqClose, its last call, is declared void in Sony's libsnd.h, so
   there is no result to return; the table's address is taken first, as the image does. */
/* func_80023A20 */
void sound_seq_stop(s32 slot_number) {
    s32 *base;
    s32 *slot;
    s32 seq;

    base = (s32 *) g_SoundSeqSlots;
    slot = base + slot_number;
    seq = *slot;
    if (seq != -1) {
        SsSeqStop((s16) seq);
        SsSeqClose((s16) seq);
        *slot = -1;
    }
}
extern s32 g_SoundSeqSlotBanks[];

static inline s32 inline_fn(s32 *word)
{
  return *word;
}
/* func_80023A84 */
void sound_bank_seq_stop(first_bank)
s32 first_bank;
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `no_seq` only holds the constant -1; behaviour is the same with the constant
     written where it is read, but then 2 of this function's 40 instruction lines differ. */
  int no_seq;
  u8 *slot_bank;
  s32 slot;
  s8 *slot_seq;
  slot = 0;
  no_seq = -1;
  slot_bank = (u8 *) g_SoundSeqSlotBanks;
  slot_seq = g_SoundSeqSlots;
  do
  {
    if ((inline_fn(slot_seq) != no_seq) && (inline_fn(slot_bank) >= first_bank))
    {
      sound_seq_stop(slot);
      sound_two_frame_wait();
    }
    slot_bank += 4;
    slot += 1;
    slot_seq += 4;
  }
  while (slot < 0x10);
}

/* The bank table: sound_init empties records 0..2 and g_SoundBankCount
   counts the ones in use (0 the sound effects' bank, resource 0xE8, or
   sound_bank_empty_fill's empty stand-in; 1 the area's; 2 the music's). Declared
   as the array of records it is, not as bytes: sound_seq_add's
   `g_SoundBanks[g_SoundBankCount - 1].count++` keeps its `- 1` on the index only
   as a subscript of a real array (through a cast pointer the 12 bytes
   are folded into the address, 24/32). The `(SoundBank *)` casts of the
   functions around it compile to the same bytes under either type. */
extern SoundBank g_SoundBanks[];

/* sound_seq_play -- START SEQUENCE k OF BANK `bank` ON SEQUENCE SLOT
   `slot`: the one routine both the music and the sound effects are
   started through.

     sound_music_play()            -> (2, 0, 0)    BGM: bank 2's SEQ 0, slot 0
     sound_effect_play(ch, kind, n) -> (0, n, ch)   kind 1, n < 0x16
                                   (1, n, ch)   kind 2, n < 0x60
                                   (1, n, ch)   kind 3, n < 8
     sound_seq_rotate_play(bank, k)     -> (bank, k, -1)   no call site in the image

   1. s = slot, or the rotating slot g_SoundSeqNextSlot when slot < 0 (read before
      anything is checked).
   2. Nothing at all happens unless bank < g_SoundBankCount (banks loaded) and
      k < g_SoundBanks[bank].count -- no error, no stop of what the slot is
      playing.
   3. sound_seq_stop(s): if g_SoundSeqSlots[s] != -1, SsSeqStop
      and SsSeqClose of it, and the slot is marked -1.
   4. seq = (s16) SsSeqOpen(g_SoundSeqs[g_SoundBanks[bank].first + k],
                            g_SoundBanks[bank].vab)           SsSeqOpen
      g_SoundSeqs is the table of SEQ addresses in the sound heap (0x80
      words, filled in load order through g_SoundSeqCount), so a bank's SEQs
      are the `count` entries from its `first`. The result is not tested:
      a failed open (-1) goes on into the next two calls and is stored.
   5. SsSeqSetVol(seq, 0x50, 0x50)                          SsSeqSetVol
   6. SsSeqPlay(seq, SSPLAY_PLAY = 1, 1)                    SsSeqPlay
   7. g_SoundSeqSlots[s] = seq (the slot's libsnd access number, 16 words) and
      g_SoundSeqSlotBanks[s] = bank (16 words; sound_bank_seq_stop(b) stops every slot
      whose bank is >= b before banks are dropped).
   8. slot == 0: sound_music_level_apply(), i.e. sound_music_volume_set(g_SoundMusicLevel) -- the
      music level is applied to the sequence just started on slot 0
      (SsSeqSetVol(seq, v, v), v = min(level * 0x50 / 0xFF, 0x50)), which
      replaces step 5's 0x50.
   9. slot < 0: sound_seq_slot_step() steps g_SoundSeqNextSlot on, 1..15 and back to 1.

   Declared s32 with no return statement (the TU's prototype; as `void`
   one delay slot differs, 73/74): $v0 leaves as whatever the last call
   left. No caller reads it.

   No frame wait, no CD access and no RNG draw anywhere under it: the
   whole call tree is libsnd bookkeeping (SsSeqStop -> _SsSndStop,
   SsSeqClose -> _SsClose, SsSeqOpen -> _SsInitSoundSeq, SsSeqSetVol /
   Snd_SetPlayMode -> _SsVmSetSeqVol) and none of it reaches VSync
   , a Cd* call or rng_draw (walked in the image,
   every callee to its leaves). The SEQ is already in RAM; SsSeqPlay only
   sets the play mode, the reading is the VBlank tick's.

   Shape the bytes depend on (measured): the record is indexed afresh at
   each of its three reads -- g_SoundBanks[bank].count, .first, .vab -- and
   cse keeps the one address in $s0. Through a pointer local
   (`b = &g_SoundBanks[bank]`, in five spellings) the multiply and the
   address halves come out in the other order with the two registers
   swapped: 68/74. */
/* func_80023B24 */
s32 sound_seq_play(s32 bank, s32 seq_number, s32 wanted_slot)
{
    s32 slot;
    s32 seq;

    slot = wanted_slot;
    if (wanted_slot < 0) {
        slot = g_SoundSeqNextSlot;
    }
    if (bank < g_SoundBankCount) {
        if (seq_number < g_SoundBanks[bank].seq_count) {
            sound_seq_stop(slot);
            seq = (s16)SsSeqOpen(((s32 *)g_SoundSeqs)[g_SoundBanks[bank].first_seq + seq_number], g_SoundBanks[bank].vab_id);
            SsSeqSetVol(seq, 0x50, 0x50);
            SsSeqPlay(seq, 1, 1);
            ((s32 *)g_SoundSeqSlots)[slot] = seq;
            g_SoundSeqSlotBanks[slot] = bank;
            if (wanted_slot == 0) {
                sound_music_level_apply();
            }
            if (wanted_slot < 0) {
                sound_seq_slot_step();
            }
        }
    }
}

/* func_80023C4C */
s32 sound_music_volume_set(s32 level)
{
  s32 var;
  s32 seq;

  var = (level * 0x50) / 0xFF;
  seq = *(s32 *)g_SoundSeqSlots;

  if (var >= 0x50)
    var = 0x50;

  SsSeqSetVol((s16)seq, (s16)var, (s16)var);
}

/* func_80023CC0 */
s16 sound_music_play_check(void)
{
  return SsIsEos(*(s16 *)g_SoundSeqSlots, 0);
}

/* func_80023CEC */
void sound_seq_rotate_play(s32 bank, s32 seq_number) {
    sound_seq_play(bank, seq_number, -1);
}

/* func_80023D0C */
void sound_music_play(void) {
    sound_seq_play(2, 0, 0);
}

/* func_80023D34 */
void sound_music_stop(void) {
    sound_seq_stop(0);
}
extern u8 g_SoundScoreTable[];

/* sound_init -- THE GAME'S SOUND SET-UP AT BOOT: libsnd's score table
   and tick mode, the empty bank table and the start of the sound heap,
   the reverb, the sequencer's tick and the master volume.

   One caller, system_init (frame.c, the boot hardware init), once, after
   SsInit and card_init and just before
   xa_file_set(g_SystemAudioXaFileName). No arguments, nothing returned.

   1. SsSetTableSize(g_SoundScoreTable, 16, 1)                     SsSetTableSize
      libsnd's score table: 16 sequences of 1 track, 176 bytes each, so
      the 0xB00 bytes from g_SoundScoreTable up to g_SoundSeqSlots (the sixteen
      sequence slots sound_seq_play fills).
   2. SsSetTickMode(SS_TICK60 = 1)                          SsSetTickMode
   3. g_SoundBankCount = 0: no bank is loaded.
      g_SoundBufNext = 0x801EB300: the sound heap's allocation pointer at its
      start (sound_buf_add reports an overrun past 0x801FE2FF, so the heap is
      0x13000 bytes).
   4. The three bank records g_SoundBanks[0..2] are emptied: buf = 0,
      vab = -1, res = -1, first = 0, count = 0. Bytes +0xA and +0xB of
      each record are not written.
   5. sound_seq_init(): the SEQ address table g_SoundSeqs (0x80 words)
      zeroed, g_SoundSeqCount = 0, the sixteen sequence slots g_SoundSeqSlots = -1,
      the rotating slot g_SoundSeqNextSlot = 1.
   6. sound_reverb_set(4, 0x30): SpuSetReverbModeType(4 | 0x100), i.e.
      SPU_REV_MODE_STUDIO_C with the work area cleared,
      SpuSetReverbModeDepth(0x3000, 0xB000), SpuSetReverb(SPU_ON).
   7. SsStart2()                                            SsStart2
      _SsStart(0): the sequencer's tick goes onto the VBlank.
   8. SpuSetReverbModeDepth(0x2800, 0x2800)                 SpuSetReverbModeDepth
      replaces step 6's depths: the game runs at 0x2800 / 0x2800.
   9. SpuSetCommonMasterVolume(0x3F80, 0x3F80)              SpuSetCommonMasterVolume

   Reads nothing. No RNG draw and no frame wait of its own.

   Shapes the bytes depend on (each measured by changing it alone):
   - the records are written as members of the indexed table,
     g_SoundBanks[i].x. The loop pass then makes the walking
     address from `lui $v0` / `addiu $a0,$v0,%lo` and keeps the counter
     for the `i < 3` test. A pointer local that is itself stepped (`b++`)
     is 32/43; cast stores through `g_SoundBanks + i * 12 + K` are 33/43.
   - g_SoundBankCount is stored before g_SoundBufNext (the other order: 42/43).
   Four callees have no prototype in this TU and take int arguments;
   SpuSetReverbModeDepth's (s16, s16) prototype changes nothing here because both
   arguments are constants. */
/* func_80023D54 */
void sound_init(void)
{
    s32 i;

    SsSetTableSize(g_SoundScoreTable, 0x10, 1);
    SsSetTickMode(1);
    g_SoundBankCount = 0;
    g_SoundBufNext = 0x801EB300;
    for (i = 0; i < 3; i++) {
        g_SoundBanks[i].buf_start = 0;
        g_SoundBanks[i].vab_id = -1;
        g_SoundBanks[i].resource = -1;
        g_SoundBanks[i].first_seq = 0;
        g_SoundBanks[i].seq_count = 0;
    }
    sound_seq_init();
    sound_reverb_set(4, 0x30);
    SsStart2();
    SpuSetReverbModeDepth(0x2800, 0x2800);
    SpuSetCommonMasterVolume(0x3F80, 0x3F80);
}

s32 sound_bank_load(s32);                                 /* extern */

/* func_80023E00 */
void sound_common_load(void) {
    sound_bank_load(0xE8);
}
/* far_syms: g_SoundMusicIndexSkips (target reaches it %hi/%lo) */
extern u8 g_SoundMusicIndexSkips[];

/* func_80023E20 */
s32 sound_music_index_get(s32 music_bank_number)
{
  s32 wanted_bank_number;
  s32 i;
  wanted_bank_number = music_bank_number;
  i = 0;
  while (i < 0x6)
  {
    if (((s8 *) g_SoundMusicIndexSkips)[i] >= wanted_bank_number)
    {
      break;
    }
    i++;
    music_bank_number--;
  }

  return music_bank_number - 1;
}

/* func_80023E5C */
s32 sound_pair_find(s32 *pairs, s32 key)
{
    s16 *p = (s16 *)pairs;

    while (p[1] != 0) {
        if (p[0] == key)
            return p[1];
        p += 2;
    }

    return p[-1];
}
/* sound_music_load -- LOAD THE MUSIC BANK OF MUSIC NUMBER n INTO BANK
   SLOT 2, unless it is the one already there. The loading half of
   sound_music_seek below, which makes the same test and only seeks.

   One caller, MusicBankLoad (music_bank.c): `void f(u8 m) { sound_music_load(m); }`,
   itself called from MusicPlay (play BGM n: m = g_MusicBankNumbers[n - 1]) and
   MusicBankLoadCopy. Nothing is returned ($v0 leaves as the last call left it,
   and the caller is void).

   1. n <= 0: nothing at all.
   2. n >= 0x44: n = 0x41.
   3. r = sound_music_index_get(n) + 0xE9 -- the bank's LUNADATA resource:
      n - 1 - (the leading g_SoundMusicIndexSkips entries below n) + 233 (sound.md
      "BGM numbers").
   4. g_SoundBankCount == 3 (three banks are loaded, so slot 2 is in use) and
      g_SoundBanks[2].res == r: nothing, the bank is the one already loaded.
   5. Otherwise sound_bank_close_from(2): sound_bank_close(2), which stops every
      sequence slot playing from bank 2 or above (sound_bank_seq_stop) and, when
      slot 2 is the last bank loaded (g_SoundBankCount == 3): SsVabClose
       of its VAB and vab = res = -1 when the id is >= 0,
      then the sound heap pointer g_SoundBufNext and the SEQ table fill
      g_SoundSeqCount go back to what the record saved and g_SoundBankCount goes
      down to 2.
      Then sound_bank_load(r): g_SoundBankLoadResource = r and resource_load_run(), the
      LUNADATA resource load -- the bank's command list is read from the
      CD and run, which opens the VAB and fills bank record 2.

   Reads g_SoundBankCount and g_SoundBanks[2].res (0x800E2CE6); writes nothing of
   its own. No RNG draw. The load under sound_bank_load reads the CD and
   waits on it.

   Shapes the bytes depend on (measured):
   - the resource number goes back into the parameter's own variable,
     `music_bank_number = sound_music_index_get(music_bank_number) + 0xE9`. The target keeps n in $s0 from
     the entry and puts the sum in $s0 again; with a local for the sum the
     parameter stays in $a0 up to the call and `sw $ra` moves: 22/28.
     (sound_music_seek is indifferent to this; here it decides.)
   - the resource is read as the member g_SoundBanks[2].res, as
     in sound_music_seek: read as a scalar `extern s16 D_800E2CE6` the `bne`'s
     delay slot stays empty and the function is one instruction long,
     26/28.
   `if (music_bank_number <= 0) return;` first, `> 0x43` for `>= 0x44`, and an s32
   return type all compile to the same bytes; the text follows
   sound_music_seek and music_bank.c's `void sound_music_load(s32)`. */
/* func_80023E9C */
void sound_music_load(s32 music_bank_number)
{
    if (music_bank_number > 0) {
        if (music_bank_number >= 0x44) {
            music_bank_number = 0x41;
        }
        music_bank_number = sound_music_index_get(music_bank_number) + 0xE9;
        if (g_SoundBankCount != 3 || music_bank_number != g_SoundBanks[2].resource) {
            sound_bank_close_from(2);
            sound_bank_load(music_bank_number);
        }
    }
}

/* sound_music_seek -- THE MUSIC BANK'S SEEK-AHEAD: does music number n
   need a bank other than the one in bank slot 2, and if so send the CD
   head to it. The answering half of sound_music_load (which loads
   the bank: sound_bank_close_from(2), sound_bank_load(r)), and for slot 2 what
   sound_area_seek is for the area bank in slot 1.

   One caller, MapSeek(map, music) in map_load.c (the seek before
   a scene change): after sound_area_seek() answered 0 it passes
   g_MusicBankNumbers[music - 1] when music != 0, returns our 1 at once, and on 0
   goes on to seek the map's own file.

   1. n <= 0: return 0, nothing else.
   2. n >= 0x44: n = 0x41 (sound_music_load's clamp).
   3. r = sound_music_index_get(n) + 0xE9 -- the bank's LUNADATA resource:
      n - 1 - (the leading g_SoundMusicIndexSkips entries below n) + 233 (sound.md
      "BGM numbers").
   4. g_SoundBankCount == 3 (three banks are loaded, so slot 2 is in use) and
      g_SoundBanks[2].res == r: return 0, the bank is the one already
      loaded. Nothing is written.
   5. Otherwise resource_seek(r) and return 1. resource_seek (resource_load.c) is
      file_seek("LUNADATA.FIL", sector word of resource r << 11): when
      the s8 at *g_FileHostBlock + 0xA is <= 0 and g_XaPlayCount == 0,
      cd_file_seek does CdControl(CdlSetloc) + CdControl(CdlSeekL) to
      the resource. Nothing is read and no bank record changes.

   Reads g_SoundBankCount and g_SoundBanks[2].res (0x800E2CE6); writes nothing of
   its own. No RNG draw, no frame wait.

   Shape the bytes depend on (measured, and read in the compiler's
   output): the resource is read as the MEMBER of the bank table,
   g_SoundBanks[2].res. The table is a far unsized array, so
   cc1 emits the address split -- `lui $2,%hi(g_SoundBanks+30)` then
   `lh $2,%lo(g_SoundBanks+30)($2)` -- and the lui fills the delay slot of
   the `bne` on g_SoundBankCount. Read as a scalar `extern s16 D_800E2CE6` (the
   spelling sound_area_load / sound_area_seek had for slot 1's resource, now the
   member g_SoundBanks[1].resource, which compiles the same there) the load is
   one `lh $2,D_800E2CE6` for the assembler to expand, the delay slot stays a nop and the function is one instruction
   long: 24/27. The result in a local or in the parameter's own variable
   compiles the same; `if (n <= 0) return 0;` first with a second early
   return moves the 0 up beside the first test (23/27). */
/* func_80023F0C */
s32 sound_music_seek(s32 music_bank_number)
{
    s32 res;

    if (music_bank_number > 0) {
        if (music_bank_number >= 0x44) {
            music_bank_number = 0x41;
        }
        res = sound_music_index_get(music_bank_number) + 0xE9;
        if (g_SoundBankCount != 3 || res != g_SoundBanks[2].resource) {
            resource_seek(res);
            return 1;
        }
    }
    return 0;
}

s32 sound_pair_find(s32 *, s32);                        /* extern */
s32 sound_bank_close_from(s32);                                 /* extern */
s32 sound_area_get();                                /* extern */

/* func_80023F78 */
void sound_area_load(void) {
    s32 resource;

    resource = sound_pair_find((s32 *)g_SoundAreaBankPairs, sound_area_get());
    if ((g_SoundBankCount < 2) || (resource != g_SoundBanks[1].resource)) {
        sound_bank_close_from(1);
        sound_bank_load(resource);
    }
}
s32 resource_seek(s32);                                    /* extern */

/* func_80023FE0 */
s32 sound_area_seek(void)
{
  s32 resource;
  s32 sought;
  resource = sound_pair_find((s32 *) g_SoundAreaBankPairs, sound_area_get());
  if ((g_SoundBankCount < 2) || ((sought = 0, resource != g_SoundBanks[1].resource)))
  {
    resource_seek(resource);
    sought = 1;
    return sought;
  }
  return sought;
}

/* func_80024044 */
void sound_btl_seek(void) {
    resource_seek(0x166);
}
void sound_bank_seq_stop();
void SsVabClose(s16);

/* func_80024064 */
void sound_bank_close(s32 bank)
{
  u8 *banks;
  s16 vab_id;
  void *record;
  s32 bank_buf;
  sound_bank_seq_stop();
  if (g_SoundBankCount == (bank + 1))
  {
    banks = (u8 *)g_SoundBanks;
    record = (bank * 0xC) + banks;
    vab_id = AS(s32, ((SoundBank *)record)->vab_id);
    if (vab_id >= 0)
    {
      SsVabClose(vab_id);
      ((SoundBank *)record)->vab_id = -1;
      ((SoundBank *)record)->resource = -1;
    }
    bank_buf = ((SoundBank *)record)->buf_start;
    g_SoundSeqCount = (s32) (((SoundBank *)record)->first_seq);
    g_SoundBufNext = bank_buf;
    g_SoundBankCount -= 1;
  }
}

/* func_800240EC */
s32 sound_bank_close_from(s32 first_bank)
{
  s32 bank;
  s32 done;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `last_bank` only holds the constant 2; behaviour is the same with the constant written
     where it is read, but then 2 of this function's 19 instruction lines differ. */
  int last_bank;
  last_bank = 2;
  bank = last_bank;
  done = first_bank > last_bank;
  if (done == 0)
  {
    do
    {
      sound_bank_close(bank);
      bank -= 1;
      done = bank < first_bank;
    }
    while (done == 0);
  }
  return done;
}

s32 resource_load_run();                                /* extern */

/* func_80024138 */
s32 sound_bank_load(s32 resource) {
    g_SoundBankLoadResource = resource;
    return resource_load_run();
}
/* sound_bank_empty_fill -- FILL THE BANK LIST WITH EMPTY RECORDS UP TO n:
   while fewer than n sound banks are open (g_SoundBankCount < n) the next record
   of g_SoundBanks is opened as a bank with nothing in it, and counted.

     b = g_SoundBanks[g_SoundBankCount]
     b.buf   = g_SoundBufNext   the sound heap pointer as it stands: the empty
                            bank owns no heap
     b.first = g_SoundSeqCount   the next free entry of the SEQ address table
                            g_SoundSeqs, as it stands (the low byte)
     b.count = 0            no SEQ: sound_seq_play starts nothing from it
     b.vab   = -1           no VAB
     b.res   = -1           no LUNADATA resource
     g_SoundBankCount++

   buf, first and count are the three stores sound_vab_head_open (command 13)
   makes when a real bank's VAB header arrives; here nothing follows them,
   so the record stays empty and dropping it (sound_bank_close: vab < 0, so
   no SsVabClose; g_SoundSeqCount and g_SoundBufNext put back from first and
   buf) changes nothing but the count.

   One caller, the main loop main (main.c), with n = 1 right
   after sound_common_load() asked for resource 0xE8 (232, the sound-effect
   bank): bank slot 0 is in use from then on whether or not that load
   opened a bank, which is what the fixed slots of the other two rely on
   -- the area bank is slot 1 (sound_area_load / sound_area_seek: g_SoundBankCount
   < 2, g_SoundBanks[1].resource) and the music bank slot 2 (sound_music_seek:
   g_SoundBankCount == 3, [2].res). When g_SoundBankCount >= n already, nothing is
   written.

   Reads g_SoundBankCount, g_SoundBufNext and g_SoundSeqCount, each again on every pass;
   writes g_SoundBanks[g_SoundBankCount] and g_SoundBankCount. There is no bound of its
   own: n > 3 would write past the three records sound_init clears
   (D_800E2CEC on). No callee, no RNG draw, no frame wait.

   MARKED SPELLING: the `s32` return type with no return statement.
   (main.c's extern is `s32` too, byte-neutral there because that
   caller reads no result.) As `void` the
   function is 37/38: $v0 is then dead at the exit label and reorg fills
   the entry `beqz` slot with `lui $v0,%hi(g_SoundBanks)`; the target leaves
   that lui after the branch and fills the slot with the hoisted
   `li $a2,-1`, which is what a non-void function that falls off its end
   compiles to (sound_seq_play
   above is the same).

   Shapes the bytes depend on (each measured alone against this text):
   - the five stores as MEMBERS of g_SoundBanks[g_SoundBankCount], the record
     indexed afresh at each one (a `(SoundBank *)` cast on the table
     compiles to the same bytes). cse keeps the first address
     for buf and first, then drops g_SoundBankCount at each byte store, so the
     target re-reads it after `first` and again after `count`, and the two
     halfword stores share the third address. Through a pointer local set
     at the top of the body it is 10/38 (one address, no re-read); as cast
     stores through the table's bytes + g_SoundBankCount * 12 it is 15-21/38.
   - buf before first in the source: the target loads g_SoundBufNext before
     g_SoundSeqCount and both stores go through the address formed before the
     first byte store (the `sw` is only emitted after the `sb` and the
     re-read). first before buf is 33/38.
   - vab and res as two statements, vab first (res first, or one chained
     assignment, swaps the two `sh`: 37/38).
   A `for (; g_SoundBankCount < bank_count; g_SoundBankCount++)` compiles to the same
   bytes. */
/* func_8002415C */
s32 sound_bank_empty_fill(s32 bank_count)
{
    while (g_SoundBankCount < bank_count) {
        g_SoundBanks[g_SoundBankCount].buf_start = g_SoundBufNext;
        g_SoundBanks[g_SoundBankCount].first_seq = g_SoundSeqCount;
        g_SoundBanks[g_SoundBankCount].seq_count = 0;
        g_SoundBanks[g_SoundBankCount].vab_id = -1;
        g_SoundBanks[g_SoundBankCount].resource = -1;
        g_SoundBankCount++;
    }
}

/* func_800241F4 */
void sound_reverb_set(s32 reverb_type, s32 depth)
{
  s16 spu_depth;
  s32 depth_then_mode;
  spu_depth = depth << 8;
  depth_then_mode = spu_depth;
  if (depth_then_mode)
  {
    depth_then_mode = reverb_type | 0x100;
    spu_depth = depth << 8;
  }
  else
  {
    depth_then_mode = reverb_type | 0x100;
  }
  if (reverb_type >= 0)
  {
    SpuSetReverbModeType(reverb_type | 0x100);
    SpuSetReverbModeDepth(spu_depth, (s16) (spu_depth | (~0x7FFF)));
    SpuSetReverb(1);
    return;
  }
  SpuSetReverbModeDepth(spu_depth, (s16) (spu_depth | (~0x7FFF)));
}

/* func_80024268 */
s32 sound_words_copy(s32 destination, s32 source, s32 byte_count) {
    s32 word;
    s32 write_address;
    s32 read_address;
    s32 words_left;

    write_address = destination;
    read_address = source;
    words_left = (s32) (byte_count + 3) >> 2;
    if (words_left != 0) {
        do {
            word = (*(s32 *)read_address);
            read_address += 4;
            words_left -= 1;
            (*(s32 *)write_address) = word;
            write_address += 4;
        } while (words_left != 0);
    }
    return write_address;
}

s32 assert_fail(s32 *, s32 *, s32);                       /* extern */

/* func_80024298 */
s32 sound_buf_add(s32 block, s32 byte_count) {
    s32 copy;

    copy = g_SoundBufNext;
    if ((u32) (copy + byte_count) > 0x801FE2FFU) {
        assert_fail((s32 *)D_80011830, (s32 *)D_800B8744, 0x334);
    }
    g_SoundBufNext = sound_words_copy(g_SoundBufNext, block, byte_count);
    return copy;
}
/* far_syms: D_80011848 (target reaches it %hi/%lo) */

/* func_80024314 */
void sound_vab_head_open(s32 unused, s32 byte_count, s32 payload)
{
  s32 *record;
  u8 *bank_seq_count;
  s32 vab_id;
  void *banks;
  u16 resource;
  s32 bank_buf;
  banks = g_SoundBanks;
  record = (s32 *) ((((u8 *) banks) + (g_SoundBankCount * 0xC)) + 0x0);
  bank_buf = g_SoundBufNext;
  *((u8 *) ((((u8 *) banks) + (g_SoundBankCount * 0xC)) + 0x8)) = g_SoundSeqCount;
  bank_seq_count = (((u8 *) banks) + (g_SoundBankCount * 0xC)) + ((unsigned char) 0x9);
  *record = bank_buf;
  *((u8 *) bank_seq_count) = 0;
  vab_id = sound_buf_add(payload, byte_count);
  vab_id = SsVabOpenHead(vab_id, -1);
  resource = g_SoundBankLoadResource;
  *((s16 *) ((((u8 *) banks) + (g_SoundBankCount * 0xC)) + 0x4)) = vab_id;
  *((s16 *) ((((u8 *) banks) + (g_SoundBankCount * 0xC)) + 0x6)) = resource;
  if ((*((s16 *) ((((u8 *) banks) + (g_SoundBankCount * 0xC)) + 0x4))) == (-1))
  {
    assert_fail((s32 *) D_80011848, (s32 *) D_800B8744, 0x363);
  }
}

/* far_syms: D_80011864 (target reaches it %hi/%lo) */
/* far_syms: D_800B8744 (target reaches it %hi/%lo) */
/* far_syms: g_SoundBanks (target reaches it %hi/%lo) */

/* func_800243D0 */
s32 sound_vab_body_transfer(s32 unused, s32 unused2, u32 *payload)
{
  void *banks;
  s16 vab;

  banks = g_SoundBanks;
  vab = SsVabTransBody(payload, *((s16 *) (((u8 *) banks) + (g_SoundBankCount * 0xC) + 0x4)));
  if (vab != *((s16 *) (((u8 *) banks) + (g_SoundBankCount * 0xC) + 0x4)))
  {
    assert_fail((s32 *)D_80011864, (s32 *)D_800B8744, 0x37C);
  }
  SsVabTransCompleted(1);
  return ++g_SoundBankCount;
}
/* sound_seq_add -- COMMAND 15 OF A SOUND BANK'S COMMAND LIST: ONE
   SEQ. The list walker resource_command_run (resource_load.c) calls every handler with
   (command word, payload bytes, payload); this one keeps the sequence in
   the sound heap and counts it into the bank being loaded.

     g_SoundSeqCount >= 0x80: nothing at all -- the SEQ address table is full,
       the sequence is neither copied nor counted, and nothing is
       reported.
     otherwise
       p = sound_buf_add(payload, bytes)    the SEQ copied to the sound heap at
                                        g_SoundBufNext (whole words; the heap
                                        pointer moves on, an overrun past
                                        0x801FE2FF is reported from there)
       g_SoundSeqs[g_SoundSeqCount++] = p     its address into the next entry of
                                        the 0x80-word SEQ address table
       g_SoundBanks[g_SoundBankCount - 1].count++

   The bank is g_SoundBankCount - 1 because command 14 (sound_vab_body_transfer, the VAB
   body) has already counted the bank whose header command 13
   (sound_vab_head_open) opened, with first = g_SoundSeqCount and count = 0: a bank's
   sequences are the `count` table entries from its `first`, which is how
   sound_seq_play finds sequence k of a bank. The command word (unused) is
   not read. Nothing guards g_SoundBankCount == 0: a SEQ ahead of any bank
   would count into the byte at 0x800E2CC5, below the table.

   Reads g_SoundSeqCount (before the call, and again after it), g_SoundBankCount;
   writes g_SoundSeqs[], g_SoundSeqCount, the newest bank's count, and through
   sound_buf_add the heap and g_SoundBufNext. Callee: sound_buf_add only (sound_words_copy,
   the word copy, under it). No RNG draw, no frame wait.

   Shapes the bytes depend on (each measured alone against this text):
   - the bank table subscripted as the ARRAY OF RECORDS it is,
     g_SoundBanks[g_SoundBankCount - 1]: the target takes 1 off the count, then
     multiplies by 12, then adds the table's address. Through a pointer --
     ((SoundBank *)g_SoundBanks)[g_SoundBankCount - 1] with the table declared
     u8[], or (g_SoundBanks + g_SoundBankCount - 1)->count on the array --
     the `- 1` is distributed and folded into the address
     (`addiu $v1,$v1,%lo(g_SoundBanks-12)`): 24/32. A local for the index
     under the cast keeps the arithmetic and moves the registers (23/32);
     a record pointer local is 19/32. Hence the TU's declaration
     `extern SoundBank g_SoundBanks[]` (byte-neutral for the eight other
     users).
   - the table index stepped inside the subscript, `[g_SoundSeqCount++]`: the
     increment is computed before the store and written after the next
     statement's read of g_SoundBankCount. `g_SoundSeqCount++;` as a statement of
     its own is 27/32.
   - the count after the address store (count first: 15/32).
   The call's result in a local first, an early `return` instead of the
   `if`, `count += 1` and an `s32` return type (the first split's io.c's prototype
   said s32; nothing is returned and the caller reads nothing) all give the same
   bytes. */
/* func_80024464 */
void sound_seq_add(s32 unused, s32 byte_count, s32 payload)
{
    if (g_SoundSeqCount < 0x80) {
        ((s32 *)g_SoundSeqs)[g_SoundSeqCount++] = sound_buf_add(payload, byte_count);
        g_SoundBanks[g_SoundBankCount - 1].seq_count++;
    }
}

/* far_syms: g_SoundIds (target reaches it %hi/%lo) */

/* func_800244E4 */
void sound_id_table_set(s32 unused, s32 unused2, s16 *payload)
{
  s32 i;
  s16 *entry;

  i = 0;
  entry = (s16 *)g_SoundIds;
  while (i < 0x10)
  {
    *entry++ = *payload++;
    i++;
  }
}
/* func_80024514 */
void sound_effect_play(s32 slot, s32 kind, u32 seq_number) {
    switch (kind) {
        case 1:
            if (seq_number < 0x16) {
                sound_seq_play(0, seq_number, slot);
            }
            break;
        case 2:
            if (seq_number < 0x60) {
                sound_seq_play(1, seq_number, slot);
            }
            break;
        case 3:
            if (seq_number < 8) {
                sound_seq_play(1, seq_number, slot);
            }
            break;
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Does what sound_seq_stop does; nothing read says why there are two. */
/* func_80024590 */
void sound_seq_stop_copy(s32 slot) {
    sound_seq_stop(slot);
}

/* Ruling of 2026-10-14 (docs/naming.md): Does what sound_area_load does; nothing read says why there are two. */
/* func_800245B0 */
void sound_area_load_copy(void) {
    sound_area_load();
}

/* func_800245D0 */
void sound_music_level_reset(void) {
    g_SoundMusicLevel = 0xFF;
    g_SoundMusicFadeStart = 0xFF;
    g_SoundMusicFadeTarget = 0xFF;
    g_SoundMusicLevelFadeCount = 0;
    g_SoundMusicLevelFadeLength = 0;
}
/* func_800245F0 */
void sound_music_level_step(void)
{
  u16 fade_count;
  u16 fade_length;
  fade_length = (u16) g_SoundMusicLevelFadeLength;
  if (fade_length != 0)
  {
    fade_count = ((u16) g_SoundMusicLevelFadeCount) + 1;
    g_SoundMusicLevelFadeCount = fade_count;
    if (((u32) (fade_count & 0xFFFF)) >= ((u16) g_SoundMusicLevelFadeLength))
    {
      g_SoundMusicLevelFadeLength = 0;
      g_SoundMusicLevel = (u16) g_SoundMusicFadeTarget;
    }
    else
    {
      g_SoundMusicLevel = ((u16) g_SoundMusicFadeStart) + (((s32) (((s16) (((u16) g_SoundMusicFadeTarget) - ((u16) g_SoundMusicFadeStart))) * ((u16) g_SoundMusicLevelFadeCount))) / ((s32) ((u16) g_SoundMusicLevelFadeLength)));
    }
    sound_music_volume_set(g_SoundMusicLevel);
  }
}

/* func_800246B0 */
void sound_music_level_apply(void) {
    sound_music_volume_set(g_SoundMusicLevel);
}
/* func_800246D4 */
s32 sound_music_level_set(s32 which, s32 level, s32 time)
{
  if (which == 0)
  {
    if (time == 0)
    {
      g_SoundMusicLevel = level;
      sound_music_volume_set(level);
    }
    else
    {
      g_SoundMusicFadeTarget = (s16) level;
      g_SoundMusicLevelFadeLength = time * 0x1E;
      g_SoundMusicLevelFadeCount = 0;
      g_SoundMusicFadeStart = (s16) g_SoundMusicLevel;
    }
  }
  return 0;
}

/* func_80024728 */
u16 sound_music_level_get(void) {
    return g_SoundMusicLevel;
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its caller's two zeros do not say what it was for. */
void func_80024734() {

}
/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; what its caller's 0 and 0xF stand for is not said. */
void func_8002473C() {

}

/* func_80024744 */
void sound_bank_close_all(void) {
    sound_bank_close_from(0);
}
/* func_80024764 */
void sound_end(void) {
    SsEnd();
}

/* func_80024784 */
s32 sound_area_get(u32 map_number)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `zero` only holds the constant 0; behaviour is the same with the constant written
     where it is read, but then 3 of this function's 10 instruction lines differ. */
  int zero;
  if (map_number < 0x9AU)
  {
    return (s32) (*(map_number + g_SoundMapAreas));
    zero = 0;
 do { } while (zero);
  }
  return (s32) map_number;
}
