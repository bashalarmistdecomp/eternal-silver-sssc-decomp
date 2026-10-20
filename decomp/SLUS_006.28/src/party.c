/* Lunar SSSC — party: the party's places: a number's place found, the place
   a joiner takes, two places swapped, the least level kept. The first split
   had it in scripting_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"

extern u8 g_PartyCount;
extern s32 g_MapPartyTwins;
extern s32 g_ChrRecords;
extern s32 g_PartyIds;
extern s32 g_MapPartyRecords;
extern s8 g_PartyLevelLeast;

/* func_80064C08 */
s32 PartyPlaceFind(unsigned int chr_id, u16 party_chr_id)
{
    s32 i;
    s32 count;
    u8 target;
    u16 *ptr;

    party_chr_id = chr_id;
    if (((chr_id - 0x10) & 0xFF) < 0xF)
        party_chr_id = chr_id - 0xF;
    if (g_PartyCount) {
        target = party_chr_id & 0xFF;
        count = g_PartyCount;
        i = 0;
        ptr = (u16 *)g_PartyIds;
        do {
            if (*ptr == target)
                return i;
            i++;
            ptr++;
        } while (i < count);
    }
    return -1;
}
extern u8 g_PartyRanks[];

/* PartyJoinPlaceGet -- WHERE A JOINING MEMBER GOES IN THE FIELD PARTY LIST: returns
   the slot index the member `id` is to be inserted at. The one caller is
   MoveChrPartyJoin (lns_move.c, command 0x23's insert) with its
   member id's low byte; it moves slots idx.. up by one and stores the id
   at idx. A leaf: it writes nothing, calls nothing, draws nothing from the
   RNG and waits no frame.

   Reads: g_PartyCount, the number of members in the list; the list itself,
   ((u16 *)g_PartyIds)[0..count-1], one member id per slot; and g_PartyRanks
   (.rodata, the twelve bytes in front of the twin-sheet table g_MapTwinSpriteDataNumbers):
   the PARTY-ORDER RANK of member ids 1..11, indexed id - 1 --
   01 05 08 06 07 03 04 0A 09 0B 02, then a 0 this function never reads.

   - id == 0xC: 1, whatever the list holds -- that member is always second.
   - id >= 0xD: the count -- appended at the end.
   - id below 0xC: rank = g_PartyRanks[id % 15 - 1], and the list is walked
     from slot 0 while i < count:
       * a slot holding 0xC is stepped over, except in slot 0, where the
         answer is 0 (the newcomer goes in front of it);
       * a slot holding 0xD or more ends the walk;
       * a slot whose member's rank is HIGHER than the newcomer's ends the
         walk (an equal or lower rank is stepped over);
     the answer is i: the first slot that ended the walk, or the count.
   So the list is kept sorted by rank, with 0xC pinned to slot 1 and the
   ids from 0xD up behind everyone. `% 15` changes nothing for an id below
   0xC; the instructions are in the target. Read from the code: an id of 0
   has no table byte (index 0xFF as a parameter, the byte in front of the
   table for a slot inside the count); nothing here guards against either.

   Source shapes the bytes need (each measured alone, of 58):
   - The parameter is OVERWRITTEN with its table index, `id = id % 15 - 1`,
     and the subscript is the parameter. The index written inside the
     subscript is 52 (the table's address is formed ahead of the multiply),
     a u8 local of its own for it 41.
   - `u8 id`: the unsigned divide (`multu`) is C's own narrowing of a u8
     operand's `% 15`, no cast. An s32 parameter masked by hand is 35, a
     u32 one 40.
   - `u8 rank` (s32: 54), `s32 i` (u8: 37), an s32 result (u8: 57).
   - The slot's id is read from the list at each of its three uses; one
     u16 local for it is 37 (the target loads the halfword twice).
   The loop as a do / while with a walked u16 pointer and the count in a
   local, under `if (g_PartyCount != 0)`, gives the same 58; this is the
   shorter text. There is no Japanese twin to choose between them. */
/* func_80064C70 */
s32 PartyJoinPlaceGet(u8 id)
{
    s32 i;
    u8 rank;

    if (id == 0xC) {
        return 1;
    }
    if (id >= 0xD) {
        return g_PartyCount;
    }
    id = id % 15 - 1;
    rank = g_PartyRanks[id];
    for (i = 0; i < g_PartyCount; i++) {
        if (((u16 *)g_PartyIds)[i] == 0xC) {
            if (i == 0) {
                return 0;
            }
        } else {
            if (((u16 *)g_PartyIds)[i] >= 0xD) {
                break;
            }
            if (rank < g_PartyRanks[((u16 *)g_PartyIds)[i] - 1]) {
                break;
            }
        }
    }
    return i;
}

/* PartyPlacesSwap -- swap two party slots a and b on the field: their ids in
   the per-slot id table g_PartyIds (u16 each), their 0x3C-byte party actor
   records in g_MapPartyRecords and the records' 0x3C twins in g_MapPartyTwins -- each
   pair through one 0x3C stack copy -- and then the status-strip rows and
   slot ids of the two (FaceListSwap(a, b), face_list.c). A leaf apart from
   that call: no frame wait, no RNG draw.

   Its only callers are MoveChrPartyJoin and MoveChrPartyLeave
   (lns_move.c), always as (0, 1): when a party change would leave id 0xC
   in the lead slot, slots 0 and 1 are exchanged.

   The id swap holds slot a's id in a u8: slot a gets slot b's whole u16,
   slot b gets only the LOW BYTE of slot a's old id (lbu, then sh).

   Shape: a and b are u8 parameters -- the target keeps the raw argument
   registers ($t2 / $t1) and masks them `andi 0xFF` at every use. The
   records are copied as struct assignments; the struct has to be
   word-aligned (s32 members) to get the four-word loop with a three-word
   tail. MapRecord is this function's own typedef: the TU's 0x3C record types
   (MapRecord, MapRecord) are defined further down. FaceListSwap is called
   undeclared. */
/* func_80064D58 */
void PartyPlacesSwap(u8 a, u8 b)
{
    MapRecord saved_record;
    u8 id;

    id = ((u16 *)g_PartyIds)[a];
    ((u16 *)g_PartyIds)[a] = ((u16 *)g_PartyIds)[b];
    ((u16 *)g_PartyIds)[b] = id;
    saved_record = ((MapRecord *)g_MapPartyRecords)[a];
    ((MapRecord *)g_MapPartyRecords)[a] = ((MapRecord *)g_MapPartyRecords)[b];
    ((MapRecord *)g_MapPartyRecords)[b] = saved_record;
    saved_record = ((MapRecord *)g_MapPartyTwins)[a];
    ((MapRecord *)g_MapPartyTwins)[a] = ((MapRecord *)g_MapPartyTwins)[b];
    ((MapRecord *)g_MapPartyTwins)[b] = saved_record;
    FaceListSwap(a, b);
}
/* PartyLevelLeastSet -- RECOMPUTES THE PARTY'S LOWEST LEVEL. No argument, no
   return value (no caller reads $v0).

   It walks the party slots in order, i = 0 .. g_PartyCount - 1 (the party
   count). For each slot it reads the member id from the per-slot id table
   g_PartyIds (u16 each). An id of 0xC is skipped (the id the specs call
   the guest / the shared list; the persistent table has twelve records,
   ids 0..0xB). For any other id it reads the level, byte +0x1C of that
   member's 0x40-byte persistent record g_ChrRecords + id * 0x40, and keeps
   the smallest, starting from 99 (0x63). The low byte of the result is
   stored to g_PartyLevelLeast: the lowest level in the party, or 99 when the
   party is empty or holds only id 0xC. Nothing else is written.

   A leaf: no callee, no RNG draw, no frame wait.

   Callers: MoveChrPartyJoin and MoveChrPartyLeave
   (lns_move.c), each after it has changed the party slots and right
   before StoryBit2CAUpdate; and MapBtlRun (map_btl.c), after the battle
   main loop BtlRun has returned. Readers of g_PartyLevelLeast:
   MapObjectPartySeenCheck (map_look.c, a map object's contact check) compares the
   object definition's byte +0x25 with it; menu_card.c and map_change.c carry
   it through the scene header g_GameMapRecord +0x1B.

   Source shape the bytes need:
   - A plain `for` over the slot index, with the count, the id table and
     the record table each read from its global where it is used. The loop
     pass does the rest: it hoists the two table pointers, the constant 0xC
     ($t1) and the count (a copy of the entry test's byte, $t0) out of the
     loop and turns the indexed id read into the walked pointer $a1.
   - What decides the registers is the entry test. In a `for` it is
     `i < count` with i still a variable, so i has eight references when
     the allocator ranks it (8 over 18 instructions, priority 1.333) and
     goes ahead of the walked pointer (7 over 13, 1.077): i in $a0, the
     pointer in $a1. Guarded by `if (g_PartyCount != 0)` with a do / while,
     i has seven (0.778), the pointer takes $a0, and the base and the count
     swap as well: 19/28. `i = 0; if (i < g_PartyCount) do { } while (...)`
     and `while` are the same bytes as the `for`.
   - The id is held in a local (re-read from the table at its second use:
     22/28). i and min are ints (min as u8: 27, i as u8: 15, i as u32:
     27). How the level read is spelled does not matter (five spellings,
     all 28).

   The Japanese build's twin (0x80068520) is this function word for word.
   Under the `cc272` marker (stock GCC 2.7.2) this text is 27/28 against
   it, with two extra words: `blez` for the entry test and an 8-byte frame
   that holds nothing (a scratch pseudo 2.7.2's combine makes for that
   test and never uses). The guarded do / while has neither under 2.7.2
   but has the $a0 / $a1 swap there too (22/28), so no text tried gives the
   twin under stock 2.7.2. Compiled by hand with the 1997-03-18 snapshot
   (gcc-970318-psx) the `for` gives the twin's 28 words in order, `beqz`
   included, inside the 16-byte frame that vendored build gives every
   leaf. */
/* func_80065028 */
void PartyLevelLeastSet(void)
{
    s32 i;
    s32 min;
    u16 id;
    u8 lv;

    min = 0x63;
    for (i = 0; i < g_PartyCount; i++) {
        id = ((u16 *)g_PartyIds)[i];
        if (id != 0xC) {
            lv = ((u8 *)g_ChrRecords)[id * 0x40 + 0x1C];
            if (lv < min) {
                min = lv;
            }
        }
    }
    g_PartyLevelLeast = min;
}
