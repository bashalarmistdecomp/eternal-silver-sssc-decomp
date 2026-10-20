/* Lunar SSSC — music_voice: the game's side of the sound: the music's kind,
   start-up values and stop, the XA voices set, started and stepped, and the
   sounds by number (SoundMapSet, SoundPlay). The first split had it in
   misc_g.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern s8 g_MusicLevelRequest[2];
extern u8 g_MusicTrackPlaying;

/* func_80055448 */
int MusicKindGet(unsigned char music_number)
{
  int masked_music_number;
  music_number &= 0xFF;
  if (music_number >= 0x44)
  {
    return 2;
  }
  masked_music_number = music_number;
  return (masked_music_number ^ 0x3C) < 1;
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its one caller passes 0 and tests the answer for 1, which says too little. */
void func_80055468() {

}

s32 xa_sector_set(s32);
s32 xa_command_check();
s32 xa_status_request();
s32 xa_voice_get(u8, s32 *, s32 *, s32 *);  /* TU declares this below */
extern u8 g_VoiceKeptNumber;  /* TU declares this below */

/* func_80055470 */
void VoiceSet(s8 voice)
{
  s32 channel;
  s32 end_offset;
  unsigned short kept_voice;
  unsigned short masked_voice;
  s32 offset;
  kept_voice = voice;
  g_VoiceKeptNumber = kept_voice;
  masked_voice = voice & 0xFF;
  xa_voice_get(masked_voice, &offset, &end_offset, &channel);
  masked_voice = voice & 0xFF;
  xa_sector_set(offset);
  xa_status_request();
  do
  {
  }
  while (0 == xa_command_check());
}

s32 xa_play_start(s32, s32);                          /* extern */
s32 xa_end_set(s32);                               /* extern */
s32 xa_status_request();                                  /* extern */
s32 xa_voice_get(u8, s32 *, s32 *, s32 *);           /* extern */
extern u8 g_VoiceStartPending;
extern u8 g_VoiceKeptNumber;

/* func_800554C4 */
void VoiceStart(void) {
    s32 channel;
    s32 end_offset;
    s32 offset;

    if (g_VoiceKeptNumber != 0) {
        xa_voice_get(g_VoiceKeptNumber, &offset, &end_offset, &channel);
        xa_end_set(end_offset);
        xa_play_start(offset, channel);
        xa_status_request();
        g_VoiceStartPending = 1;
    }
}

/* far_syms: D_800127FC (target reaches it %hi/%lo) */
/* far_syms: g_Voice (target reaches it %hi/%lo) */
/* far_syms: g_Voice (auto: target reaches it %hi/%lo — addressing row 3) */
s32 debug_printf(s32 *);
s32 debug_printf(s32 *);                                /* extern */
extern u8 D_800127FC[];
extern s16 g_Voice;

/* func_80055524 */
s32 VoiceStep(s32 keep_playing)
{
    u8 ok;

    ok = 1;
    if ((keep_playing & 0xFF) == 0) {
        xa_play_stop();
        g_Voice = 0;
        debug_printf(D_800127FC);
    }
    if (g_VoiceKeptNumber == 0) {
        return 0;
    }
    if (g_VoiceStartPending != 0) {
        if (xa_start_check() == 0) {
            return 1;
        }
        g_VoiceStartPending = 0;
    } else {
        if (xa_end_check() != 0) {
            xa_play_stop();
            ok = 0;
        }
        if (ok == 0) {
            g_VoiceKeptNumber = 0;
        }
        return ok;
    }
}

s32 sound_area_load_copy(s32);                               /* extern */
extern u8 g_SoundMapNumber;

/* func_800555CC */
void SoundMapSet(s8 map_number) {
    g_SoundMapNumber = map_number;
    sound_area_load_copy(map_number & 0xFF);
}

extern u16 g_SoundIds[];
s32 sound_seq_stop_copy();
s32 sound_effect_play();

/* Request sound `id` on channel `ch`: id 0 stops it (sound_seq_stop_copy);
   1..0xA are bank-1 sounds id - 1; 0xB..0x82 depend on g_SoundMapNumber (0x32
   or 0: 0x82 -> bank 2 #1, 0x18 -> bank 1 #0x17, 0x1F.. -> bank 2 id -
   0x1F; otherwise only ids below 0x1F, bank 1); 0x83..0x12A are looked
   up in the 16-entry table g_SoundIds (bank 3, its index), only while
   g_SoundMapNumber is set; ids past 0x12A, or a missing table entry, return 2.
   A refused id plays bank-1 #1 and returns 2. Bank-1 sounds 9..0x19 are
   shifted down 2. Plays sound_effect_play(ch, bank, snd); returns 1 or 2. */
/* func_800555F0 */
s32 SoundPlay(u16 id, u8 ch)
{
  u16 snd;
  s32 ret;
  s32 kind;
  u8 i;
  u16 *tbl;
  snd = id;
  ret = 1;
  kind = 1;
  if (id == 0)
  {
    return sound_seq_stop_copy(ch);
  }
  if (id >= 0x12B)
  {
    return 2;
  }
  if (id >= 0xB)
  {
    if (id < 0x83)
    {
      if ((g_SoundMapNumber == 0x32) || (g_SoundMapNumber == 0))
      {
        if (id == 0x82)
        {
          snd = 1;
          kind = 2;
        }
        else if (id == 0x18)
        {
          snd = id - 1;
          kind = 1;
        }
        else if (id < 0x1F)
        {
          goto fail;
        }
        else
        {
          snd = id - 0x1F;
          kind = 2;
        }
      }
      else if (id < 0x1F)
      {
        snd = id - 1;
        kind = 1;
      }
      else
      {
        goto fail;
      }
    }
    else if (g_SoundMapNumber != 0)
    {
      tbl = g_SoundIds;
      for (i = 0; i < 0x10; i++)
      {
        if (id == tbl[i])
        {
          snd = i;
          kind = 3;
          break;
        }
      }
      if (kind == 1)
      {
        return 2;
      }
    }
    else
    {
    fail:
      ret = 2;
    }
  }
  else
  {
    snd = id - 1;
  }
  if (ret == 2)
  {
    snd = 1;
  }
  if ((kind == 1) && ((u16) (snd - 9) < 0x11))
  {
    snd -= 2;
  }
  sound_effect_play(ch, kind, snd);
  return ret;
}

/* Ruling of 2026-10-14 (docs/naming.md): Returns its third argument, and nothing reaches it: nothing in the program supports a name. */
s32 func_80055760(s32 unused, s32 unused2, s32 arg2) {
    return arg2;
}

s32 cd_track_switch(s32);
s32 sound_music_stop();
void MusicStop(void);  /* TU defines this below */
s32 debug_printf(s32 *);
extern u8 g_MusicKeptLevel;
extern u8 g_MusicKeptNumber;
extern s32 D_800B87BC;
extern s32 D_800B87C0;
extern s8 D_800B87C4;
extern s16 D_800B87C6;
extern s32 D_800B87C8;
extern s32 D_800B87CC;
extern s8 D_800B87D0;

/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_80055768 */
void MusicInit(void)
{
  g_MusicKeptLevel = 0xFB;
  g_MusicKeptNumber = 0;
  g_MusicTrackPlaying = 0;
  g_SoundMapNumber = 0;
  g_MusicLevelRequest[1] = 0;
  g_MusicLevelRequest[0] = 0;
  D_800B87BC = 0;
  D_800B87C0 = 0;
  D_800B87C4 = 1;
  D_800B87C6 = 0;
  D_800B87C8 = 0;
  D_800B87CC = 0;
  D_800B87D0 = 0;
}

s32 cd_track_switch(s32);                                 /* extern */
s32 sound_music_stop();                                  /* extern */

/* func_800557AC */
void MusicStop(void) {
    sound_music_stop();
    cd_track_switch(0);
    g_MusicTrackPlaying = 0;
}
