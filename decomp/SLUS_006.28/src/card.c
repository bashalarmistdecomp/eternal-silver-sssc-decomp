/* Lunar SSSC — card: the memory card: its start, port and commands,
   accepts, format, directory, files and the save's read, write and header.
   The first split had it in cdrom_load.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"
#include "card.h"

/* g_CardDirEntries: the 15-entry table of kernel.h's struct DIRENTRY, the
   memory card's directory entry, that MemCardGetDirentry fills and
   card_dir_read and card_dir_find read. */
extern struct DIRENTRY g_CardDirEntries[15];
extern s32 g_CardAcceptResult;
extern s32 g_CardAcceptPreviousResult;
extern u8 g_CardSaveFileName[];
extern s32 g_CardPort;
extern s32 g_CardAcceptRunning;
extern s32 g_CardResult;
s32 MemCardInit(s32);
s32 MemCardStart();
void card_init(void);  /* TU defines this below */
s32 card_command_wait(s32);
void card_accept_finish(void);  /* TU defines this below */
s32 card_result_get(void);  /* TU defines this below */
void card_port_set(s32 port);
s32 MemCardAccept(s32);
s32 card_accept_wait(void);
s32 MemCardFormat(s32);
void card_format(void);
s32 MemCardReadFile(s32, s32, s32, s32, s32);
s32 card_file_read(void *name, s32 buffer, s32 offset, s32 byte_count);
void card_save_read(s32 slot, CardSave *save);
s32 card_save_write(s32 slot, CardSave *save);
s32 MemCardSync(s32, s32 *, s32 *);
s32 card_accept_poll(void);
s32 card_change_check(void);
s32 card_file_write(s32 name, s32 buffer, s32 offset, s32 byte_count);

s32 MemCardInit(s32);                                 /* extern */
s32 MemCardStart();                                  /* extern */

/* func_80024E3C */
void card_init(void) {
    MemCardInit(0);
    MemCardStart();
    _bu_init();
}

/* func_80024E6C */
void card_port_set(s32 port) {
    g_CardPort = port;
}
/* load_hazard_nop: card_command_wait */
/* func_80024E78 */
s32 card_command_wait(s32 command_mask)
{
  s32 result;
  s32 commands;
  do
  {
    MemCardSync(0, &commands, &result);
  }
  while ((commands & command_mask) != command_mask);
  if (command_mask == 2)
  {
    g_CardAcceptResult = result;
  }
  return result;
}

s32 card_command_wait(s32);                                 /* extern */

/* func_80024ED8 */
void card_accept_finish(void) {
    if (g_CardAcceptRunning != 0) {
        card_command_wait(2);
        g_CardAcceptRunning = 0;
    }
}
s32 MemCardAccept(s32);                               /* extern */

/* func_80024F04 */
s32 card_accept_wait(void) {
    card_accept_finish();
    MemCardAccept(g_CardPort);
    g_CardResult = card_command_wait(2);
}

/* func_80024F38 */
s32 card_result_get(void) {
    return g_CardResult;
}
s32 MemCardFormat(s32);                             /* extern */

/* func_80024F44 */
void card_format(void) {
    card_accept_finish();
    g_CardResult = MemCardFormat(g_CardPort);
}

/* la_absolute: card_dir_read */
extern s32 g_CardDirEntryCount;
extern s32 g_CardUsedBlockCount;
extern char g_CardDirPattern[];
s32 MemCardGetDirentry(s32 chan, char *name, struct DIRENTRY *dir, s32 *files, s32 ofs, s32 max);

/* The memory card's free blocks, for the save / load slot list. One caller,
   MenuCardRead (menu_card.c), which keeps the answer in g_MenuCardFreeBlockCount: negative
   is "no usable card", otherwise it bounds how many empty slots the list
   offers.

   1. card_accept_wait(): the pending card command is settled, MemCardAccept
      on the port g_CardPort and its result waited for (into g_CardAcceptResult and
      g_CardResult). Not 0: return -1.
   2. card_accept_finish() once more, then g_CardAcceptPreviousResult = g_CardAcceptResult: the accept
      result becomes the "seen" one card_change_check compares the next
      against.
   3. g_CardResult = MemCardGetDirentry, libmcrd's MemCardGetDirentry (port, "*.*"
      (g_CardDirPattern), g_CardDirEntries, &g_CardDirEntryCount, 0, 15): every file on the card
      into the 15-entry directory table g_CardDirEntries (0x28 bytes each, 0x258
      in all) and their number into g_CardDirEntryCount. Not 0: return -1.
   4. g_CardUsedBlockCount = the blocks those files take: each entry's size (+0x18,
      in bytes) rounded up to 8 KB blocks by a signed division, summed in
      memory.
   5. Returns 15 - g_CardUsedBlockCount.

   card_dir_find looks a file name up in the table this leaves. No RNG draw;
   no frame wait of its own (card_command_wait spins on MemCardSync).

   Shapes the bytes depend on (each measured by changing it alone):
   - the table is the struct array indexed by i: the size is read
     `lw 0x18($a2)` through a pointer stepped from the table's base, which
     $s0 holds across the call. A `u8[]` read through a cast at
     `+ i * 0x28 + 0x18` folds the 0x18 into the stepped pointer (50/52).
   - the compile marker above. The target takes &g_CardDirEntryCount as `lui $a3,
     %hi` / `addiu $a3, $a3, %lo`, adjacent and on ONE register
     (0x80024FB0), and loads the same symbol `%gp_rel` at 0x80024FD4: the
     Sony assembler's `la` of a small-data symbol (PictureVramSave and
     PictureVramRestore carry the marker for the same reason). Without it `as -G8` makes one gp-relative `addiu` (49/52,
     one instruction short). The far twin g_CardDirEntryCount_blk that config/
     registers for this TU matches as well; it is a second name for one
     variable and is not used.
   - g_CardDirPattern has no size on purpose. Its four bytes "*.*" sit in .sdata,
     yet the target splits the address (`lui $a1` three instructions ahead
     of its `addiu $a1, $a1`), which cc1 does only for a symbol it does not
     know to be small: sized `char [4]` it becomes an `la` and the `lui`
     moves (51/52). A string literal written in the call compiles that way
     under -G8 (the constant is placed in .sdata by its size, but only a
     variable's symbol is flagged small); the bytes are still in the data
     segment's asm, so the C names them.
   Two early `return -1`s and one result expression; the file count is
   read once ahead of the loop by the compiler, the block sum is not. */
/* func_80024F70 */
s32 card_dir_read(void)
{
    s32 i;

    if (card_accept_wait() != 0) {
        return -1;
    }
    card_accept_finish();
    g_CardAcceptPreviousResult = g_CardAcceptResult;
    g_CardResult = MemCardGetDirentry(g_CardPort, g_CardDirPattern, g_CardDirEntries, &g_CardDirEntryCount, 0, 15);
    if (g_CardResult != 0) {
        return -1;
    }
    g_CardUsedBlockCount = 0;
    for (i = 0; i < g_CardDirEntryCount; i++) {
        g_CardUsedBlockCount += (g_CardDirEntries[i].size + 0x1FFF) / 0x2000;
    }
    return 15 - g_CardUsedBlockCount;
}

/* Is there a file of this name on the memory card? Looks `name` up in the
   directory table card_dir_read read: for each of the g_CardDirEntryCount entries of
   g_CardDirEntries, strcmp of `name` with the entry's name (+0, 20
   bytes). Returns 0 at the first equal name and -1 when there is none, so a
   true answer means "not on the card".

   Two callers, both below: card_file_write and card_save_write create the save
   file (MemCardCreateFile) when the answer is not 0, and
   write it either way. The card is not read here; the table and the count
   are whatever the last card_dir_read left, and the count is read again from
   memory on every pass. Writes nothing. No RNG draw, no frame wait.

   Shapes (each measured by changing it alone): the table indexed by i in a
   plain loop with an early `return 0` (a stepped pointer local is 25/30;
   one result variable and a `break` 21/30). The compiler steps the entry
   pointer and counts i in the branch's delay slot by itself. `&table[i]`
   for the name and a `while` loop give the same bytes. */
/* func_80025040 */
s32 card_dir_find(char *name)
{
    s32 i;

    for (i = 0; i < g_CardDirEntryCount; i++) {
        if (strcmp(name, g_CardDirEntries[i].name) == 0) {
            return 0;
        }
    }
    return -1;
}

s32 MemCardReadFile(s32, s32, s32, s32, s32);               /* extern */

/* Returns s32: the trailing card_command_wait(3) leaves its result in $v0 and
   menu_card.c's card_save_records_head_read (func_8007473C) returns it (`return card_file_read(&buf, ...)`,
   byte-matching). There is deliberately no `return` statement — writing
   one would re-read g_CardResult and change the bytes; the fall-through is
   what the original emits. */
/* func_800250B8 */
s32 card_file_read(void *name, s32 buffer, s32 offset, s32 byte_count) {
    s32 padded_count;

    padded_count = byte_count + 0x7F;
    if (padded_count < 0) {
        padded_count = byte_count + 0xFE;
    }
    MemCardReadFile(g_CardPort, (s32)name, buffer, offset, (padded_count >> 7) << 7);
    g_CardResult = card_command_wait(3);
}
/* far_syms: g_CardSaveFileName (target reaches it %hi/%lo) */

/* func_80025110 */
void card_save_read(s32 slot, CardSave *save) {
    u8 buf[0x14];

    strcpy(buf, g_CardSaveFileName);
    buf[0x13] += slot % 10;
    buf[0x12] += (slot / 10) % 10;
    buf[0x11] += (slot / 10 / 10) % 10;
    MemCardReadFile(g_CardPort, buf, save, 0, sizeof(CardSave));
    g_CardResult = card_command_wait(3);
}
s32 card_save_write(s32 slot, CardSave *save);  /* TU defines this below */
s32 MemCardSync(s32, s32 *, s32 *);
s32 card_accept_poll(void);  /* TU defines this below */
s32 card_change_check(void);  
/* func_8002520C */
s32 card_file_write(s32 name, s32 buffer, s32 offset, s32 byte_count)
{
  s32 kept_byte_count;
  s32 missing_then_padded_count;
  s32 block_padded_count;
  s32 *byte_count_home;
  missing_then_padded_count = card_dir_find((char *)name);
  kept_byte_count = *(byte_count_home = &byte_count);
  if (missing_then_padded_count)
  {
    block_padded_count = kept_byte_count + 0x1FFF;
    if (block_padded_count < 0)
    {
      block_padded_count = kept_byte_count + 0x3FFE;
    }
    MemCardCreateFile(g_CardPort, name, block_padded_count >> 13);
  }
  missing_then_padded_count = kept_byte_count + 0x7F;
  if (missing_then_padded_count < 0)
  {
    missing_then_padded_count = kept_byte_count + 0xFE;
  }
  MemCardWriteFile(g_CardPort, name, buffer, offset, (missing_then_padded_count >> 7) << 7);
  g_CardResult = card_command_wait(4);
}

/* func_800252B0 */
s32 card_save_write(s32 slot, CardSave *save) {
    u8 buf[0x14];

    strcpy(buf, g_CardSaveFileName);
    buf[0x13] += slot % 10;
    buf[0x12] += (slot / 10) % 10;
    buf[0x11] += (slot / 10 / 10) % 10;
    if (card_dir_find(buf)) {
        MemCardCreateFile(g_CardPort, buf, 1);
    }
    MemCardWriteFile(g_CardPort, buf, save, 0, sizeof(CardSave));
    g_CardResult = card_command_wait(4);
    return g_CardResult;
}
/* far_syms: g_CardSaveIconPalettes (target reaches it %hi/%lo) */
/* far_syms: g_CardSaveIconFrames (target reaches it %hi/%lo) */
extern u8 g_CardSaveIconPalettes[];
extern u8 g_CardSaveIconFrames[];

/* The head of a save (CardSave, card.h, +0x0000..+0x01FF) in the buffer
   MenuSaveWrite builds: the letters "SC", the icon's frame count and the
   file's blocks, the title card_save_title_set makes, the 0x1C reserved
   zero bytes behind it (which cut a title that ran long), and one of three
   icon palettes and one of three sets of icon frames, picked by the save's
   number % 3. The fields take the card's file format's words. */
/* func_800253C8 */
void card_save_header_set(CardSave *save, u16 slot, u16 play_time, u16 place_name, u16 level, u16 unused, u16 unused2, u16 unused3, u16 unused4)
{
    u16 val;

    val = slot;
    save->magic[0] = 'S';
    save->magic[1] = 'C';
    save->icon_frame_count = 0x13;
    save->block_count = 0x01;
    card_save_title_set(save->title, val, place_name, level, play_time);
    bzero(save->reserved, sizeof(save->reserved));
    val = val % 3;
    memcpy(save->icon_palette, g_CardSaveIconPalettes + val * sizeof(save->icon_palette), sizeof(save->icon_palette));
    memcpy(save->icon_frames, g_CardSaveIconFrames + val * sizeof(save->icon_frames), sizeof(save->icon_frames));
}

s32 MemCardSync(s32, s32 *, s32 *);                 /* extern */

/* func_800255A0 */
s32 card_accept_poll(void) {
    s32 result;
    s32 commands;
    s32 command_state;

    if (g_CardAcceptRunning == 0) {
        if (MemCardAccept(g_CardPort) != 0) {
            g_CardAcceptRunning = 1;
        }
    } else {
        command_state = MemCardSync(1, &commands, &result);
        if (command_state != -1) {
            if ((command_state == 1) && (commands & 2)) {
                g_CardAcceptRunning = 0;
                g_CardAcceptResult = result;
            }
        } else {
            g_CardAcceptRunning = 0;
        }
    }
    return g_CardAcceptResult;
}

/* load_hazard_nop: card_change_check */
/* func_80025638 */
s32 card_change_check(void) {
    s32 changed_result;

    changed_result = 0;
    if ((g_CardAcceptResult != g_CardAcceptPreviousResult) && (g_CardAcceptResult != 0)) {
        changed_result = g_CardAcceptResult;
    }
    g_CardAcceptPreviousResult = g_CardAcceptResult;
    return changed_result;
}
