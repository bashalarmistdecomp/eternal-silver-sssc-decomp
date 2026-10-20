# Audio — Sound/Music Playback

This page maps the main executable's (`SLUS_006.28`) own sound code by file,
and describes the two requests on the music's level that the event scripts
make. What the code does — the sound banks and their command list, the bank
and sequence slots, the sound requests, the reverb, which music plays for a
music number, the CD audio track and the XA voices — is on
[sound.md](../../sound.md). Sony's side, libsnd and libspu, is on
[libsnd-map.md](../libsnd-map.md), [libetc-libsnd.md](psyq/libetc-libsnd.md)
and [libspu.md](psyq/libspu.md).

## The files

| File | What it holds | Described in |
|---|---|---|
| `sound.c` | The developers' `SOUND.C`: the assertions of `sound_buf_add` (`0x80024298`) (line 0x334), `sound_vab_head_open` (`0x80024314`) (0x363) and `sound_vab_body_transfer` (`0x800243D0`) (0x37C) name it. The sound heap; resource commands 13, 14, 15 and 17; the bank table (load, close, the empty stand-in); the area bank, the music bank and their seeks; the sequence slots and `sound_seq_play` (`0x80023B24`); `sound_effect_play` (`0x80024514`); the music's level and fade; `sound_init` (`0x80023D54`) and the reverb. | [sound.md](../../sound.md) §2, §6, §7, §"BGM numbers" |
| `music_play.c` | `MusicPlay` (`0x80054F2C`), `MusicLoadStart` (`0x80055118`), `MusicStart` (`0x800551C8`), `MusicFadeStep` (`0x80055214`), `MusicLevelStep` (`0x80055310`), `MusicBankLoadCopy` (`0x80055400`), `SoundMonoSet` (`0x800550D4`). | [sound.md](../../sound.md) §"BGM numbers"; below |
| `music_bank.c` | `MusicBankLoad` (`0x80055420`), the one caller of `sound_music_load` (`0x80023E9C`). | [sound.md](../../sound.md) §"BGM numbers" |
| `music_voice.c` | `MusicKindGet` (`0x80055448`); the XA voices `VoiceSet` (`0x80055470`), `VoiceStart` (`0x800554C4`), `VoiceStep` (`0x80055524`); `SoundMapSet` (`0x800555CC`) and `SoundPlay` (`0x800555F0`); `MusicInit` (`0x80055768`) and `MusicStop` (`0x800557AC`). | [sound.md](../../sound.md) §6 "Sound effects", §"The XA voice clips" |
| `xa.c` | The XA file (`xa_file_set` (`0x8002347C`), found through `cd_file_search` (`0x800234C4`)), a play's start, end and stop, the drive's commands and checks, and the clips' places (`xa_voice_get` (`0x800238BC`)). | [sound.md](../../sound.md) §"The XA voice clips" |
| `cd_track.c` | The table of contents, a track's play and its switch (`cd_track_play` (`0x8001B684`), `cd_track_switch` (`0x8001B71C`)). | [sound.md](../../sound.md) §"CD audio" |
| `map_music.c` | `MapMusicSet` (`0x80058EE4`), the map's music change, and `ScriptMusicStep` (`0x80058FBC`); also `StartFirstStepSet` (`0x80058ED0`), which is not sound (it sets the start screen's first step). | [sound.md](../../sound.md) §"BGM numbers"; below |

Sound effects are asked for by number through `SoundPlay` from all over the
program; the script's sound command and the sounds it plays are on
[npc-scripting.md](npc-scripting.md), the battle's on
[sound.md](../../sound.md) §"The battle's sound requests".

Nothing in the image calls `sound_seq_rotate_play` (`0x80023CEC`),
`sound_bank_close_all` (`0x80024744`), `sound_end` (`0x80024764`),
`sound_music_level_reset` (`0x800245D0`) or `MusicInit`. Three functions of
the group are empty in this build: `func_80024734` (`MusicLoadStart` calls it
with 0, 0), `func_8002473C` (`GameStart` (`0x80055B60`) calls it with 0 and
0xF) and `func_80055468` (`MusicPlay` tests its answer, which it never sets).
A fourth, `func_80054F24`, answers 0 and does nothing else; its one call is in
the arm of `MusicPlay` that the build never takes ([sound.md](../../sound.md)
§"BGM numbers"). `sound_seq_stop_copy` (`0x80024590`) and
`sound_area_load_copy` (`0x800245B0`) do what `sound_seq_stop` and
`sound_area_load` do, behind one call; nothing read says why there are two.

## The script's two music requests — `MusicFadeStep` (`0x80055214`) / `MusicLevelStep` (`0x80055310`) (music_play.c)

Both work on the request pair `g_MusicLevelRequest` = `{pending, level}`
(u8[2]) and answer 1 while the request is running, 0 when it is over. The
script side polls them from `ScriptMusicStep` (`0x80058FBC`):
`g_ScriptMusicLevelRequest` = 1 polls `MusicFadeStep(0, 0)` and 2 polls
`MusicLevelStep(0, 0)`, and a 0 answer clears `g_ScriptMusicLevelRequest`.
`ScriptCommandMusicPlay` (`0x8005E494`) sets 1 for a command that stops the
music with a time (number 0, time not 0); `ScriptCommandMusicLevelSet`
(`0x8005E5A4`) sets 2. Each of the two commands first waits out a request
still running (`ScriptMusicWait` (`0x8005A364`), script frames meanwhile), and
`MapMusicSet` (`0x80058EE4`) polls `ScriptMusicStep` with no frame between
until the request is over before it changes the music.

**`MusicFadeStep(mode, time)`**, the fade around a change of piece. With no
request running: mode 0 has the level go to 0x9B over `time`
(`sound_music_level_set(0, 0x9B, time)`) and records the request
`{1, 0x9B}`; mode 1 puts the level at 0x64 at once and has it go to the kept
level `g_MusicKeptLevel` over `time`, and records nothing; the answer is 1.
With a request running the arguments are not read: when
`sound_music_play_check()` (`0x80023CC0`) answers 1 and the level now
(`sound_music_level_get()` (`0x80024728`)) is not yet the request's level,
the answer is 1. When the level is there, `sound_seq_stop_copy(0)` stops the
music and, the request's level being non-zero, the kept level is set again at
once; then, or straight away when `sound_music_play_check` does not answer 1,
the pair is cleared and the answer is 0. `MusicLoadStart` and `MusicStart`
use mode 1 to bring a new piece in.

**`MusicLevelStep(level, time)`**, started by the script command
`ScriptCommandMusicLevelSet` (`0x8005E5A4`) with the command's byte 2 as the
level and byte 3 as the time, is a LEVEL change:

1. `sound_music_play_check()` (`0x80023CC0`) `!= 1`: `g_MusicKeptLevel =
   level`, answer 0, nothing issued.
2. Idle (`g_MusicLevelRequest[0] == 0`): `g_MusicKeptLevel = level`, then
   `sound_music_play_check()` is asked again. Still 1:
   `sound_music_level_set(0, level, time)` (`0x800246D4`) and the request
   becomes `{1, level}`. Otherwise `sound_music_level_set(0, level, 0)` and
   no request is recorded. Answer 1 either way.
3. Pending: answer 1 until `sound_music_level_get()` (`0x80024728`) (the
   level now, `g_SoundMusicLevel`) equals the pending level; then the pair is
   cleared and the answer is 0. The arguments are not read on this path.

`sound_music_level_set(0, level, time)` (`0x800246D4`) applies the level at
once for time 0 (`sound_music_volume_set(level)` (`0x80023C4C`), scaled
`level * 0x50 / 0xFF`) and otherwise arms a ramp (target
`g_SoundMusicFadeTarget`, length `g_SoundMusicLevelFadeLength = time * 0x1E`,
counter `g_SoundMusicLevelFadeCount`, start `g_SoundMusicFadeStart`) that
`sound_music_level_step` (`0x800245F0`) steps once a frame.
`g_MusicKeptLevel` is the level the other requests of the file re-issue.
`sound_music_play_check()` (`0x80023CC0`) returns `SsIsEos(*(s16
*)g_SoundSeqSlots, 0)` (`0x80095AF8`): libsnd's answer for the score on
sequence slot 0, the music's, which both functions read as 1 while the music
plays. By the bytes `MusicLevelStep` takes two bytes and returns a byte; the
declarations its callers see (lns_intp.c, map_music.c) say
`s32 (s32, s32)`, so it is an old-style definition with `u8` parameters.
