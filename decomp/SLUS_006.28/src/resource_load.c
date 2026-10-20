/* Lunar SSSC — resource_load: a resource of LUNADATA.FIL read and sought,
   and its list of commands walked (resource_command_run, command 1 =
   resource_command01_set). The first split had it in io.c. Every function
   is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
s32 resource_load(s32, u32);
s32 inflate_buffer(s32 source, s32 destination);  
extern u8 g_ResourceLunadataFileName[];
extern u16 g_ResourceSectorPairs[];
s32 file_load(u8 *, u32, s32, s32);

/* resource_load -- the by-number resource read, (resource, dst): reads one
   whole LUNADATA.FIL resource to dst and returns its length in bytes. 21
   call sites in the image (resource_load_run, ScriptResourceLoad here; the
   battle set and party loaders, the portrait loader FaceSlotGet, the
   scripting TUs' sheet and party record reads, ...).

   g_ResourceSectorPairs is the EXE's resource sector table (docs/spec/overview.md,
   "Resource Sector Table"): u16 pairs, word resource * 2 the resource's
   first sector in the file and word resource * 2 + 1 its length in
   sectors. g_ResourceLunadataFileName is the string "LUNADATA.FIL". The one call is

       file_load("LUNADATA.FIL", dst, first << 11, sectors << 11)

   i.e. the byte offset and byte length of 2048-byte sectors, and the value
   returned is that byte length (sectors << 11), NOT file_load's result,
   which is dropped. dst goes through untouched ($a1 is never written
   here). Nothing bounds the resource number: a number past the table reads
   whatever words follow it. Entry 0 is {0, 599}, the archive's own header
   region.

   Reads: g_ResourceSectorPairs (two words). Writes: no global of its own; the read
   fills dst. Callee: file_load only. No RNG draw. No frame wait of its
   own; file_load is the blocking read (which is why the function's
   captured inputs are UNTESTABLE: the original never returns under the
   harness).

   Shapes the bytes depend on (each measured against the matching text):
   - the table is a u16 ARRAY subscripted directly, `g_ResourceSectorPairs[idx + 1]`.
     The same read as pointer arithmetic (`*(g_ResourceSectorPairs + (idx + 1))`, a
     `(u16 *)` cast of a u8 table, a pointer local) is 16/24 or worse: the
     `+ 1` is then folded into the load's offset (`lhu 2(..)`) and the two
     reads share one scaled index, where the target scales `idx + 1` and
     `idx` separately and adds the table's address to each. That is why the
     TU declares the table u16[] (resource_seek reads word resource * 2 of it).
   - the resource number is doubled IN PLACE (`idx *= 2;`). With the
     doubled number in a local of its own, or `idx * 2` written out in both
     subscripts, the doubling is merged with the second read's scaling
     (`sll 2`): 20/24.
   - `idx <<= 1`, `idx += idx`, an unsigned idx and `* 2048` for `<< 11`
     all give the same bytes; retail cannot tell them apart. */
/* func_80020230 */
s32 resource_load(s32 idx, u32 dst)
{
    s32 len;

    idx *= 2;
    len = g_ResourceSectorPairs[idx + 1] << 11;
    file_load(g_ResourceLunadataFileName, dst, g_ResourceSectorPairs[idx] << 11, len);
    return len;
}

s32 file_seek(s32 *, s32);                          /* extern */

/* func_80020290 */
void resource_seek(s32 resource) {
    file_seek((s32 *)g_ResourceLunadataFileName, g_ResourceSectorPairs[resource * 2] << 0xB);
}
extern s32 g_ResourceReadBuffer;

/* func_800202CC */
void resource_load_run(s32 resource) {
    resource_load(resource, g_ResourceReadBuffer);
    resource_command_run(g_ResourceReadBuffer);
}
void resource_tim_reset(void);  /* extern */
void resource_command01_set(s32 unused, s32 unused2, s32 payload);  /* TU defines this below */
void resource_tim_add(s32 command_word, s32 unused, s32 payload);  /* extern */
void resource_tpage_load(s32 command_word, s32 unused, s32 payload);  /* extern */
void resource_palette_load(s32 command_word, s32 unused, s32 payload);  /* extern */
void resource_palette_row_load(s32 command_word, s32 unused, s32 payload);  /* extern */
void resource_bg_set(s32 unused, s32 byte_count, s32 payload);  /* extern */
void resource_bg_attribute_set(s32 unused, s32 byte_count, s32 payload);  /* extern */
void resource_map_enter_table_set(s32 unused, s32 byte_count, s32 payload);  /* extern */
void resource_bg_second_set(s32 unused, s32 unused2, s32 payload);  /* extern */
void resource_tile_animation_set(s32 unused, s32 unused2, s32 payload);  /* extern */
s32 resource_tile_pattern_set(s32 command_word, s32 unused, s32 payload);  /* extern */
s32 sound_vab_head_open(s32, u32, void *);
s32 sound_vab_body_transfer(s32, u32, void *);
void sound_seq_add(s32, u32, void *);
s32 sound_id_table_set(s32, u32, void *);

/* func_800202FC */
long long resource_command_run(s32 *commands)
{
  s32 *command;
  s32 command_number;
  s32 command_word;
  u32 byte_count;
  u32 payload_words;
  void *payload;
  command = commands;
  resource_tim_reset();
  command_word = *command;
  if (command_word != 0)
  {
    do
    {
      byte_count = command[1];
      command_number = command_word & 0xFF;
      payload = command;
      payload = payload + 8;
      switch (command_number)
      {
        case 1:
          resource_command01_set(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 2:
          resource_tim_add(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 3:
          resource_tpage_load(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 4:
          resource_palette_load(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 5:
          resource_bg_set(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 6:
          resource_bg_attribute_set(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 9:
          resource_map_enter_table_set(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 12:
          resource_palette_row_load(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 7:
          resource_bg_second_set(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 16:
          resource_tile_animation_set(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 13:
          sound_vab_head_open(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 14:
          sound_vab_body_transfer(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 15:
          sound_seq_add(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 17:
          sound_id_table_set(command_word, byte_count, payload);
          payload_words = byte_count >> 2;
          break;

        case 18:
          resource_tile_pattern_set(command_word, byte_count, payload);

        default:
          payload_words = byte_count >> 2;
          break;

      }

      command += payload_words + 2;
      command_word = *command;
    }
    while (command_word != 0);
  }
}

extern s32 g_ResourceCommand01Buffer;

/* Command 1 of resource_command_run: unpacks its payload into the block of
   g_ResourceCommand01Buffer. The block holds several things and nothing read
   names it, so the function carries its command's number, as the block does
   (2026-10-18). */
/* func_80020480 */
void resource_command01_set(s32 unused, s32 unused2, s32 payload) {
    inflate_buffer(payload, g_ResourceCommand01Buffer);
}
/* func_800204A4 */
void resource_words_copy(s32 destination, s32 source, s32 byte_count) {
    s32 word;
    s32 write_address;
    s32 read_address;
    s32 words_left;

    write_address = destination;
    read_address = source;
    words_left = byte_count >> 2;
    if (words_left != 0) {
        do {
            word = (*(s32 *)read_address);
            read_address += 4;
            words_left -= 1;
            (*(s32 *)write_address) = word;
            write_address += 4;
        } while (words_left != 0);
    }
}
