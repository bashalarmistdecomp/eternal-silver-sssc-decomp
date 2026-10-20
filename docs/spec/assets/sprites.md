# Asset: character and enemy sprites on the field, and the portraits

This page covers three families of pictures and how the code reads each:
the party's field sheets (resource `0x1E8`), the portraits by face number
(the FACE blocks, resources `0xA3` to `0xE4`), and the map object sets
(resources `0x1E9` to `0x249`): the people, enemies and chests a map
shows. The format they share, the sprite data, the sprite pages and
`sprite_draw`, is [../sprites.md](../sprites.md). Where the resources lie
on the disc is
[../file-sector-map/lunadata-sprites.md](../file-sector-map/lunadata-sprites.md);
the palette rows are [../formats/palettes.md](../formats/palettes.md);
every packed stream is unpacked by `inflate_buffer` (`0x80021D6C`,
[../formats/compression.md](../formats/compression.md)).

Counts on this page marked as measured were taken over the retail disc's
resources with station-tools, a custom decomp toolkit that will be
released later, by the rules the code reads.

## The party's field sheets — resource 0x1E8

The party members' field sprites are LUNADATA resource `0x1E8`, which
`MapPartySpriteDataLoad` (`0x80063D48`) reads whole and
`MapPartySpriteDataReload` (`0x800642A4`) reads again when
`MapSpriteDataReload` (`0x800644E0`) loads a map's sprite data anew. The
resource is a directory of 8-byte rows, `SpriteDirectoryRow` (sprite.h):

| offset | field |
|---|---|
| +0 | `data_length`: the unpacked sprite data's length |
| +2 | `data_place`: the packed sprite data's place, from the directory's start |
| +4 | `image_length`: the picture's length, 64 to a line of the 0x80-word page |
| +6 | `image_place`: the packed picture's place |

All four count 4-byte units; measured, all 31 rows' sprite data inflate to
exactly `data_length × 4` bytes. A row's sprite data has the layout of
[../sprites.md](../sprites.md) §"Sprite data", and `MapRecordSpriteDataSet`
(`0x800645A8`) points a record at it. Its picture is 8-bit, 256 pixels a
line. The directory holds 31 rows and every one is used.

Party number N (1 to 12, the numbers the battle's slot ids use too:
`MapObjectTouchRun` (`0x8007ACD8`) copies the field's list into the
battle's) has three sprite data, each loaded only when its entry of
`g_MapSpriteDataEntries` (`0x800B8C68`) is still 0:

* its own, number N, from row N − 1;
* its world sheet, number N + 0xF, from row N + 0xE, loaded and worn only
  while `g_MapPartyKind` (`0x800B8CEE`) is 0 (`MapPartyRecordInit`
  (`0x800649DC`) adds 0xF to the number then);
* its twin's, number `g_MapTwinSpriteDataNumbers[N − 1]` (`0x80012C00`:
  13, 14, 15, 28, 29 or 30 by member), from that number's row less one.
  The twin is a second record in `g_MapPartyTwins` (`0x800B8C34`), kept on
  the member's animation at the member's ground place and drawn as sprite
  entry 0x46 + place, behind every body (`MapPartyAnimationStep`
  (`0x80065CF8`)). The code never names what it shows; by eye it is a
  ground shadow.

Row 0x1E is party number `0xCE`. `sprite_page_image_add` (`0x8001808C`)
puts the own and twin pictures in sprite page slots 5 and 6 and
`sprite_page4_image_load` (`0x80017EB0`) the world pictures in slot 4,
each setting the block's texel records' page and adding the line it chose
to their v. A sprite data a map's object set has already registered under
the same number is not loaded again; measured, 45 of the 97 object sets
list numbers below 0x1F (333 copies), and every copy's animation, step,
pose and piece tables equal the row's in `0x1E8`. So a sprite data number
below 0x1F in an object set is that party member's.

Colours: `MapPartyRecordInit` gives a member kind 1, so `MapRecordInit`
(`0x80065940`) gives it the draw word 0x2000 and `sprite_draw` reads
palette (0, 504). On the field that row is resource `0xE5` after
`sprite_init`'s fix-up (colour 0 zeroed, the rest byte-swapped with bit 15
set), sent again through `sprite_init_copy` (`0x80063B10`) by `GameStart`
and after every battle by `MapBtlRun`; the battle's own party palette
(resource `0x197`) is in that row only during a fight. Number `0xCE` is
kind 2: draw word 0x2100, row 505, resource `0xE6`. No other writer of row
504 is among the game's seven `LoadClut` calls and the command list's
palette handlers; the MENU.BIN overlay and calls through pointers were not
searched for one.

Video memory captured with station-tools in three field scenes (a house
in Burg, the Caldor Isle world view, Dyne's Monument) agrees with this:
rows 504 and 505, 256 colours of 256, and 17 pages equal on every
non-zero byte at the places the allocators give. Against a battle capture
row 504 agrees in 1 colour of 256, as the code says it should not.

Animations: `MapAnimationPoseSet` (`0x8006633C`) gives pose p (from 1) the
animation numbers (p − 1) × 4 + 1 to (p − 1) × 4 + 4 and picks one by
facing: 5 the first, 1, 2 and 8 the second, 6 and 7 the third, 3 and 4 the
fourth. Facing 1 is toward smaller y and the others go round clockwise
(map.h), so the four are down, up, left and right. The party's walk plays
pose 1 (`MapPartyRingFollow` (`0x800671C4`)), which also puts the ring
entry's class into bits 0x6000 of the draw word. Measured: every own sheet
has 53 animations (row 30 has 13); what the other poses are for is not
read, and by eye they include a warp out and in, a jump and a lying-down
pose. Own sheets have 111 to 154 poses (Alex 149, Luna 154, Ramus 111, Nall
115); world sheets 26 each; twins 13 to 19.

Open: who number `0xCE` is (by eye a horned, masked figure in a red robe;
no caller that makes it was traced).

## Portraits by face number — the FACE blocks

A message window, the menus and the battle's window all name a portrait by
a face number and hand it to `FaceCellSet` (`0x8001B3DC`, face.c):

* Face 0 clears the cell.
* `FaceSlot6Find` (`0x8001B5C4`) looks for the face among the ten listed in
  the scene block in slot 6; found, its image there is used.
* Otherwise faces come five to a file: file `(face − 1) / 5 + 1`, image
  `(face − 1) % 5`. `FaceSlotGet` (`0x8001B200`) answers the slot that
  holds the file, or loads it round-robin into the next of six slots with
  `FaceDataLoad` (`0x8001B1C0`): resource file + 0xA2, or file + 0x8E from
  file 0x20 on. For a loaded block that fails `FaceDataCheck` it calls
  `assert_fail` (`0x8001B050`), which in this build does nothing.
* `FaceImageLoad` (`0x8001B310`) unpacks the block from +0x20 into the load
  buffer and sends image n, 0x1000 bytes from n × 0x1000 (64 × 64 at 8
  bits), to (0x100 + 32 × page, 0x1C0), 32 × 64 words.
* The cell becomes `0x400 | page`, and `g_FaceCellPage` (`0x800B85A8`)
  flips, so two faces shown one after the other use both places.

The six slots and the scene slot lie at `0x801A9B00 + 0x3000 × slot`
(`FaceSlotAddress` (`0x8001B15C`)). `FaceSlot6Load` (`0x8001B50C`) reads
scene block n, resource n + 0xBA, into slot 6 for n below 0x2B; script
command 0x3E, `ScriptCommandFaceListSet` (`0x8005E610`), calls it.
`FaceLoad` (`0x8001B394`) loads a face's file without showing it.

The block, each field from the function that reads it (`FaceDataHead`):

| offset | what | read by |
|---|---|---|
| +0x00 | `"FACE"` | `FaceDataCheck` (`0x8001B54C`) |
| +0x04 | u32 the packed stream's length, 1 to 0x4E1F | `FaceDataCheck` |
| +0x08 | s32 the sum of the length / 4 words from +0x20; a block that fails it is not taken | `FaceDataCheck` |
| +0x10 | u8[10] the face numbers the block holds, image i = entry i; 0 = none | `FaceSlot6Find` |
| +0x20 | the packed stream | `FaceImageLoad` |

Two families share that layout (measured):

* Files 1 to 24, resources `0xA3` to `0xBA`: five images each, listing the
  file's own faces 5k − 4 to 5k. These are faces 1 to 120, the whole set:
  108 drawn, 12 blank (32, 40, 44, 49, 50, 61, 73, 74, 75, 86, 93, 106).
* Scene blocks 1 to 42, resources `0xBB` to `0xE4`: ten images each and a
  list of up to ten faces. All 255 listed images are equal to the file
  image of the face they list: a scene block is a pre-packed set, so a
  scene's dialogue reads the disc once. Block 13 (`0xC7`) lists nothing;
  its two drawn images are faces 109 and 118.

So there are 120 faces, and 42 blocks of copies of them.

A face above 120 has no file of its own: files 25 to 31 are the first seven
scene blocks read as files, and from file 0x20 on `FaceDataLoad` reads the
file 0x14 lower, so face 156 + k shows the image of face 56 + k. Whether any
caller passes such a number was not established.

`FaceCellDraw` (`0x8001B4B0`) draws a portrait cell: 64 × 64 from
u = (cell & 1) × 64, v = 0xC0 of texture page `0x94`, palette `0x7DC0` (row
503, resource 1), ordering-table entry 0x10. In a message window `0xFAnn`
and `0xFBnn` show face nn in the left and right place (text.md §"The
controls — what each path does").

## Map object sets — the resource and its sprites

The people on a map, the enemies that walk it and its chests are one
LUNADATA resource per object set: `MapObjectsDataLoad` (`0x80065418`) reads
`u16 g_MapObjectsDataResourceNumbers[set]` (`0x800B4E2C`; 97 sets among
the numbers 1 to 153, one per map). The resource holds the definitions,
their step lists, the sprite data and the pages. Every statement below
names the function it is read from.

The resource starts with s32 words, each a byte offset from its start:

| word | what | read by |
|---|---|---|
| `[0]` | a packed stream: the definition block, unpacked to the address in `g_MapObjectDefinitions` (`0x800B8D20`), `0x801E8900` | `MapObjectsDataLoad` |
| `[1]` | the sprite data list | `MapObjectsDataLoad` |
| `[2..]` | the packed pages, ended by a 0 word | `sprite_page_list_load` (`0x800182B8`) |

* The definition block. Its first 0x80 × 0x26 bytes are the 128 object
  definitions (npc-scripting.md). From byte 0x1300 on it holds the step
  lists: `g_MapObjectStepLists` (`0x800B8BE8`) holds `0x801E9C00`, the
  definitions' address + 0x1300 (both set by `GameStart`). For a row on
  move 1 or 2, `MapObjectMoveSet` (`0x80065A2C`) takes the list that the
  definition's byte 0x12 picks from the table of 16-bit places at the
  start of the step lists; the list's byte 1 is the pose the row starts in
  and the steps' cursor starts at byte 2. Measured: that part is 0x200 to
  0x32C bytes, and 182 definitions in 38 sets walk a list.
* The sprite data list. 6-byte rows, `MapObjectsSpriteDataRow` (map.h):
  `{u16 sprite_data, u16 length, u16 place}`, ended by a number of 0. For
  each row whose entry of `g_MapSpriteDataEntries` is 0 and whose number is
  below 0x100, `MapObjectsDataLoad` unpacks the stream at the list's start
  + `place × 4` into `xmalloc_heap_alloc(length × 4)` (`0x80022F4C`) and
  puts its address in the entry. So `sprite_data` is the number a
  definition's byte 1 names, `length` the unpacked block's length and
  `place` the packed block's start, both in 4-byte units. Measured: all
  1,108 retail rows inflate to exactly `length × 4`; no list repeats a
  number or has one above 0xFF. Then entry 0 is set to `g_SpriteData1`
  (`0x800DDFB0`), bank 1 of resource 2, whatever the set.
* A sprite data block has the layout of ../sprites.md §"Sprite data";
  `MapRecordSpriteDataSet` points a record's animation table, step list,
  pose table, piece table and texel records at its head words 0 to 4.
* Pages. `sprite_page_list_load` unpacks the n-th page into the load
  buffer and sends 0x80 × 0x100 words, 256 8-bit pixels a line and 256
  lines, to `g_SpritePageCorners[n]`: sprite page slot n. A stream may hold
  fewer than 256 lines (measured: 98 of the 227 pages); the rest of the
  upload is whatever the buffer held before, not resource data. A block's
  texel records already name slot n: `MapObjectsDataLoad` calls neither
  loader that sets them. Measured: all 31,348 texel records name a page
  their own set has, and no piece reads below the lines its page holds.

Which sprite data a row is drawn from (`MapObjectInit` (`0x800656CC`)):
the kind is `(definition byte 0 & 0x3F) + 1`; a row of kind 7 gets sprite
data 0, the chest's, and any other the definition's byte 1 (measured, byte
1 is 0xFF on all 223 definitions of kind 7). A byte 1 below 0xF also makes
the row's twin: a copy of the row's 0x3C-byte record, on sprite data
`g_MapTwinSpriteDataNumbers[byte 1 − 1]`, set up with the row's kind and
mode. Measured: 131 definitions have one, all of kind 1 with byte 1 from 2
to 12, and every twin's sprite data is in the set's own list.

The colours. `MapRecordInit(record, kind, mode, sprite data)` sets the
draw word (+0x0E of the record) and the attribute word (+0x0C):

| kind | draw word | attribute | palette (`sprite_draw`: `GetClut(word & 0xFF, 0x1F8 \| ((word >> 8) & 3))`) | what is in that row on the field |
|---|---|---|---|---|
| 1 | 0x2000 | 0x20 | (0, 504) | resource `0xE5` through `sprite_init`'s fix-up |
| 2 | 0x2100 | 0x20 | (0, 505) | resource `0xE6`, the same fix-up |
| 7 | 0x2370 | 0 | (0x70, 507): 16 colours | resource 2's palette, colour 0 of each 16 zeroed (`sprite_reset` (`0x80018DBC`), which `sprite_init` ends with) |
| any other | 0x2200 | 0x20 | (0, 506) | the map's own: below |

then by the mode (the definition's byte 3): 1 ORs 0x0800 into the draw
word; 2 ORs 0x0C00; 3 makes the draw word 0x2000 whatever the kind and ORs
0x8000 into the attribute word. `sprite_draw` and `sprite_piece_draw`
read:

* semi-transparency `((word >> 10) | (attribute >> 15)) & 1`: modes 2 and
  3 draw semi-transparent, mode 1 does not;
* the colour `g_SpriteCols[(word >> 15) | ((attribute >> 14) & 2)]`
  (0x808080, 0x404040, 0, 0 in the image): mode 3 takes entry 2, the
  colour 0;
* the mix group `(word >> 10) & 3`, an index of `g_SpriteMixLevels`: mode
  1 is group 2, mode 2 group 3. A group's level makes the mix copy only
  while `g_SpriteMixOn` is set (../sprites.md §"The sprite list and
  `sprite_draw`").

None of the three changes a texel. The palette is the word's low ten bits,
and no mode but 3 touches them. Measured: the mode is 0 on 2,082 of the
2,145 definitions with sprite data, 1 on 37, 2 on 20, 3 on 6, and all 63
of those are of kind 3.

A semi-transparent plain or mirrored piece keeps the texture page word as
`sprite_init` made it, `GetTPage(1, 0, …)`, blend rate 0 (half the
background and half the sprite); only the mix copy adds 0x20, rate 1
(`sprite_plain_draw` and `sprite_mirror_draw`). The quad and scaled
drawers were not checked for this.

Row 506 is the map's palette. `resource_command_run` (`0x800202FC`) hands
a map's type-4 commands to `resource_palette_load` (`0x80020A08`): for a
sub-number of 1 to 0x7F it unpacks the payload, runs
`palette_bit15_set(buf, 0x100, 0xFF)` (`0x800207E8`: colour 0 zeroed, bit
15 set on the other 255) and `palette_rows_set(6, 1, buf)` (`0x8002061C`),
which sends it with `LoadImage` to row 0x1F4 + 6. Measured: all 97 maps
have one such command, sub-number 1, 512 bytes; after the fix-up there are
33 different palettes.

The row belongs to the map and the sprite data to the set, so a set drawn
on two maps could need two palettes. Measured, it never does:

* each of the 424 rows of the maps' enter tables (command 9,
  `MapEnterTableAddress` (`0x80055E6C`)) that names a set names the set of
  its own destination map: 97 sets, each entered on one map;
* the scripts carry 353 rows inline, in commands 0x26
  (`ScriptCommandMapChange` (`0x8005DB3C`), with row number 0) and 0x5D
  (`ScriptCommandMapScriptChange` (`0x8005F068`)), each of which hands
  `MapChange` (`0x8005922C`) a row built from the command's own bytes, map
  at byte 6 and set at byte 0x10. One names a set other than its map's:
  script 7 enters map 30 with set 60. Set 60 has only kinds 1 and 2,
  nothing from row 506, and the two maps' row-506 palettes are equal
  besides.

57 of the 97 sets draw from row 506.

The chest's sprite data. Entry 0 of `g_MapSpriteDataEntries` is
`g_SpriteData1`, bank 1 of resource 2
([battle-ui.md](battle-ui.md) §"Resource 2 (the common FX sheet)"), on
resource 2's 4-bit sheet in page slot 10: 4 poses, 5 animations, drawn
with the 16 colours at x 0x70 of row 507 (kind 7's draw word 0x2370).

Measured over the 97 sets: 1,108 sprite data, 227 pages (0 to 5 a set),
2,145 definitions with sprite data (kind 1: 132, 2: 782, 3: 202, 4: 9, 5:
794, 6: 3, 7: 223) and 131 twins. 850 (sprite data, palette) pairs are
drawn by a definition (3 sprite data under two rows each). 261 listed
sprite data are drawn by no definition of their set: the party's own (a
set registers numbers below 0x1F, so the party's loader skips them) or
rows a script makes.

What the C leaves open, or the data does:

* Sprite data 255 of sets 2, 5, 6, 56, 102, 107, 109 and 118 has 20 pieces
  280 texels wide at u = 0 of a 256-column page. What the GPU reads for
  the 24 columns past the page is the hardware's behaviour, not something
  the C says. Sets 5 and 56 have definitions on it (2 and 6).
* Which kind, and so which palette, a script gives the rows it makes was
  not read.
* `palette_update` (`0x80020134`) sends a row of `g_PaletteBuf` again
  whenever the row is marked, so a fade or a palette animation changes
  what a row holds while a map runs.
