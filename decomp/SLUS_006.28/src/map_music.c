/* Lunar SSSC — map_music: the music the map and its script ask for:
   MapMusicSet and ScriptMusicStep (a level request a script left standing).
   The first split had it in scripting.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
#include "chr.h"

extern s32 g_ScriptMusicLevelRequest;
s32 MusicFadeStep(s32, s32);
s32 MusicLevelStep(s32, s32);
s32 ScriptMusicStep(void);  
/* far_syms: g_StartFirstStep (target reaches it %hi/%lo) */

extern s32 g_StartFirstStep;

/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_80058ED0 */
void StartFirstStepSet(void) {
    g_StartFirstStep = 1;
}
extern u8 D_800B8DF8;
s32 MusicPlay(u8 id, s32 unused, u8 vol);
s32 MusicKindGet(u8 music_number);

/* THE MAP'S BGM CHANGE: makes music number `bgm` the current
   one and answers its KIND. Both callers are in map_load.c,
   MapReload and MapSet (the map entry), and both say
   g_MusicKind = MapMusicSet(g_MusicNumber, g_MusicKind): the map's BGM
   number (the area record's byte +0xF or the caller's override) and the
   kind this function answered the last time.

   A kind is MusicKindGet(number) & 0xFF: 1 for number 0x3C (the CD-audio
   track, sound.md "BGM numbers"), 2 for 0x44 and up, 0 for any other.

   1. While the script's music request is pending (g_ScriptMusicLevelRequest != 0: 1 the
      fade MusicFadeStep, 2 the level change MusicLevelStep) it calls
      ScriptMusicStep(), which polls that request and clears g_ScriptMusicLevelRequest when
      it is over. The loop holds nothing else: no frame call.
   2. bgm != 0:
      - kind == 1 (the CD track was the current one):
        MusicPlay(0, 1, 0), the stop; the 1 is the kind's low byte,
        an argument MusicPlay does not read;
      - kind = MusicKindGet((u8)bgm) & 0xFF;
      - kind == 1 (bgm is 0x3C): D_800B8DF8 = (u8)bgm and NO request --
        this function does not start the CD track;
      - any other kind: MusicPlay((u8)bgm, 0, 0), the music request
        (it refuses numbers from 0x45 up by itself), and D_800B8DF8 = 0.
   3. bgm == 0: MusicPlay(0, (u8)kind, 0), the stop, and kind = 0.
   Returns kind as 16 bits.

   D_800B8DF8 (one byte) has no other reference: these two stores are the
   only $gp accesses to its address in the image, and no other C or asm
   names it.

   No RNG draw of its own. MusicPlay's fade loop runs field or battle
   frames (music_play.c); the callees were not followed further than that.

   Shape, each measured by changing it alone in this text:
   - The answer lives in the SECOND PARAMETER: one variable that is the
     old kind on the way in, is overwritten, and is returned. Every read
     of a u16 parameter is masked, the `return` included, even straight
     after `kind = 0`; that is the target's `andi $v0, $s0, 0xFFFF`. With
     the answer in a local of its own the three masks are plain moves
     (u16 or s32 local: 51/54; u8: 47/54). The Japanese build's twin has
     the same masks: under cc272 this text is 43/55 against it, the
     local-variable text 40/55.
   - ONE `return` at the end. The target's three copies of the mask are
     the join's one instruction, which the delay-slot filler copied into
     the two jumps; with a `return` in each arm it is 35/54 (on a
     separate local).
   - The arm order is the source's: bgm != 0 first (the other way round
     45/54) and kind == 1 first (50/54).
   - The stop's second argument is the kind's low byte, not a literal 1
     (53/54): the target masks $s0 there.
   The parameters are u16 as map_load.c declares them (and the first split's
   scripting_c.c did) (s32 parameters with the masks written out give the
   same 54 words, and so does an s32 return type). The callee prototypes are
   the definitions' (music_play.c, music_voice.c); s32 prototypes with the
   masks written out, or none at all, give the same words too. */
/* func_80058EE4 */
u16 MapMusicSet(u16 bgm, u16 kind)
{
    while (g_ScriptMusicLevelRequest != 0) {
        ScriptMusicStep();
    }
    if (bgm != 0) {
        if (kind == 1) {
            MusicPlay(0, (u8)kind, 0);
        }
        kind = (u8)MusicKindGet(bgm);
        if (kind == 1) {
            D_800B8DF8 = bgm;
        } else {
            MusicPlay(bgm, 0, 0);
            D_800B8DF8 = 0;
        }
    } else {
        MusicPlay(0, (u8)kind, 0);
        kind = 0;
    }
    return kind;
}

s32 MusicFadeStep(s32, s32);                            /* extern */
s32 MusicLevelStep(s32, s32);                            /* extern */

/* func_80058FBC */
s32 ScriptMusicStep(void) {
    s32 pending;

    pending = 0;
    if (g_ScriptMusicLevelRequest == 1) {
        pending = MusicFadeStep(0, 0) & 0xFF;
    } else if (g_ScriptMusicLevelRequest == 2) {
        pending = MusicLevelStep(0, 0) & 0xFF;
    }
    if (pending == 0) {
        g_ScriptMusicLevelRequest = 0;
    }
    return pending;
}
