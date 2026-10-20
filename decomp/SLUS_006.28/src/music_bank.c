/* Lunar SSSC — music_bank: MusicBankLoad (a piece's bank through
   sound_music_load) and an empty function after it. The first split had it
   in misc_f.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

void sound_music_load(s32);

/* The music request's bank load, narrowed to a byte: MusicPlay and
   MusicBankLoadCopy pass g_MusicBankNumbers[id - 1] (the track's bank number) and this
   hands its low 8 bits to sound_music_load, which clamps it, turns it into a
   LUNADATA bank index and loads that bank into slot 2 (sound.md). */
/* Ruling of 2026-10-14 (docs/naming.md): Not exactly sound_music_load: the number is cut to a byte first; named for what it does. */
/* func_80055420 */
void MusicBankLoad(u8 track)
{
    sound_music_load(track);
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_80055440() {
}
