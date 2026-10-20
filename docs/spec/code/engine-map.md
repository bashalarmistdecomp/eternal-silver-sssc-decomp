# Lunar SSSC — the main executable's memory map

What the main executable's image (`SLUS_006.28`, once the boot file's loader
has unpacked it: [subsystems/bootstrap.md](subsystems/bootstrap.md)) holds at which address: the parts of the image, the game's
own code file by file, and the tables of function addresses the code calls
through. The C files are under `decomp/SLUS_006.28/src/`; each file's
header comment says what it holds, and the rows below are taken from those
headers. How many functions have names is in the README (§"Names").

## The image: `0x80011000` to `0x800B9000` (688 KB)

| Range | What |
|---|---|
| `0x80011000`–`0x80013C43` | Read-only data placed ahead of the code (`config/program.ld`): the game files' constant tables and strings, and the jump tables of their `switch` statements |
| `0x80013C44`–`0x80013D07` | Sony's start-up object `2mbyte.obj` (2mbyte.c): `__main`'s eight bytes at `0x80013C44`, still data to the tree ([naming.md](../../naming.md) §"Open"), then `__SN_ENTRY_POINT` (`0x80013C4C`), which clears `0x800B8820` to `0x800EAFE8`, calls `InitHeap` and then `main` ([subsystems/bootstrap.md](subsystems/bootstrap.md), [subsystems/bios-calls.md](subsystems/bios-calls.md)) |
| `0x80013D08`–`0x8008B387` | The game's own code, all C (below) |
| `0x8008B388`–`0x800A7D97` | Sony's libraries, PSY-Q release 4.5 ([naming.md](../../naming.md) §"Which release of Sony's libraries the programs link"; the members, address by address, in [subsystems/psyq/index.md](subsystems/psyq/index.md)) |
| `0x800A7D98`–`0x800B881F` | Data: the game's and the libraries' tables and initialised variables, with the small data that `$gp` (`0x800B8540`) reaches |
| `0x800B8820`–`0x800B8FFF` | Zero in the image: the start of the area the start-up clears, which runs on past the image to `0x800EAFE8` |

## The game's code, file by file

Start addresses are those of each file's first function; a row that names
several files starts at the first of them, and its files follow in address
order up to the next row.

| Start | File | What it holds |
|---|---|---|
| `0x80013D08` | main.c | `main` (`0x80013D2C`), the fixed buffers (`main_buffer_init`), the build stamp's compare and copy |
| `0x80013EB8` | bg_cover.c | the cover grid: which tiles `bg_first_draw` draws a second time over sprites |
| `0x80014014` | bg_tile.c | the two tile layers: their draws and scrolls, the tile data's block cache, loads, sets and copies (`bg_copy`), and the attribute plane's twins of them |
| `0x800159E0` | packet_shape.c | one-colour packets: boxes, four-colour boxes, triangles, lines, the draw area |
| `0x80015E88` | message_draw.c | the message window's shade and its waiting arrow |
| `0x80015F94` | start_picture.c | the start's full-screen picture |
| `0x80016128` | font_cell.c | the font's blocks on the text grid, their image loads, the lines viewer |
| `0x800169A4` | sprite_draw.c | the textured rectangles sprites are drawn with, and the sprite pages they draw from |
| `0x8001835C` | btl_draw.c | the battle's picture: background grey and wave, `BtlFrameDraw`, its data load, the party's images |
| `0x80018AE0` | sprite_list.c | the sprite list: set-up, reset, each sprite's pieces drawn, the list's draw, the mix |
| `0x8001ABA4` | icon.c | ICON.C: the icons on the cell grid, their slots, image loads and cells |
| `0x8001B050` | exception.c | `assert_fail` (empty in this build) and the hook on the exception vector |
| `0x8001B15C` | face.c | FACE.C: the portrait slots, their loads, the face cells |
| `0x8001B63C` | cd_track.c | the disc's audio tracks: table of contents, play, switch |
| `0x8001B754` | outline.c | the shapes and outlines the menus draw round their records |
| `0x8001C0CC` | screen_fade.c | the fade of the whole screen |
| `0x8001C2C8` | layer_grey.c | the layers' grey levels |
| `0x8001C548` | rng.c | the random generator |
| `0x8001C7B0` | mth_curv.c | MTH_CURV.C: square root, hypotenuse, splines, curves, 16.16 fixed point |
| `0x8001D278` | tile_layer.c | the tile layer: its cells, wave, draw, step, scroll, patterns and texture pages |
| `0x8001DE94` | layer_col.c | the layers' colours and mixes, and their fades |
| `0x8001EE98` | frame.c | the frame and the screen: `system_init`, the screen modes, draw area, `frame_start`, `frame_end`, `frame_step` |
| `0x8001F508` | pad_update.c | the pad's work for one frame: the actuators' list and the reading of both ports |
| `0x80020030` | pad_start.c | the pad's start and the buttons `pad_update` found |
| `0x800200B0` | palette_buf.c | the palette buffer and its marked rows sent to video memory |
| `0x80020230` | resource_load.c | a resource of LUNADATA.FIL read and sought; its list of commands walked |
| `0x800204D0` | tpage.c | a texture page's place and pixels sent to video memory |
| `0x80020588` | palette_row.c | palette rows sent and set, and the colour fixes |
| `0x800208A0` | resource_command.c | the handlers of a resource's commands 2 to 18 |
| `0x80020CD0` | map_resource.c | the map's and the script's resources found and loaded |
| `0x80020DC0` | cdread.c | CDREAD.C: sectors read, the disc's directory, `file_load`, `cd_load`, and `pc_load`, the host's twin of `cd_load` |
| `0x800216EC` | fmt.c | text output: `debug_printf` and `printf` (both empty), the `sim:con` console, `fmt_sprintf`, `fmt_vsprintf` |
| `0x80021D6C` | inflate.c | gzip 1.2.4's inflate and `lz_unpack` |
| `0x80022D58` | xmalloc.c | XMALLOC.C: the top that moves down and the malloc2 heap under it |
| `0x80023070` | overlay.c | OVERLAY.C: the MENU.BIN overlay loaded, and `MoviePlay` through it |
| `0x8002347C` | xa.c | XA sound from the disc |
| `0x80023978` | sound.c | SOUND.C: sequences, banks, music load, level and fade, reverb, resource commands 13 to 17 |
| `0x800247AC` | picture.c | a picture's show: its XA sound, video memory saved and restored |
| `0x80024B70` | cd_change.c | the change of discs |
| `0x80024E3C` | card.c | the memory card: start, commands, directory, files, the save's read and write |
| `0x8002566C` | packet_sprite.c | textured packets: texture page word, sprites |
| `0x80025864` | ankprint.c | ANKPRINT.C: the 40 x 28 text grid, `printmes` |
| `0x80025DF4` | profile.c | the frame's time profile |
| `0x8002603C` | screen_wait.c | the display switched on or off, two blanks waited |
| `0x8002609C` | mem_move.c | memory cleared and moved |
| `0x8002628C` | palette_animation.c | the palette animations |
| `0x800265BC` | tile_animation.c | the animated tiles |
| `0x8002687C` | pad_record.c | a pad record made ready and brought up to date |
| `0x80026AB0` | font_string.c | the font's characters written into a target, strings by id and by bytes |
| `0x80027208` | btl_animation.c to btl_plan.c | the battle: `BtlRun` (`0x80027434`) and its set-up, rounds, walks, actions, hits, statuses, shows of the acting slot, and the bosses' plans ([subsystems/battle.md](subsystems/battle.md), [subsystems/boss-ai.md](subsystems/boss-ai.md)); `chr_levelup.c` (`0x8002844C`) lies among them |
| `0x8004345C` | start_screen.c | what the program shows from its start until the player has chosen |
| `0x80044118` | btl_menu.c to btl_choice.c | the battle's menu, windows, amounts, party panel, messages, and the choices the program makes for enemies and for the party's auto-battle |
| `0x80050694` | shop_run.c, shop_screen.c, shop_strings.c, shop_face.c | the shop (`mem_request.c` lies between the last two) |
| `0x80054C9C` | layer_mix.c | the layers' mix switched bit by bit |
| `0x80054F2C` | music_play.c, music_bank.c, music_voice.c | the music, the XA voices, the sounds by number (`SoundPlay`) |
| `0x800557D4` | map_frame.c | a frame of the map |
| `0x80055B60` | game_start.c | the game's records set up |
| `0x80055EC4` | map_step.c | the parts of a map frame: the pad's, the party's, the objects', the status strip |
| `0x80056750` | chr_level.c | a character's level |
| `0x800568AC` | map_endcopy.c, map_fade.c | `MapFrameEndCopy`; the map's fades, and its picture closed in and opened out |
| `0x80057288` | cell_text.c, cell_face.c | text, icons, numbers, glyphs and faces on the cell grid |
| `0x800576EC` | story.c | the story bits |
| `0x800577C4` | chr_item.c, chr_worn.c | a character's items and worn items |
| `0x80058ED0` | map_music.c | the music the map and its script ask for |
| `0x8005901C` | movie.c | what `MoviePlay` calls around a movie |
| `0x800590C0` | map_attribute.c | the map-cell events under the party |
| `0x8005922C` | map_change.c | a map change, a door, the tile changes, the map's colour, the party's HP taken |
| `0x80059F4C` | script_run.c | the script run from a label |
| `0x8005A580` | message_disp.c | the message window |
| `0x8005B568` | lns_intp.c | the script's commands |
| `0x8005F1C8` | lns_move.c | LNS_MOVE.C: the moving characters' rows, `MoveChrAction` |
| `0x80061C54` | script_move.c | the script commands that drive the rows |
| `0x80062F04` | move_command.c, step_command.c | the move commands and the map objects' step commands |
| `0x80063A8C` | map_sprite.c | the map's sprite data loaded, freed and reloaded |
| `0x80064898` | map_party.c, party.c | the party's field records and twins, the party's places |
| `0x80065098` | map_object.c | the map objects made |
| `0x80065CF8` | map_animation.c | the field's animations |
| `0x800664D4` | map_walk.c | the party's walk |
| `0x80066F54` | map_ring.c | the party's ring of past places |
| `0x8006736C` | map_think.c | the map objects' think pass |
| `0x800679F4` | map_talk.c | the party's box against the objects, and the talk |
| `0x8006824C` | map_load.c | a map set, sought, loaded, its layers and effects |
| `0x80068DB8` | map_view.c | the map's view and second view |
| `0x800692D4` | map_over.c | whether a box may go over the attribute plane |
| `0x80069E60` | menu_glyph.c | the menu's glyphs and numbers |
| `0x8006A024` | menu_run.c to menu_list.c | the field menu, its pages, save and load, options, the use of items and spells ([subsystems/menu-ui.md](subsystems/menu-ui.md)); `face_list.c` lies among them |
| `0x80077EE8` | map_look.c | the map objects' thinks 6 to 8: the enemies' looks and chases ([subsystems/map-enemies.md](subsystems/map-enemies.md)) |
| `0x8007AB64` | map_btl.c | the battle the map starts |
| `0x8007B310` | map_touch.c | the touched map object |
| `0x8007B7C8` | btl_show.c to btl_effect.c | the battle's effect shows: spells, items, weapons, bosses, and their frame and effects |

## Tables of function addresses

The code calls through these runs of function addresses in the data. Some
still carry a label the first split gave them; those labels were never
read and say nothing about what the table holds. A table can also hold
another label inside it: every such label in the old lists
(`scene_phase_table`, `field_state_table`, `entity_type_table`,
`script_opcode_table`, `menu_state_table`, `shop_state_table`,
`save_state_table` and the rest) is a place inside one of these runs, not
a table of its own.

| Address | Entries | Label in the C | What it holds | Called by |
|---|---:|---|---|---|
| `0x800B2EF8` | 132 | `scene_state_table` | the functions an action's row names (battle effects); entries 94 to 131 are `g_BtlActingShowFunctions` (`0x800B3070`), the acting slot's shows 0x97 to 0xBC | `BtlActionRowRun` (`0x80031ABC`), entry function number − 1; `BtlActingShowRun` (`0x8003A158`), entry show − 0x97 |
| `0x800B312C` | 15 | `g_BtlMenuStepFunctions` | the battle menu's steps | `BtlMenuRun` (`0x80044118`) |
| `0x800B3258` | 4 | `entity_ai_table` | the shop's steps | `ShopStepsRun` (`0x800516CC`), entry step − 1 |
| `0x800B333C` | 97 | `g_ScriptCommandFunctions` | the event script's commands | `ScriptLabelRun` (`0x80059FDC`) |
| `0x800B45F8` | 10 | `g_MoveChrCommandFunctions` | the move commands 0xF0 to 0xF9 | `MoveChrCommandRun` (`0x80061800`) |
| `0x800B4620` | 10 | `g_StepCommandFunctions` | the map objects' step commands 0xF0 to 0xF9 | `StepCommandRun` (`0x80067768`) |
| `0x800B4658` | 9 | `vtbl_4658_table` | the map objects' thinks | `MapObjectsThink` (`0x8006736C`), entry `+0x3D & 0x3F` |
| `0x800B59F8` | 23 | `battle_phase_pre_table` | the field menu's pages 0 to 0x16 | `MenuPagesRun` (`0x8006D900`) |
| `0x800B5B1C` | 28 | `battle_state_table` | entries 0 to 7 the uses of an item or spell from the menu; 8 to 21 `g_MapObjectPartyLookFunctions` (`0x800B5B3C`); 22 to 27 `g_MapObjectPartySeenFunctions` (`0x800B5B74`) | `MenuUseRun` (`0x80076444`); `MapObjectPartyLookStep` (`0x80077F1C`); `MapObjectPartySeenStep` (`0x80077FAC`) and `MapObjectPartyAwayStep` (`0x80078060`) |
| `0x800B5CA4` | 90 | `g_BtlShowFunctions` | the battle's shows 1 to 0x5A | `BtlShowNumberRun` (`0x8007BB80`) |

Four of the `switch` jump tables in the read-only data, which the first
split's lists counted among these: `0x80012918` (8 entries) is
`MapAttributeRun` (`0x800590C0`)'s switch on the map-cell event's kind;
`0x80012994` and `0x80012A84` (59 each) are `font_string_walk`
(`0x8005A884`)'s on the message codes; `0x80012BB0` (8) is
`MoveChrGoalCheck` (`0x8005FDAC`)'s on the facing.
