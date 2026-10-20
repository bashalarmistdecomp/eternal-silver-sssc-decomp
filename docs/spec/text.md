# Lunar SSSC — text and dialogue

This page covers the game's text: the string table every menu and battle
label comes from (LUNADATA resource 1), the packed form of a string and the
routine that walks it (`font_string_walk`), the dictionary of words a
string may splice in, the control codes and what the two text paths do
with them, the field's message window and what draws it, and the names of
places, including which places fight which battle sets. The script
commands that show messages are on npc-scripting.md.

## The string table — LUNADATA resource 1

Every name the menus and the battle UI print — characters, enemies, items
and their descriptions, spells, enemy specials, battle and menu labels,
locations, scene and music titles, system messages — is one table: LUNADATA
resource **1**. It is never plain text on the disc (it is deflate-packed)
and never plain ASCII in RAM (a character inside a run is stored as its
code − 0x1F), which is why byte searches of the executable, LUNADATA and
MENU.BIN for "Slime" or "Healing Song" find nothing.

**Load chain**, read from the C:

```c
/* main (0x80013D2C), main.c, in each pass of its loop: */
main_buffer_init();       /* g_ResourceCommand01Buffer = xmalloc_top_alloc(0x15828): the
                             allocator's top moves down from 0x80156000 to 0x801407D8 */
resource_load_run(1);     /* resource_load(1, g_ResourceReadBuffer = 0x80156000), then
                             resource_command_run walks the buffer as a list of commands */
font_string_table_set();  /* g_FontStringTable = g_ResourceCommand01Buffer */
func_80026AB0();          /* the same store again, among five others */

/* resource_command_run (0x800202FC), resource_load.c: commands {u32 word (number = low
   byte), u32 length, payload}; the next is (length >> 2) words after the payload's start;
   a word of 0 ends the list. Command 1 -> resource_command01_set (0x80020480):
   inflate_buffer(payload, g_ResourceCommand01Buffer). */

/* font_string_put(id, string, mode) (0x80026F78), font_string.c — the lookup by id: */
off = ((u16 *)g_FontStringTable)[id];  if (off == 0) return 0;
font_string_walk(g_FontStringTable + off, font_string_code_put, font_string_control_run);
```

Resource 1 is a list of five commands: command 1 (0xEF34 bytes, the
block), one command 2 and three command 12 (words 0x20C, 0x30C, 0x70C:
palette rows 502, 503 and 507, `resource_palette_row_load`). The command-1
payload inflates to **0x15730** bytes (the buffer holds 0x15828). Measured:
all 87,856 bytes equal the block at `g_ResourceCommand01Buffer` in emulator
memory snapshots taken from boot on, the opening movie's included, so the
table is resident the whole game.

**Layout:** a u16 offset table from the block's start — 0x700 slots, ending
where the first string (offset 0xE00) begins — then the strings, each
`0E <text> 00 FF` in the simple case (§"The packed string" below). 1,216
slots are non-zero.

**The rest of the block.** Two other readers use fixed places in it:
`GameStart` (`0x80055B60`) sets `g_PaletteAnimationCols` to
`g_ResourceCommand01Buffer + 0x599C`, the colours the palette animations of
mode 1 copy from (`palette_animation_step`), and `main_buffer_init`
(`0x80013E04`) passes `g_ResourceCommand01Buffer + 0x899C` to `IconInit`
(`0x8001ABCC`): the icons' offset table and packed pixels, which
`IconImageLoad` unpacks with `lz_unpack` (formats/compression.md).

**The id space** — how each caller builds a string id:

| ids | category | id = | shown by |
|---|---|---|---|
| `0x001`–`0x00C` | characters | char id | `BtlLevelUpGainsDisp` (`0x8004C324`) (level-up message): `id \| 0xF000`, id `0xB` printed as `0xF001` |
| `0x081`–`0x0FF` | enemies (127) | enemy id + 0x80 | `BtlMenuFirstOpen` (`0x80044780`) (btl_menu.c): the different ids of active slots 6 to 13, `+ 0x80` |
| `0x101`–`0x1B6` | items (182) | item id \| 0x100 | `BtlActionStringDisp` (`0x8004987C`) kind 4; `BtlWinItemMessageDisp` (`0x8004BF8C`) (a drop): `item \| 0xF100` |
| `0x1B7`–`0x1E6` | spells (48) | spell row + 0x1B6 | `BtlActionStringDisp` (`0x8004987C`) kind 3: `g_BtlActionNumber + 0x1B6` |
| `0x1E7`–`0x1FF` | battle/menu labels | command (icon) number + 0x100 | a menu command byte is an icon number, and its name is the byte + 0x100: `BtlMenuFirstOpen` (`0x80044780`) and `BtlMenuSpellOpen` (`0x80046434`) (an item's icon number + 0x100), `MenuCardListDisp` (`0x800734F0`) (0x1FE, 0x1FF), `MenuOptionOpen` (`0x80075988`) (0x1FD) |
| `0x201`–`0x2FF` | descriptions (items, spells, menus) | name id + 0x100 | `MenuCursorTextDisp` (`0x8006C8F0`): string `number + 0x200` above string `number + 0x100` |
| `0x301`–`0x37E` | enemy specials (126) | attack class \| 0x300 | `BtlActionStringDisp` (`0x8004987C`) kind 7 |
| `0x401`–`0x4A4` | locations | location id + 0x400 | `MenuCardListDisp` (`0x800734F0`) and the travel menu's `MenuUsePlaceNamePut` (`0x80076E14`); ids from the map records (§"Locations") |
| `0x501`–`0x537` | movie titles | theater entry + 0x500 (entry i plays `_B{i−1:03d}.STR`) | `MenuUseMovieListDisp` (`0x80077838`) (video.md) |
| `0x538`–`0x544` | bromide titles | bromide k + 0x538 | layout only (they follow the theater, in bromide order) |
| `0x547`–`0x58A` | music titles | BGM number + 0x546 | `MenuUseMusicListDisp` (`0x800774EC`), the jukebox (sound.md §"BGM numbers") |
| `0x601`–`0x689` | system messages | direct ids | e.g. `MenuChrRecordOpen` (`0x800704F4`) 0x616-0x618, `BtlLevelUpGainDisp` (`0x8004C0D8`) 0x626 and 0x628 + n |

`BtlActionStringDisp` (`0x8004987C`) is the battle banner that names the
current action: kind 3 (magic) → `number + 0x1B6`, kind 4 by a party slot
(below 6) → `number | 0x100`, kind 7 by an enemy slot (6 or more) →
`number | 0x300`.

**Cross-checks** (data the code does not read): characters 1..12 are Alex,
Nash, Jessica, Mia, Kyle, Luna, Ramus, Ghaleon, Laike, Tempest, Alex, Nall —
the party records' ids (Alex/Luna/Ramus = 1/6/7, the party in a captured
game; the two level-99 records are Ghaleon and Laike; 12 is the companion
Nall). 127 enemy names for the executable's 127 templates, the Slime =
enemy 1. Spell row 33 = Healing Song (checked in the running game), and
rows 9-12 — spell group 2, which `ChrLevelUp` (`0x80028554`) teaches
character 2 — are Nash's Thunderbolt / Thunder Bomb / Thunder Thrust /
Spark Ball. 182 item names: the item table's real extent.

**Measured over the table.** Walking every string the way
`font_string_walk` does: 1,216 strings, 19,353 bytes read, 15,492 codes
handed on. No string of resource 1 uses a dictionary word (the dictionary
serves the maps' script text) or a run's six control characters; outside a
run the strings use only bytes 1, 2, 4 and 6, which stand for 0xFF01,
0xFF02, 0xFF00 and 0xF1FF (the table below). A string printed by id inside
a message window — `MessageControlRun`'s splices `0xF0nn`..`0xF6nn`, or
`font_string_put` with an id, as the item notice's `item | 0x100` — hands
its codes to that path's own two callbacks, so what the table holds is
codes for a callback, not text.

## The packed string — `font_string_walk` (`0x8005A884`)

`font_string_walk(p, emit_char, emit_ctl)` (message_disp.c) reads bytes up
to a byte `0xFF` and returns the address it stopped at:

* **Outside a run**, a byte `k` below `0xD6` stands for entry `k` of
  `g_FontStringCodes` (`0x800B34C0`), a table of 0xD6 16-bit codes. A byte
  from `0xD6` up is the high byte of a code whose low byte follows. A code
  up to `0xEFFF` goes to `emit_char` as a glyph code, less `0x5500` when it
  is above `0xD63F` (so `0xD640..0xEFFF` become `0x8140..0x9AFF`); a higher
  code goes to `emit_ctl`. A code `0xFFnn` cannot be written as two bytes,
  since a `0xFF` byte ends the string: the four `0xFF0n` controls come from
  table entries 1 to 4 or from a run's characters.
* **`0x0E` opens a run**, which a `0x00` closes. In a run a byte `b`:
  * `b < 0x5C`: the character `b + 0x1F`, except six that are controls
    (table below). The C is a switch over the characters 0x24 to 0x5E
    (its jump table at `0x80012A84`, indexed by the character − 0x24).
  * `b >= 0x5C`: a **dictionary word** — `font_string_word_get(n)`
    (`0x80059F4C`), word `n` of `g_FontStringWords`, with `n = b − 0x5C`,
    or the next byte + 0xA1 after `0xFD`, or the next byte + 0x19E after
    `0xFE` (so numbers up to 0x29D). A word's bytes are plain ASCII with
    the same six controls (a second switch, jump table at `0x80012994`);
    there `\` also ends the walk.

The six run characters (what a code means is the handler's business, next
section; the meanings given are the message window's):

| Character | Run byte | Codes handed to `emit_ctl` | In the field's message window |
|---|---|---|---|
| `%` (0x25) | 0x06 | `FF00` | wait for a press |
| `@` (0x40) | 0x21 | `FF02` | next row |
| `\` (0x5C) | 0x3D | `FF03` | close; from a word it also ends the walk |
| `$` (0x24) | 0x05 | `FF00`, `FF03` | wait, close, and the walk ends |
| `*` (0x2A) | 0x0B | `FF00`, `FF01` | wait, then a new page |
| `^` (0x5E) | 0x3F | `FF01` | new page |

A walk that a `$`, or a word's `\` or `$`, ends returns the address of the
run byte that ended it; otherwise the address of the `0xFF`.

### The code table — `g_FontStringCodes` (`0x800B34C0`)

0xD6 entries, never written. `font_string_walk` reads one for each byte
below 0xD6 outside a run; `MessageCodesConv` searches entries 1 to 7. In
the image:

```
Byte       16-bit code       Meaning
0x00       0x0000            the code 0, handed to emit_char (a 0 byte ends a run, not a string)
0x01       0xFF01            new page (message window)
0x02       0xFF02            next row; the third starts a new page
0x03       0xFF03            close the window
0x04       0xFF00            wait for a press, or a time-out
0x05       0xF0FF            splice string 0x0FF
0x06       0xF1FF            splice string 0x1FF
0x07       0xF2FF            splice string 0x2FF (the lookup path only)
0x08-0x0D  0x0000
0x0E       0xFE00            never read: 0x0E opens a run before the table is looked at
0x0F       0x0000
0x10       0x8140            Shift-JIS full-width space
0x11-0x97  0x990A-0x99E2     the game's own glyphs
0x98-0xCF  0x88EA-0x9862     Shift-JIS level-1 kanji, drawn from the BIOS's font
0xD0-0xD5  0x0000
```

Bytes 5 to 7 are splices by what the handlers do with `0xF0FF`, `0xF1FF`
and `0xF2FF`: `font_control_run` writes string `code & 0xFFF` for every
high byte 0xF0 to 0xF6, and `MessageControlRun` for 0xF0, 0xF1, 0xF4, 0xF5
and 0xF6 only. The table's strings use byte 6: "Load?" is `{0x1FF}?`.

**Glyph codes.** `font_code_put` (`0x80026D64`) draws a code with the
pattern `font_pattern_get` (`0x80026B1C`) gives: codes 0x20 to 0x7A from
`g_FontPatterns20`, codes 0x9908 to 0x99F8 from `g_FontPatterns9908`
(`code − 0x9908`, 241 patterns of the game's own), and any other code from
the BIOS's Shift-JIS font (Sony's `Krom2RawAdd`; a blank pattern when it
answers −1). 0x9908 to 0x99F8 lie in Shift-JIS's kanji range; the program
draws them from its own patterns instead of the BIOS's.

### The dictionary — `g_FontStringWords` (`0x800B386C`)

Dialogue and some strings splice in **words**: common words and phrases
stored NUL-separated in the executable and picked by number inside a run.

| Property | Value |
|----------|-------|
| Base | `g_FontStringWords` (`font_string_word_get` (`0x80059F4C`) counts NULs from here; it has one caller, `font_string_walk`) |
| Words | 512, of 2 to 34 bytes from 0x20 to 0x7E, ending at `0x800B45F0` |
| Numbers a run can give | 0..0x29D (`b − 0x5C`, `FD` + 0xA1, `FE` + 0x19E) |

Common word parts and phrases (entries 0..3 are `ta`, `te`, `I `, `He`):
```
"ing "  "Mia "  "t's "  "I'm "  "And "  "she "  "But "
" you "  ", huh?"  " could "  ", but "  "about "
"the magic"  "the Vile Tribe "  "the world"  "Alex"
"Dragonmaster"  "Magic Guild"  "I'm sure "
```

### Packing a list of codes — `MessageCodesConv` (`0x8005AC44`)

`MessageCodesConv` (message_disp.c) packs the other way, for `MessageDisp`
(`0x8005AD50`): a list of 16-bit codes ended by 0xFFFF becomes a string
ended by 0xFF, in 0x40 bytes of `MessageDisp`'s stack. A code that entry i
(1 to 7) of `g_FontStringCodes` holds becomes the one byte i; any other
code is written as its two bytes, high first. A code from 0xA to 0xEFFF is
first printed with the line "ERR:MessageConv( code=%04x ) can't convert."
and written all the same.

**It mangles digits.** A code below 0xA (a digit) has 0x824F added before
the table search, but the bytes written are those of the code as it was
read: 0x00 and the digit. `font_string_walk` reads them back as entry 0 of
the table (the code 0) and the digit's own entry, which for digits 1 to 7
is one of the controls above (0xFF01 a new page, 0xFF03 a close, …); the
sum 0x824F + n, a full-width digit, is never written. Only two callers pass
digits: `MenuPageNumberHang` (`0x8006D9A8`) and `MenuUseNumberHang`
(`0x80076688`), which show a number and then stop for good, for menu pages
and uses that have no function of their own. Read from the C, not played.

## The controls — what each path does

`font_string_walk` hands each control to its caller's `emit_ctl`. Two
callers give it different handlers: the lookup path `font_string_put`
(`0x80026F78`) (labels and menus, through `font_string_control_run`
(`0x80026F54`) to `font_control_run` (`0x8002710C`), font_string.c), and
the field's message window (`ScriptCommandMessageDisp` (`0x8005AB1C`),
through `MessageStringControlRun` (`0x8005A864`) to `MessageControlRun`
(`0x8005AF20`), message_disp.c). Both switch on `code & 0xFF00`.

| Code | `font_control_run` (labels) | `MessageControlRun` (message window) |
|------|-----------------------------|--------------------------------------|
| `0xF0nn`–`0xF6nn` | splice string `code & 0xFFF` (all seven high bytes) — "Load?" is `{0x1FF}?` | splice string `code & 0xFFF` for `F0`, `F1`, `F4`, `F5`, `F6` through `font_string_walk` with the same two callbacks, so splices nest (a case `0xF080` can never match); `F2`, `F3`: nothing |
| `0xF8nn` | nothing | wait nn frames (`ScriptFrameStep(1)` (`0x8005A1F0`) each) |
| `0xF9nn` | `font_pen_move(nn)` (`0x800270CC`): the pen moves nn **pixels** (the x within the 16-px cell plus nn, whole cells carried) | the same |
| `0xFAnn` | nothing | `g_MessageFaceFirst = MessageFaceDisp(nn, 0x10, g_MessageFaceFirst, 0)` (`0x8005B2F0`) — **face nn in the left portrait place** (bit 0x10, grid cell (1, 0x12)); nn = 0 takes the shown faces down |
| `0xFBnn` | nothing | `g_MessageFaceSecond = MessageFaceDisp(nn, 0x20, g_MessageFaceSecond, 0x8000)` — the same in the right place (bit 0x20, cell (0x1F, 0x12)) |
| `0xFCnn` | `font_col_set((code >> 6) & 0xFC)` (`0x800270BC`), which is 0xF0 for every nn — not one of the four styles | `font_col_set((nn & 3) << 2)` — the glyph **style** `g_FontColBits` (0/4/8/12, OR'd into every glyph nibble: four colour variants of the text sub-CLUT, assets/battle-ui.md) |
| `0xFF00` | nothing (labels are one line) | **Wait for a press.** Pad record `g_MessagePadRecord` (`pad_record_init(rec, 0x14, 0xD0, 0)` (`0x8002687C`)), then one frame per pass (`ScriptFrameStep(1)`, `pad_record_update(rec, 0x14)` (`0x80026930`), `MessageArrowStep()` (`0x8005A678`)) until the repeat word (`rec+4`) has bit 0x40. With `g_MessageTimed` set the repeat word is zeroed every frame (input ignored). A new press of bit 4 (`rec+2`) counts up `D_800B8E34`, which nothing reads. With `g_MessagePressWait == 0` the wait also ends by itself after `(row + 1) * 60 + 40` frames |
| `0xFF01` | nothing | **New page**: clear the 0x1980-byte buffer, `MessageLineDisp(1)` (`0x8005AD94`) for rows 0..2, row = 0, pen to `0x801CBB00` |
| `0xFF02` | nothing | **Next row**: `++row < 3` moves the pen to `0x801CBB00 + row * 0x880`; the third advance falls into `0xFF01` |
| `0xFF03` | nothing | `MessageClose()` (`0x8005A72C`) — close the window |

The window is three rows of 0x880 bytes at `0x801CBB00`; `g_MessageLine`
is the current row. `0xFF00`, `0xFF02` and `0xF8nn` first call
`MessageLineDisp(1)` (`0x8005AD94`) when `g_MessageSpeed == 0` (below). No
RNG draw; the frame waits are the `ScriptFrameStep(1)` calls.
`MessageControlRun` is the field twin of the battle popup's
`BtlMessageControlRun` (`0x8004C5E4`) (btl_message.c, menu-ui.md).

**`MessageLineDisp(n)` (`0x8005AD94`) — show the current row, then wait n
frames** (message_disp.c). `n == 0` does nothing, not even the upload: with
a text speed `g_MessageSpeed` of 0 the per-glyph call
`MessageLineDisp(g_MessageSpeed)` shows nothing, which is why the three
codes above make the call themselves. Otherwise, by the row counter
`g_MessageLine`:

| row | blanked — `CellRowsClear(first, 6, 0x20)` (`0x800573E0`) | shown |
|---|---|---|
| 0..2, the window's own three rows | grid rows 0..5 | `MessageRowDisp(g_MessageRow)` (`0x8005B4A0`) re-places the window's label strip (that callee waits one frame itself) |
| 3..6, the four one-line rows | grid rows 0x16..0x1B | `font_image_load(0x801CBB00 + row * 0x880, row * 0x10, 0x10)` (`0x80016438`) uploads the row's 16 cells (16 × 16 pixels, 4-bit, 0x80 bytes each) to label strip `row` from cell 0; then `font_cell_set(x, y, row, 0, 0x10)` (`0x800161CC`) writes the one grid cell that shows that run — (3, 2) for row 3, (3, 4) for row 4, (8, 4) for row 5, (0x11, 4) for row 6 |
| any other | grid rows 0x16..0x1B | nothing |

Then `ScriptFrameStep(1)` n times, one field frame each; `n < 0` (the `−1`
callers, the prompts' redraws) draws and adds no frame of its own. Callers
pass the text speed, 1 or −1. Rows 3..6 are the buffers `0x801CD480`,
`0x801CDD00`, `0x801CE580`, `0x801CEE00` — rows 3 and 4 hold the prompts'
answers (npc-scripting.md, the two-answer and four-way prompts). No RNG
draw.

The string table bears this out: `{FF02}` sits where a line breaks
("What? It's locked, Alex!{FF02}I wonder what's inside..."), `{FF00}{FF01}`
where a page ends ("Data cannot be saved.{FF00}{FF01}To save data,…"), and
`{FC02}…{FC00}` brackets emphasised words. `0xFA38` opens the four lines
Nall speaks (ids 1611, 1612, 1642, 1643): it is a portrait. The chain is
`MessageFaceDisp` (`0x8005B2F0`) → `func_800761A4` (face_list.c, which
returns its argument) → `MessageFaceSet` (`0x8005B36C`) (message_disp.c) →
`CellFacePlacesSet` (`0x80057608`) (cell_face.c), which calls
`FaceCellSet(column, row, face)` (`0x8001B3DC`) (face.c), the portrait
loader, once for each place bit. Face 0x38 = 56 = (12 − 1) × 5 + 1, the
first face of character 12. Face numbering and the FACE blocks:
assets/sprites.md §"Portraits by face number".

The shop has a twin of `MessageFaceDisp` for a party member:
`ShopFaceSet(n, places, kept, a3)` (`0x80054C0C`) (shop_face.c) makes face
`(n − 1) * 5 + 1`, falls back on the kept face when `func_800761A4`
answers a negative 16-bit value (as it does for n = 0), and calls
`MessageFaceSet(face, places, kept, a3, 0)`, which does not go on to
`MessageRowDisp`. Its three callers are the shop's (shop_screen.c), each
with the member under the cursor, place bit 1 and `g_MessageFaceFirst`,
storing the result back there.

### The message window — what draws it

* **The window is the battle UI's label layer, driven from the field.** Its
  `emit_char` is `MessageStringCodePut` (`0x8005A834`), whose whole body is
  `font_code_put(code, 1); MessageLineDisp(g_MessageSpeed);` (message_disp.c).
  So a message is drawn with the executable's 16-px **label font**
  (§"Glyph codes" above, outlined by `font_pattern_outline` (`0x80026B98`))
  — not with resource 1's 8-px atlas.
* **A glyph** is OR-ed column by column into 16 × 16, 4-bit cells of the
  buffer at `0x801CBB00`; a pixel is `style | value` (style 0 / 4 / 8 / 12
  from `0xFCnn`, value 1 ink, 3 outline). A row is 0x11 cells = 0x880
  bytes = 272 px. **There is no wrap**: a row longer than that goes on,
  mid-glyph, into the next row's buffer.
* **Shown** by `MessageLineDisp` (`0x8005AD94`) → `MessageRowDisp`
  (`0x8005B4A0`) → `CellTextDisp` (`0x800572E0`): the row's cells are
  uploaded into the eight VRAM label strips (`font_line_image_load`
  (`0x80016350`); strip places `g_FontLines`) and kind-2 cells written on
  the 40 × 28 cell grid at rows 0x14, 0x16, 0x18. **Drawn** by
  `font_cell_draw` (`0x80016134`) (font_cell.c): one sprite per kind-2 cell,
  CLUT `0x7ECE` = row 507, entries 0xE0..0xEF, ordering-table entry 0x38.
* **The band** — `MessageShadeDraw` (`0x80015E88`) (message_draw.c): a flat
  rectangle of colour 0 with the semi-transparency bit under draw mode 0x1F
  (the background halved), 320 wide, `g_MessageShadeHeight` × 8 px high,
  centred on y 0xB8: y 152..216 when fully open. Ordering-table entry 0x40.
* **The "more" arrow** — `MessageArrowDraw` (`0x80015F0C`) (message_draw.c):
  a flat triangle (x, y) – (x + 8, y) – (x + 4, y + 5), point down, colour
  0x20F0F0 (0x00BBGGRR), entry 0x10, bobbing 0 to 4 px.
* **A portrait** — `FaceCellDraw` (`0x8001B4B0`) (face.c): 64 × 64, texture
  page 0x94, CLUT `0x7DC0` (row 503), entry 0x10, at the kind-4 cell the
  face was put on.
* **Timing**, all of it `ScriptFrameStep(1)` calls (one field frame each):
  opening 9 frames (`MessageOpen`: the band grows 1 to 8, a frame each, and
  one more); a glyph `g_MessageSpeed + 1` frames (0 at speed 0, where the
  row appears at once); showing a row 2; a new page 6; closing 18
  (`MessageClose`: five rows emptied and shown, the faces taken down, the
  band lowered 7 to 0, and one more).
* **The option bytes** `*g_GameOptions` = 0x801DC374 (7 bytes, set by
  `GameStart` (`0x80055B60`), game_start.c). `ScriptLabelRun`
  (`0x80059FDC`) copies byte 4 into the text speed `g_MessageSpeed` and
  byte 3 into `g_MessagePressWait` before each run. A new game sets byte 4
  to 0 and byte 3 to 1; the options page steps the speed 0, 1, 2. **A
  wait for a press ends by itself only when `g_MessagePressWait` is 0** —
  after `(row + 1) * 60 + 40` frames — so in a new game it waits for ever.
* **`0xFCnn` means two things** (the table above): in the window a style
  `(nn & 3) << 2`; on the label path 0xF0 for every nn.
* **At speed 0 a line with no control code after it is never shown**: only
  `0xFF00`, `0xFF02`, `0xF8nn` and a new page upload a row.
* **The two-answer prompt** — `MessageAnswerRun` (`0x8005B6C0`) (lns_intp.c):
  the answers are rows 3 and 4 (`0x801CD480`, `0x801CDD00`), put on the
  grid at cells (3, 2) and (3, 4) — the top of the screen — and showing
  them takes the window's second and third row off the grid. The bar is a
  filled 0x110 × 0x10 rectangle (`MessageCursorSet(0x110, 0x10, 0x7B1, 0)`
  (`0x8005B694`)), opaque, colour 0x7B1 of the palette buffer = CLUT row
  507, entry 0xB1 (`sprite_piece_box_draw` (`0x80019668`)), at x
  `g_MapViewX + 0x18`, y `g_MapViewY + sel * 16 + 16`; up / down (repeat
  word & 0x5000) flips `sel` with sound request (1, 1), the button (press
  word & 0x40) chooses with (2, 1). A button already down at the prompt's
  first poll is not a press; held on, it answers after 11 frames.
* **Row 507 on the field.** `main` loads it twice at start-up: resource 2's
  copy first (`sprite_reset` (`0x80018DBC`), which loads its palette to row
  507), then resource 1's (its command 0x70C). `sprite_reset` loads
  resource 2's again when `sprite_init` (`0x80018BFC`) and `MenuNoWalkRun`
  (`0x80075BD8`) call it. Which one the field holds under a window was not
  traced; sub-CLUTs 11..14 (entries 0xB0..0xEF — the bar's and the text's)
  are word-identical in both, measured on the retail disc.

**Not read:** which end of the field's ordering table is drawn first (if a
higher entry is drawn further back, the opaque bar lies behind the two
answers; the other way round it would cover them); whether the scripting
side's camera `g_MapViewX` / `g_MapViewY` equals the renderer's
`g_BgScrollX` / `g_BgScrollY` when the bar is drawn; what the options menu
calls option byte 3 (nothing in the program names it).

## Locations — which name a map has

Every map entry runs **`MapSet(map, music, script)` (`0x80068780`)**
(map_load.c; the order of its steps is in npc-scripting.md §"Map set-up"):
`g_MapNumber = map`, then a copy of the map's 16-byte record
`MapTblGet(map)` (`0x80068618`) =
`g_MapTbls + 16 * (map < 0xA0 ? g_MapTblNumbers[map] : 0)` (`MapTbl`,
map.h):

| byte | to | meaning |
|---|---|---|
| 0 | `g_MapPartyKind` | the party kind (map.h: 0 gives the party's records sprite data number + 0xF; 2 puts every party record on place 0's ring entry) |
| 3 | — | the record's own map id (no code reads it on its own) |
| 6 | `g_MapPlaceName` | **location id** — the name is string `0x400 + id`; ids 0x51..0x64 (the towns) also set story bit `id + 0x28A` |
| 8 | — | the map's **event-script id**: `MapSet` puts its third argument there when that is not 0, and `MapLoad(&rec, 1)` (`0x80068A10`) hands it to `ScriptChange` (`0x80068D7C`) (npc-scripting.md §"Event scripts") |
| 0xF | `g_MusicNumber` | **BGM number** (unless the caller passes one other than 0xFF; party kind 0 with music 6 plays 7 once story bit 0x3C1 is set) → `MapMusicSet` (`0x80058EE4`) (sound.md §"BGM numbers") |

`g_MapPlaceName` is also set by the load-game restore (`GameEnter`
(`0x80059498`), from the save block's byte +0xE) and by script command 0x49
(`ScriptCommandPlaceNameSet` (`0x8005E9D4`)), which gives the place another
name than the map's record gave it. It is printed by the save/load screen
(`MenuCardListDisp` (`0x800734F0`): `g_MapPlaceName + 0x400`, and each slot
header's +0xE `+ 0x400`) and saved by `MenuSaveWrite` (`0x80074024`).

**Map id → resource:** `MapSeek` (`0x8006891C`) → `MapResourceFind`
(`0x80020CD0`) = `resource_pair_find(g_MapResourcePairs, map)`
(`0x80020C58`): (s16 map id, s16 resource) pairs ending at 0 — 97 maps, ids
0x01..0x99 → resources 0x04..0x64 of LUNADATA.FIL. The map resource's
header byte 19 equals its map id (all 97). Records for map ids 0x87-0x8C
and 0x8F-0x92 have no resource (INFERRED leftovers).

**The location string lists are one id space** (string = 0x400 + id):
0x451-0x4A4 (ids 0x51-0xA4) are what the map records hold — 0x51-0x5F the
travel menu's towns (item-use menu 3, battle.md); 0x401-0x44C (ids
0x01-0x4C, styled `FC03 … FC00`) are reached by no code of the executable
(INFERRED: script text). The executable's ASCII short names at
`g_CardSavePlaceNames` (84 pointers, index `id − 0x51`) feed only the
memory-card title.

**The memory-card title — `card_save_title_set(buf, slot, loc, level, time)`
(`0x8001BF50`)** (outline.c; called by `card_save_header_set`
(`0x800253C8`) (card.c), which hands it the 64-byte Shift-JIS field at save
header `+4`). It `sprintf`s `"LUNAR-%02d %d:%02d L%d %s"` straight into
`buf` with slot + 1, time / 60, time % 60, the level and
`g_CardSavePlaceNames[loc − 0x51]` (no range check on the 84 pointers),
then widens the ASCII to full-width Shift-JIS in place, last character
first, through the table `g_CardSaveTitleWideChars`: 0x60 two-byte entries
for 0x20..0x7F, high byte first, ending exactly where
`g_CardSavePlaceNames` starts (space → 81 40, '0'.. → 82 4F.., 'A'.. →
82 60.., 'a'.. → 82 81.., punctuation to its full-width form, 0x7F → 00
00), after two zero bytes have been put behind the last. The result is
2 × len + 2 bytes: 32 characters fill the field exactly, and a 14-letter
name (Grindery Enter, Thieves' Guild, Thieves'Bazaar) or a three-digit hour
count runs past it — the caller's `bzero(header + 0x44, 0x1C)` afterwards
cuts the overrun off. No frame wait, no RNG draw.

Examples: map 0x01 → 0x465 "Caldor Isle"; 0x15 (map 21) → 0x451 "Burg";
0x43 → "Ghaleon's Lair"; 0x99 → "Meribia Port".

## Arena locations — which places fight which battle sets

Every battle in the field starts one of two ways, both read from the C;
they are the only calls of `MapBtlRun` (`0x8007AE58`), and `MapBtlRun` is
the only caller of `BtlRun` (`0x80027434`):

* **object** — the lead touches a map object (`MapObjectTouchFind`
  (`0x8007AB64`): a row of kind 4, 5 or 6 whose byte `+0x3D & 0x3F` is not
  3), and `MapObjectTouchRun` (`0x8007ACD8`) acts on the object's definition
  byte 0xB: kinds 5 and 6 run `MapBtlRun(def[0xB], 0, 0, 0, 1, 1, 1)`, so
  **def[0xB] is an encounter id**. Kind 4 runs `ScriptLabelRun(def[0xB])`
  (`0x80059FDC`), a **script event**, not a battle (and not a map exit).
  The object definitions come from the map's object set (npc-scripting.md
  §"A map's object definitions come from its object set, not its map
  file").
* **script** — a BATTLE command (id 0x2B, `ScriptCommandMapBtlRun`
  (`0x8005DFF0`), lns_intp.c) in the map's own event script: operand
  `row + 1` selects the 8-byte row of `g_ScriptMapBtlRows` whose byte 0 is
  the encounter id (battle.md §"The script's BATTLE command").

An encounter's enemy set is the first byte of its row of
`g_BtlNumberRows` (`g_BtlEnemySet`; battle.md §"Battle entry graphics");
a map's location is its record's byte 6.

**Measured over the retail data** (every map's object set and event
script, walked with the rules above): 97 maps (all have scene entries), 97
object sets read, 728 encounter-bearing definitions on 36 maps, 62 scripts
all walked to their end (every label a command boundary), 79 BATTLE
commands in 27 scripts. **All 48 sets with a graphics resource are
reached** — sets 1..51 less 30, 42 and 44, whose `g_BtlDataResources`
entry is 0: 26 by objects, 30 by scripts, none unresolved. Bosses and story
fights (the sets no wandering monster uses) are reached by scripts only.

What the counts rest on:

* **Location 158, "Monster Test"** (map 9, script 0) holds 29 BATTLE
  commands (encounters 223-254) and shows up in 30 sets. Map 9's scene
  table only enters itself and nothing leads to it; script 0 is the test
  room's battle menu (script 200 its map-select menu). It is retail data.
* **Shared scripts.** One script serves several maps (script 1 serves
  seven); a BATTLE in it counts for every map of the script. The bytecode
  does not say which map it fires in.
* **Script jumps (`0x42`) are not followed**: the destination script runs
  where the party arrives, so its BATTLE commands count for that map's own
  script.
* **Location overrides.** `SET_LOCATION` (0x49) occurs only in scripts 25,
  45 and 200 — none of which holds a BATTLE — so no battle's location is
  overridden.
* The 9 kind-4 object events (maps 113 and 123) start map script events;
  the battles those events reach are BATTLE commands of the same map's
  script and are counted there.

## Related functions

| Address | Name | Purpose |
|---------|------|---------|
| `0x8005A884` | `font_string_walk` | The packed-string walker (above) |
| `0x80059F4C` | `font_string_word_get` | Word n of the dictionary at `g_FontStringWords` |
| `0x80026F78` | `font_string_put` | String-table lookup by id → `font_string_walk` |
| `0x8002710C` | `font_control_run` | The lookup path's control handler (splices, pen, style) |
| `0x8005AF20` | `MessageControlRun` | A message window's control handler |
| `0x8004987C` | `BtlActionStringDisp` | Battle banner: names the action (spell / item / enemy special) |
| `0x800202CC` | `resource_load_run` | Load a resource and walk it as a command list (`resource_command_run` (`0x800202FC`)) |
| `0x80020480` | `resource_command01_set` | Command 1: inflate into the string block |
| `0x8005AC44` | `MessageCodesConv` | Packs a list of 16-bit codes into a string, through `g_FontStringCodes`; mangles digits (above) |
| `0x8005B568` | `ScriptErrorHang` | Where the script reader stops on an error: `assert_fail("TextError!", "LNS_INTP.C", 0x66A)`, an empty function in this build, then a loop with no end |
| `0x80025AC4` | `printmes` | ANKPRINT.C's debug printf onto the 40 × 28 cell grid, named by its own assertion text. Nothing in the image calls it, and it is not the game's text renderer (code/subsystems/io-hardware.md) |

## Open

- What each of the game's own glyphs (`g_FontPatterns9908`, codes 0x9908
  to 0x99F8) is called.
- Which code reads the bromide titles (`0x538`–`0x544`); they follow the
  theater's titles in bromide order, and no reader of them was found.
