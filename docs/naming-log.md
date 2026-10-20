# Names: the log

How the main executable's functions were named, unit by unit and wave by
wave, on 2026-10-13 and 2026-10-14. [naming.md](naming.md) has the rules
these readings follow; this page is the record of what each unit and wave
named, what was changed before it was applied, and what was left.

## Units read one at a time

The first five are the game's random-number generator, in `ui.c`:
`rng_seed`, `rng_refill`, `rng_next`, `rng_below` and `rng_draw`. A sixth
function there, `0x8001C790`, returns `rng_next()` exactly as `rng_draw`
does. It has a row saying so and no name: nothing read so far says why
there are two.

The next thirteen are the memory allocator, in `io.c` (`0x80022D58` to
`0x80022FE8`). Their prefix is the developers' own file name: the assertions
in two of them (`0x80022E1C`, `0x80022EE4`) name `XMALLOC.C`. The other
eleven carry the prefix because each reads or writes the same five words
(`g_XmallocMax` to `g_XmallocTop`) or is called only by one that does. That is a
reading of the C, not a statement of which file the developers kept each in.

The unit has two parts, and the developers' own usage print labels them.
"malloc2" is Sony's heap, which starts where the program's data ends and
grows up. "xmalloc" is an area that starts at `0x80156000` and grows down
toward it. The names follow that:

| | |
|---|---|
| `xmalloc_init` | sets the five words, clears the memory, sets up the heap |
| `xmalloc_clear_ram` | zeroes from the heap's start to `0x801FF000` |
| `xmalloc_heap_alloc`, `xmalloc_heap_free` | Sony's `malloc2` and `free2`: what the game code calls (55 and 54 calls) |
| `xmalloc_heap_reset` | Sony's `InitHeap2` over the same range: the heap is empty again |
| `xmalloc_heap_update_max` | keeps the highest address the heap has reached |
| `xmalloc_heap_find_end` | walks the heap's block headers to its end marker |
| `xmalloc_heap_set_limit` | stores Sony's `_max_heap`, the address the heap may grow to |
| `xmalloc_heap_check_start` | asserts that Sony's `_TopAllocArea` is still an address in memory |
| `xmalloc_top_alloc` | takes a block off the top of the area that grows down |
| `xmalloc_top_set`, `xmalloc_top_reset` | move that top, or put it back at `0x80156000` |
| `xmalloc_print_usage` | prints both parts' use, in the developers' own two lines |

Three of them (`xmalloc_top_reset`, `xmalloc_heap_check_start`,
`xmalloc_print_usage`) are reached by no jump and no address word in the
main executable or in the overlay. They are named all the same, because the
C says plainly what each does, and each row says that nothing reaches it.
`_max_heap` and `_TopAllocArea` are the names release 4.5's `i_heap2.o`
gives the two words; the words themselves are data, and data is not named
yet.

The next eleven are what the developers' checks and prints go through, in
`ui.c` and `io.c`. No assertion names a source file for them, so their
prefixes are coined:

| | |
|---|---|
| `assert_fail` | an empty function. Each of its 15 calls passes a message, a developers' file name and a line number: it is where a failed check reports |
| `debug_printf` | takes a format and arguments and does nothing. Its 19 calls pass the developers' debug text |
| `fmt_sprintf`, `fmt_vsprintf` | the game's own formatter (`%d %x %s %c`, pad and width), which the save title, file names and `printmes` go through. Sony's `sprintf` is not in the image |
| `fmt_put_char`, `fmt_put_string`, `fmt_put_number`, `fmt_digit_char`, `fmt_read_width`, `fmt_digit_value` | its parts |
| `sim_console_write` | writes a string to the file `sim:con`. Nothing reaches it |

`debug_printf` is also what the first split called that function. The name
was taken away with the other unread ones on 2026-10-13 and is given again
here as a reading, with the row saying what was read.

Seven neighbours were read and not named. Each has a row saying what it
does and why it has no name: five beside `assert_fail` (three empty
functions, one that writes to an address the console has no memory at, one
that loops forever), `0x800216E0` (stores its argument in a word nothing
reads) and `0x800217AC` (`sim_console_write` behind a test of a byte that
nothing read so far explains).

The next fourteen read files, in `io.c` around the developers' own
`cd_load` (`0x80020DC0` to `0x80021584`). `cd_load` is proven there, so the
unit is spelled as it is, and the names are built on it:

| | |
|---|---|
| `cd_read_start` | marks the buffer and starts Sony's `CdRead`; does not wait |
| `cd_read_check` | Sony's `CdReadSync`, and -2 when the mark is still in the buffer |
| `cd_sector_load` | reads a run of sectors and waits, starting over until it is done. `cd_load` reads a named file through it |
| `cd_dir_init`, `cd_dir_read`, `cd_dir_add`, `cd_subdir_read` | build the game's table of the disc's files from the disc's directory |
| `cd_dir_find` | finds a file's row in that table by name |
| `cd_file_seek` | sends the drive to a place in a file, reading nothing |
| `pc_load` | `cd_load`'s twin for the developers' host PC. Its assertions name `CDREAD.C` and begin with "PC" |
| `file_load`, `file_seek`, `file_read_check` | pick between the two: the PC when the byte at `0x8001000A` is above 0, the disc otherwise |
| `hex_dump` | gathers bytes sixteen at a time and does nothing with them. The name is a reading of that shape, and its row says so |

`file_load` is also what the first split called that function; as with
`debug_printf`, the name is given again as a reading. `cd_subdir_read` and
`file_read_check` are reached by nothing. `0x80020E7C` was read and not
named: it sets a word that changes what `cd_sector_load` calls while it
waits, and the function it then calls has not been read.

**The first unit of the game's own code** is the script actors' movement,
in `scripting_d.c`, beside the developers' own `MoveChrAction`. That name is
proven there, so the unit is spelled as it is: `CapitalisedWords`, the thing
first and the action last.

Each of the 18 rows at `g_MoveChrRows` (an actor moved by script; row 0 is the
camera) runs a list of commands, one byte and its operands. Once a frame
the current command's handler is called through the table at `0x800B45F8`:

| Name | Address | Command | What was read |
|---|---|---|---|
| `MoveChrUpdate` | `0x800617D8` | | calls the next one, then `MoveChrAction`; once a field frame |
| `MoveChrCommandRun` | `0x80061800` | | calls the handler of each busy row's current command |
| `MoveChrWalkStep` | `0x8005F520` | | one tick of a row on its way to a goal |
| `MoveCommandPoseSet` | `0x80062FF4` | `F0 pose` | picks the actor's animation for a pose and the row's facing |
| `MoveCommandFacingSet` | `0x80063038` | `F1 facing` | sets the row's facing and re-picks the animation |
| `MoveCommandHeightStepSet` | `0x80063080` | `F2 step` | sets how fast the row's height changes |
| `MoveCommandMoveTo` | `0x800630A4` | `F3 x y z facing` | sets a goal, then `MoveChrWalkStep` every frame |
| `MoveCommandMoveBy` | `0x8006312C` | `F4 dx dy dz facing` | the same, from where the actor is |
| `MoveCommandSpeedSet` | `0x800632D8` | `F5 speed` | picks the pixels in a tick and the frames between ticks |
| `MoveCommandWait` | `0x80063330` | `F6 frames` | counts frames down |
| `MoveCommandPlace` | `0x8006338C` | `F7 x y z facing` | puts the actor on a spot at once |
| `MoveCommandScale` | `0x800635A0` | `F8 ix iy rate` | sets the actor's two scales, at once or over time |
| `MoveCommandEnd` | `0x800636E4` | `F9` | clears the row's command pointer and its busy bit |

`MoveCommand` is spelled out: the developers shorten "character" to `Chr`,
and no name of theirs shows how they shortened "command". The command
names (`MoveTo`, `MoveBy`, `Place`, `Scale`) are the specification's
(`docs/spec/code/subsystems/npc-scripting.md`), which reads those four the
same way.

A second table, at `0x800B4620`, holds the same ten commands for rows that
carry a list of steps (the map's own objects). Its ten functions
(`0x80063714` to `0x80063A64`) were read and each has a row saying what it
does. They are not named yet, because the function that walks the list and
calls the table (`0x80067768`) has not been read.

## Waves of readers

The first wave, with the group the applying session read itself:

| Group | Where | Names begin | Named | Read, not named |
|---|---|---|---:|---:|
| Map objects' step list | `0x8006370C` to `0x800677D4` | `StepCommand`, `Step` | 12 | 0 |
| Icons | `0x8001ABCC` to `0x8001AF04` | `Icon` | 7 | 0 |
| Face portraits | `0x8001B15C` to `0x8001B50C` | `Face` | 9 | 2 |
| An unreached hook on the exception vector, and the five beside `assert_fail` | `0x8001B01C` to `0x8001B0F0` | | 0 | 9 |
| Streamed sound from the disc | `0x8002347C` to `0x800238BC` | `xa_` (and `cd_file_search`) | 13 | 1 |
| Sequenced sound and its banks | `0x80023978` to `0x80024784` | `sound_` | 40 | 4 |
| A picture show | `0x800247AC` to `0x80024A7C` | `Picture` | 7 | 0 |
| Disc change, a full-screen picture, a wait | `0x80024B70` to `0x80024D2C` | `cd_`, `screen_`, `vsync_` | 5 | 0 |
| Memory card | `0x80024E3C` to `0x80025638` | `card_` | 16 | 0 |
| Drawing packets | `0x8002566C` to `0x800257EC` | `packet_` | 4 | 0 |
| The text grid, around `printmes` | `0x80025864` to `0x80025DC0` | `ankprint_` | 9 | 0 |
| Frame-time marks | `0x80025DF4` to `0x80025EAC` | `profile_` | 4 | 0 |
| Fixed-point math and curves, around `MTH_Div` | `0x8001C7B0` to `0x8001D110` | `MTH_` | 7 | 5 |
| A scrolling tile layer | `0x8001D278` to `0x8001DAE4` | | 0 | 13 |

Each name's row is in the register. `xa`, `sound`, `ankprint` and `MTH` are
the developers' own words (their text or their file names); `card`,
`packet`, `profile`, `Picture`, `Icon`, `Face` and `Step` are coined, and
`Icon` and `Face` follow the developers' file names `ICON.C` and `FACE.C`.

The script found nothing wrong in the five proposals as they were handed
in. The sample was 25 functions, five or so from each proposal, and all 25
rows said what the C does. What the applying session changed: one name
whose key word came from the specification and not from the C
(`FaceSlot6Load`); two spelled `chk` (`card_change_check`,
`cd_seek_check`); five that two readers had named on different words for
the same thing (`sound_music_level_set` and its four neighbours); seven of
a group that is the game's own code inside a window spelled for code under
the game (`PictureShow` and six more); and two names withheld, one for a
second function that does what `sound_seq_stop` does and one for the
square root below.

The second wave, the same day:

| Window | Names begin | Named | Read, not named |
|---|---|---:|---:|
| `ui.c`, `0x8001B058` to `0x8001C548` | `exception_`, `Face`, `cd_`, `Shape`, `Outline`, `ShopStock`, `card_`, `PlayTime`, `screen_fade_` | 18 | 10 |
| `boot_init.c` and `rendering.c`, `0x80013D08` to `0x800161CC` | `build_stamp_`, `main_buffer_`, `bg_`, `packet_`, `Message`, `StartPicture` | 45 | 6 |
| `cdrom_load.c`, `0x80023070` to `0x8002347C`, and `rendering2.c`, `0x80025FE0` to `0x80027500` | `pad_`, `overlay_`, `Movie`, `screen_`, `mem_`, `palette_`, `tile_`, `font_`, `Btl` | 43 | 6 |
| `ui.c`, `0x8001D278` to `0x8001EEB8` | `tile_`, `layer_` | 42 | 6 |
| `ui.c` and `io.c`, `0x8001EEB8` to `0x80020DC0` | `system_`, `screen_`, `frame_`, `packet_buf_`, `pad_`, `palette_`, `resource_`, `tpage_`, `tim_`, `Map`, `Script` | 52 | 4 |

`bg` is the developers' word (`bg_copy`), `mix` is theirs (`ColMix`, and
their line `mapmix=%d`), `Btl` is theirs (`BtlEndMessageDisp`, which lies in
the stretch of code the `Btl` names' callers lie in), and `overlay` is their
file name. The other first words are coined.

`tile_` is the first word of two neighbouring things: the tile layer (a
grid of 16 x 16-pixel tiles drawn over the battle's background, `ui.c`) and
the tiles' pictures in video memory with their animation (`rendering2.c`).

The script again found nothing wrong in the proposals as handed in, and
all 23 functions of the sample were as their rows say. What the applying
session changed: `BtlRun` (proposed as `BtlMain`: the action comes last);
`MoviePlay` (proposed in lower case under `overlay_`: it sets the story's
bits and reloads the faces, which is the game's own code); `pad_start`
(proposed as `pad_init`, one letter's case away from Sony's `PAD_init`);
and seven functions one reader had left unnamed only because another
group's word was not yet given (`resource_bg_second_set` and six more),
named once those words were.

Found and not yet named: six functions that set the layers' brightness
(`0x8001C2C8` to `0x8001C4EC`), a pair that switches a use of the layers'
levels on and off (`0x8001F4B8`, `0x8001F4E0`), and the drawing of the text
grid's kind-2 cells (`0x80016128` on, which runs past `0x800161CC`). Each
function read has a row. Later waves named all of them (`layer_grey_set`
and its neighbours, `layer_mix_switch_on` and `layer_mix_switch_off`,
`font_cell_switch_set` and its neighbours) but one: `func_8001C4E4`, an
empty function, is left by the ruling on the functions first left unnamed.

The third wave, the same day:

| Window | Names begin | Named | Read, not named |
|---|---|---:|---:|
| `rendering.c`, `0x800161CC` to `0x80018000` | `font_`, `ankprint_grid_`, `screen_`, `packet_`, `sprite_` | 25 | 4 |
| `rendering.c`, `0x80018000` to `0x8001ABCC` | `sprite_`, `Btl`, `debug_`, `packet_`, `palette_`, `Icon` | 38 | 2 |
| The battle, `0x80027934` to `0x80029E30` | `Btl`, `Chr` | 22 | 0 |
| The battle, `0x80029E30` to `0x8002C190` | `Btl` | 18 | 1 |
| The battle, `0x8002C190` to `0x8002E5D4` | `Btl` | 18 | 0 |

`sprite_` is the first word of the list of sprites (`g_SpriteList`), its
pictures in video memory, its drawing and its mix; it also stands in
`packet_sprite_add`, where it is the console's textured rectangle.

In the battle, a **slot** is one of the fourteen places in a battle (0 to 4
the party, 5 one more of the party's side, 6 to 13 the enemies), and an
**id** is the number of who stands in it. Four names carry a number where
the C does not say who or what is meant: `BtlSlot5Revive`,
`BtlSlot5ReviveTry`, `BtlSlot6AnimationAlternate` and
`BtlEnemy4FPlaceMark`. `BtlSpecialInit` is the set-up of the battles
numbered `0xEB` and up; "special" is the reader's word.

The three readers of the battle began with different words for the same
things. One of them took the other two's words after reading their
proposals, and said so: the agreement of those three files is not three
independent readings of the words. The readings of what each function does
were made separately.

The script found nothing wrong, and all 17 functions of the sample were as
their rows say. What the applying session changed: four names proposed as
`BtlLevel...` are `ChrLevel...` (`ChrLevelAdd`, `ChrLevelUp`,
`ChrLevelGainGet`, `ChrLevelGainAdd`), because they work on the characters'
records and one of them is reached only from the script's commands.

The fourth wave, the same day, went on through the battle:

| Window | What it holds | Named | Read, not named |
|---|---|---:|---:|
| `0x8002E5D4` to `0x80030B8C` | how an action is run by its kind, adjusted and given a target; the statuses; a hit and its amount | 28 | 0 |
| `0x80030B8C` to `0x80032C64` | the attack; the action's row and its function; adding to hp and mp, clearing a status | 40 | 0 |
| `0x80032C64` to `0x800344B8` | the functions an action's row names: hits, tries for a status, raising a stat, removing | 39 | 1 |
| `0x800344B8` to `0x80036A18` | more of those; marking the slots an action reaches; taking hp | 31 | 0 |
| `0x80036A18` to `0x800393F0` | leaving a battle, animations by action and status, the battle's frame, its end | 25 | 0 |

All are `Btl` but two arithmetic helpers (`ratio_mul`, `ratio_13_10_mul`).
The words these names share, each from one reader's row and taken up by the
others:

- an action has a **kind** (2 an attack, 3 a spell, 4 an item, 5 a walk),
  a **number** and a **target**; its kind and number pick a **row** of
  data, and the row carries the number of the function that does it, an
  entry of the table at `0x800B2EF8` (`BtlActionRowRun`);
- a **hit** is an action's **amount** landing on a slot; the amount is
  worked out from the acting slot's stats, by the word at +8
  (`BtlHitAmountSet`) or the word at +0xE (`BtlStat0EHitAmountSet`);
- a slot is **marked** when an action reaches it (`BtlTargetAreaMark`,
  `BtlTargetFrontMark`, `BtlTargetOtherSideMark`);
- a **status** is named by its bit, in hexadecimal (`BtlTargetStatus4Set`,
  `BtlTargetStatus10TryRun`), and a stat by its place in the stats
  (`BtlTargetStat0ARaiseRun`): the C does not say what each is, and the
  specification's words for them were not confirmed.

The five readers again read one another's proposals and moved toward each
other's words, and said so; one file was rewritten three times as two
readers moved toward each other. What the applying session then changed,
all of it to make the five agree: sixteen names of one reader that began
`BtlAction...` or spelled a status with two digits (`BtlTargetStatus8Run`,
`BtlActingRemoveRun`, `BtlStat0EHitAmountSet` and thirteen more),
`BtlTargetFrontMark` (proposed with "Ahead", where two readers had
"Front"), `BtlSlotSpellAnimationGet` (proposed with "Kind3") and
`BtlEnemyClearRun` (proposed as `BtlEnemyRemoveRun`, beside another
reader's `BtlEnemyAllRemoveRun`, which does less). The script found
nothing wrong, and all 15 functions of the sample were as their rows say.

With this wave ten functions that earlier readers had left for someone
else's word were named (`font_cell_draw`, `layer_mix_switch_on`,
`pad_frame_step`, `cd_wait_btl_frame_set` and six more).

The fifth wave, the same day (`0x800393F0` to `0x8004571C`, 111
functions, all named):

| What | Names | Count |
|---|---|---:|
| Picking an action's spell, item or target at random | `BtlActingSpellPick`, `BtlPartyTargetPick`, `BtlSlotNearestFind` and eight more | 11 |
| Helpers on the generator | `rng_list_pick`, `rng_7f_draw`, `rng_mask_below` | 3 |
| The battle's shows: what is seen when an action whose show number is `0x97` or more is done | `BtlActingShowRun`, which calls entry (number - `0x97`) of the table `D_800B3070`; its 38 entries `BtlShow97Run` to `BtlShowBCRun`; and their helpers (`BtlShotRun`, `BtlShowEnd`, `BtlExtraRecordsAlloc`) | 53 |
| The enemies' choice of action in the battles numbered `0xEB` and up | `BtlEnemyActionSpecialSet` and the functions for each enemy id, `BtlEnemy6FActionSet` to `BtlEnemy7FActionSet` | 19 |
| What the program shows from its start until a game begins | `StartRun`, `StartScreenShow`, `StartMenuRun` and eight more | 11 |
| The battle's menu of commands | `BtlMenuRun`, `BtlMenuOpen`, `BtlMenuPresetOpen` and eight more | 11 |
| Others | `tpage_word_make`, `BtlActingAnimationSet`, `BtlActingKindBSet` | 3 |

The 38 shows and the enemies' functions are named by number. The C tells
one show from another, and one enemy from another, only by its number, and
what each show looks like is in its row. A reader who had first given the
shows words (`BtlActingSixCopiesShow`) gave them up for the numbers: one
table, and words that could not tell apart several shows that are a walk
and one animation. `Start` follows `StartPictureShow` of the second wave;
the specification's word for it, the title, is not in the C.

The script found nothing wrong, all 14 functions of the sample were as
their rows say, and the applying session changed no name: the five readers
had made their words one before they handed in.

The sixth wave, the same day (`0x8004571C` to `0x800512C4`, 144
functions: 130 named, 14 read and not named):

| What | Names | Named |
|---|---|---:|
| The battle's menu of commands: its steps, one after another, by the table at `0x800B312C` | `BtlMenuKindOpen`, `BtlMenuSpellStep`, `BtlMenuItemDisp`, `BtlMenuTargetOpen`, `BtlMenuWalkStep`, `BtlMenuCursorMove`, `BtlMenuFrameStep` and others | 34 |
| The battle's two windows and what is written in them | `BtlWindow0Set`, `BtlWindowsMoveWait`, `BtlActionStringDisp`, `BtlBlackLinesRun` and others | 19 |
| The amounts shown over the slots | `BtlAmountRecordStart`, `BtlAmountRecordsStep` and six more | 8 |
| The party's panel | `BtlPartyPanelDisp` and three more | 4 |
| The battle's messages, round the developers' `BtlEndMessageDisp` | `BtlMessageDisp`, `BtlWinMessageDisp`, `BtlLevelUpGainsDisp`, `BtlMessagePadWait` and others | 13 |
| The enemies' choice of action in the battles below `0xEB` | `BtlEnemyActionSet`, `BtlEnemyActionPick`, `BtlPartyTargetHpPick` and others | 11 |
| The party's choice of action when the program makes it | `BtlPartyActionSet`, `BtlPartyOneChoiceSet`, `BtlPartyNeedSpellSet`, `BtlActingSpellListMake` and others | 39 |
| The shop's first two functions | `ShopRun`, `ShopOpen` | 2 |

`Disp` in these names is the developers' own word, from `BtlEndMessageDisp`:
a function that writes text on the screen ends in it. Four names that
readers had ended in `Show` were changed to match (`BtlMenuKindDisp`,
`BtlMenuSpellDisp`, `BtlMenuItemDisp`, and the fifth wave's
`BtlMenuPresetRowDisp`). `Show` stays where it is the noun: the battle's
shows.

What else the applying session changed: six names of the windows that one
reader had moved under `BtlMenu` while another moved the other way (the
windows hold messages as well as the menu, so they are `BtlWindow...`);
`BtlWindow1IconDisp` and `BtlWindow1IconClose`, proposed with "Auto", a
word for what the icon means that the C does not give; and
`BtlEnemyFourIdsCheck`, proposed as `BtlEnemy2F383E3FCheck`. The script
found nothing wrong, and all 12 functions of the sample were as their rows
say.

The seventh wave, the same day (`0x800512C4` to `0x8005B568`, 180
functions: 153 named, 27 read and not named), left the battle:

| What | Names | Named |
|---|---|---:|
| The shop, step by step like the battle's menu | `ShopStepsRun`, `ShopBuySellStep`, `ShopMemberStep`, `ShopItemStep`, `ShopDealRun`, `ShopMoneyDisp`, `ShopClose` and others | 34 |
| The game's side of the sound | `MusicPlay`, `MusicLoadStart`, `MusicStop`, `VoiceSet`, `VoiceStart`, `VoiceStep`, `SoundPlay`, `SoundMapSet` and others | 13 |
| The game from its start, and a frame of the map | `GameStart`, `GameRun`, `GameEnter`, `MapFrameStep`, `MapFrameEnd`, `MapPadStep`, `MapPartyStep`, `MapFadeOut`, `MapStatusDisp` and others | 24 |
| The story's bits | `StoryBitSet`, `StoryBitClear`, `StoryBitCheck` and two more | 5 |
| The characters' items, worn items, spells and levels | `ChrItemGive`, `ChrItemRemove`, `ChrWornItemSet`, `ChrItemListSort`, `ChrSpellUseGet`, `ChrLevelSet` and others | 24 |
| The map: changing it, its colour, its attributes | `MapChange`, `MapPartyPlace`, `MapBgChangeStart`, `MapColSet`, `MapColFadeStart`, `MapAttributeRun` and others | 15 |
| The message outside the battle | `MessageOpen`, `MessageDisp`, `MessageLineDisp`, `MessageFaceSet`, `MessageControlRun`, `MessageCodesConv` and others | 17 |
| The script's frame | `ScriptLabelRun`, `ScriptFrameStep`, `ScriptMusicWait`, `ScriptCommandMessageDisp` and others | 8 |
| The text grid's helpers outside the battle | `CellTextDisp`, `CellNumberSet`, `CellGlyphSet` and five more | 8 |
| Helpers under the game | `line_path_make`, `layer_mix_bits_switch_on`, `layer_mix_bits_switch_off`, `font_string_walk`, `font_string_word_get` | 5 |

The sound is spelled two ways on purpose. `sound_`, `xa_` and `cd_track_`
are the code that works Sony's library and the drive, with its own
numbers. `Music`, `Voice` and `SoundPlay` are the game's side: they take
the numbers the script, the map and the battle hold and turn them into
the other's. `voice` is the developers' word (their line `voice=0`).

"Story" in `StoryBitSet` is the readers' word for the bits at
`g_StoryBits`, which the script and the movies set and test; the C has no
word for them.

`MessageCodesConv` (`0x8005AC44`) is the function whose debug line reads
"ERR:MessageConv( code=%04x ) can't convert.". That line gives it no
`original` name, by the rule above. Its coined name says what it was read
to do, turn a message's codes into a string, and it is one word away from
the developers' text.

Three of the developers' debug lines stand in `MapPartyStep`
(`0x80056248`), each printed just before something is done and with no
value: "PartyTalkCheck1" before the one call of `0x80067D1C`,
"MMENU_ACTSEL1" before a call of `0x8006A024`, and "Status Indication1"
before the party's status is asked for. They name no function by the
rule, and they are given to the readers of those two functions as the
developers' words for what is called.

The script found nothing wrong, all 12 functions of the sample were as
their rows say, and the applying session changed no name.

The eighth wave, the same day (`0x8005B568` to `0x80066E64`, 177
functions: 171 named, 6 read and not named), read the script:

| What | Names | Named |
|---|---|---:|
| The script's commands, the entries of the table at `0x800B333C` | `ScriptCommandLabelJump`, `ScriptCommandLabelCall`, `ScriptCommandStoryBitsSet`, `ScriptCommandMoneyAdd`, `ScriptCommandChrItemGive`, `ScriptCommandPartyJoin`, `ScriptCommandMapChange`, `ScriptCommandBtlRun`, `ScriptCommandShopRun`, `ScriptCommandMoviePlay`, `ScriptCommandMusicPlay`, `ScriptCommandVoiceSet`, `ScriptCommandColMix` and others | 92 |
| The script's reader and its message helpers | `ScriptErrorHang`, `ScriptPointerAdvance`, `ScriptListCount`, `ScriptPagesRun`, `MessageAnswerRun`, `MessageNoticeDisp` and others | 17 |
| The script actors' rows | `MoveChrInit`, `MoveChrRowInit`, `MoveChrViewStep`, `MoveChrScaleStep`, `MoveChrFollowStart`, `MoveChrPartyJoin`, `MoveChrPartyLeave` and others | 16 |
| The party's and the map's objects' records, their pictures and animations | `MapPartyRecordsInit`, `MapObjectsInit`, `MapPartyAnimationStep`, `MapObjectsAnimationStep`, `MapAnimationSet`, `MapSpriteDataReload` and others | 39 |
| The party's places | `PartyPlaceFind`, `PartyPlacesSwap`, `PartyJoinPlaceGet`, `PartyLevelLeastSet` | 4 |
| Others | `ChrRecordsReset`, `value_goal_step`, `MoveChrError11Report` | 3 |

A command's name is `ScriptCommand` and then the name of what it calls,
where it calls one function of a named group (`ScriptCommandMapChange`
calls `MapChange`, `ScriptCommandColMix` calls the developers' `ColMix`).
A command that goes on at a label when something holds ends in `Check`;
one that goes on at a label in any case ends in `Jump`. The story's bits
that some commands work on are named by the number the command adds to
each byte of its list (`ScriptCommandStoryBits580Set`): the C does not say
what those bits stand for. Four commands that only step over their
operands are named by their entry's number, as the battle's shows are
(`ScriptCommand36Skip`, `ScriptCommand37Skip`, `ScriptCommand4ASkip`,
`ScriptCommand4BSkip`).

`ScriptErrorHang` is where the reader stops on a bad command: it hands the
reader's state to the empty function at `0x80013E98`, makes the assertion
call "TextError!" in the developers' file `LNS_INTP.C`, and loops for
ever. `ScriptErrorReport` (`0x8005F1C0`) is empty, and is named from what
its nine callers do just before they call it: each stores an error number
in `g_ScriptErrorNumber`.

Two readers began with words of their own for the same things and each
moved to the other's at the same moment, and then back; they said so with
the times. The applying session took the words both files hold now. The
script found nothing wrong, all 13 functions of the sample were as their
rows say, and no name was changed.

The ninth wave, the 14th (`0x80066E64` to `0x800734F0`, 122 functions:
119 named, 3 read and not named), read the map round the developers'
`MapEffectSet` and `OverCheck`, and the menu of the field:

| What | Names | Named |
|---|---|---:|
| The map: its party's ring of spots, its objects' thinking, its view, loading it, its attributes | `MapPartyRingPush`, `MapPartyRingFollow`, `MapObjectsThink`, `MapObjectWanderStep`, `MapPartyTalkCheck`, `MapViewStep`, `MapSet`, `MapLoad`, `MapAttributeFreeCheck`, `MapAttributeClassGet` and others | 39 |
| The menu of the field, run by `MenuRun` through its 23 pages, the table at `0x800B59F8` | `MenuRun`, `MenuOpen`, `MenuClose`, `MenuFrameStep`, `MenuPagesRun`, `MenuCommandStep`, `MenuItemUseStep`, `MenuSpellStep`, `MenuWornItemStep`, `MenuChrRecordStep`, `MenuPresetStep`, `MenuBtlPlacesStep`, `MenuSaveStep` and others | 79 |
| Others | `ScriptChange` | 1 |

`Menu` alone, not `MapMenu`: the start screen and the end of a battle run
it too. Its pages are named by what each is for where the C shows it
(`MenuWornItemStep`, `MenuSaveStep`), and by number where it does not:
`MenuPage7Step` is a choice of three of which two are the card's and the
third is not, and was proposed as `MenuCardCommandStep`.

Two names take words from the developers' text that names no function by
the rule. `MapPartyTalkCheck` (`0x80067D1C`) is the one call after their
line "PartyTalkCheck1"; it looks for the map object the party's first
record faces and runs what that object has. `MapTblGet` (`0x80068618`)
gives the map's 16-byte record that `MapEffectSet` takes as its first
parameter, the one their line prints as `tbl=`. Both are `coined`, and
their rows say where the word comes from. "MMENU_ACTSEL1" gave the menu its
first word.

The script found nothing wrong, all 10 functions of the sample were as
their rows say, and the applying session changed one name, the page above.

The tenth wave, the 14th (`0x800734F0` to `0x8007EEC0`, 129 functions:
124 named, 5 read and not named). None of it is the battle's own turn,
though the first split filed it as `battle.c` and `battle_b.c`:

| What | Names | Named |
|---|---|---:|
| The menu's card, load and option pages | `MenuSaveWrite`, `MenuLoadStep`, `MenuCardListDisp`, `MenuOptionStep` and others | 15 |
| The menu run on its own, for the start screen and after a lost battle | `MenuNoWalkRun`, `MenuNoWalkOpen`, `MenuNoWalkClose`, `StartMenuPageRun` | 4 |
| The use of an item or a spell from the menu | `MenuUseRun`, `MenuUseTargetRun`, `MenuUsePlaceNameRun`, `MenuUseMusicRun`, `MenuUseMovieRun` and others | 26 |
| The face list | `FaceListInit`, `FaceListPartySet`, `FaceListLoad`, `FaceListSwap` and four more | 8 |
| What the map's objects do: their thinks, walks, turns, and what happens when the party touches one | `MapObjectPartySeenCheck`, `MapObjectTouchRun`, `MapObjectRandomWalkStart`, `MapObjectFacingTurnStep`, the walkers `MapObjectPartyLook00Step` to `MapObjectPartyLook0CStep` and `MapObjectPartySeen01Step` to `MapObjectPartySeen06Step`, and others | 42 |
| A battle run from the map | `MapBtlRun` | 1 |
| The battle's shows below number `0x97` | `BtlShowNumberRun`, which picks a show by its number; `BtlShow01Run` to `BtlShow0BRun`; `BtlShowOpen`, `BtlShowClose`, `BtlEffectShotRun` and others | 21 |
| Helpers under the game | `value_div`, `rng_range_draw`, `pair_list_sort`, `card_save_sum_get`, `card_save_sum_check`, `card_save_records_head_read`, `profile_draw_flip` | 7 |

What else the applying session changed: `MenuNoWalkRun` and its two
neighbours, proposed with "Alone", a word that does not say what is
different (the party does not walk onto the screen, and the request is
returned, not acted on); `BtlShowNumberRun`, proposed as `BtlShowRun`, one
letter from `BtlShotRun`; and the eighth wave's `ScriptCommandBtlRun`,
now `ScriptCommandMapBtlRun`, because the function it calls is
`MapBtlRun`. The script found nothing wrong, and all 12 functions of the
sample were as their rows say.

The eleventh and last wave, the 14th (`0x8007EEC0` to `0x8008B388`, 112
functions: 106 named, 6 read and not named), read the battle's shows below
`0x97` and the machinery all shows run on. The first split's `menu.c` and
`save.c` here hold no menu and no saving: it is all the battle's shows.

| What | Names | Named |
|---|---|---:|
| The shows below `0x97`, by number | `BtlShow0CRun` to `BtlShow56Run` | 52 |
| Their helpers | `BtlShowPlacesPlay`, `BtlEffectCopyTargetPlay`, `BtlShowRecordTravelRun`, `BtlPartyRightMove`, `BtlShakesRun` and others | 12 |
| The amount show, the enemies' fall, the mix flash | `BtlAmountShowStart`, `BtlAmountShowCheck`, `BtlAmountShowRun`, `BtlEnemyFallShow`, `BtlMixFlashSet`, `BtlMixFlashStep` | 6 |
| The machinery of a show: its frame, sounds, wave, colour pulse, records, the tile layer's patterns, travel along a path | `BtlShowFrameStep`, `BtlShowSoundSet`, `BtlShowWaveSet`, `BtlShowPulseSet`, `BtlShowAnimationPlay`, `BtlEffectLoad`, `BtlTilePatternPut`, `BtlTravelRun` and others | 33 |
| Helpers under the game | `ratio_4_5_mul`, `ratio_29_32_mul`, `arc_path_make` | 3 |

One function that is the entry of several shows is named by the lowest
number, and its row lists the others (`BtlShow11Run` is shows `0x11`,
`0x12`, `0x24` and `0x29`). What the applying session changed:
`BtlShowRecordTravelRun`, proposed as `BtlBankShotRun`. It makes a show
record from a block of sprite data and runs `BtlTravelRun`, not
`BtlShotRun`, and "bank" was one reader's word for what the register
calls sprite data. The script found nothing wrong, and all 10 functions of
the sample were as their rows say.

With this wave every function of the game's own code has been read.


## Global data, first wave (2026-10-14)

Five readers read the 200 globals the game's code uses most, 73% of all
its uses of a global, in five groups by address. 193 were named; 4 are
fact rows (written and never read, or a pair of words from two different
pages); 3 carry the developers' own labels from their debug text and were
put to the owner, who ruled that such a label is taken, capitalised after
`g_` (`g_MapMix`, `g_MapEffect`, `g_Voice`).

| Group | Addresses | What it is | Named |
|---|---|---|---:|
| 0 | `0x80012D10` to `0x800B8B6C` | pad actuators, the shop's cursors, buffers, options, play time | 40 |
| 1 | `0x800B8B74` to `0x800B8C68` | the battle's core: acting slot, target, action, party and enemy counts, the script reader | 40 |
| 2 | `0x800B8C74` to `0x800B8D84` | the battle's walk grid, the map's number, view and colour, the battle menu | 40 |
| 3 | `0x800B8D88` to `0x800B8F70` | the battle's choices, the shop, actors moved by script, the field menu | 38 |
| 4 | `0x800B8F74` to `0x800E5588` | the field menu's lists, the battle's 14 slots and windows, the shows | 38 |

The linker's second names for a global at the same address (`_blk`, `_s`,
`_h` in `config/alias_syms.txt`) took the global's name with their own
ending. The menu overlay reads one of these globals and calls it
`g_PadButtons` as the main program does. The functions' rows in the
register call the named globals by their names.

## Global data, second wave (2026-10-14)

The next 200 globals by use, 51% of the uses of a global still named for
its address after the first wave, were read the same way. 195 were named;
2 are fact rows (only ever set to 0, and passed only to parameters that are
not read). 3 are the variables of published source code the game carries,
gzip's `mask_bits` (`0x800AF608`) and the work arrays `h` and `d` of
Okumura's spline routines (`0x800B8980`, `0x800B8984`). They were put to
the owner, who ruled that such a variable takes its published name,
capitalised after `g_`: `g_MaskBits`, `g_MthH`, `g_MthD`.

| Group | Addresses | What it is | Named |
|---|---|---|---:|
| 0 | `0x1F800020` to `0x800B8928` | the scratchpad's packet words, the pad's actuators, the background's buffers, sprite and spell tables | 39 |
| 1 | `0x800B8936` to `0x800B8B9A` | the story bits, the save block, the pad, inflate's pointers, the heap, the font, slot 5's roaming | 36 |
| 2 | `0x800B8BA8` to `0x800B8D30` | the battle's winnings and walk, the map's limits, view and colours | 40 |
| 3 | `0x800B8D3C` to `0x800B8F2C` | the battle menu's targets, the shop, the message window, the field menu | 40 |
| 4 | `0x800B8F30` to `0x800E5568` | the field menu, the battle's per-slot status rounds and show places, pad records | 40 |

The globals of the main program that the menu overlay reads take the same
names there (`g_PadButtons`, `g_MovieSkipAllowed`). Old notes kept on a
named global's row now call functions and globals by today's names.

## Global data, third wave (2026-10-14)

The next 200 globals by use, each used 4 to 8 times, were read the same
way. 193 were named and 7 are fact rows (written and never read, two
counters nothing reads, and two strings: a developers' file name in
assertions and a printed message). After this wave 92% of the game code's
uses of a global read by name.

| Group | Addresses | What it is | Named |
|---|---|---|---:|
| 0 | `0x1F800000` to `0x800B8649` | the background's slots, pages and colours, item and attack tables, the pad's actuator lists, the screen's fades | 38 |
| 1 | `0x800B864A` to `0x800B88BC` | the layers' grey and mix, the tile layer, the console, inflate's window position, the screen's size | 40 |
| 2 | `0x800B88C8` to `0x800B8C48` | the background's blocks, the CD's directory, inflate's bit buffer, the heap, XA, the font | 40 |
| 3 | `0x800B8C60` to `0x800B8F88` | the map's view shake and second view, sound and voice numbers, the message window, the menu's options | 36 |
| 4 | `0x800B8FB4` to `0x800E5590` | the menu's use lists, ordering tables, sprite page maps, the tile wave, the pad's buffers, the shows | 39 |

Five are variables of published source and take their published names
under the owner's ruling of the second wave: inflate.c's `bb`, `bk` and
`wp` (`g_Bb`, `g_Bk`, `g_Wp`) and `spline2.c`'s arrays `x` and `y`
(`g_MthX`, `g_MthY`). Three were put to the owner. Two heap words the
developers' line prints as `max=` and `last=` would have been `g_Max` and
`g_Last` by the letter of the labels ruling; the owner ruled that a bare
label takes the group first (`g_XmallocMax`, `g_XmallocLast`). A list that
holds the same bytes as `g_MoveChrMoveToList` is `g_MoveChrMoveToListCopy`:
the owner ruled that the twin ruling for functions holds for data.

## Parameters and locals, the pilot (2026-10-14)

One reader named the 70 machine names (`argN`, `temp_`, `var_`) of the 15
functions of `misc_j.c`, the map's fades and the text cells, under the
owner's rulings of the day (lower_case, the C as the record). None was
kept. Sony's functions took the two parameter names Sony's own prototypes
give (`SetVideoMode`'s `mode`, `_card_clear`'s `chan`); the rest of Sony's
prototypes give types only.

## Parameters and locals, the main program (2026-10-14)

Fifteen readers, five at a time, named the parameters and locals still
named by the decompiler in the game's 802 functions of the main program
that had any: 2,286 names, about 10,000 uses, 8 kept with a reason
(parameters nothing reads, and four values the C only copies). Each set of
five was applied and proven by the build before the next began, so later
readers read the earlier names in the C. The functions of the game that
stand in files named for Sony's library (`psyq_gpu_b.c`) were read with
the game's.

A variable the C uses for two values in turn is named for both
(`count_then_i`); a copy the match needs is named for what differs
(`kept_`, `old_`, `next_`). Afterwards the prototypes at the top of each
file took their definitions' parameter names (1,183 names), by position;
their types were not touched.

The sweep that followed (four readers) named the last placeholder names
inside the game's functions: register names written by hand while matching
(`a0`, `v0`, `s0`), the permuter's (`new_var`, `ra_e_0`) and placeholders
(`temp`, `tmp`, `arg_80`): 494 names in 291 functions, 9 kept with the
earlier readers' reasons. A register binding keeps its `asm("a0")`; the
variable bound takes a name for what it holds. The prototypes took the new
parameter names as before (76).

## Structs, the pilot: the battle record (2026-10-14)

The first split defined each struct in the file that needed it, as a view
naming only the fields that file used: 468 typedefs under 311 names, 65 of
them 0x40 bytes long. A reader went through every 0x40-byte view and the
code using them and wrote the battle record's one layout, with what each
field holds and who writes it: `BtlRecord` (`btl.h`), whose bytes +4 to
+0x27 are the sprite list's entry, `SpriteEntry` (`sprite.h`), which the
battle copies whole into the list every frame. The views did not agree on
the words at +8 and +0xA; the reader settled them from the drawing code:
+8 is the depth the sprites are sorted by, +0xA the y they are drawn at.

The 58 views of the record and the 2 of its sprite entry in 13 files were
replaced by the two types, each access found by the compiler (the true
fields renamed for one compile, so every access through an old view
failed with its line) and rewritten to the true field; 67 accesses keep
the type they had through `AS(type, field)`. Nine views were other memory
of the same size: the characters' records and Sony's graphics function
table. All three programs build byte-identical.

## Structs, the first families (2026-10-14)

Three readers worked out three families the same way, and the views were
replaced file by file, each step proven by the build:

| Type | Header | Bytes | What it is | Views replaced |
|---|---|---:|---|---:|
| `MapRecord` | `map.h` | 0x3C | the animated records of the map, the shop, the menus and the messages (the party's records and twins, the objects' twins, the script actors) | 26 |
| `MapObject` | `map.h` | 0x60 | the map's objects (`g_MapObjects`), a `MapRecord` at +0 and the object's own words | 15 |
| `MapBox` | `map.h` | 4 | an object's box: dx, dy, half width, half height | 7 |
| `BtlStats` | `btl.h` | 0x3C | the battle's fourteen slots' stats (`CharStats`) | 11 |
| `ChrRecord` | `chr.h` | 0x40 | the characters' own records (`g_ChrRecords`), which the battle copies its stats from | 7 |

Their local sprite views became `SpriteEntry`. `MapRecord`'s first two
bytes and `MapObject`'s words +0x4C and +0x54 hold different things for
different code (an on byte to the party's code, a kind to the objects'),
and `BtlStats` holds the party's or an enemy's words at +0x1A..+0x2D: those
are unions, and the member an access takes is the one its function uses.
A wrong member compiles to the same code, so the build cannot tell; the
picks were read afterwards and eleven were corrected (the byte at +1 is a
definition number where the code multiplies it by 0x26). The menu's
10-byte text entry that the first split had also called `SpriteEntry`
was renamed `MenuTextEntry`, for what its comments say it holds.

After this wave 333 of the 468 typedefs of the first split are still in
the C.

## Structs, the subsystem passes (2026-10-14)

Four readers took every typedef still in the game's files, by subsystem
(the battle, the map and the script, the field menu with the shop and the
UI, loading with the background and tile drawing), and gave each one of
the existing types, a new type shared with its other views, or a reason
to leave it. Their files were checked against one another first: two
types the battle and the menu passes had both defined were kept once, and
a view that is two tables' rows at once was kept for a person.

59 new types in 20 headers, among them the script's rows (`MoveChrRow`,
`script.h`), the map's object definitions (`MapObjectDefinition`), the
menu's option entries (`menu.h`), the shop's lists (`shop.h`), the save's
head and the map's record of the saved game (`game.h`), the sprite data's
piece, pose and step rows (`sprite.h`), gzip's `Huft` (`inflate.h`), the
CD's directory rows (`cd.h`) and the GPU packets the game writes
(`packet.h`). The views were replaced file by file; a file whose views did
not all keep the match was redone one view at a time, keeping those that
did. 122 typedefs left the C in this wave.

Three lessons. A true field takes the signedness most of its views read:
`PathPoint`'s words were first signed, and even `AS(u16, path[i].x)`
changed the arithmetic of two shows' indexing; six of its eight views read
them unsigned. While a view is replaced, it becomes a scratch copy of its
true type with renamed fields, so that only its own accesses fail to
compile: renaming the true type's fields made accesses of other code that
use the same type look like the view's. And the layout checks earned their
place at once: a field called `size` and an array inside a union both
broke a header, and the size check stopped the compile.

## Structs, Sony's library (2026-10-14)

A reader took the 165 struct typedefs of Sony's files and the Sony types
the game's files defined again. 31 types, one definition each, are in
`include/psyq_private.h`, under Sony's names where Sony's own headers give
one (`CdlATV`, `StHEADER`, `device_table`, `iob`) and otherwise under the
tree's (`SsScore`, `McrdGlobal`, `PadInfo`, `SpuRXX`); they replace 113
per-file copies. Their fields are written as declared, function pointers
and all. 40 copies of `OT_TAG` and `PADTYPE` that nothing named were
removed; five copies are types Sony's public headers already give the
build (`SpuVolume`, `SpuReverbAttr`, `ProgAtr`) and wait for a person, as
do two drawing records of the battle's code in `psyq_gpu_b.c`.

The rewrite learned four things on the way: a name before a member's dot
may end in a digit (`D_800EA960.unk50`), not only a number; GCC 2.8
reports the first unknown member of an expression and skips the rest, so
the compile and the rewrite repeat until nothing is left; a view named
like its true type (a file's own `PadInfo` beside the header's) waits
under another name until its turn; and the scratch copy of a type is
included after the real headers, whose other types it may name.

After this, 55 struct typedefs are left in the C, of the first split's
468: 26 views whose code reads the memory in a way the true layout does
not reproduce, or that are two memories at once, wait for a person; the
rest are local buffers the readers left with a reason, and the five
Sony public types.

## Structs, the views done by hand (2026-10-14)

Of the 26 views left for a person, 15 are done, each proven by the build:
the script's seven `MoveChrRow` views and the message answers (once each
header includes the headers of the types its fields point at), the two
views of the saved game's map record (their `padA` is the address of the
embedded ring entry), the CD's directory record (its name a one-element
array, the bytes running on past the record), the party panel's record
(its nested values flattened onto `BtlPartyPanelMember`), the window
pieces (`SpritePiece`: a constant stored into a field takes no cast, and
the cast had changed the code), the amount's three digit pieces (a new
`BtlAmountDigits`, an array of `SpritePiece`), and the second row of the
field menu's items, which is element 6 of an array of `BtlMenuItem`
(`BtlMenuItemRun`): indexing the item pointer itself made GCC form the
element's address before the access, where the old view folded the 0x48
into each access's offset.

The battle's windows followed: each window's pair of sprite list entries
is a `BtlWindowSprites` (the outline's and the box's `SpriteEntry`), and
the window templates are `SpriteEntry`s. A store of an address into an
entry's address word is written as a store of the value converted
(`= (s32)&pieces[i]`): casting the field instead changed GCC's code.

The last nine followed: the tile and palette animations with their seeds
(a view and the view it embeds are replaced in one run), the three banks'
decode targets (`SpriteDataList`), the script rows' casts of their actor
(`row->actor->sprite.x`), the map objects' stretch of the sprite list (a
new `MapObjectSprites`, its objects' entries and their twins'), and the
field menu's `BattleEntity`, which was the type of two pointers to
different memory: `g_MenuRenderEntries` is a `SpriteEntry *` and
`g_MenuShapes` a `SpritePiece *`, each old field mapped by its offset to
an element and a field. The step lists the map's records read are
`SpriteStep`s and their animation tables `SpriteAnimationRow`s, where one
view had stood for both.

30 of the first split's 468 struct typedefs are left in the C, each for a
reason: local buffers the readers found no type for (strings, a debug
box, copies of a read-only table), one forcing type (`BtlMenuItemRun`),
two of Sony's drawing packets (`POLY_FT4`) and five copies of Sony's
public types.

