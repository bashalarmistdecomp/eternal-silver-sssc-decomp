/* Lunar SSSC MENU.BIN overlay — movie: the game's movie player
   (0x8019610C-0x80196BDC; movie_play is the entry the boot EXE calls).
   Every function is C but movie_slice_load, still assembly (README,
   "Outstanding"). The build must stay byte-matching. Compiled -G0 (cc_flags.txt): the overlay never
   uses $gp. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include <libpress.h>
extern s32 g_MovieFrameNumber;
extern s32 D_801A88D8;
extern s32 D_801A8914;
void movie_vram_block_move(s32, s32, s32, s32);                    /* extern */
void movie_vram_clear(s32, s32, s32, s32);                        /* extern */
extern char D_80196000[];
s32 strcmp(s8 *s1, s8 *s2);                      /* strcmp */

/* func_8019610C */
s32 movie_stamp_compare(s8 *s)
{
    return strcmp(s, D_80196000);
}

/* The movie file's record: movie_file_search fills its position and size,
   movie_stream_play reads the position. */
extern CdlFILE D_801A8924;

s32 cd_file_search(CdlFILE *fp, char *name);   /* boot EXE: the game's CdSearchFile */

/* movie_file_search(name): looks name up in the game's file table
   (cd_file_search fills D_801A8924); 1 when it is NOT there. */
/* func_80196130 */
s32 movie_file_search(char *name) {
    return cd_file_search(&D_801A8924, name) == 0;
}

/* func_80196158 */
void movie_vram_clear(s32 x, s32 y, s32 w, s32 h)
{
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    ClearImage(&rect, 0, 0, 0);
    DrawSync(0);
}

/* func_8019619C */
void movie_vram_block_move(s32 x, s32 y, s32 dx, s32 dy)
{
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = rect.h = 16;
    MoveImage(&rect, dx, dy);
}


/* func_801961D8 */
void movie_vram_save(void) {
    s32 temp_s0;
    s32 var_s1;
    s32 var_s2;

    var_s1 = 0;
    var_s2 = 0xE0;
    do {
        temp_s0 = var_s1 * 0x10;
        movie_vram_block_move(0x140, temp_s0, temp_s0, 0xD0);
        movie_vram_block_move(0x140, var_s2, temp_s0, 0x1B0);
        var_s1 += 1;
        var_s2 += 0x10;
    } while (var_s1 < 0xC);
    DrawSync(0);
    movie_vram_clear(0, 0, 0x150, 0xC0);
    movie_vram_clear(0, 0xE0, 0x150, 0xC0);
}
/* func_80196278 */
void movie_vram_restore(void) {
    s32 temp_s0;
    s32 var_s1;
    s32 var_s2;

    var_s1 = 0;
    var_s2 = 0xE0;
    do {
        temp_s0 = var_s1 * 0x10;
        movie_vram_block_move(temp_s0, 0xD0, 0x140, temp_s0);
        movie_vram_block_move(temp_s0, 0x1B0, 0x140, var_s2);
        var_s1 += 1;
        var_s2 += 0x10;
    } while (var_s1 < 0xC);
    DrawSync(0);
    movie_vram_clear(0, 0, 0x140, 0x1C0);
}
extern s32 D_80197728;
extern s32 D_8019772C;
void movie_stream_play(s32 start, s32 end);

/* func_80196304 */
s32 movie_file_play(char *name, s32 start, s32 end)
{
    if (movie_file_search(name) != 0) {
        return -1;
    }
    D_801A88D8 = 0;
    D_80197728 = 0;
    D_8019772C = 0;
    SetDispMask(0);
    movie_vram_save();
    movie_stream_play(start, end);
    SetDispMask(0);
    movie_vram_restore();
    return D_801A88D8;
}
extern s32 D_80197724;
extern CdlFILE D_801A88B4;
extern s32 D_801A88D0;
extern s32 D_801A88D4;
s32 CdPosToInt(CdlLOC *p);                           /* CdPosToInt */
CdlLOC *CdIntToPos(s32 i, CdlLOC *p);                /* CdIntToPos */
void movie_frames_play(CdlFILE *fp);  /* TU defines this below */

/* func_8019638C */
void movie_stream_play(s32 start, s32 end)
{
    D_801A88D0 = start;
    D_801A88D4 = end;
    g_MovieFrameNumber = 0;
    SpuSetCommonCDMix(1);
    SpuSetCommonCDVolume(0x7000, 0x7000);
    CdIntToPos(CdPosToInt(&D_801A8924.pos) + D_801A88D0 * D_80197724,
                  &D_801A88B4.pos);
    movie_frames_play(&D_801A88B4);
}

typedef struct {
    u_long *vlcbuf[2];  /* 0x00 VLC buffers (double) */
    s32 vlcid;          /* 0x08 buffer the next VLC decode fills */
    u_short *imgbuf;    /* 0x0C slice image buffer */
    RECT rect[2];       /* 0x10 VRAM frame buffers (double) */
    s32 rectid;         /* 0x20 buffer being transferred */
    RECT slice;         /* 0x24 one DecDCTout's area */
    s32 isdone;         /* 0x2C whole frame decoded */
} DECENV;
s32 debug_printf();
void pad_frame_step(void);
s32 CdControlB(u8, u8 *, u8 *);
void movie_decode_init(DECENV *, s32, s32, s32, s32);
void movie_stream_start(CdlLOC *loc, void (*callback)());  /* TU defines this below */
void movie_slice_load(void);
s32 movie_frame_decode(DECENV *);
void movie_decode_wait(DECENV *, s32);
extern char D_80196020[];
extern u8 D_80197720;
extern u8 D_80197721;
extern CdlLOC D_801A88CC;
extern DECENV D_801A88E4;
extern s32 g_PadButtons;
extern u16 g_MovieSkipAllowed;

/* movie_frames_play: the movie's play loop (the SDK movie samples' anim()),
   called once per movie by movie_stream_play with the CdlFILE whose position
   it has just computed (D_801A88B4).
   - copies the file record, keeps its minute:second:sector in the stream
     start position D_801A88CC (CdlLOC; track untouched) and prints it
     ("MOV.STR pos (%x:%x:%x)", D_80196020) from there;
   - sets up the decode environment D_801A88E4 (DECENV: VRAM buffers at
     (0,0) and (0,0xE0), movie_decode_init), starts the stream at D_801A88CC
     with movie_slice_load as the slice callback (movie_stream_start) and decodes
     the first frame's VLC data (movie_frame_decode);
   - per frame: DecDCTin (DecDCTin, mode 3 = 24-bit) and DecDCTout
     (DecDCTout, slice.w * slice.h / 2 words) of the current frame,
     VLC-decodes the next one (movie_frame_decode, which sets the ended flag
     D_801A8914 at the end frame), waits for the decode (movie_decode_wait),
     VSync (VSync), then shows the buffer NOT being decoded into:
     SetDefDispEnv / SetDefDrawEnv (SetDefDispEnv / SetDefDrawEnv) on
     rect[rectid ? 0 : 1], a 24-bit display (isrgb24 = 1, disp.w * 2 / 3)
     at screen (D_80197720, D_80197721) as signed bytes, 0xE0 x 0xC0,
     PutDispEnv / PutDrawEnv (PutDispEnv / PutDrawEnv) and
     SetDispMask(1) (SetDispMask);
   - leaves when the ended flag is 1 (before the frame tick), else runs
     pad_frame_step (the boot EXE's one-line wrapper round the vibration
     driver step func_8001F508; video.md) and leaves early, setting D_801A88D8 = 1 (skipped), on pad bit 0x800 of
     g_PadButtons while g_MovieSkipAllowed is non-zero, or on the pad's low half
     == 0x4F;
   - shutdown: SpuSetCommonCDVolume(0, 0) (CD audio volume left/right; the caller
     set 0x7000), CdControlB Mute (0xB), Setmode (0xE, param 0x80 = double
     speed), DecDCToutCallback(0) (DecDCToutCallback), the CD sync and ready
     callbacks cleared (CdDataCallback, CdReadyCallback), Pause (9), Demute
     (0xC); the frame number g_MovieFrameNumber = 0xFFFF.
   Shapes: the two skip tests are two ifs, each storing 1 and breaking
   (one `||` condition with a shared store does not hoist the constant 1
   into $s3: 167/184); the debug print reads the fields back from
   D_801A88CC, not from the copy (175/184 that way).
   MARKED SPELLING (unused local): pad8 and pad16. The target frame (0xE8)
   has untouched holes at sp+0x90 (8 bytes, between draw and result) and
   sp+0xA0 (16 bytes, between result and file); no instruction reads or
   writes them -- the original's locals, contents unknown. Without them
   the frame is 0x20 short and every local moves (151/184). */
/* func_80196414 */
void movie_frames_play(CdlFILE *fp) {
    DISPENV disp;
    DRAWENV draw;
    s32 id;
    u8 pad8[8];
    u_char result[8];
    u8 pad16[16];
    CdlFILE file;
    u_char param[4];

    file = *fp;
    D_801A88CC.minute = file.pos.minute;
    D_801A88CC.second = file.pos.second;
    D_801A88CC.sector = file.pos.sector;
    debug_printf(D_80196020, D_801A88CC.minute, D_801A88CC.second, D_801A88CC.sector);
    movie_decode_init(&D_801A88E4, 0, 0, 0, 0xE0);
    movie_stream_start(&D_801A88CC, movie_slice_load);
    movie_frame_decode(&D_801A88E4);
    while (1) {
        DecDCTin(D_801A88E4.vlcbuf[D_801A88E4.vlcid], 3);
        DecDCTout((u_long *)D_801A88E4.imgbuf, D_801A88E4.slice.w * D_801A88E4.slice.h / 2);
        movie_frame_decode(&D_801A88E4);
        movie_decode_wait(&D_801A88E4, 0);
        VSync(0);
        id = D_801A88E4.rectid ? 0 : 1;
        SetDefDispEnv(&disp, D_801A88E4.rect[id].x, D_801A88E4.rect[id].y,
                      D_801A88E4.rect[id].w, D_801A88E4.rect[id].h);
        SetDefDrawEnv(&draw, D_801A88E4.rect[id].x, D_801A88E4.rect[id].y,
                      D_801A88E4.rect[id].w, D_801A88E4.rect[id].h);
        disp.screen.w = 0xE0;
        disp.screen.h = 0xC0;
        disp.isrgb24 = 1;
        disp.screen.x = (s8)D_80197720;
        disp.screen.y = (s8)D_80197721;
        disp.disp.w = disp.disp.w * 2 / 3;
        PutDispEnv(&disp);
        PutDrawEnv(&draw);
        SetDispMask(1);
        if (D_801A8914 == 1) {
            break;
        }
        pad_frame_step();
        if (g_MovieSkipAllowed != 0 && (g_PadButtons & 0x800)) {
            D_801A88D8 = 1;
            break;
        }
        if ((u16)g_PadButtons == 0x4F) {
            D_801A88D8 = 1;
            break;
        }
    }
    SpuSetCommonCDVolume(0, 0);
    CdControlB(0xB, 0, result);
    param[0] = 0x80;
    CdControlB(0xE, param, 0);
    DecDCToutCallback(0);
    CdDataCallback(0);
    CdReadyCallback(0);
    CdControlB(9, 0, 0);
    CdControlB(0xC, 0, result);
    g_MovieFrameNumber = 0xFFFF;
}
/* func_801966F4 */
void movie_decode_init(DECENV *dec, s32 x0, s32 y0, s32 x1, s32 y1)
{
    dec->vlcbuf[0] = (u_long *)0x80156000;
    dec->vlcbuf[1] = (u_long *)0x8016B000;
    dec->vlcid = 0;
    dec->imgbuf = (u_short *)0x80190000;
    dec->rectid = 0;
    dec->isdone = 0;
    dec->rect[0].x = x0;
    dec->rect[0].y = y0;
    dec->rect[0].w = 0x150;
    dec->rect[0].h = 0xC0;
    dec->rect[1].x = x1;
    dec->rect[1].y = y1;
    dec->rect[1].w = 0x150;
    dec->rect[1].h = 0xC0;
    dec->slice.x = x0;
    dec->slice.y = y0;
    dec->slice.w = 0x18;
    dec->slice.h = 0xE0;
}

/* far_syms: D_80197730 (target reaches it %hi/%lo) */
/* far_syms: D_801A8914 (target reaches it %hi/%lo) */

void StSetRing(u_long *ring_addr, u_long ring_size);    /* StSetRing */
void movie_read_start(CdlLOC *loc);
extern u_long *D_80197730;                              /* the stream's ring buffer */

/* movie_stream_start(loc, callback): the stream's start (the SDK movie
   samples' strInit): DecDCTReset(0), the ended flag D_801A8914 = 0,
   DecDCToutCallback(callback), StSetRing(ring, 0x20 sectors),
   StSetStream(24-bit, from frame 1, no end, no callbacks), then seek and
   read from loc (movie_read_start). */
/* func_80196768 */
void movie_stream_start(CdlLOC *loc, void (*callback)()) {
    DecDCTReset(0);
    D_801A8914 = 0;
    DecDCToutCallback(callback);
    StSetRing(D_80197730, 0x20);
    StSetStream(1, 1, -1, 0, 0);
    movie_read_start(loc);
}

/* func_801967DC */
INCLUDE_ASM("asm/nonmatchings/movie", movie_slice_load);
u_long *movie_frame_next(DECENV *dec);
u_long StFreeRing(u_long *base);               /* StFreeRing */

/* movie_frame_decode: VLC-decode the next movie frame (the SDK movie samples'
   strNextVlc), called by the play loop movie_frames_play once before the loop
   and once per frame with the decode environment D_801A88E4.
   - polls movie_frame_next (strNext) for the next frame in the CD ring,
     up to 0x800000 times; gives up with -1 when none arrives;
   - flips dec->vlcid (+0x08) and VLC-decodes the frame into
     dec->vlcbuf[vlcid] (+0x00 / +0x04) with DecDCTvlc (DecDCTvlc);
   - frees the frame's ring slot (StFreeRing, StFreeRing); returns 0.
   Shapes: the sample's while loop with the countdown inside; the flip as
   `vlcid ? 0 : 1` (sltiu). */
/* func_801968D0 */
s32 movie_frame_decode(DECENV *dec) {
    s32 cnt;
    u_long *next;

    cnt = 0x800000;
    while ((next = movie_frame_next(dec)) == 0) {
        if (--cnt == 0) {
            return -1;
        }
    }
    dec->vlcid = dec->vlcid ? 0 : 1;
    DecDCTvlc(next, dec->vlcbuf[dec->vlcid]);
    StFreeRing(next);
    return 0;
}
u_long StGetNext(u_long **addr, u_long **header);  /* StGetNext */

/* movie_frame_next: fetch the next movie frame from the CD ring (the SDK movie
   samples' strNext), called by movie_frame_decode with the decode environment.
   - polls StGetNext (StGetNext) up to 0x800000 times for the next
     frame's data and its STR sector header; returns 0 when none arrives;
   - keeps the header's frame number (+0x08) in the frame clock g_MovieFrameNumber
     and sets the ended flag D_801A8914 = 1 once it reaches the end frame
     D_801A88D4 (unsigned compare);
   - remembers the frame size (header +0x10 width, +0x12 height) in
     D_80197728 / D_8019772C when it changed (the sample's ClearImage of the
     screen on a size change is not here);
   - sizes both frame rects (+0x14/+0x1C w = width * 3 / 2: 24-bit pixels in
     16-bit VRAM words; +0x16/+0x1E h = height) and the slice height (+0x2A);
   - returns the frame's data address (for DecDCTvlc and StFreeRing).
   Shapes: the sample's text as is, with the header pointer passed to
   StGetNext through a (u_long **) cast. */
/* func_80196954 */
u_long *movie_frame_next(DECENV *dec) {
    u_long *addr;
    StHEADER *sector;
    s32 cnt;

    cnt = 0x800000;
    while (StGetNext(&addr, (u_long **)&sector)) {
        if (--cnt == 0) {
            return 0;
        }
    }
    g_MovieFrameNumber = sector->frameCount;
    if (sector->frameCount >= D_801A88D4) {
        D_801A8914 = 1;
    }
    if (D_80197728 != sector->width || D_8019772C != sector->height) {
        D_80197728 = sector->width;
        D_8019772C = sector->height;
    }
    dec->rect[0].w = dec->rect[1].w = D_80197728 * 3 / 2;
    dec->rect[0].h = dec->rect[1].h = D_8019772C;
    dec->slice.h = D_8019772C;
    return addr;
}
s32 printf();                                   /* printf */
extern char D_80196038[];                              /* "time out in decoding !" */

/* movie_decode_wait: wait for the frame being decoded (the SDK movie samples'
   strSync), called once per frame by the play loop movie_frames_play with the
   decode environment D_801A88E4 and mode 0 (never read).
   - spins while dec->isdone (+0x2C) is 0 — the slice callback
     movie_slice_load sets it after the frame's last slice;
   - every 0x800000 polls without it, prints "time out in decoding !"
     (D_80196038, printf printf) and forces the switch the callback
     would have made: isdone = 1, rectid (+0x20) flipped, slice x/y (+0x24,
     +0x26) = the new rect[rectid]'s (+0x10 + rectid * 8);
   - clears isdone for the next frame.
   Shapes: the counter is `volatile u_long`, as in the SDK sample's strSync:
   the bytes keep it in its stack slot (sp+0x10) and read it back after
   every store (lw / addiu / sw / lw); a plain local lives in a register
   and the slot disappears (24/48). */
/* func_80196A4C */
void movie_decode_wait(DECENV *dec, s32 mode) {
    volatile u_long cnt;

    cnt = 0x800000;
    while (dec->isdone == 0) {
        if (--cnt == 0) {
            printf(D_80196038);
            dec->isdone = 1;
            dec->rectid = dec->rectid ? 0 : 1;
            dec->slice.x = dec->rect[dec->rectid].x;
            dec->slice.y = dec->rect[dec->rectid].y;
        }
    }
    dec->isdone = 0;
}


/* movie_read_start(loc): CdlSeekL to loc until the CD takes it, then
   CdRead2(0x1C0: streaming, double speed, real-time ADPCM) until it does. */
/* func_80196B0C */
void movie_read_start(CdlLOC *loc) {
    do {

    } while (CdControl(0x15, (u_char *)loc, 0) == 0);
    do {

    } while (CdRead2(0x1C0) == 0);
}
extern char D_80196050[];   /* "_B%03d.STR" */
extern u16 D_8019769C[];    /* end frame per movie, 0 = none */
s32 fmt_sprintf();        /* sprintf */

/* func_80196B54 */
void movie_play(u32 n)
{
    char name[16];
    s32 half;
    s32 end;

    half = 0;
    if (n >= 100) {
        n -= 100;
        half = 1;
    }
    fmt_sprintf(name, D_80196050, n);
    end = D_8019769C[n];
    if (half) {
        name[1] = 'C';
        end >>= 1;
    }
    if (end != 0) {
        movie_file_play(name, 0, end - 5);
    }
}
