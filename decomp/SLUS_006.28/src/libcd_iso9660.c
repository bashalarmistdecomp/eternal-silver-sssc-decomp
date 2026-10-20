/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libcd/iso9660.c) and psyz (decomp/src/libcd/iso9660.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member iso9660.o: CdSearchFile, _cmp,
   CD_newmedia, func_80092AC4, CD_cachefile, cd_read. The first
   split had it in psyq_cd_aa.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: CdSearchFile */
/* gnu_as_loads: CdSearchFile */
/* return_slot_fill: CdSearchFile */
/* reorder_fill: CdSearchFile */
/* far_syms: D_800B636C */
/* far_syms: D_800B6724 */
extern s32 D_800B6360;              /* CD_debug */
extern s32 D_800B636C;              /* CD_nopen: lid-open count */
extern s32 D_800B6724;              /* the CD_nopen the directory cache was built at */
extern CdlFILE D_800E7160[64];      /* the cached directory's files */
extern char D_80013534[];           /* "%s: path level (%d) error\n" */
extern char D_80013550[];           /* "%s: dir was not found\n" */
extern char D_80013568[];           /* "CdSearchFile: disc error\n" */
extern char D_80013584[];           /* "CdSearchFile: searching %s...\n" */
extern char D_800135A4[];           /* "%s:  found\n" */
extern char D_800135B0[];           /* "%s: not found\n" */
s32 CD_newmedia(void);  /* TU defines this below */
s32 func_80092AC4(s32 arg0, char *arg1);  /* TU defines this below */
s32 CD_cachefile(s32 arg0);  /* TU defines this below */
s32 _cmp(u8 *a, u8 *b);  /* TU defines this below */
s32 printf();

/* libcd CdSearchFile (ISO9660.OBJ): resolve an absolute path ("\DIR\FILE.EXT;1") to its
   directory entry. Rebuilds the directory cache (CD_newmedia = CD_newmedia) when the lid-open
   count D_800B636C moved since it was built (D_800B6724); walks the '\'-separated
   components from directory 1 with func_80092AC4 (CD_searchdir), at most 8 levels; loads
   that directory's files (CD_cachefile = CD_cachefile) and compares the last component
   with each D_800E7160[i].name (_cmp = _cmp). A hit copies the CdlFILE to *fp and
   returns the cache entry; every failure returns 0 and prints at CD_debug > 0. libcd.h
   types name as char *, but the library was built with signed char, so the C reads it
   through s8. */
/* func_80092508 */
CdlFILE *CdSearchFile(CdlFILE *fp, char *name)
{
    s8 buf[32];
    int dir;
    int i;
    s8 *namePtr;
    s8 *bufPtr;

    if (D_800B6724 != D_800B636C) {
        if (CD_newmedia() == 0) {
            return 0;
        }
        D_800B6724 = D_800B636C;
    }
    if (((s8 *)name)[0] != '\\') {
        return 0;
    }
    buf[0] = 0;
    dir = 1;
    namePtr = (s8 *)name;
    for (i = 0; i < 8; i++) {
        bufPtr = buf;
        while (*namePtr != '\\') {
            if (*namePtr == 0) {
                goto out;
            }
            *bufPtr++ = *namePtr++;
        }
        if (*namePtr == 0) {
            goto out;
        }
        namePtr++;
        *bufPtr = 0;
        dir = func_80092AC4(dir, buf);
        if (dir == -1) {
            buf[0] = 0;
            break;
        }
    }
out:
    if (i >= 8) {
        if (D_800B6360 > 0) {
            printf(D_80013534, name, i);
        }
        return 0;
    }
    if (buf[0] == 0) {
        if (D_800B6360 > 0) {
            printf(D_80013550, name);
        }
        return 0;
    }
    *bufPtr = 0;
    if (CD_cachefile(dir) == 0) {
        if (D_800B6360 > 0) {
            printf(D_80013568);
        }
        return 0;
    }
    if (D_800B6360 > 1) {
        printf(D_80013584, buf);
    }
    for (i = 0; i < 64; i++) {
        if ((s8)D_800E7160[i].name[0] == 0) {
            break;
        }
        if (_cmp(D_800E7160[i].name, buf)) {
            if (D_800B6360 > 1) {
                printf(D_800135A4, buf);
            }
            *fp = D_800E7160[i];
            return &D_800E7160[i];
        }
    }
    if (D_800B6360 > 0) {
        printf(D_800135B0, buf);
    }
    return 0;
}

/* func_800927E0 */
s32 _cmp(u8 *a, u8 *b) {
    return strncmp(a, b, 0xC) == 0;
}

/* libcd ISO9660.OBJ's directory cache: one record per path-table entry (CdlMAXDIR = 128) */
/* an ISO9660 little-endian 32-bit field at any alignment: copied as four bytes (lwl/lwr) */
/* cc97_lib: CD_newmedia */
/* gnu_as_loads: CD_newmedia */
/* return_slot_fill: CD_newmedia */
/* reorder_fill: CD_newmedia */
/* far_syms: D_800B6360 */
/* far_syms: D_800B6720 */
extern CdlDIR D_800E7760[128];
extern u8 D_800E8D60[0x800];        /* load_buf: one sector */
extern s32 D_800B6360;              /* CD_debug */
extern s32 D_800B6720;              /* the directory number D_800E7160 holds */
extern char D_800135C0[];           /* "CD_newmedia: Read error in cd_read(PVD)\n" */
extern char D_800135EC[];           /* "CD001" */
extern char D_800135F4[];           /* "CD_newmedia: Disc format error in cd_read(PVD)\n" */
extern char D_80013624[];           /* "CD_newmedia: Read error (PT:%08x)\n" */
extern char D_80013648[];           /* "CD_newmedia: sarching dir..\n" */
extern char D_80013668[];           /* "\t%08x,%04x,%04x,%s\n" */
extern char D_8001367C[];           /* "CD_newmedia: %d dir entries found\n" */
s32 cd_read(s32 sectors, s32 lba, u8 *buf);  /* TU defines this below */
s32 printf();
s32 strncmp();
s32 memcpy();

/* libcd CD_newmedia (ISO9660.OBJ; the tree's name is the tag string it checks). Reads the
   primary volume descriptor (sector 16), checks its "CD001" tag, reads the first sector of
   the little-endian path table (LBA at PVD +0x8C) and builds the directory cache
   D_800E7760 from its records {name length, -, LBA, parent, name}; a short table ends with
   unk4 = 0. Returns 1, or 0 on a read or format error. */
/* func_80092800 */
s32 CD_newmedia(void)
{
    CdlLBA lba;
    u8 *p;
    s32 i;

    if (cd_read(1, 0x10, D_800E8D60) != 1) {
        if (D_800B6360 > 0) {
            printf(D_800135C0);
        }
        return 0;
    }
    if (strncmp(&D_800E8D60[1], D_800135EC, 5) != 0) {
        if (D_800B6360 > 0) {
            printf(D_800135F4);
        }
        return 0;
    }

    (&lba)->i = ((CdlLBA *)&D_800E8D60[0x8C])->i;
    if (cd_read(1, lba.addr, D_800E8D60) != 1) {
        if (D_800B6360 > 0) {
            printf(D_80013624, lba.addr);
        }
        return 0;
    }

    p = D_800E8D60;
    if (D_800B6360 > 1) {
        printf(D_80013648);
    }
    i = 0;
    while (p < D_800E8D60 + 0x800) {
        if (p[0] == 0) {
            break;
        }
        ((CdlLBA *)&D_800E7760[i].lba)->i = ((CdlLBA *)&p[2])->i;
        D_800E7760[i].parent = p[6];
        D_800E7760[i].number = i + 1;
        memcpy(D_800E7760[i].name, &p[8], p[0]);
        D_800E7760[i].name[p[0]] = '\0';
        p += 8 + p[0] + p[0] % 2;
        if (D_800B6360 > 1) {
            printf(D_80013668, D_800E7760[i].lba, D_800E7760[i].number,
                          D_800E7760[i].parent, D_800E7760[i].name);
        }
        if (++i >= 128) {
            break;
        }
    }
    if (i < 128) {
        D_800E7760[i].parent = 0;
    }

    D_800B6720 = 0;
    if (D_800B6360 > 1) {
        printf(D_8001367C, i);
    }
    return 1;
}

/* cc97_lib: func_80092AC4 */
/* gnu_as_loads: func_80092AC4 */
/* return_slot_fill: func_80092AC4 */
extern CdlDIR D_800E7760[128];
s32 strcmp(char *, char *);

s32 func_80092AC4(s32 arg0, char *arg1)
{
    s32 i;

    for (i = 0; i < 128; i++) {
        if (D_800E7760[i].parent == 0) {
            return -1;
        }
        if (D_800E7760[i].parent != arg0) {
            continue;
        }
        if (strcmp(arg1, D_800E7760[i].name) == 0) {
            return i + 1;
        }
    }
    return -1;
}
/* cc97_lib: CD_cachefile */
/* gnu_as_loads: CD_cachefile */
/* return_slot_fill: CD_cachefile */
/* reorder_fill: CD_cachefile */
/* far_syms: D_800B6360 D_800B6720 */
extern CdlDIR D_800E7760[128];
extern CdlFILE D_800E7160[64];      /* file[CdlMAXFILE]: the cached directory's files */
extern u8 D_800E8D60[0x800];        /* load_buf: one sector */
extern char D_800136A0[];           /* "CD_cachefile: dir not found\n" */
extern char D_800136C0[];           /* "CD_cachefile: searching...\n" */
extern s32 D_800136DC[];            /* "." (a string literal: word-aligned) */
extern s32 D_800136E0[];            /* ".." (a string literal: word-aligned) */
extern char D_800136E4[];           /* "\t(%02x:%02x:%02x) %8d %s\n" */
extern char D_80013700[];           /* "CD_cachefile: %d files found\n" */
CdlLOC *CdIntToPos(int i, CdlLOC *p);

/* libcd CD_cachefile(dir) (ISO9660.OBJ, static; named by its own debug strings): cache the
   files of directory number `dir` in D_800E7160[64] unless D_800B6720 says they already are.
   Reads the directory's first sector (extent from the path-table cache D_800E7760[dir - 1])
   and walks its ISO9660 records: length at +0 (0 ends), extent LBA at +2, size at +10, name
   length at +0x20, name at +0x21; records 0 and 1 become "." and "..". Stops at 64 files or
   the end of that one sector. Returns 1, or -1 when the sector cannot be read. The two short
   copies are the original's strcpy from a literal (GCC's block move of strlen + 1 bytes). */
/* func_80092B68 */
s32 CD_cachefile(s32 arg0)
{
    s32 lba;
    u8 *ptr;
    s32 i;

    if (arg0 == D_800B6720) {
        return 1;
    }
    if (cd_read(1, (D_800E7760 - 1)[arg0].lba, D_800E8D60) != 1) {
        if (D_800B6360 > 0) {
            printf(D_800136A0);
        }
        return -1;
    }
    if (D_800B6360 > 1) {
        printf(D_800136C0);
    }
    i = 0;
    ptr = D_800E8D60;
    while (ptr < D_800E8D60 + 0x800) {
        if (ptr[0] == 0) {
            break;
        }
        memcpy(&lba, &ptr[2], 4);
        CdIntToPos(lba, &D_800E7160[i].pos);
        memcpy(&D_800E7160[i].size, &ptr[10], 4);
        switch (i) {
        case 0:
            memcpy(D_800E7160[0].name, D_800136DC, 2);      /* strcpy(file[0].name, ".") */
            break;
        case 1:
            memcpy(D_800E7160[1].name, D_800136E0, 3);      /* strcpy(file[1].name, "..") */
            break;
        default:
            memcpy(D_800E7160[i].name, &ptr[0x21], ptr[0x20]);
            D_800E7160[i].name[ptr[0x20]] = '\0';
            break;
        }
        if (D_800B6360 > 1) {
            printf(D_800136E4, D_800E7160[i].pos.minute, D_800E7160[i].pos.second,
                          D_800E7160[i].pos.sector, D_800E7160[i].size, D_800E7160[i].name);
        }
        ptr += ptr[0];
        if (++i >= 64) {
            break;
        }
    }
    D_800B6720 = arg0;
    if (i < 64) {
        D_800E7160[i].name[0] = '\0';
    }
    if (D_800B6360 > 1) {
        printf(D_80013700, i);
    }
    return 1;
}

/* cc97_lib: cd_read */
/* return_slot_fill: cd_read */

s32 CdReadSync(s32 mode, u8 *result);

/* libcd cd_read(sectors, lba, buf) (ISO9660.OBJ, static; the name is sotn-decomp's): read
   `sectors` sectors at logical block lba into buf and wait: CdlSetloc (2) with
   CdIntToPos(lba), CdRead(sectors, buf, CdlModeSpeed 0x80), CdReadSync(0, 0); 1 when that
   returned 0. */
/* func_80092E04 */
s32 cd_read(s32 sectors, s32 lba, u8 *buf)
{
    CdlLOC pos;

    CdIntToPos(lba, &pos);
    CdControl(2, (u_char *)&pos, 0);
    CdRead(sectors, (s32 *)buf, 0x80);
    return CdReadSync(0, 0) == 0;
}

INCLUDE_PAD(12);
