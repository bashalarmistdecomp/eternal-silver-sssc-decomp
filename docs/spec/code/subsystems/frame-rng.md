# Per-frame RNG in battle

What draws from the game's random numbers on an ordinary battle frame, in
what order and under which conditions, and the frame-driven draws outside
battle (the movies' vibration scripts, the field's map objects). The
generator itself is menu-ui.md §"The random numbers — `rng.c`"; the draws a
spell presentation makes of its own are spell-rng.md.

## The frame wait — `BtlFrameStep` (`0x80038D2C`)

Every battle frame passes through `BtlFrameStep` (`0x80038D2C`)
(btl_frame.c), called by the battle logic, by `BtlShowFrameStep`
(`0x80089DDC`) for presentation scripts, by the knockback, the walks and
more. In order:

1. **`g_PadPressedButtons == 2`** (a newly pressed pad value): flip the
   vibration option `g_GameOptions[5]` (`0x800B8968`). On: start the vibration script
   (`g_PadActuatorList = g_BtlActuatorFFList`, `g_PadActuatorRestFlags =
   0x3F`, `g_PadActuatorListWait = 0`). Off: clear those and the motor
   levels `g_PadActuatorLevels` (`0x800B8698`) / `g_PadActuator1Level` (`0x800B8699`). Then
   `SoundPlay(3 - flag, 1)` (`0x800555F0`).
2. **`g_PadPressedButtons == 1`**: flip `g_GameOptions[6]`, `SoundPlay(3 -
   flag, 1)`.
3. If `g_BtlAutoEndAllowed` (`0x800B8C70`) is set, the party AI is on (`g_BtlPartyMode ==
   1`) and `BtlPadHeldCheck(0x10)` (`0x80037604`) sees pad bit 0x10:
   `g_BtlPartyMode = 0` and `BtlWindow1IconClose()` (`0x800497D4`), the AI
   cancel. It enters the frame wait again through `BtlWindowSet` (`0x8004A1C0`) →
   `BtlMenuFrameStep` (`0x8004A1C0`, `0x80048964`), so a cancel runs nested
   frames, each with the same draw sites.
4. **Draw site 1** — if slot 5 holds Nall (`g_BtlSlotIds[5] == 0xC`, by the
   string table): `BtlSlot5RoamStep` (`0x80038EE8`), the idle machine
   (below).
5. Every frame: `BtlMixFlashStep` (`0x8008A0C0`), `BtlAnimationStep(g_BtlRecords,
   0, 0x1C)` (`0x800272A8`) (the actors' animation tick), `BtlExtraRecordsStep`
   (`0x8004163C`), `BtlAmountRecordsStep` (`0x8004AF94`) (the popups' tick),
   `BtlMenuOutlineStep` (`0x800489F0`), `BtlShowVoiceStep` (`0x80089D78`),
   **`BtlFrameDraw` (`0x80018564`) (the arena draw; draw site 2 is under
   it)**, `tile_layer_step` (`0x8001DA78`), and last `return
   sound_music_level_step()` (`0x800245F0`).

No other callee of the frame wait reaches `rng_draw` (`0x8001C770`); the
program's call graph, decoded from its image, and emulator traces of an
idle battle, a cast and the battle's controls agree (both made with
station-tools, a custom decomp toolkit that will be released later). A
function that skips the frame wait on some pass makes no draws there: the
knockback `BtlTargetPushBack` (`0x80037C64`) waits only on passes that move
the target, so at a screen edge no frame passes and nothing is drawn.

## Draw site 1 — the idle machine `BtlSlot5RoamStep` (`0x80038EE8`)

Runs only when slot 5 is Nall (step 4) and `g_BtlSlot5Roaming`
(`0x800B8B26`) is not 0. Three machines of their own on slot 5's 0x40-byte
record, stepped A, B, C every frame; each draws only in its state 0, and a
machine sent back to state 0 draws on the following frame.

* **A — animation** (`g_BtlSlot5RoamAnimationState` (`0x800B8B30`), timer
  `g_BtlSlot5RoamAnimationWait` (`0x800B8B20`)). State 0: draw `rand() & 1`.
  If 1: animation 2 (the record's `animation`, `+0x30`, is 0) or animation
  3 (not 0), state 1 or 2, wait = (`end`, `+0x3A`, `& 0x7FFF`) − 1 — no
  second draw. If 0: a second draw, wait = `rand() & 0x7F`, state 3. States
  1 and 2 wait for `step` (`+0x38`) to equal the wait, then take animation 1
  or 0 and go back to state 0; state 3 counts the wait down until it wraps
  to 0xFFFF. **1 or 2 draws.**
* **B — x drift** (`g_BtlSlot5RoamXState` (`0x800B8B31`), timer
  `g_BtlSlot5RoamXCount` (`0x800B8B22`)). State 0 always draws twice:
  `rand() & 3` — 0: `sprite.attr` (`+0xC`) = 0x20, state 1; 1: `+0xC` =
  0x2020, state 2; 2 or 3: state 3 — then count = `rand() & 0x7F`. State
  1: x (`+6`) + 1 while x < 0x130; state 2: − 1 while x >= 0x11. States 1
  to 3 go back to 0 when the count wraps. **2 draws.**
* **C — y bob** (`g_BtlSlot5RoamYState` (`0x800B8B32`), timer
  `g_BtlSlot5RoamYCount` (`0x800B8B24`), step timer `g_BtlSlot5RoamYWait`
  (`0x800B8B1E`)). State 0 always draws twice: `rand() & 3` — 0: state 1,
  1: state 2 (both with count = `rand() & 0xF` and step timer 7); 2 or 3:
  state 3, count = `rand() & 0x7F`. State 1: y (`sprite.y`, `+0xA`) − 1
  every 7 frames, stopping below 0x59; state 2: + 1, stopping at 0x68 or
  more. States 1 to 3 go back to 0 when the count wraps. **2 draws.**

Per frame: **0 to 6 draws** (A 0, 1 or 2; B 0 or 2; C 0 or 2), in the order
A, B, C.

### When the idle machine is on — the writers of `g_BtlSlot5Roaming`

* **Battle start** — the party bundle `BtlPartyLoad` (`0x8002B114`): after
  placing the party, if slot 5 is Nall it sets `g_BtlSlot5Roaming = 1` and
  zeroes all three machines' states and timers; otherwise
  `g_BtlSlot5Roaming = 0`. So with Nall there, **all three machines start
  in state 0 and draw on the first battle frame — 5 or 6 draws** (A 1 or 2,
  B 2, C 2) before anything else. Battle entry `BtlInit` (`0x80029E68`)
  clears slot 5's id before the bundle in encounter 0xE4, so Nall is absent
  there.
* **Nall's revive** — `BtlSlot5Revive` (`0x80028E5C`) sets
  `g_BtlSlot5Roaming = 0` first: the revive's own frames (the walk, 60 + 33
  frames) have no idle draws.
* **The per-round revive roll** — `BtlSlot5ReviveTry` (`0x80027B6C`), after
  the revive and Nall's walk back: the three states go back to 0 and
  `g_BtlSlot5Roaming = 1`, so the machine is on again and draws on the next
  frame (its timers are not reset here).
* **The victory revive** — `BtlWinRun` (`0x80028140`) → `BtlSlot5Revive(0)`
  does not turn it back on: after a victory revive the idle machine stays
  off.
* **The escape** — `BtlSlot5EscapeMove` (`0x80037578`) (btl_move.c), when
  the party gets away (`BtlPartyEscapeTry` (`0x80027DAC`), `BtlSlotLeave`
  (`0x8002E398`)): with Nall in slot 5 it clears `g_BtlSlot5Roaming`, sets
  his `+0xC` to 0x20 and moves his x by 8 a frame, one frame wait each,
  until it reaches 0x150. Those frames have no idle draws.

## Draw site 2 — the vibration driver `pad_update` (`0x8001F508`)

Reached from the frame wait's own tree, every frame, with no condition:
`BtlFrameStep` → `BtlFrameDraw` (the arena draw) → `frame_end`
(`0x8001F30C`) (the frame's end; `VSync` (`0x800949A8`)) → `pad_frame_step`
(`0x8001EE98`) → `pad_update`. Its draws therefore come after site 1's in
the same frame.

It steps the vibration script at `g_PadActuatorList` (`0x800B86C0`) and both motors. In
the motor way **0x48 (random)**: one `rng_draw` per motor each time that
motor's period timer runs out, adding `rand % (2 * step) − step` to the
level (below the lower limit → the upper limit, above the upper limit → the
lower limit). This runs whenever a vibration script is going — **even with
vibration switched off**: the draws come before the off test, which only
zeroes the levels afterwards. The frame wait's step 1 and spell scripts
(`g_PadActuatorList = g_BtlActuatorFFList` and others) start those scripts.

**Which scripts reach way 0x48.** The 25 vibration scripts outside the
movies are entries 1 to 25 of the table `g_ScriptActuatorLists`
(`0x800AE870`), from `g_MapActuatorOneFrameList` (`0x800AE250`) to
`g_BtlActuatorQuickenList` (`0x800AE3F4`); the battle's code starts them by
address, the event-script command below through the table. Read op by op, exactly one
reaches way 0x48: `g_BtlActuatorRandomList` (`0x800AE310`), entry 9: op
0x49 (motor 1) after a 0x50-frame wait, period 3, for 0x28 frames — ten
draws, on script frames 85, 89 .. 121. The frame wait's own toggle script
`g_BtlActuatorFFList` (`0x800AE258`) and every other one draw nothing. So in battle site 2
is silent except while these start the random script:

* the spells Ice Wall (`BtlShow1DRun` (`0x800813C0`)), Inferno
  (`BtlShow2ARun` (`0x80082D9C`)) and Flash Wind (`BtlShow2FRun`
  (`0x80085194`)) (spell-rng.md);
* the breath attacks' two routines, `BtlPartyRightMove` (`0x80086700`) and
  `BtlPartyRightMovePatternsRun` (`0x80086B64`) (battle.md §"The breath
  attacks' party sweep — `BtlPartyRightMove` (`0x80086700`)" and the
  section after it). `BtlPartyRightMovePatternsRun` starts it a second time
  after the sweep, and that second run reaches its ten draws too: 20 draws
  for Thunder Breath and Cold Breath, 10 for Fire Breath (battle.md §"The
  breath sweep's long form — `BtlPartyRightMovePatternsRun`
  (`0x80086B64`)");
* enemy action 0xBA, `BtlShowBARun` (`0x80040348`), starts it but puts
  another script in its place before frame 85, so it draws nothing
  (battle.md §"The vibration scripts — `g_PadActuatorList`").

On the field the event-script command `ScriptCommandActuatorListStart`
(`0x8005D294`) starts entry n of `g_ScriptActuatorLists` for the byte n the
script gives, so a script that names 9 starts the random script outside
battle; which scripts of the disc do so is not read here.

**The ten movie rumble scripts.** The "play event" routine `MoviePlay`
(`0x800232D4`) starts one of ten more scripts, the entries of
`g_MovieActuatorLists` (`0x800AE8D8`) (entry 0 at `0x800AE420` to entry 9
at `0x800AE774`): entries 0 to 8 for movies 4, 0x11, 0x15, 0x18, 0x1D,
0x24, 0x26, 0x29 and 0x32, entry 9 for movie 0x25. They are not among the
25 above. Six of them — movies 4, 0x15, 0x24, 0x26, 0x29, 0x32 — put motor
1 on way 0x48, and **they draw from this same generator while the movie
plays**: the MENU overlay's play loop `movie_frames_play` (`0x80196414`)
calls `pad_frame_step` once per movie frame shown. Watched to the end they
make 11, 179, 346, 294, 436 and 77 draws; leaving a movie early cuts the
count short, and every viewing draws again. The scripts' movie-frame clock
(op 0xA0, `g_MovieFrameNumber` (`0x801A893C`)) and the evidence are in video.md, "The
movie rumble scripts".

## On the field: the map objects and the event scripts

The same generator is drawn outside battle by:

* two behaviours of the think pass `MapObjectsThink` (`0x8006736C`), when
  an object's think timer (`+0x40`) runs out: behaviour 0, the wander
  (`MapObjectWanderStep` (`0x8006758C`): `rand % 8 + 1`, only when it
  turns), and behaviour 5, stand and turn at random (`MapObjectTurnStep`
  (`0x800678F4`): one draw per sixteen thinks, none while a kind-3 object's
  animation is still running). npc-scripting.md has both. The walk steps
  beside them draw nothing: `StepWalkStep` (`0x800677D4`) and
  `StepCommandMoveBy` (`0x8006382C`) (an object's walk-to and walk-by), the box test
  `MapBoxTouchGet` (`0x80067BFC`) and the mover's arrival test
  `MoveChrGoalCheck` (`0x8005FDAC`);
* the map objects' ways of meeting the party, in map_look.c:
  `rng_range_draw` (`0x800780DC`), a value from min to max that draws only
  when the row gives no fixed value, is called by `MapObjectRandomWalkStart`
  (`0x80078B88`), `MapObjectFacingTurnStep` (`0x80078D5C`), the party-look
  steps `MapObjectPartyLook02Step` (`0x80078E9C`),
  `MapObjectPartyLook04Step` (`0x800791C0`), `MapObjectPartyLook06Step`
  (`0x80079488`), `MapObjectPartyLook07Step` (`0x800795A0`) and
  `MapObjectPartyLook0BStep` (`0x800797E8`), and the party-seen steps
  `MapObjectPartySeen01Step` (`0x800799C4`), `MapObjectPartySeen03Step`
  (`0x80079CAC`), `MapObjectPartySeen04Step` (`0x80079F2C`),
  `MapObjectPartySeen05Step` (`0x8007A0B4`) and `MapObjectPartySeen06Step`
  (`0x8007A1E0`); `MapObjectRandomSpotPlace` (`0x8007816C`),
  `MapObjectRandomWalkStart`, `MapObjectPartyLook02Step` and
  `MapObjectPartyLook0AStep` (`0x800796F0`) also draw directly;
* the view shake, `MapViewScrollSet` (`0x80068E98`): on every other step
  of the shake, two draws for the motors when the vibration option is on,
  then two coin draws for the signs of the x and y offsets;
* the event-script commands `ScriptCommandLabelPick` (`0x8005C470`) (one
  draw, which picks among the labels the command lists) and
  `ScriptCommandMapPadDirectionsChange` (`0x8005EB98`) (one draw, a row 1
  to 4 of the field's D-pad map).
