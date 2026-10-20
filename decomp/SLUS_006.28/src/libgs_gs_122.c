/* PSY-Q 4.5 libgs, member gs_122.o: GsGetTimInfo. The first split had it in
   psyq_cd_k.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: GsDrawOt */

/* cc97_lib: GsGetTimInfo */

/* libgs GsGetTimInfo (GS_122.OBJ): reads the header of a TIM image into a GsIMAGE. `tim`
   points at the TIM's flag word (the word after the 0x10 id). pmode = the flag word. With
   the CLUT bit (bit 3) set, the next block is the CLUT: its byte length, then x, y, width
   and height as 16-bit pairs (cx, cy, cw, ch) and the CLUT data (clut); the pixel block
   starts that byte length after the length word. The pixel block is laid out the same way:
   byte length, px, py, pw, ph, then the data (pixel). Without the CLUT bit the pixel block
   follows the flag word directly and cx / cy / cw / ch / clut are left untouched. */
/* func_8008FC48 */
void GsGetTimInfo(unsigned long *tim, GsIMAGE *im)
{
    unsigned long *pix;

    im->pmode = *tim;
    if ((im->pmode >> 3) & 1) {
        tim++;
        pix = tim + (*tim / 4);
        tim++;
        im->cx = *(unsigned short *)tim;
        im->cy = *((unsigned short *)tim + 1);
        tim++;
        im->cw = *(unsigned short *)tim;
        im->ch = *((unsigned short *)tim + 1);
        tim++;
        im->clut = tim;
        pix++;
        im->px = *(unsigned short *)pix;
        im->py = *((unsigned short *)pix + 1);
        pix++;
        im->pw = *(unsigned short *)pix;
        im->ph = *((unsigned short *)pix + 1);
        pix++;
        im->pixel = pix;
    } else {
        tim += 2;
        im->px = *(unsigned short *)tim;
        im->py = *((unsigned short *)tim + 1);
        tim++;
        im->pw = *(unsigned short *)tim;
        im->ph = *((unsigned short *)tim + 1);
        tim++;
        im->pixel = tim;
    }
}

INCLUDE_PAD(12);
