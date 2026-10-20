/* Lunar SSSC — cdread: the reading of files: sectors read and checked, the
   disc's directory read and searched, file_load, file_seek and cd_load, and
   pc_load, the host PC's twin of cd_load. The name is the developers' own:
   CDREAD.C, the source file pc_load's three assertions name ('PC open
   ERROR', 'lseek OFFSET error', 'PC read ERROR'); the file's other
   functions have it by being of its group. The first split had it in io.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
extern u_long *g_CdReadBuffer;
extern s32 g_CdDirRowCount;
extern s32 g_CdDirRows;
extern void *g_FileHostBlock;
extern s32 g_CdWaitBtlFrame;
s32 file_load(u8 *, u32, s32, s32);
s32 file_seek(s32 *, s32);
void cd_wait_btl_frame_set(s32 wait_btl_frame);
extern long HWD0, VWD0;
extern long HWD0;
extern long VWD0;
s32 cd_file_seek();
s32 cd_read_check(s32);
s32 file_read_check(void);
s32 cd_dir_read(s32, s32);
void cd_subdir_read(s32 row_count, s32 buffer);
s32 debug_printf();
s32 assert_fail(s32 *, s32 *, s32);
extern long HWD0, VWD0;
void cd_wait_btl_frame_set(s32 wait_btl_frame);
void cd_wait_btl_frame_set(s32 wait_btl_frame);


/* cd_read_start -- start a CD read, (lba, buf, size): `size` bytes, rounded
   up to whole 2048-byte sectors, from logical sector lba of the disc to the
   address buf. It only starts the read; cd_read_check below answers how it
   stands. One call site in the image: cd_sector_load, the CD read's
   wait-and-retry loop, once per try.

   In order:
     1. g_CdReadBuffer = buf: the destination of the read in flight (gp
        global; cd_read_check is its only other user).
     2. *buf = 0x5555AAA5: a marker in the buffer's first word. A read
        that libcd reports complete with this word still in place is
        cd_read_check's -2, "no sector reached the buffer".
     3. size += 0x7FF; size /= 0x800: the sector count. The division is
        signed (a negative sum gets 0x7FF more before the shift).
        cd_sector_load hands on what its own callers pass: 0x800
        (cd_dir_init), 0x2000 (cd_dir_read) and cd_load's byte
        length, which is 1 for file_load's host-mode read: one sector.
     4. CdIntToPos(lba, &loc): libcd CdIntToPos, lba to minute /
        second / sector in the CdlLOC on this frame (sp+0x10).
     5. CdControl(2, &loc, 0): CdlSetloc.
     6. CdRead(sectors, buf, 0x80): libcd CdRead with mode 0x80,
        CdlModeSpeed (double speed).

   Reads no global; writes g_CdReadBuffer and the first word of the buffer.
   Callees: CdIntToPos, CdControl, CdRead. No RNG draw. No frame
   wait of its own (the waiting is cd_sector_load's, one cd_read_check(1)
   per frame). Untestable by the behaviour harness (station-tools): the
   original does not finish under it.

   Shapes (each measured by changing it alone in this text):
   - the sum is stored in the variable, then divided: `size += 0x7FF;
     size /= 0x800;`. The target keeps the sum in $s0 and its division
     adds a second 0x7FF to that register for a negative sum. Written as
     one expression, `size = (size + 0x7FF) / 0x800;`, the two constants
     are folded into `addiu $v1, $s0, 0xFFE` off the unsummed size: 24/29.
     A separate `sectors` local is 25/29, the quotient written inside
     CdRead's argument 17/29, `size += 0x7FF;` with `size / 0x800` in the
     argument 22/29, and `size >>= 11` 16/29 (no sign fix-up at all).
   - the two stores come first, g_CdReadBuffer before the marker. The marker
     first is 27/29; the sector count ahead of the stores is 20/29.
   - lba is never copied: $a0 goes into CdIntToPos as it arrived.
   - `u8 loc[4]`, as cd_file_seek has it, gives the same 29 instructions
     with no casts; CdlLOC is the SDK's type for what CdIntToPos fills,
     and the casts are for this TU's prototype of CdIntToPos.

   MARKED SPELLING: `s32 buf`. It is an address (CdRead's `u_long *buf`),
   and `u_long *buf` gives the same 29 instructions (29/29). It is kept an
   int because the one caller, cd_sector_load below, carries the address in
   an `s32` (as its own callers cd_load and cd_dir_read do), and a
   pointer parameter would make that call an implicit int-to-pointer
   conversion.
   MARKED SPELLING: `void`. The target leaves with CdRead's answer still
   in $v0 and its caller does not read it; `return CdRead(...);`
   in an `s32` function gives the same 29 instructions (29/29), so the
   bytes do not say whether the source returned a value. Nothing in the
   tree declared a return type for it.

   No marker, no forcing device. */
/* func_80020DC0 */
void cd_read_start(s32 lba, s32 buf, s32 size)
{
    CdlLOC loc;

    g_CdReadBuffer = (u_long *)buf;
    *g_CdReadBuffer = 0x5555AAA5;
    size += 0x7FF;
    size /= 0x800;
    CdIntToPos(lba, (u8 *)&loc);
    CdControl(2, (u_char *)&loc, 0);
    CdRead(size, (s32 *)buf, 0x80);
}

s32 CdReadSync(s32 mode, u8 *result);

/* cd_read_check -- the status of the CD read that cd_read_start started,
   (mode) -> s32.

   cd_read_start(lba, buf, size) keeps the destination in g_CdReadBuffer and
   writes the word 0x5555AAA5 at buf[0] before it seeks and calls CdRead.
   This function asks libcd how the read stands, r = CdReadSync(mode, 0)
   (CDREAD.OBJ's), and answers
     r   when r != 0: sectors still to come (> 0), or libcd's -1 (more
         than 1200 vblanks since CdRead);
     -2  when r == 0 (libcd says the read is complete) but the first word
         of the buffer *g_CdReadBuffer is still 0x5555AAA5, i.e. no sector
         reached the buffer;
     0   when r == 0 and that word was overwritten.
   A file whose own first word is 0x5555AAA5 would read as -2 every time.

   Reads g_CdReadBuffer and the word it points at; writes nothing. mode is
   handed to CdReadSync as it is: 0 waits inside libcd until the read ends
   (a busy loop on the vblank count, no frame is presented), non-zero
   returns at once. No RNG draw.

   Callers: cd_sector_load (mode 1, once per frame of its wait loop: > 0
   waits on, up to 0x1E0 frames, 0 ends the load, anything else restarts
   the read) and file_read_check (mode 0, when the signed byte at +0xA of
   the block g_FileHostBlock points at is <= 0).

   Shapes (each measured): one `&&` test with an early `return -2`
   matches; the same test as two nested ifs matches too.
   `if (r != 0 || word != K) return r; return -2;` and the conditional
   expression are 11/18, and assigning -2 to r (or to a second local) is
   11/18: the target keeps r in $a1 and builds -2 straight into $v0 in the
   branch's delay slot. No marker, no forcing device. */
/* func_80020E34 */
s32 cd_read_check(s32 sync_mode) {
    s32 r;

    r = CdReadSync(sync_mode, 0);
    if (r == 0 && *g_CdReadBuffer == 0x5555AAA5) {
        return -2;
    }
    return r;
}

/* func_80020E7C */
void cd_wait_btl_frame_set(s32 wait_btl_frame) {
    g_CdWaitBtlFrame = wait_btl_frame;
}
/* far_syms: g_CdLunadataFileName (target reaches it %hi/%lo) */
/* far_syms: g_BtlRunning (target reaches it %hi/%lo) */
/* far_syms: g_BtlRunning (auto: target reaches it %hi/%lo — addressing row 3) */
extern u8 g_CdLunadataFileName[];
extern s32 g_XaPlayCount;
extern u8 g_BtlRunning;

/* func_80020E88 */
void cd_sector_load(s32 sector, s32 buffer, s32 byte_count)
{
  s32 result;
  s32 mid_count;
  s32 outer_count;
  s32 buf[6];
  outer_count = 0;
 if (g_XaPlayCount != 0) { VoiceStep(0);
  }
  while (1)
  {
    result = byte_count;
    cd_read_start(sector, buffer, result);
    mid_count = 0;
    while (1)
    {
      result = cd_read_check(1);
      if (result <= 0)
      {
        break;
      }
      if ((g_BtlRunning != 0) && (g_CdWaitBtlFrame != 0))
      {
        BtlFrameStep();
      }
      else
      {
        VSync(0);
      }
      mid_count++;
      if (mid_count >= 0x1E0)
      {
        break;
      }
    }

    if (result == 0)
    {
      break;
    }
    outer_count++;
    if ((CdSearchFile(buf, g_CdLunadataFileName) != 0) && (outer_count < 0x40))
    {
      continue;
    }
    CdInit();
    outer_count = 0;
  }

}
/* far_syms: g_CdDirAudioXaFileName (target reaches it %hi/%lo) */
u8 *cd_dir_find(u8 *name);  /* TU defines this below */
extern u8 g_CdDirAudioXaFileName[];

/* func_80020F98 */
s32 cd_dir_init()
{
  u16 buf[0x1000];
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `one` only holds the constant 1; behaviour is the same with the constant written
     where it is read, but then 5 of this function's 39 instruction lines differ. */
  char one;
  FileHostBlock *p;
  p = (FileHostBlock *) g_FileHostBlock;
  g_XaPlayCount = 0;
  g_CdWaitBtlFrame = 0;
  if ((p->host_mode != 0) && (p->unk_07 == 1))
  {
    xa_file_set(g_CdDirAudioXaFileName);
    return 0;
  }
  cd_toc_read();
  cd_sector_load(0x12, (s32) buf, 0x800);
  g_CdDirRowCount = 0;
  one = 1;
  if ((buf[one] | (buf[2] << 16)) != 0x16)
  {
    if (one)
    {
      return -one;
    }
  }
  cd_dir_read(0x16, (s32) buf);
  xa_file_set(g_CdDirAudioXaFileName);
  return 0;
}
extern s32 D_800B86E0;
extern u8 g_FileHostPathFormat[];
extern u8 g_FileLunadataFileName[];
extern u8 g_FileHostDirectory[];
s32 cd_load(s32 name, s32 buffer, s32 offset, s32 byte_count);  /* TU defines this below */
s32 pc_load(u8 *name, u8 *dst, s32 offset, s32 len);  /* TU defines this below */
s32 fmt_sprintf();  /* extern */

/* file_load -- the read of part of a named file that every loader goes
   through, (name, dst, offset, len) -> s32: `len` bytes at byte offset
   `offset` of the file `name` to the address dst, from the CD or, in a
   development build's host mode, from the host PC. Three call sites in the
   image: resource_load above (every by-number LUNADATA resource),
   overlay_disc_wait and overlay_load (overlay.c, the overlay files).
   None reads the result.

   The mode is the signed byte at +0xA of the block g_FileHostBlock points at,
   the byte file_seek, file_read_check and cd_dir_init test too.

   <= 0, the CD:
       cd_load(name, dst, offset, len)
     the CD read (it looks `name` up in the directory table g_CdDirRows and
     runs cd_sector_load on the file's first sector + (offset >> 11)).

   > 0, the host PC. Four steps, in this order:
     1. cd_load(g_FileLunadataFileName, dst, D_800B86E0 << 24, 1): a CD read all
        the same, of the file "LUNADATA.FIL" (the string at g_FileLunadataFileName),
        length 1 (cd_read_start rounds it up to one sector, so 2048 bytes
        land at dst), at byte offset 0 or 0x1000000, i.e. sector 0 or
        sector 0x2000 of the archive, as D_800B86E0 is 0 or 1.
     2. D_800B86E0 ^= 1. The word is 0 in the image and nothing else in the
        image's asm touches it, so successive host-mode loads read the two
        positions in turn and the drive seeks 0x2000 sectors each time.
        (What that is for is not in the code; a stand-in for the CD's seek
        time while the data comes from the host is a guess.)
     3. fmt_sprintf(path, g_FileHostPathFormat, g_FileHostDirectory, name): the game's
        sprintf with the format "%s%s" (g_FileHostPathFormat, five bytes in the
        small-data area) and the string "sim:\LUNA\MASTER\" (g_FileHostDirectory),
        so path = "sim:\LUNA\MASTER\<name>". path is 64 bytes of this
        frame (0x10..0x4F); nothing bounds the length of name.
     4. pc_load(path, dst, offset, len): the host read, which
        overwrites what step 1 left at dst.

   Returns what its last call returned: cd_load's answer on the CD
   (the file's length word, or -1 when the name is not in the table),
   pc_load's masked length on the host.

   Reads the mode byte; reads and writes D_800B86E0 in host mode only.
   Callees: cd_load, fmt_sprintf, pc_load. No RNG draw. No
   frame wait of its own: the CD read's loop (cd_sector_load) presents or
   waits a frame per pass until the read is done, which is why the
   captured inputs are UNTESTABLE (the behaviour harness of station-tools:
   the original does not finish under it).

   Shapes (each measured against this text):
   - one if / else whose arms each make their own last call. With a
     `return` written in each arm (as if / else, or the host arm returning
     and the CD read after the if) it is 42/50: the CD arm is then laid
     out first, straight after the test, where no label has been crossed
     since entry and the four argument copies $sN -> $aN are dropped as
     no-ops, and the host arm sits behind a label and gains `move $a1,
     $s0`. The target is the other way round: the host arm falls through
     from the test and its first call has no instruction for $a1 at all
     (dst is still there from entry), and the CD arm, behind the label
     .L800210CC, copies all four. That is why dst is the first call's
     second argument: nothing else leaves $a1 unwritten there.
   - `path` is the only local in memory: frame 0x68 = 16 (argument homes)
     + 64 + five saved registers. The same logic plus an unused
     `s32 pad[2]` makes the frame 0x70: 38/50.
   - "%s%s" lies in the small-data area and the target still forms its
     address with lui / addiu: an unsized array declaration gives that.

   MARKED SPELLING: the result local `r` and `return r;`. The target
   leaves both arms through the one epilogue with the callee's answer
   still in $v0, and no caller reads it. The same if / else with no
   `return` statement at all (50/50), and an early `return` of the CD read
   on `<= 0` ahead of the host steps (50/50), give the same 50
   instructions: the bytes do not say whether the source returned a
   value. `return r;` is kept because the TU, overlay.c and
   resource_load.c all prototype the function `s32` and it is what the code does.

   No marker, no forcing device. */
/* func_80021034 */
s32 file_load(u8 *name, u32 dst, s32 offset, s32 len)
{
    u8 path[64];
    s32 r;

    if (((FileHostBlock *)g_FileHostBlock)->host_mode > 0) {
        cd_load((s32)g_FileLunadataFileName, dst, D_800B86E0 << 24, 1);
        D_800B86E0 ^= 1;
        fmt_sprintf(path, g_FileHostPathFormat, g_FileHostDirectory, name);
        r = pc_load(path, (u8 *)dst, offset, len);
    } else {
        r = cd_load((s32)name, dst, offset, len);
    }
    return r;
}

s32 cd_file_seek();                                  /* extern */

/* func_800210FC */
s32 file_seek(s32 *name, s32 offset) {
    if (((FileHostBlock *)g_FileHostBlock)->host_mode <= 0) {
        cd_file_seek();
    }
}

s32 cd_read_check(s32);                               /* extern */

/* func_80021130 */
s32 file_read_check(void)
{
  s32 read_state;
  read_state = 0;
  if (((FileHostBlock *)g_FileHostBlock)->host_mode <= 0)
  {
    read_state = cd_read_check(0);
    return read_state;
  }
  return read_state;
}

/* func_80021164 */
u8 *cd_dir_find(u8 *name)
{
  u8 *dir_entry;
  u8 *kept_name;
  s32 i;

  kept_name = name;
  i = 0;
  dir_entry = (u8 *)g_CdDirRows;
  if (g_CdDirRowCount > 0)
  {
    do
    {
      if (strcmp(kept_name, dir_entry) == 0)
      {
        return dir_entry;
      }
      i += 1;
      dir_entry += 0x18;
    } while (i < g_CdDirRowCount);
  }
  return 0;
}
extern u8 D_800115E0[];

/* func_800211D8 */
s32 cd_load(s32 name, s32 buffer, s32 offset, s32 byte_count)
{
  s32 file_byte_count;
  u8 unused;
  void *dir_entry;
  dir_entry = cd_dir_find((u8 *)name);
  if (dir_entry != 0)
  {
    file_byte_count = ((CdDirRow *)dir_entry)->size;
    byte_count &= 0x7FFFFFFF;
    if (byte_count >= file_byte_count)
    {
      byte_count = file_byte_count;
    }
    cd_sector_load(((CdDirRow *)dir_entry)->first_sector + (offset >> 0xB), buffer, byte_count);
    return ((CdDirRow *)dir_entry)->size;
  }
  debug_printf(D_800115E0, name);
  return -1;
}


/* func_80021284 */
s32 cd_file_seek(u8 *name, s32 offset)
{
    u8 loc[4];
    u8 *entry;

    if (g_XaPlayCount == 0) {
        entry = cd_dir_find(name);
        if (entry != 0) {
            CdIntToPos(((CdDirRow *)entry)->first_sector + (offset >> 11), loc);
            CdControl(2, loc, 0);
            CdControl(0x15, 0, 0);
        }
    }
}

/* func_800212EC */
s32 cd_dir_read(s32 sector, s32 buffer)
{
  u8 *sector_data;
  u8 *dir_record;
  u8 *sector_end;
  s32 sectors_done;
  s32 sector_count;
  s32 dir_byte_count;

  sector_data = (u8 *)buffer;
  cd_sector_load(sector, buffer, 0x2000);
  dir_byte_count = ((CdDirRecord *)sector_data)->size_lo | (((CdDirRecord *)sector_data)->size_hi << 16);
  if (dir_byte_count < 0)
    dir_byte_count += 0x7FF;
  sector_count = dir_byte_count >> 11;
  if (sector_count >= 4)
    sector_count = 4;
  sectors_done = 0;
  while (sectors_done < sector_count)
  {
    if (*(u16 *)sector_data == 0)
      goto L800213B8;
    dir_record = sector_data;
    sector_end = sector_data + 0x800;
    do
    {
      cd_dir_add((s32)dir_record);
      dir_record += *(u16 *)dir_record & 0xFFFE;
      if (dir_record >= sector_end)
        goto L800213B8;
      if (g_CdDirRowCount >= 0xA0)
        goto L800213B8;
    } while (*(u16 *)dir_record != 0);
    sectors_done++;
    sector_data += 0x800;
  }
L800213B8:
}

s32 cd_dir_read(s32, s32);

/* func_800213D8 */
void cd_subdir_read(s32 row_count, s32 buffer)
{
  s32 i;
  s32 *first_sector;
  s32 dir_rows;
  i = 0;
  dir_rows = g_CdDirRows;
  if (row_count > 0)
  {
    first_sector = (s32 *)(dir_rows + 0x10);
    do
    {
      if (((short)(*((s32 *) (((u8 *) first_sector) - 2)))) & 0x200)
      {
        cd_dir_read(first_sector[0], buffer);
      }
      i += 1;
      first_sector += 6;
    }
    while (i < row_count);
  }
}

/* Add one directory record of the disc to the game's directory table:
   cd_dir_add(rec). Its one caller is cd_dir_read above, which reads
   up to four sectors of a directory and hands over each record in turn
   (it stops at 0xA0 rows).

   A record whose first name character, read as a signed char, is 0x20 or
   below is skipped with nothing touched: that is the directory's own two
   entries (names 0x00 and 0x01), and any name that starts with a control
   character, a space or a byte of 0x80 and up. Otherwise the row is
   (CdDirRow *)g_CdDirRows + g_CdDirRowCount, and g_CdDirRowCount goes up by one before
   the row is filled:
     lba   = lba_lo | lba_hi << 16        (record +2, +4)
     size  = size_lo | size_hi << 16      (record +0xA, +0xC)
     flags = zone | flags << 8            (record +0x18, +0x19, both as
                                           signed chars, so a negative byte
                                           at +0x18 sets the whole high
                                           byte; cd_subdir_read tests 0x200,
                                           the directory bit)
     name  = up to name_len (record +0x20) characters from record +0x21,
             ending early at a ';' (the version suffix), then a 0.
   The copy has no bound of its own: the name field is 14 bytes and the
   three values above are stored first, so a name of 14 characters or more
   would run over flags, then lba. Readers of the table: cd_dir_find
   (lookup by name), cd_load (lba, size), cd_file_seek (lba),
   cd_subdir_read (flags, lba). Reads g_CdDirRows; reads and writes
   g_CdDirRowCount; writes the row. No call, no RNG, no frame wait.

   Shapes, each measured by changing it alone in this text:
     - the name is written through its own pointer, set once the three
       values are stored. Walking the row pointer itself is 16/47
       (measured on the cast spelling of this text, `u8 *ent` with
       `*(s32 *)(ent + 0x10)`, which matches too). Read in the allocator
       dumps: with its own name pointer the row pointer lives inside one
       basic block, so local-alloc gives it $a0 before the global
       allocator places anything; the record parameter then cannot stay
       in $a0 and goes to $a1, which is the target's first instruction
       `addu $a1, $a0, $zero`, and the name pointer takes over $a0. Walked,
       the row pointer is a global pseudo and gets $a1, the record $a2.
     - the count's `++` is inside the row expression. As a statement of
       its own after it: 41/47 (the store of the count moves ahead of the
       multiply). Base first: the product first is 41/47.
     - `lo | hi << 16` in that order (the lba as `hi << 16 | lo`: 45/47).
     - the record's bytes at +0x18, +0x19 and +0x20 are signed chars
       (unsigned +0x18 / +0x19: 38/47; unsigned +0x20: 46/47); the count
       `n` is an int (`s8 n`: 41/47).
     - `*dst++ = *name++;` as one statement (three statements: 43/47).
   Byte-equal: `&((CdDirRow *)g_CdDirRows)[g_CdDirRowCount++]`; the test as
   `rec->name[0] > 0x20`, as `>= 0x21`, or as an early return; a while
   loop, or the ';' test folded into the loop condition with `&&`; `u8`
   for the row's name and for `dst`; `u16` for the row's flags; `u8`
   members at +0x18 / +0x19 read with `(s8)` casts.

   MARKED SPELLING: the `s32` return type. Nothing is returned and the
   caller reads nothing: the function is `void (CdDirRecord *)` by the
   bytes. cd_dir_read calls it above with no declaration in scope
   (`cd_dir_add((s32)s0);`), which declares it implicitly as returning
   int, and `s32` is the definition that agrees with that; `void` gives
   the same 47 instructions and the compiler's "type mismatch with
   previous implicit declaration" warning. */
/* func_80021454 */
s32 cd_dir_add(CdDirRecord *rec)
{
    CdDirRow *ent;
    s8 *dst;
    s8 *name;
    s32 n;

    name = rec->name;
    if (*name > 0x20) {
        ent = (CdDirRow *)g_CdDirRows + g_CdDirRowCount++;
        ent->first_sector = rec->sector_lo | (rec->sector_hi << 16);
        ent->size = rec->size_lo | (rec->size_hi << 16);
        ent->flags = rec->zone | (rec->flags << 8);
        dst = ent->name;
        for (n = rec->name_len; n != 0; n--) {
            if (*name == ';') {
                break;
            }
            *dst++ = *name++;
        }
        *dst = 0;
    }
}

/* A hex dump whose print calls are not in this build: hex_dump(p, n).
   It walks n bytes from p, collecting them sixteen at a time, each widened
   to an int, into a row on its own stack (the frame is that row: 0x40
   bytes at $sp + 0); after every full row it walks the sixteen values and
   turns each one below 0x20 into '.', the text column of a dump line. The
   clamped value goes nowhere: there is no call, no global is read or
   written and nothing is stored outside the frame, so in this build the
   function has no effect at all. A last row of fewer than sixteen bytes
   is collected and never walked. No result ($v0 is not set; no caller
   reads it). No RNG, no frame wait.

   Callers, all three for a state the code treats as an error:
   bg_block_load and bg_attribute_block_load (bg_tile.c) pass their
   layout table and 0x210 when a block's raw-data offset is odd;
   MessageStringDisp (message_disp.c) passes its record and 0x10. They declare
   it `s32 hex_dump();`, with no prototype.

   Shapes: one counter for the fill position and the row walk (a second
   variable for the walk is 23/29, a register swap); `i >= 16` (`i == 16`
   is 25/29); `for (i = 0; n > 0; n--)` is byte-equal to the while loop.

   MARKED SPELLING: the second test, `if (c >= 0x7F) c = '.';`. Its
   operator and constant are NOT in the bytes; what the bytes hold is that
   a second test read `c` after the first one. Evidence: the target forms
   '.' in $v1 at 0x80021558 (`addiu $v1, $zero, 0x2E`) and nothing reads
   it, and the `beqz` at 0x80021550 jumps over it with an empty delay slot.
   With the first test alone the clamp is dead code and flow deletes all
   six rows 0x80021544-0x80021558 (17/29). With a second test on `c` whose
   own assignment is dead, the compiler's dumps show: flow deletes that
   second assignment and keeps the first, because the second test's `slti`
   reads `c` (the block after the first test starts with `c` live); the
   second test's branch then goes to the next instruction, and it and its
   `slti` are still in the sched2 dump and gone in the dbr dump, while the
   first test and its `li` stay. Byte-equal second tests, each measured:
   `c > 0x7E`, `c >= 0x80`, `c != 0`, and any other value assigned in its
   body. Not byte-equal: both bounds in one test with `||` (17/29), an
   else-if (20/29), an empty second body (17/29), `c == 0x7F` (28/29).
   0x7F is the first value past printable ASCII, the usual upper bound of
   this clamp; the function has no Japanese twin to read it from. */
/* func_80021510 */
void hex_dump(u8 *p, s32 n)
{
    s32 buf[16];
    s32 i;
    s32 c;

    i = 0;
    while (n > 0) {
        buf[i++] = *p++;
        if (i >= 16) {
            for (i = 0; i < 16; i++) {
                c = buf[i];
                if (c < 0x20) {
                    c = '.';
                }
                if (c >= 0x7F) {
                    c = '.';
                }
            }
            i = 0;
        }
        n--;
    }
}

/* far_syms: D_80011608 (target reaches it %hi/%lo) */
/* far_syms: D_80011624 (target reaches it %hi/%lo) */
/* far_syms: D_80011634 (target reaches it %hi/%lo) */
/* far_syms: D_80011640 (target reaches it %hi/%lo) */
/* far_syms: D_80011654 (target reaches it %hi/%lo) */
extern u8 D_80011608[];
extern u8 D_80011624[];
extern u8 D_80011634[];
extern u8 D_80011640[];
extern u8 D_80011654[];
/* Read part of a file from the development host PC: pc_load(name,
   dst, offset, len), CDREAD.C's twin of the CD read cd_load with the
   same four arguments. Its one caller is file_load (this TU, above):
   when the signed byte at +0xA of the block g_FileHostBlock points at is > 0
   it builds the host path with fmt_sprintf and calls this instead of
   cd_load, and it leaves through its epilogue with this function's
   result still in $v0.

   In order:
     1. g_XaPlayCount != 0 (the count xa_read_start raises and xa_play_stop
        clears, xa.c): VoiceStep(0) first, as the CD read
        cd_sector_load opens.
     2. fd = open(name, 1), the BIOS `open` stub. On -1:
        debug_printf(D_80011608, name) (a Shift-JIS "file not found (%s)"
        line; debug_printf is empty in this build) and
        assert_fail("PC open ERROR", "CDREAD.C", 0x2AC). assert_fail
        (exception.c) is empty in this build, so execution goes on with fd = -1.
     3. len &= 0xFFFFFF; that masked length is the return value.
     4. offset != 0: an offset that is not a multiple of 0x800 gives
        assert_fail("lseek OFFSET error", "CDREAD.C", 0x2B1); then
        lseek(fd, offset, 0), the BIOS `lseek` stub from the
        file's start. Offset 0 makes no call.
     5. While len != 0: n = len, or 0x8000 when len > 0x7FFF;
        r = read(fd, dst, n), the BIOS `read` stub. r == 0 ends
        the loop; r == -1 gives assert_fail("PC read ERROR", "CDREAD.C",
        0x2BA) and the loop goes on; then len -= n, dst += n (by the
        request, not by what was read).
     6. close(fd), the BIOS `close` stub; return the masked length.
   Strings: D_80011608 / D_80011624 / D_80011634 / D_80011640 /
   D_80011654. No RNG, no frame wait; reads g_XaPlayCount, writes no global.

   The load of g_XaPlayCount is the first instruction, ahead of the
   `addiu $sp`: the plain test gives that by itself. No marker, no forcing
   device. */
/* func_80021584 */
s32 pc_load(u8 *name, u8 *dst, s32 offset, s32 len)
{
    s32 fd;
    s32 total;
    s32 n;
    s32 r;

    if (g_XaPlayCount != 0) {
        VoiceStep(0);
    }
    fd = open(name, 1);
    if (fd == -1) {
        debug_printf(D_80011608, name);
        assert_fail(D_80011624, D_80011634, 0x2AC);
    }
    len &= 0xFFFFFF;
    total = len;
    if (offset != 0) {
        if (offset & 0x7FF) {
            assert_fail(D_80011640, D_80011634, 0x2B1);
        }
        lseek(fd, offset, 0);
    }
    while (len != 0) {
        n = len;
        if (len > 0x7FFF) {
            n = 0x8000;
        }
        r = read(fd, dst, n);
        if (r == 0) {
            break;
        }
        if (r == -1) {
            assert_fail(D_80011654, D_80011634, 0x2BA);
        }
        len -= n;
        dst += n;
    }
    close(fd);
    return total;
}
