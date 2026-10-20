/* Lunar SSSC — overlay: the MENU.BIN overlay: the disc holding the movie
   found or asked for, the overlay loaded and its build stamp checked, and
   MoviePlay, which shows a movie through it. The name is the developers'
   own: OVERLAY.C, the source file overlay_load's assertion names ('OVERLAY
   ERROR', line 0xB2); the file's other functions have it by being of its
   group. The first split had it in cdrom_load.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"

extern u8 g_OverlayFileName[];
extern u8 D_80011740[];
extern u8 D_80011750[];
s32 cd_file_search(void *file, s32 name);  /* extern */
s32 MusicStop();
s32 MovieScreenClose();
void func_800230D4(void);  /* TU defines this below */
s32 func_800590B8();
void func_800230FC(void);  /* TU defines this below */
s32 build_stamp_compare_copy(s32);
s32 overlay_stamp_check(void);  /* TU defines this below */
s32 resource_load_run();
s32 assert_fail(s32 *, s32 *, s32);
extern s32 g_PadPressRepeatWait;
extern s32 g_PadPressRepeatPreviousButtons;
extern s32 g_FrameVSyncCount;
extern s32 g_PadPressedButtons;
extern s32 g_PadButtons;

/* func_80023070 */
unsigned char pad_press_repeat(void)
{
  s32 wait_left;
  if (g_PadButtons == g_PadPressRepeatPreviousButtons)
  {
    if (g_PadButtons != 0)
    {
      wait_left = g_PadPressRepeatWait - g_FrameVSyncCount;
      g_PadPressRepeatWait = wait_left;
      if (wait_left < 0)
      {
        g_PadPressedButtons = g_PadButtons;
        g_PadPressRepeatWait = 4;
      }
    }
  }
  else
  {
    if (g_PadButtons == 0)
    {
      g_PadPressRepeatWait = 0x20;
    }
    g_PadPressRepeatPreviousButtons = g_PadButtons;
  }
}

s32 MusicStop();                                  /* extern */
s32 MovieScreenClose();                                  /* extern */

/* Ruling of 2026-10-14 (docs/naming.md): Reached, and does work: two calls whose joint purpose nothing read states. */
void func_800230D4(void) {
    MovieScreenClose();
    MusicStop();
}

s32 func_800590B8();                                  /* extern */

/* Ruling of 2026-10-14 (docs/naming.md): Only calls an empty function; its one call passes nothing, and nothing read says what it was for. */
void func_800230FC(void) {
    func_800590B8();
}
/* far_syms: g_OverlayMovieFileNameFormat (target reaches it %hi/%lo) */
extern u8 g_OverlayMovieFileNameFormat[];

/* func_8002311C */
s32 overlay_movie_search(movie)
s32 movie;
{
    s32 file[6];
    s32 name[8];
    fmt_sprintf(name, g_OverlayMovieFileNameFormat, movie);
    return cd_file_search(file, name);
}

/* far_syms: g_OverlayLunadataFileName (target reaches it %hi/%lo) */
extern u8 g_OverlayLunadataFileName[];

/* func_8002315C */
s32 overlay_lunadata_search(void) {
    s32 buf[6];
    return cd_file_search(buf, (s32)g_OverlayLunadataFileName);
}
/* far_syms: g_OverlayFileName (target reaches it %hi/%lo) */
s32 file_load(s32 *, s32, s32, s32);
s32 EnterCriticalSection();
void overlay_load(void);  /* TU defines this below */
void PictureShow(s32 picture);  /* extern */
s32 cd_seek_check(void);  
/* func_80023184 */
void overlay_disc_wait(s32 movie)
{
  s32 failed_flag;
  s32 disc_picture;
  failed_flag = 0;
  if (overlay_movie_search() != 0)
  {
    return;
  }
  disc_picture = (unsigned char) ((movie < 0x24) ^ 1);
  resource_load_run(0x257);
  while (1)
  {
    if (cd_change_wait(disc_picture | failed_flag) != 0)
    {
      return;
    }
    if (cd_seek_check() != 0)
    {
      failed_flag = 2;
      continue;
    }
    cd_dir_init();
    failed_flag = (overlay_lunadata_search() == 0) << 1;
    if (overlay_movie_search(movie) == 0)
    {
      continue;
    }
    file_load((s32 *) g_OverlayFileName, 0x80196000, 0, 0x800);
    if (overlay_stamp_check() != 0)
    {
      failed_flag = 2;
      continue;
    }
    return;
  }

}

s32 file_load(s32 *, s32, s32, s32);                          /* extern */
s32 assert_fail(s32 *, s32 *, s32);                       /* extern */
s32 EnterCriticalSection();                                  /* extern */
s32 overlay_stamp_check(void);  /* TU defines this below */

/* func_80023240 */
void overlay_load(void) {
    file_load((s32 *)g_OverlayFileName, 0x80196000, 0, -1);
    if (overlay_stamp_check() != 0) {
        assert_fail((s32 *)D_80011740, (s32 *)D_80011750, 0xB2);
    }
    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
}

s32 build_stamp_compare_copy(s32);                                 /* extern */

/* func_800232B0 */
s32 overlay_stamp_check(void) {
    return build_stamp_compare_copy(0x80196000);
}
/* far_syms: g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_PadActuatorLevels g_PadActuator1Level g_MovieFromMenu */

extern u8 *g_MovieActuatorLists[];
extern u8 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
extern s16 g_PadActuatorListWait;
extern s8 g_PadActuatorLevels;
extern s8 g_PadActuator1Level;
extern s16 g_MovieFromMenu;

/* "Play event n": every movie and every bromide runs through here (the
   story script's opcode ScriptCommandMoviePlay, the opening StartMoviePlay with n = 0,
   the theater MenuUseMoviePlay, which sets g_MovieFromMenu = 1 around the call).

   The 47-word switch table ends at 0x8001181C, and seven strings stand
   between it and the TU's next table at 0x800118C8 (D_8001181C,
   D_80011830, D_80011848, D_80011864, D_80011884, D_80011898,
   D_800118BC) -- not tables. The boot_data piece ends at 0x80011760 in
   program.ld, so overlay.c.o(.rodata) starts at 0x80011760: the
   compiler's table for this function comes first, and the seven strings
   follow it as raw INCLUDE_RODATA bridges (no alignment; xa.c's .rodata)
   in the original order. That part is 0x168 bytes, a multiple of 8, so the `.align 3`
   before the TU's next table still pads nothing.

   1. func_800230D4() (MovieScreenClose, then MusicStop) and event flag
      n + 0x154 set (StoryBitSet), which is what lists the scene in the
      theater.
   2. n in 0x37..0x43: bromide n - 0x37, PictureShow (LUNADATA resource
      0x24A + k). Nothing else of step 3 runs.
   3. Otherwise a movie: overlay_disc_wait(n) (the disc check for "_B%03d.STR"),
      overlay_load() (MENU.BIN to 0x80196000), then the switch -- ten
      movies start a DualShock script from the pointer table g_MovieActuatorLists
      (the trio g_PadActuatorList = script, g_PadActuatorRestFlags = 0x3F, g_PadActuatorListWait = 0):
        n     4     0x11  0x15  0x18  0x1D  0x24  0x25  0x26  0x29  0x32
        slot  0     1     2     3     4     5     9     6     7     8
      (0x25 takes the LAST slot and is the last case in the code). Every
      other n starts none. movie_play(n), the overlay's player, runs the
      movie. Outside the theater (g_MovieFromMenu == 0), movie 0x23 is followed
      by overlay_disc_wait(0x24): the disc 2 prompt. Then the portrait cache is rebuilt
      -- it starts at 0x801A9B00, just past MENU.BIN's image (0x12940 bytes
      from 0x80196000), and the code treats it as lost: FaceSlotReset()
      forgets the six slot ids and FaceListLoad() loads the party's
      portrait blocks again (g_FaceList[0..5], then g_FaceList[6]).
   4. func_800230FC() (func_800590B8, an empty function) and the vibration
      state is dropped: script pointer, g_PadActuatorRestFlags, g_PadActuatorListWait and both
      actuator bytes g_PadActuatorLevels / g_PadActuator1Level to 0.

   No RNG call and no frame wait of its own; both live in the callees.

   Codegen: a plain switch whose cases are written in the target's address
   order, each with the whole trio -- cross-jumping merges the two constant
   stores and the pointer store into the one shared tail and leaves each
   case its load, the jump and `li 0x3F` in the delay slot. The scalars are
   far (lui $at stores); the table is an incomplete array, far by
   declaration. */
/* func_800232D4 */
void MoviePlay(s32 movie) {
    func_800230D4();
    StoryBitSet(movie + 0x154);
    if ((u32)(movie - 0x37) < 0xD) {
        PictureShow(movie - 0x37);
    } else {
        overlay_disc_wait(movie);
        overlay_load();
        switch (movie) {
        case 4:
            g_PadActuatorList = g_MovieActuatorLists[0];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x11:
            g_PadActuatorList = g_MovieActuatorLists[1];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x15:
            g_PadActuatorList = g_MovieActuatorLists[2];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x18:
            g_PadActuatorList = g_MovieActuatorLists[3];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x1D:
            g_PadActuatorList = g_MovieActuatorLists[4];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x24:
            g_PadActuatorList = g_MovieActuatorLists[5];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x26:
            g_PadActuatorList = g_MovieActuatorLists[6];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x29:
            g_PadActuatorList = g_MovieActuatorLists[7];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x32:
            g_PadActuatorList = g_MovieActuatorLists[8];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        case 0x25:
            g_PadActuatorList = g_MovieActuatorLists[9];
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            break;
        }
        movie_play(movie);
        if (g_MovieFromMenu == 0 && movie == 0x23) {
            overlay_disc_wait(0x24);
        }
        FaceSlotReset();
        FaceListLoad();
    }
    func_800230FC();
    g_PadActuatorList = 0;
    g_PadActuatorRestFlags = 0;
    g_PadActuatorListWait = 0;
    g_PadActuatorLevels = 0;
    g_PadActuator1Level = 0;
}
