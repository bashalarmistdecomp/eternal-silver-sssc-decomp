/* Lunar SSSC MENU.BIN overlay — libpress_vlc: VLC.OBJ of Sony's libpress, release 4.1
   (0x8019731C-0x80197690): the size word DecDCTvlcSize keeps in .text
   (D_8019731C), DecDCTvlcSize (0x80197320) and DecDCTvlc. Both were written
   in assembly (handwritten `add` / `addi`, a data word inside the code,
   registers carried across the two entry points) and stay asm:
   config/asm_only.txt holds the evidence. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80197320 */
INCLUDE_ASM("asm/nonmatchings/vlc", DecDCTvlcSize);

/* func_80197350 */
INCLUDE_ASM("asm/nonmatchings/vlc", DecDCTvlc);
