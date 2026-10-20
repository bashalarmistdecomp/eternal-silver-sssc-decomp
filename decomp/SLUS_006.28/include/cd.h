/* cd.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef CD_H
#define CD_H

#include "layout.h"

/* CdDirRow: one 0x18-byte row of the game's table of the disc's files, at the
   address in g_CdDirRows (0x80010100, stored by system_init; up to 0xA0 rows,
   g_CdDirRowCount of them): a file's or directory's name, flags, first sector
   and length. cd_dir_add makes each row from a directory record as the disc
   holds it (CdDirRecord); cd_dir_find, cd_load, cd_file_seek and
   cd_subdir_read read the rows. */
typedef struct {
    /* +0x00..+0x0D the file's name ended by a 0, without its version suffix:
       cd_dir_add copies the record's name up to its ';'. Nothing bounds the
       copy: a name of 14 characters or more runs over flags and first_sector,
       which are written before it. cd_dir_find compares it with the name it is
       asked for (strcmp) and answers the row's address. */
    s8 name[14];
    /* +0x0E the record's two flag bytes: low the record's +0x18 (the last byte
       of its recording date, the time zone), high its +0x19 (the file flags),
       both read signed, so a negative +0x18 fills the high byte too.
       cd_subdir_read reads the row of each directory (bit 0x200, the file
       flags' 2) on with cd_dir_read; nothing reads the low byte. Written by
       cd_dir_add. */
    s16 flags;
    /* +0x10 the file's first sector on the disc (the record's +2 and +4
       halves). cd_load reads from first_sector + (offset >> 11) with
       cd_sector_load; cd_file_seek seeks to it (CdIntToPos, CdControl 2 and
       0x15); cd_subdir_read reads a directory's records from it. Written by
       cd_dir_add. */
    s32 first_sector;
    /* +0x14 the file's length in bytes (the record's +0xA and +0xC halves):
       cd_load holds its read to it and answers it. Written by cd_dir_add. */
    s32 size;
} CdDirRow;

LAYOUT_SIZE(CdDirRow, 0x18);
LAYOUT_AT(CdDirRow, name, name, 0x00);
LAYOUT_AT(CdDirRow, flags, flags, 0x0e);
LAYOUT_AT(CdDirRow, first_sector, first_sector, 0x10);
LAYOUT_AT(CdDirRow, size, size, 0x14);

/* CdDirRecord: a directory record as the disc holds it (ISO 9660's directory
   record, a fixed part of 0x21 bytes and then the name): what cd_dir_add reads
   of each record cd_dir_read hands it from up to four sectors of a directory;
   0x22 bytes with the name's first byte. The disc's 32-bit values are read as
   their little-endian halves, because +2 and +0xA are not word-aligned. */
typedef struct {
    /* +0x00..+0x01 read as one 16-bit word: its low byte is the record's
       length in bytes (ISO 9660's byte 0), its high byte the extended
       attribute record's length (byte 1). cd_dir_read steps to the next record
       by length & 0xFFFE and ends a sector's records at a word of 0;
       cd_dir_add does not read it. */
    u16 length;
    /* +0x02 the low half of the file's first sector (ISO 9660's location of
       the extent, its little-endian copy at +2..+5): cd_dir_add makes the
       row's first_sector as sector_lo | sector_hi << 16. */
    u16 sector_lo;
    /* +0x04 the high half of the first sector, read with sector_lo by
       cd_dir_add. */
    u16 sector_hi;
    /* +0x06..+0x09 the first sector again, big-endian (ISO 9660 keeps both
       orders); nothing reads it. */
    u8 pad_06[4];
    /* +0x0A the low half of the file's length in bytes (ISO 9660's data
       length, its little-endian copy): cd_dir_add makes the row's size as
       size_lo | size_hi << 16; cd_dir_read reads the first record of a
       directory's first sector (the directory's own) the same way, as the
       directory's size, to count its sectors (at most 4). */
    u16 size_lo;
    /* +0x0C the high half of the length, read with size_lo (cd_dir_add,
       cd_dir_read). */
    u16 size_hi;
    /* +0x0E..+0x17 the length again, big-endian (+0xE..+0x11), and the first
       six bytes of the recording date (+0x12..+0x17); nothing reads them. */
    u8 pad_0e[10];
    /* +0x18 the recording date's last byte (ISO 9660: the offset from
       Greenwich in quarter hours): cd_dir_add puts it, read signed, in the low
       byte of the row's flags, where nothing reads it. */
    s8 zone;
    /* +0x19 the file flags (ISO 9660; 2 a directory): cd_dir_add puts them,
       read signed, in the high byte of the row's flags, whose bit 0x200
       cd_subdir_read tests. */
    s8 flags;
    /* +0x1A..+0x1F the file unit size, the interleave gap and the volume
       sequence number (ISO 9660); nothing reads them. */
    u8 pad_1a[6];
    /* +0x20 the name's length in bytes: the most cd_dir_add copies. */
    s8 name_len;
    /* +0x21 the first byte of the name, which runs on past the record's fixed
       part for name_len bytes, its version suffix ';1' last. cd_dir_add skips
       a record whose first byte, read signed, is 0x20 or below (the
       directory's own two records, names 0 and 1) and copies the rest to the
       row up to the ';'. Declared as a one-element array: the name runs on
       past the record's fixed part, and the code takes its address. */
    s8 name[1];
} CdDirRecord;

LAYOUT_SIZE(CdDirRecord, 0x22);
LAYOUT_AT(CdDirRecord, length, length, 0x00);
LAYOUT_AT(CdDirRecord, sector_lo, sector_lo, 0x02);
LAYOUT_AT(CdDirRecord, sector_hi, sector_hi, 0x04);
LAYOUT_AT(CdDirRecord, pad_06, pad_06, 0x06);
LAYOUT_AT(CdDirRecord, size_lo, size_lo, 0x0a);
LAYOUT_AT(CdDirRecord, size_hi, size_hi, 0x0c);
LAYOUT_AT(CdDirRecord, pad_0e, pad_0e, 0x0e);
LAYOUT_AT(CdDirRecord, zone, zone, 0x18);
LAYOUT_AT(CdDirRecord, flags, flags, 0x19);
LAYOUT_AT(CdDirRecord, pad_1a, pad_1a, 0x1a);
LAYOUT_AT(CdDirRecord, name_len, name_len, 0x20);
LAYOUT_AT(CdDirRecord, name, name, 0x21);

/* FileHostBlock: the block at the address in g_FileHostBlock (0x80010000,
   set once by system_init), below the program, which no statement under
   src/ writes: three of its bytes say whether the files are read from the
   developers' host PC and how. The type covers the bytes read; what else
   the block holds and who fills it are not known (g_FileHostBlock's register
   row). */
typedef struct {
    /* +0x00..+0x06 nothing reads or writes them. */
    u8 pad_00[7];
    /* +0x07 read signed by cd_dir_init: with host_mode not 0 and this 1, it
       reads no directory from the disc and only sets the audio file
       (xa_file_set). What it stands for is not known. */
    s8 unk_07;
    /* +0x08..+0x09 nothing reads or writes them. */
    u8 pad_08[2];
    /* +0x0A the host mode, read signed: above 0 when files are read from the
       developers' host PC: file_load then reads with pc_load from
       sim:\LUNA\MASTER\, file_seek and file_read_check do nothing;
       cd_dir_init tests it not 0 with unk_07. */
    s8 host_mode;
    /* +0x0B read signed by system_init and func_800217AC, which compare it
       with 5: not 5, system_init calls the empty func_8001B02C; 5,
       func_800217AC writes nothing to the host's console. What it stands
       for is not known. */
    s8 unk_0b;
} FileHostBlock;

LAYOUT_SIZE(FileHostBlock, 0x0c);
LAYOUT_AT(FileHostBlock, pad_00, pad_00, 0x00);
LAYOUT_AT(FileHostBlock, unk_07, unk_07, 0x07);
LAYOUT_AT(FileHostBlock, pad_08, pad_08, 0x08);
LAYOUT_AT(FileHostBlock, host_mode, host_mode, 0x0a);
LAYOUT_AT(FileHostBlock, unk_0b, unk_0b, 0x0b);

#endif
