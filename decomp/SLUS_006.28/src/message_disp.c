/* Lunar SSSC — message_disp: the message: its records and arrow, open and
   close, its strings, codes, lines and controls written, and its faces. The
   first split had it in scripting_c.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_MenuPage */

#include "include_asm.h"
#include "psyq_types.h"
#include "script.h"
#include "pad.h"
#include "game.h"
#include "chr.h"
#include "map.h"
#include "sprite.h"

extern s32 g_MessageOpenAllowed;
extern long HWD0;
extern long VWD0;
extern s32 g_MessageCursorEntry;
extern s32 g_MessageCursorPiece;
extern s32 g_ScriptEnded;
s32 MessageShadeSet(s32);                               /* extern */
extern s32 g_MenuPage;
extern u8 g_MessagePressWait;
void MessageOpenClear(void);
extern s32 g_MessageOpen;
extern u8 g_MessageSpeed;
extern s32 g_ScriptCommandPointer;
s32 ScriptPointerAdvance(u8);  /* u8: the callee masks its argument on entry (andi 0xFF); every caller passes 2 or 4 */
extern u8 g_FontStringCodes[];
extern long HWD0, VWD0;		 
s32 debug_printf();
extern u8 g_FaceListEntries[];
void MessageRowDisp(s32 row);
extern void *g_MessageArrowRecord;
extern u16 g_MessageArrowX;
s32 ScriptFrameStep(s32);
extern long HWD0, VWD0;
/* far_syms: g_MapBgChange[0] (target reaches it %hi/%lo) */
extern u16 g_MessageFaceFirst;
extern u16 g_MessageFaceSecond;
extern u16 g_MessageFacePlaces;
extern u8 g_MessagePadRecord[];
s32 pad_record_init(s32 *, s32, s32, s32);
s32 font_target_clear(s32, s32);
extern u16 g_MessageLine;
extern u16 g_MessageFaceFirst;
extern u16 g_MessageFaceSecond;
extern u16 g_MessageFacePlaces;
s32 MessageRecordsInit();
void MessageClose(void);
s32 MessageStringDisp(s32 *, s32);
extern u16 g_MessageRow;
extern u8 g_MessageTimed;
s32 ScriptFrameStep(s32);
s32 MapStatusClear();
signed char *font_string_word_get(int word_number);  /* extern */
void ScriptMoveEndWait(void);  /* extern */

extern u16 g_SpriteData0[];
s32 mem_clear(s32, s32);
void MapAnimationSet(void *record, u16 animation);

/* MessageRecordsInit() -- build the message window's three display records from
   nothing. Callers: the event runner ScriptLabelRun (on every call), the
   field message MessageStringDisp (only while g_ScriptEnded != 0) and the script
   command ScriptCommandShopRun (after ShopRun). No arguments. In order:
     1. The three pointers are set to fixed addresses, the records back to
        back in 0x78 bytes: g_MessageCursorPiece = 0x801C9B00 (a 0x18-byte slice),
        g_MessageCursorEntry = 0x801C9B18 (a 0x24-byte render entry), g_MessageArrowRecord =
        0x801C9B3C (a 0x3C-byte anim actor).
     2. mem_clear(0x801C9B00, 0x78) zeroes all three.
     3. The slice: kind 0x4000 (a TILE), colour index 0x7B1. Its points
        stay 0 here; MessageCursorSet writes them (and the colour again).
     4. The entry (the prompts' cursor): on = 1, +0xA = 0x6000, +0x14 =
        the slice's address. x and y stay 0; the prompts place it.
     5. The actor: its entry's on (+4) = 1 and +0xE = 0x6360; then
        g_MessageArrowX = 0x128, the x the "more" arrow is drawn at
        (MessageArrowStep: MessageArrowDraw(g_MessageArrowX, 0xD0) while the actor's
        on is set; MessageRowDisp sets 0xE0, 0xE8 or 0x128 by the bits of
        g_MessageFacePlaces); then five addresses into the anim bank g_SpriteData0, whose
        head (SpriteDataHead) holds offsets in 4-byte units from the bank's start:
        +0x10 = bank + 4 * texel_records, +0x14 = bank + 4 * pose_table,
        +0x18 = bank + 4 * piece_table, +0x28 = bank + 4 * animation_table,
        +0x2C = bank + 4 * step_list; and +0x32 (the previous anim id) = 0xFFFF.
     6. MapAnimationSet(actor, 5): play anim 5. The record was just cleared,
        so its anim id is 0: MapAnimationSet stores that 0 over the 0xFFFF of
        step 5 (+0x32), sets +0x30 = 5 and restarts the animation
        (MapAnimationStart(actor, 1): +0x38 / +0x3A from the anim table's entry
        5, +0x34 = 0, +0x36 from the step table).
   No frame wait and no RNG anywhere under it: mem_clear is a leaf
   clear, MapAnimationSet calls only MapAnimationStart, a leaf. The epilogue sets
   no $v0 (the TU's forward declarations say s32, kept); ScriptCommandShopRun
   ends on this call and its own callers were not checked for reading one.

   Matching.
   - The stores are struct-member stores. A store through a member of a
     pointed-to struct does not conflict with a read of a scalar at a fixed
     address, so the three pointer globals are each loaded once, the
     bank's word 0 is loaded early (it fills the load delay of
     texel_records) and its sum is stored late (in piece_table's); that leaves the
     scheduler something to put in every load delay, and the second
     call's `li $a1, 5` sinks to the top of the block, which is what keeps
     $a1 away from the slice pointer. As `*(s16 *)(p + n) = ...` through
     three pointer locals: 49/62; the same casts on the globals: 46/62.
     Pointer locals of the struct types match too; the globals are
     spelled at each use as the prompts below do.
   - on before +0xA in the entry (57/62 the other way): the two `on`
     stores share one register holding 1, and with the 0x6000 store
     between its first use and its last that register cannot be $v0,
     where the short constants go -- it is $a3, as in the target, and the
     bank's %hi then reuses $a3 after it.
   - The bank's five words are pointer accesses through their members'
     addresses (`*(u16 *)&bank->texel_records`), as in MenuRecordsInit: as
     member loads, or AS(u16, ...), the function grows by 0x10 bytes.
   - The bank's word 0 is read through the pointer local like the other
     four (`*(u16 *)&bank->animation_table`): after cse it is a plain load
     at a fixed address, which the member stores do not pin. Read as the
     array's own element
     (`*g_SpriteData0`, `g_SpriteData0[0]`) it is an aggregate reference, stays
     behind the +0x18 store and the delay slots go to the call's two
     argument loads instead (47/62).
   - The statement order of the five sums is +0x10, +0x14, +0x18, +0x28,
     +0x2C and then +0x32, the order MenuUseShowPlacesPlay and MenuRecordsInit fill
     the same fields in; the target's interleaving is the scheduler's.
     +0x28 before +0x18: 58/62; +0x32 before +0x2C: 37/62.
   - `u16 prev`: the target loads 0xFFFF with ori (s16: addiu -1, 61/62). */
/* func_8005A580 */
s32 MessageRecordsInit(void)
{
    SpriteDataHead *bank;

    g_MessageCursorPiece = 0x801C9B00;
    g_MessageCursorEntry = 0x801C9B18;
    g_MessageArrowRecord = (void *)0x801C9B3C;
    mem_clear(0x801C9B00, 0x78);
    AS(s16, ((SpritePiece *)g_MessageCursorPiece)->kind) = 0x4000;
    AS(s16, ((SpritePiece *)g_MessageCursorPiece)->texel_col.col) = 0x7B1;
    AS(s16, ((SpriteEntry *)g_MessageCursorEntry)->on) = 1;
    ((SpriteEntry *)g_MessageCursorEntry)->flags = 0x6000;
    ((SpriteEntry *)g_MessageCursorEntry)->piece_table = g_MessageCursorPiece;
    AS(s16, ((MapRecord *)g_MessageArrowRecord)->sprite.on) = 1;
    ((MapRecord *)g_MessageArrowRecord)->sprite.flags = 0x6360;
    g_MessageArrowX = 0x128;
    bank = (SpriteDataHead *)g_SpriteData0;
    ((MapRecord *)g_MessageArrowRecord)->sprite.texel_records = *(u16 *)&bank->texel_records * 4 + (s32)bank;
    ((MapRecord *)g_MessageArrowRecord)->sprite.pose_table = *(u16 *)&bank->pose_table * 4 + (s32)bank;
    ((MapRecord *)g_MessageArrowRecord)->sprite.piece_table = *(u16 *)&bank->piece_table * 4 + (s32)bank;
    ((MapRecord *)g_MessageArrowRecord)->animation_table = *(u16 *)&bank->animation_table * 4 + (s32)bank;
    ((MapRecord *)g_MessageArrowRecord)->step_list = *(u16 *)&bank->step_list * 4 + (s32)bank;
    ((MapRecord *)g_MessageArrowRecord)->animation_before = 0xFFFF;
    MapAnimationSet(g_MessageArrowRecord, 5);
}

extern u16 g_MapViewX;
extern u16 g_MapViewY;

/* func_8005A678 */
void MessageArrowStep(void)
{
  *((s16 *) (((u8 *) g_MessageArrowRecord) + (0x6 ^ 0))) = (s16) (g_MessageArrowX + g_MapViewX);
  ((MapRecord *)g_MessageArrowRecord)->sprite.y = (s16) (g_MapViewY + 0xD0);
  if (((unsigned short)(AS(s32, ((MapRecord *)g_MessageArrowRecord)->sprite.on))) != 0)
  {
    MessageArrowDraw(g_MessageArrowX, 0xD0);
  }
}

/* func_8005A6C4 */
void MessageOpenClear(void) {
    g_MessageOpen = 0;
}

/* func_8005A6D0 */
void MessageOpen(void)
{
  int end_height;
  s32 height;
  if (g_MessageOpen == 0)
  {
    g_MessageOpen = 1;
    frame_start();
    height = 1;
    do
    {
      MessageShadeSet(height);
      ScriptFrameStep(1);
      height += 1;
    }
    while (height < (end_height = 9));
    ScriptFrameStep(1);
  }
}

/* far_syms: g_MenuPage (target reaches it %hi/%lo) */

u16 MessageFaceEntrySet(s32, s32, u16, s32, s32);               /* extern */
void MessageLinesClear(void);  /* extern */
extern s32 D_800B8940;

/* func_8005A72C */
void MessageClose(void)
{
  s32 height;
  if ((g_MessageOpen != 0) || (g_MenuPage != 0))
  {
    MessageLinesClear();
    g_MessageFaceFirst = MessageFaceEntrySet(0, 0x10, g_MessageFaceFirst, 0, 1);
    if (g_MenuPage == 0)
    {
      height = 7;
      do
      {
        MessageShadeSet(height);
        ScriptFrameStep(1);
        height -= 1;
      }
      while (height >= 0);
      ScriptFrameStep(1);
      g_MessageOpen = 0;
      D_800B8940 = 0;
    }
  }
}

s32 MapStatusDisp();                                  /* extern */
extern u16 g_MapStatusRequest;
extern u16 g_MapIdleCount;

/* func_8005A7C8 */
s32 MapStatusClear(void) {
    s32 request_then_result;

    if (((u16) g_MapIdleCount >= 0x12CU) || (request_then_result = g_MapStatusRequest, (request_then_result != 0))) {
        g_MapIdleCount = 0;
        g_MapStatusRequest = 2;
        MapStatusDisp();
        request_then_result = ScriptFrameStep(1);
    }
    return (s32) request_then_result;
}

/* Ruling of 2026-10-14 (docs/naming.md): Fills three bytes of its own stack and returns; nothing reaches it, and nothing uses what it makes. */
s32 func_8005A818(s32 code) {
    volatile u8 buf[3];
    buf[0] = code >> 8;
    buf[1] = code;
    buf[2] = 0;
    return code >> 8;
}

s32 font_code_put(s32, s32);                            /* extern */
void MessageLineDisp(s32);                                /* extern */

/* func_8005A834 */
void MessageStringCodePut(s32 code) {
    font_code_put(code & 0xFFFF, 1);
    MessageLineDisp(g_MessageSpeed);
}

void MessageControlRun(s32);                               /* extern */

/* func_8005A864 */
void MessageStringControlRun(s32 code) {
    MessageControlRun(code & 0xFFFF);
}

/* far_syms: g_FontStringCodes */

/* The text decoder (docs/spec/text.md, "font_string_walk"): walks the
   byte stream at p until 0xFF. A 0x0E byte opens a run, ended by 0x00:
   bytes below 0x5C are characters (b + 0x1F), bytes from 0x5C up index the
   fragment dictionary (font_string_word_get; 0xFD/0xFE extend the index). Six
   characters are controls: '%' newline, '@' pause, '\\' close (inside a
   fragment it also ends the message), '$' newline + close and the end of
   the message, '*' newline + wait, '^' wait. Any other byte is a 16-bit
   code (g_FontStringCodes[b] below 0xD6, else b and the next byte): above 0xEFFF
   it goes to emit_ctl, otherwise to emit_char (0xD640..0xEFFF less
   0x5500). Returns a pointer to the 0xFF it stopped on, or to the run
   byte that ended the message (the '$', or the last byte of the fragment
   index whose text ended it).

   Shape notes (each read off the target's registers): c is the raw byte,
   k its unsigned copy for the 0x0E/0xD6 tests and the table index, and
   one u16 `code` carries every derived value (fragment index, run
   character, table/two-byte code). The run and fragment loops are goto
   loops: as structured loops their references weigh more and frag
   outranks p for $s0. code = c + 0x1F is formed before the >= 0x5C test,
   so the direct switch's zero-extension sits after a label and is kept. */
/* func_8005A884 */
u8 *font_string_walk(u8 *p, void (*emit_char)(u16), void (*emit_ctl)(u16))
{
    u8 *frag;
    u16 c;
    u32 k;
    u16 code;

    while ((c = *p) != 0xFF) {
        k = c;
        if (k == 0xE) {
            p++;
        run:
            if (*p == 0) goto run_end;
            c = *p++;
            code = c + 0x1F;
            if (c >= 0x5C) {
                if (c == 0xFD) {
                    code = *p++;
                    code += 0xA1;
                } else if (c == 0xFE) {
                    code = *p++;
                    code += 0x19E;
                } else {
                    code = c - 0x5C;
                }
                frag = (u8 *)font_string_word_get(code);
                if (*frag != 0) {
                frag_next:
                    code = *frag++;
                    switch (code) {
                    case 0x25:
                        emit_ctl(0xFF00);
                        break;
                    case 0x40:
                        emit_ctl(0xFF02);
                        break;
                    case 0x5C:
                        emit_ctl(0xFF03);
                        return p - 1;
                    case 0x24:
                        emit_ctl(0xFF00);
                        emit_ctl(0xFF03);
                        return p - 1;
                    case 0x2A:
                        emit_ctl(0xFF00);
                        emit_ctl(0xFF01);
                        break;
                    case 0x5E:
                        emit_ctl(0xFF01);
                        break;
                    default:
                        emit_char(code);
                        break;
                    }
                    if (*frag != 0) goto frag_next;
                }
            } else {
                switch (code) {
                case 0x25:
                    emit_ctl(0xFF00);
                    break;
                case 0x40:
                    emit_ctl(0xFF02);
                    break;
                case 0x5C:
                    emit_ctl(0xFF03);
                    break;
                case 0x24:
                    emit_ctl(0xFF00);
                    emit_ctl(0xFF03);
                    return p - 1;
                case 0x2A:
                    emit_ctl(0xFF00);
                    emit_ctl(0xFF01);
                    break;
                case 0x5E:
                    emit_ctl(0xFF01);
                    break;
                default:
                    emit_char(code);
                    break;
                }
            }
            goto run;
        run_end: ;
        } else {
            if (k < 0xD6) {
                code = ((u16 *)g_FontStringCodes)[k];
            } else {
                code = (c << 8) | p[1];
                p++;
            }
            if (code <= 0xEFFF) {
                if (code > 0xD63F) {
                    code -= 0x5500;
                }
                emit_char(code);
            } else {
                emit_ctl(code);
            }
        }
        p++;
    }
    return p;
}

extern u8 g_MessageTimed;

/* func_8005AB1C */
void ScriptCommandMessageDisp(void)
{
  s32 next_command;
  if (g_MessageTimed == 0)
  {
    ScriptMoveEndWait();
  }
  MapStatusClear();
  if (g_MessageOpenAllowed != 0)
  {
    MessageOpen();
  }
  next_command = font_string_walk(ScriptPointerAdvance(2), &MessageStringCodePut, &MessageStringControlRun);
  next_command = next_command + 1;
  if (next_command & 1)
  {
    next_command += 1;
  }
  g_ScriptCommandPointer = next_command;
}

void hex_dump();

/* func_8005AB98 */
s32 MessageStringDisp(s32 *string, s32 open_allowed)
{
  s32 old_command;
  u8 old_speed;
  u8 old_press_wait;
  hex_dump(string, 0x10);
  old_command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = (s32) (((u8 *) string) - 2);
  old_speed = g_MessageSpeed;
  old_press_wait = g_MessagePressWait;
  g_MessageRow = 0x14;
  g_MessageLine = 0;
  font_target_clear(0x801CBB00, 0x55);
  g_MessageTimed = 0;
  g_MessageOpenAllowed = open_allowed;
  if (g_ScriptEnded != 0)
  {
    MessageRecordsInit();
  }
  ScriptCommandMessageDisp();
  g_ScriptCommandPointer = old_command;
  g_MessageSpeed = old_speed;
  g_MessageOpenAllowed = 1;
  g_MessagePressWait = old_press_wait;
}

/* far_syms: D_8001293C (target reaches it %hi/%lo) */
/* far_syms: g_FontStringCodes (target reaches it %hi/%lo) */
s32 MessageDisp(s32 message, s32 open_allowed);  /* TU defines this below */
void CellAllUpdate();  /* TU defines this below */
void func_8005B2E8();  /* TU defines this below */
s32 MessageFaceSet(s32, s32, s32, s32, s32);
s32 func_800761A4(s32);
s32 MessageFaceDisp(s32 face, s32 places, s32 old_face, s32 unused);  /* TU defines this below */
s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
s32 CellRowsClear(s32, s32, s32);
s32 mem_clear(s32, s32);
s32 MessageRecordsInit();
extern u8 D_8001293C[];

/* func_8005AC44 */
void MessageCodesConv(u8 *string, u16 *message)
{
  u8 *out;
  u16 *in;
  u16 *table;
  s32 code;
  s32 high;
  s32 low;
  s32 i;
  in = message;
  out = string;
  if ((*in) != 0xFFFF)
  {
    table = (u16 *) g_FontStringCodes;
    while ((*in) != 0xFFFF)
    {
      code = *in;
      in++;
      high = code >> 8;
      low = code & 0xFF;
      if (code < 0xA)
      {
        code += 0x824F;
        goto lookup;
      }
      if (code > 0xEFFF)
      {
        goto lookup;
      }
      debug_printf(D_8001293C, code);
      lookup:
      i = 1;

      while (i < 8)
      {
        if (code == table[i])
        {
          *(out++) = i;
          break;
        }
        i++;
      }

      if (i == 8)
      {
        *(out++) = high;
        *(out++) = low;
      }
    }

  }
  *out = 0xFF;
}

/* func_8005AD50 */
s32 MessageDisp(s32 message, s32 open_allowed) {
    s32 local[0x10];
    MessageCodesConv(local, message);
    return MessageStringDisp(local, open_allowed);
}

/* Empty. Takes an argument it ignores — menu_list.c/menu_nowalk.c call it
   with one, and an empty body cannot reveal that from the asm alone. */
/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; named from what its callers pass: the kinds of cell they have just changed. */
/* func_8005AD8C */
void CellAllUpdate() {

}
/* MessageLineDisp — shows the field message window's current text row
   and waits: MessageLineDisp(n), called
   by the window's glyph callback with the text speed (g_MessageSpeed), by its
   control handler MessageControlRun with 1, and with -1 to draw without a
   wait. n == 0 does nothing at all (a text speed of 0 draws nothing per
   glyph). Otherwise, on the row counter g_MessageLine:
     rows 0..2  the window's own three rows: blank cell-grid rows 0..5
         (CellRowsClear(0, 6, 0x20) = ankprint_area_clear(0, 0, 0x28, 6, 0x20)),
         then MessageRowDisp(g_MessageRow) — re-place the window's label
         strip at its grid row g_MessageRow (which also waits one frame).
     rows 3..6  the four extra one-line rows: blank grid rows 0x16..0x1B,
         then font_image_load(0x801CBB00 + row * 0x880, row * 0x10, 0x10)
         LoadImages the row's 16 cells (16x16 4bpp, 0x80 bytes each) from
         its buffer to label strip `row` of g_FontLines, cell 0, and
         font_cell_set(x, y, row, 0, 0x10) writes the grid cell that shows
         that strip's 16 cells, at grid (3, 2) for row 3, (3, 4) for row 4,
         (8, 4) for row 5 and (0x11, 4) for row 6.
     any other row  only the blanking of grid rows 0x16..0x1B.
   Then n field frames, ScriptFrameStep(1) each; n < 0 waits none. No RNG.
   The row is read again for the switch (a u16 global, one lhu each use);
   the four call pairs are written out per case — cross-jumping merges
   only the font_cell_set tails, and the compare tree, not a jump table,
   is what a four-case switch gives. Callees with no declaration above
   this point in the TU are called undeclared. */
/* func_8005AD94 */
void MessageLineDisp(s32 frame_count)
{
    s32 i;

    if (frame_count != 0) {
        if (g_MessageLine < 3) {
            CellRowsClear(0, 6, 0x20);
            MessageRowDisp(g_MessageRow);
        } else {
            CellRowsClear(0x16, 6, 0x20);
            switch (g_MessageLine) {
            case 3:
                font_image_load(0x801CD480, 0x30, 0x10);
                font_cell_set(3, 2, 3, 0, 0x10);
                break;
            case 4:
                font_image_load(0x801CDD00, 0x40, 0x10);
                font_cell_set(3, 4, 4, 0, 0x10);
                break;
            case 5:
                font_image_load(0x801CE580, 0x50, 0x10);
                font_cell_set(8, 4, 5, 0, 0x10);
                break;
            case 6:
                font_image_load(0x801CEE00, 0x60, 0x10);
                font_cell_set(0x11, 4, 6, 0, 0x10);
                break;
            }
        }
        for (i = 0; i < frame_count; i++) {
            ScriptFrameStep(1);
        }
    }
}

/* far_syms: g_FontStringTable */
extern s32 g_FontStringTable;
extern u16 D_800B8E34;
s32 pad_record_update(u8 *, s32);
void font_target_set(u8 *target);
void font_col_set(s32);
void font_pen_move(s32);

/* MessageControlRun — a field message window's control-code handler:
   font_string_walk's emit_ctl callback
   (through MessageStringControlRun, which passes code & 0xFFFF). The field twin of
   the battle popup's BtlMessageControlRun (btl_message.c), dispatched the same way on
   code & 0xFF00:
     0xF0xx / 0xF1xx / 0xF4xx / 0xF5xx / 0xF6xx  splice string code & 0xFFF
         of the offset table at g_FontStringTable through font_string_walk
         with the same two callbacks (so a spliced string may nest). A case
         0xF080 is present; it can never equal a 0xFF00-masked value.
     0xF8nn  wait nn frames (ScriptFrameStep(1) each).
     0xF9nn  font_pen_move(nn) — advance the pen nn pixels.
     0xFAnn  g_MessageFaceFirst = MessageFaceDisp(nn, 0x10, g_MessageFaceFirst, 0).
     0xFBnn  g_MessageFaceSecond = MessageFaceDisp(nn, 0x20, g_MessageFaceSecond, 0x8000).
     0xFCnn  font_col_set((nn & 3) << 2).
     0xFF00  the page prompt: start the pad record g_MessagePadRecord
             (pad_record_init(rec, 0x14, 0xD0, 0)) and loop a frame at a
             time (ScriptFrameStep(1), pad_record_update(rec, 0x14),
             MessageArrowStep()) until rec+4 has bit 0x40. With g_MessageTimed
             set the pad's rec+4 is zeroed each frame (input ignored);
             a new press of bit 4 (rec+2) counts in D_800B8E34. With
             g_MessagePressWait == 0 the wait also ends by itself after
             (g_MessageLine + 1) * 60 + 40 frames.
     0xFF01  new page: row counter g_MessageLine = 0, clear 0x1980 bytes at
             0x801CBB00 (mem_clear), MessageLineDisp(1) for rows 0..2,
             g_MessageLine = 0, font_target_set(0x801CBB00).
     0xFF02  next row: ++g_MessageLine < 3 -> font_target_set(0x801CBB00 +
             g_MessageLine * 0x880); the third advance falls into 0xFF01.
     0xFF03  MessageClose() — close the window.
   0xFF00, 0xFF02 and 0xF8nn first call MessageLineDisp(1) when the text
   speed g_MessageSpeed is 0 (the per-glyph MessageLineDisp(g_MessageSpeed) drew
   nothing). Anything else: nothing. No RNG.
   Written as the twin is: nested switch, the row loop as a `for` on the
   global, the 0xF8 bound as `code & 0xFF` in the loop test (loop.c hoists
   it and cse2 turns the hoisted copy into `move s1,v0` of the entry
   test's value). reorg, not the source, gives that loop its
   `beqz exit; j top` bottom: an NE-tested back branch followed by an
   unconditional jump is swapped (relax_delay_slots, mostly_true_jump > 0),
   the EQ-tested bottom of the 0xFF00 loop is not. */
/* func_8005AF20 */
void MessageControlRun(s32 code)
{
    u16 i;
    u8 *p;

    switch (code & 0xFF00) {
    case 0xFF00:
        switch (code) {
        case 0xFF02:
            if (g_MessageSpeed == 0) {
                MessageLineDisp(1);
            }
            if (++g_MessageLine < 3) {
                font_target_set((u8 *)0x801CBB00 + g_MessageLine * 0x880);
                break;
            }
        case 0xFF01:
            g_MessageLine = 0;
            mem_clear(0x801CBB00, 0x1980);
            for (; g_MessageLine < 3; g_MessageLine++) {
                MessageLineDisp(1);
            }
            g_MessageLine = 0;
            font_target_set((u8 *)0x801CBB00);
            break;
        case 0xFF00:
            if (g_MessageSpeed == 0) {
                MessageLineDisp(1);
            }
            p = g_MessagePadRecord;
            pad_record_init((s32 *)p, 0x14, 0xD0, 0);
            i = 0;
            while (!(((PadRecord *)p)->repeated & 0x40)) {
                if (g_MessagePressWait == 0 && i >= (g_MessageLine + 1) * 60 + 0x28) {
                    break;
                }
                ScriptFrameStep(1);
                pad_record_update(p, 0x14);
                if (g_MessageTimed != 0) {
                    ((PadRecord *)p)->repeated = 0;
                } else {
                    func_8005B2E8();
                }
                i++;
                MessageArrowStep();
                if (((PadRecord *)p)->pressed & 4) {
                    D_800B8E34++;
                }
            }
            break;
        case 0xFF03:
            MessageClose();
            break;
        }
        break;
    case 0xF900:
        font_pen_move(code & 0xFF);
        break;
    case 0xFC00:
        font_col_set((code & 3) << 2);
        break;
    case 0xF800:
        if (g_MessageSpeed == 0) {
            MessageLineDisp(1);
        }
        for (i = 0; i < (code & 0xFF); i++) {
            ScriptFrameStep(1);
        }
        break;
    case 0xFA00:
        g_MessageFaceFirst = MessageFaceDisp(code & 0xFF, 0x10, g_MessageFaceFirst, 0);
        break;
    case 0xFB00:
        g_MessageFaceSecond = MessageFaceDisp(code & 0xFF, 0x20, g_MessageFaceSecond, 0x8000);
        break;
    case 0xF000:
    case 0xF080:
    case 0xF100:
    case 0xF400:
    case 0xF500:
    case 0xF600:
        font_string_walk((u8 *)(g_FontStringTable + *(u16 *)((code & 0xFFF) * 2 + g_FontStringTable)), MessageStringCodePut, MessageStringControlRun);
        break;
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its one call, in a wait for a press, passes nothing and does not say what for. */
void func_8005B2E8() {

}

s32 MessageFaceSet(s32, s32, s32, s32, s32);         /* extern */
s32 func_800761A4(s32);                             /* extern */

/* func_8005B2F0 */
s32 MessageFaceDisp(s32 face, s32 places, s32 old_face, s32 unused) {
    s32 shown_face;

    shown_face = func_800761A4(face & 0xFFFF);
    if ((s16) shown_face < 0) {
        shown_face = func_800761A4(old_face & 0xFFFF);
    }
    return MessageFaceSet(shown_face & 0xFFFF, places & 0xFFFF, old_face & 0xFFFF, unused & 0xFFFF, 1) & 0xFFFF;
}

void CellFacePlacesSet(s32, s32, s32);

/* MessageFaceSet -- SHOW A FACE IN A SET OF FACE SLOTS, or take every shown
   face down, and keep g_MessageFacePlaces, the slot bits that are up. ("Face" is the
   tree's reading of CellFacePlacesSet -> FaceCellSet, the portrait loader
   of menu_run.c's, menu_page.c's and menu-ui.md's notes; FaceCellSet is still asm and was
   not read for this conversion.)

   face  the face to show, 16 bits; 0 = none (face 0 goes to every slot
         that is up)
   places  the slots to show it in, one bit each. CellFacePlacesSet (cell_face.c)
         turns each bit into the pair it hands FaceCellSet with the
         face: 1 -> (3, 0x12), 2 -> (0x1C, 0x12), 0x10 -> (1, 0x12),
         0x20 -> (0x1F, 0x12), 4 -> (6, 5), 8 -> (4, 0xB)
   old_face  what to return when face is 0: the caller's own record of the
         face it had up
   unused  NOT READ. The callers pass 0 or 0x8000
   show_lines  non-zero: finish with MessageRowDisp(g_MessageRow)

   face != 0:
     - when a slot OUTSIDE places is showing (g_MessageFacePlaces & ~places):
       CellFacePlacesSet(places, 0xDEAD0001, 0), then g_MessageFacePlaces = 0. The call
       is given the NEW slots with face 0, not the slots that were
       showing: a slot outside places drops out of g_MessageFacePlaces without
       being given face 0 here;
     - CellFacePlacesSet(places, 0xDEAD0001, face): the face into every slot of
       places; then g_MessageFacePlaces = places.
   face == 0: CellFacePlacesSet(g_MessageFacePlaces, 0xDEAD0001, 0) gives face 0 to
     every slot showing, g_MessageFacePlaces = 0, and the result becomes old_face.
   Then, with show_lines set, MessageRowDisp(g_MessageRow): the text window's
   label strip is placed again at its grid row g_MessageRow, by the bits of
   g_MessageFacePlaces as this call has just left them.
   Returns face, or old_face when face was 0, as 16 bits.

   Callers: MessageFaceDisp (text commands 0xFAnn / 0xFBnn: slots 0x10 /
   0x20, show_lines = 1) and MessageFaceEntrySet (the menus' face, a record number
   turned into a face through g_FaceListEntries; show_lines from its caller) in this
   TU, ShopFaceSet in shop_face.c (show_lines = 0). The first and the third
   return the result, and their callers store it in the face variable
   that went in as old_face (g_MessageFaceFirst / g_MessageFaceSecond); MessageFaceEntrySet drops
   it and returns its own record number.

   CellFacePlacesSet does not read its second argument (cell_face.c), so
   0xDEAD0001 is a filler. Reads g_MessageFacePlaces and g_MessageRow, writes
   g_MessageFacePlaces. No RNG draw here. Frame waits: MessageRowDisp ends in
   ScriptFrameStep(1), one field frame, when show_lines is set; what
   CellFacePlacesSet's callee FaceCellSet (ui, asm) waits for was not
   checked.

   Source shapes the bytes need:
   - face, places and old_face are 16-BIT PARAMETERS. This compiler keeps a
     narrow parameter in a 16-bit pseudo copied from the incoming
     register and zero-extends it at each use: that is the `andi` at
     each call rather than once on entry, and the second register for
     places ($s0 = $a1 is the incoming word, which the `nor` and the `sh`
     read; $s2 = $s0 the 16-bit variable the two calls extend). The
     incoming copies are compiler temporaries, not parameter registers
     with a stack home, which is what ranks places's above face ($s0, $s1)
     and old_face's above the extended face ($s3, $s4): local-alloc.c
     doubles the live length of a register that has a REG_EQUIV home,
     so an s32 places ranks 4 refs / 42 where the temporary ranks 4 / 21.
     s32 parameters narrowed into u16 locals give the same instructions
     in other registers, 37/55 (41/55 with only places so).
   - the stale-slot test is taken at 16 bits. At int width, `if
     (g_MessageFacePlaces & ~places)`, places is zero-extended once ahead of the
     `nor`, both calls share that register and the frame loses $s5:
     35/55.

   MARKED SPELLING: an old-style definition. The bytes prove 16-bit
   parameters, and this TU (twice) and shop_face.c declare the function
   (s32, s32, s32, s32, s32); an old-style definition is compatible with
   those prototypes and leaves the three callers' bytes alone. The
   prototype form `s32 MessageFaceSet(u16 face, u16 places, u16 old_face, u16
   unused, s32 show_lines)` is the same 55/55, and with this TU's two
   declarations changed to (u16, u16, u16, u16, s32) MessageFaceDisp and
   MessageFaceEntrySet keep every row -- but shop_face.c's ShopFaceSet
   SIGN-extends its first argument (sll / sra), which that prototype
   turns into an `andi` (33/36), so no one prototype with a u16 face
   fits the three callers and the old-style form may be the original's.
   unused's type is the callers' (they pass 16 bits); nothing in the body
   reads it. The return type is the prototypes'; u16 gives the same
   bytes.

   MARKED SPELLING: the u16 local `stale`. Something in the source made
   the AND / NOT 16 bits wide; `g_MessageFacePlaces & (u16)~places` and
   `(u16)(g_MessageFacePlaces & ~places)` in the `if` give the same 55/55 and which
   of the three it was is not recoverable. Without any of them: 35/55.

   Japanese build (SLPS_013.97, twin 0x8005E6E4): its instructions are
   retail's, register for register. Under the `cc272` stand-in every
   spelling above scores 24/56: GCC 2.7.2 as vendored reads the `sh`
   and the face = old_face copy from the 16-bit variables, so the incoming
   copies of places and old_face die early and one saved register goes. The
   vendored 970318 snapshot gives this text the twin's six registers. */
/* func_8005B36C */
s32 MessageFaceSet(face, places, old_face, unused, show_lines)
    u16 face;
    u16 places;
    u16 old_face;
    u16 unused;
    s32 show_lines;
{
    u16 stale;

    if (face != 0) {
        stale = g_MessageFacePlaces & ~places;
        if (stale != 0) {
            CellFacePlacesSet(places, 0xDEAD0001, 0);
            g_MessageFacePlaces = 0;
        }
        CellFacePlacesSet(places, 0xDEAD0001, face);
        g_MessageFacePlaces = places;
    } else {
        CellFacePlacesSet(g_MessageFacePlaces, 0xDEAD0001, 0);
        face = old_face;
        g_MessageFacePlaces = 0;
    }
    if (show_lines != 0) {
        MessageRowDisp(g_MessageRow);
    }
    return face;
}

/* far_syms: g_FaceListEntries (target reaches it %hi/%lo) */

/* func_8005B448 */
u16 MessageFaceEntrySet(s32 entry_then_face, s32 places, u16 old_face, s32 unused, s32 show_lines) {
    s32 local;
    u16 result;

    result = (u16)entry_then_face;
    if (result != 0) {
        entry_then_face = g_FaceListEntries[result - 1];
    }
    local = MessageFaceSet(entry_then_face & 0xFFFF, places & 0xFFFF, old_face & 0xFFFF, unused & 0xFFFF, show_lines);
    return result;
}

extern u16 g_MessageRow;

/* func_8005B4A0 */
void MessageRowDisp(s32 row)
{
  s32 column;
  s32 shown_block_count;
  s32 arrow_x;
  CellRowsClear(g_MessageRow, 6, 0x20);
  g_MessageRow = row;
  if (g_MessageFacePlaces & 1)
  {
    column = 0xC;
    shown_block_count = 0x11;
    arrow_x = 0x128;
    g_MessageArrowX = arrow_x;
  }
  else
    if (g_MessageFacePlaces & 2)
  {
    column = 2;
    shown_block_count = 0x11;
    arrow_x = 0xE0;
    g_MessageArrowX = arrow_x;
  }
  else
    if (g_MessageFacePlaces & 0x10)
  {
    column = 0xA;
    shown_block_count = 0x11;
    arrow_x = 0x128;
    g_MessageArrowX = arrow_x;
  }
  else
    if (g_MessageFacePlaces & 0x20)
  {
    column = 2;
    shown_block_count = 0x11;
    arrow_x = 0xE8;
    g_MessageArrowX = arrow_x;
  }
  else
  {
    column = 2;
    shown_block_count = 0x11;
    arrow_x = 0x128;
    g_MessageArrowX = arrow_x;
  }
  CellTextDisp(column, g_MessageRow, shown_block_count, 3, 0x11, 0xDEAD2001, 0xED00);
  CellAllUpdate(0x20);
  ScriptFrameStep(1);
}
