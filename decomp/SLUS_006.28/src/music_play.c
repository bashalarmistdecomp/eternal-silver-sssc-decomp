/* Lunar SSSC — music_play: the music played: a piece by number, loaded and
   started, its level's fade and step, and its bank. The first split had it
   in misc_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"

extern u8 g_MusicLevelRequest[2];
extern const u8 g_MusicBankNumbers[];
extern s8 g_MusicKeptNumber;
extern u8 g_MusicKeptLevel;

/* Ruling of 2026-10-14 (docs/naming.md): Returns 0 in this build; its one caller, never run, does not say what it asked. */
s32 func_80054F24(void) {
    return 0;
}

/* far_syms: g_MusicBankNumbers */
extern u8 g_MusicTrackPlaying;
extern s32 g_MusicWaitFrameKind;
void sound_music_stop(void);
void cd_track_switch(s32);
s32 func_80055468(s32); /* an empty stub in this build: the test reads a stale $v0 */
s32 MusicBankLoad(s32);
s32 MusicFadeStep(s32, s32);
s32 ScriptFrameStep(s32);
s32 BtlFrameStep();
void MusicLoadStart(s32 music_number, s32 time);  /* TU defines this below */
void MusicStart(s32 unused, s32 time);  /* TU defines this below */

/* Music request. id 0 stops the music (sound_music_stop); ids from 0x45 up are
   rejected (returns 2). id 0x3C is the special track that restarts through
   cd_track_switch (stopping it first if g_MusicTrackPlaying says it is playing). Any
   other id: with the sound system ready (func_80055468 == 1) and the track
   not already current (g_MusicKeptNumber), queue g_MusicBankNumbers[id - 1] and run the
   fade loop -- wait for MusicFadeStep to report the fade done, then for
   func_80054F24 to report idle, servicing the script (g_MusicWaitFrameKind == 1,
   ScriptFrameStep) or battle (== 2, BtlFrameStep) per-frame update
   meanwhile -- then start it with the time `vol` (MusicStart). Not ready:
   hand it to MusicLoadStart instead, with the same time. arg1 (the
   callers' fade mode) is not read here. Returns 1, or 2 for a bad id. */
/* func_80054F2C */
s32 MusicPlay(u8 id, s32 unused, u8 vol)
{
    u8 ret;
    u8 busy;

    ret = 1;
    busy = ret;
    if (id == 0) {
        sound_music_stop();
    } else if (id < 0x45) {
        if (id == 0x3C) {
            if (g_MusicTrackPlaying != 0) {
                cd_track_switch(0);
            }
            cd_track_switch(2);
            g_MusicTrackPlaying = ret;
        } else if ((u8)func_80055468(0) == 1) {
            if (id != (u8)g_MusicKeptNumber) {
                MusicBankLoad(g_MusicBankNumbers[id - 1]);
                MusicFadeStep(0, 1);
                while (1) {
                    if (busy == 0) {
                        if (func_80054F24() == 0) {
                            break;
                        }
                    } else if ((u8)MusicFadeStep(0, 1) == 0) {
                        busy = 0;
                    }
                    switch (g_MusicWaitFrameKind) {
                    case 0:
                        break;
                    case 1:
                        ScriptFrameStep(1);
                        break;
                    case 2:
                        BtlFrameStep();
                        break;
                    }
                }
                MusicStart(id, vol);
            }
        } else {
            MusicLoadStart(id, vol);
        }
    } else {
        ret = 2;
    }
    return ret;
}

extern void *g_GameOptions;

/* func_800550D4 */
void SoundMonoSet(void) {
    if (((GameOptions *)g_GameOptions)->mono != 0) {
        SsSetMono();
        return;
    }
    SsSetStereo();
}

s32 sound_music_play();
s32 sound_music_level_set(s32, u8, s32);
s32 func_80024734(s32, s32);
s32 MusicFadeStep(s32, s32);
void MusicBankLoadCopy(s32 music_bank_number);  /* TU defines this below */

/* With a non-zero (u8) time, MusicFadeStep(1, time); otherwise
   sound_music_level_set(0, g_MusicKeptLevel, 0). Then g_MusicKeptNumber = music_number and
   MusicBankLoadCopy((u8) (g_MusicBankNumbers[(u8) music_number - 1])), func_80024734(0, 0),
   sound_music_play(), and the same time test again at the end.
   g_MusicBankNumbers is read as a CONST array: the read is then unchanging, so
   GCC 2.8's true_dependence lets the scheduler hoist the lbu above the
   g_MusicKeptNumber store (byte accesses get no MEM_IN_STRUCT exemption). */
/* func_80055118 */
void MusicLoadStart(s32 music_number, s32 time)
{
  if ((time & 0xFF) != 0)
  {
    MusicFadeStep(1, time & 0xFF);
  }
  else
  {
    sound_music_level_set(0, g_MusicKeptLevel, 0);
  }
  g_MusicKeptNumber = music_number;
  MusicBankLoadCopy((u8) (g_MusicBankNumbers[(music_number & 0xFF) - 1]));
  func_80024734(0, 0);
  sound_music_play();
  if ((time & 0xFF) != 0)
  {
    MusicFadeStep(1, time & 0xFF);
    return;
  }
  sound_music_level_set(0, g_MusicKeptLevel, 0);
}

s32 sound_music_play();                                  /* extern */
s32 sound_music_level_set(s32, u8, s32);                          /* extern */
s32 MusicFadeStep(s32, s32);                            /* extern */

/* func_800551C8 */
void MusicStart(s32 unused, s32 time) {
    s32 time_byte;

    sound_music_play();
    time_byte = time & 0xFF;
    if (time_byte != 0) {
        MusicFadeStep(1, time_byte);
        return;
    }
    sound_music_level_set(0, g_MusicKeptLevel, 0);
}

void sound_seq_stop_copy(s32 slot);

/* The core of the music request (MusicPlay / MusicLoadStart). Idle
   (g_MusicLevelRequest[0] == 0): mode 0 issues sound_music_level_set(0, 0x9B, vol) and marks
   the request pending as {1, 0x9B}; mode 1 issues 0x64 and then the
   g_MusicKeptLevel track at vol; any other mode does nothing. Pending: once
   sound_music_play_check reports 1 and sound_music_level_get returns the pending id
   g_MusicLevelRequest[1], sound_seq_stop_copy(0) runs and, if an id was pending, the
   g_MusicKeptLevel track is re-issued; then (or straight away when
   sound_music_play_check != 1) the request clears and 0 is returned. Otherwise 1.
   sound_music_play_check (s16) and sound_music_level_get (u16) are called undeclared here:
   the target compares their $v0 unextended. */
/* func_80055214 */
s32 MusicFadeStep(s32 mode, s32 vol)
{
    u8 ret;
    s32 cur;

    ret = 1;
    if (g_MusicLevelRequest[0] == 0) {
        switch ((u8)mode) {
        case 0:
            sound_music_level_set(0, 0x9B, (u8)vol);
            g_MusicLevelRequest[0] = 1;
            g_MusicLevelRequest[1] = 0x9B;
            break;
        case 1:
            sound_music_level_set(0, 0x64, 0);
            sound_music_level_set(0, g_MusicKeptLevel, (u8)vol);
            break;
        }
    } else {
        if (sound_music_play_check() == 1) {
            cur = sound_music_level_get();
            if (g_MusicLevelRequest[1] != cur) {
                goto end;
            }
            sound_seq_stop_copy(0);
            if (g_MusicLevelRequest[1] != 0) {
                sound_music_level_set(0, g_MusicKeptLevel, 0);
            }
        }
        ret = 0;
        g_MusicLevelRequest[0] = 0;
        g_MusicLevelRequest[1] = 0;
    }
end:
    return ret;
}

/* The music level request, MusicFadeStep's sibling on the same request
   pair g_MusicLevelRequest = {pending, level}: level is a level, time a time, both
   bytes. Answers 1 while the request is running and 0 when it is over.
   - sound_music_play_check() != 1: g_MusicKeptLevel = level (the level the other requests
     in this file re-issue) and 0; nothing is issued.
   - Idle (g_MusicLevelRequest[0] == 0): g_MusicKeptLevel = level, then sound_music_play_check is
     asked AGAIN. Still 1: sound_music_level_set(0, level, time) (the ramp to level
     over time; immediate when time is 0) and the request becomes
     {1, level}. Otherwise sound_music_level_set(0, level, 0), the level at once,
     and no request is recorded. 1 either way.
   - Pending: 1 until sound_music_level_get() (the level now) equals the pending
     level g_MusicLevelRequest[1]; then the pair is cleared and 0. The arguments
     are not read on this path.
   Callers: the script command at ScriptCommandMusicLevelSet starts it with (the
   script's s16 at +2, its byte at +3) and g_ScriptMusicLevelRequest = 2; ScriptMusicStep
   polls it with (0, 0) while g_ScriptMusicLevelRequest == 2. Both mask the result to a
   byte. No frame wait, no RNG, no sound request of its own.
   sound_music_play_check (s16) and sound_music_level_get (u16) are called undeclared, as
   in MusicFadeStep: the target compares their $v0 unextended.
   MARKED SPELLING: an old-style definition. By the bytes the function
   takes two bytes and returns a byte; the declarations in scripting*.c
   say s32 (s32, s32) and are left as they are, because the callers were
   matched under them (a u8 in a prototype they can see would change how
   they pass the arguments).
   Shape: the target keeps level twice, raw in $s0 for the three byte stores and as
   the converted byte in $s4, masked at the two calls; s32 parameters
   with casts score 30/60, and an s32 time alone 57/60. `ret` is a u8
   like MusicFadeStep's (s32: 29/60). The level now is read into `cur`
   before the pending level is loaded (the comparison written with the
   call inside it loads the global first, 53/60). The second `== 1` is
   the target's `bne $s1, $s2`: the first call's result, known to be 1
   there, stands in for the constant. */
/* func_80055310 */
s32 MusicLevelStep(level, time)
u8 level;
u8 time;
{
    u8 ret;
    s32 cur;

    ret = 1;
    if (sound_music_play_check() == 1) {
        if (g_MusicLevelRequest[0] == 0) {
            g_MusicKeptLevel = level;
            if (sound_music_play_check() == 1) {
                sound_music_level_set(0, level, time);
                g_MusicLevelRequest[0] = 1;
                g_MusicLevelRequest[1] = level;
            } else {
                sound_music_level_set(0, level, 0);
            }
        } else {
            cur = sound_music_level_get();
            if (g_MusicLevelRequest[1] == cur) {
                g_MusicLevelRequest[0] = 0;
                g_MusicLevelRequest[1] = 0;
                ret = 0;
            }
        }
    } else {
        g_MusicKeptLevel = level;
        ret = 0;
    }
    return ret;
}

s32 MusicBankLoad(s32);                               /* extern */

/* Ruling of 2026-10-14 (docs/naming.md): Does what MusicBankLoad does; nothing read says why there are two. */
/* func_80055400 */
void MusicBankLoadCopy(s32 music_bank_number) {
    MusicBankLoad(music_bank_number & 0xFF);
}
