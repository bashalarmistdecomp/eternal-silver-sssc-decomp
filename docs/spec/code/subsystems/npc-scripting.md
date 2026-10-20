# NPC logic and event scripting (LNS_INTP)

This page covers the game's event scripts and what they drive on the field:
the script reader and its 97 commands, the movers that scripts steer (the
camera, the second scroll, the party and the script's own actors), and the
field code those commands share with the field frame: the map-cell events,
the map objects with their behaviours and step lists, the party's actors
and position ring, the tile-attribute probes, the message window's prompts
and notices, chests and shops. The last sections walk scenes of the disc's
own scripts.

## The files

Two of the developers' file names survive in the image, in assertion
messages. `LNS_INTP.C` is the file `ScriptErrorHang` (`0x8005B568`) names
("TextError!", line 0x66A); `LNS_MOVE.C` is the file `MoveChrAction`
(`0x800618B4`) names (line 0x3FF). Today's files:

| File | What it holds |
|---|---|
| `lns_intp.c` | 88 functions, `ScriptErrorHang` (`0x8005B568`) to `ScriptErrorReport` (`0x8005F1C0`): the error hang and report, the command pointer's step and list count, the answers, pages and notices, and the handlers of 74 of the command table's 97 entries |
| `lns_move.c` | 23 functions, `MoveChrInit` (`0x8005F1C8`) to `MoveChrSpriteDataSet` (`0x80061B8C`): the mover slots made live and stepped (walk, camera, second scroll, scale), their goals and follows, the party's join and leave, the move-command run and `MoveChrAction` |
| `script_run.c` | the reader's loop `ScriptLabelRun` (`0x80059FDC`) and the frames a script lets go by and waits on (`ScriptFrameStep` (`0x8005A1F0`)) |
| `script_move.c` | the handlers of 22 commands that drive the movers (0x32..0x3D, 0x3F, 0x40, 0x4A..0x51) |
| `move_command.c` | the ten move-script ops 0xF0..0xF9 of `g_MoveChrCommandFunctions` (`0x800B45F8`) |
| `step_command.c` | the ten step ops 0xF0..0xF9 of the map objects, `g_StepCommandFunctions` (`0x800B4620`) |
| `message_disp.c` | the message window, and command 2's handler `ScriptCommandMessageDisp` (`0x8005AB1C`) |
| `map_attribute.c`, `map_change.c`, `map_object.c`, `map_think.c`, `map_talk.c`, `map_walk.c`, `map_ring.c`, `map_party.c`, `map_animation.c`, … | the field code below |

Each file's header comment says what it holds and which first-split file
its code came from. Every function in them is C.

## Key functions

| Address | Name | Purpose |
|---------|------|---------|
| `0x80059FDC` | `ScriptLabelRun` | The event script's runner (script_run.c): runs the map's script from a label, calling entry (command word) of `g_ScriptCommandFunctions` (`0x800B333C`, 97 entries; §"The command table") for each command until `g_ScriptEnded` is set |
| `0x8005B568` | `ScriptErrorHang` | Where the script reader stops on an error (lns_intp.c): the empty `ScriptStateReport` (`0x80013E98`), `assert_fail("TextError!", "LNS_INTP.C", 0x66A)`, then a loop with no end. Command ids 0, 1, 6 and 0x1B are this function |
| `0x8005A884` | `font_string_walk` | Walks a packed string and hands each character code and each control to two functions (message_disp.c; §"The message codes") |
| `0x8005AC44` | `MessageCodesConv` | Packs a message of 16-bit codes into such a string (message_disp.c); prints "ERR:MessageConv( code=%04x ) can't convert." for each code from 0xA to 0xEFFF |
| `0x8005BD84` | `MessageNoticeDisp` | The field's one-line notice (lns_intp.c; §"The field's one-line notice"); no entry of the command table |
| `0x80061800` | `MoveChrCommandRun` | Runs the current move command of each busy mover row through `g_MoveChrCommandFunctions` (`0x800B45F8`, ops 0xF0..0xF9) (lns_move.c) |
| `0x800618B4` | `MoveChrAction` | The script actors' animation tick and draw, once a field frame (lns_move.c; §"The script actors' animation tick and draw"). The name is the developers': its assertion's message begins "MoveChrAction:" and names the file LNS_MOVE.C |
| `0x8006736C` | `MapObjectsThink` | One pass over the map's objects, each due row calling its behaviour from `vtbl_4658_table` (`0x800B4658`) (map_think.c; §"The map objects' think pass") |
| `0x800590C0` | `MapAttributeRun` | Acts on the map-cell event under the party's first record (map_attribute.c; §"The map-cell events") |

## The command table

An event-script command is a u16 id followed by its operands. The reader
`ScriptLabelRun` (`0x80059FDC`) calls entry `id` of
`g_ScriptCommandFunctions` (`0x800B333C`), 97 function addresses (ids
0..0x60), until `g_ScriptEnded` is set. The first split cut this one table
into data labels that still carry its names: `script_opcode_table`
(`0x800B335C`) starts at id 8, `script_cmd_table` (`0x800B33AC`) at 0x1C,
`move_cmd_table` (`0x800B340C`) at 0x34 and `event_trigger_table`
(`0x800B3474`) at 0x4E. An index into one of those labels is not a command
id. The entries, read from the image (handlers in `lns_intp.c` unless
marked):

| id | Handler | What it does |
|---|---|---|
| 0x00, 0x01, 0x06, 0x1B | `ScriptErrorHang` (`0x8005B568`) | no command: the error hang |
| 0x02 | `ScriptCommandMessageDisp` (`0x8005AB1C`, message_disp.c) | show a message |
| 0x03 | `ScriptCommandLabelJump` (`0x8005BF5C`) | go on at a label |
| 0x04 | `ScriptCommandLabelCall` (`0x8005C06C`) | call a label |
| 0x05 | `ScriptCommandLabelReturn` (`0x8005C0E4`) | return from a call |
| 0x07 | `ScriptCommandAnswerRun` (`0x8005C42C`) | a choice: two answers, or the paged form (§"Event-script command 7, SELECT, the paged form") |
| 0x08 | `ScriptCommandLabelPick` (`0x8005C470`) | go on at a label of a list, picked by an RNG draw, or at the next command |
| 0x09, 0x0A | `ScriptCommandStoryBitsSet` (`0x8005C504`), `ScriptCommandStoryBitsClear` (`0x8005C570`) | set, clear the story bits of a u16 list |
| 0x0B..0x0D | `ScriptCommandStoryBitsAllCheck` (`0x8005C5DC`), `ScriptCommandStoryBitsNoneCheck` (`0x8005C684`), `ScriptCommandStoryBitsAnyCheck` (`0x8005C72C`) | go on at the command's label when all, none or any of a u16 list's bits are set |
| 0x0E..0x12 | `ScriptCommandStoryBits580Set` (`0x8005C7D4`), `…580Clear` (`0x8005C844`), `…580AllCheck` (`0x8005C8B4`), `…580NoneCheck` (`0x8005C960`), `…580AnyCheck` (`0x8005CA0C`) | the same five on bits 0x580 + each byte of a byte list |
| 0x13..0x17 | `ScriptCommandStoryBitPairsSet` (`0x8005CAB8`), `…PairsClear` (`0x8005CB38`), `…PairsAllCheck` (`0x8005CBB8`), `…PairsNoneCheck` (`0x8005CC74`), `…PairsAnyCheck` (`0x8005CD30`) | the same on bit `(a − 0x58) × 0x80 + b` for each byte pair `a, b` |
| 0x18, 0x19 | `ScriptCommandStoryBits480Set` (`0x8005CDEC`), `ScriptCommandStoryBits480Clear` (`0x8005CE5C`) | set, clear bits 0x480 + each byte |
| 0x1A | `ScriptCommandStoryBitPairs2620Set` (`0x8005CECC`) | set bit `b + 0x2620 + (k << 5)` for each byte pair `a, b`, with k = a − 0x35 from 0x65 up and a − 0x15 below: the chests' flag (§"Chests, examine cells and the item notice") |
| 0x1C, 0x1D | `ScriptCommandMoneyAdd` (`0x8005CF6C`), `ScriptCommandMoneyTake` (`0x8005CFF8`) | add, take silver, with the number notice |
| 0x1E | `ScriptCommandMoneyAboveJump` (`0x8005D078`) | go on at a label when the silver is more than an amount |
| 0x1F, 0x20 | `ScriptCommandChrItemGive` (`0x8005D0E0`), `ScriptCommandChrItemRemove` (`0x8005D164`) | give, take an item, with the item notice |
| 0x21 | `ScriptCommandChrItemHeldJump` (`0x8005D1CC`) | go on at a label when an item is held |
| 0x22 | `ScriptCommandChrLevelItemSet` (`0x8005D2F0`) | set a member's level, items or gear |
| 0x23, 0x24 | `ScriptCommandPartyJoin` (`0x8005D60C`), `ScriptCommandPartyLeave` (`0x8005D70C`) | a member joins, leaves |
| 0x25 | `ScriptCommandMapObjectsRemove` (`0x8005D9D4`) | remove map objects |
| 0x26 | `ScriptCommandMapChange` (`0x8005DB3C`) | change scene, then go on at a label |
| 0x27 | `ScriptCommandMapPartyPlace` (`0x8005DCBC`) | put the party on another spot of the same map |
| 0x28 | `ScriptCommandMapBgChangeStart` (`0x8005DD84`) | start a tile animation once the running one has ended |
| 0x29, 0x2A | `ScriptCommandMapFadeOut` (`0x8005DE88`), `ScriptCommandMapFadeIn` (`0x8005DF08`) | fade out, in |
| 0x2B | `ScriptCommandMapBtlRun` (`0x8005DFF0`) | a battle |
| 0x2C | `ScriptCommandMapViewShakeStart` (`0x8005E274`) | shake the view |
| 0x2D | `ScriptCommandMoviePlay` (`0x8005E2B4`) | play a movie |
| 0x2E | `ScriptCommandShopRun` (`0x8005E384`) | open a shop |
| 0x2F | `ScriptCommandPartyHpMpFill` (`0x8005E3D0`) | fill every member's HP and MP, with a sound and a colour flash |
| 0x30, 0x31 | `ScriptCommandMusicPlay` (`0x8005E494`), `ScriptCommandSoundPlay` (`0x8005E524`) | music, a sound |
| 0x32 | `ScriptCommandMoveChrCommandsStart` (`0x80061C54`, script_move.c) | start the move-script list of every live mover that has one and is not running it |
| 0x33 | `ScriptCommandMoveChrRowsAllEnd` (`0x80061CF8`, script_move.c) | end every mover but slot 1 |
| 0x34 | `ScriptCommandMoveChrRowActorSet` (`0x80061EAC`, script_move.c) | bind a mover to a map object, or spawn an actor for it |
| 0x35 | `ScriptCommandMoveChrRowsEnd` (`0x80062314`, script_move.c) | release movers |
| 0x36, 0x37, 0x4A, 0x4B | `ScriptCommand36Skip` (`0x800624DC`), `ScriptCommand37Skip` (`0x80062508`), `ScriptCommand4ASkip` (`0x800629B8`), `ScriptCommand4BSkip` (`0x800629E4`) (script_move.c) | step over the command and its byte list, nothing else |
| 0x38 | `ScriptCommandMoveChrCommandsSet` (`0x80062534`, script_move.c) | give a mover its move-script list |
| 0x39, 0x3A | `ScriptCommandMoveChrPartyFollowStart` (`0x80062640`), `ScriptCommandMoveChrPartyFollowEnd` (`0x80062680`) (script_move.c) | the followers walk home and follow the lead again; the followers are taken into movers 2..6 |
| 0x3B, 0x3C | `ScriptCommandMoveChrAttributeCheckOn` (`0x800628C4`), `ScriptCommandMoveChrAttributeCheckOff` (`0x800628E0`) (script_move.c) | the movers' tile check on, off |
| 0x3D | `ScriptCommandMoveChrCommandEndWait` (`0x80062984`, script_move.c) | wait until none of the movers a mask names is busy |
| 0x3E | `ScriptCommandFaceListSet` (`0x8005E610`) | load a scene's portraits |
| 0x3F, 0x40 | `ScriptCommandMoveChrViewFollowStart` (`0x800628F8`), `ScriptCommandMoveChrViewFollowEnd` (`0x8006293C`) (script_move.c) | the camera walks back and follows the lead again; the camera is taken into mover 0 |
| 0x41 | `ScriptCommandMapObjectsAdd` (`0x8005D7E0`) | spawn map objects |
| 0x42 | `ScriptCommandScriptChange` (`0x8005BFCC`) | read another script and go on at a label of it |
| 0x43 | `ScriptCommandChr1ChrBChange` (`0x8005D470`) | switch the lead between ids 1 and 0xB |
| 0x44, 0x45 | `ScriptCommandMapDrawAreaClose` (`0x8005DF58`), `ScriptCommandMapDrawAreaOpen` (`0x8005DFA4`) | close the picture in on the screen's middle, open it again |
| 0x46 | `ScriptCommandMapColSet` (`0x8005DDC4`) | the map's colour from a preset row |
| 0x47 | `ScriptCommandColMix` (`0x8005DE54`) | the developers' `ColMix` (`0x80059E04`) |
| 0x48 | `ScriptCommandMapPartyKindSet` (`0x8005E890`) | change `g_MapPartyKind` and rebuild the party's records |
| 0x49 | `ScriptCommandPlaceNameSet` (`0x8005E9D4`) | set `g_MapPlaceName` |
| 0x4C, 0x4D | `ScriptCommandMoveChrSecondViewFollowStart` (`0x80062A10`), `ScriptCommandMoveChrSecondViewFollowEnd` (`0x80062A54`) (script_move.c) | the second scroll walks back to where the camera puts it and follows the camera again; the second scroll is taken into mover 0x11 |
| 0x4E..0x51 | `ScriptCommandMixGroupSet` (`0x80062ACC`), `ScriptCommandMixGroupClear` (`0x80062C38`), `ScriptCommandMixGroupFadeStart` (`0x80062E00`), `ScriptCommandMixFadeWait` (`0x80062EBC`) (script_move.c) | the colour-ramp groups (§"Event-script command 0x4E, mover slots into a colour-ramp group") |
| 0x52..0x54 | `ScriptCommandVoiceSet` (`0x8005E9FC`), `ScriptCommandVoiceStart` (`0x8005EA44`), `ScriptCommandVoiceEndWait` (`0x8005EA8C`) | load, start and wait for an XA voice clip |
| 0x55 | `ScriptCommandMessageTimedSet` (`0x8005EB04`) | messages that wait for no button, on or off |
| 0x56 | `ScriptCommandMapPadDirectionsChange` (`0x8005EB98`) | turn the d-pad map inside a box |
| 0x57 | `ScriptCommandChrLevelGapAdd` (`0x8005EC30`) | raise a member to the first slot's level |
| 0x58 | `ScriptCommandPartyMpHalve` (`0x8005ECC8`) | halve the party's MP after a wait |
| 0x59 | `ScriptCommandChrSpellSet` (`0x8005EE30`) | put a spell in a member's list |
| 0x5A | `ScriptCommandMusicLevelSet` (`0x8005E5A4`) | the music's level ramp |
| 0x5B | `ScriptCommandPartyMpClear` (`0x8005EEE0`) | zero the MP, the max MP and the word at `+0x3C` of each member below id 0xC but 8 and 9 |
| 0x5C | `ScriptCommandStoryBitsItemsReset` (`0x8005EF58`) | the whole-party reset |
| 0x5D | `ScriptCommandMapScriptChange` (`0x8005F068`) | change scene and script |
| 0x5E | `ScriptCommandGameEnterRequest` (`0x8005F1A8`) | end the script and set `g_GameEnterRequest`, which the next field frame acts on |
| 0x5F, 0x60 | `ScriptCommandActuatorLevelsSet` (`0x8005D23C`), `ScriptCommandActuatorListStart` (`0x8005D294`) | the controller's actuators: set their two levels, start one of their command lists |

## Map objects and event scripts

What a map's objects and its event script are made from, read from the
C named in each line. text.md §"Arena locations" builds on the touch and
battle readings here.

### Event scripts

* The script id is byte 8 (`script`) of the map's 16-byte record
  (`MapTbl`): `MapLoad` (`0x80068A10`) calls `ScriptChange(rec[8])`
  (`0x80068D7C`). `resource_pair_find(g_ScriptResourcePairs, id)`
  (`0x80020C58`) gives the resource (62 scripts, ids 0..59, 100, 200).
  `ScriptResourceLoad` (`0x80020D20`) copies 0x9F80 bytes to `g_ScriptBase`
  = 0x801DCB80 (4-aligned, so offsets keep their address parity).
* **Label table**: label `n`'s entry is the u16 at byte `2n` of the base,
  and the label starts that many u16 words from the base (`ScriptLabelRun`
  (`0x80059FDC`): `g_ScriptCommandPointer = g_ScriptBase + entry × 2`; an
  entry of 0 is the error hang). The table ends where the lowest label's
  code begins. Every jump target is a label, so a linear sweep of the code
  area sees every command that can ever run. Labels 999 (scripts 7 and 8)
  point at the zero word after the last command, an end sentinel.
* **Command**: a u16 id and its operands; §"The command table" lists the 97
  handlers. Ids 0, 1, 6 and 0x1B are the error hang `ScriptErrorHang`
  (`0x8005B568`). The Japanese build's keyword numbers run one above these
  ids (§"The Japanese build's script keywords").
* Operand helpers: `ScriptPointerAdvance(n)` (`0x8005B5B8`) moves the
  pointer to the next multiple of `n` above it,
  `g_ScriptCommandPointer = (g_ScriptCommandPointer & −n) + n`;
  `ScriptListCount(1, s)` (`0x8005B5D8`) walks a byte list (step `s`) to its
  zero byte `t` and leaves the pointer at `(t | 1) + 1`; `(2, 1)` walks a
  u16 list to a zero word and leaves `t + 2`; `ScriptStringEndFind`
  (`0x8005C144`) returns the address after the next 0xFF (rounded up to even
  when asked). A conditional command keeps its jump label at `+2`
  (`ScriptCommandLabelJump` (`0x8005BF5C`) reads the u16 at
  `g_ScriptCommandPointer + 2`) and its list from `+4`.
* **Lengths** (c = the command's address; each from how far the named
  handler moves `g_ScriptCommandPointer`):

| ids | length | handler |
|---|---|---|
| 0x02 | to `font_string_walk` (`0x8005A884`)'s stop pointer + 1, rounded up to even. The walk stops at 0xFF, or early at a '$', or at a '\\' that comes from a dictionary word; most messages end with the '$' char 0x05 and no 0xFF | `ScriptCommandMessageDisp` (`0x8005AB1C`) |
| 0x03, 0x04 | 4: `[id][label]` (0x04 keeps c + 4 as its return point) | `ScriptCommandLabelJump` (`0x8005BF5C`), `ScriptCommandLabelCall` (`0x8005C06C`) |
| 0x05 | 2 | `ScriptCommandLabelReturn` (`0x8005C0E4`) |
| 0x07 | `[07][label][u8 start answer][pad]` + two 0xFF-ended strings, the second end rounded up to even | `ScriptCommandAnswerRun` (`0x8005C42C`) → `ScriptAnswerRun` (`0x8005C198`) or `ScriptPagesRun` (`0x8005C2C0`), one layout |
| 0x08..0x0A | 2 + u16 list | `ScriptCommandLabelPick` (`0x8005C470`), `ScriptCommandStoryBitsSet` (`0x8005C504`), `ScriptCommandStoryBitsClear` (`0x8005C570`) |
| 0x0B..0x0D | 4 + u16 list (label at +2) | `ScriptCommandStoryBitsAllCheck` (`0x8005C5DC`), `…NoneCheck` (`0x8005C684`), `…AnyCheck` (`0x8005C72C`) |
| 0x0E, 0x0F, 0x18, 0x19 | 2 + byte list | `ScriptCommandStoryBits580Set` (`0x8005C7D4`), `…580Clear` (`0x8005C844`), `ScriptCommandStoryBits480Set` (`0x8005CDEC`), `…480Clear` (`0x8005CE5C`) |
| 0x25, 0x41 | 2 + byte list | `ScriptCommandMapObjectsRemove` (`0x8005D9D4`), `ScriptCommandMapObjectsAdd` (`0x8005D7E0`) |
| 0x35..0x37, 0x4A, 0x4B | 2 + byte list | `ScriptCommandMoveChrRowsEnd` (`0x80062314`), `ScriptCommand36Skip` (`0x800624DC`), `ScriptCommand37Skip` (`0x80062508`), `ScriptCommand4ASkip` (`0x800629B8`), `ScriptCommand4BSkip` (`0x800629E4`) |
| 0x3E, 0x4E, 0x4F | 2 + byte list | `ScriptCommandFaceListSet` (`0x8005E610`), `ScriptCommandMixGroupSet` (`0x80062ACC`), `ScriptCommandMixGroupClear` (`0x80062C38`) |
| 0x10..0x12 | 4 + byte list (label at +2) | `ScriptCommandStoryBits580AllCheck` (`0x8005C8B4`), `…580NoneCheck` (`0x8005C960`), `…580AnyCheck` (`0x8005CA0C`) |
| 0x13, 0x14, 0x1A | 2 + byte-pair list | `ScriptCommandStoryBitPairsSet` (`0x8005CAB8`), `…PairsClear` (`0x8005CB38`), `ScriptCommandStoryBitPairs2620Set` (`0x8005CECC`) |
| 0x15..0x17 | 4 + byte-pair list (label at +2) | `ScriptCommandStoryBitPairsAllCheck` (`0x8005CBB8`), `…PairsNoneCheck` (`0x8005CC74`), `…PairsAnyCheck` (`0x8005CD30`) |
| 0x1C..0x1E | `((c + 2) & −4) + 8`: a byte (0x1E: the label) at +2, then a 32-bit amount at the next multiple of 4 above c + 2 | `ScriptCommandMoneyAdd` (`0x8005CF6C`), `ScriptCommandMoneyTake` (`0x8005CFF8`), `ScriptCommandMoneyAboveJump` (`0x8005D078`) |
| 0x1F, 0x20 | 4 | `ScriptCommandChrItemGive` (`0x8005D0E0`), `ScriptCommandChrItemRemove` (`0x8005D164`) |
| 0x21, 0x22, 0x24 | 6 | `ScriptCommandChrItemHeldJump` (`0x8005D1CC`), `ScriptCommandChrLevelItemSet` (`0x8005D2F0`), `ScriptCommandPartyLeave` (`0x8005D70C`) |
| 0x23, 0x28..0x2A, 0x2E, 0x30, 0x31 | 4 | `ScriptCommandPartyJoin` (`0x8005D60C`), `ScriptCommandMapBgChangeStart` (`0x8005DD84`), `ScriptCommandMapFadeOut` (`0x8005DE88`), `ScriptCommandMapFadeIn` (`0x8005DF08`), `ScriptCommandShopRun` (`0x8005E384`), `ScriptCommandMusicPlay` (`0x8005E494`), `ScriptCommandSoundPlay` (`0x8005E524`) |
| 0x33, 0x43..0x45, 0x47..0x49 | 4 | `ScriptCommandMoveChrRowsAllEnd` (`0x80061CF8`), `ScriptCommandChr1ChrBChange` (`0x8005D470`), `ScriptCommandMapDrawAreaClose` (`0x8005DF58`), `ScriptCommandMapDrawAreaOpen` (`0x8005DFA4`), `ScriptCommandColMix` (`0x8005DE54`), `ScriptCommandMapPartyKindSet` (`0x8005E890`), `ScriptCommandPlaceNameSet` (`0x8005E9D4`) |
| 0x52, 0x55..0x57, 0x59, 0x5A, 0x5F, 0x60 | 4 | `ScriptCommandVoiceSet` (`0x8005E9FC`), `ScriptCommandMessageTimedSet` (`0x8005EB04`), `ScriptCommandMapPadDirectionsChange` (`0x8005EB98`), `ScriptCommandChrLevelGapAdd` (`0x8005EC30`), `ScriptCommandChrSpellSet` (`0x8005EE30`), `ScriptCommandMusicLevelSet` (`0x8005E5A4`), `ScriptCommandActuatorLevelsSet` (`0x8005D23C`), `ScriptCommandActuatorListStart` (`0x8005D294`) |
| 0x26 | 8, or 0x12 when `u8[c+4] == 0` (the 11-byte row inline) | `ScriptCommandMapChange` (`0x8005DB3C`) |
| 0x27 | 4, or 0xE when `u8[c+2] == 0` (the 9-byte row inline) | `ScriptCommandMapPartyPlace` (`0x8005DCBC`) |
| 0x2B | 8: `[2B][label][row+1][restore][music][bgm]` | `ScriptCommandMapBtlRun` (`0x8005DFF0`) |
| 0x2C, 0x2D, 0x46, 0x50 | 6 | `ScriptCommandMapViewShakeStart` (`0x8005E274`), `ScriptCommandMoviePlay` (`0x8005E2B4`), `ScriptCommandMapColSet` (`0x8005DDC4`), `ScriptCommandMixGroupFadeStart` (`0x80062E00`) |
| 0x2F, 0x58, 0x5B, 0x5C | 2 | `ScriptCommandPartyHpMpFill` (`0x8005E3D0`), `ScriptCommandPartyMpHalve` (`0x8005ECC8`), `ScriptCommandPartyMpClear` (`0x8005EEE0`), `ScriptCommandStoryBitsItemsReset` (`0x8005EF58`) |
| 0x32, 0x39..0x3C, 0x3F, 0x40 | 2 | `ScriptCommandMoveChrCommandsStart` (`0x80061C54`), `ScriptCommandMoveChrPartyFollowStart` (`0x80062640`), `…PartyFollowEnd` (`0x80062680`), `ScriptCommandMoveChrAttributeCheckOn` (`0x800628C4`), `…AttributeCheckOff` (`0x800628E0`), `ScriptCommandMoveChrViewFollowStart` (`0x800628F8`), `…ViewFollowEnd` (`0x8006293C`) |
| 0x4C, 0x4D, 0x51, 0x53, 0x54, 0x5E | 2 | `ScriptCommandMoveChrSecondViewFollowStart` (`0x80062A10`), `…SecondViewFollowEnd` (`0x80062A54`), `ScriptCommandMixFadeWait` (`0x80062EBC`), `ScriptCommandVoiceStart` (`0x8005EA44`), `ScriptCommandVoiceEndWait` (`0x8005EA8C`), `ScriptCommandGameEnterRequest` (`0x8005F1A8`) |
| 0x34 | 4, or 0x10 when `u8[c+2] >= 0xB` (an actor of the script's own) | `ScriptCommandMoveChrRowActorSet` (`0x80061EAC`) |
| 0x38 | `[38][slot]` + move-script bytes from c + 3 to 0xF9 (F0-F2, F5, F6 = 2; F3, F4, F7 = 5; F8 = 4), the end rounded up past the 0xF9 to even | `ScriptCommandMoveChrCommandsSet` (`0x80062534`) |
| 0x3D | `(c & −4) + 8`: a 32-bit mask at the next multiple of 4 above c | `ScriptCommandMoveChrCommandEndWait` (`0x80062984`) |
| 0x42 | 6: `[42][label][script id]` | `ScriptCommandScriptChange` (`0x8005BFCC`) |
| 0x5D | 0x12: `[5D][label][script id][flag]` + the 11-byte row + a music byte | `ScriptCommandMapScriptChange` (`0x8005F068`) |

  The handlers of 0x03, 0x05, 0x26, 0x42 and 0x5D never leave the pointer
  on the next command (they go on at a label, return, or end the script),
  and 0x5E ends the script without moving it: for those the length is the
  operands they read, which is what a sweep of the code needs. 0x42 carries
  the script id as a third word: its handler reads the label at +2 and the
  byte at +4, and the command is 6 bytes (taken for a 4-byte jump, the next
  command would start in the middle of the id). 0x59's handler moves the
  pointer to c + 4 before its spell-row test, so it is 4 bytes on both
  paths. 0x4F and 0x52 step over their command word by calling another
  command's handler first: `ScriptCommandMixGroupClear` calls
  `ScriptCommandMixFadeWait` (0x51's) and `ScriptCommandVoiceSet` calls
  `ScriptCommandVoiceEndWait` (0x54's), each of which adds 2 to the pointer
  before it waits. With these lengths, all 62 scripts sweep to their end
  with every label on a command boundary (checked with station-tools).
* Script 200 (the select menu) has `0xFFFF` filler between entries, and
  script 51 has one stray `ff 00` after a '$'-terminated message at
  0x1896: the preceding command falls through into an invalid id and no
  label lies in the gap, so the bytes are unreachable.
* Command 0x49 (`ScriptCommandPlaceNameSet`, `g_MapPlaceName = u8[c+2]`)
  occurs only in scripts 25 (95, 138), 45 (164) and 200 (95).
* **Special scripts.** Script 0 (map 9, location 158 "Monster Test") holds
  29 battle commands (0x2B) over encounters 223-254 and jumps to script
  200; script 200 is the test room's select menu (jumps to 20 scripts).
  They are the two scripts in which `ScriptCommandAnswerRun` (`0x8005C42C`)
  runs command 7 in its paged form (§"Event-script command 7, SELECT, the
  paged form"). Both are retail data of a debug room that no other map's
  scene table enters.

### A map's object definitions come from its object set, not its map file

* Command 9 of the map resource's command list
  (`resource_map_enter_table_set` (`0x80020B8C`)) copies its payload to
  `MapEnterTableAddress()` (`0x80055E6C`) = 0x801E6B00, the address
  `g_MapEnterTable` holds: the scene table, 128 rows of 11 bytes
  (`MapEnterRow`), row 0 unused. `MapChange(idx)` (`0x8005922C`) reads row
  `idx`: `+0` the destination map, `+1` the object set, `+2` the fade, `+3`
  a sound played first (0 for none), `+4..+7` the view's limits in units of
  8 pixels, `+8..+10` the spot the party is put on (x, y, facing − 1). The
  bytes after it in the same payload are the 9-byte rows at 0x801E7080
  (`g_MapPartyPlaceRows`, `MapPartyPlace` (`0x80059730`), same-map moves)
  and the bg change table at 0x801E7500 (`g_MapBgChangeTable`). It is only
  ~2.8 KB: the definitions are not in it.
* `MapObjectsInit(set)` (`0x800655C8`) → `MapObjectsDataLoad(set)`
  (`0x80065418`): `res = u16 g_MapObjectsDataResourceNumbers[set]` (a table
  in the EXE, 0x134 bytes: sets 0..153, 0 = none), the resource is read,
  and `inflate_buffer` (`0x80021D6C`) unpacks `buf + u32 buf[0]` to
  `g_MapObjectDefinitions` (0x801E8900). `buf + u32 buf[1]` is a
  0-terminated list of 6-byte `{id, size, off}` rows
  (`MapObjectsSpriteDataRow`): the sprite data number a definition's byte 1
  names, the unpacked block's size in words, and its packed offset in words
  from the list (assets/sprites.md §"Map object sets"). The first 0x80 ×
  0x26 bytes are the 128 object definitions (`MapObjectDefinition`); the
  rest is the table at `g_MapObjectStepLists` (0x801E9C00). A dump of the
  live table in the opening's Burg scene, taken with station-tools (a
  custom decomp toolkit that will be released later), matches the unpacked
  definitions byte for byte.
* In the retail tables every scene row's object set equals its
  destination map id (or 0 = none): 97 maps ↔ 97 sets 1..153. A saved game
  takes the set from byte 0x1C of its map record instead, where
  `MenuSaveWrite` (`0x80074024`) stored `g_MapObjectsDataNumber`: `GameEnter`
  (`0x80059498`) hands it to `MapObjectsRestore` (`0x80065098`).
* `MapObjectsInit` (`0x800655C8`) makes definition `i` (the object's id)
  when `def[1] != 0` (its sprite data) and, if `def[2] != 0`, its story bit
  is clear (bit `def[2] + 0x580` for kind < 4, else `i + 0x680 +
  ((map − 0x65) << 7)`), through `MapObjectInit(row, twin, def, i)`
  (`0x800656CC`): kind = `(def[0] & 0x3F) + 1`, behaviour number =
  `(def[0] & 0x40) | def[0xC]`. The rows are filled from 0 with no gap.
* **Touch → what happens** (`MapObjectTouchFind` (`0x8007AB64`) →
  `MapObjectTouchRun` (`0x8007ACD8`)): rows of kind 4, 5 or 6 whose
  `behaviour & 0x3F != 3`; `def[0xB] == 0` does nothing. Kind 4 runs
  `ScriptLabelRun(def[0xB])` (`0x80059FDC`): `def[0xB]` is a label of the
  map's script (the kind-4 events in maps 113 and 123 are labels 100-109).
  Kinds 5 and 6 run `MapBtlRun(def[0xB], …)` (`0x8007AE58`): `def[0xB]` is
  the encounter id. Counted on the disc's object sets: 794 definitions of
  kind 5 (wandering monsters), only 3 of kind 6 (none with an event), 9
  kind-4 events.

## The Japanese build's script keywords

The Japanese release (`SLPS_013.97`) kept a `{const char *name; int opcode}`
table at its `0x800A642C` that the North American build does not have: the
script language's keywords and their opcode numbers. 90 keywords, then 25
aliases (alternate spellings). Opcodes 0x00–0x02 and 0x06 have no keyword.
The Japanese build is not part of this repository, so the table below is
given as it was read there and is not checked here.

| opcode | keyword | opcode | keyword | opcode | keyword |
|---:|---|---:|---|---:|---|
| `0x03` | TALK | `0x23` | SETPARAM | `0x42` | CHRON |
| `0x04` | JUMP | `0x24` | WITHIN | `0x43` | JUMPPACK |
| `0x05` | CALL | `0x25` | WITHOUT | `0x44` | CHANGE |
| `0x07` | RETURN | `0x26` | KILL | `0x45` | WIPEOUT |
| `0x08` | SELECT | `0x27` | EXIT | `0x46` | WIPEIN |
| `0x09` | RND | `0x28` | WARP | `0x47` | OFFSET |
| `0x0a` | SET | `0x29` | OPEN | `0x48` | COLMIX |
| `0x0b` | RESET | `0x2a` | FADEOUT | `0x49` | ATRSET |
| `0x0c` | AND | `0x2b` | FADEIN | `0x4a` | PLACESET |
| `0x0d` | NOT | `0x2c` | FIGHT | `0x4b` | MVLOCK |
| `0x0e` | OR | `0x2d` | SHAKE | `0x4c` | MVUNLOCK |
| `0x0f` | CHRSET | `0x2e` | EVENT | `0x4d` | MVEFFON |
| `0x10` | CHRRESET | `0x2f` | SHOP | `0x4e` | MVEFFOFF |
| `0x11` | CHRAND | `0x30` | FIGURE | `0x4f` | FADESET |
| `0x12` | CHRNOT | `0x31` | PLAY | `0x50` | FADEKILL |
| `0x13` | CHROR | `0x32` | SE | `0x51` | CHRFADE |
| `0x14` | MONSET | `0x33` | MVSTART | `0x52` | FADESYNC |
| `0x15` | MONRESET | `0x34` | MVEND | `0x53` | VOICELOAD |
| `0x16` | MONAND | `0x35` | MVSET | `0x54` | VOICE |
| `0x17` | MONNOT | `0x36` | MVKILL | `0x55` | VOICESYNC |
| `0x18` | MONOR | `0x37` | MVLOAD | `0x56` | FEEDSET |
| `0x19` | OPENSET | `0x38` | MVDELETE | `0x57` | CONFUSION |
| `0x1a` | OPENRESET | `0x39` | MVDATA | `0x58` | LEVEL |
| `0x1b` | BOXSET | `0x3a` | MVPARTYON | `0x59` | DRAIN |
| `0x1c` | TEXTERROR | `0x3b` | MVPARTYOFF | `0x5a` | MAGICSET |
| `0x1d` | SETMONEY | `0x3c` | MVSTOPON | `0x5b` | VOLUME |
| `0x1e` | TAKEMONEY | `0x3d` | MVSTOPOFF | `0x5c` | MPKILL |
| `0x1f` | CHKMONEY | `0x3e` | MVSYNC | `0x5d` | ALLRESET |
| `0x20` | SETITEM | `0x3f` | FACELOAD | `0x0b` | DEBUGEXIT |
| `0x21` | TAKEITEM | `0x40` | MVSCRON |  |  |
| `0x22` | CHKITEM | `0x41` | MVSCROFF |  |  |

Aliases (same opcode, alternate spelling): `BIT`=0xe, `CHR_SET`=0xf, `CHR_RESET`=0x18, `OPEN_SET`=0x19, `OPEN_RESET`=0x24, `WITH_OUT`=0x29, `F_O`=0x2a, `F_I`=0x32, `M_START`=0x33, `M_END`=0x34, `M_SET`=0x38, `M_DATA`=0x39, `M_PARTY_ON`=0x3a, `M_PARTY_OFF`=0x3b, `M_STOP_ON`=0x3c, `M_STOP_OFF`=0x3d, `M_SYNC`=0x3f, `M_SCROLL_ON`=0x40, `M_SCROLL_OFF`=0x45, `W_I`=0x25, `GET_CHR`=0x41, `PUT_CHR`=0x42, `JUMP_PAC`=0x0, `???`=0x99.

**Against today's command table.** Set beside §"The command table", the
keyword whose opcode is n + 1 names what retail event-script command n
does, for nearly every id from 0x02 to 0x5C. This is an observation about
the names. It does not show that the Japanese build's command table is one
entry longer, or that its keyword table counts from another base.

* Agreeing: 0x02 `ScriptCommandMessageDisp` (`0x8005AB1C`) TALK; 0x03, 0x04 JUMP, CALL;
  0x07 `ScriptCommandAnswerRun` (`0x8005C42C`) SELECT; 0x08 `ScriptCommandLabelPick` (`0x8005C470`) RND
  (a label picked by an RNG draw); 0x09..0x0D SET, RESET, AND, NOT, OR (the
  story bits of a u16 list); 0x0E..0x12 CHRSET..CHROR (bits 0x580 + a byte,
  the story bit `MapObjectsInit` (`0x800655C8`) tests for map objects of
  kinds 1 to 3); 0x13..0x17 MONSET..MONOR (bit `(a − 0x58) × 0x80 + b`,
  which for `a` = the map's number is `b + 0x680 + ((map − 0x65) << 7)`,
  the story bit of definition `b` of kind 4 and up); 0x18, 0x19 OPENSET,
  OPENRESET (bits 0x480 + a byte, the bits `MapBgChangeStart` (`0x80059900`)
  numbers a bg change's story bit by) and 0x28 `ScriptCommandMapBgChangeStart` (`0x8005DD84`) OPEN;
  0x1A `ScriptCommandStoryBitPairs2620Set` (`0x8005CECC`) BOXSET (the bit `MapObjectInit`
  (`0x800656CC`) tests for a chest, kind 7); 0x1B `ScriptErrorHang` (`0x8005B568`)
  TEXTERROR (its assertion message is "TextError!"); 0x1C..0x21
  SETMONEY..CHKITEM; 0x22 SETPARAM; 0x23 WITHIN; 0x24 WITHOUT; 0x25 KILL;
  0x26 EXIT; 0x27 WARP; 0x29 FADEOUT; 0x2A FADEIN; 0x2B FIGHT; 0x2C SHAKE;
  0x2E SHOP; 0x30 PLAY; 0x31 SE; 0x32..0x35 MVSTART, MVEND, MVSET, MVKILL;
  0x38 MVDATA; 0x39, 0x3A MVPARTYON, MVPARTYOFF; 0x3B, 0x3C MVSTOPON,
  MVSTOPOFF (the movers' tile check); 0x3D MVSYNC; 0x3E FACELOAD; 0x3F,
  0x40 MVSCRON, MVSCROFF (the camera); 0x41 CHRON; 0x42 JUMPPACK (another
  script); 0x43 CHANGE; 0x44 WIPEOUT; 0x45 WIPEIN; 0x47 `ScriptCommandColMix` (`0x8005DE54`)
  COLMIX; 0x49 PLACESET; 0x4E..0x51 FADESET, FADEKILL, CHRFADE, FADESYNC (the
  colour-ramp groups); 0x52..0x54 VOICELOAD, VOICE, VOICESYNC; 0x55
  `ScriptCommandMessageTimedSet` (`0x8005EB04`) FEEDSET; 0x56
  `ScriptCommandMapPadDirectionsChange` (`0x8005EB98`) CONFUSION; 0x57 LEVEL; 0x58
  `ScriptCommandPartyMpHalve` (`0x8005ECC8`) DRAIN; 0x59 MAGICSET; 0x5A
  `ScriptCommandMusicLevelSet` (`0x8005E5A4`) VOLUME; 0x5B MPKILL; 0x5C
  `ScriptCommandStoryBitsItemsReset` (`0x8005EF58`) ALLRESET. Ids 0x00 and 0x01, the error
  hang, fall on opcodes 0x01 and 0x02, which have no keyword.
* The one break: retail 0x05 is `ScriptCommandLabelReturn` (`0x8005C0E4`) and 0x06 the
  error hang, while the keyword table has no keyword at 0x06 and RETURN at
  0x07. Shifted by one, return would be retail 0x06 and the empty slot
  0x05; retail has the two the other way round.
* Keywords whose meaning the handler's name does not plainly show: 0x2D
  `ScriptCommandMoviePlay` (`0x8005E2B4`) EVENT, 0x2F `ScriptCommandPartyHpMpFill` (`0x8005E3D0`) FIGURE,
  0x46 `ScriptCommandMapColSet` (`0x8005DDC4`) OFFSET, 0x48 `ScriptCommandMapPartyKindSet` (`0x8005E890`)
  ATRSET, 0x4C, 0x4D `ScriptCommandMoveChrSecondViewFollowStart` (`0x80062A10`) and
  `…End` MVEFFON, MVEFFOFF.
* The four commands retail steps over, 0x36, 0x37, 0x4A and 0x4B
  (`ScriptCommand36Skip` (`0x800624DC`) and its three twins, which pass the command and its
  byte list and do nothing else), fall on MVLOAD, MVDELETE, MVLOCK and
  MVUNLOCK.
* Retail ids 0x5D..0x60 have no keyword.

## The map-cell events and the message codes

### The map-cell events — `MapAttributeRun` (`0x800590C0`)

The jump table at `0x80012918` (nine words, kinds 1 to 9) is this
function's switch. Its two callers are `MapPartyStep` (`0x80056248`)
(map_step.c, the party's part of a map frame) and `GameEnter`
(`0x80059498`) (map_change.c, near its end).

```c
void MapAttributeRun(void) {
    ev = MapAttributeFind(
             rec->sprite.x + g_MapPartyBoxDx,       /* rec = g_MapPartyRecords: the party's first MapRecord (+6) */
             rec->sprite.y + g_MapPartyBoxDy,       /* +0xA; s8 offsets: the cell being probed */
             g_MapPartyBoxHalfWidth, g_MapPartyBoxHalfHeight);   /* u8, a half width and a half height */
    code = ev & 0xFFFF;
    if (code == 0) return;
    arg = ev & 0x7F;
    switch (code >> 7) {                        /* kinds 1..9, 9-word table */
    case 1: MapChange(arg, 0, 1, 1, 1, 0xFF);      break;
    case 2: MapPartyPlace(arg, 0, 1, 1, 1);        break;
    case 3: if (g_MapBgChange[0] == 0 || g_MapBgChange[0x15] != 0)
                MapBgChangeStart(arg, 0);          break;
    case 4: MapAttributeKind4Run(arg);             break;
    case 5: MapAttributeKind5Run(arg);             break;
    case 6: if (g_MapPadPressedButtons & 0x40)
                MapAttributeKind6Run(arg);         break;
    case 8: MapColFadeStart(arg >> 4, arg & 0xF);  break;
    case 9: ColMix(arg, 1);                        break;
    }                                           /* kind 7 and anything else: nothing */
}
```

What the handlers do, from their C in `map_change.c`:

| Kind | Call | What the callee does |
|---|---|---|
| 1 | `MapChange(arg, 0, 1, 1, 1, 0xFF)` (`0x8005922C`) | Leaves for another map by row `arg` of the 11-byte `MapEnterRow`s at the address in `g_MapEnterTable`: byte 3 is a sound played first (`SoundPlay(it, 3)` (`0x800555F0`)); byte 0, the map's number, goes to `MapSeek` (`0x8006891C`) and `MapSet` (`0x80068780`); bytes 4..7, in units of 8 pixels, become the map's limits `g_MapLimitLeft` to `g_MapLimitBottom`, which the view is kept inside; bytes 8..10 are the spot and facing handed to `MapPartySpotSet` (`0x80064B8C`) |
| 2 | `MapPartyPlace(arg, 0, 1, 1, 1)` (`0x80059730`) | A move inside the map — a door, a stair: row `arg` of the 9-byte `MapPartyPlaceRow`s at the address in `g_MapPartyPlaceRows` (0x801E7080, which `GameStart` (`0x80055B60`) stores): a sound (`SoundPlay(byte 1, 3)`), the new limits, the party's new spot and facing, the view set again (`MapViewStep(1, 1)`), with the draw area closed before (`MapDrawAreaClose` (`0x80056F5C`)) and opened after (`MapDrawAreaOpen` (`0x800570D8`)); byte 0 is only handed to those two, which do not read it. It also zeroes `g_MapBgChangeNumber` and `g_MapPartyBlinkCount` |
| 3 | `MapBgChangeStart(arg, 0)` (`0x80059900`) | Starts bg change `arg`: a rectangle of the map's bg copied over, some frames apart, from a list of other places of the bg, by the row that word `arg` of the table at the address in `g_MapBgChangeTable` (0x801E7500) points at (field-frame.md §"The map's tile animation"). Called only while `g_MapBgChange[0]` is 0 (no change runs) or `g_MapBgChange[0x15]`, the running change's delay, is not 0 |
| 4 | `MapAttributeKind4Run(arg)` (`0x80059C48`) | `MapViewStep(1, 1)` (`0x80068DB8`), then `ScriptLabelRun(arg)` (`0x80059FDC`) |
| 5 | `MapAttributeKind5Run(arg)` (`0x80059C7C`) | `MapPartyRingRewind()` (`0x80066F54`): the party's position ring stepped back `g_MapPartyWalkStepCount` entries and the party put there (§"The party's position ring stepped back"), then `ScriptLabelRun(arg)` (`0x80059FDC`) |
| 6 | `MapAttributeKind6Run(arg)` (`0x80059CA8`) | `ScriptLabelRun(arg)` (`0x80059FDC`) alone, and only when `g_MapPadPressedButtons & 0x40` |
| 8 | `MapColFadeStart(arg >> 4, arg & 0xF)` (`0x80059CC8`) | A row (bits 4..6 of `arg`) and a scale (bits 0..3): `MapColSet(row, scale)` (`0x80059D10`) sets the three values of `g_MapCol`, and `layer_col_fade_start` (`0x8001EA74`) fades the layers to them over 0x18 frames. Row 0 does nothing |
| 9 | `ColMix(arg, 1)` (`0x80059E04`) | Sets `g_MapMix` to `arg * 2` (0x1F for 0xF) and hands it to `layer_mix_fade_start(8, 1, 0x18, v)` (`0x8001EBA4`); its debug string is `ColMix(no=%d,sw=%d)`, from which the developers' name is read |

`ScriptLabelRun(n)` (`0x80059FDC`) (script_run.c) resets the script
reader's run state and looks label `n` up in the table of 16-bit offsets
at the script's base `g_ScriptBase`, so kinds 4 to 6 run the map's
script from label `arg`. Kind 6's word is the pad's: `MapPadStep`
(`0x80055EC4`) (map_step.c) sets it to `pad & (prev ^ pad)`, the buttons
that went down in this frame's poll — so kind 6 is "the button pressed
while standing on the cell", a shop's counter (§"Chests, examine cells and
the item notice"). The cell probe `MapAttributeFind` (`0x80069A20`)
(map_over.c) returns the low 11 bits of the first attribute under the box
whose low 7 bits are not 0 and whose bits 7..10 are below 10: the kind in
bits 7..10 and the argument in bits 0..6 (§"The map-cell event probe").

### The message codes — `font_string_walk` (`0x8005A884`)

docs/spec/text.md has the whole format of a packed string; what follows
is what the C does. `font_string_walk` (message_disp.c) reads a string up
to a byte 0xFF and hands each character code to one function and each
control to another (arguments: the string, the two functions; it returns
the address it stopped at). `ScriptCommandMessageDisp` (`0x8005AB1C`,
event-script command 2) and `MessageControlRun` (`0x8005AF20`) call it
with `MessageStringCodePut` (`0x8005A834`) and `MessageStringControlRun`
(`0x8005A864`); `font_string_put` (`0x80026F78`) (font_string.c) and
`BtlMessageControlRun` (`0x8004C5E4`) (btl_message.c) pass their own pairs.

* Outside a run, a byte k below 0xD6 stands for entry k of
  `g_FontStringCodes` (`0x800B34C0`, 0xD6 16-bit codes), and a byte from
  0xD6 up is the high byte of a code whose low byte follows. A code up to
  0xEFFF is a character (less 0x5500 when it is above 0xD63F); a higher
  code is a control. Entries 1 to 7 of the table are controls: 0xFF01,
  0xFF02, 0xFF03, 0xFF00, 0xF0FF, 0xF1FF and 0xF2FF.
* A byte 0x0E opens a run, which a 0 byte closes. In a run a byte b below
  0x5C is the character b + 0x1F. A byte from 0x5C up picks a word of
  `g_FontStringWords` (`0x800B386C`, 512 words of printable bytes, to
  `0x800B45F0`) through `font_string_word_get` (`0x80059F4C`): word
  b − 0x5C, or the next byte + 0xA1 after 0xFD, or the next byte + 0x19E
  after 0xFE. Each byte of the word is a character.
* Six characters of a run are controls. The function has two switches over
  the characters 0x24 to 0x5E, each with a 59-entry jump table indexed by
  the character − 0x24: the table at `0x80012994` for a character of a
  word, the one at `0x80012A84` for a character the run gives directly.
  Their other entries lead to the default, the character handed on.

| Character | Run byte | Controls | In the field's message window |
|---|---|---|---|
| `%` (0x25) | 0x06 | 0xFF00 | wait for a press |
| `@` (0x40) | 0x21 | 0xFF02 | next row |
| `\` (0x5C) | 0x3D | 0xFF03 | close; from a word it also ends the walk |
| `$` (0x24) | 0x05 | 0xFF00, 0xFF03 | wait, close, and the walk ends |
| `*` (0x2A) | 0x0B | 0xFF00, 0xFF01 | wait, then a new page |
| `^` (0x5E) | 0x3F | 0xFF01 | new page |

The meanings are `MessageControlRun`'s: 0xFF00 waits for a press (or,
while `g_MessagePressWait` is 0, for a time that grows with the row);
0xFF01 clears the three rows and starts again at the first; 0xFF02 goes on
to the next row, and the third advance starts a new page; 0xFF03 closes
the message (`MessageClose` (`0x8005A72C`)). `BtlMessageControlRun` reads
the same four codes the same way for the battle's messages.

`MessageCodesConv` (`0x8005AC44`) (message_disp.c) packs the other way,
for `MessageDisp` (`0x8005AD50`): a list of 16-bit codes ended by 0xFFFF
becomes a string ended by 0xFF. A code that entry i (1 to 7) of
`g_FontStringCodes` holds becomes the one byte i; any other code is
written as its two bytes, high first. A code from 0xA to 0xEFFF is first
printed with the line "ERR:MessageConv( code=%04x ) can't convert." and
written all the same. A code below 0xA (a digit) has 0x824F added before
the lookup, but the two bytes written are those of the code as it was
read, 0x00 and the digit; `font_string_walk` turns them into entry 0 of
the table (the code 0) and the digit's own entry, which for digits 1 to 7
is one of the controls above (read from the code, not played). The callers that pass digits are
`MenuPageNumberHang` (`0x8006D9A8`) and `MenuUseNumberHang`
(`0x80076688`), which show a number and then stop for good. The line's
own name, MessageConv, is not this function's: docs/naming.md lists it
among the lines that print something other than the function's
arguments.

## Debug strings

The map and script code's printed lines and assertions. `debug_printf`
(`0x800216EC`) (fmt.c) and `assert_fail` (`0x8001B050`) (exception.c) are
empty in this build, so none of them is shown.

| Address | String | Context |
|---------|--------|---------|
| `0x800128FC` | a Shift-JIS star, then `ColMix(no=%d,sw=%d)\n` | `ColMix` (`0x80059E04`) (map_change.c) prints it with its two arguments; the developers' name `ColMix` is read from it |
| `0x8001293C` | `ERR:MessageConv( code=%04x ) can't convert.\n` | `MessageCodesConv` (`0x8005AC44`) prints it for each code from 0xA to 0xEFFF |
| `0x8001296C` | `TextError!` | `ScriptErrorHang` (`0x8005B568`): the message of its assertion (line 0x66A) |
| `0x80012978` | `LNS_INTP.C` | The same assertion's file: the developers' name of lns_intp.c |
| `0x80012984` | `XX no1=%x\n` | `MessageNoticeDisp` (`0x8005BD84`) prints it with its first argument, `no1`, the id of a second string |
| `0x80012B84` | `MoveChrAction:...` | `MoveChrAction` (`0x800618B4`): the message of its assertion (line 0x3FF), followed by Shift-JIS text; the developers' name `MoveChrAction` is read from it |
| `0x80012BA0` | `LNS_MOVE.C` | The same assertion's file: the developers' name of lns_move.c |
| `0x80012CA8` | a star, then `MapEffectSet(tbl=%x,sw=%d)\n` | `MapEffectSet` (`0x80068AC0`) (map_load.c) prints it with its first and third arguments; the developers' name is read from it |
| `0x80012CC8` | a star, then `mapeffect=0x%x\n` | `MapEffectSet` prints it with the map's effect flags `g_MapEffect` when they are not 0 |
| `0x80012CDC` | `OverCheck(%d,%d) %d, %d,%d,%d,%d %d\n` | `OverCheck` (`0x800695A8`) (map_over.c): printing it with its arguments and four bytes of a box is the function's only statement. No call in the main executable or in MENU.BIN reaches `OverCheck`, and no data word holds its address |

## The movement tables

The read-only data from `0x80012BB0` holds, in order: the jump table of
`MoveChrGoalCheck`'s first switch (eight words); the jump table of the
switch in `ScriptCommandMoveChrCommandsSet` (`0x80062534`) (script_move.c,
event-script command 0x38) that steps over move commands 0xF0..0xF8 to
find where the script goes on (nine words, `0x80012BD0`);
`g_PartyRanks` (`0x80012BF4`); `g_MapTwinSpriteDataNumbers`
(`0x80012C00`); and the two speed tables (`0x80012C0C`, `0x80012C14`).

### The goal check's facing switch — `MoveChrGoalCheck` (`0x8005FDAC`)

The eight words at `0x80012BB0` are the jump table of the first switch in
`MoveChrGoalCheck` (lns_move.c), which `MoveChrWalkStep` (`0x8005F520`)
calls to ask whether a mover row's walk has reached its goal
(§"A map object's walk step"). The switch is on the facing the step was
made along, 1 to 8, and holds y at the goal's y once it is reached:

| Facing | Entry | y test |
|---|---|---|
| 1, 2, 8 | `0x8005FE10` | y at or below the goal's y is set to it; otherwise not done |
| 4, 5, 6 | `0x8005FE30` | y at or above the goal's y is set to it; otherwise not done |
| 3, 7 | `0x8005FE54` | none |

The second switch, on x (2, 3, 4: at or above the goal's x; 6, 7, 8: at
or below), compiles to compares, not a table.

### The party's join order — `g_PartyRanks` (`0x80012BF4`)

12-byte table: `01 05 08 06 07 03 04 0A 09 0B 02 00`. The first eleven
bytes are the rank of each party id 1 to 11 in the party's order; the
last byte, 0, is never read. Its one reader is `PartyJoinPlaceGet`
(`0x80064C70`) (party.c), which `MoveChrPartyJoin` (`0x80060684`)
(lns_move.c) calls for event-script command 0x23: a joining id below 0xC
goes in front of the first member whose rank is a larger number, so the
party is kept in rank order; id 0xC always goes second and ids from 0xD up
last (§"Commands 0x23 and 0x5C").

### The twin-sheet map and the speed tables

The 12 bytes at `0x80012C00` are the twin-sheet map
`g_MapTwinSpriteDataNumbers`: byte n − 1 is the number of the sprite data
a twin is drawn with for a party member or object of sprite data number
n, `0D 1E 1D 1D 0E 1E 0E 0E 0E 1C 0D 0F` in the EXE image.
`ScriptCommandMoveChrRowActorSet` (`0x80061EAC`) (event-script command
0x34, §"Event-script command 0x34, bind or spawn a script actor") reads
it as `g_MapTwinSpriteDataNumbers[sheet - 1]`, and `MapPartySpriteDataLoad`
(`0x80063D48`), `MapObjectInit` (`0x800656CC`) and others read it the same
way. That command admits sheets below 0xF (`sheet < 0xF`), so sheets 13
and 14 read past the map into the first two bytes of the next table —
both 1.

The speed tables follow at `0x80012C0C`: `g_MoveSpeedPixels` =
`01 01 01 01 02 04 08 10`, the pixels a moving row goes in one tick, and
`g_MoveSpeedIntervals` (`0x80012C14`) = `08 04 02 01 01 01 01 01`, the
frames between two ticks (u8[8] each, indexed by the same speed).
`MoveCommandSpeedSet` (`0x800632D8`, move op 0xF5) stores entry [operand]
of the first in a mover row's `speed` (+0x1E) and of the second in its
`interval` (+0x25) and `interval_left` (+0x24); `StepCommandSpeedSet`
(`0x80063938`) does the same for a map object's row. `MoveChrFollowStart`
(`0x8005FED0`) uses entry 3 of both (1 and 1), in the same three fields
(§"Send movers home and release them").

## The script commands, one by one

Each section below reads one command's handler, or a few that belong
together, from its C. §"The command table" lists every command; the
commands that drive the movers are under §"The movers", and the short ones
under §"Smaller commands and helpers".

### Event-script command 7, SELECT, the paged form — `ScriptPagesRun` (`0x8005C2C0`)

Event-script command 7, SELECT, in its paged form. `g_ScriptCommandFunctions[7]` is
`ScriptCommandAnswerRun` (`0x8005C42C`) (the word at 0x800B3358 of the EXE image), and that
function picks the form by the script's number `g_ScriptNumber`:

    if (g_ScriptNumber == 0 || g_ScriptNumber == 0xC8) ScriptPagesRun();   /* this one */
    else                                               ScriptAnswerRun();  /* two answers */

`ScriptAnswerRun` is `0x8005C198`. `ScriptCommandAnswerRun` (`0x8005C42C`) is this function's
only caller. The function takes nothing and returns nothing. The C does not say what
scripts 0 and 0xC8 are.

Both forms read the same entry: `[u16 7][u16 label][u8 start answer][pad]`, then two
strings that each end on 0xFF, the second end rounded up to even. This function reads it
like this (`cmd` = the command's address, "the cursor" = `g_ScriptCommandPointer`):

1. `ScriptMoveEndWait()` (`0x8005A508`) once, before the first page. The TEXT command
   `ScriptCommandMessageDisp` (`0x8005AB1C`) calls it before a message too: `MapStatusClear()` (`0x8005A7C8`),
   `MapBgChangeEndWait()` (`0x8005A53C`), `MoveChrCommandEndWait(0x3FFFF)` (`0x8005F3C8`); the last waits
   until none of the 18 mover slots is busy.
2. Per page (the label `again` in the C):
   - the cursor goes to `cmd + 4`; the byte there is the start answer;
     `ScriptPointerAdvance(2)` (`0x8005B5B8`) moves the cursor on to `cmd + 6` (it aligns:
     `(cursor & -2) + 2`).
   - Two `MessageAnswer` records (script.h: `u16 string_id` at +0, `s32 string` at +4,
     8 bytes) on the stack: both ids 0; `str[0].string` = the cursor (the first string);
     the cursor = `ScriptStringEndFind(0)` (`0x8005C144`) (one past the first 0xFF),
     `str[1].string` = that (the second string); the cursor = `ScriptStringEndFind(1)` (`0x8005C144`)
     (one past the second 0xFF, rounded up to even): the cursor is now on the next
     command.
   - Unless the skip flag is set, `sel = MessagePageRun(sel, str)` (`0x8005B894`), the four-way
     prompt (§"The message window's answer prompts"). It ignores the `sel` passed in
     and answers by the pad record's repeat word, tested in this order: 0x4000 -> 0,
     0x40 -> 1, 0x1000 -> 2, 0x10 -> 3. With the skip flag set, `sel = 3` and nothing is
     drawn or waited for.
3. The answer:

| sel | what happens |
|---|---|
| 0 | `ScriptPagePush(cmd)` (`0x8005C238`): this page's offset `(cmd - g_ScriptBase) >> 1` goes onto the ring; return. The cursor is on the next command, so the script runs on. |
| 1 | the cursor is put back on `cmd`; `ScriptCommandLabelJump()` (`0x8005BF5C`) reads the u16 at `cmd + 2`, the entry's label, and goes on there; return. |
| 2 | `ScriptPagePop()` (`0x8005C278`) takes the last offset off the ring. 0 (the entry there is 0): return, the script runs on. Otherwise the cursor = that address and the page there is read and shown again. |
| 3 | `ScriptPagePush(cmd)` (`0x8005C238`); skip = 1; if the next command's id (`*(u16 *)` the cursor) is 7, go round again: that page is walked with no prompt and answers 3 itself. At the first command that is not 7: `ScriptPagePop()` (`0x8005C278`) takes the last page back off the ring (0: return); skip = 0; the cursor = it; go round again, so that page is shown with its prompt. |
| other | return (cannot happen: `MessagePageRun` (`0x8005B894`) returns 0..3). |

So a run of consecutive command-7 entries is a set of pages of two strings each:
answer 0 goes on to the next entry, answer 2 back to the one before, answer 3 to the
last of the run, and answer 1 leaves through the entry's label. On the pad, answer 0
is down (0x4000), 1 CROSS (0x40), 2 up (0x1000) and 3 TRIANGLE (0x10); the bits and
what the button names rest on are in §"The message window's answer prompts".

The ring: `g_ScriptPageRingEntries`, 0x80 u16 entries, index `g_ScriptPageRingIndex`.
`ScriptPagePush(addr)` (`0x8005C238`) stores `(addr - g_ScriptBase) >> 1` (the place counted in
16-bit words from the script's base) at the index and steps it `(+1) & 0x7F`;
`ScriptPagePop()` (`0x8005C278`) steps it `(-1) & 0x7F` first, then returns
`g_ScriptBase + entry * 2`, or 0 when the entry is 0. The entry is not zeroed, and
nothing but `ScriptPagePush` writes the ring. A page taken off the ring is put back
on it only when it is left by answer 0 or 3.

Easy to assume otherwise:
- The start-answer byte at `cmd + 4` is read and passed, but the four-way prompt never
  uses it; only the two-answer form does.
- Answer 2 on the first page (nothing on the ring) does not stay on the page: the
  function returns with the cursor on the next command.
- In skip mode every walked page still has its two strings scanned (the cursor has to
  get past them); only the prompt is skipped.
- Answer 2's pop moves the ring index back even when it finds 0.

No RNG draw of its own. Frames: `ScriptMoveEndWait` (`0x8005A508`)'s wait and the prompt's.

### Event-script command 0x22, set a member's level, items or gear — `ScriptCommandChrLevelItemSet` (`0x8005D2F0`)

`void ScriptCommandChrLevelItemSet(void)` is event-script command 0x22:
`g_ScriptCommandFunctions[0x22]`, the word at 0x800B33C4. No `jal` reaches it; the
runner calls it through the table.

The command is 6 bytes: the u16 id, then `who`, `kind`, `val`, and one pad byte.
`ScriptPointerAdvance(2)` (`0x8005B5B8`) moves the cursor to the operands (c + 2) and returns that address;
the handler reads the three bytes and ends with `g_ScriptCommandPointer += 4` (cursor = c + 6).

- `who` — a member id. `0xC` is the shared item list (it has no record in
  `g_ChrRecords`).
- `kind` — 0, 7, or a worn-slot number.
- `val` — a level, or an item id, by kind.

The C also tests `who == 0xC` before the kind, with nothing in the test.

Arms, in the order the code tests them:

1. `kind == 0` — set level `val`. `rec` = the member's 0x40-byte character record
   (`ChrRecord`, chr.h), `g_ChrRecords + who * 0x40`.
   - `val < rec->level` (+0x1C, unsigned bytes): the record is reloaded raw.
     `resource_load(0xE7, 0x801CFB00)` (`0x80020230`) reads resource 0xE7 to the fixed buffer
     0x801CFB00 (its first 0x300 bytes are the twelve records `ChrRecordsReset` (`0x80063B30`)
     copies for a new game), and `mem_move(g_ChrRecords + who * 0x40, 0x801CFB00 +
     who * 0x40, 0x40)` (`0x800260FC`) copies this id's 0x40 bytes over the record.
   - Then, with the level read again (so after a reload it is the resource's level):
     `rec->level != val` → `ChrLevelAdd(who, val - rec->level)` (`0x8002844C`) (that many level-ups,
     stopping at 99) and `rec->hp = rec->hp_max`, `rec->mp = rec->mp_max`.
   - `val == rec->level` does nothing at all: no level-up and no HP / MP refill.
   - A `val` below even the reloaded level makes the count negative. `ChrLevelAdd` (`0x8002844C`)
     counts with the count's low byte (0xFE for -2), so the member is raised to 99.
     Read from the code, not played.
   - The reload overwrites the whole record, worn items (+0x1E..+0x23) and carried
     items (+0x24..+0x29) included, with the resource's bytes; the worn items'
     values are not taken off before or added after. `ChrLevelSet` (`0x80056780`) (chr_level.c)
     does the same for a level but takes the six worn items off before the copy and
     puts them back after; nothing reaches `ChrLevelSet` (no `jal`, and no data word
     holds its address). Read from the code, not played.
2. `kind == 7` — items.
   - `val != 0`: `ChrItemGive(who, val)` (`0x800577C4`), the inventory add (§"The inventory rule").
   - `val == 0`, `who == 0xC`: `mem_clear(g_ChrItemList0C, 0x60)` (`0x8002609C`): the 0x60
     bytes of the shared list are zeroed.
   - `val == 0`, any other `who`: `mem_clear(rec->items, 6)` (`0x8002609C`): the member's six
     carried items (+0x24..+0x29) are zeroed.
3. Any other `kind` — gear. `kind` is the worn-slot number (1..6 are the slots, at
   record + 0x1D + slot), `val` the item.
   - `val != 0`: `ChrItemUseGet(who, val, 1)` (`0x80058088`) (chr_worn.c). Question 1 asks whether
     `who` may wear the item: 0 when bit `who - 1` of the u16 at +0x10 of the item's
     0x12-byte row is set, -1 when it is clear, -2 for an item outside 1..0xB6. The
     function writes nothing and calls nothing, and its answer is not looked at, so
     the call has no effect: the item is put on whether or not `who` may wear it.
   - `ChrWornItemRemove(who, kind)` (`0x80058038`): takes off what is worn in the slot (zeroes the byte;
     `ChrWornItemValuesRemove` (`0x8005886C`) takes its two stat bonuses back and clears story bit
     0x2BC + the item row's story bit when the row has one) and returns the item,
     which this handler drops: the old item is not put in the carried items.
   - `ChrWornItemSet(who, kind, val)` (`0x80057FFC`) (chr_item.c): stores `val` in the slot and
     `ChrWornItemValuesAdd(who, val)` (`0x800587D0`) adds its bonuses (and sets that story bit). With
     `val == 0` the slot is left empty.
   - There is no range check on `kind`: 8 and up would write past the six slots
     (record + 0x25 and on, the carried items). Read from the code.

No RNG draw and no frame wait of its own (`ChrItemGive` (`0x800577C4`) has a path with frames —
the make-room page — when nobody has an empty item byte; `resource_load` reads the disc).

**Record fields used (`ChrRecord`, chr.h)**

| Offset | Member | Here |
|---|---|---|
| +0x00 / +0x02 | `hp` / `hp_max` (u16) | `hp = hp_max` after a level change |
| +0x04 / +0x06 | `mp` / `mp_max` (u16) | `mp = mp_max` after a level change |
| +0x1C | `level` (u8) | compared with `val`, subtracted from it |
| +0x1E | `worn_items[6]` (u8) | slot `kind` (1..6) written by the gear arm |
| +0x24 | `items[6]` (u8) | cleared by kind 7 / val 0 |

Globals: `g_ChrRecords` (the address of the records), `g_ChrItemList0C` (the address of
the 0x60-byte shared list), `g_ScriptCommandPointer` (the script cursor). The literal
0x801CFB00 is the same load buffer `ChrLevelSet` (`0x80056780`) (chr_level.c) and
`ScriptCommandStoryBitsItemsReset` (`0x8005EF58`) use.

### Commands 0x23 and 0x5C — a member joins; the party reset — `ScriptCommandPartyJoin` (`0x8005D60C`), `ScriptCommandStoryBitsItemsReset` (`0x8005EF58`)

**Command 0x23, `ScriptCommandPartyJoin` (`0x8005D60C`, lns_intp.c)**: 4
bytes: the id word, `from` = ip[2], `arg` = ip[3].

1. `MoveChrCommandEndWait(0x7E)` (`0x8005F3C8`) waits, a frame per pass,
   while any of mover slots 1..6 (the followers) is busy.
2. `from != 0`: slot = arg, id = `MoveChrRowIdGet(slot)` (`0x800605E4`) (the
   id that map object shows; 0xFF when the slot is not live). `from == 0`:
   id = arg, slot = 0, and an id in 0x10..0x1E has 0xF subtracted.
3. `g_PartyCount >= 6` (the party is full): script error 5, through
   `ScriptErrorHang` (`0x8005B568`).
4. Already in the party list (`PartyPlaceFind(id) >= 0` (`0x80064C08`)):
   nothing more.
5. Otherwise, for id < 0x1F: the persistent record's HP = max HP and MP =
   max MP, and event flag `0x2BC + id` is set; for id >= 0x1F no record
   write, flag `0x2BC + 0xD`. Nothing checks the id against the table's
   twelve records: id 0xC would write into the first bytes of
   `g_ChrPlacePresetRecords`, the characters' battle places and presets,
   which `GameStart` (`0x80055B60`) places right behind the table
   (0x801DBF80 + 12 × 0x40 = 0x801DC280; read from the code).
6. `MoveChrPartyJoin(id, slot)` (`0x80060684`) inserts the member: at its
   party spot when slot is 0, taking over the map object otherwise.

**Command 0x5C, `ScriptCommandStoryBitsItemsReset` (`0x8005EF58`,
lns_intp.c)**: 2 bytes, no operands: the whole-party reset.

1. Event flag 1 set: `g_ScriptEnded = 1` (script over) and `g_GameEnded = 1`
   (the run loop's end flag, bootstrap.md). The rest still runs.
2. `mem_clear(g_StoryBits, 0x800)` (`0x8002609C`): the WHOLE event-flag
   bitmap is zeroed, flag 1 included.
3. `resource_load(0xE7, 0x801CFB00)` (`0x80020230`): the level-1 party
   records are read from the disc.
4. For member ids 1..11: all six worn slots unequipped (`ChrWornItemRemove`
   (`0x80058038`), the returned items dropped); the six carried cells
   `+0x24..+0x29` copied from 0xE7's record; the level-1 gear equipped
   (`ChrWornItemSet(id, slot, …)` (`0x80057FFC`) from the 66-byte table at
   0xE7 + 0x300, the one `ChrRecordsReset` (`0x80063B30`) equips from).
5. The shared item list (`g_ChrItemList0C`, 0x68 bytes) is zeroed and
   `MapSpriteDataFree(1)` (`0x80063C34`) frees every loaded sheet-table entry
   and clears the party's sheet list `g_MapPartySpriteDataIds`.

Level, HP / MP, stats (apart from gear bonuses) and spells are not touched.
Its frames are the disc read's.

### Event-script command 0x25, remove map objects — `ScriptCommandMapObjectsRemove` (`0x8005D9D4`)

`void ScriptCommandMapObjectsRemove(void)` is event-script command 0x25:
`g_ScriptCommandFunctions[0x25]`, the word at `0x800B33D0`. Nothing calls it with a `jal`.

It removes map objects: the inverse of command 0x41, `ScriptCommandMapObjectsAdd` (`0x8005D7E0`)
(§"Event-script command 0x41, spawn map objects from a list"), the function right above
it in lns_intp.c. Same operand: a 0-terminated list of definition ids, one byte each,
right after the 2-byte command word.

Steps:

1. `g_ScriptCommandPointer += 2` (the script pointer onto the list); `ip` = that address;
   `n = ScriptListCount(1, 1)` (`0x8005B5D8`) counts the list and leaves the script pointer past its
   terminator. `n <= 0`: nothing happens.
2. For each of the `n` ids `id = *ip`:
   - `idx = MapObjectFind(id)` (`0x80065C34`) (map_object.c): the first live row of the 0x80 rows of
     `g_MapObjects` (0x60-byte `MapObject` rows) whose byte `+1` equals `id`; -1 when
     there is none. `idx < 0`: `g_ScriptErrorNumber = 0xA` and `ScriptErrorHang()` (`0x8005B568`),
     which does not return: it calls two functions that are empty in this build and
     then loops for ever. Nothing reads the error number. The code after the call is
     still laid out, as it is after every such call.
   - `row = g_MapObjects + idx * 0x60`; `kind = row[0] & 0x7F`; `flag` = byte `+2`
     (`story_bit`) of the object's 0x26-byte `MapObjectDefinition`,
     `g_MapObjectDefinitions + row[1] * 0x26`. The definition is indexed by the row's
     byte `+1`, not by the list byte, though the lookup has just matched the two.
   - `kind < 4`: when `flag != 0`, `StoryBitSet(flag + 0x580)` (`0x800576EC`).
     Otherwise: when `flag != 0`, `StoryBitSet((u16)(id + 0x680 + ((g_MapNumber -
     0x65) << 7)))` (`0x800576EC`) (`g_MapNumber` = the map number, as in the spawn).
     `StoryBitSet` (`0x800576EC`) (story.c) sets a story bit of `*g_StoryBits` and answers whether
     it was clear; the answer is not read here.
   - The row's first byte is zeroed, then the first byte of row `idx` of
     `g_MapObjectTwins` (the 0x3C-byte twins): both are free again.
3. Return. No frame wait and no RNG draw of its own (`ScriptListCount` (`0x8005B5D8`),
   `MapObjectFind` (`0x80065C34`) and `StoryBitSet` (`0x800576EC`) have neither; `ScriptErrorHang`'s loop runs no frame).

Facts beside the spawn's:

- The flags are the spawn's two flags, with the opposite sense. The spawn
  (`ScriptCommandMapObjectsAdd` (`0x8005D7E0`)) clears `0x580 + flag` (kinds 1..3) or `id + 0x680 +
  ((map - 0x65) << 7)` (other kinds) with `StoryBitClear` (`0x80057730`); this command sets the
  same flag. `MapObjectsInit` (`0x800655C8`), which makes a map's objects from its object set,
  does not make an object whose flag is set (`MapObjectDefinitionSpotFind` (`0x800684FC`), which
  nothing calls, makes the same test). So an object with a flag that this command
  removes stays off the map when the map is entered again.
- The kind test reads the row, not the definition. The spawn tests the
  definition's `(info[0] & 0x3F) + 1 < 4`; this one tests the row's byte `+0` with
  bit 0x80 masked off. `MapObjectInit` (`0x800656CC`) stores that same value in row `+0`
  (§"Spawn a map object from its definition"), and `MapObject` (map.h) has bit 0x80
  of that byte set while a script row holds the object, so the mask keeps a held
  object's kind readable.
- Only the two first bytes are written. Nothing else of the row or the twin is
  cleared, no sheet byte is touched (the spawn calls `MapSpriteDataMarkSet(sheet, 0)` (`0x8005F340`);
  there is no counterpart here), and no callee is told. A script row that holds the
  object (bit 0x80) is not released by this function; whether something else notices
  the dead row was not read.
- An id with no live row is script error 0xA, not a no-op — the mirror of the
  spawn's error 0xB for an id that is already live.

Fields and globals:

| Where | As used | Meaning here |
|---|---|---|
| `g_ScriptCommandPointer` | script pointer | advanced by 2 onto the id list |
| `g_MapObjects` → 0x60-byte `MapObject` rows | `+0` u8 kind (bit 0x80 masked), `+1` u8 definition id | the map-object rows |
| `g_MapObjectTwins` → 0x3C-byte rows | `+0` u8 | the rows' twins |
| `g_MapObjectDefinitions` → 0x26-byte `MapObjectDefinition`s | `+2` u8 | the object's story bit (0 = none) |
| `g_MapNumber` (u16) | map number | second flag form |
| `g_ScriptErrorNumber` (s16) | script error code | 0xA = "no live object with this id" |

### Event-script command 0x26, change scene, then go on at a label — `ScriptCommandMapChange` (`0x8005DB3C`)

Event-script command 0x26: `g_ScriptCommandFunctions[0x26]`, the word at `0x800B33D4`. It
is a table handler; nothing calls it with a `jal`. The command is 8 bytes, or 0x12 when
the scene entry follows inline.

The command, `c` = its first byte (the u16 id):

| Offset | Width | Meaning |
|---|---|---|
| `c+2` | u16 | label to go on at; 0 = the script ends |
| `c+4` | u8 | `idx`: row of the enter table `g_MapEnterTable` (11-byte `MapEnterRow` rows); 0 = the row follows inline |
| `c+5` | u8 | flag: 0 = `MapChange` (`0x8005922C`) also runs its closing fade (`MapFadeIn` (`0x800569E0`)); non-zero = it does not |
| `c+6` | u8 | **idx != 0** (8-byte command): the music number, `MapChange` (`0x8005922C`)'s last argument |
| `c+6..c+0x10` | 11 x u8 | **idx == 0** (0x12-byte command): the row, in the order `b0, b2, b4, b5, b6, b7, b8, b9, b10, b3, b1` |
| `c+0x11` | u8 | idx == 0: the music number, `MapChange` (`0x8005922C`)'s last argument |

The inline row is not stored in table order. By `MapEnterRow` (map.h: `+0` the map's
number, `+1` the object set, `+2` the fade, `+3` a sound, `+4..+7` the view's limits,
`+8` / `+9` the spot's x and y, `+10` the facing less 1) the script order is: map,
fade, the four limits, x, y, facing, then the sound, then the object set.

Steps, in source order:

1. `ScriptMusicWait()` (`0x8005A364`) (script_run.c): when `g_ScriptMusicLevelRequest` is non-zero it
   runs field frames (`ScriptFrameStep(1)` (`0x8005A1F0`)) for as long as `ScriptMusicStep()` (`0x80058FBC`) answers
   non-zero, then one more. The request is a move of the music's level that command
   0x30 (`ScriptCommandMusicPlay` (`0x8005E494`)) or 0x5A (`ScriptCommandMusicLevelSet` (`0x8005E5A4`)) started; so
   the command first waits for that move to end.
2. The script pointer `g_ScriptCommandPointer` is bumped twice by 2 (to `c+4`) and left there; the
   operand bytes are never stepped over, because the pointer is replaced in step 7 (or
   the script ends).
3. `idx == 0`: the eleven inline bytes are copied into a `MapEnterRow` on the stack
   and the music number is the byte at `c+0x11`; otherwise the stack record is left
   uninitialised (`MapChange` does not read it when `idx != 0`) and the music number is
   the byte at `c+6`.
4. `MapChange(idx, &row, flag == 0, g_MoveChrViewFollow, g_MoveChrSecondViewFollow, music)` (`0x8005922C`)
   (map_change.c), the scene change; the map-cell event of kind 1 makes it too
   (§"The map-cell events"; the object set: §"A map's object definitions come from
   its object set, not its map file"). In order: `idx != 0` takes row `idx` of
   `g_MapEnterTable`, else `*src`; a non-zero sound byte is played (`SoundPlay(sound, 3)` (`0x800555F0`));
   while the fade's state byte `g_MapFadeState` is 0, `MapSeek(map, music)` (`0x8006891C`) sends
   the drive on ahead to what the map needs (it reads nothing; sound.md §"BGM numbers")
   and `MapFadeOut(g_MapLayersMask | 0x130, 0x19, target, view follow, second view follow)` (`0x800568D4`)
   fades the map out (target -1, or -2 for fade 1; fade 2 sets the state byte to 2
   first, the party's own fade); then `MapSet(map, music, 0)` (`0x80068780`) makes the map the
   one the game is on and loads it (§"Map set-up"); `MapObjectsInit(object set)` (`0x800655C8`)
   makes its objects, the party is put on the spot, the view's limits are set; and
   when the third argument is non-zero, `MapFadeIn(g_MapLayersMask | 0x130, 0x19, view follow,
   second view follow)` (`0x800569E0`). The music number 0xFF keeps the map's own music.
   The two follow bytes go to the fades, which hand them to `MapFrameStep` (`0x800557D4`) at every
   frame: `g_MoveChrViewFollow` is 1 while the view follows the party and no mover slot
   holds it (0 while slot 0 holds it), `g_MoveChrSecondViewFollow` the same for the second view
   and slot 0x11 (§"Send movers home and release them").
5. `g_ScriptKeptBlinkCount = 0`: the count `ScriptLabelRun` (`0x80059FDC`) puts back into
   `g_MapPartyBlinkCount` when the run ends, so the party does not blink after the change.
6. `g_MoveChrSecondViewFollow == 0` (read again after the call): `MoveChrSecondViewFollowSet()` (`0x80061788`)
   (lns_move.c) sets the second view following again at once: `g_MoveChrSecondViewFollow = 1`,
   the first byte of mover slot 0x11 (`g_MoveChrRows + 0x330`) cleared, and bit 0x11 of
   `g_MoveChrBusyBits` and of `g_MoveChrLiveBits` cleared — unless the low two bits of
   `g_MapEffect` (the second view's mode) are 1 or 2, in which case it does nothing. Then
   `MapSpriteDataMarksClear()` (`0x8005F2C4`): `mem_clear(g_MapSpriteDataMarks, 0x100)` (`0x8002609C`).
7. The jump, statement for statement the tail of `ScriptCommandScriptChange` (`0x8005BFCC`)
   (command 0x42):
   - label 0: `g_ScriptEnded = 1` and return — the script is over;
   - `off = u16 at g_ScriptBase + label * 2`; `off == 0`: script error 0
     (`g_ScriptErrorNumber = 0`, `ScriptErrorHang()` (`0x8005B568`), which does not return);
   - `g_ScriptLabel = label`, `g_ScriptCommandPointer = g_ScriptBase + off * 2`.
   The address in `g_ScriptBase` never changes (`GameStart` writes it once), but the
   script there does: `MapSet` (`0x80068780`) loads the map through `MapLoad` (`0x80068A10`), which calls
   `ScriptChange` (`0x80068D7C`) with byte 8 of the map's record, and `ScriptChange` reads that
   script to the base when it is not the one there already. So the label is a label
   of the new map's script.

Frames: `ScriptMusicWait` (`0x8005A364`)'s and whatever `MapChange` (`0x8005922C`) waits (its fades). No RNG draw
of its own.

**Command 0x5D, `ScriptCommandMapScriptChange` (`0x8005F068`)**

0x5D is the fixed-form twin: a 0x12-byte command, `[u16 0x5D][u16 label][u8 script]
[u8 flag]`, then the eleven row bytes in 0x26's order and the music number at
`c+0x11`; the row is always inline. It calls `MapChange(0, &row, flag == 0,
g_MoveChrViewFollow, g_MoveChrSecondViewFollow, music)` (`0x8005922C`), sets `g_ScriptKeptBlinkCount = 0`, calls
`MapSpriteDataMarksClear()` (`0x8005F2C4`), then `ScriptChange(script)` (`0x80068D7C`) with the byte at `c+4` — so
the label is looked up in the script the command names, not in the map's — and jumps
as 0x42 does, but with no label-0 test: a label of 0 is looked up like any other. It
has no `ScriptMusicWait` (`0x8005A364`) and no `MoveChrSecondViewFollowSet` (`0x80061788`).

### Load a scene's portraits — `ScriptCommandFaceListSet` (`0x8005E610`)

Event-script command 0x3E: it loads portraits into the eight slots
`*g_FaceList` (0..5 small, 6 the wide one, 7 a spare beside it). Its
operand is a byte list (`ScriptPointerAdvance(2)` (`0x8005B5B8`),
`ScriptListCount(1, 1)` (`0x8005B5D8`) bytes long).

* **Byte 0** is the wide portrait (0xFF = leave it). If it differs from
  slot 6: slot 6 takes it, slot 7 is cleared and the wide one is marked
  for loading; a non-zero byte `+5` of its 10-byte `g_FaceListEntryRows`
  row also marks slot 7.
* **Each further byte** is a character id. The cursor skips filled slots
  — it tests `ids[slot] != 0` before `slot < 6`, so slot 6 is read once
  the six are full. An id `PartyPlaceFind` (`0x80064C08`) finds in the
  field party needs no slot; any other takes the free slot.
* **No free slot**: if slot 7 is already marked the list ends there;
  otherwise slot 6's id moves to slot 7, the new id takes slot 6, the wide
  one is marked again, and the list ends.
* **Tail**: with the wide one unmarked and no slot 0..5 taken it returns.
  Wide marked -> `func_8007608C(6)` (an empty function),
  `FaceSlot6Load(slot 6's id)` (`0x8001B50C`), and `func_8007608C(7)` when
  slot 7 is marked and non-zero. Then `FaceListByteLoad(i)` (`0x80076094`)
  per marked slot 0..5, `FaceListEntriesSet()` (`0x800760D0`), which
  rebuilds the 0x28 entries at `g_FaceListEntries`, and
  `g_MessageFaceSecond = g_MessageFaceFirst = 0` (the two faces kept by the
  message window, the shop and the menu).

No frame wait or RNG draw of its own. A face read from the disc waits in
the CD read's loop, `cd_sector_load` (`0x80020E88`), which calls
`VSync(0)` once a pass outside a battle until the read is done.

### Event-script command 0x41, spawn map objects from a list — `ScriptCommandMapObjectsAdd` (`0x8005D7E0`)

Operand: a 0-terminated list of definition ids, one byte each, after the
2-byte command word (`ScriptListCount` (`0x8005B5D8`) counts it). Per id
(`info` = its definition in `g_MapObjectDefinitions`), four checks, each
reporting through `g_ScriptErrorNumber` / `ScriptErrorHang` (`0x8005B568`):
`info[1]` (the sheet) is 0 -> error 0xA; the sheet is not loaded -> 9; a
live row already carries this id (`MapObjectFind(id) >= 0` (`0x80065C34`)) -> 0xB; no
free row of the 0x80 (`MapObjectFreeFind()` (`0x80065CA0`)) -> 2. **`ScriptErrorHang` (`0x8005B568`) prints,
asserts (line 0x66A) and spins forever** — an already-live object is a
script error, not something the command updates. When the definition's
byte `+2` is non-zero, event flag `0x580 + that byte` (kinds 1..3) or `id
+ 0x680 + ((map − 0x65) << 7)` (any other kind) is CLEARED
(`StoryBitClear` (`0x80057730`)). Then `MapObjectInit(row, twin, info, id)` (`0x800656CC`) and
`MapSpriteDataMarkSet(sheet, 0)` (`0x8005F340`). No frame, no RNG of its own.

### Switch the lead's form — event-script command 0x43, `ScriptCommandChr1ChrBChange` (`0x8005D470`)

Entry 0x43 of `g_ScriptCommandFunctions`, the word at `0x800B3448`, is the
function's only reference in the image. One operand byte, `ip[2]`; the command is 4 bytes
long. Operand non-zero makes character id 0xB the lead (old id 1); zero
makes id 1 the lead (old id 0xB). The string table names BOTH ids "Alex"
(strings 0x001 and 0x00B), so this swaps two forms of one character; which
form is which is not in this function.

1. If the lead — the first u16 of the party list `g_PartyIds` — already
   is the new id, nothing more happens (the command is consumed). There is
   no test that the lead IS the old id, nor of either record.
2. The party list's first entry and the first byte of actor row 0
   (`g_MapPartyRecords`) take the new id.
3. The OLD id's whole 0x40-byte persistent record (`ChrRecord`, `g_ChrRecords + id *
   0x40`) is copied over the new id's, and so is its 8-byte formation /
   preset record (`ChrPlacePresetRecord`, `g_ChrPlacePresetRecords + id * 8`). Nothing of the destination
   survives and nothing is re-derived afterwards — HP, level, experience,
   worn and carried items and spells all carry across. The source is left as it was.
4. `MapPartySpriteDataLoad(0)` (`0x80063D48`) when `MapPartySpriteDataMissingCount()`
   (`0x800647E4`) is non-zero (the party's field sheets, §"The party's field sprite sheets").
5. The eight portrait-slot ids `g_FaceList[0..7]` are searched for the new
   id; found at `i`, `FaceListSwap(0, i)` (`0x80076268`) swaps that slot with slot 0.
6. `MapRecordSpriteDataSet(g_MapPartyRecords, sheet)` (`0x800645A8`) binds actor row 0 to sheet `new` —
   `new + 0xF` when the area kind `g_MapPartyKind` is 0.
7. Event flag `old + 0x2BC` is cleared and flag `0x2BC + sheet` is set —
   step 6's sheet number, so it includes the `+ 0xF` when that applied.

The body has no frame wait and no RNG draw. Of its callees only
`MapPartySpriteDataLoad` waits: it reads the sheet directory from the disc,
and the read waits for the drive frame by frame (`VSync(0)` in
`cd_sector_load` (`0x80020E88`)). No callee draws from the RNG.

### Event-script command 0x4E, mover slots into a colour-ramp group — `ScriptCommandMixGroupSet` (`0x80062ACC`)

Event-script command 0x4E: entry 0x4E of the event-script handler table
`g_ScriptCommandFunctions` (the word at 0x800B3474). `void ScriptCommandMixGroupSet(void)`
(script_move.c); nothing calls it with a `jal`.

The command: `[u16 0x4E][group][slot + 1] ... [0]` — a zero-terminated byte list whose
first byte is the group, 1 or 2, and whose other bytes are mover slot numbers + 1
(the 0x30-byte `MoveChrRow` rows of `g_MoveChrRows`; slot 0 holds the camera, 0x11 the
second scroll). It puts the listed slots in one of the two colour-ramp groups (the
register's mix groups).

Steps, in source order:

1. The wait: `while (layer_mix_fade_check(0xC00C) & 0xFF) ScriptFrameStep(1);` (`0x8001ED94`, `0x8005A1F0`) — one field frame a
   pass. 0xC00C is the four layers command 0x50 can name (4, 8, 0x4000, 0x8000,
   see below). `layer_mix_fade_check` (`0x8001ED94`) (layer_col.c) looks only at the 8 bit, and only
   when `g_BtlRunning != 0` (in battle), where it returns `tile_mix_fade_check()` (`0x8001DDF0`); so
   in the field this loop never waits. `ScriptCommandMixFadeWait` (`0x80062EBC`) (command 0x51) is the
   same wait after its cursor step.
2. `p = g_ScriptCommandPointer` (the cursor, on the command's id); `g_ScriptCommandPointer += 2`;
   `count = ScriptListCount(1, 1)` (`0x8005B5D8`) walks the byte list from the cursor to its zero byte
   and leaves the cursor past it; its result is the number of non-zero bytes, the group
   byte included.
3. `g = p[2] - 1` (a `u8`), the group as an index 0 / 1.
4. For each of the `count - 1` slot bytes from `p + 3`, with `n = byte - 1`:
   - bit `n` is set in the group's own mask word and cleared in the other one. The two
     words are `g_MoveChrMixGroup1Bits` (g = 0) and `g_MoveChrMixGroup2Bits` (g = 1), which the C
     reaches as `(&g_MoveChrMixGroup1Bits)[g]` and `[g ^ 1]`.
   - unless `n` is 0 or 0x11: the slot's actor (`MoveChrRow.actor`, `+0xC`) and its twin
     (`.twin`, `+0x10`) each get their draw word (`sprite.flags`, record `+0xE`) as
     `& 0xE3FF` and then `| (g + 6) << 10` — 0x1800 for group 1, 0x1C00 for group 2.

No RNG draw. The only frames are the wait's. No value returned.

**The other commands of the set (0x4F, 0x50, 0x51)**

- 0x4F, `ScriptCommandMixGroupClear` (`0x80062C38`): `[u16 0x4F][slot + 1]...[0]` — takes
  slots out. It first calls `ScriptCommandMixFadeWait()` (`0x80062EBC`) (cursor += 2, then the same
  wait as here), so its cursor has already stepped over its own id when it reads its
  list. Then for each slot the bit is cleared in both words, and:
  slot 0 gets layer 4's own mix back (`layer_mix_set(4, g_MapLayer4Mix)` (`0x8001ECA0`) when bit
  0x80 of `g_MapEffect` is set, otherwise `layer_mix_set(4, 0)` and then `(4, 0x20)`, no
  mix); slot 0x11 layer 8's (`layer_mix_set(8, g_MapMix)` (`0x8001ECA0`), and `(8, 0x20)` as
  well when `g_MapMix` is 0); any other slot's actor gets its draw word `& 0xE3FF`,
  then `| 0x800` or `| 0xC00` when byte +3 (`mode`) of its object's definition is 1
  or 2 (the bits `MapObjectInit` gives it), and its twin only the `& 0xE3FF`.
- 0x50, `ScriptCommandMixGroupFadeStart` (`0x80062E00`):
  `[u16 0x50][group][level][frames][pad]`, 6 bytes. `MapStatusClear()` (`0x8005A7C8`); then
  `layer_mix_fade_start(layers, 1, frames, lv)` (`0x8001EBA4`) with `lv = level * 2` (0x1F for level 0xF) and
  `layers = ((word & 1) << 2) | (word & 0x20000 ? 8 : 0) | (g == 0 ? 0x4000 : 0x8000)`,
  `word` the group's mask word: layer 4 when the camera's slot is in the group, layer
  8 when the second scroll's is, and 0x4000 or 0x8000 for the group itself.
- 0x51, `ScriptCommandMixFadeWait` (`0x80062EBC`) (§"Event-script command 0x51, the colour-ramp
  wait on its own"): `g_ScriptCommandPointer += 2`, then one field frame a pass while
  `layer_mix_fade_check(0xC00C) & 0xFF` (`0x8001ED94`).

What the four commands show:

- `g_MoveChrMixGroup1Bits` / `g_MoveChrMixGroup2Bits` are the two groups' membership masks, one bit a
  mover slot (bit 0 the camera, bit 0x11 the second scroll). `MoveChrInit` (`0x8005F1C8`)
  (lns_move.c) zeroes both before a script's run; command 0x4F clears a slot's bit in
  both.
- `layer_mix_fade_start` (`0x8001EBA4`) tests only bits 4 and 8 of its mask. The 0x4000 / 0x8000
  that command 0x50 passes for the group do nothing in this build, and outside a
  battle the mix of layers 4 and 8 is set at once, `frames` unused; only in battle
  does layer 8 fade over `frames`, through `tile_mix_fade_start` (`0x8001DDA4`). So in the field
  command 0x50 sets the mix of layer 4 or 8 at once, for a group that holds the
  camera's or the second scroll's slot, and does nothing for the group's actors.
- The draw word's bits 10..12: `sprite_piece_draw` (`0x80019DBC`) takes bits 0xC00, `(word >>
  10) & 3`, as the sprite's mix group, whose level `sprite_mix_set` (`0x8001A8D0`) keeps (-1 for
  no mix), and `sprite_draw` (`0x8001A528`) takes bit 0x400 as semi-transparency. So an actor in
  group 1 (0x1800) is drawn in mix group 2, and one in group 2 (0x1C00) in mix group 3
  and semi-transparent; bit 0x1000 is not among the bits those two read. None of the
  four commands sets the levels of mix groups 2 and 3 (`layer_mix_set` (`0x8001ECA0`) does, for
  its mask bits 0x400 and 0x800).
- 0xC00C in the wait = 4 | 8 | 0x4000 | 0x8000: exactly the layers 0x50 can name.

Record fields used: `MoveChrRow` `+0xC` actor pointer, `+0x10` twin pointer; the
actor's and twin's u16 draw word at record `+0xE` (`MapRecord.sprite.flags`). Globals:
`g_ScriptCommandPointer` (the cursor), `g_MoveChrRows` (the slot table),
`g_MoveChrMixGroup1Bits` / `g_MoveChrMixGroup2Bits`.

### Event-script command 0x51, the colour-ramp wait on its own — `ScriptCommandMixFadeWait` (`0x80062EBC`)

`void ScriptCommandMixFadeWait(void)` (`0x80062EBC`, script_move.c) is entry 0x51 of the
event-script handler table `g_ScriptCommandFunctions` (the word at 0x800B3480 in the EXE
image). The script cursor `g_ScriptCommandPointer` (s32) steps over the two-byte command,
which is the id word with no operands; then one field frame a pass,
`ScriptFrameStep(1)` (`0x8005A1F0`), while `layer_mix_fade_check(0xC00C) & 0xFF` (`0x8001ED94`).
That is the wait commands 0x4E and 0x4F open with (§"Event-script command 0x4E, mover
slots into a colour-ramp group"; 0xC00C = the four layer bits, 4, 8, 0x4000 and 0x8000,
that command 0x50 `ScriptCommandMixGroupFadeStart` (`0x80062E00`) can hand to
`layer_mix_fade_start`). `ScriptCommandMixGroupClear` (`0x80062C38`, command 0x4F) calls this
function directly as its first statement, for the step over the id word and the same
wait. No RNG draw, no other state.

The loop never runs in an event script. `layer_mix_fade_check` (layer_col.c) answers 0
whenever the battle byte `g_BtlRunning` is 0 (and for any mask without bit 8), and
outside a battle `layer_mix_fade_start` sets the mix at once, so there is no fade to wait
for. `g_BtlRunning` is 1 only inside `BtlRun` (`0x80027434`) (btl_run.c sets it, and
`BtlBlackLinesRun` in btl_window.c drops it round one call), and no battle file runs an
event script. So commands 0x4E, 0x4F and 0x51 wait no frame in the game.

### Event-script command 0x58, halve the party's MP — `ScriptCommandPartyMpHalve` (`0x8005ECC8`)

`void ScriptCommandPartyMpHalve(void)` (`0x8005ECC8`) is event-script command 0x58: the word at
`0x800B349C` of the handler table `g_ScriptCommandFunctions`, which the runner
`ScriptLabelRun` (`0x80059FDC`) (script_run.c) indexes with the u16 at the script pointer. The table
word is the function's only reference in the image (no `jal`). The command is two
bytes: the id word, no operand.

It halves every party member's MP after a 300-frame wait.

1. `g_ScriptCommandPointer += 2` — the script pointer steps over the id word.
2. Three arrays on the stack are zeroed with `mem_clear` (`0x8002609C`): `half` (6 x u16),
   `mp20` (6 x s32), `lost` (6 x s32).
3. `g_MapIdleCount = 0x12C` — the field's idle count is put at its limit. `MapPadStep`
   (`0x80055EC4`) (map_step.c), which counts it up while the party stands and raises the
   status request `g_MapStatusRequest` on reaching 0x12C, is called only by `MapPartyStep`
   (`0x80056248`), the party's step of the field's own frame; `ScriptFrameStep` (`0x8005A1F0`) does not call
   it. So nothing counts during the wait, and `MapStatusClear` in step 7 finds the
   count at 0x12C.
4. First walk over the party list: `ptr = (u16 *)g_PartyIds`, six ids. A zero id ends the
   walk; an id of 0xC or more is skipped (the shared-list id 0xC and anything above have
   no member record). For a member id `1..0xB`, with its 0x40-byte `ChrRecord` at
   `g_ChrRecords + id * 0x40` and `mp` = the u16 at `+4` (`mp`; `mp_max` is `+6`):
   - `mp20[i] = mp * 20`
   - `half[i] = mp >> 1`
   - `lost[i] = mp - half[i]`
   `mp20` and `lost` are written and never read — nothing else in the function touches
   them, and their addresses go only to the zeroing calls. (20 is the outer count of the
   wait in step 5.)
5. The wait: `for (j = 0; j < 20; j++) for (k = 0; k < 15; k++) ScriptFrameStep(1);` (`0x8005A1F0`) —
   300 field frames (`ScriptFrameStep(1)` (`0x8005A1F0`) is the scripts' one-frame call).
6. Second walk, same rules, both globals re-read: the member's MP word `+4 = half[i]`.
   Max MP (`+6`), HP (`+0` / `+2`) and everything else in the record are left alone.
   Slots the first walk skipped keep their zeroed `half[i]`, but the second walk skips the
   same slots, so no zero is ever written from them — unless the party list changed
   during the 300 frames (then a newly listed member below 0xC would get MP 0; read from
   the code, not played).
7. `MapStatusClear()` (`0x8005A7C8`) — when the idle count is `>= 0x12C` (or a request is
   pending) it zeroes the count, sets `g_MapStatusRequest = 2` (the request that takes the
   status off), calls `MapStatusDisp` (`0x80056408`) (the idle status strip, menu-ui.md), which for
   request 2 clears rows 0x16..0x19 of the text grid, and lets one more frame go by.

No RNG draw of its own; the frames are the 300 of step 5 plus `MapStatusClear` (`0x8005A7C8`)'s one.

Callers: none by `jal`; reached only through the handler table. Which event scripts use
command 0x58 was not looked up.

**Fields and globals**

| Thing | Type | Meaning here |
|---|---|---|
| `g_ScriptCommandPointer` | 32-bit address | script pointer |
| `g_PartyIds` | address of `u16[6]` | the party list's member ids, zero-terminated |
| `g_ChrRecords` | address of 0x40-byte `ChrRecord`s | the character records, indexed by member id |
| record `+0x04` | u16 | current MP |
| `g_MapIdleCount` | u16 | the field's idle count, limit 0x12C |
| `mem_clear(buf, n)` (`0x8002609C`) | function | zero `n` bytes |
| `ScriptFrameStep(1)` (`0x8005A1F0`) | function | one field frame |
| `MapStatusClear()` (`0x8005A7C8`) | function (message_disp.c) | take the status off (see 7) |

## The movers

The movers are the 0x30-byte rows of `g_MoveChrRows` (`MoveChrRow`), each
live while its bit in `g_MoveChrLiveBits` is set: slot 0 is the camera,
slot 0x11 the second scroll, slots 1..6 the party's actors, and the others
actors that event-script command 0x34 binds or spawns. Command 0x38 gives a
mover a move-script list, and `MoveChrCommandRun` (`0x80061800`) runs its
ops 0xF0..0xF9 through `g_MoveChrCommandFunctions` (`0x800B45F8`).

### Event-script command 0x34, bind or spawn a script actor — `ScriptCommandMoveChrRowActorSet` (`0x80061EAC`)

Its address is the word at `0x800B340C`, entry 0x34 of
`g_ScriptCommandFunctions` (`0x800B333C`); no jump reaches it. It binds
script slot `ip[2]` (`ip` is the command's first byte) to an actor. A slot
is a 0x30-byte `MoveChrRow` of `g_MoveChrRows`, live when its bit is set in
`g_MoveChrLiveBits`. By the register's reading of `g_MoveChrRows`, slot 0
is the view, slots 1..6 the party's places 0..5, slots 7..0xA hold map
objects a command binds, slots 0xB..0x10 actors of their own, and slot
0x11 the second scroll.

* **A slot that is already live** reports error 0x10 (`g_ScriptErrorNumber`,
  then `ScriptErrorReport` (`0x8005F1C0`), an empty function) and carries
  on — none of the three error paths returns.
* **`slot < 0xB` — bind an existing map object** (4-byte command).
  `ip[3]` is a definition number: `MapObjectFind` (`0x80065C34`) returns
  the first live `g_MapObjects` row whose `+1` byte holds it (error 0xA when
  none does). `MoveChrRowInit` (`0x8005F410`) opens the slot, which then
  gets `+3` = the row index, `+0xC` = the 0x60-byte `MapObject` row,
  `+0x10` = its 0x3C-byte twin in `g_MapObjectTwins`, `+0x14` = the address
  of the row's box at `+0x50`; the row's `+0x3C` (layer) / `+0x3E` (facing)
  bytes go to the slot's `+0x20` / `+0x21`, and bit 0x80 of the row's first
  byte is set.
* **`slot >= 0xB` — spawn a script-owned actor** (16-byte command) in row
  `slot - 0xB` of `g_MoveChrOwnActors` (0x60-byte `MapObject` rows) /
  `g_MoveChrOwnActorTwins` (0x3C-byte `MapRecord` twins). `MoveChrRowInit`
  opens the slot; its `+0xC`, `+0x10` and `+0x14` point at the row, the
  twin and the row's box at `+0x50`:

  | Operand | Meaning |
  |---|---|
  | `ip[3]` | sprite sheet — index into the 0x10-byte `g_MapSpriteDataEntries` records (error 9 when its pointer is null); stored in the slot's `+3` |
  | `ip[4]` | `MapRecordInit`'s kind, also stored at the row's `+4` |
  | `ip[5]`, `ip[6]` | x, y in 8-pixel units |
  | `ip[7]` | height: taken off the draw y (`+0xA`); `+8` keeps the ground y; kept in the slot's `+2` |
  | `ip[8]` | row `+0x3E` (slot `+0x21`) — the facing handed to `MapAnimationPoseSet` |
  | `ip[9]` | row `+0x3C` (slot `+0x20`) |
  | `ip[0xA]` | `MapRecordInit`'s mode |
  | `ip[0xB..0xE]` | four unaligned bytes copied to the row's box at `+0x50` |

  The row's first byte is set to 1. `MapRecordInit(row, kind, mode,
  sheet)` (`0x80065940`) points the row at the sheet, `MapAnimationPoseSet(row,
  1, facing)` (`0x8006633C`) gives it pose 1, `+0x3D = 2`, `+0x42 = 0`, and
  `MapAttributeClassGet(1, 0x20, x, drawY, y, &row+0x50, row+0x3C)`
  (`0x8006960C`) is OR'd into `+0xE` after bits 0x6000 are cleared. Sheets
  below 0xF also fill the twin row: a copy of the row's first 0x3C bytes,
  set up again by `MapRecordInit` with sheet
  `g_MapTwinSpriteDataNumbers[sheet - 1]` (§"The twin-sheet map and the
  speed tables") and the same kind at `+4`, then
  `MapAnimationPoseSet(twin, 1, facing)`.

Last, the command pointer moves past the command (4 or 16 bytes).

### Event-script command 0x35, release script slots — `ScriptCommandMoveChrRowsEnd` (`0x80062314`)

The inverse of command 0x34 (§"Event-script command 0x34, bind or spawn a
script actor"). The operand is a zero-terminated list of slot ids
(`ScriptPointerAdvance(2)` (`0x8005B5B8`) returns the list,
`ScriptListCount(1, 1)` (`0x8005B5D8`) its length, leaving the script
cursor past it). Per id, in list order:

1. `MoveChrCommandEndWait(1 << id)` (`0x8005F3C8`) waits out the slot's mover.
2. A slot that is not live (bit `id` of `g_MoveChrLiveBits` clear) sets
   `g_ScriptErrorNumber = 0xF`, calls `ScriptErrorReport()` (`0x8005F1C0`)
   and **returns**: the rest of the list is not processed and the closing
   frame is not made (command 0x34's three error paths all carry on).
3. `id < 0xB`, a bound map object, row index `idx` = the slot's `+3`:
   row `+0x3E` gets the slot's facing `+0x21` (the bind copied the row's
   facing there; the slot's moves change it), bit 0x80 of the row's
   first byte is cleared (the bind set it), and when the sprite sheet of
   the row's 0x26-byte definition (`+1` of `g_MapObjectDefinitions[row
   +1]`) is below 0xF, a sheet with a twin, that first byte is also copied
   to the first byte of the 0x3C twin `g_MapObjectTwins[idx]`.
4. `id >= 0xB`, a script-spawned actor: the first byte of row `id - 0xB`
   of `g_MoveChrOwnActors` (0x60 rows) and of `g_MoveChrOwnActorTwins`
   (0x3C twins) = 0, which switches the actor off.
5. Either way the slot's first byte (`g_MoveChrRows[id]`) = 0 and bit `id`
   of `g_MoveChrLiveBits` is cleared.

After the whole list: one frame (`ScriptFrameStep(1)` (`0x8005A1F0`)). Its
address is the word at `0x800B3410`, entry 0x35 of
`g_ScriptCommandFunctions`; no jump reaches it.

### Send movers home and release them — `MoveChrFollowStart` (`0x8005FED0`)

`void MoveChrFollowStart(s32 row_mask)` (`0x8005FED0`): walks the script
slots the mask names to where they stand when they follow, waits for
them, then releases the slots and sets their groups following again. Four
script commands call it:

* `ScriptCommandMoveChrViewFollowStart` (`0x800628F8`, command 0x3F) with
  1, `ScriptCommandMoveChrSecondViewFollowStart` (`0x80062A10`, command
  0x4C) with 0x20000 and `ScriptCommandMoveChrPartyFollowStart`
  (`0x80062640`, command 0x39) with 0x7E, each only while its group's flag
  is 0 and after waiting out the movers itself (`MoveChrCommandEndWait`
  with 0x2007F, or 0x7E for the party).
* `ScriptCommandMoveChrRowsAllEnd` (`0x80061CF8`, command 0x33) with
  0x2007F, all three groups at once. Its operand byte decides how the
  movers stop first: 0 waits until every slot's list has ended
  (`MoveChrCommandEndWait(0x3FFFF)`), any other value clears all busy
  bits and leaves each list where it is. After the call it lets go of every
  slot 7..0x10 still live much as command 0x35 does (an object gets the
  slot's facing back and bit 0x80 cleared, an own actor's first byte is
  zeroed) but touches no twin; then it turns off the sprite-list entries
  `MoveChrAction` fills (`sprite_run_off(6, 0xB)` and `(0x4C, 0xB)`) and
  puts the mix of layer mask 4 back as the map has it.

A live slot in the mask (0x30 rows of `g_MoveChrRows`, live bits
`g_MoveChrLiveBits`, busy bits `g_MoveChrBusyBits`) gets a goal at `+0x18` / `+0x1A`,
speed `g_MoveSpeedPixels[3]` at `+0x1E`, `g_MoveSpeedIntervals[3]` at `+0x24` / `+0x25`, the
built-in script `g_MoveChrMoveToList` (`F3 00 00 00 00 F9`: one move-to, then end)
as both its base (`+4`) and its pointer (`+8`), `+1 = 1` (running) and its
busy bit:

| Mask bit | Slot | Goal |
|---|---|---|
| 0 | 0, the camera (`g_MapViewX` / `g_MapViewY`) | `MapViewGoalGet` (`0x80069080`): the lead (the first party record's x and ground y) 0xA0 from the view's left and 0x78 from its top on a 0x140 × 0xE0 screen, held inside the map's limits `g_MapLimitLeft` / `g_MapLimitTop` / `g_MapLimitRight` / `g_MapLimitBottom`, the rectangle `MapChange` (`0x8005922C`) and `MapPartyPlace` (`0x80059730`) set — not the map's edges (field-frame.md) |
| 17 | 17, the second scroll (`g_MapSecondViewX` / `g_MapSecondViewY`) | `MapSecondViewGoalGet` (`0x80069130`) (§"The second scroll's goal"). With bit 0 in the same mask the camera position is swapped for the camera's goal around that call and restored after, so the second scroll aims at where the camera will stop |
| 2-6 | 2..6, the followers | party spot `i - 1` from `MapPartyRingEntryGet` (`0x80066FB4`) — spot 0 for all of them when `g_MapPartyKind == 2` — plus `+0x1C` = the spot's byte 4, `+0x23 = 0`, `+0x22 = +0x21` |

Bit 1 is in 0x7E (and 0x2007F) but no slot 1 is touched.
`MoveChrCommandEndWait(mask)` (`0x8005F3C8`) then waits, one frame per
pass (`ScriptFrameStep(1)` (`0x8005A1F0`)), while any of the mask's busy
bits is set. Release: the group's flag is set (`g_MoveChrViewFollow` /
`g_MoveChrSecondViewFollow` / `g_MoveChrPartyFollow` = 1, under which the
view is made from the lead again, the second scroll from the view, and the
followers follow the party's ring), and each named slot's first byte and
live bit are cleared **whether the slot was live or not**. Each
follower's two actor rows (`g_MapPartyRecords` / `g_MapPartyTwins`, row
`i - 1`) are switched off (first byte 0) when `g_MapPartyKind == 2`,
otherwise handed to `MapAnimationPoseSet(row, -1, spot byte 5)` with
party spot `i - 1`. No RNG draw in the function itself; its callees were
not traced for draws.

### One tick of a mover slot — `MoveChrWalkStep` (`0x8005F520`)

`void MoveChrWalkStep(MoveChrRow *row, u16 row_number)` (`0x8005F520`):
one tick of script slot `row_number` (below, `n`), a 0x30-byte `MoveChrRow`
of `g_MoveChrRows`. Two callers, the move-script ops 0xF3, `MoveCommandMoveTo`
(`0x800630A4`), and 0xF4, `MoveCommandMoveBy` (`0x8006312C`; §"Move-script
opcode 0xF4, MOVE BY"), both reached only through
`g_MoveChrCommandFunctions` (`0x800B45F8`).

The slot's fields as this function uses them: `+1` running, `+2` height,
`+8` script pointer, `+0xC` its actor (x `+6`, y `+8`, draw y `+0xA`, flags
`+0xE`), `+0x14` the probe box and `+0x20` the layer handed to the
collision calls, `+0x18` / `+0x1A` the goal, `+0x1C` the goal height,
`+0x1E` passes per tick (the speed `MoveChrFollowStart` sets), `+0x1F`
the height step, `+0x21` the facing last taken, `+0x22` the facing aimed
for, `+0x23` a facing override, `+0x24` / `+0x25` the tick countdown and
its reload.

1. `+0x24` is counted down; unless it reaches 0, return. Otherwise it is
   reloaded from `+0x25`: the slot steps once every `+0x25` calls.
2. Slot 0 is the camera — `MoveChrViewStep(row)` (`0x8005F8A4`) — and slot
   0x11 the second scroll — `MoveChrSecondViewStep(row)` (`0x8005F948`);
   both return at once.
3. Any other slot walks its actor. The height `+2` is stepped toward
   `+0x1C` by `value_goal_step(z, goal, step)` (`0x8005FD40`) once (the step is the byte
   `+0x1F`; §"Step a value toward a goal"), then up to `+0x1E`
   passes run:
   * when `+0x22` equals `+0x21`, the aim is recomputed:
     `MapFacingTowardGet(x, y, goalX, goalY, +0x21)` (`0x8005FBBC`);
   * `dir = MapSpotMoveTry(+0x22, 1, &x, &y, box, layer, g_MoveChrAttributeCheckOn
     == 0)` (`0x8006661C`) steps (x, y) by 1 along it;
   * `done = MoveChrGoalCheck(row, &x, &y, &z, dir)` (`0x8005FDAC`) — whether to stop
     (§"A map object's walk step" has its rule); then a non-zero `dir` is
     kept in `+0x21`;
   * **slot 1:** `MapPartyRingPush(x, y, z, dir)` (`0x80067114`), with `dir` replaced by
     `+0x23` when that is set; then `MapPartyRingFollow()` (`0x800671C4`) when `g_MoveChrPartyFollow` is set,
     otherwise party spot 0 is read back (`MapPartyRingEntryGet(0, &spot)` (`0x80066FB4`)) into
     the actor — x, y, `+0xA = y − spot byte 4`, `+0xE`'s 0x6000 bits — and
     party twin 0 is put on the same spot (`MapPartyTwinSpotSet(0, &spot)`
     (`0x800672E8`));
   * **other slots:** the actor gets x, y and `+0xA = y − z`;
     `MapAttributeClassGet(+0x1E, 0x20, x, y − z, y, box, layer)` (`0x8006960C`) re-classifies
     its footprint into `+0xE`'s 0x6000 bits; and below slot 7 (the
     followers, slots 2..6) party twin `n − 1` is put on that spot
     (`MapPartyTwinSpotSet(n − 1, &spot)` with x, y, z, `+0x21` and the
     class);
   * `+2 = z`; on `done`, `+1 = 0`, the script pointer `+8` advances by 5,
     a non-zero `+0x23` is copied into `+0x21`, and the passes end.
4. Last, the actor's animation is set from its facing:
   `MapAnimationPoseSet(actor, -1, facing)`, the facing being `+0x23`,
   or `+0x21` when `+0x23` is 0.

Step 4 runs on every stepping tick, moved or not. The first argument of
`MapAttributeClassGet` (`0x8006960C`) here is the slot's pass count `+0x1E`, where
`ScriptCommandMoveChrRowActorSet` passes 1.

### Call the released mover groups back — `MoveChrFollowRestart` (`0x800602C4`)

`void MoveChrFollowRestart(void)` (`0x800602C4`): the no-argument twin of
`MoveChrFollowStart`, for the groups that function has released. Two
callers in lns_move.c, `MoveChrPartyJoin` (`0x80060684`) and
`MoveChrPartyLeave` (`0x80060F18`), each as its last statement and only
while `g_MoveChrPartyFollow` is set: the party has changed, and the
places with it.

The mask is built from the group flags `MoveChrFollowStart` leaves
at 1: bit 0 when `g_MoveChrViewFollow` (the camera) is set, 0x7E when `g_MoveChrPartyFollow`
(the followers) is set; with neither it returns. The second scroll's flag
`g_MoveChrSecondViewFollow` (slot 17) is not read here. `MoveChrCommandEndWait(mask)` (`0x8005F3C8`) waits out
the busy bits first, then:

| Group | What |
|---|---|
| camera (bit 0) | `g_MoveChrViewFollow = 0`; slot 0 is opened again (`MoveChrRowInit(0)` (`0x8005F410`)); its goal `+0x18` / `+0x1A` is `MapViewGoalGet` (`0x80069080`)'s; the built-in script `g_MoveChrMoveToListCopy` is both base (`+4`) and pointer (`+8`); `+1 = 1`; busy bit 0 |
| followers (0x7E) | `g_MoveChrPartyFollow = 0`; each slot `i` = 2..6 whose `g_MapPartyRecords` row `i − 1` is on is opened (`MoveChrRowInit(i)` (`0x8005F410`)) and bound again: `+3` = the low byte of member id `i − 1` (`g_PartyIds`), `+0xC` / `+0x10` = its two actor rows (`g_MapPartyRecords` / `g_MapPartyTwins` row `i − 1`), `+0x14 = &g_MapPartyBoxDx`, the goal = party spot `i − 1` from `MapPartyRingEntryGet` (`0x80066FB4`) (spot 0 for all of them when `g_MapPartyKind == 2`), `+0x1C` = the spot's byte 4, `+0x23 = 0`, `+0x21 = +0x22` = the spot's byte 5, the same script, `+1 = 1`, busy bit `i` |

No pace is set: the slots keep `MoveChrRowInit`'s one pass (one pixel) a
tick and a tick every frame, the same as `MoveChrFollowStart`'s entry 3 of the speed
tables. `g_MoveChrMoveToListCopy` holds the same bytes as
`MoveChrFollowStart`'s `g_MoveChrMoveToList` (`F3 00 00 00 00 F9`: one
move-to, then end). `MoveChrCommandEndWait(mask)` (`0x8005F3C8`) then
waits for the slots, and `MoveChrFollowStart`'s release is repeated: the
group flag back to 1, each slot's first byte and live bit cleared —
slots 2..6 whether they were opened or not. Last, outside the 0x7E test
(which, from its two callers, always passes): rows 1..5 of both actor
tables are switched off (first byte 0) when `g_MapPartyKind == 2`;
otherwise each row whose `g_MapPartyRecords` entry is on is handed, in
both tables, to `MapAnimationPoseSet(row, -1, spot byte 5)` with the party
spot of its own index. No RNG draw in the function itself.
`MapViewGoalGet` (`0x80069080`) is called here with no declaration in the
file (implicit int), as in `MoveChrFollowStart`.

### Step a value toward a goal — `value_goal_step(cur, goal, step)` (`0x8005FD40`)

`s16 value_goal_step(s16 cur, s16 goal, s32 step)` (`0x8005FD40`) steps `cur` toward `goal`
by `step` without passing it:

* `cur > goal`: `cur - step`, or `goal` when that has gone below the goal;
* `cur < goal`: `cur + step`, or `goal` when that has gone above the goal;
* equal: `cur` unchanged.

`cur` and `goal` are compared as signed 16-bit and the sum is cut to 16
bits before the overshoot test. `step` is as wide as an int: the code
subtracts and adds the whole argument register, and lns_move.c declares the
function `s16 value_goal_step(s16, s16, s32)`. A pure leaf: no frame, no global,
nothing read or written through a pointer, no call, so no frame wait and no RNG draw.

Five call sites, all in lns_move.c:

* `MoveChrWalkStep` (`0x8005F520`), the mover tick (§"One tick of a mover slot"): a slot's
  height `+2` toward the goal height `+0x1C` by the byte `+0x1F`, once
  per tick ahead of the passes;
* `MoveChrViewStep` (`0x8005F8A4`), the camera slot (slot 0): `g_MapViewX` and
  `g_MapViewY` toward the slot's goal `+0x18` / `+0x1A` by the byte
  `+0x1E`;
* `MoveChrSecondViewStep` (`0x8005F948`), the second scroll (slot 0x11): `g_MapSecondViewX` and
  `g_MapSecondViewY`, by the same two fields.

### The facing to turn to — `MapFacingTowardGet` (`0x8005FBBC`)

`s32 MapFacingTowardGet(s32 x0, s32 y0, s32 x1, s32 y1, u16 facing)`
(`0x8005FBBC`, lns_move.c) gives **the facing to turn to**. An actor at
(x0, y0) that currently faces `facing` wants to head for (x1, y1). The answer
is one of the eight facings, or 0 for "no answer".

Facings are 1..8 clockwise from up with y growing downward (1 up, 3 right, 5
down, 7 left): the numbering of the step tables `g_MapFacingDxs`
(`0x80012C1C`) and `g_MapFacingDys` (`0x80012C24`) that `MapSpotMove`
(`0x80066848`) steps by (dx `0 1 1 1 0 -1 -1 -1`, dy `-1 -1 0 1 1 1 0 -1`;
§"The step primitive"). This function's same-x / same-y answers agree with
them.

It makes no call, no store and reads no global: a pure function of its five
arguments. No RNG draw, no frame wait.

In order:

1. `x0 == x1 && y0 == y1` → 0.
2. `facing == 0` → 0.
3. `x0 == x1` → 5 when the goal is below (`y0 < y1`), else 1.
4. `y0 == y1` → 3 when the goal is to the right (`x0 < x1`), else 7.
5. `adx = |x0 - x1|`, `ady = |y0 - y1|`.
6. `adx == ady` (the exact diagonal): right of the actor 4 (below) / 2
   (above); left of it 6 (below) / 8 (above). The current facing is not
   consulted.
7. Otherwise the bearing lies strictly between two neighbouring facings, and
   the CURRENT facing picks one of the two:

   | where the goal is | bearing between | answer |
   |---|---|---|
   | flatter than the diagonal (`ady < adx`), right, below | 3 and 4 | facing 4..7 → 4, else 3 |
   | flatter, right, above | 2 and 3 | facing 3..6 → 3, else 2 |
   | flatter, left, below | 6 and 7 | facing 3..6 → 6, else 7 |
   | flatter, left, above | 7 and 8 | facing 4..7 → 7, else 8 |
   | steeper (`ady > adx`), below, right | 4 and 5 | facing 1..4 → 4, else 5 |
   | steeper, below, left | 5 and 6 | facing 2..5 → 5, else 6 |
   | steeper, above, right | 1 and 2 | facing 2..5 → 2, else 1 |
   | steeper, above, left | 8 and 1 | facing 1..4 → 1, else 8 |

   One rule sums up the eight rows: of the two facings that bracket the
   bearing it answers the clockwise one when the current facing is one of
   the four facings clockwise from the bearing, and the counter-clockwise one
   otherwise, i.e. the bracket facing nearer to the current facing round the
   circle. The rule is a summary of the table; the table is what the code
   does.

Things that are easy to assume otherwise:

- The answer is never "the nearest of eight directions to the bearing". Off
  the axes and the exact diagonals it depends on the current facing: the
  same two points give two different answers for two actors facing opposite
  ways.
- A facing above 8 is not rejected; it takes the "else" answer of its row.
  Only 0 is special (answer 0).
- The return value 0 means "leave the facing alone" to the callers that test
  it: `MapObjectPartyLook03Step`, `MapObjectPartySeen02Step`,
  `MapObjectPartySeen03Step`, `MapObjectPartySeen04Step` and
  `MapObjectPartySeen06Step` act only on a non-zero answer. `MoveChrWalkStep`,
  `StepWalkStep`, `MapObjectTowardWalk`, `MapObjectPartySeen05Step` and
  `MapObjectTouchApproach` store it as it is, 0 included.
- The coordinates are compared as full ints. The callers pass 16-bit values
  (pixels), so nothing wraps.

**Callers (11 call sites, one in each of these functions)**

| caller | file | passes |
|---|---|---|
| `MoveChrWalkStep` (`0x8005F520`), one tick of a mover slot | lns_move.c | `((s16)x, (s16)y, goal +0x18, goal +0x1A, facing +0x21)`, x / y being the actor's x and ground y as the tick steps them; only while the slot's aim `+0x22` still equals its facing `+0x21`; the answer goes to `+0x22` |
| `StepWalkStep` (`0x800677D4`) | map_think.c | `(x +6, ground y +8, goal +0x54, goal +0x58, facing +0x3E)`; the answer goes to `+0x3E` (§"A map object's walk step") |
| `MapObjectTowardWalk` (`0x800782B8`) | map_look.c | a stepped copy of x / ground y, the goal it is given, facing `+0x3E`; only while `+0x3E` equals `+0x4A` or `+0x4A` is 0, the mover tick's rule again; the answer goes to `+0x4A` |
| `MapObjectAwayWalk` (`0x800784C4`) | map_look.c | a stepped copy of x / ground y, the point to get away from, `+0x3E`; the answer turned round, `((answer + 3) & 7) + 1`, goes to `+0x4A` (an answer of 0 becomes 4) |
| `MapObjectPartyLook03Step` (`0x80078FD8`) | map_look.c | `(x, ground y, target +0x4C / +0x4E, +0x3E)`, the target being bytes 4 / 5 of the object's definition × 8 |
| `MapObjectPartySeen02Step` (`0x80079B18`) | map_look.c | the same with the target from definition bytes 0x23 / 0x24 × 8 |
| `MapObjectPartySeen03Step` (`0x80079CAC`), `MapObjectPartySeen04Step` (`0x80079F2C`), `MapObjectPartySeen05Step` (`0x8007A0B4`) | map_look.c | `(x, ground y, lead x, lead ground y, +0x3E)`, the lead being record 0 of `g_MapPartyRecords` (`+6`, `+8`) |
| `MapObjectPartySeen06Step` (`0x8007A1E0`) | map_look.c | the lead's position, first stored in `+0x4C / +0x4E`; a non-zero answer is turned round, `((answer + 3) & 7) + 1`, before `MapObjectFacingSet` (`0x8007A51C`): the object faces away from the lead |
| `MapObjectTouchApproach` (`0x8007B310`) | map_touch.c | `(pos[0], pos[1], lead x, lead ground y, want)`, only while `want == dir`, the mover tick's rule again |

Every caller uses the answer.

### Move-script opcode 0xF7, PLACE — `MoveCommandPlace` (`0x8006338C`)

Entry 7 of the move-script handler table `g_MoveChrCommandFunctions`
(`0x800B45F8`) (nothing calls it directly): `MoveChrCommandRun`
(`0x80061800`) calls `handler(slot, n)` for each busy mover slot (a
0x30-byte `MoveChrRow`) on its current script byte. A five-byte command
`F7 x y z facing` (x, y in 8-px units) that puts the slot's actor on a spot
at once.

* `n == 0` (the camera): `g_MapViewX` / `g_MapViewY` = x·8 / y·8, then
  `MapViewScrollSet(g_MoveChrSecondViewFollow)` (`0x80068E98`); z and facing
  are skipped.
* `n == 0x11` (the second scroll): `g_MapSecondViewX` / `g_MapSecondViewY`
  the same, then `MapSecondViewScrollSet()` (`0x80069058`).
* Other slots: the actor row (`+0xC`) gets x (`+6`) = x·8 and ground y
  (`+8`) = y·8; the slot's height (`+2`) = z; draw y (`+0xA`) = ground y −
  z. **Slot 1 (the lead)**: `MapPartyRingFill(x·8, y·8, z, +0x21)`
  (`0x80067064`), which fills all 0x60 entries of the party's ring with that
  spot, that facing and the class `MapAttributeClassGet` gives for the
  party's box there, and puts the ring's head at 0; then party spot 0 is read back (`MapPartyRingEntryGet`
  (`0x80066FB4`)) and its class (`+6`) becomes the row's 0x6000 bits of
  `+0xE`; **with `g_MoveChrPartyFollow` set, `MapPartyRingFollow()`
  (`0x800671C4`) runs and the command ends there — the twin is not moved
  and the facing byte is skipped, not applied.** Any other slot:
  `MapAttributeClassGet(+0x1E, 0x20, x·8, draw y, ground y, the box +0x14,
  the layer +0x20)` (`0x8006960C`) sets those bits. Then a live twin row
  (`+0x10`, first byte non-zero) gets the same x, and y·8 as BOTH its
  ground y and draw y (no height taken off), and `MoveCommandFacingSet(slot,
  n)` (`0x80063038`) — opcode 0xF1's handler — reads the facing byte and
  gives the actor's animation that facing.

### Move-script opcode 0xF4, MOVE BY — `MoveCommandMoveBy` (`0x8006312C`)

Entry 4 of the move-script handler table `g_MoveChrCommandFunctions`
(`0x800B45F8`); like 0xF7 it is called by `MoveChrCommandRun`
(`0x80061800`) as `handler(slot, n)`. The table, opcodes 0xF0..0xF9 (each
handler in move_command.c):

| Opcode | Handler | What it does |
|---|---|---|
| 0xF0 | `MoveCommandPoseSet` (`0x80062FF4`) | `F0 pose`: `MapAnimationPoseSet(actor, pose, +0x21)`, the pose with the slot's facing |
| 0xF1 | `MoveCommandFacingSet` (`0x80063038`) | `F1 facing`: the facing into `+0x21`, then `MapAnimationPoseSet(actor, -1, facing)` |
| 0xF2 | `MoveCommandHeightStepSet` (`0x80063080`) | `F2 step`: a byte into `+0x1F`, the pixels a tick moves the height toward the goal height |
| 0xF3 | `MoveCommandMoveTo` (`0x800630A4`) | MOVE TO, five bytes, below |
| 0xF4 | `MoveCommandMoveBy` (`0x8006312C`) | MOVE BY, five bytes, this section |
| 0xF5 | `MoveCommandSpeedSet` (`0x800632D8`) | `F5 speed`: `+0x1E` = `g_MoveSpeedPixels[speed]` (pixels a tick), `+0x25` and its countdown `+0x24` = `g_MoveSpeedIntervals[speed]` (frames between ticks) |
| 0xF6 | `MoveCommandWait` (`0x80063330`) | `F6 n`: wait; `+0x1C` counts down from n once a frame and the script goes on at 0 (a byte of 0 waits 0x10000 frames) |
| 0xF7 | `MoveCommandPlace` (`0x8006338C`) | PLACE, five bytes, above |
| 0xF8 | `MoveCommandScale` (`0x800635A0`) | SCALE, four bytes (§"Move-script opcode 0xF8, SCALE") |
| 0xF9 | `MoveCommandEnd` (`0x800636E4`) | end: clears the list start `+4`, the script pointer `+8` and the slot's bit in `g_MoveChrBusyBits` (the built-in scripts end on F9) |

The command is five bytes, `F4 dx dy dz facing`; `dx`, `dy`, `dz` are
SIGNED bytes, `dx` and `dy` in 8-pixel units. `void
MoveCommandMoveBy(MoveChrRow *row, u16 row_number)`; below, the row is the
slot and its number `n`:

1. Slot not running (`+1 == 0`): the goal is where the mover is now plus
   the three bytes. Slot 0 (the camera): x = `g_MapViewX / 8 + dx`, y =
   `g_MapViewY / 8 + dy`, z = 0. Slot 0x11 (the second scroll): the same
   from `g_MapSecondViewX` / `g_MapSecondViewY`. Any other slot: x = actor x
   (`+6`) `/ 8 + dx`, y = actor ground y (`+8`) `/ 8 + dy`, z = the slot's
   height (`+2`) `+ dz`. The divisions are C's signed `/ 8` (toward zero).
   x, y and z must each be 0..0xFF; if any is not,
   `MoveChrError11Report(slot, n)` (`0x80062FC8`) reports script error
   **0x11** (`g_ScriptCommandWord = n`, `g_ScriptErrorNumber = 0x11`,
   `ScriptErrorReport()` (`0x8005F1C0`)) and the handler returns at once: no
   goal stored, `+1` still 0, the script pointer still on the command, the
   mover not stepped this frame. `ScriptErrorReport` is empty in this
   build, so nothing is shown, and the same test runs again at the next
   call.
   In range: goal `+0x18 = x * 8`, `+0x1A = y * 8`, goal height `+0x1C =
   z`, facing override `+0x23` = the command's last byte, `+0x22 = +0x21`,
   `+1 = 1`.
2. Always, and on every later call while `+1` is set:
   `MoveChrWalkStep(slot, n)` (`0x8005F520`), one tick of the mover; its
   steppers clear `+1` and advance the script pointer by 5 when the goal is
   reached.

No frame wait and no RNG draw of its own. `MoveCommandMoveBy` is the only
caller of `MoveChrError11Report` (`0x80062FC8`) in the image, and it passes
`n` zero-extended (that function's second parameter is a `u16`).

**Opcode 0xF3, MOVE TO — `MoveCommandMoveTo` (`0x800630A4`).** `F3 x y z
facing`, absolute: while `+1 == 0`, `+0x18 = p[1] << 3`, `+0x1A = p[2] <<
3`, `+0x1C = p[3]` (unsigned bytes, no range check), `+0x23 = p[4]`,
`+0x22 = +0x21`, `+1 = 1`; then `MoveChrWalkStep(slot, n)` (`0x8005F520`).
It is the opcode of the built-in scripts `g_MoveChrMoveToList` (used by
`MoveChrFollowStart` (`0x8005FED0`)) and `g_MoveChrMoveToListCopy` (used by
`MoveChrFollowRestart` (`0x800602C4`)), both `F3 00 00 00 00 F9`, whose
users have already filled the goal and set `+1`, so F3 only ticks.

### Move-script opcode 0xF8, SCALE — `MoveCommandScale` (`0x800635A0`)

Entry 8 of the move-script handler table `g_MoveChrCommandFunctions`
(`0x800B45F8`; the word at `0x800B4618`), so **move-script opcode 0xF8**; in
move_command.c. No call reaches it by name: `MoveChrCommandRun`
(`0x80061800`) calls `handler(slot, n)` for each busy mover slot on its
current script byte and then `MoveChrScaleStep(slot)` (`0x8005F9EC`). It is
**SCALE**: it sets, or starts a glide of, the x / y scale of the slot's actor
row and its twin.

`void MoveCommandScale(MoveChrRow *row, u16 unused)`: `row` is the 0x30-byte
`g_MoveChrRows` slot; the second parameter is not used (declared for the
table's call).

**The command**

`F8 ix iy rate`, four bytes. `ix` and `iy` index the scale table:

| index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| `g_MoveChrScales[i]` | 0 | 0x40 | 0x80 | 0xC0 | 0x100 | 0x180 | 0x200 | 0x300 | 0x400 | 0 |

0x100 is 1:1 (index 4 is what `MoveChrRowInit` (`0x8005F410`) gives a fresh
slot: `+0x26 = +0x27 = 4`, `+0x28 = +0x29 = 0`), so the steps are 25 / 50 /
75 / 100 / 150 / 200 / 300 / 400 %. Indices are not range-checked.

1. **A glide is still running** (`+0x28` or `+0x29` non-zero; the two bytes
   are read as one halfword): nothing happens and the script pointer `+8`
   stays on the command. So an F8 that follows an F8 waits for the first
   one's glide; any other command does not.
2. **`rate == 0`, at once.** `+0x26 = ix`, `+0x27 = iy`,
   `MoveChrScaleSet(slot)` (`0x80062F04`).
3. **`rate != 0`, a glide.**
   - `MoveChrScaleSet(slot)` first, with the indices as they still stand;
   - the actor row (`+0xC`) and the twin row (`+0x10`) get `+0x20 = 1`
     (u16, `sprite.scaled`);
   - goals `+0x2C = g_MoveChrScales[ix]`, `+0x2E = g_MoveChrScales[iy]`;
   - period `+0x2A = 1 << (rate - 1)` (stored as a byte: 1, 2, 4 … 128
     frames for rates 1..8), countdown `+0x2B = 1`;
   - direction x `+0x28`: 0 when `ix` equals the slot's index `+0x26`, 1
     when it is below it (shrink), 2 when above (grow); direction y `+0x29`
     the same from `iy` and `+0x27`;
   - last, `+0x26 = ix`, `+0x27 = iy`.
4. In cases 2 and 3 the script pointer `+8` advances by 4. The glide does
   not hold the script.

No frame wait and no RNG draw of its own.

The glide is stepped only while the slot is busy: `MoveChrCommandRun` calls
`MoveChrScaleStep` only for slots whose bit is set in `g_MoveChrBusyBits`,
and `MoveCommandEnd` (`0x800636E4`, op 0xF9) clears that bit. A glide still
running when the slot's list ends stops where it is, short of its goal, until
the slot is busy again (read from the code, not played).

**The two functions around it**

- `MoveChrScaleSet(slot)` (`0x80062F04`, move_command.c): apply the slot's
  indices. Both rows' `+0x20` = 0 when `+0x26 == 4 && +0x27 == 4` (the C
  tests the two bytes at once, `(*(u32 *)(slot + 0x24) & 0xFFFF0000) ==
  0x4040000`), else 1; both rows' `+0x24` (`sprite.scale_x`) =
  `g_MoveChrScales[+0x26]` and `+0x26` (`sprite.scale_y`) =
  `g_MoveChrScales[+0x27]`.
- `MoveChrScaleStep(slot)` (`0x8005F9EC`, lns_move.c): the glide's tick.
  While `+0x28 / +0x29` is non-zero and the countdown `--(+0x2B)` reaches 0:
  per axis, direction 2 adds 0xC to both rows' scale word and direction 1
  takes 0xC off; the direction byte is cleared once the actor row's value is
  no longer short of the goal (`!(row < goal)` growing, `!(row > goal)`
  shrinking, signed 16-bit). When both directions are 0 it calls
  `MoveChrScaleSet(slot)`, which snaps both rows to the table values, and
  returns; otherwise the countdown is reloaded from the period `+0x2A`. So a
  glide moves 0xC (about 4.7 % of 1:1) every `1 << (rate - 1)` frames. The
  first step comes in the same pass as the command: the countdown is 1, and
  `MoveChrCommandRun` calls `MoveChrScaleStep` right after the handler.

### The script actors' animation tick and draw — `MoveChrAction` (`0x800618B4`)

The name is the developers' own: the function's assertion message begins
"MoveChrAction:" and goes on in Shift-JIS, "invalid pattern", with the
file `LNS_MOVE.C` and line 0x3FF. It is not a movement step: it is the
third of the field's three per-frame tick-and-draw passes, beside
`MapPartyAnimationStep` (`0x80065CF8`) (party records) and
`MapObjectsAnimationStep` (`0x80065FD8`) (map objects), for script slots
7..0x10 of `g_MoveChrRows` (0x30-byte `MoveChrRow`s). `MapFrameStep`
(`0x800557D4`) and `ScriptFrameStep` (`0x8005A1F0`) (when its argument is
not 0) call `MoveChrUpdate` (`0x800617D8`), which runs `MoveChrCommandRun`
(`0x80061800`) (steps the movers) and then this.

* First it switches off sprite-list entries 6..0x10 and 0x4C..0x56
  (`sprite_run_off(6, 0xB)` and `sprite_run_off(0x4C, 0xB)`,
  `0x80019A7C`): one entry more at each end than it fills, 0x10 being the
  first map object's and 0x56 the first object twin's. Both callers run
  `MapObjectsAnimationStep` after it, which clears and fills those again.
* Per live slot i (bit i of `g_MoveChrLiveBits`), on the actor record `p`
  (slot `+0xC`) and its twin `q` (slot `+0x10`): the timer `+0x34`
  advances; at the length `+0x36` the step `+0x38` moves on. At the end
  `+0x3A`: if the animation's count has bit 15, the last step holds (step
  back by one, timer 0, that step's length), otherwise
  `MapAnimationStart(p, 1)` (`0x80066468`) restarts it. Not at the end:
  timer 0 and the next step's length, and only on this path is the
  step-list pointer `+0x2C` checked to lie in 0x80000000..0x801FFFFF, with
  `assert_fail` (`0x8001B050`) called otherwise. `assert_fail` is an empty
  function in this build, so the length is read through the pointer
  either way.
* The record's 0x24-byte sprite entry goes to sprite-list entry `i - 1`
  (6..0xF) with the current step's pose.
* A live twin (first byte non-zero) is put on the record's animation
  (`MapAnimationSet` (`0x800663EC`)) and synced to the same distance from
  the end, timer and length; it takes the record's x, and both its ground
  y and its drawn y from the record's ground y, so it stands on the ground
  under a raised actor. It goes to entry `i + 0x45` (0x4C..0x55) with its
  own step's pose.

Unlike the map-object pass there is no camera cull and no held or
behaviour test: every live slot is ticked and drawn. No frame wait, no RNG
draw.

## The message window

The prompts and notices the commands show, in `lns_intp.c`, and the
window's records in `message_disp.c`.

### The message window's answer prompts — `MessageAnswerRun` (`0x8005B6C0`) / `MessagePageRun` (`0x8005B894`)

Both take `(u8 sel, MessageAnswer *str)`, `str` being two 8-byte
`MessageAnswer` records (a u16 string id at `+0`; at `+4` the address of a
string, used when the id is 0) the caller builds on its stack. Each draws the two
strings with `font_string_put(id, string, 1)` (`0x80026F78`), puts the cursor
(`MessageCursorSet(0x110, 0x10, 0x7B1, 0)` (`0x8005B694`); the 0x24-byte render entry
`*g_MessageCursorEntry` gets `x = g_MapViewX + 0x18`, `y = g_MapViewY + sel * 16 +
16`, depth (`+4`) `= g_MapViewY + 0xE0`), starts the pad record `g_MessagePadRecord`
(`pad_record_init(rec, 0x14, 0x5040, 0)` (`0x8002687C`)) and then runs one frame per pass
(`ScriptFrameStep(1)` (`0x8005A1F0`), `pad_record_update(rec, 0x14)` (`0x80026930`)), each pass copying the
cursor entry to `g_SpriteList + 0x14D0`: entry 0x94 of the sprite list, the
entry the close switches off with `sprite_run_off(0x94, 1)`.

| | `MessageAnswerRun` (`0x8005B6C0`) (two answers) | `MessagePageRun` (`0x8005B894`) (four ways) |
|---|---|---|
| caller | `ScriptAnswerRun` (`0x8005C198`), compares the result with 1 | `ScriptPagesRun` (`0x8005C2C0`) |
| rows | `str[0]` on row 3 (pen `0x801CD480`), then `str[1]` on row 4 (`0x801CDD00`); each followed by `ankprint_rows_clear(2 / 4, 1, 0x20)` (`0x80025A68`) and `MessageLineDisp(1)` (`0x8005AD94`) | `str[0]` on row 4 first, then `str[1]` on row 3; each followed by `MessageLineDisp(-1)` (`0x8005AD94`), no `ankprint_rows_clear` (`0x80025A68`) |
| `sel` on entry | the answer to start on | never read: set to 0 |
| each pass | `repeat (+4) & 0x5000` (up or down) flips `sel` (`^= 1`) with sound `SoundPlay(1, 1)` (`0x800555F0`); the cursor's y follows | the cursor's y is set again from `sel`, which stays 0, so the cursor never moves |
| ends on | `press (+2) & 0x40` (CROSS) | the REPEAT word (`+4`), tested in this order: 0x4000 (down) -> 0, 0x40 (CROSS) -> 1, 0x1000 (up) -> 2, 0x10 (TRIANGLE) -> 3 |
| close | `SoundPlay(2, 1)` (`0x800555F0`), `sprite_run_off(0x94, 1)` (`0x80019A7C`), `MessageLinesClearQuick()` (`0x8005BCFC`) | `SoundPlay(1, 1)` (`0x800555F0`), then the same two calls |
| returns | `sel` (0 or 1) | 0..3 |

No RNG. The button names rest on this: `pad_update` (`0x8001F508`) builds
the pad word from the controller's two button bytes, inverted (the first
byte high), and `MapPadStep` (`0x80055EC4`) turns 0x1000 into direction 1
(up), 0x2000 into 3 (right), 0x4000 into 5 (down) and 0x8000 into 7 (left).
That is the PlayStation controller's own byte layout, in which 0x10 is
TRIANGLE and 0x40 CROSS, the names menu-ui.md gives the same two bits in the
battle menu.

### The field's one-line notice — `MessageNoticeDisp` (`0x8005BD84`)

`void MessageNoticeDisp(u16 name_id, u16 msg_id)`: "<verb> <name>" on
text row 5 with a button wait. Both arguments are string ids of the
offset table at `g_FontStringTable` (text.md, "The string table"). Messages
`0x65A..0x65E` are Lost / Got / Use / Gave / Bet. Callers:
`MessageNoticeItemDisp` (`0x8005BB6C`) (§"The item notice and the window's records") with `((item & 0xFF) | 0x100, n + 0x65A)`, an item
name after the verb; `MessageNoticeNumberDisp` (`0x8005BA70`) with `(0, n + 0x65A)` after
drawing a number itself.

1. `g_MessageLine = 5`, sound `SoundPlay(0xC, 1)` (`0x800555F0`); the pen goes to row
   5's buffer (`0x801CBB00 + 5 * 0x880`) with 0x10 cells cleared
   (`font_target_clear` (`0x8002707C`), then `font_target_set` (`0x800270AC`) on the same address).
2. `name_id == 0`: the message alone (`font_string_proportional_put(msg_id)` (`0x80027054`)).
3. `name_id != 0` and its table entry non-zero:
   `debug_printf("XX no1=%x\n", name_id)` (`0x800216EC`), the message, a frame, then a
   space (`font_bytes_put(D_800B87E4, 1)` (`0x80026FD4`)), the name's text
   (`font_string_put(0, text, 1)` (`0x80026F78`)) and another frame.
4. `name_id != 0` with a ZERO table entry: nothing is drawn, not even
   the message.
5. `name_id != 0` either way: `ankprint_rows_clear(4, 1, 0x20)` (`0x80025A68`).
6. `MessageLineDisp(1)` (`0x8005AD94`) draws the row; the pad record `g_MessagePadRecord` is
   started (`pad_record_init(rec, 0x14, 0x40, 0)` (`0x8002687C`)) and the function runs
   one frame per pass until the record's PRESS word (`+2`) has bit 0x40.
   There is no timeout (the message window's `0xFF00` wait in
   `MessageControlRun` (`0x8005AF20`) tests `repeat` and, while
   `g_MessagePressWait` is 0, has one).
7. Close: `MessageLinesClear()` (`0x8005BC74`) (rows cleared), `CellRowsClear(2, 4, 0x20)` (`0x800573E0`),
   `CellTextDisp(3, 2, 0x11, 2, 0x11, 0xDEAD2001, 0xEDCC)` (`0x800572E0`),
   `CellAllUpdate(0x20)` (`0x8005AD8C`), one more frame.

No RNG; it returns nothing. Rodata beside its format string:
`0x8001296C "TextError!"`, `0x80012978 "LNS_INTP.C"`,
`0x80012984 "XX no1=%x\n"`. Its frame holds 16 bytes no instruction
touches; the C carries them as an unused `u8 pad[16]`, as it does in
`GameEnter` (`0x80059498`).

### The item notice and the window's records — `MessageNoticeItemDisp` (`0x8005BB6C`), `MessageRecordsInit` (`0x8005A580`)

**`MessageNoticeItemDisp(item, n)` (`0x8005BB6C`)** shows an item's icon beside the one-line
notice "<verb> <item name>" and waits for the button. Callers:
`ScriptCommandChrItemGive` (`0x8005D0E0`, command 0x1F; n = k + 1) and
`ScriptCommandChrItemRemove` (`0x8005D164`, command 0x20; n = k), both only when k != 1.

1. `MapStatusClear()` (`0x8005A7C8`) (the idle strip's reset; one frame when it acts).
2. `g_MessageNoticeBand = 1` — from now on every field frame queues the dark band
   `packet_box_add(0, 0x14, 0x140, 0x20, 0x2000000, 0x40)` (`0x80015A60`) (x 0, y 0x14,
   0x140 wide, 0x20 high). One frame.
3. Grid rows 3..5 lose their icon cells; `CellIconSet(4, 3, 0xDEAD0001, item, 0)`
   (`0x800573B8`), which is `IconCellSet(4, 3, item, 0)` (`0x8001AF04`) behind a filler
   argument, draws the item's icon at cell (4, 3). The fourth word of the upload request
   at `D_800E4768`, its length, is set to 0xA00; its address word and its word 0 are not
   written. One frame.
4. Grid rows 2..5 lose their label cells; `CellTextDisp(8, 4, 0xF, 1,
   0x11, 0xDEAD2001, 0xEE10)` (`0x800572E0`) places text row 4's strip at grid (8, 4).
   One frame.
5. `MessageNoticeDisp((item & 0xFF) | 0x100, n + 0x65A)` (`0x8005BD84`): the message, a
   space and the item's name; it loops a frame per pass until the confirm
   bit 0x40 is pressed — no timeout — then closes.
6. The icon cells are cleared, `g_MessageNoticeBand = 0`, one frame.

Four frames of its own, plus step 1's frame and those of `MessageNoticeDisp` (its
writing, the button wait and its closing). No RNG call in the function itself.

**`MessageRecordsInit()` (`0x8005A580`)** builds the message window's three display records
back to back at 0x801C9B00 (0x78 bytes, zeroed first): the 0x18-byte piece
`g_MessageCursorPiece` (a `SpritePiece` of kind 0x4000, a box in one colour, colour 0x7B1);
the 0x24-byte sprite entry `g_MessageCursorEntry` (the prompts' cursor: `on` = 1, `flags`
(`+0xA`) = 0x6000, `piece_table` (`+0x14`) = the piece); and the 0x3C-byte `MapRecord`
`g_MessageArrowRecord` (the "more" arrow: its sprite entry on, `+0xE` = 0x6360, five
pointers into the animation bank `g_SpriteData0` taken from that bank's u16 header,
`animation_before` = 0xFFFF, `g_MessageArrowX` = 0x128 its x). `MapAnimationSet(actor, 5)`
(`0x800663EC`) then starts animation 5. No frame wait, no RNG draw. Callers: `ScriptLabelRun`
(`0x80059FDC`), `MessageStringDisp` (`0x8005AB98`) (when `g_ScriptEnded != 0`, message_disp.c —
that is, while NO event script runs) and `ScriptCommandShopRun` (`0x8005E384`).

## The party on the field

The party's six field actors are the 0x3C-byte records `g_MapPartyRecords`
(`MapRecord`) and their twins `g_MapPartyTwins`; the lead is record 0.

### One party slot's field actor and its twin — `MapPartyRecordInit` (`0x800649DC`)

`void MapPartyRecordInit(s32 slot, s32 id)` sets up row `slot` (0..5)
of the party actor rows `g_MapPartyRecords` and of their twins `g_MapPartyTwins`
(`MapRecord`s, 0x3C bytes each). Callers: `MapPartyRecordsInit` (`0x80064898`), for each
of the six slots whose `((u16 *)g_PartyIds)[i]` is non-zero, and
`MoveChrPartyJoin` (`0x80060684`) after it has put a member in a slot (it then
overwrites the row's position with the party spot).

1. `id < 0x1F`: kind = 1 and the twin's sheet = `g_MapTwinSpriteDataNumbers[id16 - 1]`,
   where `id16` is the slot's whole u16 from `g_PartyIds`, not the
   argument. Otherwise the twin's sheet = 0xD and kind = 2.
2. `g_MapPartyKind == 0`: `id += 0xF` — not conditioned on the id.
3. The row's `+0` = the low byte of the slot's member id (the live
   byte), `+1` = 0; `MapRecordInit(row, kind, 0, id)` (`0x80065940`); x
   (`+6`) = 0xA0, the ground y (`+8`) = 0x78, the drawn y (`+0xA`) = 0x78;
   `MapAnimationPoseSet(row, 1, 1)` (`0x8006633C`).
4. The twin = the row's whole 0x3C bytes (a struct copy, made after
   `MapRecordInit` and `MapAnimationPoseSet`, so the twin inherits what they set);
   `MapRecordInit(twin, 1, 0, twin's sheet)`;
   `MapAnimationPoseSet(twin, 1, 1)`.
5. `g_MapPartyKind == 0` or `id >= 0x1F` (the id after step 2): the twin's
   `+0` = 0. So the twin stays on only when `g_MapPartyKind != 0` and the
   member id is below 0x1F.

No RNG draw and no frame wait of its own.

### The party's field sprite sheets — `MapPartySpriteDataLoad` (`0x80063D48`)

It keeps the party members' field sprite sheets loaded.
`void MapPartySpriteDataLoad(u8 skip_load)`. Three calls in the C and each
passes the constant 0, behind `MapPartySpriteDataMissingCount() != 0`
(`0x800647E4`): `MapPartyRecordsInit` (`0x80064898`), `MoveChrPartyJoin`
(`0x80060684`) and `ScriptCommandChr1ChrBChange` (`0x8005D470`;
§"Switch the lead's form").

1. An empty count-down loop (`for (i = 5; i >= 0; i--) {}`), the same
   stub its neighbour `MapPartyImagesSend` (`0x80064084`) has.
2. **Did the party change?** For each entry of the last recorded list
   `g_MapPartySpriteDataIds` (u8[6], up to its first 0) the current list `g_PartyIds`
   (u16[6]) is searched; a recorded member that is no longer there sets a
   flag. With the flag set: `func_80063D30()` (an empty count-down, its
   result unused), a scan of `g_MapPartySpriteDataIds` to its first 0 that does
   nothing, and `sprite_page_image_add(-1, 0)` (`0x8001808C`; §"The field
   sprite sheets' VRAM pages").
3. **Record:** `g_MapPartySpriteDataIds[i] = (u8)g_PartyIds[i]` for all six.
4. **Unless `skip_load`:** `dir = g_ResourceReadBuffer; resource_load(0x1E8,
   dir)` (`0x80020230`) reads resource 0x1E8, a directory of 8-byte
   `SpriteDirectoryRow`s (`data_length` and `data_place` first, both in
   units of 4 bytes), into the scratch buffer. Then for each member id in
   `g_PartyIds`, stopping at the first 0, up to three sheets are loaded —
   each only when its entry in the 16-byte loaded-sprite registry
   `g_MapSpriteDataEntries[sheet].data` (sprites.md) is still 0, and each
   by the same steps: `dst = xmalloc_heap_alloc(dir[n].data_length * 4)`
   (`0x80022F4C`), `inflate_buffer(g_ResourceReadBuffer + dir[n].data_place
   * 4, dst)` (`0x80021D6C`), `g_MapSpriteDataEntries[sheet].data = dst`,
   then a registration call:

   | Sheet | Directory row `n` | When | Registered with |
   |---|---|---|---|
   | the member's own, `id` | `id − 1`, or 0x1E for id 0xCE | always | `sprite_page_image_add(n, dst)` (`0x8001808C`) |
   | `id + 0xF` | `id + 0xE` | ids below 0x1F, and only when `g_MapPartyKind == 0` | `sprite_page4_image_load(n, dst)` (`0x80017EB0`) |
   | the twin sheet `g_MapTwinSpriteDataNumbers[id − 1]` (§"The twin-sheet map and the speed tables") | that value − 1 | ids below 0x1F | `sprite_page_image_add(n, dst)` (`0x8001808C`) |

The `id == 0xCE` test is compiled in the second block as well, where the
id (at most 0x1E + 0xF) can never be 0xCE. The twin map is indexed for
every id below 0x1F, though it is 12 bytes long (`ScriptCommandMoveChrRowActorSet`
admits 14): an id above 12 would read on into the tables that follow.
Which ids the party list really holds was not traced.

### The field sprite sheets' VRAM pages — `sprite_page4_image_load` (`0x80017EB0`) / `sprite_page_image_add` (`0x8001808C`)

The two "registration calls" of §"The party's field sprite sheets", in
sprite_draw.c. They are the field twins of the battle's party page
allocator `BtlPartyImageLoad` (`0x800187D8`, btl_draw.c; battle.md §"The
party bundle"): each keeps an occupancy map of 8-line units, inflates the
sheet's page into `g_LoadBuffer`, `LoadImage`s it 0x80 VRAM words wide at
the slot's corner in `g_SpritePageCorners` moved down `row * 8`, marks its
units, and patches every 12-byte descriptor of the block — `+2` = the page
slot, `+0xA` (v) `+= row * 8`. Only the first unit of a run is tested for
free; the units after it are not looked at. They draw nothing on screen,
wait no frame and take nothing from the RNG.

The directory both read is resource 0x1E8, in `g_ResourceReadBuffer`, one
8-byte row per sheet: `+4` is the page image's length in 4-byte words, 64
to a line of 0x80 VRAM words (lines = length / 64, a signed divide), and
`+6` its offset in 4-byte words from the directory's start. A page needs
`(length / 64 + 7) / 8` units.

* **`sprite_page4_image_load(id, blk)` (`0x80017EB0`)** — a sheet's page
  into slot 4: map `g_SpritePage4UseMap` (12 units), the first free unit
  with room taken, units marked `id + 1`. `id == -1` clears its map and
  returns 0.
* **`sprite_page_image_add(id, blk)` (`0x8001808C`)** — the same for slots 5
  and 6: map `g_SpritePageUseMap` (2 × 0x20 units), slot 5 tried first.
  `id == -1` clears both its columns, calls `sprite_page4_image_load(-1, 0)`
  (so slot 4's map is cleared too) and returns 0. `MapPartySpriteDataLoad`
  (`0x80063D48`) makes that reset when a member it had loaded has left the
  party; `MapSpriteDataFree` (`0x80063C34`), `MapPartyImagesSend`
  (`0x80064084`) and `MapSpriteDataReload` (`0x800644E0`) make it too.
* **A page that fits nowhere is still loaded** — at unit 12 of slot 4, or
  in slot 7 at the row where slot 6's search stopped — and its units are
  marked past the end of the map (original behaviour).

### Reloading the party's field sheets after a fight — `MapPartySpriteDataReload` (`0x800642A4`)

Its only caller is `MapSpriteDataReload` (`0x800644E0`), which the
battle entry `MapBtlRun` (`0x8007AE58`) runs when the field is restored. The caller
marks every loaded sheet stale (each non-zero `g_MapSpriteDataEntries[1..0xFF].data`
becomes 1), reloads the map objects' sheets (`MapObjectsSpriteDataReload` (`0x8006419C`)),
resets the sheet page maps (`sprite_page_image_add(-1, 0)` (`0x8001808C`)), calls
this, points the party's records and twins at their sheets again
(`MapPartySpriteDataSet` (`0x80064624`)), then zeroes every `.data` still equal to 1.

This function reads the sheet directory (resource 0x1E8, `resource_load` (`0x80020230`)) and,
for party slots 0..5 up to the first empty one, reloads each STALE sheet
exactly as `MapPartySpriteDataLoad` (`0x80063D48`) loads a missing one (§"The party's field
sprite sheets"). With `id` the low byte of the slot's member id: the member's own
sheet `id`; for ids below 0x1F, sheet `id + 0xF` when `g_MapPartyKind == 0`; and for
ids below 0x1F, the twin sheet `g_MapTwinSpriteDataNumbers[id − 1]`. Each is inflated
and registered, the `id + 0xF` sheet with `sprite_page4_image_load` (`0x80017EB0`), the
other two with `sprite_page_image_add` (`0x8001808C`). It rebuilds no actor. No RNG
draw. The directory read waits for the drive frame by frame, and here each of those
waits is Sony's `VSync(0)` in `cd_sector_load` (`0x80020E88`): `BtlRun` (`0x80027434`)
has already cleared `g_CdWaitBtlFrame` and `g_BtlRunning` when `MapBtlRun` reloads,
so no battle frame (`BtlFrameStep` (`0x80038D2C`), with the per-frame draws of
frame-rng.md) runs during the reload.

### The party actors' animation tick — `MapPartyAnimationStep` (`0x80065CF8`)

`void MapPartyAnimationStep(void)` (map_animation.c), once per field frame
(callers `MapFrameStep` (`0x800557D4`), `MapPartyStep` (`0x80056248`),
`ScriptFrameStep` (`0x8005A1F0`), `MenuFrameStep` (`0x8006ACB0`)). It is the
field's counterpart of the battle tick `BtlAnimationStep` (`0x800272A8`)
(battle.md "Actor animation"), over the six 0x3C-byte party records
`g_MapPartyRecords[i]` (`MapRecord`) and their twins `g_MapPartyTwins[i]`.

A `MapRecord`'s animation words: `+0x28` the animation table (a 4-byte row
per animation number: a u16 count of steps, whose bit 15 holds the last
step, and the u16 number of the first step), `+0x2C` the step list (4-byte
steps: a pose and a length in frames), `+0x30` the animation number,
`+0x34` the timer, `+0x36` the current step's length, `+0x38` the step,
`+0x3A` one past the last step.

1. `sprite_run_off(0, 6)` and `sprite_run_off(0x46, 6)` (`0x80019A7C`)
   switch off sprite-list entries 0..5 and 0x46..0x4B (`spr = g_SpriteList`,
   0x24-byte entries).
2. For each live record (`+0x00` set): unless it is held (`+0x01`), the
   timer `+0x34` counts up to the length `+0x36`, then the step `+0x38`
   moves on. Below the end `+0x3A` the timer is zeroed and the length read
   from the new step. At the end, bit 15 of the animation's count decides:
   set, the step goes back by one and the last step holds (timer 0, its
   length again); clear, `MapAnimationStart(rec, 1)` (`0x80066468`) restarts
   the animation. Held or not, the 0x24-byte sprite entry at `+0x04` is
   copied to `spr[i]` and the copy's pose (its `+0x18`) is set from the
   current step.
3. For each live twin: `MapAnimationSet(twin, record.animation)`
   (`0x800663EC`) puts it on the same animation,
   `twin.step = twin.end - (record.end - record.step)`, the timer and length
   are copied from the record, and its entry goes to `spr[0x46 + i]` with
   its own step's pose.
4. If the u16 `g_MapPartyBlinkCount` is non-zero it is decremented, and on
   an odd result both entries of every live record get `+0x00 = 0`: the
   party is drawn every other frame, a blink.

`+0x01` is a hold byte: while it is not 0 the record is drawn on its step
but its timer does not count. The menu, the save screens and the shop
(menu_run.c, menu_card.c, shop_screen.c) set it on the members they hold
still, with bit 0x8000 of the draw word `+0x0E` to shade them.

### Swap two party slots — `PartyPlacesSwap` (`0x80064D58`)

`void PartyPlacesSwap(u8 a, u8 b)` (party.c). Only `MoveChrPartyJoin`
(`0x80060684`) and `MoveChrPartyLeave` (`0x80060F18`) call it, always as
`(0, 1)`, and both use it to keep id 0xC at place 1, behind the lead:
`MoveChrPartyJoin` when the joining id is to take place 0 and place 1
holds 0xC (before the places move up to make room), `MoveChrPartyLeave`
when the lead has left, place 0 now holds 0xC and more than one id is
left. It swaps the u16 ids `g_PartyIds[a]` / `[b]`, the 0x3C party records
`g_MapPartyRecords[a]` / `[b]` and their twins in `g_MapPartyTwins` (each
pair through one stack copy), then calls `FaceListSwap(a, b)`
(`0x80076268`) (face_list.c), which swaps the two slots' 5-byte
status-strip rows in `g_FaceListEntries` and their slot ids in
`g_FaceList`. **Original behaviour: the id temporary is a byte** — slot a
gets slot b's whole u16, slot b only the low byte of slot a's old id. No
frame wait, no RNG.

### The talk / examine check — `MapPartyTalkCheck` (`0x80067D1C`)

Its only caller, `MapPartyStep` (`0x80056248`), prints "PartyTalkCheck1" and
calls it when `g_MapPadPressedButtons == 0x40` (and not while debug is
allowed with button 0x100 held).

* **Facing**: `dir` = byte `+5` (the facing) of the current 8-byte entry of
  the party's ring (`g_MapPartyRingEntries[g_MapPartyRingHead[0]]`).
* **Probe rectangle**, published for `MapBoxTouchGet` (`0x80067BFC`): centre
  `g_MapProbeBoxX` / `g_MapProbeBoxY` = party actor record 0's x / ground y
  plus the s8 offsets `g_MapPartyBoxDx` / `g_MapPartyBoxDy`; half sizes
  `g_MapProbeBoxHalfWidth` / `g_MapProbeBoxHalfHeight` =
  `g_MapPartyBoxHalfWidth` / `g_MapPartyBoxHalfHeight` (the four bytes
  `MapPartyBoxSet` (`0x8006497C`) sets).
* **Scan**: the 0x80 map objects (`g_MapObjects`) in order, stopping at the
  FIRST hit. A row whose first byte is 0, 4, 5 or 6 is skipped.
  `MapBoxTouchGet(x + box.dx, ground y + box.dy, &box)` (`0x80067BFC`) says
  which side of the probe the row's box (`+0x50`) touches, numbered as the
  facings, and the row is a hit only when the lead faces that side: 3 ->
  dir 2..4; 7 -> dir 6..8; 5 -> dir 4..6; 1 -> dir 0..2 or 8; 0 (apart),
  the even corner codes and 9 (overlapping) never hit.
* **Hit**: the event is byte `+0xB` of the row's definition
  (`g_MapObjectDefinitions[` the row's `+1` byte `]`). A chest (row `+0 ==
  7`): `MapObjectKind7Run(index, event)` (`0x80067F84`). Otherwise, with a
  non-zero event: the row is turned to face the lead
  (`MapAnimationPoseSet(row, -1, ((dir + 3) & 7) + 1)`),
  `g_MapTalkObjectDefinition` = the row's `+1` byte, and
  `ScriptLabelRun(event)` (`0x80059FDC`) runs it; if
  `g_MapTalkObjectDefinition` is not 0xFF afterwards the row gets its own
  facing back (`+0x3E`) and `g_MapTalkObjectDefinition = 0xFF`.

No frame wait or RNG draw in the function itself.

### The party's position ring stepped back — `MapPartyRingRewind` (`0x80066F54`)

No arguments, nothing returned (map_ring.c). Its one caller is
`MapAttributeKind5Run` (`0x80059C7C`, map_change.c), the handler of kind 5 of the
map-cell event word (§"The map-cell events"), which calls this and then starts
map script `arg` with `ScriptLabelRun` (`0x80059FDC`).

**The ring** is 0x60 8-byte `MapPartyRingEntry` entries at the address in
`g_MapPartyRingEntries` (0x801EA000), with the head index `g_MapPartyRingHead` (0..0x5F).
`MapPartyRingPush` (`0x80067114`, map_ring.c) moves the head on one (0x5F wraps to 0) and
writes the lead's new place there; `MapPartyRingFollow` (`0x800671C4`) puts live party
actor k (the rows of `g_MapPartyRecords`) at the entry 16 * k behind the head.

This function moves the head BACK `g_MapPartyWalkStepCount` entries, one at a time, 0
wrapping to 0x5F, and then calls `MapPartyRingFollow()` (`0x800671C4`). Only the index is
written, no entry. With `g_MapPartyWalkStepCount == 0` the head is left where it is
and `MapPartyRingFollow` (`0x800671C4`) still runs. No RNG draw and no frame wait of its own.

`g_MapPartyWalkStepCount` is the lead's steps per frame (`MapPartyBoxSet` (`0x8006497C`): 1 with
`g_MapPartyKind == 0`, else 2). By io-hardware.md §"The field's pad poll",
`MapPartyStep` (`0x80056248`) calls `MapPartyWalkStep` (`0x800664D4`) up to that many times a
frame, and each call pushes at most one entry. The rewind goes back by the full count
whatever the frame pushed: `MapPartyStep` stops at the first `MapPartyWalkStep` that
answers 0, and a step that `MapPartyObjectsFreeCheck` (`0x800679F4`) refuses pushes nothing,
so a frame can push fewer entries than the head is moved back.

The bytes of this function show the index is a member of a two-byte aggregate
(the store goes through a register that holds the symbol's address); they
do not show which aggregate. Today's C declares it `u16 g_MapPartyRingHead[1]` in
map_ring.c, map_walk.c and map_talk.c and spells it `g_MapPartyRingHead[0]`.
`MapPartyRingPush` (`0x80067114`) reads and writes the same index.

### The field's d-pad map — `MapPadDirectionsSet(n)` (`0x80066F04`)

`void MapPadDirectionsSet(s32 n)` (`0x80066F04`, map_walk.c) sets which way each d-pad
direction walks. Row `n` of a six-row, nine-byte table at `g_MapPadDirectionsRows`
(`n >= 5` is taken as 5) is copied, each byte widened to a halfword, into the nine `u16`
direction codes at `g_MapPadDirections`. No callee, no RNG draw, no frame wait,
nothing returned.

`g_MapPadDirections[k]` (.bss, `u16[9]`; the next symbol is 0x18 on) is what the
field's pad poll `MapPadStep` (`0x80055EC4`) returns for d-pad index k (io-hardware.md
§"The field's pad poll"): 0 nothing held, then clockwise from up: 1 up, 2
up-right, 3 right, 4 down-right, 5 down, 6 down-left, 7 left, 8 up-left.
The returned code is the direction handed to `MapPartyWalkStep` (`0x800664D4`). The table
is .bss, so every direction reads 0 until the first call. map_step.c declares
`g_MapPadDirections` as `u8 []` and reads the halfword through a cast.

`g_MapPadDirectionsRows` (.data, `u8[6][9]`, 54 bytes and 2 of padding before
`g_MapTblNumbers`), read from the EXE image:

| row | bytes | meaning |
|---|---|---|
| 0 | 0 1 2 3 4 5 6 7 8 | as pressed |
| 1 | 0 7 8 1 2 3 4 5 6 | a quarter turn anticlockwise (up walks left) |
| 2 | 0 7 6 5 4 3 2 1 8 | mirrored: up <-> left, right <-> down |
| 3 | 0 3 2 1 8 7 6 5 4 | mirrored: up <-> right, down <-> left |
| 4 | 0 3 4 5 6 7 8 1 2 | a quarter turn clockwise (up walks right) |
| 5 | all 0 | the pad moves nobody |

Callers (five sites):

* with 0: `MapChange` (`0x8005922C`) and `GameEnter` (`0x80059498`) (map_change.c, entering
  the field), `MapPadStep` (`0x80055EC4`) (map_step.c, when the lead is outside the box
  `g_MapPadDirectionsState` guards and no colour fade runs) and `MapAttributeFind`
  (`0x80069A20`) (on entry, when `g_MapPadDirectionsState` is 0);
* with `(rng_draw() & 3) + 1` (`0x8001C770`), one RNG draw picking a row 1..4:
  `ScriptCommandMapPadDirectionsChange` (`0x8005EB98`) (command 0x56), which then sets
  `g_MapPadDirectionsState = 1` and the box `g_MapPadDirectionsBoxLeft`, `…Right`, `…Top`,
  `…Bottom` around the lead.

No caller passes 5 or more, so nothing selects row 5 through this
function.

**A second writer of the table.** Three functions touch `g_MapPadDirections`: the pad
poll (a read), this function, and `MapAttributeFind` (`0x80069A20`, map_over.c;
§"The map-cell event probe"). That one calls this function with 0 on
entry when `g_MapPadDirectionsState` is 0, and where a scanned map cell's event is of
kind 0xB it sets `D_800B8E0A = 1` and stores entries 3 and 7 (right, left)
= 2 and 6 when the event's own argument (`id & 0x7F`) is 3, = 4 and 8 when
it is 7, so right and left walk a diagonal.

## The map objects

A map's objects are 0x80 rows of 0x60 bytes at `*g_MapObjects`
(`MapObject`), each with a 0x3C-byte twin at `*g_MapObjectTwins`, spawned
from the 0x26-byte definitions (`MapObjectDefinition`) of the map's object
set (§"A map's object definitions come from its object set, not its map
file").

### Load a map's object set — `MapObjectsDataLoad` (`0x80065418`)

`void MapObjectsDataLoad(u8 set_number)` (`0x80065418`, map_object.c)
**loads a map's object set** (`set` below).

Two callers, both in map_object.c; each passes its argument masked to a byte,
calls this as a statement and then tests its own copy of the set:
- `MapObjectsRestore` (`0x80065098`), which `GameEnter` (`0x80059498`) calls
  with the saved set and object list: `set = arg & 0xFF;
  MapObjectsDataLoad(set);` then, when the set is non-zero,
  `MapObjectRestore` (`0x80065128`) for each of the 0x80 0x28-byte entries
  of the list whose first byte is not 0 (§"Spawn a map's objects from a saved game").
- `MapObjectsInit` (`0x800655C8`), which `MapChange` (`0x8005922C`) calls
  with the scene's object set: the same, then its own spawn loop when the set
  is non-zero (§"A map's objects spawned from the set, and a lookup nothing
  calls").

Steps, in source order:

1. `g_MapObjectsDataNumber = set` (u16 global: the current object set).
2. `mem_clear(g_MapObjects, 0x3000)` (`0x8002609C`) and
   `mem_clear(g_MapObjectTwins, 0x1E00)`: the two map-object tables are
   zeroed (0x80 rows of 0x60 and their 0x80 twins of 0x3C) **whatever the
   set is**, set 0 included.
3. `g_MapObjectsDataNumber == 0`: return. Set 0 is "no objects".
4. `resource_load(((u16 *)g_MapObjectsDataResourceNumbers)[set],
   g_ResourceReadBuffer)` (`0x80020230`): the set's resource number comes
   from the EXE table `g_MapObjectsDataResourceNumbers` (a u16 for each set)
   and the resource of LUNADATA.FIL is read to the work buffer
   `g_ResourceReadBuffer`. `hdr = (s32 *)g_ResourceReadBuffer` is taken
   BEFORE the read (it is the read's second argument), and the global is
   read again after every call.
5. `inflate_buffer(g_ResourceReadBuffer + hdr[0], g_MapObjectDefinitions)`
   (`0x80021D6C`): the packed object definitions at buffer + `hdr[0]` are
   inflated to `g_MapObjectDefinitions` (0x80 definitions of 0x26 bytes,
   then, at +0x1300, the step lists `g_MapObjectStepLists` points at).
6. `list = (MapObjectsSpriteDataRow *)(g_ResourceReadBuffer + hdr[1])`: a
   list of 6-byte sheet loads (`sprite_data` +0, `length` +2, `place` +4),
   ended by an entry whose `sprite_data` is 0. For each entry, with `id =
   list[i].sprite_data`:
   - `g_MapSpriteDataEntries[id].data == 0 && id < 0x100`: the sheet is not
     loaded yet and the id is in the table. The entry is read before the
     range test, so an id of 0x100 or more reads past the table (0x100
     `MapSpriteDataEntry` rows of 0x10 bytes; `MapRecordsClear`
     (`0x80063A8C`, map_sprite.c) clears its 0x1000 bytes) before it is
     turned away.
   - then `src = g_ResourceReadBuffer + hdr[1] + list[i].place * 4`, `dst =
     xmalloc_heap_alloc(list[i].length * 4)` (`0x80022F4C`) (the allocator
     the party-sheet loaders use), `inflate_buffer(src, dst)`,
     `g_MapSpriteDataEntries[id].data = dst`.
   A sheet that is already loaded is kept; nothing in this function frees one
   or registers a VRAM page (the party loaders call `sprite_page_image_add`
   (`0x8001808C`) / `sprite_page4_image_load` (`0x80017EB0`) after each
   load; this one does not).
7. `g_MapSpriteDataEntries[0].data = (s32)g_SpriteData1`: entry 0 of the
   sheet table is pointed at `g_SpriteData1`, the second of the three sprite
   blocks `sprite_reset` (`0x80018DBC`) unpacks from resource 2: the treasure
   chest (docs/spec/assets/battle-ui.md, "Bank 1 (`g_SpriteData1`, 4
   frames)").
8. `sprite_page_list_load()` (`0x800182B8`, sprite_draw.c), the page-slot
   loader of [sprites.md](../../sprites.md) §"The sprite pages":
   it walks the u32 offsets from `g_ResourceReadBuffer + 8` on, inflating
   and `LoadImage`-ing each. So the resource's header is `{defs offset,
   sheet-list offset, page offsets..., 0}`.
9. An empty loop of 256 passes (`for (i = 0xFF; i >= 0; i--) {}`), then
   return.

No RNG draw and no frame wait of its own; `resource_load` is the disc read.

### A map's objects spawned from the set, and a lookup nothing calls — `MapObjectsInit` (`0x800655C8`), `MapObjectDefinitionSpotFind` (`0x800684FC`)

**`MapObjectsInit(set)` (`0x800655C8`)** (called by the scene change `MapChange` (`0x8005922C`) with
the scene entry's byte `+1`, `MapEnterRow.object_set`): `MapObjectsDataLoad(set & 0xFF)`
(`0x80065418`) loads the set; set 0 ends there. Otherwise the 0x80 definitions of
`g_MapObjectDefinitions` (`MapObjectDefinition`, 0x26 bytes) are walked, i = 0..0x7F:

* `+1` (the sheet) == 0: skipped.
* kind = (`+0` & 0x3F) + 1. When `+2` is non-zero a story bit is tested
  with `StoryBitCheck` (`0x80057778`) — bit `0x580 + (+2)` for kind < 4, otherwise
  `(u16)(i + 0x680 + ((g_MapNumber − 0x65) << 7))`, a per-map, per-object
  bit — and a SET bit skips the object (already taken, opened or gone).
* Otherwise `MapObjectInit(g_MapObjects + n * 0x60, g_MapObjectTwins + n *
  0x3C, definition, i)` (`0x800656CC`) and n++: the rows are packed in spawn order.

Its frames are the set load's. The bytes show a statement in the arm with no story bit
that leaves no code (today's C writes a dead `r = 0;` there); which statement the
original had cannot be recovered.

**`MapObjectDefinitionSpotFind(sheet, &x, &y, &kind)` (`0x800684FC`)** (map_load.c) is called by
nothing in the image (no `jal`, `j`, branch, data word or address-forming pair; the JP
build's twin is uncalled too). It would look an object up by sprite sheet:
the first definition whose `+1` equals `sheet` gives `*x = +4`, `*y = +5`
(8-pixel cells, raw bytes) and `*kind` = (`+0` & 0x3F) + 1; if that
definition's story bit (the same two forms as above) is set, `*x = *y =
−0x80`. No match writes nothing. It has an `int` return type and returns
no value.

### Spawn a map object from its definition — `MapObjectInit` (`0x800656CC`)

`void MapObjectInit(row, twin, info, u8 id)`: `MapObjectRestore`
(`0x80065128`)'s steps without a saved game's entry. Callers
`MoveChrPartyLeave` (`0x80060F18`), `ScriptCommandMapObjectsAdd`
(`0x8005D7E0`) and `MapObjectsInit` (`0x800655C8`) each pass a row of
`g_MapObjects` (a `MapObject`), its twin in `g_MapObjectTwins`, a 0x26-byte
`MapObjectDefinition` of `g_MapObjectDefinitions` and that definition's
index. In `MapObjectsInit` (`0x800655C8`) the rows are packed: the row is
number n of the objects spawned so far, not row i.

Definition fields read here: `+0` bits 0..5 = kind − 1, bit 0x40 -> row
`+0x3D`, bit 0x80 -> `MapAttributeClassGet` (`0x8006960C`)'s layer
(`+0x3C`); `+1` the sheet (0 when the kind is 7); `+3` the render mode;
`+4` / `+5` x / y in 8-px units (ground y and draw y the same); `+6` the
facing − 1 (`+0x3E`); `+7` the four footprint bytes (`+0x50`); `+0xC` OR'd
into `+0x3D`.

Order: row `+1` = id, `+0` = kind; `MapRecordInit(row, kind, mode, sheet)`
(`0x80065940`); position and bytes; `+0x42 = 0`; the box; `n =
MapObjectMoveSet(row, info, 0)` (`0x80065A2C`) — **here its result IS the
pose** (`MapObjectRestore` ignores it and derives the pose from the saved
entry's animation number); the footprint class
`MapAttributeClassGet(1, 0x20, x, draw y, ground y, &box, layer)` put into
bits 0x6000 of `+0xE`.

**Chests.** Kind 7 with `+0x3E >= 5` tests event flag `id + 0x2620 +
((map > 100 ? map − 0x35 : map − 0x15) << 5)` (`StoryBitCheck`
(`0x80057778`), map = `g_MapNumber`); if it is set, `+0x3E −= 4` — the
OPENED state. `MapObjectKind7Run` (`0x80067F84`) makes the same −4 when it
opens the row (any event byte below 0xF0) and sets the same flag ONLY after
silver or an item was taken — not for an empty chest, and not when the
item is put back (§"Chests, examine cells and the item notice"). Then
`MapAnimationPoseSet(row, n, +0x3E)` (`0x8006633C`). A definition sheet
below 0xF also fills the twin (the row's first 0x3C bytes, then
`MapRecordInit` with sheet `g_MapTwinSpriteDataNumbers[sheet − 1]`) and
gives it the DEFINITION's facing (`info +6 + 1`) — so an opened chest's
twin gets the closed value (original behaviour). No frame wait or RNG draw
anywhere in its call tree.

### Spawn a map's objects from a saved game — `MapObjectRestore` (`0x80065128`)

`MapObjectsRestore(set, entries)` (`0x80065098`), called only by
`GameEnter` (`0x80059498`), makes the map's objects again from the entries
a saved game keeps of them. `MapObjectsDataLoad(set)` (`0x80065418`)
clears the rows and loads the set's data; then, when the set number is not
0, `MapObjectRestore(entry, row i of g_MapObjects, twin i of
g_MapObjectTwins)` runs for each of the 0x80 entries of 0x28 bytes whose
first byte is not 0. `MenuSaveWrite` (`0x80074024`) writes the entries, one
for each row in use; `MenuLoadRead` (`0x80074824`) lays them out again, one
for every row, behind the game's 0xC00-byte block (at `0x801D0700` where
`GameEnter` takes them; §"Entering the field from the start screen or a load" has
the path with no save).

| Entry (`MapObjectSaveEntry`, 0x28 bytes) | |
|---|---|
| `+0` | the kind; 0 for a free row |
| `+1` | the number of the row's 0x26-byte definition at `g_MapObjectDefinitions` |
| `+2`, `+4`, `+6` | x, ground y, drawn y |
| `+8` | the animation number the row was on |
| `+0xA..+0xE` | the row's `+0x3D`, `+0x3E`, `+0x3F`, `+0x41`, `+0x42` (number, facing, speed, interval, turn count) |
| `+0xF..+0x17` | the row's `+0x43..+0x4B` (the look and its state) |
| `+0x18`, `+0x1C` | the row's `+0x4C`, `+0x4E` and `+0x54..+0x5A`, as 16-bit words |
| `+0x24` | the row's step-list address `+0x5C` |

The think timer `+0x40` is not saved: it keeps what `MapObjectMoveSet`
sets.

| Definition (0x26 bytes) | |
|---|---|
| `+0` bit 0x80 | the row's layer `+0x3C` (`MapAttributeClassGet` (`0x8006960C`)'s layer) |
| `+1` | the sprite data (0 is used when the kind is 7) |
| `+3` | `MapRecordInit`'s mode |
| `+6` | the facing, less one: the twin is turned to it + 1 |
| `+7` | the four box bytes for the row's `+0x50` |

Order: the kind and the definition number stored,
`MapRecordInit(row, kind, mode, sprite data)` (`0x80065940`), x and both
y's, the layer, the number, facing and turn count, the box,
`MapObjectMoveSet(row, definition, 0)` (`0x80065A2C`) (its answer is not
used, and the entry's bytes then overwrite most of what it set), the rest
of the entry's copies, the pose `n = (animation - 1) / 4 + 1` (the group of
the saved animation, §"Group and facing to animation id"), the footprint
class OR'd into bits 0x6000 of `+0xE`, then
`MapAnimationPoseSet(row, n, row+0x3E)`. A definition sprite data below
0xF also makes the twin: a copy of the row's first 0x3C bytes,
`MapRecordInit` with sprite data `g_MapTwinSpriteDataNumbers[sprite data -
1]`, and `MapAnimationPoseSet(twin, n, definition +6 + 1)`: the definition's
facing, not the saved one, until `MapObjectsAnimationStep` puts the twin
on the row's animation the next time the row is drawn. `MapObjectInit`
(`0x800656CC`) makes a row the same way from the definition alone
(§"Spawn a map object from its definition"). No frame wait, no RNG.

### Sheets reloaded after a fight — `MapObjectsSpriteDataReload(set, skip_low)` (`0x8006419C`)

Its caller `MapSpriteDataReload` (`0x800644E0`), called by `MapBtlRun` (`0x8007AE58`) after
the battle, first marks every loaded sheet stale (each non-zero `data` word of
`g_MapSpriteDataEntries[1..0xFF]` becomes 1). This function then reads the object set's
resource again (`resource_load(g_MapObjectsDataResourceNumbers[set], g_ResourceReadBuffer)`
(`0x80020230`)), walks the same list of 6-byte `MapObjectsSpriteDataRow` entries
(`sprite_data`, `length`, `place`; ended by a `sprite_data` of 0) that
`MapObjectsDataLoad` (`0x80065418`) walks, and for each entry still marked stale — ids below
0x1F passed over when `skip_low` is set — allocates (`xmalloc_heap_alloc(length * 4)`
(`0x80022F4C`)), inflates (`inflate_buffer` (`0x80021D6C`)) and stores the new pointer.
Nothing else of the set loader happens: no object definitions, no page images, no VRAM
page registration. Its frames are the disc read's.

### A map object's behaviour set-up — `MapObjectMoveSet(row, info, arg2)` (`0x80065A2C`)

Sets the object's behaviour state from its 0x26-byte definition
(`MapObjectDefinition`) and returns the animation group. A leaf. Callers: the
two spawners `MapObjectRestore` (`0x80065128`) and `MapObjectInit` (`0x800656CC`)
(`arg2 = 0`; only `MapObjectInit` uses the result) and `MapObjectNumberReset` (`0x8007A408`)
(map_look.c), which first puts the definition's `+0xC` into the row's number
bits and passes `arg2 = 1`. Mode = the row's number, `+0x3D & 0x3F` (the entry
of `vtbl_4658_table` the row thinks with).

* **Mode 0 or 6 — a roamer; returns 1.** With `arg2 == 0` the ROAMING
  RECTANGLE is set in pixels from four definition bytes in 8-px cells —
  `+0x54` = x + (s8)info[0xE]·8 (not below 0), `+0x56` = x + (info[0xF] +
  1)·8 (not above 0x7FF), `+0x58` = draw y + (s8)info[0x10]·8 (not below
  0), `+0x5A` = draw y + (info[0x11] + 1)·8 (not above 0x7FF) — and five
  parameter bytes come from info `+0x14..+0x18`; with `arg2 != 0` the
  rectangle is kept and the five bytes come from `+0x19..+0x1D`. The speed
  class c = info[0xD] indexes the two speed tables at entry c + 2 for row
  kinds 4, 5, 6 and c + 3 for all others: `+0x3F` (steps per think) =
  `g_MoveSpeedPixels[…]`, `+0x41` (think period) = `g_MoveSpeedIntervals[…]`. The five
  bytes go to `+0x43..+0x47`; `+0x48`, `+0x4A`, `+0x4B` are zeroed.
* **Mode 1 or 2 — a scripted mover.** `+0x3F = +0x41 = 1`, `+0x4A = 0`,
  `+0x56 = 0`; the step list is entry info[0x12] of the u16 offset table
  at `g_MapObjectStepLists` — `+0x5C` = its address, `+0x5A` (the offset
  `StepCommandRun` reads it at) = 2 — and the function returns the list's byte 1.
* **Any other mode**: `+0x41 = 1`, returns 1.

Last, always, the think timer `+0x40 = +0x41`.

### The map objects' animation tick and draw — `MapObjectsAnimationStep` (`0x80065FD8`)

`void MapObjectsAnimationStep(void)` (map_animation.c) is
`MapPartyAnimationStep` (`0x80065CF8`)'s counterpart for the 0x80 map
objects (0x60-byte `MapObject` rows at `*g_MapObjects`) and their twins
(0x3C-byte `MapRecord`s at `*g_MapObjectTwins`). Callers: `MapFrameStep`
(`0x800557D4`), `MapObjectsStep` (`0x800563D4`) and `ScriptFrameStep`
(`0x8005A1F0`), once a frame each; `ShopOpen` (`0x800506EC`), `ShopClose`
(`0x800512C4`), `MenuOpen` (`0x8006A328`) and `MenuClose` (`0x8006A81C`),
each once after it has changed `g_MapObjectsSpriteBits`.

1. `sprite_run_off(0x10, 0x20)` and `sprite_run_off(0x56, 0x10)`
   (`0x80019A7C`) switch off entries 0x10..0x2F and 0x56..0x65.
2. A row is skipped when `+0x00` is 0 or has bit 0x80 (held by a script
   slot).
3. The animation tick (the same timer / step / hold-or-restart steps as
   `MapPartyAnimationStep`, on `+0x28..+0x3A`) runs unless the behaviour
   `+0x3D & 0x3F` is 3, or is 6 with `(+0x43 & 0x7F) == 1` and
   `+0x46 == 0`.
4. Cull, after the tick: nothing is drawn when the drawn y `+0x0A` is
   outside `g_MapViewY - 100 .. + 0x144` or x `+0x06` outside
   `g_MapViewX - 100 .. + 0x1A4`.
5. Draw: the 0x24-byte sprite entry at `+0x04` goes to entry `0x10 + n`,
   its pose (`+0x18`) is set from the current step, its draw word (`+0x0A`)
   gets `|= g_MapObjectsSpriteBits`, `n++`.
6. The drawn row's live twin, while `m < 0x10`:
   `MapAnimationSet(twin, row.animation)` (`0x800663EC`),
   `twin.step = twin.end - (row.end - row.step)`, timer and length copied
   from the row, x, drawn y and ground y copied from the row, the entry to
   `0x56 + m` with the twin's own step's pose and
   `+0x0A |= g_MapObjectsSpriteBits`, `m++`.
7. The pass ends after 0x20 drawn rows, so the rows after them are not
   animated that frame either.

The twins' run of entries starts 0x46 after the bodies' (0x10 + 0x46 =
0x56), the constant `MapPartyAnimationStep` and `MoveChrAction` use too;
but a map object's twin takes the next free twin entry, so it is 0x46 above
its body only while every drawn row before it has a twin. Of the first 0x6E
entries, 0..5 are the party records, 6..0xF the script actors
(§"The script actors' animation tick and draw"), 0x10..0x2F the map objects, 0x46..0x4B the party
twins, 0x4C..0x55 the script actors' twins and 0x56..0x65 the object twins.

The behaviour test of step 3: behaviours 3 and 4 never think (the think
pass below skips both), so such an object stands; 3 also keeps its
animation still and cannot be touched (`MapObjectTouchFind`), 4 is
animated. Behaviour 6 is `MapObjectPartyLookStep` (`0x80077F1C`): its look
`+0x43 & 0x7F` is an index of the 14 functions at
`g_MapObjectPartyLookFunctions` (`0x800B5B3C`), and look 1,
`MapObjectPartyLook01Step` (`0x80078E94`), stands where it is and only
watches for the party. `+0x46` is the look's fixed length (0 = drawn at
random). So the test freezes the animation of a standing look with no
fixed length.

`g_MapObjectsSpriteBits` is ORed into the draw word of every object and
twin drawn; bit 0x8000 of that word is colour table 1. `ShopOpen` and
`MenuOpen` set it to 0x8000 part way through opening and call this function
once; `ShopClose` and `MenuClose` set it back to 0 and call it again. So
the map objects carry the bit while the shop or the field menu is open.

### The map objects' think pass — `MapObjectsThink` (`0x8006736C`)

`void MapObjectsThink(void)` (map_think.c), called once per field frame
from `MapObjectsStep` (`0x800563D4`): one pass over the 0x80 map objects
(0x60-byte rows at `*g_MapObjects`), then `MapObjectTouchFind()`
(`0x8007AB64`), the touch scan (its own section below).

A row is skipped when its kind `+0x00` is 0 (a free row) or 7 (a chest: it
never thinks); when its behaviour `+0x3D & 0x3F` is 3 or 4; when its drawn
position is off camera (`+0x0A` outside `g_MapViewY - 0x64 .. + 0x144`,
`+0x06` outside `g_MapViewX - 0x64 .. + 0x1A4`); when `+0x00 & 0x80`
(bound to a script slot by `ScriptCommandMoveChrRowActorSet`
(`0x80061EAC`), event-script command 0x34); or when it is outside the
map's limits (`+0x06` against `g_MapLimitLeft .. g_MapLimitRight`, ground y
`+0x08` against `g_MapLimitTop .. g_MapLimitBottom`).

The rest are staggered on `g_MapObjectsThinkPhase` (u16), which this
function steps 0, 1, 2, 3, 0 at its end: a row thinks only when its index's
low bit equals the counter's, and a row of kind below 4 only when
`(index & 3) == counter`. So kinds 1 to 3, the objects the party talks to
(`MapPartyTalkCheck` (`0x80067D1C`)), think one frame in four, and kinds 4
to 6, the ones it touches (`MapObjectTouchFind`), one frame in two. The
code gives no reason for the difference.

A row that thinks counts its think timer `+0x40` down; only when it
reaches 0 is it reloaded from the interval `+0x41` and
`vtbl_4658_table[behaviour](row, index)` called. On a non-zero answer (the
row moved) it recomputes its footprint class,
`+0x0E = MapAttributeClassGet(1, 0x20, x, drawn y, ground y, row + 0x50, +0x3C) | (+0x0E & 0x9FFF)`
(`0x8006960C`), and refreshes its animation with
`MapAnimationPoseSet(row, -1, +0x3E)` (`0x8006633C`).

`vtbl_4658_table` (`0x800B4658`, the data label the C uses) holds the nine
behaviours. The label covers five words: entries 5 to 8 lie under the next
label, `overworld_state_table` (`0x800B466C`), and the word after entry 8
is not a function address.

| Behaviour | Function | What it does |
|---|---|---|
| 0 | `MapObjectWanderStep` (`0x8006758C`) | the wander (§"Behaviour 0, the wander") |
| 1, 2 | `StepCommandRun` (`0x80067768`) | the step list: it reads the op byte at `+0x5C` (the list) + `+0x5A` (the cursor); an op from 0xF0 to 0xF9 calls `g_StepCommandFunctions[op - 0xF0](row, index)` and returns its answer, any other byte answers 0 |
| 3, 4 | `func_80067584` | returns 0; never called, since the pass skips both |
| 5 | `MapObjectTurnStep` (`0x800678F4`) | stand and turn at random (§"Map-object behaviour 5, stand and turn at random") |
| 6 | `MapObjectPartyLookStep` (`0x80077F1C`) | the look: entry `+0x43 & 0x7F` of `g_MapObjectPartyLookFunctions` (`0x800B5B3C`) |
| 7 | `MapObjectPartySeenStep` (`0x80077FAC`) | the party seen: entry (definition `+0x1E & 0x7F`) - 1 of the six seen steps at `g_MapObjectPartySeenFunctions` (`0x800B5B74`) |
| 8 | `MapObjectPartyAwayStep` (`0x80078060`) | away from the party: the last seen step, `MapObjectPartySeen06Step` (`0x8007A1E0`), which turns the row's back on the lead and walks it away |

Step op 0xF9 is `StepCommandRestart` (`0x80063A64`): it sets the cursor
`+0x5A` back to 2 and runs `StepCommandRun` again on the same row, so the
list starts over. Behaviours 6 to 8 call `MapObjectFacingTurnStep`
(`0x80078D5C`) in place of their look or seen step while a turn is planned
(`+0x4B` not 0), and then `MapObjectPartySeenCheck` (`0x8007A58C`) (the
looks and the seen steps are in map_look.c). When that check sees the
party from a behaviour below 7, the row goes on behaviour 7, or on 8 when
its definition's byte `+0x25` is below the party's least level
`g_PartyLevelLeast` or its seen step is 6. So definition `+0x25` is a
level: once the party's lowest level is above it, the object turns away
from the party instead of running its seen step.

### Behaviour 0, the wander — `MapObjectWanderStep(row, index)` (`0x8006758C`)

Entry 0 of `vtbl_4658_table` (`0x800B4658`); the think pass `MapObjectsThink`
(`0x8006736C`) calls it each time the row's think timer runs out. **Facing
`+0x3E == 0`: the object stands** (returns 1, nothing written). Otherwise it
tries up to `+0x3F` one-pixel steps along the facing
(`MapSpotMove(facing, 1, &sx, &sy)` (`0x80066848`) on a copy of x and draw
y). A step is refused, ending the walk, when it

* leaves the roaming rectangle (`+0x54 <= x < +0x56`, `+0x58 <= y <
  +0x5A`) -> blocked count `+0x42 = 0x10`;
* is not free on the tile map
  (`MapAttributeFreeCheck(x + box.dx, y + box.dy, w, h, layer,
  0) == 0` (`0x800692D4`)) -> `+0x42 = 0x10`;
* lands on someone (`MapObjectOverlapFind(x, y, index) != 0` (`0x80067AC4`): the
  lead party actor returns 1, any other live map object its index + 2) ->
  `+0x42 += 1`.

**The RNG: exactly one draw, and only when the walk was cut short AND the
blocked count has reached 0x10** — the count is cleared and `+0x3E =
rng_draw() % 8 + 1` (`0x8001C770`) (a signed remainder). So a wall or the
rectangle's edge turns the object on the same call; another actor does
only after sixteen blocked thinks; a walk that ran all its steps clears
the count and draws nothing. It returns 0 when not even the first step
was taken (the position is not stored, though a facing may have been
drawn); otherwise x and BOTH y's take the last good position — the ground
y is overwritten with the draw y, so a wanderer has height 0 — and it
returns 1. `MapObjectPartyLook00Step` (`0x80078E74`, map_look.c) does nothing but
call this function; its address is entries 0 and 0xD of
`g_MapObjectPartyLookFunctions` (`0x800B5B3C`), the table
`MapObjectPartyLookStep` (`0x80077F1C`) calls by the row's `+0x43 & 0x7F`.

### Map-object behaviour 5, stand and turn at random — `MapObjectTurnStep` (`0x800678F4`)

Entry 5 of `vtbl_4658_table` (`0x800B4658`; the word at `0x800B466C`), in
map_think.c. The label `vtbl_4658_table` is 0x14 bytes, so entries 5..8 lie
under the next label, `overworld_state_table` (`0x800B466C`); entries 6..8
are `MapObjectPartyLookStep` (`0x80077F1C`), `MapObjectPartySeenStep`
(`0x80077FAC`) and `MapObjectPartyAwayStep` (`0x80078060`). The think pass
`MapObjectsThink` (`0x8006736C`) calls it as `(row, index)` when the row's
think timer `+0x40` runs out; the index is not read.

1. `++row[+0x42]`; below 0x10 it returns 0.
2. From 0x10 on, an object of kind 3 whose animation is not on its last
   frame (`step + 1 < end`, `+0x38` / `+0x3A`) returns 0 with the count left
   standing, so the test repeats every think. That kind test is `row[+0] %
   0x7F == 3`, a REMAINDER, where the test of step 4 masks with 0x7F: the
   original's own slip. The two differ only for a byte with bit 0x80 set
   (0x82 % 0x7F is 3), and the think pass never calls a behaviour for such a
   row (bound to a script slot), so on this path they agree.
3. Otherwise **one RNG draw**: `dir = rng_draw() % 8 + 1` (`0x8001C770`),
   and `row[+0x42] = 0`.
4. Kind not 3: `row[+0x3E] = dir` and `MapAnimationPoseSet(row, 1,
   row[+0x3E])` (`0x8006633C`). Kind 3: `MapAnimationSet(row, dir)`
   (`0x800663EC`), animation id `dir`, the facing byte not written.

It always returns 0, and the object never moves. Each such object so draws
at most once per 16 thinks, on the field (frame-rng.md).

### A map object's walk step — `StepWalkStep` (`0x800677D4`), `StepCommandMoveBy` (`0x8006382C`), `MoveChrGoalCheck` (`0x8005FDAC`)

A map object's STEP LIST (`+0x5C`, cursor `+0x5A`) is run by the dispatcher
`StepCommandRun` (`0x80067768`, map_think.c) through the ten handlers of
`g_StepCommandFunctions` (`0x800B4620`; step_command.c): [0]
`StepCommandPoseSet` (`0x80063714`), [1] `StepCommandFacingSet`
(`0x80063764`), [2] `StepCommandHang` (`0x8006370C`) (an empty endless loop),
[3] `StepCommandMoveTo` (`0x800637BC`) (op 0xF3), [4] `StepCommandMoveBy`
(`0x8006382C`) (op 0xF4), [5] `StepCommandSpeedSet` (`0x80063938`), [6]
`StepCommandWait` (`0x80063990`), [7] `StepCommandPlace` (`0x800639F4`), [8]
`StepCommandScaleSkip` (`0x80063A4C`), [9] `StepCommandRestart`
(`0x80063A64`). Ops 0xF3 and 0xF4 are the object's own WALK TO / WALK BY:
twins of the move script's 0xF3 / 0xF4, not part of that interpreter.

**`StepCommandMoveBy(row, index)`** (`0x8006382C`, step_command.c): op `F4
dx dy -- facing`, 5 bytes, byte 3 never read. On the op's first think
(`+0x56 == 0`): cx = (s16)`+6` / 8 + (s8)dx, cy = (s16)`+8` / 8 + (s8)dy
(signed divides, toward zero); a cell outside 0..0xFF calls
`StepCommandHang(row, index)` (`0x8006370C`), the endless loop, a hang; then
the goal `+0x54` = cx × 8, `+0x58` = cy × 8, `+0x4A` = byte 4 (the facing to
show) and `+0x56` = 1. So the goal is the object's CELL plus the offset, not
its position plus the offset. Every think it returns `StepWalkStep(row,
index)`.

**`StepWalkStep(row, index)` (`0x800677D4`)** (map_think.c) is one think of
the walk, shared by ops 0xF3 and 0xF4:

1. `+0x3E` = `MapFacingTowardGet(x +6, ground y +8, goal x, goal y, +0x3E)`
   (`0x8005FBBC`), the facing toward the goal (a 0 is stored too).
2. Stack copies of x and GROUND y are moved `+0x3F` pixels along that facing
   (`MapSpotMove` (`0x80066848`)). No tile test here.
3. `r = MapObjectOverlapFind(sx, sy, index)` (`0x80067AC4`); when `(r &
   0xFFFF) == 0` the step is taken: `+6` = sx, `+0xA` = sy, `+8` = sy.
4. Arrival, tested whether or not the step was taken, by EXACT equality of
   `+6` / `+8` with the goal: `+0x56` = 0, the cursor `+0x5A` += 5, and
   `+0x3E` = `+0x4A` when that is non-zero.
5. `MapAnimationPoseSet(row, −1, +0x4A if non-zero, else +0x3E)`
   (`0x8006633C`) on every think.
6. It returns `r & 0xFFFF`: NON-ZERO WHEN BLOCKED (1 = the lead, i + 2 =
   object i), 0 when the step was taken. That is the opposite sense to the
   wander's, so the think pass `MapObjectsThink` (`0x8006736C`), which
   re-classifies the footprint (`MapAttributeClassGet` (`0x8006960C`)) and resets the pose
   (`MapAnimationPoseSet(row, −1, +0x3E)`) after a non-zero answer, does so
   only on blocked thinks.

Read from the code, not played: a facing of 0 still steps (`MapSpotMove`
indexes (facing − 1) & 7, so it moves up-left like facing 8); and since
arrival is exact equality, a step size that does not divide the distance can
overshoot. No RNG draw and no frame wait on the path.

**`MoveChrGoalCheck(slot, &x, &y, &z, dir)` (`0x8005FDAC`)** (lns_move.c) is
the MOVE SCRIPT's arrival test (the mover tick `MoveChrWalkStep`
(`0x8005F520`)): `done = (*pz == +0x1C)`, the goal height read unsigned;
`dir == 0` returns that; a `dir` different from the slot's `+0x22` returns 0.
Otherwise by facing: 1, 2, 8: `*py <= goal y` → `*py = goal y`, else done =
0; 4, 5, 6: `*py >= goal y` likewise; then 2, 3, 4: `*px >= goal x` → `*px =
goal x`, else done = 0; 6, 7, 8: `*px <= goal x` likewise. A straight facing
therefore tests ONE axis: heading right, the mover stops at the goal's x
whatever its y. It calls nothing.

### Is someone standing there — a map object's move test — `MapObjectOverlapFind` (`0x80067AC4`)

`s32 MapObjectOverlapFind(s16 spot_x, s16 spot_y, u16 row_number)`
(`0x80067AC4`, map_talk.c; `x`, `y` and `index` below) answers **"is someone
standing there"** for a map object's move. Would map object `index` (a
0x60-byte `MapObject` row of `g_MapObjects`), put at (`x`, `y`), overlap the
lead party actor or another live map object?

1. The probe rectangle `MapBoxTouchGet` (`0x80067BFC`) tests against is
   published in the four globals, from the object's OWN box (`+0x50`, a
   `MapBox`: s8 dx, s8 dy, u8 half width, u8 half height):
   - `g_MapProbeBoxX = x + box.dx` (centre x; the s16 parameter plus the
     signed byte)
   - `g_MapProbeBoxY = y + box.dy` (centre y)
   - `g_MapProbeBoxHalfWidth = box.half_width`, `g_MapProbeBoxHalfHeight =
     box.half_height`
2. The lead: `MapBoxTouchGet(lead.x + g_MapPartyBoxDx, lead.y +
   g_MapPartyBoxDy, &g_MapPartyBoxDx)` with `lead` = record 0 of the party
   actor rows (`g_MapPartyRecords`): `+6` x and `+8` ground y, both read
   SIGNED here. The four bytes `g_MapPartyBoxDx`, `g_MapPartyBoxDy`,
   `g_MapPartyBoxHalfWidth`, `g_MapPartyBoxHalfHeight` (`0x800B8C28`..
   `0x800B8C2B`) are the lead's own box; `MapPartyBoxSet` (`0x8006497C`)
   sets them to (0, 0, 6, 4) for party kind 0 and (0, -1, 8, 5) for any
   other. Answer 9 → **return 1**.
3. The other objects: for i = 0..0x7F in order, row `i` of `g_MapObjects`:
   skipped when free (`+0 == 0`) or when `i == index`; otherwise
   `MapBoxTouchGet(row.x + g_MapPartyBoxDx, row.y + g_MapPartyBoxDy,
   &row.box)`; answer 9 → **return i + 2** (2..0x81, narrowed to 16 bits).
4. Nobody: **return 0**.

`MapBoxTouchGet` answers 9 only for a strict overlap on both axes, and 1..8
for an edge or corner touch (§"The box-against-probe test" below). Only 9
counts in this function, so two boxes that merely touch do not block.

Things the C shows that are easy to assume otherwise:

- **The other objects' centres use the LEAD's offsets.** In step 3 the row's
  x / ground y get `g_MapPartyBoxDx` / `g_MapPartyBoxDy` added (the lead's
  box.dx / box.dy), not the row's own `box.dx` / `box.dy`; only the box
  handed to `MapBoxTouchGet` is the row's. `MapPartyTalkCheck`
  (`0x80067D1C`), the talk check, does add the row's own `box.dx` /
  `box.dy`. With the lead's offsets being (0, 0) or (0, -1) the visible
  effect is that another object's own dx / dy are ignored and its centre is
  one pixel higher in the second lead form. Read from the code, not played.
- **No kind test.** A live row of any kind blocks (`MapPartyTalkCheck` skips
  kinds 4..6; this does not), including a row bound to a script slot (bit
  0x80).
- **An object whose own box.half_width is 0 is never blocked**
  (`MapBoxTouchGet` answers 0 for a zero-width probe), and a row whose
  box.half_width is 0 never blocks.
- The party is represented by its lead only: party records 1.. are not
  tested.
- The four probe globals are left set to this object's rectangle on return.
- No RNG draw, no frame wait; `MapBoxTouchGet` is a leaf.

**Callers (7 call sites; every one uses the answer masked to 16 bits)**

| caller | file | passes |
|---|---|---|
| `MapObjectWanderStep` (`0x8006758C`), behaviour 0, the wander | map_think.c | `(sx, sy, index & 0xFFFF)`, the stepped copy of x / draw y; non-zero → `+0x42 += 1` and the walk ends |
| `StepWalkStep` (`0x800677D4`) | map_think.c | `(sx, sy, index & 0xFFFF)`, the stepped copy of x / ground y; 0 takes the step |
| `MapObjectRandomSpotPlace` (`0x8007816C`) | map_look.c | a random position inside the row's bounds, rerolled `while` the answer is non-zero |
| `MapObjectTowardWalk` (`0x800782B8`), `MapObjectAwayWalk` (`0x800784C4`) | map_look.c | a stepped position and their own index; `== 0` accepts the step |
| `MapObjectFacingWalk` (`0x8007868C`) | map_look.c | a stepped position and its own index; 0 accepts the step, a non-zero answer is handed back through its third argument |
| `MapObjectOpenFacingFind` (`0x800787F0`) | map_look.c | a stepped position and its own index; non-zero marks that facing blocked |

The index argument is every caller's own second argument masked to 16 bits.

**Fields and globals**

| Where | As used here | Meaning |
|---|---|---|
| `g_MapObjects` (s32) → `MapObject[0x80]` | `+0` u8 live, `+6` s16 x, `+8` s16 ground y, `+0x50` box | the map-object rows |
| `g_MapPartyRecords` (s32) → party actor rows | `+6` / `+8`, read signed | record 0 = the lead |
| `g_MapPartyBoxDx` / `g_MapPartyBoxDy` (s8) | the lead's box.dx / box.dy; `&g_MapPartyBoxDx` = the lead's 4-byte box | set by `MapPartyBoxSet` (`0x8006497C`) |
| `g_MapProbeBoxX` / `g_MapProbeBoxY` (s32) | probe centre | read by `MapBoxTouchGet` (`0x80067BFC`) |
| `g_MapProbeBoxHalfWidth` / `g_MapProbeBoxHalfHeight` (s32) | probe half sizes | read by `MapBoxTouchGet` |

### The box-against-probe test — `MapBoxTouchGet(cx, cy, box)` (`0x80067BFC`)

`s32 MapBoxTouchGet(s32 box_x, s32 box_y, MapBox *box)` (map_talk.c;
`cx`, `cy` below). A leaf: four globals and two bytes read, nothing written.
The probe rectangle is centre `g_MapProbeBoxX` / `g_MapProbeBoxY`, half sizes
`g_MapProbeBoxHalfWidth` / `g_MapProbeBoxHalfHeight`, published by the
caller; of `box` only the half sizes `+2` (w) and `+3` (h) are read, since
the callers have already added its dx / dy to cx / cy. With adx = |cx −
`g_MapProbeBoxX`|, ady = |cy − `g_MapProbeBoxY`|, W = box.w +
`g_MapProbeBoxHalfWidth` and H = box.h + `g_MapProbeBoxHalfHeight`:

| answer | when |
|---|---|
| 0 | `g_MapProbeBoxHalfWidth == 0` or `box.w == 0` (only the two WIDTHS are tested for zero), or adx > W, or ady > H |
| 9 | adx < W and ady < H: a real overlap |
| 1 / 5 | adx < W, ady == H: the box is above (1) / below (5) the probe |
| 7 / 3 | adx == W, ady < H: left (7) / right (3) |
| 8, 2, 4, 6 | both equal, the corners: up-left 8, up-right 2, down-right 4, down-left 6 |

The side is numbered like the eight facings, and an exact tie on an axis
goes to the up / left side. Eleven call sites, each masking the answer to
16 bits: `MapPartyObjectsFreeCheck` (`0x800679F4`), `MapObjectOverlapFind`
(two) and `MapPartyTalkCheck` (`0x80067D1C`) in map_talk.c;
`MapObjectPartySeenCheck` (`0x8007A58C`, five) in map_look.c;
`MapObjectTouchFind` (`0x8007AB64`) in map_btl.c; `MapObjectTouchApproach`
(`0x8007B310`) in map_touch.c.

### The field's touch scan — `MapObjectTouchFind` (`0x8007AB64`)

`void MapObjectTouchFind(void)` (`0x8007AB64`), in map_btl.c:
`MapObjectsThink` (`0x8006736C`) calls it once per field frame, after the
think pass above. It finds the first touch-triggered map object the lead is
touching and records its index; `MapObjectTouchRun` (`0x8007ACD8`) consumes
the result.

* **Gate.** Nothing happens while a touch is already pending
  (`g_MapTouchedObject >= 0`) or while the u16 grace counter
  `g_MapPartyBlinkCount` is non-zero. `MapObjectTouchRun` (`0x8007ACD8`)
  sets that counter to 0x12C when its fight returns 2, and the party
  animation tick counts it down with the blink.
* **Probe rectangle**, published for `MapBoxTouchGet` (`0x80067BFC`) in the
  same four globals `MapPartyTalkCheck` (`0x80067D1C`),
  `MapObjectTouchApproach` (`0x8007B310`) and `MapObjectPartySeenCheck`
  (`0x8007A58C`) set: centre `g_MapProbeBoxX` / `g_MapProbeBoxY` = the
  lead's x (+6) / ground y (+8), record 0 of `g_MapPartyRecords`, plus the
  s8 offsets `g_MapPartyBoxDx` / `g_MapPartyBoxDy`; half sizes
  `g_MapProbeBoxHalfWidth` / `g_MapProbeBoxHalfHeight` =
  `g_MapPartyBoxHalfWidth` / `g_MapPartyBoxHalfHeight`.
* **Scan.** The 0x80 map objects (0x60-byte rows of `*g_MapObjects`) in
  order. A row counts when its kind byte `+0x00` is 4, 5 or 6 and its
  behaviour `+0x3D & 0x3F` is not 3.
* **Box test.** The row's 4-byte box at `+0x50` (`MapBox`: `{s8 dx, s8 dy,
  u8 half width, u8 half height}`) is copied to the stack and both half
  sizes are grown by 8 × byte `+0x13` of the row's 0x26-byte definition
  (`g_MapObjectDefinitions[row +1]`): **definition byte +0x13 is the touch
  reach in 8-pixel units.** A non-zero
  `MapBoxTouchGet(x + box.dx, ground y + box.dy, &box) & 0xFFFF`
  (`0x80067BFC`) is a touch.
* **Line of sight.** `MapAttributeLineCheck(&row->x, lead + 6,
  row +0x3C)` (`0x80069C6C`, map_over.c) must then return non-zero: no
  blocking tile of the attribute map lies on the line from the object to
  the lead, with the row's layer as the flag (§"Line of sight on the tile
  map").
* **Result.** The first confirmed row's index goes to `g_MapTouchedObject`
  (-1 = none) and the scan stops. `MapObjectTouchRun` (`0x8007ACD8`) does
  nothing for a definition byte `+0xB` of 0; otherwise
  `MapObjectTouchApproach` brings the object up to the party, and the byte
  runs as a script label for kind 4 (`ScriptLabelRun` (`0x80059FDC`)) and
  as the number of a battle for kinds 5 and 6 (`MapBtlRun` (`0x8007AE58`)).

`MapBoxTouchGet` (§"The box-against-probe test") is used here only as zero
/ non-zero. No frame wait and no RNG draw.

### Group and facing to animation id — `MapAnimationPoseSet` (`0x8006633C`)

`void MapAnimationPoseSet(void *record, s16 pose, u16 facing)`
(map_animation.c), 76 call sites in 51 functions. It touches no global: it
picks an animation number and calls `MapAnimationSet(record, id)`
(`0x800663EC`).

Animation numbers are 0 (none) or `4 * (pose - 1) + 1 + k`: four to a pose
(a group), k = 0..3 a facing column. `pose == 0` plays number 0.
`pose > 0` takes base `((pose - 1) * 4) | 1`. `pose < 0` keeps the record's
current group: base `((record.u16[+0x30] - 1) & 0xFFFD) | 1` (the mask is
0xFFFD in the bytes, not 0xFFFC; with the `| 1` the low two bits come out
01 either way). Then the facing (cases 1..8) picks the column: 5 the base,
1 / 2 / 8 base + 1, 6 / 7 base + 2, 3 / 4 base + 3; any other facing
returns without a call.

`MapAnimationSet` (`0x800663EC`) returns if `+0x30` already holds the
number; otherwise it saves the old number at `+0x32`, stores the new one at
`+0x30` and calls `MapAnimationStart(record, restart)` (`0x80066468`), with
`restart` 0 only when both numbers are in 1..0x3C and share a group of
four, so that a change of facing alone keeps the timer and the step's
length. `MapAnimationStart` puts the step `+0x38` on the animation's first
step and the end `+0x3A` at that step + (count & 0x7FFF); with `restart`
it also zeroes the timer and reads the first step's length.

## Walking and the tile attributes

How a step is tried against the map's attribute plane, and the probes
that read it.

### The step primitive — `MapSpotMoveTry` (`0x8006661C`)

`s32 MapSpotMoveTry(u16 dir, u16 dist, s16 *px, s16 *py, MapBox *box, u8
layer, u8 flag)` — "move this point one step along this facing if the
tiles allow". Callers: the mover tick `MoveChrWalkStep` (`0x8005F520`), the
lead's step `MapPartyWalkStep` (`0x800664D4`) (it steps party record 0 with
the party's box, layer 0 and `g_MapPartyAttributeCheckOff` as `flag`), and
the map objects' `MapObjectAwayWalk` (`0x800784C4`) and
`MapObjectTowardWalk` (`0x800782B8`) (map_look.c) and
`MapObjectTouchApproach` (`0x8007B310`) (map_touch.c).

* `dir` is a facing 1..8, clockwise from up; `MapSpotMove` (`0x80066848`)
  reads, for `(dir − 1) & 7`, dx from `g_MapFacingDxs` (`0x80012C1C`) and dy
  from `g_MapFacingDys` (`0x80012C24`): dx `0 1 1 1 0 −1 −1 −1`, dy
  `−1 −1 0 1 1 1 0 −1` in the image. The mask makes `dir + 1` = 9 and
  `dir − 1` = 0 wrap. **`dir == 0` returns 0 at once, nothing touched.**
* `dist` is pixels per step; `box` the footprint, a `MapBox` (signed `dx`,
  `dy`; `half_width`, `half_height`). Every probe is
  `MapAttributeFreeCheck(x + dx, y + dy, half_width, half_height, layer,
  flag)` (`0x800692D4`, map_over.c), non-zero = free; a non-zero `flag`
  answers free without reading the map.
* **Three candidates are projected from the SAME start — along `dir`,
  `dir + 1`, then `dir − 1` — and the first free one becomes the point.**
  It returns the facing actually moved along: `dir`, `(dir & 7) + 1` or
  `((dir − 2) & 7) + 1`.
* All three blocked: `MapSpotSideMoveTry(dir, dist, px, py, dx, dy,
  half_width, half_height, layer)` (`0x800668A8`) (the obstacle side-step)
  decides, moves the point itself, and its answer is returned; `flag` is
  not passed to it.

The point is written back only when a step is taken. No frame wait or RNG
draw of its own.

### Obstacle side-step — `MapSpotSideMoveTry` (`0x800668A8`)

It is the movement code's detour search. When an actor's straight step
and both steps beside it are blocked, `MapSpotMoveTry` (`0x8006661C`, its
only caller) asks it for a 90° side-step:
`s32 MapSpotSideMoveTry(u16 dir, u16 mode, s16 *px, s16 *py, s32 dx, s32
dy, u16 w, u16 h, u8 layer)`. `dir` is the 8-way compass direction 1..8,
`mode` the step size (below 4 = grid-aligned stepping), (w, h) the box's
half-extent and `layer` the collision layer.

Two probes start at (`*px + dx`, `*py + dy`): A walks along dir + 2 and B
along dir − 2 (the `((d ± k) & 7) + 1` wrap), each through
`MapSpotMove(dir, dist, &x, &y)` (`0x80066848`) (move a point by dist along dir). With
mode < 4, `MapSpotSideAlignCount(dir, x, y, w, h, side)` (`0x80066E64`) gives each probe a
countdown to the 4-unit grid (two eight-way switches, one per probe; the
count is 0..3, taken from the box's trailing edge, not the coordinate —
§"Smaller commands and helpers"): the probe steps by 1 until it reaches
the grid, then by 4. With mode ≥ 4 it steps by mode. On each iteration
probe A moves when `cntB >= cntA`, and B then moves too if `cntB < cntA`
after A's move: both can move in one pass, and A goes first on a tie (the
counters cap at 0x41, so each probe walks at most 0x40 pixels). It dies
when its box leaves 0..0x800 on either axis. From its new spot, when its
own box is free there, it looks ahead along dir, then dir ± 1, with
`MapAttributeFreeCheck(x, y, w, h, layer, 0)` (`0x800692D4`; nonzero =
passable); a hit turns the direction to dir ± 2 and ends the search. A
probe whose own square is blocked backs up one step and drops to step 1
if its step was ≥ 4 (mode < 4), otherwise it dies. When a hit ends the
search, `*px, *py` move one mode step in the new direction and that
direction is returned; otherwise nothing moves and the answer is 0.

The original has a copy-paste bug, kept in the C: probe B's mode ≥ 4
branch clears A's countdown, not its own, so B's countdown is left unset
on that path. And a hit on the step that killed its own probe returns 0.

### A spawned actor's footprint class — `MapAttributeClassGet` (`0x8006960C`)

`s32 MapAttributeClassGet(u16 step, u16 top, s32 x, s32 y, u16 ground,
MapBox *box, s32 layer)` (`0x8006960C`) classifies a record's footprint on
the attribute map (the secondary tile map, 8-pixel cells): `y` is the
record's drawn y, `ground` its ground y and `box` its `{dx, dy, w, h}`. It
returns 0x2000 or 0x4000. Eight functions call it, and each puts the
answer into bits 0x6000 of a record's `+0xE` flags (the party's ring keeps
it in an entry's `+6`): `ScriptCommandMoveChrRowActorSet` (above),
`MapObjectInit` (`0x800656CC`), `MapObjectRestore` (`0x80065128`) and
`MapObjectsThink` (`0x8006736C`) with step 1, top 0x20 and the row's
`+0x3C` as the layer; `MoveChrWalkStep` (`0x8005F520`) and
`MoveCommandPlace` (`0x8006338C`) with the slot's pass count `+0x1E`, 0x20
and the slot's `+0x20`; `MapPartyRingFill` (`0x80067064`) and
`MapPartyRingPush` (`0x80067114`) with 1, 0x18, the lead's box
`g_MapPartyBoxDx` and layer 0. By the register's readings of `sprite_draw`
(`0x8001A528`) and `bg_cover_box_set` (`0x80013EF0`), both classes put the
sprite at ordering-table entry 0x200 + its depth, and with 0x2000 the
background's covering tiles are drawn over it.

1. Off the ground (`y != ground`): 0x4000 at once.
2. A tile with 0x8000 inside the box `(x + dx, y + dy) ± (w, h)` decides:
   0x2000 if `layer == 0` or that tile also has 0x4000, else 0x4000.
3. Otherwise the tile rows from `(y - top)` down to the box top, widened
   on both sides by their count, are scanned for 0x4000. With a layer and
   no such tile: 0x4000.
4. Last, the box is tried with `MapAttributeFreeCheck` (`0x800692D4`) at
   `step`, 2 × `step`, … up to `top + dy - h` pixels above the box's
   centre. Where it is blocked, x moves `step` toward whichever of x + 1
   and x − 1 (in that order) is free; with both blocked the answer is
   0x4000. Anything else: 0x2000.

Both scans inline the attribute lookup of `bg_attribute_get`
(`0x80015928`): slot `q = ((tx >> 4) & 3) | ((ty >> 2) & 0xC)` of the
block cache in the CPU's scratchpad (`g_BgAttributeSlotBlocks`) must hold
block `(tx >> 4) + (ty & 0xF0)`, else `bg_attribute_block_load`
(`0x800154F8`) loads it, and the cell is word
`(tx & 0xF) + ((ty & 0xF) << 4)` of the slot's 256 in
`secondary_tile_table`; a cell outside 0..255 reads 0. After the second
scan the code tests `acc & 0x8000`, which can never be true — only 0x4000
is accumulated — and the original keeps the test.

### The map-cell event probe — `MapAttributeFind(x, y, w, h)` (`0x80069A20`)

`s32 (s32 x, s32 y, s32 w, s32 h)`: the first EVENT under the pixel box x ± w, y ± h on the attribute plane, 0 for none. Low edges below 0 become 0; there is no high clamp (the footprint probe clamps to 0x7FF). Cells `x0 >> 3 .. ((x + w + 7) >> 3) - 1` by `y0 >> 3 .. ((y + h + 7) >> 3) - 1`, row by row, left to right, each read through the block cache exactly as the footprint probe reads it (a cell outside 0..255 reads 0). No RNG, no frame wait.

The event is the cell word's LOW 11 BITS — the same word whose bits 0x3800 / 0x4000 / 0x8000 the movement probes read: `id = word & 0x7FF`, kind = `id >> 7` (0..15), argument = `id & 0x7F`. A cell whose argument is 0 is skipped whatever its kind.

| kind | effect |
|---|---|
| 0..9 | the scan stops, `id` is returned (`MapAttributeRun` (`0x800590C0`) dispatches kinds 1..9; kind 0 and kind 7 fall through its switch) |
| 10 | `g_MapPartyHpTakeOn = 1`, `g_MapPartyHpTakeAmount` = argument; scan goes on. `MapPadStep` (`0x80055EC4`) (map_step.c) reads the latch: while it is set the counter `g_MapPartyHpTakeWait` goes down by one each call, and when it is 0 `MapPartyHpTake` (`0x80059E84`) runs, which takes `g_MapPartyHpTakeAmount` off the first u16 (the HP) of the 0x40-byte record of each party member with an id below 0xC (never below 1) and restarts the counter at 0x1E |
| 11 | `D_800B8E0A = 1` (nothing reads that byte) and the d-pad map `g_MapPadDirections` is patched: argument 3 → `[3]` (right) = 2 (up-right), `[7]` (left) = 6 (down-left); argument 7 → `[3]` = 4 (down-right), `[7]` = 8 (up-left); other arguments patch nothing. Scan goes on |
| 12..15 | nothing |

On entry, before the scan: `g_MapPartyHpTakeOn = D_800B8E0A = 0`, and when `g_MapPadDirectionsState` is 0 (no scrambled-controls box armed by command 0x56) `MapPadDirectionsSet(0)` (`0x80066F04`) restores the d-pad map. So the kind-10 latch and the kind-11 diagonal hold only while the box covers such a cell, and a kind 0..9 cell earlier in the scan hides every later cell. Callers: `MapAttributeRun` (`0x800590C0`), with the lead's x and drawn y plus the party box (`g_MapPartyBoxDx`, `g_MapPartyBoxDy`, `g_MapPartyBoxHalfWidth`, `g_MapPartyBoxHalfHeight`), and `MapObjectPartySeen02Step` (`0x80079B18`, map_look.c), a map object that walks to a spot once it has seen the party: there, when its definition's byte `+0x23` is non-zero, it probes its own x and drawn y (`+0x06` / `+0x0A`) plus its box (`+0x50..+0x53`) and acts on kind 4 only (`ScriptLabelRun(arg)` (`0x80059FDC`)).

### The footprint probe — `MapAttributeFreeCheck(x, y, w, h, layer, flag)` (`0x800692D4`)

`s32 (s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag)`: is the pixel box x ± w, y ± h free on the attribute plane? 1 = free, 0 = blocked. `flag` non-zero answers 1 at once (no cell is read). The box is clamped (low edges to 0, high edges >= 0x800 to 0x7FF) and covers cells `x0 >> 3 .. ((x1 + 7) >> 3) - 1` by `y0 >> 3 .. ((y1 + 7) >> 3) - 1` — high edges exclusive; an empty box is free. Each cell is read through the block cache (`g_BgAttributeSlotBlocks`, s16[16] in the CPU's scratchpad at `0x1F800020`: slot `((tx >> 4) & 3) | ((ty >> 2) & 0xC)` holds block id `(tx >> 4) + (ty & 0xF0)`, else `bg_attribute_block_load(tx, ty)` (`0x800154F8`) loads it) from `secondary_tile_table[(tx & 0xF) + ((ty & 0xF) << 4) + (slot << 8)]` (u16); a cell outside 0..255 reads 0. No write, no RNG, no frame.

Attribute cell word, as this probe reads it:

| bits | meaning |
|---|---|
| 0x4000 | solid. With `layer == 0` EVERY cell counts as solid; with a layer only cells with this bit do |
| 0x3800 | shape; 0 = never blocks. 0x3800 = full block, blocks anywhere in the box |
| 0x0800 | diagonal tolerated in the box's first-row, first-column cell when `(x0 & 7) + (y0 & 7) >= 7` |
| 0x1000 | first row, last column, when `((x1 - 1) & 7) < (y0 & 7)` |
| 0x2000 | last row, first column, when `(x0 & 7) >= ((y1 - 1) & 7)` |
| 0x1800 | last row, last column, when `((x1 - 1) & 7) + ((y1 - 1) & 7) < 7` |

A cell blocks when it is solid and shaped, unless it is the one corner cell its diagonal is tolerated in and the slope test passes. First row is tested before last and first column before last: a one-row box applies only the first-row rules, a one-column box only the first-column ones. Shape values 0x2800 and 0x3000 are not tolerated anywhere (they block like any unlisted shape). Bit 0x8000 is not read here (`MapAttributeClassGet` (`0x8006960C`) reads it).

The bytes show both tables read as array members of structures; the C reads
them through the struct views `BgAttributeSlotBlocks` and `BgAttributeSlots`
(bg.h).

### Line of sight on the tile map — `MapAttributeLineCheck(from, to, flag)` (`0x80069C6C`)

Two points in pixels (`MapSpot`s, x and y read as u16; both callers pass
a record's x and ground y, its `+6`). `n = (max(|dx|, |dy|) + 3) >> 2` —
one sample per 4 px of the longer axis; `n == 0` returns 1. It walks in
16.16 fixed point from `from`, adding `(dx << 16) / n`, `(dy << 16) / n`
per step and sampling the START of each step (so `from` is sampled and
`to` never is): tile `(x >> 19, y >> 19)` through the quadrant cache
(§"A spawned actor's footprint class"), anything outside 0..255 reading
as 0. **A tile with any bit of 0x3800 blocks; with `flag` non-zero only
one that also has bit 0x4000 counts.** Returns 1 = clear, 0 at the first
blocking sample. Callers (map_look.c and map_btl.c): `MapObjectPartySeenCheck` (`0x8007A58C`)
confirms, with flag 1, that a map object sees the lead once its sight area
has met the lead's box; `MapObjectTouchFind` (`0x8007AB64`) confirms a map object's
touch with the row's `+0x3C` byte (its layer) — the test before a field enemy starts
a fight. No RNG, no frame.

## Entering a map

The field entered from the start screen or a load, a map's set-up and
screen effects, the second scroll, and the fades with the party.

### Entering the field from the start screen or a load — `GameEnter` (`0x80059498`)

`void GameEnter(s32 request)`; `id = request & 0xFFFF`. Four callers:
`GameStart` (`0x80055B60`) passes the start screen's result (`StartRun`
(`0x8004345C`)); `MapFrameUpdate` (`0x80055840`) and `MapBtlRun`
(`0x8007AE58`) pass what `MenuNoWalkRun()` (`0x80075BD8`) returns after they
set the menu page `g_MenuPage` to 0x14; `MenuRun` (`0x8006A024`) passes its
request `g_MenuRequest` when that is 1 to 4. It is the mirror of the save
writer `MenuSaveWrite` (`0x80074024`), which writes the fields read here.

1. **id 0 or 4**: transition out (`MapFadeOut(g_MapLayersMask | 0x130, 0x19,
   -2, 0, 1)` (`0x800568D4`), −2 being the highest colour `g_LayerColHighest`),
   `MapSpriteDataFree(0)` (`0x80063C34`), `xmalloc_heap_reset()` (`0x80022E9C`),
   `GameRecordsReset()` (`0x80055E78`), `ScriptChange(0x64)` (`0x80068D7C`),
   `ScriptLabelRun(1)` (`0x80059FDC`), return. No saved state is read.
2. **Otherwise** the same transition with −1 (the lowest colour,
   `g_LayerColLowest`), `MapSpriteDataFree(0)` (`0x80063C34`),
   `xmalloc_heap_reset()` (`0x80022E9C`), then: **id < 0x100 (a save was
   read)** — the 0xC00-byte state block is copied from `0x801CFB00` into
   `*g_GameSaveBlock` (`mem_move_32` (`0x800261FC`)), the saved object list is
   at `0x801D0700`, `SoundMonoSet()` (`0x800550D4`); **id >= 0x100** — the
   map's record `*g_GameMapRecord` (a `GameMapRecord`, 0x20 bytes inside the
   save block) gets `map` (`+0`) = `(u8)request` and `music_number` (`+0x1E`)
   = `script_number` (`+0x1D`) = 0.
3. `MapSet(map, music_number, script_number)` (`0x80068780`) sets the map up
   (§"Map set-up").
4. Record to globals: `g_PlayTimeMinutes`, `g_MapPlaceName` from the save
   block (`+0xA`, `+0xE`); `g_MapPartyKind` (`+1`), `g_MapMix` (`+2`), the
   6-byte `g_MapCol` block (`+4`), `g_PartyCount` (`+0x1A`),
   `g_PartyLevelLeast` (`+0x1B`) from the map's record.
5. `MapBgChangeStoryBitsCopy()` (`0x80059B6C`), `MessageReset()`
   (`0x80059F7C`), `MapPadDirectionsSet(0)` (`0x80066F04`) (the d-pad map as
   pressed: §"The field's d-pad map"), `FaceListLoad()` (`0x8007601C`),
   `MapPartyInit(&record->ring_entry)` (`0x80064BE0`; the ring entry is at
   `+0xA`), then `MapObjectsRestore(record->objects_data_number, list)`
   (`0x80065098`; the set number is at `+0x1C`) spawns the map objects
   (§"Spawn a map's objects from a saved game").
6. Debug print "mapmix=%d" of `g_MapMix`; `layer_mix_set(8,
   (u8)g_MapMix)` (`0x8001ECA0`), and `(8, 0x20)` too when it is 0.
7. `g_MapBgChange[1] = [0] = 0`; `g_MapBgChangeNumber`, `D_800B8E0C`,
   `g_MapPartyBlinkCount` = 0; `g_MapSecondViewKeptY`, `g_MapSecondViewKeptX`,
   `g_MapViewKeptY`, `g_MapViewKeptX` = −1; the scroll bounds
   `g_MapLimitLeft` / `g_MapLimitRight` / `g_MapLimitTop` /
   `g_MapLimitBottom` = the record's `+0x12` / `+0x14` / `+0x16` / `+0x18`;
   `MapViewStep(1, 1)` (`0x80068DB8`), `MapAttributeRun()` (`0x800590C0`),
   and the transition in (`MapFadeIn(g_MapLayersMask | 0x130, 0x19, 1, 1)`
   (`0x800569E0`)).

**Original quirk:** on the id >= 0x100 path the object-list pointer is
never set (the C assigns it only in the save arm), so `MapObjectsRestore`
(`0x80065098`) receives whatever the caller left; it reads the pointer only
when the record's `+0x1C` is non-zero. The C also carries a 16-byte unused
local: the original frame has 16 bytes no instruction touches, their
declaration unknown.

### Map set-up — `MapSet(map, bgm, b8)` (`0x80068780`)

Runs on field entry (`GameEnter` (`0x80059498`): the map, music and script
numbers of the save block's map record `g_GameMapRecord`) and on a scene
change (`MapChange` (`0x8005922C`): the destination map, its
music argument, 0). What the area record's bytes mean is in text.md
§"Locations"; the order, from the C:

1. `g_MapNumber = map`; the 16-byte area record (a `MapTbl`) at `MapTblGet(map)` (`0x80068618`) is
   COPIED to the stack — the two overrides below change the copy only.
2. `g_MapPartyKind` = the record's `+0` (area kind), `g_MapPlaceName` = `+6`
   (location id); ids 0x51..0x64 set event flag id + 0x28A.
3. `bgm != 0xFF` replaces the copy's `+0xF`; `b8 != 0` replaces its `+8`
   (the script `ScriptChange` (`0x80068D7C`) loads from inside `MapLoad` (`0x80068A10`)).
4. `g_MusicNumber` = the copy's `+0xF`; area kind 0 with BGM 6 becomes 7 once
   event flag 0x3C1 is set.
5. `g_MapSoundMapNumber = g_MapNumber` (stored as a halfword), then
   `SoundMapSet((u8)map)` (`0x800555CC`), which loads the area's sound bank.
6. `g_MusicKind = MapMusicSet(g_MusicNumber, g_MusicKind)` (`0x80058EE4`) — the BGM change
   (sound.md §"BGM numbers"; §"Smaller commands and helpers").
7. `MapLoad(&copy, 1)` (`0x80068A10`): `MapResourceLoad` (`0x80020D70`) loads the
   map, `MapEffectSet` (`0x80068AC0`) runs with `sw = 1` (§"A map's screen
   effects"), `ScriptChange` loads the script of `+8`, and the palette and
   tile animation sets of `+5` and `+4` are set.
8. Event flag 0x2D3 is set when the copy's `+0xA` is non-zero, else
   cleared; event flag 0x2D2 likewise from byte 0x16 of the scene table
   `*g_MapEnterTable`.

No frame wait and no RNG draw of its own; the loads happen inside steps 5,
6 and 7.

### A map's screen effects — `MapEffectSet` (`0x80068AC0`)

The name is the developers': the function prints it,
`"MapEffectSet(tbl=%x,sw=%d)"`, as its first statement. It sets a map's
screen effects from the map's 16-byte record `tbl` (`MapTbl`, from
`MapTblGet` (`0x80068618`)), bytes `+0xB..+0xE`. Only `MapLoad(tbl, sw)`
(`0x80068A10`) calls it, passing an unused local as the second argument;
`MapSet` (`0x80068780`) (§"Map set-up") calls that with `sw = 1`,
`MapReload` (`0x800686EC`) and `MapDataReload` (`0x80068748`) with 0.

* **First**: `g_BgMode = 0`.
* **`sw != 0` resets**: the flags `g_MapEffect = tbl[0xB]`; the second
  scroll `g_MapSecondViewX` / `g_MapSecondViewY`, `g_MapSecondViewSpeedX`
  / `g_MapSecondViewSpeedY`, `g_MapCol` / `g_MapColSecond` /
  `g_MapColThird`, `g_MapMix` and `g_MapLayer4Mix` are zeroed; bit 0x8 of
  `g_MapLayersMask` is cleared; `layer_mix_set(8, 0)` (`0x8001ECA0`),
  `(8, 0x20)`, `(4, 0)`, `(4, 0x20)`; `D_800B8EDC = 0`.
* **Always**: `g_MapViewShakeWait`, `g_MapViewShakeCount`,
  `g_MapViewShakeY`, `g_MapViewShakeX` = 0 and `layer_order_set(8, 4)`
  (`0x8001EE68`). With `g_MapEffect` 0 that is all; the rest runs only for
  a map with effect flags (on a reload, the flags of the map last entered).
* **`sw != 0`, by flag** (after printing `"mapeffect=0x%x"`):

  | Flag | Effect |
  |---|---|
  | `& 3 == 3` | `g_MapSecondViewSpeedX` / `g_MapSecondViewSpeedY` = two values from the s8 table `g_MapSecondViewSpeeds` (−8 −4 −2 −1 0 1 2 4 8), indexed by the high and low nibble of `+0xC`; `& 3` of 1 or 2 does nothing here |
  | `& 0xC` | `D_800B8D1E = +0xD & 0x1F`, a row of the 3-byte table `D_80012C98`; `D_800B8EE4` / `D_800B8EE2` = its bytes 1 / 2; `D_800B8EE6 = D_800B8EE0 = 0`. Otherwise `func_800692CC(0)` (an empty function) |
  | `& 0x30` | 0x10: `MapColSet((+0xE >> 4) & 7, +0xE & 0xF)` (`0x80059D10`) unless the first is 0; 0x20 or 0x30: `ColMix(+0xE & 0xF, 0)` (`0x80059E04`) |
  | `& 0x80` | `g_MapLayer4Mix = (+0xD >> 5) * 4`, then `layer_mix_set(4, that)` (`0x8001ECA0`) |

* **`sw == 0`** re-applies the stored values: `layer_mix_set(8,
  g_MapMix)` (`0x8001ECA0`), `(8, 0x20)` when it is 0, `(4, g_MapLayer4Mix)`
  when that is non-zero.
* **Last, either way**: flag 0x40 -> `g_BgMode = 1`, `layer_order_set(8,
  2)` (`0x8001EE68`) and `layer_mix_switch_on(8)` (`0x8001F4B8`), which
  acts only on 0x100 and so does nothing here; otherwise
  `func_8001C4E4(5)` when `g_MapMix` is set, else `(3)`, an empty
  function.

No frame wait and no RNG draw anywhere in its call tree. `g_MapLayer4Mix`
is a 16-bit word, written and tested here as 16 bits; `script_move.c`
declares it `u8` and reads its low byte.

### The second scroll's goal — `MapSecondViewGoalGet(out)` (`0x80069130`)

Writes where the second background layer (`g_MapSecondViewX` / `g_MapSecondViewY`)
should be to `out` (a `MapSpot`, two s16), by the scroll mode in the low two bits of
the map-effect flags `g_MapEffect` (`MapEffectSet` (`0x80068AC0`), the developers'
name, takes them from the area record's `+0xB`). **No mode clamps anything.**

| mode | goal |
|---|---|
| 0 | nothing is written |
| 1 | the camera (`g_MapViewX`, `g_MapViewY`); `D_800B8EDC = 1` |
| 2 | flag 0x40 of `g_MapEffect` set: camera / 2 (signed, toward zero); clear: camera × 2; `D_800B8EDC = 1` |
| 3 | per axis: second scroll + (camera − the last recorded camera `g_MapViewKeptX` / `g_MapViewKeptY`) + drift |

`D_800B8EDC` is a word nothing reads: this function and `MapViewStep` set
it, `MapEffectSet` zeroes it.

**Mode 3's drift** — a layer that also moves by itself. The call counter
`g_MapSecondViewDriftCount` is incremented first. On an ODD count, per axis with the
speed v = `g_MapSecondViewSpeedX` (x) / `g_MapSecondViewSpeedY` (y) (MapEffectSet's values from
the s8 table `g_MapSecondViewSpeeds`: −8 −4 −2 −1 0 1 2 4 8, picked by the high (x)
and low (y) four bits of the area record's `+0xC`): v outside −1..1 adds
v / 2; v inside adds v, and only once the counter has reached 3. Then a
counter ≥ 3 is reset to 0. The counter so runs 1, 2, 3: a fast axis moves
v / 2 on counts 1 and 3, a slow one v on count 3 — **v pixels per three
calls** either way.

Three callers:

* `MapViewStep(snap, arg1)` (`0x80068DB8`) — after `MapViewScrollSet(arg1)` (`0x80068E98`), **in mode 3
  on every call**: a goal that differs from the second scroll is stored,
  `MapSecondViewScrollSet()` (`0x80069058`) runs and `D_800B8EDC = 1`. Its tail records the camera
  in `g_MapViewKeptX` / `g_MapViewKeptY` and the second scroll in `g_MapSecondViewKeptX` /
  `g_MapSecondViewKeptY`.
* `MapViewScrollSet(arg0)` (`0x80068E98`) — with `arg0 != 0`, the camera off its last
  recorded position and the mode NOT 3: the second scroll is set to the
  goal at once. In mode 0 nothing was written, so this stores the caller's
  own uninitialised stack pair; whether a mode-0 map draws that layer at
  all was not read.
* `MoveChrFollowStart` (`0x8005FED0`), mask bit 17 (the table in §"Send movers home
  and release them"): the goal becomes slot 17's destination.

No frame wait, no RNG draw.

### A field fade with the party glowing — `MapFadePartyIn` (`0x80056CB8`)

`void MapFadePartyIn(s32 layer_mask, s32 length, s32 view_follow, s32
second_view_follow)`: what the fade `MapFadeIn` (`0x800569E0`) (its only
caller) runs in place of its own fade when `g_MapFadeState == 2`.
`layer_mask` is `layer_col_fade_start`'s mask, `length` the duration in
frames, `view_follow` / `second_view_follow` the field frame `MapFrameStep`
(`0x800557D4`)'s two arguments.

The vocabulary of this and the next section: the "glow" is the sprites'
mix. `layer_mix_switch_on(0x100)` (`0x8001F4B8`) sets `g_SpriteMixOn`, and
while it is 1 `sprite_piece_draw` (`0x80019DBC`) gives each sprite the
level of its mix group (bits 0xC00 of its entry's `+0xA`) from
`g_SpriteMixLevels`; `layer_mix_set` bits 0x100, 0x200, 0x400 and 0x800 set
groups 0 to 3. `layer_mix_switch_off(0x100)` (`0x8001F4E0`) clears the
switch. A record's `+0xE` is its sprite entry's `+0xA`; its bits 0x6000 pick
the sprite's ordering-table entry (the register's reading of the sprite
list), and the footprint class `MapAttributeClassGet` (`0x8006960C`) gives
(0x2000 or 0x4000) is what normally sits there.

1. Vibration script `g_BtlActuatorToggleList` (`g_PadActuatorList` = the
   list, `g_PadActuatorRestFlags` = 0x3F, `g_PadActuatorListWait` = 0), the
   mix on (`layer_mix_switch_on(0x100)`), `layer_mix_set(0xD00, 0x1F)`
   (`0x8001ECA0`): level 0x1F in groups 0, 2 and 3.
2. Each present party member (`g_PartyIds[i] != 0`): the `+4` halfword (the
   record's on word) of both actor rows (`g_MapPartyTwins`, then
   `g_MapPartyRecords`) is cleared, both get `+0xE |= 0x6000`, and both are
   sent `MapAnimationPoseSet(row, 0xD, spot.facing)` (`0x8006633C`) — **with
   member ZERO's spot (`MapPartyRingEntryGet(0, &spot)` (`0x80066FB4`)) for
   every member**.
3. `layer_col_set(-1, 0x100, -1)` (`0x8001EB74`), one field frame,
   `MapPartyOnPlay()` (`0x80068400`) (every member's two rows switched on,
   `SoundPlay(0xE, 1)` (`0x800555F0`), 0x3C field frames, then pose 1 with
   ring entry 0's facing).
4. `layer_mask` loses bit 0x100 and gains 0x80000; three calls
   `layer_mix_fade_start(layer, 1, length, level)` (`0x8001EBA4`) for layers
   0x100 / 0x400 / 0x800 with levels 0 / 0xC / 0x12, which do nothing:
   `layer_mix_fade_start` tests only bits 4 and 8 of its mask. Then
   `layer_col_fade_start(0, layer_mask, 1, length, g_MapCol)` (`0x8001EA74`).
5. One field frame per pass while `layer_col_fade_check(0) & 0xFF`
   (`0x8001EDDC`).
6. The mix off (`layer_mix_switch_off(0x100)`), `layer_col_set(0, 0x100,
   g_MapCol)` (`0x8001EB74`), and each present member's two rows take their
   OWN spot's class back (`MapPartyRingEntryGet(i, &spot)`; `+0xE =
   spot.class | (+0xE & 0x9FFF)`, the class being the entry's `+6`).
7. `layer_mix_set(0x80F35, 0xFF)` (`0x8001ECA0`) and vibration script
   `g_MapActuatorOneFrameList`.

No RNG draw of its own; neither vibration script uses the random motion
(`g_BtlActuatorToggleList` toggles both actuators for 0xB4 frames,
`g_MapActuatorOneFrameList` is a one-frame level on actuator 0).

### The fade's other opening — `MapFadePartyOut` (`0x80056AEC`)

`void MapFadePartyOut(s32 layer_mask, u32 length, s32 view_follow, s32
second_view_follow)`: what `MapFadeOut` (`0x800568D4`) (its only caller) runs
when `g_MapFadeState == 2`, before its own `layer_col_fade_start(0,
layer_mask | 0x80000, 3, length, target)` (`0x8001EA74`), `target` being
`MapFadeOut`'s third argument. The arguments are `MapFadePartyIn`
(`0x80056CB8`)'s, and so is the vocabulary (above).

1. Vibration script `g_BtlActuatorToggleList` (the same three stores as
   `MapFadePartyIn` (`0x80056CB8`) step 1).
2. Every present party member (`((u16 *)g_PartyIds)[i] != 0`, i = 0..5):
   `+0xE |= 0x6000` on both its actor rows (`g_MapPartyRecords`'s, then
   `g_MapPartyTwins`'s). No `+4` clear, no spot lookup and no
   `MapAnimationPoseSet`, all of which `MapFadePartyIn` (`0x80056CB8`) does.
3. `layer_mask` loses bit 0x100 and gains 0x80000; `layer_col_set(-1,
   0x100, 0)` (`0x8001EB74`), `layer_col_fade_start(0, layer_mask, 1,
   length, -1)` (`0x8001EA74`) (−1, the lowest colour, where
   `MapFadePartyIn` (`0x80056CB8`) passes the colour block), the mix on
   (`layer_mix_switch_on(0x100)` (`0x8001F4B8`)).
4. `length` field frames, counted unsigned (none when `length` is 0).
5. `layer_mix_fade_start(0xD00, 1, length, 0x1F)` (`0x8001EBA4`) and a
   field frame per pass while `layer_mix_fade_check(0xD00) & 0xFF`
   (`0x8001ED94`). Both test only bits 4 and 8 of the mask (bit 8 alone for
   the check), so the first does nothing and the loop runs no frame.
6. `MapPartyPose4Play()` (`0x80068334`) (each present member's two rows get
   pose 4 with their ring entry's facing, sound `SoundPlay(0xE, 1)`
   (`0x800555F0`), 60 field frames), `g_MapFadeState = 2` again, the mix
   levels set with `layer_mix_set` (`0x8001ECA0`) (0x100 -> 0, 0x400 -> 0xC,
   0x800 -> 0x12), the mix off (`layer_mix_switch_off(0x100)`
   (`0x8001F4E0`)).
7. Vibration script `g_MapActuatorOneFrameList`.

No RNG draw of its own; it returns nothing.

## Smaller commands and helpers

**Command 0x29, fade the screen out — `ScriptCommandMapFadeOut` (`0x8005DE88`)** (lns_intp.c;
four bytes: id word, length, kind). The cursor moves on by 4;
`len` = byte +2 (frames); the colour argument is -1; only while the fade
state byte `g_MapFadeState` is 0 the kind byte +3 is read: 1 makes the colour
-2, 2 stores `g_MapFadeState = 2`, at which the next `MapFadeOut` (`0x800568D4`) and the
`MapFadeIn` (`0x800569E0`) after it go through `MapFadePartyOut` (`0x80056AEC`) and
`MapFadePartyIn` (`0x80056CB8`): the map goes dark while the party is still shown, and the
party plays its leaving and arriving. Then
`MapFadeOut(g_MapLayersMask | 0x130, len, colour, g_MoveChrViewFollow, g_MoveChrSecondViewFollow)` (`0x800568D4`).
It is the test `MapChange` (`0x8005922C`) makes on a scene entry's byte +2, with the
length fixed there at 0x19. -1 / -2 are `layer_col_fade_start` (`0x8001EA74`)'s sentinels for
the triples `g_LayerColLowest` (-0x100 three times) and `g_LayerColHighest` (0x100
three times); the ramp's converter `layer_col_conv` (`0x8001E050`) turns -0x100 into level 0
and 0x100 into level 0xFF (0 gives 0x80, the map's own level), so -1 fades the layers to
their darkest and -2 to their brightest. No RNG draw of its own; it waits frames through
`MapFadeOut` (`0x800568D4`).

**Command 0x56, the turned D-pad box — `ScriptCommandMapPadDirectionsChange` (`0x8005EB98`)** (lns_intp.c;
four bytes: id word, radius byte, an unread byte). `r = byte *
8`; `MapPadDirectionsSet((rng_draw() & 3) + 1)` (`0x80066F04`, `0x8001C770`), ONE RNG draw picking row
1..4 of the field's D-pad map (a quarter turn either way or one of the two
mirrors); `g_MapPadDirectionsState = 1`; then the box around the lead (record 0 of
`g_MapPartyRecords`, u16 words +6 and +8): `g_MapPadDirectionsBoxLeft = x - r`, `g_MapPadDirectionsBoxRight =
x + r`, `g_MapPadDirectionsBoxTop = y - r`, `g_MapPadDirectionsBoxBottom = y + r`. `MapPadStep` (`0x80055EC4`)
(map_step.c) reads the box every field frame while `g_MapPadDirectionsState` is non-zero,
with the lead's x and the same +8. Outside it (x below left or from right on, y below top
or from bottom on), once no colour fade runs, it puts row 0 back, sets the map's colour
`g_MapCol` again and clears the state. Inside it, whenever no colour fade runs, it starts
a 0x18-frame fade of the layers to `g_MapPadDirectionsCol` (0x30, 0x18, 0x30 in the image)
from state 1, or back to `g_MapCol` from state 2, and turns 1 into 2 and 2 into 1, so the
screen pulses while the pad is turned. So the turned pad lasts until the lead leaves the
square of half-side r around where it stood. No frame wait. The +8 the box uses is the
lead's ground y (`sprite.depth`); the tile lookup passes +0xA, the drawn y, which is the
ground y less the height (`MapPartyRingFollow`), so the two agree while the lead is on the
ground.

**Command 0x57, raise a member to the first slot's level —
`ScriptCommandChrLevelGapAdd` (`0x8005EC30`)** (lns_intp.c; four bytes: id word, who,
mode). `n` = the level of the member whose id is the FIRST u16 at
`*g_PartyIds` (no test for an empty slot or an id of 0xC or more), `own` =
who's level (`ChrRecord.level`, `+0x1C` of `g_ChrRecords + id * 0x40`). `n < own`:
nothing happens. Otherwise the gap `n - own`, halved (signed `/ 2`) when
mode == 1, goes to `ChrLevelAdd(who, gap)` (`0x8002844C`), and who's HP and MP are set
to their maxima, also for a gap of 0. No RNG draw or frame wait: `ChrLevelAdd` and
what it calls (`ChrLevelUp` (`0x80028554`), `ChrLevelGainGet` (`0x80028520`),
`ChrLevelGainAdd` (`0x800289E0`)) draw no number and let no frame go by.

**Command 0x59, put a spell in a member's list — `ScriptCommandChrSpellSet` (`0x8005EE30`)**
(lns_intp.c; four bytes: id word, who, row). The spell
table's 12-byte row `row` (`g_ChrSpellRows + row * 12 - 12`; `g_ChrSpellRows` holds the
address of `g_BtlSpellRows`, `0x800AA734`) must have learner byte +7 equal to who (0xB is
compared as 1), else error 0xC through `g_ScriptErrorNumber` and `ScriptErrorHang`
(`0x8005B568`), which never returns. Then the row number is stored at
`ChrRecord.spells[row byte +3 >> 4]` (byte `+0x2A + slot`) of the record at
`g_ChrRecords + who * 0x40`; who is used unchanged there, so id 0xB gets the store in its
own record. No range check on who, row or the nibble; rows 1..50 of the image carry
nibbles 0..7 only. No RNG draw, no frame wait. Which event scripts use it was not
looked up.

**The script error reporter and the cursor step — `ScriptErrorHang` (`0x8005B568`) /
`ScriptPointerAdvance` (`0x8005B5B8`)** (lns_intp.c). `ScriptErrorHang` (`0x8005B568`) is line
1642 (0x66A) of `LNS_INTP.C`: the debug print `ScriptStateReport` (`0x80013E98`) (script number
`g_ScriptNumber`, label `g_ScriptLabel`, `g_ScriptCommandWord`, `g_MapTalkObjectDefinition`, the script
base `g_ScriptBase`, the cursor minus 2), the assert `assert_fail` (`0x8001B050`) with
the strings at `D_8001296C` / `D_80012978` ("TextError!", "LNS_INTP.C")
and 0x66A, then an endless empty loop. Both callees are empty in this
build, so in the retail game it simply stops there. It never reads the
error code the callers store in `g_ScriptErrorNumber`. Reached by fifteen calls from
twelve functions and as commands 0, 1, 6 and 0x1B of `g_ScriptCommandFunctions`.
`ScriptPointerAdvance(n)` (`0x8005B5B8`)
is `g_ScriptCommandPointer = (g_ScriptCommandPointer & -n) + n; return g_ScriptCommandPointer;` with a
one-byte n: the cursor rounded DOWN to a multiple of n, plus n (a cursor
already on a multiple still moves on by n). All 38 callers pass 2 (34
sites) or 4.

**A face into face slots — `MessageFaceSet` (`0x8005B36C`)** (message_disp.c).
`(face, places, old_face, unused, show_lines)`; the C declares the first four 16 bits wide.
face != 0: if a place outside `places` is up (`g_MessageFacePlaces & ~places`),
`CellFacePlacesSet(places, 0xDEAD0001, 0)` (`0x80057608`) and `g_MessageFacePlaces = 0` (the call is
given the NEW places, so the place that was up is dropped from the bits
without being given face 0 here); then `CellFacePlacesSet(places, 0xDEAD0001,
face)` (`0x80057608`) and `g_MessageFacePlaces = places`. face == 0: `CellFacePlacesSet(g_MessageFacePlaces,
0xDEAD0001, 0)` (`0x80057608`), `g_MessageFacePlaces = 0`, and the result is `old_face`.
show_lines != 0: `MessageRowDisp(g_MessageRow)` (`0x8005B4A0`) (one field frame). Returns the
face or `old_face` as 16 bits. `CellFacePlacesSet` (`0x80057608`) does not read its second
argument; the fourth parameter is not read (the text control 0xFAnn passes 0, 0xFBnn
0x8000). Callers: `MessageFaceDisp` (`0x8005B2F0`) (text controls 0xFAnn / 0xFBnn),
`MessageFaceEntrySet` (`0x8005B448`) (the menus' face), `ShopFaceSet` (`0x80054C0C`)
(shop_face.c). `CellFacePlacesSet` (cell_face.c) calls `FaceCellSet` (`0x8001B3DC`, face.c)
for each place: face 0 clears the cell
(`ankprint_cell_set(x, y, 0)` (`0x80025864`)); otherwise `FaceSlot6Find(face)` (`0x8001B5C4`) gives the
face's place among the ten face numbers listed in the face data held in portrait slot 6,
or -1, in which case the face data number is `(face - 1) / 5 + 1`, the image
`(face - 1) % 5`, and `FaceSlotGet(number)` (`0x8001B200`) returns the cache slot (a disc
read on a miss). Then `FaceImageLoad(slot, image, g_FaceCellPage)` (`0x8001B310`) uploads the
64 × 64 image, the cell word is `g_FaceCellPage | 0x400`, and the page `g_FaceCellPage`
flips between 0 and 1, so two portraits can be on screen at once and a third overwrites
the older one. `FaceLoad(face)` (`0x8001B394`) (face.c) is the preload alone:
`FaceSlotGet((face - 1) / 5 + 1)` for a non-zero face.

**The default colour triple from a preset row — `MapColSet` (`0x80059D10`)**
(map_change.c; `g_MapCol` is declared there once, as a `MapCol`).
`(row, scale)`: `g_MapCol = g_MapColRows[row]` (a 6-byte `MapCol` row of three
s16), each channel multiplied by `scale` and stored back as 16 bits, then
clamped to -0xFF..0xFF. `g_MapColRows` is ten rows right in front of the
command table `g_ScriptCommandFunctions`; every row is grey: 0, -14, 16, -6, -14, 48,
12, 16, 16, 16. With a 4-bit scale only row 5 can reach the clamp. Called
by `MapColFadeStart` (`0x80059CC8`) (nothing for row 0; otherwise an 0x18-frame ramp
`layer_col_fade_start(0, g_MapLayersMask | 0x100, 1, 0x18, triple)` (`0x8001EA74`)
after it; the map-cell dispatch `MapAttributeRun` (`0x800590C0`) passes `(arg >> 4, arg
& 0xF)` for kind 8, i.e. preset row and strength), by the script command
`ScriptCommandMapColSet` (`0x8005DDC4`, command 0x46, six bytes; operand bytes +2 / +3 =
row and scale; with its switch byte +4 set it calls `MapColSet` and then
`layer_col_set(0, g_MapLayersMask | 0x100, triple)` (`0x8001EB74`) at once when
`g_MapFadeState` and `g_MapDrawAreaClosed` are both 0; with +4 at 0 it calls
`MapColFadeStart` instead) and by `MapEffectSet` (`0x80068AC0`, map_load.c, the developers'
name from its own debug print: when bits 0x30 of the map's effect word `g_MapEffect` are
0x10, with `((col >> 4) & 7, col & 0xF)` of the map record's byte +0xE, `MapTbl.col`,
skipped when the first is 0). A leaf: no RNG draw, no frame wait.

**The map's BGM change — `MapMusicSet` (`0x80058EE4`)** (map_music.c).
`kind = MapMusicSet(bgm, kind)` (`0x80058EE4`), called by `MapReload` (`0x800686EC`) and the map
entry `MapSet` (`0x80068780`) as `g_MusicKind = MapMusicSet(g_MusicNumber,
g_MusicKind)` (`0x80058EE4`). A kind is `MusicKindGet(number)` (`0x80055448`): 1 for 0x3C (the CD-audio
track), 2 from 0x44 up, else 0. It first polls `ScriptMusicStep()` (`0x80058FBC`), with no
frame between, while the script's music request `g_ScriptMusicLevelRequest` is pending.
bgm != 0: if the old kind was 1 the music is stopped (`MusicPlay(0, 1, 0)` (`0x80054F2C`));
the new kind is computed; for kind 1 it ONLY stores the number in `D_800B8DF8` and
makes no request (it does not start the CD track; `MusicPlay` starts it for 0x3C, and
which caller does so for such a map was not looked for), otherwise `MusicPlay(bgm, 0, 0)`
(`0x80054F2C`) and `D_800B8DF8 = 0`. bgm == 0: the stop, and kind 0. `D_800B8DF8` is
written and never read (no gp-relative access in the image but this function's two
stores). `MusicKindGet` (`0x80055448`) answers 2 from 0x44 and `MusicPlay` (`0x80054F2C`)
refuses from 0x45, so 0x44 is requested and honoured. No RNG draw and no frame:
`MusicPlay`'s only frame-waiting arm, a fade loop, is one the built program never takes
(it runs `MusicLoadStart` (`0x80055118`) for every sequence number; sound.md).

**Where a joining member goes — `PartyJoinPlaceGet` (`0x80064C70`)** (party.c).
`idx = PartyJoinPlaceGet(id)` (`0x80064C70`), called once, by command 0x23's
insert `MoveChrPartyJoin` (`0x80060684`). id 0xC: 1. id 0xD and up: the count
`g_PartyCount`. Otherwise `rank = g_PartyRanks[id % 15 - 1]` and the list
`((u16 *)g_PartyIds)[0..count-1]` is walked: a slot holding 0xC is stepped
over (in slot 0 the answer is 0), a slot holding 0xD or more ends the
walk, and so does a member of higher rank. `g_PartyRanks` (.rodata, ids
1..11): `01 05 08 06 07 03 04 0A 09 0B 02`. So the list is kept sorted by
rank, 0xC is kept in slot 1 (with the caller's swap) and ids from 0xD up
stay behind everyone. A leaf that only reads. An id of 0 is not guarded
(it indexes byte 0xFF of the table).

**The party's lowest level — `PartyLevelLeastSet` (`0x80065028`)** (party.c).
`g_PartyLevelLeast` = the smallest `ChrRecord.level` over the `g_PartyCount` slots
whose id is not 0xC, starting from 99. Called after the party slots
change (`MoveChrPartyJoin` (`0x80060684`), `MoveChrPartyLeave` (`0x80060F18`)) and after a
battle (`MapBtlRun` (`0x8007AE58`)). The reader is the map-object contact check
`MapObjectPartySeenCheck` (`0x8007A58C`) (map_look.c): an object that sees the party whose
definition byte +0x25 (`MapObjectDefinition.level`) is below `g_PartyLevelLeast` (or whose
`seen & 0x7F` is 6) goes on behaviour 8, `MapObjectPartyAwayStep` (`0x80078060`), which
walks it away from the party; otherwise on behaviour 7, `MapObjectPartySeenStep`
(`0x80077FAC`). A leaf.

**The animation setter — `MapAnimationSet` (`0x800663EC`)** (map_animation.c).
`(rec, id)`: nothing when `+0x30` already holds the id; otherwise `+0x32`
= the old id, `+0x30` = id, and `MapAnimationStart(rec, restart)` (`0x80066468`) with restart
0 only when both ids are in 1..60 and `((old - 1) & 0xFFFC) + 1 == ((new -
1) & 0xFFFC) + 1` (the same group of four: only the facing column
changed), else 1. `MapAnimationStart` (`0x80066468`) re-seats the step and its end either
way and, only on a restart, clears the frame timer and reloads the step's length. 38 call
sites in 21 functions. map_animation.c defines the second parameter `u16`, as the bytes
give it; menu_list.c declares it `s32`.

**The side-step probe's countdown — `MapSpotSideAlignCount` (`0x80066E64`)** (map_walk.c).
`k = MapSpotSideAlignCount(dir, x, y, w, h, flag)` (`0x80066E64`), 0..3: the number of single
pixels a probe of `MapSpotSideMoveTry` (`0x800668A8`) walks before the trailing edge of
its box, on the axis it moves sideways along, is a multiple of 4. x and y are the box's
middle, w and h its half width and height. Two switches on dir (one per probe: flag != 0
is probe A, a quarter turn clockwise). Moving +x: `(w - x) & 3`; +y: `(h - y) & 3`; -x:
`(x + w) & 3`; -y: `(y + h) & 3`. Axis by dir 1..8: probe A `+x +y +y -x -x -y -y
+x`, probe B `-x -x -y -y +x +x +y +y`. A dir outside 1..8 takes no arm (the switches have
no default) and the result is left unset, & 3. Pure arithmetic.

**A place onto the party's position ring — `MapPartyRingPush` (`0x80067114`)**
(map_ring.c). `(x, y, z, dir)` builds the 8-byte `MapPartyRingEntry` `{u16
x; u16 y; u8 height; u8 facing; u16 class}` with class = `MapAttributeClassGet(1, 0x18,
x, y - z, y, &g_MapPartyBoxDx, 0)` (`0x8006960C`) (0x2000 or 0x4000; any z != 0 gives
0x4000), moves the head `g_MapPartyRingHead` on one (0x5F wraps to 0) AFTER that
call, and copies the entry to `g_MapPartyRingEntries[head]` (0x60 entries of 8
bytes). Callers: the lead's walk step `MapPartyWalkStep` (`0x800664D4`) (z = 0) and the
script mover tick `MoveChrWalkStep` (`0x8005F520`) (slot 1). It returns nothing.

**Is the lead's next place clear of map objects — `MapPartyObjectsFreeCheck` (`0x800679F4`)**
(map_talk.c). `(x, y)`, called by `MapPartyWalkStep` (`0x800664D4`). It
publishes `MapBoxTouchGet` (`0x80067BFC`)'s probe rectangle as the lead's box at that
place (`g_MapProbeBoxX = x + g_MapPartyBoxDx`, `g_MapProbeBoxY = y + g_MapPartyBoxDy`,
`g_MapProbeBoxHalfWidth = g_MapPartyBoxHalfWidth`, `g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfHeight`), then walks the
0x80 rows (0x60 bytes) of `g_MapObjects` and returns 0 at the first live
row whose own box (x +6 plus dx, ground y +8 plus dy, the box at +0x50)
STRICTLY overlaps it (`MapBoxTouchGet(...) & 0xFFFF == 9` (`0x80067BFC`)), else 1. An
edge or corner touch does not block. It is `MapObjectOverlapFind` (`0x80067AC4`) the other way
round.

## Chests, examine cells and the item notice

What the field does when the lead examines a map object of kind 7 (a chest,
or a thing to examine), how silver and items are given (event-script
commands 0x1C and 0x1F, `ChrItemGive`), the one-line notices that show
them, and the attribute cells of kind 6 that the lead examines, which is
how a shop opens. The counts of retail definitions were taken over the
disc's map-object sets with station-tools, a custom decomp toolkit that
will be released later.

### A kind-7 row — `MapObjectKind7Run` (`0x80067F84`)

`MapObjectKind7Run(row, event byte)` (map_talk.c) is called by the talk
scan `MapPartyTalkCheck` (`0x80067D1C`) for a row of kind 7, with the
row's index and its definition's byte +0xB, whatever that byte is (a row
of any other kind runs only a non-zero byte). The row's facing byte +0x3E
is its state, and with it its picture: `MapAnimationPoseSet`
(`0x8006633C`) picks the animation of the four from it (facing 5 the
first, 1, 2 and 8 the second, 6 and 7 the third, 3 and 4 the fourth).
`MapObjectInit` (`0x800656CC`) gives a kind-7 row sprite data 0, whose
animations 1..4 are the chest:

| state | what | animation |
|---|---|---|
| 5 | shut | 1 |
| 7 | shut, and locked while event flag 0x267 is clear | 3 |
| 1 | state 5 opened (5 − 4) | 2 |
| 3 | state 7 opened | 4 |
| 6, 8 | shut; no retail definition uses them | 3, 2 |

Below 5 the function returns at once: nothing happens at an open row. The
event byte is what is inside:

| byte | content | retail definitions (224) |
|---|---|---|
| 0 | nothing: the row opens, string 0x64C ("It's empty.") | 5 |
| 1..0xB6 | item of that id | 148 |
| 0xB7..0xEF | silver `g_MapObjectKind7Amounts[byte − 0xB7]` (u16, `0x800B4F60`) | 25, bytes 0xB7..0xCA = entries 0..19; entries 20..56 are 0 |
| 0xF0..0xFF | not a chest: the row does not open; label `byte + 0x230` of the map's event script runs | 46, bytes 0xF0..0xFA |

208 definitions are of state 5 and 16 of state 7 (15 items, one silver).
All 224 are on behaviour 3 and have no flag byte.

In order: (1) state < 5 → return. (2) state 7 and flag 0x267 clear → the
message `0xF64B` ("What? It's locked, Alex!"), return; neither the row nor
the lead is touched. (3) The lead's record takes pose 0xA
(`MapAnimationPoseSet(lead, 0xA, facing)`: animations 37..40, group 9 in
the counting of §"What the pose groups are on the party sheets"), facing
as it faces (byte +5 of the entry `MapPartyRingEntryGet(0)` (`0x80066FB4`)
copies out). (4) byte < 0xF0: state −= 4, `SoundPlay(0xB, 2)`
(`0x800555F0`), the row's animation for the new state. (5) byte 0: the
message `0xF64C`. Byte ≥ 0xF0: event flag 0x2D6 set, `ScriptLabelRun(byte
+ 0x230)` (`0x80059FDC`), flag 0x2D6 cleared. Byte ≥ 0xB7:
`g_ChrItemNoRoomState = 0`, command 0x1C (`ScriptCommandMoneyAdd`
(`0x8005CF6C`)) with the table's amount. Else: flag 0x2D6 set,
`g_ChrItemNoRoomState = 0`, `g_ChrItemNoRoomItem = item`, command 0x1F
(`ScriptCommandChrItemGive` (`0x8005D0E0`)), flag 0x2D6 cleared. For both
commands the function builds the command's bytes on its stack
(`MapObjectKind7Command`, map.h, with k = 0) and points
`g_ScriptCommandPointer` at them. (6) After silver or an item:
`g_ChrItemNoRoomState == 2` — the item was not taken — gives the message
`0xF66B` ("Let's put it back for now."), state += 4, sound `(0xB, 2)`
again, the shut animation; otherwise the flag below is set. (7) The lead
takes pose 1 (group 0, the walk).

**The flag that keeps a chest open** is `n + 0x2620 + ((map > 100 ? map −
0x35 : map − 0x15) << 5)`, n being the row's byte +1 (its definition's
number) and map `g_MapNumber`. `MapObjectInit` reads the same flag at
spawn: a kind-7 row of state 5 or more whose flag is set starts with
state − 4, open. Command 0x1A (`ScriptCommandStoryBitPairs2620Set`
(`0x8005CECC`)) sets these flags from a script. `MapObjectKind7Run` sets
the flag only when silver or an item was taken. An empty chest sets none
(it is shut again at the next visit); neither does a put-back, a locked
chest or a thing to examine. (The two arms of the formula overlap: maps
101 and 69 give the same base, as do 102 / 70 and 103 / 71; of the maps
that have a record in the image's map table (`g_MapTblNumbers`
(`0x800B5198`)) these three are the only such pairs. Whether any pair has
chests on the same indices was not looked at.)

**What sets event flag 0x267**, the 16 locked chests' key: no C function
sets it by its number. Two scripts do, with command 0x09
(`ScriptCommandStoryBitsSet` (`0x8005C504`), set a list of flags):
script 14 (Reza and the Thieves' Guild, maps 39 and 42) at offset 0x6634,
and script 200 (the world map's select menu, with 0x266). In script 14 it
is the Thieves' Crest: after "Kyle! You passed! Congratulations, my
friend. And, welcome to the Thieves' Guild! Here, take this..." the flag
is set, and the next line is "Oops, I almost forgot...check out the back
of your crest for instructions on how to open treasure chests. You'll be
able to crack any chest, no matter how tightly it's locked." The crest is
item 0xB0 ("Thieves' Crest", string 0x1B0): in the image's item table
(`g_BtlItemTable` (`0x800A9A68`)) its kind is 6 and its price 0, and no
row of the shops' stock (`g_ShopStockRows` (`0x800A8468`)) holds it:
given, never sold. The scripts were searched with station-tools.

A message is the 8-byte template `g_MapObjectKind7Message` (`0x800B87F0`:
the words 0, 0xFF00, 0xFF03, 0xFFFF) with its first word replaced by a
string splice, through `MessageDisp` (`0x8005AD50`) → `MessageCodesConv`
(`0x8005AC44`) → `MessageStringDisp(.., 1)` (`0x8005AB98`): the string in
the three-row window, a button, the close.

### Give silver, give an item — commands 0x1C and 0x1F

`ScriptCommandMoneyAdd` (`0x8005CF6C`, lns_intp.c), `{u16 0x1C, u8 k,
…, s32 amount}`: the amount is the 32-bit word at the first multiple of 4
past the k byte (+4 for a command on a multiple of 4, +6 otherwise). The
silver `*g_GameMoney` rises by the amount and stops at 9,999,999 (the
amount is cut to 9,999,999 less the silver); unless k is 1 the number
notice shows what was added, with verb k + 1. `ScriptCommandChrItemGive`
(`0x8005D0E0`), `{u16 0x1F, u8 item, u8 k}`: unless k is 1 the item
notice comes first, then the item is placed (`ChrItemGive(0, item)`
(`0x800577C4`)); when that answers 0 and `g_ChrItemNoRoomState` is not 1,
`g_ChrItemNoRoomState = 2` (not taken). So an item is in the inventory
only after its notice has been dismissed.

### The inventory rule — `ChrItemGive` (`0x800577C4`)

`ChrItemGive(who, item)` (chr_item.c). There are no counts and nothing
stacks: one item a cell (a byte, 0 = empty), the same item twice is two
cells. The cells (their addresses set by `GameStart` (`0x80055B60`),
game_start.c): each member's six carried cells, `ChrRecord` +0x24..+0x29
(`g_ChrRecords + id × 0x40`; +0x1E..+0x23 are the six worn cells); the
shared bag, 0x60 cells at `g_ChrItemList0C`, which are the cells of
member id 0xC; eight pending cells at `g_ChrItemWaitingList`; the silver,
a u32 at `*g_GameMoney`.

The party slots in order (`g_PartyIds[i]`, `i < g_PartyCount`), ids 8, 9
and 10 passed over, `who` ≠ 0 = only that member: the first slot with an
empty cell (`ChrItemRoomCheck` (`0x80057BE8`)) takes the item in its first
empty cell, and that list is sorted (`ChrItemListSort` (`0x80058B3C`),
chr_worn.c: item kinds below 5 first — or last, with options byte 2 — then
by `g_ChrItemSortRanks[kind]`, u8[8] at `0x80012864`, then by id). "Full"
is "no party slot has an empty cell". An item whose mask (+0x10 of its
row) has bit 15 — retail items 17, 56, 82, 110, 144, 168 — offered to a
member other than ids 1 and 0xB goes to the bag's last cell that is empty
or holds an item without that bit, and the member gets what that cell
held; with no such cell the member keeps it.

Nobody has room: while flag 0x2D6 is set the item is remembered in
`g_ChrItemNoRoomItem`. With `g_ChrItemNoRoomState == 0` the text
`g_ChrItemNoRoomString` (`0x800B45F4`: `F6 6A 03 FF`, string 0x66A "I
have to drop something / I can't carry any more, Alex!" and the close)
shows and the field menu opens on page 0x16 (`MenuRun(0x16)`
(`0x8006A024`)); the answer is 1 exactly when flag 0x2D6 is clear
afterwards. The page clears it when a drop made room: `MenuItemDropStep`
(`0x8006E4A0`, menu_page.c) raises `g_ChrItemNoRoomState` by 2 when the
page opens, and after a drop at state 2 calls `ChrItemGive(0,
g_ChrItemNoRoomItem)` and clears the flag; backing out takes the 2 off
again. With `g_ChrItemNoRoomState` not 0 the item goes to the pending
cells (`ChrItemWaitingAdd` (`0x80057A18`)) and the answer is 1.

State 1 is a script run without flag 0x2D6 (`ScriptLabelRun` sets 0 when
the flag is set and 1 when not). Its items wait in the pending cells, and
when the run has ended and `ChrItemWaitingCheck` (`0x80057DA4`) finds one,
`ScriptLabelRun` shows the same string and opens page 0x16, which makes
the state 3. At 3 the page refuses to be left (sound 4). Each drop gives
out one pending item, the last first (`ChrItemWaitingGive`
(`0x80057DE0`)); when the one given out is the first cell, the state
becomes 2 and the page may be left, which clears the pending cells.

### Close the gap a removed item leaves — `ChrItemListGapClose` (`0x800589E0`)

`void ChrItemListGapClose(u16 list, u8 cell)` (`0x800589E0`, chr_worn.c)
CLOSES THE GAP a removed item leaves in one of the three item lists. Only
caller: `ChrItemRemove` (`0x80057A54`, chr_item.c, the inventory remove),
which zeroes the found cell and then calls this with the list and the cell's
index; chr_item.c declares it `void ChrItemListGapClose(u16 list, u8 cell);`.

| `list` | cells | N |
|---|---|---|
| 0xD | the pending cells, `g_ChrItemWaitingList` (pointer held in an s32 global) | 8 |
| 0xC | the shared bag, `g_ChrItemList0C` (same) | 0x60 |
| any other value | member `list`'s carried cells: `ChrRecord.items`, bytes `+0x24..+0x29` of its 0x40 record, `g_ChrRecords + list * 0x40` | 6 |

All three arms are the same loop:

```
for (i = cell + 1; i < N; i++) {
    c[i - 1] = c[i];
    if (c[i] == 0) break;
}
c[i - 1] = 0;
```

- Every cell after `cell` moves down one place, up to and including the
  first empty one. Order is kept.
- Ran to the end (`i == N`): the final store clears the LAST cell of the
  list, `c[N - 1]`, the cell the last copy moved from.
- Stopped on an empty cell at `i`: the copy has already put that 0 at
  `i - 1`, and the final store writes 0 at `i - 1` again (`i` is not
  advanced on the break). Cells above `i` are not touched.
- The value in `cell` itself is never read. With `cell` = the last index
  (`cell + 1 == N`) the loop does not run and `c[cell]` is cleared.
- The member arm takes ANY id: no test for ids 8-10 or for a record that
  exists; the caller has filtered those (`ChrItemRemove` skips ids 8, 9 and
  10).
- `list` is a u16 and `cell` a u8 parameter; the pointer globals are re-read
  from memory after every byte store.
- No call, no RNG draw, no frame wait. Leaf function, no stack frame.

### The item / number notice — frames and layout

Not the three-row window: one line, text row 5, at grid cell (8, 4) =
screen (64, 32) (`MessageLineDisp` (`0x8005AD94`) shows line 5 there), on
a band of its own. `ScriptFrameStep` (`0x8005A1F0`) (script_run.c) draws
that band every frame while `g_MessageNoticeBand` is 1 or 2 —
`packet_box_add(0, y, 0x140, h, 0x2000000, 0x40)` (`0x80015A60`), the same
primitive, colour word and ordering-table entry as the message window's
shade (`MessageShadeDraw` (`0x80015E88`)): y 0x14, h 0x20 for an item
(1), y 0x1C, h 0x18 for a number (2).

* **An item** (`MessageNoticeItemDisp` (`0x8005BB6C`)): the icon, 24 × 24
  at grid cell (4, 3) = (32, 24) (`CellIconSet` (`0x800573B8`) →
  `IconCellSet` (`0x8001AF04`); drawn by `IconCellDraw` (`0x8001AD60`):
  texture page 0x92, CLUT 0x7D80, entry 0x20); the text "<verb> <name>" —
  string `0x65A + n` (Lost, Got, Use, Gave, Bet; a chest's n is 1), the
  string `D_800B87E4` (`0x800B87E4`: one space) and string `item | 0x100`.
  16 frames + the button wait: 3 before the text, 1 after the verb, 1
  after the name, 1 showing the row, the wait, 8 clearing the five rows,
  1, and 1 with the icon and the band gone.
* **A number** (`MessageNoticeNumberDisp` (`0x8005BA70`)): the verb alone
  at (64, 32); the number's last five characters as 8 × 16 digits of
  sub-CLUT 14 in cells (0xB..0xF, 4) = x 88..127, right-aligned
  (`CellNumberSet` (`0x80057400`), `"%10d"`); the unit sign, atlas glyph
  0x8F of sub-CLUT 12, at cell (0x10, 4) = (128, 32) (`CellGlyphSet`
  (`0x80057500`); `ankprint_glyph_draw` (`0x80025B90`), entry 0x30). 12
  frames + the wait.
* **The wait** (`MessageNoticeDisp` (`0x8005BD84`)) ends on the poll that
  finds pad bit 0x40 newly down. The pad record is started with 0x40 held
  (`pad_record_init(rec, 0x14, 0x40, 0)` (`0x8002687C`)), so a button
  still down from before must be let go first. No time-out.
* **Sounds**: `SoundPlay(0xB, 2)` (`0x800555F0`) when the lid moves (open,
  and shut again on a put-back); `(0xC, 1)` at the start of every notice
  (`MessageNoticeDisp`, lns_intp.c), and from the make-room page after a
  drop (its sound request 0xC). `SoundPlay` (music_voice.c) turns 0xB and
  0xC into kind 1, sequences 8 and 9 of bank 0, not of the area's bank 1
  (`sound_effect_play` (`0x80024514`)), except while `g_SoundMapNumber` is
  0x32 or 0: there both are refused and sequence 1 of bank 0 plays in
  their place.
* `MapStatusClear` (`0x8005A7C8`) (message_disp.c) is the first call of
  both notices: it takes the status strip down and runs one frame more
  when the idle counter `g_MapIdleCount` has reached 300 or the strip is
  up.

Frames of a whole chest, W = a button wait: open already 0; locked or
empty — the message window (9 open (`MessageOpen` (`0x8005A6D0`)), 2 the
row when the message speed `g_MessageSpeed` is 0, W, 18 close
(`MessageClose` (`0x8005A72C`)), plus the string's own); silver 12 + W;
an item 16 + W; nobody can carry it — 16 + W, string 0x66A in the message
window, the make-room page, and with no room made string 0x66B. The
message window's W also ends on its own after (line + 1) × 60 + 0x28
frames when `g_MessagePressWait` is 0 (`MessageControlRun` (`0x8005AF20`),
code 0xFF00); the notices' W does not. The lead stands in pose 0xA
(animations 37..40 of its sheet) from the opening to the end.

### Examine cells (event kind 6), and how a shop opens

Not through a row. **A shop's counter is two cells of the attribute plane
with an event word of kind 6** (`cell & 0x7FF = 0x300 | label`): the
cells the lead stands on in front of the counter. The cell dispatch
`MapAttributeRun` (`0x800590C0`) (map_attribute.c) takes the first
attribute under the party's box whose low 7 bits are not 0 and whose kind
is below 10 (`MapAttributeFind` (`0x80069A20`)) and switches on its kind,
bits 7 up. `MapPartyStep`
(`0x80056248`) (map_step.c) calls it every field frame after the pad poll
(`MapPadStep` (`0x80055EC4`)) and the walk, and before its switch on the
pressed buttons. It runs kind 6 as

    case 6: if (g_MapPadPressedButtons & 0x40) MapAttributeKind6Run(arg);   /* ScriptLabelRun(arg & 0xFF) */

(`MapAttributeKind6Run` (`0x80059CA8`), map_change.c) — in every frame the
probe reports the cell and the button went down in that frame's poll,
whichever way the lead faces; never on entry alone. (Kinds 4 and 5 run
with no button test, in every frame the lead is on the cell.)
`g_MapPadHeldButtons` / `g_MapPadPressedButtons` are set by `MapPadStep`
alone (`GameStart` only zeroes them), whose one caller is `MapPartyStep`:
a message window's frames (`ScriptFrameStep`) do not poll, so a press is
measured against the pad word of the last field frame. The talk scan
(`case 0x40` of the switch that follows) runs in the same frame, after the
cell's script has returned; that switch compares the whole word with
0x40, so it runs only when no other button went down in the same poll.

The label's script greets, then holds event-script command 0x2E
(`ScriptCommandShopRun` (`0x8005E384`), lns_intp.c; 4 bytes: `2E 00
<shop> 00`). It calls `MapStatusClear`, then `ShopRun(shop)`
(`0x80050694`) (shop_run.c), then `MessageRecordsInit` (`0x8005A580`).
`ShopRun` does nothing for shop 0 or 8; for any other,
`g_ShopStockRow = shop − 1`, then `ShopOpen()` (`0x800506EC`) (open),
`ShopStepsRun()` (`0x800516CC`) (the shop's loop), `ShopClose()`
(`0x800512C4`) (close), all three in shop_screen.c.

Vane (map 31, script 9): the shopkeeper is definition 28 — kind 2, sheet
193, behaviour 3, at pixel (616, 1856), event byte 87 — and cells (76, 236)
and (77, 236) hold 0x0357 = kind 6, label 87. Label 87 is `03 00 86 01` (a
jump to label 390), flag tests, a TEXT, `2E 00 0A 00` — shop 10 — and a
jump to the farewell TEXT. Labels 88, 89, 90 are shops 11, 12, 13 the same
way: cells (76, 206) / (77, 206), (76, 180) / (77, 180), (114, 180) / (115,
180), definitions 29, 30, 31. The shopkeeper's own event byte is the same
label. Across the maps, of the labels that reach a command 0x2E, 38 are
named by kind-6 cells, 38 by a kind-1 / kind-2 definition's event byte
(often both) and one by a kind-4 cell; a kind-7 row never opens a shop.

Kind 6 is also the bookshelf, the sign and the like: map 31 alone has 47
labels on kind-6 cells (7, 20..59, 87..90, 100, 101). And a thing to examine
that is a row (kind 7, event byte 0xF0..0xFA) runs labels 0x320..0x32A: none
of the 46 reaches a shop; on their way those scripts meet command 0x1F 26
times, 0x1C 4 times and 0x2B (a battle) 3 times.

### Not yet known

* `MenuRun` (`0x8006A024`)'s own frames (the menu's opening and closing,
  `MenuOpen` (`0x8006A328`) / `MenuClose` (`0x8006A81C`)) were not
  counted, so the frames of a chest whose item makes the make-room page
  open are not known in full.

## The opening's own scene

The scenes below are read from the disc's own event scripts, walked and
their text read with station-tools. A portrait in a message is the code
`0xFA00 | face` (`MessageControlRun` (`0x8005AF20`): 0xFAnn shows face nn
in the first place, 0xFBnn in the second). Every command id below is named
by its handler (§"The command table"). Command bytes are written as the id
followed by the operands; the command word's high byte, 0, is left out. A
map's script is byte 8 of its `MapTbl` record (`MapTblGet`
(`0x80068618`)).

### Command ids met here, by their handlers

| id | handler | operands | what |
|---|---|---|---|
| 0x29 | `ScriptCommandMapFadeOut` (`0x8005DE88`) | `[29][len][kind]` | FADEOUT: `MapFadeOut(g_MapLayersMask \| 0x130, len, colour, g_MoveChrViewFollow, g_MoveChrSecondViewFollow)` (`0x800568D4`). The colour is −1 (black) unless, while the fade's state byte `g_MapFadeState` is 0, kind is 1: −2 (`g_LayerColHighest`, white); kind 2 sets `g_MapFadeState = 2` (the party-lit fade) |
| 0x2A | `ScriptCommandMapFadeIn` (`0x8005DF08`) | `[2A][len]` | FADEIN: `MapFadeIn(g_MapLayersMask \| 0x130, len, g_MoveChrViewFollow, g_MoveChrSecondViewFollow)` (`0x800569E0`) |
| 0x2D | `ScriptCommandMoviePlay` (`0x8005E2B4`) | `[2D][n][c][bgm]` | PLAY MOVIE `n` (`MoviePlay(n)` (`0x800232D4`), the file `_B%03d.STR`; video.md); `bgm != 0xFF` makes it the music number first; `c != 2`: the map again (`MapReload` (`0x800686EC`): the area's sound bank, the music, the map), its bg changes (`MapBgChangeStoryBitsCopy` (`0x80059B6C`)) and the view; `c == 0`: a 25-frame fade in |
| 0x30 | `ScriptCommandMusicPlay` (`0x8005E494`) | `[30][bgm][time]` | the music: `MusicPlay(bgm, g_MusicKind, time)` (`0x80054F2C`) (0 stops); then one frame |
| 0x31 | `ScriptCommandSoundPlay` (`0x8005E524`) | `[31][id]` | a sound request: `SoundPlay(id, slot)` on slots 4 and 5 in turn; id 0 stops both |
| 0x46 | `ScriptCommandMapColSet` (`0x8005DDC4`) | `[46][row][scale][now]` | the map's colour, `g_MapColRows[row] × scale`, clamped ±0xFF (`MapColSet` (`0x80059D10`)): `now` ≠ 0 sets it at once (unless a fade is running or the draw area is closed), `now` = 0 ramps to it (`MapColFadeStart` (`0x80059CC8`), nothing for row 0) |
| 0x52 / 0x53 / 0x54 | `ScriptCommandVoiceSet` (`0x8005E9FC`) / `ScriptCommandVoiceStart` (`0x8005EA44`) / `ScriptCommandVoiceEndWait` (`0x8005EA8C`) | `[52][clip]` | VOICELOAD (`VoiceSet` (`0x80055470`), after waiting for a clip still playing), VOICE (`VoiceStart` (`0x800554C4`); the error hang, number 9, when no clip is loaded), VOICESYNC (wait while the clip plays, at most 0x12C0 frames) — the XA clips of sound.md §"The XA voice clips", same table `g_XaVoices` (`0x800ADFF2`) |
| 0x5A | `ScriptCommandMusicLevelSet` (`0x8005E5A4`) | `[5A][level][time]` | the music's level ramp, `MusicLevelStep` (`0x80055310`), over time × 30 frames (audio.md) |
| 0x3B / 0x3C | `ScriptCommandMoveChrAttributeCheckOn` (`0x800628C4`) / `ScriptCommandMoveChrAttributeCheckOff` (`0x800628E0`) | — | `g_MoveChrAttributeCheckOn = 1` / `0`. A mover's walk hands `g_MoveChrAttributeCheckOn == 0` to `MapSpotMoveTry` (`0x8006661C`), which tests the cells only for 0, so after 0x3C a mover walks through every cell; `MoveChrInit` (`0x8005F1C8`) sets it back to 1 at every run. Scenes open with 0x3C (script 1 labels 20, 22, 32) |

Move-script ops (command 0x38): **F0 g** (`MoveCommandPoseSet`
(`0x80062FF4`)) = `MapAnimationPoseSet(actor, g, +0x21)` — pose g, which
is group **g − 1** in §"What the pose groups are on the party sheets" (F0
01 = group 0, the walk); **F1 f** (`MoveCommandFacingSet` (`0x80063038`))
= `+0x21 = f`, then `MapAnimationPoseSet(actor, −1, f)`; F3 and F4 are
`MoveCommandMoveTo` (`0x800630A4`) and `MoveCommandMoveBy` (`0x8006312C`);
**F5 s** (`MoveCommandSpeedSet` (`0x800632D8`)) = `+0x1E =
g_MoveSpeedPixels[s]` (pixels a tick), `+0x24 = +0x25 =
g_MoveSpeedIntervals[s]` (frames a tick); **F6 n** (`MoveCommandWait`
(`0x80063330`)) = n frames; F7, PLACE, is `MoveCommandPlace`
(`0x8006338C`). Row 0 is the view, row 1 the lead, rows 2..6 the party's
other places (they hold its records after command 0x3A); command 0x34
binds a map object to a row below 0xB (the scenes use 7..10) or makes a
row's own actor from 0xB up (§"Event-script command 0x34, bind or spawn a
script actor").

### Burg Springs (map 36, script 1) — "Luna at the Springs" is a movie

The springs' kind-4 cells run labels 5 (29 cells: the way out, scene entry
4 → Burg), **20** (32 cells) and 22 (3 cells; the late-game "I always get
such a strange feeling when we practice our songs, Alex."). Label 20 (0xA12):
a flag test; 0x3C; Alex (row 1) `F0 01 F5 03 F1 01 F4 (0, −3)` and the
camera with him, then `F4 (0, −13)`; **FADEOUT 100**; **`2D 03 01 FF` —
movie 3, `_B003.STR`, "Luna at the Springs" (string 0x504)**: the singing
and the ocarina are this movie, not a field scene; 0x23 member 6 (Luna)
joins; 0x27 with an inline party-place row (view limits 0..320 × 0..624,
the party to (160, 112) facing down); three PLACEs at `F5 06`: Alex (0x14,
0x10) facing up, row 2 (0x12, 0x0E) facing right, Luna (row 3) (0x14,
0x0E) facing down; VOICELOAD 0x2B; **FADEIN 100**; Nall's "Hey! You're
good!…". After Luna's "Alex, why would you travel to such a dangerous
place?!": VOICE (clip 0x2B — the set-up voice of presentations 5..8, the
Dragon spells), 60 frames, sound 0x83 and the shake (0x2C) — "Yikes!" /
"It sounded like a…roar?".

**Luna singing on the field** is script 2 (Burg, map 21) label 282:
`52 2C` (clip 0x2C), the text, `5A 64 01` (the music down to 0x64 over 30
frames), `53`, `54` (wait for the clip), `5A FB 01`.

**Alex's song is BGM 31, "Dragon--Someday"**: script 2 at 0x506A plays
`30 1F 05` right after Luna's "Alex...?" and before "I know where you are,
Alex."; the scene change that follows (`26 D2 00 …`, an inline entry to
map 34) runs script 1 label 210: the night colour (`46 01 09 01`), `5A FB
02`, FADEIN 100, Alex waits 600 frames (`F6 FF F6 FF F6 5A`), Luna walks to
him: "I heard your song on the breeze."; then BGM 0x42 (66, "Chirping
Crickets").

### Ghaleon takes Luna — Quark's lair (map 23, script 1 label 32)

The game's own version of the opening's middle: row 0xB is a spawned
Ghaleon on **sheet 0xD0** of set 23 (`34 0B D0 02 …`), row 0xC sheet 0x85;
after movie 0x15 "Ghaleon vs Quark" the 0x2D command's BGM operand is 0x27
(**39, "Magic Emperor Ghaleon"**); VOICE 0x5E, "AAAHHHHH!!!"; Ghaleon `F0 08`
100 frames (on sheet 0xD0); a spawned actor on **sheet 0x86** is his spell,
flying down at `F5 05` with sound 0x87; Alex and Nall `F0 02` knocked back
two cells; **three white flashes**: `29 01 01` (FADEOUT 1 frame to white),
`2A 0F` (FADEIN 15), three times; Alex `F0 02` facing up 8 frames, then
**`F0 03` facing left** (lying), 280 frames; sound 0x0E; Ghaleon `F0 04`
and a PLACE away; **Luna `F0 0B`** at her place, then beside him; the music
stops (`30 00 0A`) and **FADEOUT 250**. Sheets 0x85, 0x86 and 0xD0 are
registered in set 23 but drawn by no definition: here the script's own
actors (command 0x34 from row 0xB) draw them.

### What the pose groups are on the party sheets

All twelve party sheets have 53 animations: id 0 and groups 0..12. Group g
is pose g + 1 in `MapAnimationPoseSet`'s counting, animations 4g + 1 ..
4g + 4, one for each facing column. Read from the sheets' own animation
and step tables on the disc (with station-tools):

* **group 3 (F0 04) and group 12 (F0 0D) are the teleport**: group 3 a held
  58-frame run of glowing poses ending on a pose that draws nothing (Luna
  88) or a sparkle (Ghaleon 47); group 12 the same run backwards, ending
  standing. They are the groups the party-lit fades put the party on:
  `MapPartyPose4Play` (`0x80068334`) sets pose 4 (group 3) on every party
  record and twin, with `SoundPlay(0xE, 1)` and 60 frames;
  `MapFadePartyIn` (`0x80056CB8`) sets pose 0xD (group 12), and
  `MapPartyOnPlay` (`0x80068400`) then shows the records for 60 frames with
  the same sound. Script 1 at 0x531C places the whole party on `F0 0D`
  and, 60 frames on, `F0 01`: "W...where are we? Dyne's Monument?!" —
  Laike's teleport.
* **group 2 (F0 03) is down**: Alex column 0/1 pose 32, columns 2/3 poses
  33/34 (lying); Luna 16 / 18 / 17. The scripts lay the party down with it
  after a lost fight (script 39 at 0x1988) or a blow (script 14 label 35)
  and put it to bed with it (below).
* **Luna's group 10 (F0 0B)**, column 0 only: poses 149, 88, 149, 88, 150,
  88, 150, 88, 151, 152, 153 then 88 — 30 frames, held on 88: she fades and
  is gone. Her group 7 (F0 08), columns 2/3, holds the pink item and glows;
  her group 9 (F0 0A) is the goddess form (poses 57..60).
* **Ghaleon (the party sheet of id 8)**: group 0 (the walk) is the hooded
  Ghaleon (poses 0..11); group 7 (F0 08) is the unhooded, white-haired walk
  (31..42); group 9 (F0 0A) walks with a staff (18..29). The F0 08 of
  Quark's lair plays on sheet 0xD0, not on this sheet.

### A bed

Script 6 (Meribia's inn, maps 30 / 69) at 0x5358: "Ha, ha! Good night,
Nall! And good night, Alex." — FADEOUT 50, movie 5 ("Luna's Dream"), then
Alex, Nall and Luna are each put on **F0 03** and moved (`F3`) onto their
beds (Alex facing right, Luna facing down), FADEIN 50; Luna `F0 03` facing
down **200 frames**, then `F0 01` facing down **100 frames**, walks out of
bed (`F5 02 F4 (4, 0)`, `F4 (0, −4)`): "Good morning...". There is no wake-up
of Alex in Vane in the scripts.

**Mia's bed in the Magic Guild (map 33)** is found by its examine label:
script 10 label 31 — "…and you're sullying the sheets of the Guildmaster's
daughter?!" — sits on a ring of 15 kind-6 cells round x 1096..1136, y
96..144 (the bed inside it). The room is party-place row 2
(`MapPartyPlaceRow`) of map 33: view limits 1008..1488 × 0..288, arrival
(1240, 272) facing up, door sound 13. Label 870 is the second answer
("This isn't the time or the place for a nap").

### Lemia

* **Her field sprite is object set 33's definition 19**: sheet 247 (16 × 26
  cells, CLUT row 505), at (520, 352) facing down, flag 84, talk label 88.
  In script 10 (the guild before the story's turn) label 88 jumps to 420,
  which is a bare RETURN; in **script 47** — which script 10 enters with
  command 0x42 (at 0x1264 and 0x6390) — label 88 jumps to 420: **"My name
  is Lemia Ausa... ...but I'm having a difficult time recalling much more
  than that."** beside face 82.
* **Her portraits**: **82** the real Lemia, freed ("The gentle light shining
  forth from Althena's Mirror has awakened me... ...and liberated me from
  the curse of this sorceress!", script 44; "My name is Lemia Ausa...",
  script 47); **83** Lemia resolute ("You must stop the Grindery at all
  costs, my daughter", script 48); **80** Lemia's face as the impostor wears
  it ("Guards! Throw this urchin into the dungeon...", script 47); **81**
  Xenobia, masked.
* **Face 65 is Phacia's.** Script 8 at 0x39EC carries faces [27, 0, 65]:
  "It was your application that allowed us to enter Vane. Thank you,
  Phacia..." then face 65: "Thanks aren't necessary. All I did was give
  you a piece of paper with my name on it." — the woman of "Please accept
  this application form and at least visit Vane...".
* The party's faces follow `(id − 1) × 5 + 1` and the four after it: Alex 1,
  Luna 26, Ghaleon 36 (38: "How dare you...", script 44).

### Ghaleon's own spell, Hell Wave

Ghaleon's own spell is **Hell Wave**, presentation 0xB3 (attack class
0x73, name string 0x373: a class's name is string class + 0x300). It is a
battle's, not the field's. Encounter 0xFE's formation 255 has one slot,
enemy 0x7D, in actor 6 (boss-ai.md). His action function
`BtlEnemy7DActionSet` (`0x800431AC`, btl_plan.c) counts his turns in
`g_BtlSpecialBytes[0]`, which `BtlStateInit` (`0x8002B7DC`) zeroes: his
first three turns plan kind 0xB, no spell; the fourth class 0x63 (Nitro
Dagger, spell 0x2B); every turn after that class 0x73 at
`BtlPartyTargetPick(2)` (`0x8003995C`), a draw among the party's slots.
Before the show, `BtlActingShowRun` (`0x8003A158`) starts voice 0x38 for
actor 6 in battle 0xFE. Show 0xB3 (`BtlShowB3Run` (`0x8003F2EC`),
btl_enemy.c) plays two extra records of effect 0x5D in battle 0xFE (0x60
in any other) at the caster's place, 0x3A pixels apart, while the
caster's own record is off, with sound 0x66; it draws no random number.
