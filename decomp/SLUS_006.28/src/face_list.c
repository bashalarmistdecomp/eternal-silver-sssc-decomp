/* Lunar SSSC — face_list: the face list: made ready, the party's set,
   loaded, its entries set, bytes moved and swapped. The first split had it
   in battle.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "game.h"
#include "chr.h"
#include "menu.h"
#include "map.h"
void FaceListByteLoad(s32 byte);

extern u8 *g_FaceList;      /* base pointer (gp-relative word) */
extern u8 g_FaceListEntries[];     /* large object -> lui/%lo addressing */
extern u8 g_FaceListEntryRows[];     /* 0xA-stride table of face entry rows, row id - 1 (far array) */

extern void FaceSlotResetCopy(void);
extern void FaceLoad(s32 face);
extern void FaceSlot6Load(s32 face_data_number);
extern void FaceListEntriesSet(void);
extern void mem_clear(void *destination, u32 byte_count);
extern void mem_move(u8 *dst, u8 *src, s32 n);

extern s32 g_PartyIds;     extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  
/* func_80075EB4 */
void FaceListInit(void) {
    g_FaceList = (u8 *)0x801DBE78;
    mem_clear(g_FaceListEntries, 0x28);
    mem_clear(g_FaceList, 8);
    FaceSlotResetCopy();
}
/* Syncs bytes 0 to 5 of the face list g_FaceList from the u16 id list at
   *g_PartyIds, counting how many actually changed.  ids >= 0xD clear the
   byte; the first 0 id ends the scan and the remaining bytes are zeroed.
   Only if something changed does it load the faces of the changed bytes
   (FaceListByteLoad); FaceListEntriesSet follows either way.
   The `>= 0xD` case must be written as the `if` (not an `else`) so it is
   the fall-through block, matching the target's branch polarity. */
/* func_80075EFC */
void FaceListPartySet(void) {
    u8 changed[8];
    s32 count;
    s32 i;

    mem_clear(changed, 8);
    count = 0;
    i = 0;
    while (i < 6) {
        if (*(u16 *)(g_PartyIds + i * 2) == 0) {
            break;
        }
        if (*(u16 *)(g_PartyIds + i * 2) >= 0xD) {
            g_FaceList[i] = 0;
        } else if (g_FaceList[i] != *(u16 *)(g_PartyIds + i * 2)) {
            count++;
            g_FaceList[i] = *(u16 *)(g_PartyIds + i * 2);
            changed[i] = 1;
        }
        i++;
    }
    while (i < 6) {
        g_FaceList[i] = 0;
        i++;
    }
    if (count != 0) {
        i = 0;
        while (i < 6) {
            if (changed[i] != 0) {
                FaceListByteLoad(i);
            }
            i++;
        }
    }
    FaceListEntriesSet();
}
/* func_8007601C */
void FaceListLoad(void) {
    s32 i = 0;
    do {
        if (g_FaceList[i] != 0) {
            FaceListByteLoad(i);
        }
        i += 1;
    } while (i < 6);
    FaceSlot6Load(g_FaceList[6]);
    FaceListEntriesSet();
}
/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its callers pass the number of a face list byte just stored, which does not say what for. */
void func_8007608C() {
}
/* func_80076094 */
void FaceListByteLoad(s32 byte) {
    FaceLoad((g_FaceList[byte] - 1) * 5 + 1);
}
/* Rebuilds the 0x28 face entries at g_FaceListEntries from the 8 bytes of the
   face list *g_FaceList.  Each non-zero byte copies a row out of the
   0xA-stride table g_FaceListEntryRows (row = id-1): bytes 0..5 and 7 take 5 entries
   at offset i*5, byte 6 is the wide one — 0xA entries at offset 0x1E (byte
   7's five, copied after it, overwrite its last five). */
/* func_800760D0 */
void FaceListEntriesSet(void) {
    s32 i;
    s32 n;
    s32 base;
    s32 j;

    mem_clear(g_FaceListEntries, 0x28);
    i = 0;
    do {
        n = 5;
        if (i != 6) {
            base = i * 5;
        } else {
            n = 0xA;
            base = 0x1E;
        }
        if (g_FaceList[i] != 0) {
            for (j = 0; j < n; j++) {
                g_FaceListEntries[base + j] = g_FaceListEntryRows[(g_FaceList[i] - 1) * 0xA + j];
            }
        }
        i++;
    } while (i < 8);
}
/* Ruling of 2026-10-14 (docs/naming.md): Returns its argument; its callers fall back to the kept face on a negative answer, which does not say what it decided. */
s16 func_800761A4(s16 face) {
    return face;
}
/* Moves one byte of the face list, with its 5 face entries, into its
   neighbour: copies byte byte's entries over those of byte byte+1
   (later != 0) or byte-1 (later == 0) (mem_move's destination is its first
   argument), moves the g_FaceList byte the same direction, then clears
   byte byte and its five entries.  The two arms are exact mirror images,
   which is why the target holds two full copies of the block.

   `other` is a CODEGEN FORCING DEVICE, not recovered logic — it is never
   read, and no instruction is emitted for it.  Every block of the target
   materialises `&g_FaceListEntries` (lui/%lo) BEFORE computing its index; with a
   single reference per block GCC evaluates the more complex operand first
   and emits the index first instead, which costs 6 instructions of
   reordering.  A second reference to the symbol in the same block makes CSE
   pull the base into its own pseudo, which is then scheduled first — the
   target's order.  The index used for `other` only has to differ from the
   live one so it is not folded into it; the address add itself is dead and
   is eliminated.  Verified: removing any one of the three drops the match
   (arms only = 44/46, merge block only = 40/46).

   Cleaner spellings all measured WORSE and were rejected: a `base` variable
   hoisted before the branch (GCC keeps it live in one register: 3
   instructions short), a stride-5 struct array (30/46), `(u8 *)((s32)
   g_FaceListEntries + n * 5)` (32/46), and computing `cur` in both arms (21/46). */
/* func_800761B0 */
void FaceListByteMove(s32 byte, s32 later) {
    u8 *src;
    u8 *cur;
    u8 *other;

    if (later != 0) {
        other = &g_FaceListEntries[(byte - 1) * 5];
        src = &g_FaceListEntries[(byte + 1) * 5];
        g_FaceList[byte + 1] = g_FaceList[byte];
    } else {
        other = &g_FaceListEntries[(byte + 1) * 5];
        src = &g_FaceListEntries[(byte - 1) * 5];
        g_FaceList[byte - 1] = g_FaceList[byte];
    }
    other = &g_FaceListEntries[(byte + 2) * 5];
    cur = &g_FaceListEntries[byte * 5];
    mem_move(src, cur, 5);
    g_FaceList[byte] = 0;
    mem_clear(cur, 5);
}
/* Swaps two bytes of the face list: exchanges their 5 face entries in
   g_FaceListEntries (via a stack scratch row) and the bytes in g_FaceList.
   `base` must be a temp: the target materializes &g_FaceListEntries once, before
   the index arithmetic, and shares it between both rows. */
/* func_80076268 */
void FaceListSwap(s32 byte_a, s32 byte_b) {
    u8 kept_entries[8];
    u8 *base;
    u8 *rowB;
    u8 *rowA;
    u8 saved;

    base = g_FaceListEntries;
    rowB = base + byte_b * 5;
    rowA = base + byte_a * 5;
    mem_move(kept_entries, rowB, 5);
    saved = g_FaceList[byte_b];
    mem_move(rowB, rowA, 5);
    g_FaceList[byte_b] = g_FaceList[byte_a];
    mem_move(rowA, kept_entries, 5);
    g_FaceList[byte_a] = saved;
}
/* Moves the id chr_id of the face list to the byte given as byte: finds it by scanning
   g_FaceList downward from 6, saves its byte and 5 face entries, moves
   every byte between there and byte one place later (FaceListByteMove(k, 1)
   from k = i - 1 down to byte), then drops the saved byte and entries into
   byte.  A not-found id (i < 0) gets the same moves from byte 4 down to
   byte, which leaves byte byte empty; nothing is put there.
   `saved` is deliberately uninitialised on the not-found path — the target
   reads $s3 there without ever writing it.

   DO NOT SIMPLIFY the duplicated shift-loop body or merge `base`/`put_base`
   into one temp.  Both are pure block-layout/register-coloring coercion
   (semantically inert — the two branches of the
   `if (g_FaceList)` are identical):
     - one shared base temp        -> 70/71 (the kept_entries-arg setup loses the
                                      guard branch's delay slot to the lui)
     - unduplicated shift loop     -> 49/71 (pos/i land in swapped $s0/$s1)
   Neither simplified form matches. */
/* func_80076328 */
void FaceListIdMove(s32 chr_id, s32 byte) {
    u8 kept_entries[8];
    u8 *base;
    s32 pos;
    s32 i;
    u8 *put_base;
    s32 doit;
    u8 saved;

    doit = 0;
    i = 6;
    while ((i >= 0) && (g_FaceList[i] != chr_id)) {
        i--;
    }
    if (byte < i) {
        base = g_FaceListEntries;
        mem_move(kept_entries, base + (i * 5), 5);
        pos = i;
        saved = g_FaceList[i];
        doit = 1;
    } else if (i < 0) {
        pos = 5;
        doit = 1;
    }
    if (doit != 0) {
        while (byte < pos) {
            if (g_FaceList) {
                pos--;
                FaceListByteMove(pos, 1);
            } else {
                pos--;
                FaceListByteMove(pos, 1);
            }
        }
        if (byte < i) {
            put_base = g_FaceListEntries;
            mem_move(put_base + (byte * 5), kept_entries, 5);
            g_FaceList[byte] = saved;
        }
    }
}
