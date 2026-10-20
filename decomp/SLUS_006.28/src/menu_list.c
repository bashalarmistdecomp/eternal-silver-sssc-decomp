/* Lunar SSSC — menu_list: the lists the use of an item or a spell shows:
   place names, music and movies, each picked, set and shown, and the list's
   run, open and close, with the uses' ends (MenuUseMapRow2Run, MenuUseEnd,
   MenuUseDrawAreaOpen). The first split had it in battle_b.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_MenuPage */

/* Where the first split's battle_b.c began, translation unit B of the battle
   segment (0x80076968 onward), as that file's header said it:

   Split out of the first split's battle.c because the original battle
   segment was several .c files: with -G8 each TU decides per symbol whether
   a global is "small" (reached %gp_rel) or not (reached %hi/%lo), and ~15
   battle globals are reached BOTH ways across the segment.  One TU can only
   pick one mode, so the hi/lo users below were unmatchable while they lived
   in the first split's battle.c.

   Boundary evidence: every %gp_rel user of a flipped global sits at
   <= 0x80075CE0 and every %hi/%lo user at >= 0x80076968, and no converted
   function in this range touches a flipped global at all (verified by
   disassembling the linked image, not just the remaining .s files).

   Declare flipped globals HERE the way this TU reaches them; the first split's
   battle.c keeps the gp-side declarations.  See docs/spec/code/subsystems/battle.md. */

#include "include_asm.h"
/* Sony's libspu.h declares SpuSetCommonCDVolume(short, short). MenuUseMusicRun hands it two
   ints with no sign extension, which a call compiled under that prototype cannot do: the
   original of this file called it with no such prototype in scope. So the header's
   declaration is renamed out of the way for this file, and the function is declared below
   as it is called here. */
#define SpuSetCommonCDVolume SpuSetCommonCDVolume_sdk_prototype
#include "psyq_types.h"
#include "pad.h"
#include "menu.h"
#include "map.h"
#include "sprite.h"
#undef SpuSetCommonCDVolume
extern void SpuSetCommonCDVolume(s32 cd_left, s32 cd_right);

/* --- Battle segment externs (accumulate here) ---
   NOTE: these gp-relative (small, <=8 byte) declarations reflect the FIRST
   battle translation unit (funcs up to ~0x75E34).  A later TU (MenuUsePlaceNameRun
   and on, 0x76968+) accesses several of the same globals via %hi/%lo; those
   functions cannot match against these declarations, which is why the
   segment is split into two TUs.  See docs/spec/code/subsystems/battle.md. */
extern u32 g_MenuBackOutLatch;
/* far_syms.txt scopes this to @menu_list.c: TU-A reaches it %gp_rel, this TU
   reaches it lui $at + %lo.  Same object, different addressing per TU. */
extern s32 g_MenuRequest;
/* 4-byte word: read as `lw` for value tests and as `lhu` (low half) where
   it feeds MenuPageDisp — one s32 declaration covers both via a cast. */
/* far_syms @menu_list.c: MenuUseEnd stores it with a WORD store via
   lui $at, while TU-A treats the same object as a u16 reached %gp_rel. */
extern s32 g_MenuPage;
extern SpriteEntry *g_MenuRenderEntries;   /* the field menu's sprite entries */
extern SpritePiece *g_MenuShapes;         
extern void CellAllUpdate(s32 kind_mask);
extern void sprite_run_off(s32 first, s32 count);
extern void MenuFrameStep(void);
extern void font_target_clear(u8 *target, s32 block_count);
extern void MenuPageDisp(s32 page);
extern void font_target_set(u8 *target);
extern s32 font_col_set(s32 col_bits);
extern void font_string_proportional_put(s32 string_id);
extern void MenuTextPartsLoad(s32 part_bits);
extern void mem_clear(void *destination, u32 byte_count);
extern void mem_move(u8 *dst, u8 *src, s32 n);
extern s32 StoryBitCheck(u32 flag);
extern void SoundPlay(s32 id, s32 ch);
extern s32 MenuMemberCursorFind(s32 chr_id);
extern u8 g_MenuIconBytes[];     /* far byte: mode/portrait id poked before 8006D298 */
extern s32 MusicKindGet(u8 music_number);
extern s32 MusicPlay(u8 id, s32 unused, u8 vol);
extern void cd_track_switch(s32 on);
extern u16 g_MusicNumber;     /* currently-loaded track id (low byte used as u8) */
extern u16 g_MusicKind;     
extern void ScriptLabelRun(s32 label);

/* The use lists' function-pointer slots (gp-relative words holding the
   set / pick / disp functions that MenuUseMusicRun and MenuUseMovieRun
   install for MenuUseListRun). */
extern void *g_MenuUseListPickFunction;
extern void *g_MenuUseListSetFunction;
extern void *g_MenuUseListDispFunction;
void MenuUseMoviePick(u16 *list, s32 cursor);   /* address installed by MenuUseMovieRun */
extern s32 MenuUseMovieListSet(u16 *list, u16 button);
extern void MenuUseMovieListDisp(s32 list);
extern void MenuUseListRun(void);
extern void MenuUseListClose(s32 text, s32 row_block_count, u16 part_bits);
extern void MenuUseMoviePlay(s32 movie);
extern void MenuUseDrawAreaOpen(void);
extern s16 g_MovieSkipAllowed;
extern s32 g_MenuUsedMark;
extern s32 g_MenuUseNumber;
/* 4-byte word: `lw` in MenuUseMoviePlay, low byte (`lbu`) in MenuUseActionSet. */
extern s32 g_MenuUseUserId;
extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  /* Defined later in this file (source order = address order), but installed
   as handler pointers by MenuUseMusicRun which precedes them. */
void MenuUseMusicPick(s32 list, s32 cursor);
s32 MenuUseMusicListSet(u16 *list, u16 button);
void MenuUseMusicListDisp(s32 list);
extern u16 g_MenuMemberCursor;      /* far in this TU (far_syms.txt, @menu_list.c): its one user here, MenuUseDrawAreaOpen, reads it lui + lhu; menu_card.c's, menu_nowalk.c's and menu_option.c's users reach it %gp_rel */
extern s16 g_MapViewX;
extern s16 g_MapViewY;
/* UNPROTOTYPED here on purpose: MenuUseMoviePick passes MenuMemberCursorFind's
   result through unmasked, so this TU did no argument conversion at all.
   The parameter is really 16-bit (`sh $a0, 0x18($sp)` at 0x8006C254, and
   menu_card.c, menu_option.c and menu_places.c prototype it u16) — claiming
   `s32` would become a FUNC-CONFLICT against the definition the moment
   MenuIconBytesDisp converts. An empty parameter list claims nothing and emits the same
   bytes. */
extern void MenuIconBytesDisp();
extern s32 g_SpriteList;
extern u16 g_MenuPadRecord[];     /* pad/input block polled by pad_record_update */
extern u16 g_MenuSoundRequest;      /* far_syms @menu_list.c: sound cue, lui $at store */
extern s32 g_MenuBackOutPattern;      /* far_syms @menu_list.c: confirm stamp 0x30B30B30 */
extern s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
/* GCC 2.8.1 emits its 4-word block-move loop + tail word for a 0x24-byte
   struct assignment; used by MenuBtlPlacesDisp to copy BattleEntity+0x48. */
extern s32 g_MapLayersMask;
extern s16 g_MapCol;
extern s16 g_MapColSecond;
extern s16 g_MapColThird;
s32 MenuUsePlaceName56Check(u16 place_name);
s32 MenuUsePlaceNameBitCheck(s32 place_name);
void MenuUseEnd(void);
/* TU-B reaches this via lui %hi + lw %lo and uses the loaded word as an
   address, so it is a far POINTER-holding symbol here: declare it an array
   and read [0].  menu_card.c, menu_nowalk.c and menu_places.c keep the gp-side
   `void *` declaration. */
extern u8 *g_MenuRecords;
extern void MenuCommandsSet(s32, u16);
extern void MenuCommandsDisp(s32);
extern void MenuIconBytesSet(s32, s32);
extern void MoviePlay(void);
extern void MapReload(void);
extern void MapBgChangeStoryBitsCopy(void);
extern void MapViewStep(s32, s32);
extern void MapBgChangeStart(s16, u8);
extern void MenuTextStringsPut(void);
extern void MenuItemIconBytesDisp(s32, s32, s32);
extern s16 g_MovieFromMenu;
extern s8 D_800B8E0C;       /* far_syms.txt: lui $at + sb */
extern u8 g_MapBgChangeNumber;       /* far_syms.txt: lui $at + sb */
extern u8 g_MapBgChange[];
extern s32 D_800E4768[];
extern void StoryBitSet(s32 bit);
/* Script-side movement projection: steps (*px, *py) `dist` tiles along `dir`
   using the actor's probe rect at `probe`, returns the facing actually used. */
extern u8 *g_MenuKeptIds;
extern void MenuRecordsInit(void);
extern void MapAnimationSet(u8 *record, s32 animation);
extern void MenuUseListOpen(u8 *list, s32 mark_x, s32 cursor_width);
extern void font_image_load(s32 a, s32 b, s32 c);
s32 MenuUsePlaceNameListSet(u16 *list, u16 button);  /* TU defines this below */
extern void pad_record_update(u16 *rec, s32 delay);
void MenuUsePlaceNamePut(s32 place_name, s32 row);  /* TU defines this below */

/* Entry 3 of MenuUseRun's table (the use of an item or spell from the
   menu): a three-row pick list of place names over the u16 list buffer that
   MenuUseListOpen clears and MenuUsePlaceNameListSet fills. When StoryBitCheck(0x2D1) is nonzero it only shows
   message 0x3B6 (the buffer saved and restored around it) and returns.
   Otherwise the loop polls the pad block (pad_record_update): 0x1000 / 0x4000
   in word 2 move the cursor within rows 1..3 or, at an edge, scroll through
   MenuUsePlaceNameListSet, whose nonzero result asks for a redraw of the
   three rows (MenuUsePlaceNamePut per row); in word 1, 0x100 / 0x20
   (or a g_MenuBackOutLatch already set) record 1 / 2 in g_MenuBackOutLatch, stamp
   g_MenuBackOutPattern and leave with cue 3; 0xC0 offers the entry under the cursor
   to MenuUsePlaceName56Check - accepted: g_MenuRequest = entry, cue 2, leave (then
   MenuUseEnd); refused: cue 4. Each frame the cursor object's y
   (g_MenuRenderEntries +0x4E) is g_MapViewY + cursor * 16 + 0x90, its 0x24-byte
   block at +0x48 is copied to g_SpriteList + 0x12FC, and the two markers at
   g_MenuRecords +0x130 / +0x16C are on while list[0] / list[4] are nonzero. */
/* func_80076968 */
void MenuUsePlaceNameRun(void)
{
    u16 list[6];
    s32 cursor;
    s32 pending;
    s32 bits;
    s32 i;
    s32 y;
    u16 *q;

    g_MenuUsedMark = 0;
    ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
    ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
    MenuIconBytesSet(0, 0);
    if (StoryBitCheck(0x2D1) != 0) {
        mem_move((u8 *)list, g_MenuKeptIds, 0xC);
        ((MapRecord *)g_MenuRecords)[2].sprite.on = 0;
        MenuFrameStep();
        MenuIconBytesDisp(0xFFFF, 1, 2, 0);
        ScriptLabelRun(0x3B6);
        MenuRecordsInit();
        MapAnimationSet(g_MenuRecords, 0xB);
        mem_move(g_MenuKeptIds, (u8 *)list, 0xC);
        return;
    }
    MenuUseListOpen((u8 *)list, 0x108, 0x9F);
    font_target_clear((u8 *)0x801CEF00, 0x18);
    ((MapRecord *)g_MenuRecords)[6].sprite.x = ((MapRecord *)g_MenuRecords)[5].sprite.x = g_MapViewX + 0x128;
    font_image_load(0x801CEF00, 0x68, 8);
    font_image_load(0x801CF300, 0x70, 8);
    font_image_load(0x801CF700, 0x78, 8);
    CellTextDisp(0x10, 0x14, 8, 3, 8, 0xDEAD2001, 0xEEA0);
    CellAllUpdate(0x20);
    MenuFrameStep();
    pending = MenuUsePlaceNameListSet(list, 0);
    cursor = 1;
    for (;;) {
        pad_record_update(g_MenuPadRecord, 0x14);
        bits = g_MenuPadRecord[2];
        if (bits & 0x1000) {
            if (cursor >= 2) {
                cursor--;
            } else {
                pending = MenuUsePlaceNameListSet(list, 0x1000);
            }
            g_MenuSoundRequest = 1;
        } else if (bits & 0x4000) {
            if (cursor < 3) {
                cursor++;
            } else {
                pending = MenuUsePlaceNameListSet(list, 0x4000);
            }
            g_MenuSoundRequest = 1;
        } else {
            bits = g_MenuPadRecord[1];
            if ((bits & 0x130) != 0 || g_MenuBackOutLatch != 0) {
                if (bits & 0x100) {
                    g_MenuBackOutLatch = 1;
                } else if (bits & 0x20) {
                    g_MenuBackOutLatch = 2;
                }
                g_MenuBackOutPattern = 0x30B30B30;
                g_MenuSoundRequest = 3;
                break;
            }
            if (bits & 0xC0) {
                q = &list[cursor];
                if (MenuUsePlaceName56Check(*q) != 0) {
                    g_MenuRequest = *q;
                    g_MenuSoundRequest = 2;
                    break;
                }
                g_MenuSoundRequest = 4;
            }
        }
        y = cursor * 0x10 + 0x90;
        g_MenuRenderEntries[2].y = g_MapViewY + y;
        *(SpriteEntry *)(g_SpriteList + 0x12FC) = *(SpriteEntry *)((u8 *)g_MenuRenderEntries + 0x48);
        if (list[0] != 0) {
            ((MapRecord *)g_MenuRecords)[5].sprite.on = 1;
        } else {
            ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
        }
        if (list[4] != 0) {
            ((MapRecord *)g_MenuRecords)[6].sprite.on = 1;
        } else {
            ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
        }
        if (pending != 0) {
            font_target_clear((u8 *)0x801CEF00, 0x1E);
            for (i = 1; i < 4; i++) {
                MenuUsePlaceNamePut(list[i], i + 4);
            }
            font_image_load(0x801CF400, 0x72, 0xA);
            font_image_load(0x801CF900, 0x7C, 0xA);
            font_image_load(0x801CFE00, 0x86, 0xA);
            CellTextDisp(0x10, 0x14, 0xA, 3, 0xA, 0xDEAD2001, 0xEEA0);
            pending = 0;
        }
        if (g_MenuSoundRequest != 0) {
            SoundPlay(g_MenuSoundRequest, 1);
            g_MenuSoundRequest = 0;
        }
        MenuFrameStep();
    }
    MenuUseListClose(0x801CEF00, 0xA, 0x40);
    if (g_MenuSoundRequest == 2) {
        MenuUseEnd();
    }
}

/* func_80076E14 */
void MenuUsePlaceNamePut(s32 place_name, s32 row) {
    s32 kept_place_name;
    s32 old_col_bits;
    s32 a;
    s32 t;
    kept_place_name = place_name & 0xFFFF;
    old_col_bits = font_col_set((MenuUsePlaceName56Check(kept_place_name) == 0) << 2) & 0xFF;
    a = row << 0xA;
    t = ((row - 5) << 8) + 0x801CDB00;
    font_target_set((u8 *)(a + t));
    font_string_proportional_put(kept_place_name + 0x400);
    font_col_set(old_col_bits);
}
/* func_80076E88 */
s32 MenuUsePlaceName56Check(u16 place_name) {
    if (place_name != 0x56) {
        return 1;
    }
    return StoryBitCheck(0x29E) == 0;
}
/* Scroll/rebuild the 5-slot list of place names at list (same family as
   MenuUseMovieListSet / MenuUseMusicListSet, and the same id-scan helper shape).

   button == 0x1000 scrolls the list DOWN: every slot shifts one place later,
   then slot 0 is refilled with the highest enabled id below list[1], falling
   back to a fresh scan from 0x5F if nothing was found.
   button == 0x4000 scrolls UP: slots shift one place earlier and slot 4 is
   refilled with the lowest enabled id above list[3], falling back to a scan
   from 0x51.  With list[3] == 0 there is nothing above, so it just clears.
   Any other button rebuilds from scratch: zero the block, then walk 0x51..0x5F
   filling slots 1..4 with the first four enabled ids; every later hit
   overwrites slot 0, so slot 0 ends up holding the last enabled id.

   Two shape requirements, both about where GCC puts the loops' "found"
   tails.  The `if (list[n] == 0) { second scan }` nesting (rather than an
   early `return 1`) is what makes all four tails land out-of-line together
   before the 0x4000 branch, as the target has them; a do{}while(0) wrapper
   around the 0x4000 body — which is what MenuUseMovieListSet needs — costs 7 here.
   The duplicated `i = 0x51` before `count = 1` is load-bearing: it is the
   only form that emits the loop init ahead of the counter setup. */
/* func_80076EC0 */
s32 MenuUsePlaceNameListSet(u16 *list, u16 button) {
    s32 i;
    s32 count;
    s32 t;
    u16 *p;
    u16 *q;

    if (button == 0x1000) {
        i = 4;
        p = list + 4;
        do {
            p[0] = p[-1];
            i--;
            p--;
        } while (i > 0);
        i = list[1] - 1;
        list[0] = 0;
        for (; i >= 0x51; i--) {
            if (MenuUsePlaceNameBitCheck(i) != 0) {
                list[0] = i;
                break;
            }
        }
        if (list[0] == 0) {
            for (i = 0x5F; i >= 0x51; i--) {
                if (MenuUsePlaceNameBitCheck(i) != 0) {
                    list[0] = i;
                    break;
                }
            }
        }
        return 1;
    }
    if (button == 0x4000) {
        i = 0;
        p = list;
        do {
            p[0] = p[1];
            i++;
            p++;
        } while (i < 4);
        t = list[3];
        list[4] = 0;
        if (t != 0) {
            for (i = t + 1; i < 0x60; i++) {
                if (MenuUsePlaceNameBitCheck(i) != 0) {
                    list[4] = i;
                    break;
                }
            }
            if (list[4] == 0) {
                for (i = 0x51; i < 0x60; i++) {
                    if (MenuUsePlaceNameBitCheck(i) != 0) {
                        list[4] = i;
                        break;
                    }
                }
            }
        }
        return 1;
    }
    mem_clear((u8 *)list, 5);
    i = 0x51;
    count = 1;
    q = list + 1;
    for (i = 0x51; i < 0x60; i++) {
        if (MenuUsePlaceNameBitCheck(i) != 0) {
            if (count < 5) {
                *q = i;
                q++;
                count++;
            } else {
                list[0] = i;
            }
        }
    }
    return 1;
}
/* func_800770B0 */
s32 MenuUsePlaceNameBitCheck(s32 place_name) {
    if (place_name < 0x65) {
        return StoryBitCheck((place_name + 0x28A) & 0xFFFF);
    }
    return 1;
}
/* Entry 5 of MenuUseRun's table — end the menu with MenuUseEnd, then leave
   request 5 in g_MenuRequest (MenuRun answers it with MapRow2Change) and set
   the used mark g_MenuUsedMark.  It needs this TU because g_MenuRequest is
   reached %gp_rel from menu_card.c and menu_nowalk.c. */
/* func_800770E4 */
void MenuUseMapRow2Run(void) {
    MenuUseEnd();
    g_MenuRequest = 5;
    g_MenuUsedMark = 1;
}
/* End the menu's pages after a use that leaves a request: clear the page
   g_MenuPage (MenuPagesRun stops), blank the on halfwords of six of the
   menu's records, then reissue empty commands and icon bytes and
   MenuPageDisp(0xFFFF). */
/* func_80077114 */
void MenuUseEnd(void) {
    u8 *w;

    w = g_MenuRecords;
    g_MenuPage = 0;
    ((MapRecord *)w)->sprite.on = 0;
    ((MapRecord *)w)[1].sprite.on = 0;
    ((MapRecord *)w)[2].sprite.on = 0;
    ((MapRecord *)w)[3].sprite.on = 0;
    ((MapRecord *)w)[5].sprite.on = 0;
    ((MapRecord *)w)[6].sprite.on = 0;
    MenuCommandsSet(0, 0);
    MenuCommandsDisp(4);
    MenuIconBytesSet(0, 0);
    MenuIconBytesDisp(0xFFFF, 1, 2, 0);
    MenuPageDisp(0xFFFF);
}
/* Entry 6 of MenuUseRun's table, the music list.  The one-shot branch
   (script flag 0x387 set and 0x47F not yet) stops the music and plays the
   disc's audio track (cd_track_switch(2)): ~0x2F8 idle frames, then its CD
   volume (SpuSetCommonCDVolume) stepping 0x7BBB down by 0x444 per frame, and
   the track off.  Otherwise it installs the list's three functions, runs
   MenuUseListRun, and — if a pick changed the current piece of music —
   stops it when its kind differs and plays the piece kept from before
   again, before MenuUseListClose puts the menu back. */
/* func_80077190 */
void MenuUseMusicRun(void) {
    s32 left_volume;
    s32 frames_done;
    s32 volume;
    u16 kind;
    s32 kept_music_number;

    if ((StoryBitCheck(0x387) != 0) && (StoryBitCheck(0x47F) == 0)) {
        StoryBitSet(0x47F);
        MusicPlay(0, 0, 0);
        cd_track_switch(2);
        frames_done = 0;
        do {
            frames_done += 1;
            MenuFrameStep();
        } while (frames_done < 0x2F8);
        volume = 0x7BBB;
        left_volume = 0x7BBB;
        do {
            SpuSetCommonCDVolume(left_volume, volume);
            volume -= 0x444;
            MenuFrameStep();
            left_volume = volume;
        } while (volume > 0);
        SpuSetCommonCDVolume(0, 0);
        cd_track_switch(0);
        return;
    }
    g_MenuUseListSetFunction = MenuUseMusicListSet;
    g_MenuUseListPickFunction = MenuUseMusicPick;
    kept_music_number = g_MusicNumber;
    g_MenuUseListDispFunction = MenuUseMusicListDisp;
    MenuUseListRun();
    if (g_MusicNumber != kept_music_number) {
        kind = MusicKindGet((u8) g_MusicNumber) & 0xFF;
        if (g_MusicKind != kind) {
            MusicPlay(0, 2, 0);
            cd_track_switch(0);
        }
        g_MusicNumber = kept_music_number;
        g_MusicKind = kind;
        MusicPlay((s32) (u8) g_MusicNumber, 0, 0);
    }
    MenuUseListClose(0x801CE700, 0xB, 0x50);
}
/* Selects entry cursor of the u16 id table at list and makes it current.
   Skips everything if that id is already loaded; otherwise maps its low byte
   through MusicKindGet to a bank id, tears the old bank down when it
   differs, then records both ids and activates the new one. */
/* func_800772E0 */
void MenuUseMusicPick(s32 list, s32 cursor) {
    u16 *p;
    u16 t;

    SoundPlay(2, 1);
    p = (u16 *)(cursor * 2 + list);
    if (g_MusicNumber != *p) {
        t = MusicKindGet((u8)*p) & 0xFF;
        if (g_MusicKind != t) {
            MusicPlay(0, 2, 0);
            cd_track_switch(0);
        }
        g_MusicNumber = *p;
        g_MusicKind = t;
        MusicPlay((u8)g_MusicNumber, 0, 0);
    }
}
/* Identical routine to MenuUseMovieListSet (5-slot shortlist maintenance) over a
   different id space: entries 0x19A+n, scanned up to 0x45.  Same do/while(0)
   block-layout requirement — see the comment on MenuUseMovieListSet. */
/* func_80077380 */
s32 MenuUseMusicListSet(u16 *list, u16 button) {
    s32 i;
    s32 count;
    s32 t;
    u16 *p;

    if (button == 0x1000) {
        i = 4;
        p = list + 4;
        do {
            p[0] = p[-1];
            i--;
            p--;
        } while (i > 0);
        t = list[1];
        list[0] = 0;
        for (i = t - 1; i > 0; i--) {
            if (StoryBitCheck((i + 0x19A) & 0xFFFF) != 0) {
                list[0] = i;
                return 1;
            }
        }
        return 1;
    }
    if (button == 0x4000) {
        i = 0;
        p = list;
        do {
            p[0] = p[1];
            i++;
            p++;
        } while (i < 4);
        t = list[3];
        /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
           the block runs once, so behaviour is the same without the `do` /
           `while (0)`, but then 8 of this function's 91 instruction lines differ. */
        do {
            list[4] = 0;
            if (t != 0) {
                for (i = t + 1; i < 0x45; i++) {
                    if (StoryBitCheck((i + 0x19A) & 0xFFFF) != 0) {
                        list[4] = i;
                        return 1;
                    }
                }
            }
            return 1;
        } while (0);
    }
    mem_clear((u8 *)list, 5);
    i = 1;
    count = 1;
    list++;
    for (; i < 0x45; i++) {
        if (StoryBitCheck((i + 0x19A) & 0xFFFF) != 0) {
            *list = i;
            list++;
            count++;
            if (count >= 5) {
                return 1;
            }
        }
    }
    return 1;
}
/* Writes the music list's 3 rows into the 0x801CE700 text buffer: row i at
   +i*0x580, its string id (+ 0x546) read from the halfword list at list+2.
   The offset/pointer walkers in the asm are GCC strength-reduction IVs;
   writing them as source locals puts their init before the
   hoisted 0x801CE700 constant and misses by 2 instructions. */
/* func_800774EC */
void MenuUseMusicListDisp(s32 list) {
    s32 counter;
    font_target_clear((u8 *)0x801CE700, 0x21);
    for (counter = 1; counter < 4; counter++) {
        font_target_set((u8 *)((counter - 1) * 0x580 + 0x801CE700));
        font_string_proportional_put(*(u16 *)(list + counter * 2) + 0x546);
    }
    MenuTextPartsLoad(0x50);
}
/* func_8007757C */
void MenuUseMovieRun(void) {
    g_MenuUseListSetFunction = (void *)MenuUseMovieListSet;
    g_MenuUseListPickFunction = (void *)MenuUseMoviePick;
    g_MenuUseListDispFunction = (void *)MenuUseMovieListDisp;
    MenuUseListRun();
    MenuUseListClose(0x801CE700, 0xB, 0x50);
}
/* Play the movie under the list's cursor (cursor): movie list[cursor] - 1
   (MenuUseMoviePlay), then rebuild the icons for the user g_MenuUseUserId, lay
   out the text at 0x10, 0x14, redraw the movie list, re-park the list's two
   marks and open the screen (MenuUseDrawAreaOpen). */
/* func_800775D4 */
void MenuUseMoviePick(u16 *list, s32 cursor) {
    u8 *w;
    s16 x;
    s16 y;
    s16 z;

    SoundPlay(2, 1);
    g_MovieSkipAllowed = 0x8D0;
    MenuUseMoviePlay(list[cursor] - 1);
    MenuIconBytesSet(0, 0);
    g_MenuIconBytes[0] = 0xB5;
    MenuIconBytesDisp(MenuMemberCursorFind(g_MenuUseUserId), 1, 2, 0);
    CellTextDisp(0x10, 0x14, 0xB, 3, 0xB, 0xDEAD2001, 0xEE60);
    CellAllUpdate(0x20);
    MenuUseMovieListDisp((s32)list);
    w = g_MenuRecords;
    x = g_MapViewX + 0x130;
    y = g_MapViewY + 0xA0;
    z = g_MapViewY + 0xC8;
    ((MapRecord *)w)[5].sprite.x = x;
    ((MapRecord *)w)[5].sprite.y = y;
    ((MapRecord *)w)[6].sprite.x = x;
    ((MapRecord *)w)[6].sprite.y = z;
    MenuUseDrawAreaOpen();
}
/* 5-slot movie list maintenance.  arg1 selects the operation:
   0x1000 scrolls the list down (shift right, then search *backwards* from
   slot 1 for the next available id), 0x4000 scrolls up (shift left, then
   search forwards from slot 3), anything else rebuilds all four entries
   from scratch.  StoryBitCheck(id) tests whether entry 0x153+n is usable. */
/* func_800776CC */
s32 MenuUseMovieListSet(u16 *list, u16 button) {
    s32 i;
    s32 count;
    s32 t;
    u16 *p;

    if (button == 0x1000) {
        i = 4;
        p = list + 4;
        do {
            p[0] = p[-1];
            i--;
            p--;
        } while (i > 0);
        t = list[1];
        list[0] = 0;
        for (i = t - 1; i > 0; i--) {
            if (StoryBitCheck((i + 0x153) & 0xFFFF) != 0) {
                list[0] = i;
                return 1;
            }
        }
        return 1;
    }
    if (button == 0x4000) {
        i = 0;
        p = list;
        do {
            p[0] = p[1];
            i++;
            p++;
        } while (i < 4);
        t = list[3];
        /* The do/while(0) is load-bearing: it is the only shape that makes
           GCC emit both loops' "found" tails out-of-line *before* this
           branch's code (target .L80077748/.L80077750).  Without it the
           function is byte-identical except those two 2-instruction blocks
           land after the loop instead (81/91).  The equivalent if/else
           form does NOT match.  Do not "simplify". */
        /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
           the block runs once, so behaviour is the same without the `do` /
           `while (0)`, but then 8 of this function's 91 instruction lines differ. */
        do {
            list[4] = 0;
            if (t != 0) {
                for (i = t + 1; i < 0x38; i++) {
                    if (StoryBitCheck((i + 0x153) & 0xFFFF) != 0) {
                        list[4] = i;
                        return 1;
                    }
                }
            }
            return 1;
        } while (0);
    }
    mem_clear((u8 *)list, 5);
    i = 1;
    count = 1;
    list++;
    for (; i < 0x38; i++) {
        if (StoryBitCheck((i + 0x153) & 0xFFFF) != 0) {
            *list = i;
            list++;
            count++;
            if (count >= 5) {
                return 1;
            }
        }
    }
    return 1;
}
/* Same 3-row write as MenuUseMusicListDisp, for the movie list, strings from the 0x500 id base. */
/* func_80077838 */
void MenuUseMovieListDisp(s32 list) {
    s32 counter;
    font_target_clear((u8 *)0x801CE700, 0x21);
    for (counter = 1; counter < 4; counter++) {
        font_target_set((u8 *)((counter - 1) * 0x580 + 0x801CE700));
        font_string_proportional_put(*(u16 *)(list + counter * 2) + 0x500);
    }
    MenuTextPartsLoad(0x50);
}
/* Plays a movie or shows a picture (MoviePlay) with the flag g_MovieFromMenu
   raised, reloads the map (MapReload, MapBgChangeStoryBitsCopy,
   MapViewStep), clears the two D_800B8E0C/0D latches (re-issuing the old
   g_MapBgChangeNumber value to MapBgChangeStart), then puts the menu's item page
   back for the user g_MenuUseUserId.

   `prev` must be `s32`, not `u8` — a narrow local re-masks with `andi` at
   the MapBgChangeStart call site.  MenuCommandsSet takes two arguments here;
   the `lui $a2, %hi(D_800E4768)` before it is just GCC using a
   call-clobbered scratch register, not a third argument (m2c shows one).
   movie is accepted but never named in the body — MenuUsePictureRun passes a
   computed value ((hi >> 4) + 0x37), MenuUseMoviePick a list entry - 1, and
   the target never touches $a0 before the MoviePlay call, so MoviePlay
   receives it as its number.  Declared with the parameter so that the
   callers stay correct. */
/* func_800778C8 */
void MenuUseMoviePlay(s32 movie) {
    s32 on_list;
    s32 prev;

    g_MovieFromMenu = 1;
    MoviePlay();
    g_MovieFromMenu = 0;
    MapReload();
    MapBgChangeStoryBitsCopy();
    MapViewStep(0, 1);
    prev = g_MapBgChangeNumber;
    D_800B8E0C = 0;
    g_MapBgChangeNumber = 0;
    g_MapBgChange[1] = 0;
    g_MapBgChange[0] = 0;
    MapBgChangeStart(prev, 0);
    on_list = (g_MenuUseUserId == 0xC);
    MenuTextStringsPut();
    D_800E4768[1] = 0x801CBB00;
    D_800E4768[3] = 0x6000;
    D_800E4768[0] = 1;
    MenuCommandsSet(g_MenuUseUserId, 4);
    MenuCommandsDisp(0);
    MenuItemIconBytesDisp(g_MenuUseUserId, on_list, 0);
}
/* far_syms: g_MapCol_blk */
extern MapCol g_MapCol_blk;     /* the three scalars above as one 6-byte block (far alias) */
extern void MenuCursorTextDisp(s32 icon, char col_bits);
extern void MenuLineUpDisp(u16, s32, s32, s32);
extern void MapDrawAreaOpen(s32 layer_mask, u32 step_count, s32 unused, s32 view_follow, s32 second_view_follow);
extern s32 layer_col_set(s32, s32, s32);

/* Present the picked entry (callers MenuUsePictureRun and MenuUseMoviePick):
   draw entry g_MenuUseNumber's two text strings, switch anim actors 0, 2 and 3
   on (the s16 at +4 of the 0x3C-byte records at g_MenuRecords: +0x04, +0x7C,
   +0xB8), run the page's frame call, then open the screen with the 25-step
   window transition - with the default colour triple g_MapCol / CF2 / CF4
   forced to -100 while it runs. The triple is saved and restored as one
   6-byte block through the far alias g_MapCol_blk while the three scalars
   are stored %gp_rel: both addressings in one function (battle.md). The
   copy is a local, not an argument: MapDrawAreaOpen takes five.
   The actor flags must be in-struct stores so the far load of g_MenuMemberCursor
   can be lifted over them (`*(s16 *)(p + off)` casts: 47/51).
   Both store runs are chains written low member first (the target stores
   high to low). */
/* func_800779A0 */
void MenuUseDrawAreaOpen(void)
{
    MapCol saved;

    MenuCursorTextDisp(g_MenuUseNumber, 0);
    AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 1;
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0);
    saved = g_MapCol_blk;
    g_MapCol = g_MapColSecond = g_MapColThird = -100;
    MapDrawAreaOpen(g_MapLayersMask | 0x130, 0x19, 0, 0, 0);
    layer_col_set(-1, 0x100, 0);
    g_MapCol_blk = saved;
}

/* The list loop of the menu's uses (MenuUseMusicRun, MenuUseMovieRun).
   pad_record_update polls the pad block
   g_MenuPadRecord: bit 0x1000 moves the cursor up (or, at the top, asks handler
   g_MenuUseListSetFunction to scroll with 0x1000), bit 0x4000 moves down the same way.
   Otherwise pad word +2 decides: bits 0x130 (or an already-set g_MenuBackOutLatch)
   latch the back-out — g_MenuBackOutLatch = 1 for 0x100, 2 for 0x20 — stamp
   g_MenuBackOutPattern = 0x30B30B30, set g_MenuSoundRequest = 3 and leave; bits 0xC0 call
   handler g_MenuUseListPickFunction.  Every pass republishes the row Y into
   BattleEntity+0x4E, copies the 0x24-byte block at +0x48 over
   *g_SpriteList + 0x12FC, and mirrors "entries above / below" into the window
   block's 0x130 / 0x16C.

   Three placement requirements, all load-bearing:
     - g_MenuRecords is re-read INSIDE each arm of the two if/elses; binding it
       to a local hoists the load above the test and costs 18 instructions.
     - `pending = 0` goes AFTER the handler call so GCC can sink it into the
       jalr delay slot (+4).
     - the row-Y offset must be hoisted into its own statement (rowY) — the
       last instruction of 203. */
/* func_80077A6C */
void MenuUseListRun(void)
{
  u16 list[5];
  s32 cursor;
  s32 pending;
  u8 one;
  u8 *pad;
  s32 bits;
  s32 t;
  SpriteEntry *dst;
  SpriteEntry *src;
  s32 rowY;
  MenuIconBytesSet(0, 0);
  MenuUseListOpen((u8 *) list, 0x130, 0xAF);
  font_target_clear((u8 *) 0x801CE700, 0x21);
  MenuTextPartsLoad(0x50);
  CellTextDisp(0x10, 0x14, 0xB, 3, 0xB, 0xDEAD2001, 0xEE60);
  CellAllUpdate(0x20);
  cursor = 1;
  MenuFrameStep();
  one = cursor;
  pending = ((s32 (*)(u16 *, s32)) g_MenuUseListSetFunction)(list, 0);
  pad = g_MenuPadRecord;
  for (;;)
  {
    pad_record_update(pad, 0x14);
    bits = ((PadRecord *)pad)->repeated;
    if (bits & 0x1000)
    {
      if (cursor >= 2)
      {
        cursor--;
      }
      else
        if (list[0] != 0)
      {
        pending = ((s32 (*)(u16 *, s32)) g_MenuUseListSetFunction)(list, 0x1000);
      }
      g_MenuSoundRequest = one;
    }
    else
      if (bits & 0x4000)
    {
      t = cursor + 1;
      if (cursor < 3)
      {
        if (list[t] != 0)
        {
          cursor = t;
        }
      }
      else
        if (list[4] != 0)
      {
        pending = ((s32 (*)(u16 *, s32)) g_MenuUseListSetFunction)(list, 0x4000);
      }
      g_MenuSoundRequest = one;
    }
    else
    {
      bits = ((PadRecord *)pad)->pressed;
      if (((bits & 0x130) != 0) || (g_MenuBackOutLatch != 0))
      {
        if (bits & 0x100)
        {
          g_MenuBackOutLatch = 1;
        }
        else
          if (bits & 0x20)
        {
          g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        g_MenuSoundRequest = 3;
        break;
      }
      if (bits & 0xC0)
      {
        ((void (*)(u16 *, s32)) g_MenuUseListPickFunction)(list, cursor);
      }
    }
    dst = (SpriteEntry *) (g_SpriteList + 0x12FC);
    src = (SpriteEntry *) (((u8 *) g_MenuRenderEntries) + 0x48);
    rowY = (cursor * 0x10) + 0x90;
    g_MenuRenderEntries[2].y = g_MapViewY + rowY;
    *dst = *src;
    if (list[0] != 0)
    {
      ((MapRecord *)g_MenuRecords)[5].sprite.on = one;
    }
    else
    {
      ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
    }
    if (list[4] != 0)
    {
      ((MapRecord *)g_MenuRecords)[6].sprite.on = one;
    }
    else
    {
      ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
    }
    if (pending != 0)
    {
      ((void (*)(u16 *)) g_MenuUseListDispFunction)(list);
      pending = 0;
    }
    if (g_MenuSoundRequest != 0)
    {
      SoundPlay(g_MenuSoundRequest, 1);
      g_MenuSoundRequest = 0;
    }
    MenuFrameStep();
  }

  SoundPlay(g_MenuSoundRequest, 1);
  g_MenuSoundRequest = 0;
}

/* Lay out the scrolling pick list of MenuUsePlaceNameRun / MenuUseListRun: clear
   the busy word, switch the two scroll markers (anim actors 5 and 6) off
   and park them at x = g_MapViewX + mark_x, y = g_MapViewY + 0xA0 / + 0xC8,
   size the cursor highlight (slice 0) to cursor_width x 0xF, put the cursor's
   render entry (entry 2) at x = g_MapViewX + 0x80, poke g_MenuUseNumber into the
   first icon-grid cell, redraw the grid for g_MenuUseUserId's slot and zero the
   caller's 12-byte list (list).
   The records must be written as struct members: an in-struct store cannot
   alias a scalar global, which is what lets the target load g_MenuUseUserId
   first and g_MenuRecords once (raw `*(s16 *)(p + off)` casts: 30/46). */
/* func_80077D98 */
void MenuUseListOpen(u8 *list, s32 mark_x, s32 cursor_width)
{
    g_MenuUsedMark = 0;
    AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
    AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
    ((MapRecord *)g_MenuRecords)[5].sprite.x = ((MapRecord *)g_MenuRecords)[6].sprite.x = g_MapViewX + mark_x;
    ((MapRecord *)g_MenuRecords)[5].sprite.y = g_MapViewY + 0xA0;
    ((MapRecord *)g_MenuRecords)[6].sprite.y = g_MapViewY + 0xC8;
    g_MenuShapes[0].x1 = g_MenuShapes[0].x2 = cursor_width;
    g_MenuShapes[0].y2 = g_MenuShapes[0].y3 = 0xF;
    g_MenuRenderEntries[2].x = g_MapViewX + 0x80;
    g_MenuIconBytes[0] = g_MenuUseNumber;
    MenuIconBytesDisp(MenuMemberCursorFind(g_MenuUseUserId), 1, 2, 0);
    mem_clear(list, 0xC);
}

/* Put the menu back after a use's list: park the list's two marks at
   (g_MapViewX+0x120, g_MapViewY+0x98 / +0xC8), clear the three flag
   halfwords, switch sprite-list entries 0x87.. off, then clear row_block_count * 3
   blocks of the text at text and load the runs part_bits again.
   The three coordinate addends must be computed into `s16` temps up front —
   the target loads both globals and forms all three values before any store,
   and computing them at point of use interleaves them (27/38). */
/* func_80077E50 */
void MenuUseListClose(s32 text, s32 row_block_count, u16 part_bits) {
    u8 *w;
    s16 x;
    s16 y;
    s16 z;

    w = g_MenuRecords;
    x = g_MapViewX + 0x120;
    y = g_MapViewY + 0x98;
    z = g_MapViewY + 0xC8;
    ((MapRecord *)w)[5].sprite.x = x;
    ((MapRecord *)w)[5].sprite.y = y;
    ((MapRecord *)w)[6].sprite.x = x;
    ((MapRecord *)w)[6].sprite.y = z;
    ((MapRecord *)w)[2].sprite.on = 0;
    ((MapRecord *)w)[5].sprite.on = 0;
    ((MapRecord *)w)[6].sprite.on = 0;
    sprite_run_off(0x87, 7);
    font_target_clear((u8 *)text, (u16)(row_block_count * 3));
    MenuTextPartsLoad((u16)part_bits);
}
