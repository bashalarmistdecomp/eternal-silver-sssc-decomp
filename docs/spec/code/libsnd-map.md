# libsnd and libspu on the music path (SLUS_006.28)

This page maps the functions of Sony's sound libraries that the game's music
and sound-effect path reaches, from the game's `sound.c` layer down to the SPU
registers: each function's name, address, file, size and member, and what it
does; then the call graph from boot, bank load, play and the per-tick service;
and last, what the game's SEQ data actually uses. The sound system as a whole
is in [sound.md](../sound.md). libsnd's members are described in
[libetc-libsnd.md](subsystems/psyq/libetc-libsnd.md), libspu's hardware layer
and allocator in [libspu.md](subsystems/psyq/libspu.md), and how Sony's
objects were compiled in [codegen.md](subsystems/psyq/codegen.md).

## The functions

Every function below is Sony's PSY-Q 4.5 code: its member lies whole in the
image, and the register's row gives the member and the offset in it
([naming.md](../../naming.md), "How the SDK names were proven"). A few small
members fit at more than one address on their bytes alone (`de_15.o`,
`de_17.o`, `de_18.o`, `de_19.o`, `play.o`, `ut_roff.o`, `ut_ron.o`,
`vm_doff.o`, `s_i.o`); the register places each by a call or a reference from
a member that is already proven, which lands on it. A static with no name in
the register keeps its address, and the role column says what it does.

The functions are listed in address order. Bytes is the size of the function's code, without the zero words that close its member (`INCLUDE_PAD` in the C). Other functions of the same
members that the path does not reach (`SetVideoMode`, `SsSetLoop`,
`SsSepSetVol` and more) lie between the rows. libetc's interrupt
dispatcher `func_80094E44` (`intr.o` `+0x26C`) is on the path too and lies
before the table's first row. `MemCardInit` (`0x800A1068`, libmcrd `init.o`),
right after `SpuSetReverbModeDepth`, and `InitCARD` (`0x800A10B8`, libcard
`init.o`) are not sound code: `card_init` (`0x80024E3C`) reaches them.

| Name | Address | File | Bytes | Member | Role |
|---|---|---|---|---|---|
| `func_80095274` | `0x80095274` | `libetc_intr_2.c` | 36 | libetc `intr.o` `+0x69C`, static | Clears a block of words; libetc's interrupt setup `func_80094D6C` (`intr.o` `+0x194`) clears its state with it. |
| `_96_remove` | `0x800952A0` | `libapi_c114.c` | 12 | libapi `c114.o` `+0x8` | BIOS call stub. |
| `ReturnFromException` | `0x800952B8` | `libapi_a23.c` | 12 | libapi `a23.o` | BIOS call stub. |
| `HookEntryInt` | `0x800952D8` | `libapi_a25.c` | 12 | libapi `a25.o` | BIOS call stub. |
| `setjmp` | `0x800952E8` | `libc2_setjmp.c` | 60 | libc2 `setjmp.o` | The C library's `setjmp`. The interrupt setup `func_80094D6C` saves with it the context every interrupt resumes in, which runs the dispatcher `func_80094E44`. |
| `startIntrVSync` | `0x80095368` | `libetc_intr_vb_1.c` | 88 | libetc `intr_vb.o` | Sets up the vertical-blank handler `func_800953C0` and clears its eight callback slots. |
| `func_800953C0` | `0x800953C0` | `libetc_intr_vb_2.c` | 108 | libetc `intr_vb.o` `+0x58`, static | The vertical-blank handler: counts the field and calls each callback slot that is set. |
| `setIntrVSync` | `0x8009542C` | `libetc_intr_vb_2.c` | 44 | libetc `intr_vb.o` `+0xC4`, static | Installs a vertical-blank callback slot; `VSyncCallback` reaches it. |
| `func_80095458` | `0x80095458` | `libetc_intr_vb_2.c` | 36 | libetc `intr_vb.o` `+0xF0`, static | Clears a block of words (the callback slots). |
| `startIntrDMA` | `0x80095488` | `libetc_intr_dma_1.c` | 76 | libetc `intr_dma.o` | Sets up the DMA handler `func_800954D4` and clears its callback slots. |
| `func_800954D4` | `0x800954D4` | `libetc_intr_dma_2.c` | 384 | libetc `intr_dma.o` `+0x4C`, static | The DMA interrupt handler: acknowledges each finished channel and calls its callback. |
| `func_80095654` | `0x80095654` | `libetc_intr_dma_2.c` | 172 | libetc `intr_dma.o` `+0x1CC`, static | Installs a DMA channel's completion callback; `DMACallback` reaches it. |
| `func_80095700` | `0x80095700` | `libetc_intr_dma_2.c` | 36 | libetc `intr_dma.o` `+0x278`, static | Clears a block of words (the DMA callback slots). |
| `GetVideoMode` | `0x8009573C` | `libetc_vmode.c` | 16 | libetc `vmode.o` `+0x14` | The console's video mode; `SsSetTickMode` reads it. |
| `func_80095758` | `0x80095758` | `libsnd_ssclose_1.c` | 380 | libsnd `ssclose.o` `+0`, static | The close worker of `SsSeqClose`: silences the score, keys its voices off, frees its access number and resets its records. |
| `SsSeqClose` | `0x800958D4` | `libsnd_ssclose_2.c` | 36 | libsnd `ssclose.o` `+0x17C` | Close a score. |
| `SsEnd` | `0x80095928` | `libsnd_ssend.c` | 172 | libsnd `ssend.o` | Removes the sequencer tick. |
| `_SsInit` | `0x800959D8` | `libsnd_ssinit.c` | 160 | libsnd `ssinit.o` | Writes the SPU's common registers, `_SsVmInit(24)`, clears the mark callbacks, VBLANK_MINUS = 60. |
| `SsInit` | `0x80095A78` | `libsnd_ssinit_c.c` | 56 | libsnd `ssinit_c.o` | `ResetCallback`, `SpuInit`, `SpuClearReverbWorkArea(7)`, `_SsInit`. |
| `SsIsEos` | `0x80095AF8` | `libsnd_ssloop.c` | 64 | libsnd `ssloop.o` `+0x40` | Has a score reached its end? |
| `SsSeqOpen` | `0x80095B38` | `libsnd_ssopenq.c` | 652 | libsnd `ssopenq.o` | Opens a SEQ: takes the lowest free of 32 access numbers, `_SsInitSoundSeq`, and fills the whole handler table `SsFCALL` on every call. |
| `_SsContBankChange` | `0x80095DC8` | `libsnd_cc_0.c` | 116 | libsnd `cc_0.o` | CC0, bank change. `_SsSetControlChange` handles controller 0 itself and never calls this slot. |
| `_SsContDataEntry` | `0x80095E48` | `libsnd_cc_6.c` | 892 | libsnd `cc_6.o` | CC6, data entry: a loop's repeat count, RPN changes, or the NRPN tone-attribute handlers. |
| `_SsContMainVol` | `0x800961C8` | `libsnd_cc_7.c` | 204 | libsnd `cc_7.o` | CC7, channel volume: `_SsVmSetVol` on the channel's voices. |
| `_SsContPanpot` | `0x80096298` | `libsnd_cc_10.c` | 204 | libsnd `cc_10.o` | CC10, channel pan: `_SsVmSetVol`. |
| `_SsContExpression` | `0x80096368` | `libsnd_cc_11.c` | 228 | libsnd `cc_11.o` | CC11, expression: `_SsVmSetProgVol`, then `_SsVmSetVol`. |
| `_SsContDamper` | `0x80096458` | `libsnd_cc_64.c` | 164 | libsnd `cc_64.o` | CC64, damper: `_SsVmDamperOff` or `_SsVmDamperOn`. |
| `_SsContExternal` | `0x80096508` | `libsnd_cc_91.c` | 132 | libsnd `cc_91.o` | CC91: `SsUtSetReverbDepth(data, data)`. |
| `_SsContNrpn1` | `0x80096598` | `libsnd_cc_98.c` | 256 | libsnd `cc_98.o` | CC98, NRPN first byte: the attribute index, or the mark callback when the second byte is 40. |
| `_SsContNrpn2` | `0x80096698` | `libsnd_cc_99.c` | 308 | libsnd `cc_99.o` | CC99, NRPN second byte: 20 loop start, 30 loop end, else the tone number. |
| `_SsContRpn1` | `0x800967D8` | `libsnd_cc_100.c` | 108 | libsnd `cc_100.o` | CC100, RPN first byte. |
| `_SsContRpn2` | `0x80096848` | `libsnd_cc_101.c` | 108 | libsnd `cc_101.o` | CC101, RPN second byte. |
| `_SsContResetAll` | `0x800968B8` | `libsnd_cc_121.c` | 184 | libsnd `cc_121.o` | CC121, reset all: `SsUtReverbOff`, `_SsVmDamperOff`, the channel's program, volume and pan. |
| `_SsSetNrpnVabAttr0` | `0x80096978` | `libsnd_de_0.c` | 140 | libsnd `de_0.o` | NRPN tone attribute 0. |
| `_SsSetNrpnVabAttr1` | `0x80096A08` | `libsnd_de_1.c` | 184 | libsnd `de_1.o` | NRPN tone attribute 1. |
| `_SsSetNrpnVabAttr2` | `0x80096AC8` | `libsnd_de_2.c` | 140 | libsnd `de_2.o` | NRPN tone attribute 2. |
| `_SsSetNrpnVabAttr3` | `0x80096B58` | `libsnd_de_3.c` | 140 | libsnd `de_3.o` | NRPN tone attribute 3. |
| `_SsSetNrpnVabAttr4` | `0x80096BE8` | `libsnd_de_4.c` | 176 | libsnd `de_4.o` | NRPN tone attribute 4. |
| `_SsUtResolveADSR` | `0x80096C98` | `libsnd_ccadsr.c` | 92 | libsnd `ccadsr.o` | Splits a tone's two ADSR words into nine fields. |
| `_SsUtBuildADSR` | `0x80096CF4` | `libsnd_ccadsr.c` | 164 | libsnd `ccadsr.o` `+0x5C` | Packs the nine fields back into the two ADSR words. |
| `_SsSetNrpnVabAttr5` | `0x80096D98` | `libsnd_de_5.c` | 180 | libsnd `de_5.o` | NRPN tone attribute 5. |
| `_SsSetNrpnVabAttr6` | `0x80096E58` | `libsnd_de_6.c` | 172 | libsnd `de_6.o` | NRPN tone attribute 6. |
| `_SsSetNrpnVabAttr7` | `0x80096F08` | `libsnd_de_7.c` | 172 | libsnd `de_7.o` | NRPN tone attribute 7. |
| `_SsSetNrpnVabAttr8` | `0x80096FB8` | `libsnd_de_8.c` | 176 | libsnd `de_8.o` | NRPN tone attribute 8. |
| `_SsSetNrpnVabAttr9` | `0x80097068` | `libsnd_de_9.c` | 180 | libsnd `de_9.o` | NRPN tone attribute 9. |
| `_SsSetNrpnVabAttr10` | `0x80097128` | `libsnd_de_10.c` | 176 | libsnd `de_10.o` | NRPN tone attribute 10. |
| `_SsSetNrpnVabAttr11` | `0x800971D8` | `libsnd_de_11.c` | 164 | libsnd `de_11.o` | NRPN tone attribute 11. |
| `_SsSetNrpnVabAttr12` | `0x80097288` | `libsnd_de_12.c` | 216 | libsnd `de_12.o` | NRPN tone attribute 12. |
| `_SsSetNrpnVabAttr13` | `0x80097368` | `libsnd_de_13.c` | 156 | libsnd `de_13.o` | NRPN tone attribute 13. |
| `_SsSetNrpnVabAttr14` | `0x80097408` | `libsnd_de_14.c` | 156 | libsnd `de_14.o` | NRPN tone attribute 14. |
| `_SsSetNrpnVabAttr15` | `0x800974A8` | `libsnd_de_15.c` | 36 | libsnd `de_15.o` | NRPN attribute 15: `SsUtSetReverbType`. |
| `_SsSetNrpnVabAttr16` | `0x800974D8` | `libsnd_de_16.c` | 40 | libsnd `de_16.o` | NRPN attribute 16: `SsUtSetReverbDepth`. |
| `_SsSetNrpnVabAttr17` | `0x80097508` | `libsnd_de_17.c` | 36 | libsnd `de_17.o` | NRPN attribute 17: `SsUtSetReverbFeedback`. |
| `_SsSetNrpnVabAttr18` | `0x80097538` | `libsnd_de_18.c` | 36 | libsnd `de_18.o` | NRPN attribute 18: `SsUtSetReverbDelay`. |
| `_SsSetNrpnVabAttr19` | `0x80097568` | `libsnd_de_19.c` | 36 | libsnd `de_19.o` | NRPN attribute 19: `SsUtSetReverbDelay`. |
| `_SsSetPitchBend` | `0x80097598` | `libsnd_midibend.c` | 176 | libsnd `midibend.o` | Pitch-bend event: `_SsVmPitchBend`. |
| `_SsSetControlChange` | `0x80097648` | `libsnd_midicc.c` | 556 | libsnd `midicc.o` | Control-change event: dispatches through `SsFCALL.control[]`. |
| `_SsGetMetaEvent` | `0x80097878` | `libsnd_midimeta.c` | 456 | libsnd `midimeta.o` | The tempo meta event (FF 51): tempo = 60000000 / microseconds per quarter note, and the play step re-derived. End of track (0x2F) never reaches it. |
| `_SsNoteOn` | `0x80097A48` | `libsnd_midinote.c` | 212 | libsnd `midinote.o` | Note event: `_SsVmKeyOn` with the channel's program and pan; velocity 0 is `_SsVmKeyOff`. |
| `_SsSetProgramChange` | `0x80097B28` | `libsnd_midiprog.c` | 108 | libsnd `midiprog.o` | Program change: the channel's program. |
| `_SsReadDeltaValue` | `0x80097B98` | `libsnd_miditime.c` | 164 | libsnd `miditime.o` | Reads a SEQ delta time (a variable-length number) and returns it times 10. |
| `_SsInitSoundSeq` | `0x80097C48` | `libsnd_seqinit.c` | 836 | libsnd `seqinit.o` | `SsSeqOpen`'s worker: parses the SEQ header and derives the play step from the tempo and VBLANK_MINUS. |
| `SsSeqPlay` | `0x80097F98` | `libsnd_ssplay.c` | 56 | libsnd `ssplay.o` | `Snd_SetPlayMode(seq, 0, mode, l_count)`. |
| `Snd_SetPlayMode` | `0x80098018` | `libsnd_playmode.c` | 280 | libsnd `playmode.o` | Rewinds the score; mode 1 plays it and re-applies its volume, mode 0 asks for a pause. |
| `func_80098138` | `0x80098138` | `libsnd_ssstart_1.c` | 560 | libsnd `ssstart.o` `+0`, static | Installs the sequencer tick; with tick mode 5 and argument 0, `VSyncCallback(tick_cb)`. |
| `SsStart2` | `0x80098388` | `libsnd_ssstart_2.c` | 32 | libsnd `ssstart.o` `+0x250` | `func_80098138(0)`. |
| `func_800983A8` | `0x800983A8` | `libsnd_ssstart_3.c` | 76 | libsnd `ssstart.o` `+0x270`, static | The handler `func_80098138` puts on interrupt 0 in tick mode 5 when the tick does not ride `VSyncCallback`: the VSync handler it displaced, then the tick. |
| `func_800983F4` | `0x800983F4` | `libsnd_ssstart_4.c` | 72 | libsnd `ssstart.o` `+0x2BC`, static | The tick handler below 70 Hz: runs the tick every second call. |
| `SsSeqCalledTbyT` | `0x80098448` | `libsnd_sscall.c` | 624 | libsnd `sscall.o` | The sequencer tick: `_SsVmFlush` first, then per open score the play step and the crescendo, tempo, pause, replay and stop flags. |
| `_SsSndCrescendo` | `0x800986B8` | `libsnd_cres.c` | 520 | libsnd `cres.o` | One tick of a volume ramp (flags 0x10 / 0x20). |
| `_SsSndPause` | `0x800988C8` | `libsnd_pause.c` | 152 | libsnd `pause.o` | Pause: keys the score's voices off. |
| `_SsSndPlay` | `0x80098968` | `libsnd_play.c` | 44 | libsnd `play.o` | The tick's step for a playing score: `_SsSeqPlay`. |
| `_SsSeqPlay` | `0x80098998` | `libsnd_midiread.c` | 252 | libsnd `midiread.o` | Counts the delta time down and reads the events that are due. |
| `_SsSeqGetEof` | `0x80098A94` | `libsnd_midiread.c` | 580 | libsnd `midiread.o` `+0xFC` | End of track: repeat, chain the next score, or finish. |
| `_SsGetSeqData` | `0x80098CD8` | `libsnd_midiread.c` | 940 | libsnd `midiread.o` `+0x340` | Reads one SEQ event, running status included, and calls its handler. |
| `_SsSndNextSep` | `0x80099088` | `libsnd_next.c` | 252 | libsnd `next.o` | Starts a chained score. |
| `_SsSndReplay` | `0x80099188` | `libsnd_replay.c` | 96 | libsnd `replay.o` | Resumes a paused score. |
| `_SsSndStop` | `0x800991E8` | `libsnd_ssstop_1.c` | 384 | libsnd `ssstop.o` | Stops and rewinds a score. |
| `SsSeqStop` | `0x80099368` | `libsnd_ssstop_2.c` | 40 | libsnd `ssstop.o` `+0x180` | Stop a score. |
| `SsSetTableSize` | `0x800993C8` | `libsnd_sstable.c` | 536 | libsnd `sstable.o` | The score table (the game: 16 scores of 1 track). |
| `SsSetTickMode` | `0x800995E8` | `libsnd_sstick.c` | 328 | libsnd `sstick.o` | Tick mode (the game: 1, SS_TICK60, which is SS_TICKVSYNC on NTSC with VBLANK_MINUS 60). |
| `SsSeqSetVol` | `0x800997C8` | `libsnd_ssvol.c` | 104 | libsnd `ssvol.o` `+0x90` | Score volume: stored, or, when the score's flags are exactly 1 (playing), applied to its voices by `_SsVmSetSeqVol` (the game: 0x50, the music scaled by the option). |
| `_SsSndTempo` | `0x800998F8` | `libsnd_tempo.c` | 540 | libsnd `tempo.o` | One tick of a tempo ramp (flags 0x40 / 0x80). |
| `SsUtGetProgAtr` | `0x80099B18` | `libsnd_ut_gpa.c` | 260 | libsnd `ut_gpa.o` | Reads a program's attributes. |
| `SsUtGetVagAtr` | `0x80099C28` | `libsnd_ut_gva.c` | 568 | libsnd `ut_gva.o` | Reads a tone's attributes. |
| `SsUtSetReverbDelay` | `0x80099E68` | `libsnd_ut_rdel.c` | 60 | libsnd `ut_rdel.o` | Reverb delay. |
| `SsUtSetReverbDepth` | `0x80099EA8` | `libsnd_ut_rdep.c` | 140 | libsnd `ut_rdep.o` | Reverb depth (callers: CC91 and NRPN attribute 16). |
| `SsUtSetReverbType` | `0x80099F38` | `libsnd_ut_rev.c` | 156 | libsnd `ut_rev.o` | Reverb type (caller: NRPN attribute 15). |
| `SsUtSetReverbFeedback` | `0x80099FE8` | `libsnd_ut_rfb.c` | 60 | libsnd `ut_rfb.o` | Reverb feedback. |
| `SsUtReverbOff` | `0x8009A028` | `libsnd_ut_roff.c` | 32 | libsnd `ut_roff.o` | `SpuSetReverb(0)`. |
| `SsUtReverbOn` | `0x8009A048` | `libsnd_ut_ron.c` | 32 | libsnd `ut_ron.o` | `SpuSetReverb(1)`. |
| `SsUtSetVagAtr` | `0x8009A068` | `libsnd_ut_sva.c` | 452 | libsnd `ut_sva.o` | Writes a tone's attributes. |
| `_SsVmDamperOff` | `0x8009A238` | `libsnd_vm_doff.c` | 12 | libsnd `vm_doff.o` | Damper off. |
| `_SsVmDamperOn` | `0x8009A248` | `libsnd_vm_don.c` | 16 | libsnd `vm_don.o` | Damper on. |
| `_SsVmFlush` | `0x8009A258` | `libsnd_vm_f.c` | 1220 | libsnd `vm_f.o` | Sends the voice manager's pending state to the SPU: envelope read-back and retiring silent voices, dirty shadow registers through `SpuSetVoiceAttr`, key off then key on, reverb and noise bits. |
| `_SsVmInit` | `0x8009A728` | `libsnd_vm_init.c` | 848 | libsnd `vm_init.o` | Resets the voice manager for 24 voices; `SpuInitMalloc(32, …)`. |
| `_SsVmKeyOn` | `0x8009AA78` | `libsnd_vm_key_1.c` | 1308 | libsnd `vm_key.o` | Key on: every tone of the program that covers the note gets a voice (`_SsVmAlloc`, `_SsVmDoAllocate`) and starts (`_SsVmKeyOnNow`, or `vmNoiseOn` for the noise tone). |
| `_SsVmKeyOff` | `0x8009AF94` | `libsnd_vm_key_1.c` | 352 | libsnd `vm_key.o` `+0x51C` | Keys off every voice playing this note of this program, VAB and score. |
| `_SsVmAlloc` | `0x8009B228` | `libsnd_vm_aloc1.c` | 624 | libsnd `vm_aloc1.o` | Picks a voice: an idle one, else the lowest priority no higher than the note's, ties to the lowest envelope level, then the oldest. |
| `_SsVmDoAllocate` | `0x8009B498` | `libsnd_vm_aloc2.c` | 456 | libsnd `vm_aloc2.o` | Fills the new voice's shadow registers: sample address, ADSR1, ADSR2 with the damper's release. |
| `note2pitch` | `0x8009B668` | `libsnd_vm_n2p.c` | 76 | libsnd `vm_n2p.o` | Note to SPU pitch, by `SsPitchFromNote`. |
| `note2pitch2` | `0x8009B6B4` | `libsnd_vm_n2p.c` | 104 | libsnd `vm_n2p.o` `+0x4C` | Bent note to SPU pitch. |
| `SsPitchFromNote` | `0x8009B71C` | `libsnd_vm_n2p.c` | 288 | libsnd `vm_n2p.o` `+0xB4` | The pitch arithmetic. |
| `vmNoiseOn` | `0x8009B848` | `libsnd_vm_no1.c` | 1384 | libsnd `vm_no1.o` | Key on of the noise tone (VAG number 0xFF). |
| `vmNoiseOff` | `0x8009BDB8` | `libsnd_vm_noff.c` | 56 | libsnd `vm_noff.o` | Noise off. |
| `_SsVmKeyOffNow` | `0x8009BDF8` | `libsnd_vm_nowof.c` | 192 | libsnd `vm_nowof.o` | Keys the current voice off at once. |
| `_SsVmKeyOnNow` | `0x8009BEB8` | `libsnd_vm_nowon.c` | 1224 | libsnd `vm_nowon.o` | The note-on volume and pan: note × VAB master × program master × tone volume, split by the score's L/R volumes, panned by tone, program and note (/ 63), equal in mono, squared / 0x3FFF; sets the key-on and reverb bits. |
| `_SsVmPBVoice` | `0x8009C388` | `libsnd_vm_pb.c` | 492 | libsnd `vm_pb.o` | Pitch bend of one voice, by the tone's bend range: `note2pitch2`. |
| `_SsVmPitchBend` | `0x8009C574` | `libsnd_vm_pb.c` | 220 | libsnd `vm_pb.o` `+0x1EC` | Pitch bend of every voice that plays this program of this VAB for the score. |
| `_SsVmSetProgVol` | `0x8009C658` | `libsnd_vm_prog.c` | 112 | libsnd `vm_prog.o` | Program volume (the CC11 path). |
| `_SsVmSetSeqVol` | `0x8009C7D8` | `libsnd_vm_seq.c` | 1388 | libsnd `vm_seq.o` | Score volume: recomputes every voice of the score by the same law, squared. |
| `_SsVmGetSeqVol` | `0x8009CD44` | `libsnd_vm_seq.c` | 100 | libsnd `vm_seq.o` `+0x56C` | Reads a score's L/R volume. |
| `_SsVmSeqKeyOff` | `0x8009CE38` | `libsnd_vm_seq.c` | 176 | libsnd `vm_seq.o` `+0x660` | Keys off every voice of a score. |
| `_SsVmSelectToneAndVag` | `0x8009CEE8` | `libsnd_vm_stav.c` | 196 | libsnd `vm_stav.o` | Lists every tone of the program whose key range holds the note. |
| `_SsVmSetVol` | `0x8009CFB8` | `libsnd_vm_vol.c` | 1440 | libsnd `vm_vol.o` | Channel volume or pan changed: recomputes the channel's voices by the same law, squared. |
| `_SsVmVSetUp` | `0x8009D558` | `libsnd_vm_vsu.c` | 192 | libsnd `vm_vsu.o` | Makes a VAB's tables current. |
| `SsSetMono` | `0x8009D618` | `libsnd_vs_mono.c` | 16 | libsnd `vs_mono.o` | Mono: the volume code gives both channels the louder. |
| `SsSetStereo` | `0x8009D628` | `libsnd_vs_mono.c` | 12 | libsnd `vs_mono.o` `+0x10` | Stereo. |
| `SsVabClose` | `0x8009D638` | `libsnd_vs_vab.c` | 164 | libsnd `vs_vab.o` | Close a VAB. |
| `SsVabOpenHead` | `0x8009D6E8` | `libsnd_vs_vh2.c` | 52 | libsnd `vs_vh2.o` | Open a VAB header, its sample memory allocated by `func_8009D71C`. |
| `func_8009D71C` | `0x8009D71C` | `libsnd_vs_vh2.c` | 104 | libsnd `vs_vh2.o` `+0x34`, static | `SsVabOpenHead`'s allocator: `SpuMalloc`; on failure frees the slot and returns -1. |
| `_SsVabOpenHeadWithMode` | `0x8009D7F8` | `libsnd_vs_vh.c` | 1004 | libsnd `vs_vh.o` `+0x70` | Parses the VAB header and reserves its SPU memory through the allocator it is given. |
| `SsVabTransBody` | `0x8009DBE8` | `libsnd_vs_vtb.c` | 192 | libsnd `vs_vtb.o` | Sends the VAB body to SPU RAM: `SpuSetTransferMode`, `SpuSetTransferStartAddr`, `SpuWrite`. |
| `SsVabTransCompleted` | `0x8009DCA8` | `libsnd_vs_vtc.c` | 40 | libsnd `vs_vtc.o` | Has the body's transfer finished? |
| `SpuInit` | `0x8009DCD8` | `libspu_s_i.c` | 32 | libspu `s_i.o` | `_SpuInit(0)`. |
| `_SpuInit` | `0x8009DCF8` | `libspu_s_ini_1.c` | 232 | libspu `s_ini.o` | SPU reset and libspu state ([libspu.md](subsystems/psyq/libspu.md)). |
| `SpuStart` | `0x8009DDE0` | `libspu_s_ini_2.c` | 112 | libspu `s_ini.o` `+0xE8` | The DMA callback and the transfer event. |
| `OpenEvent` | `0x8009DE58` | `libapi_a08.c` | 12 | libapi `a08.o` | BIOS call stub. |
| `EnableEvent` | `0x8009DE68` | `libapi_a12.c` | 12 | libapi `a12.o` | BIOS call stub. |
| `_spu_init` | `0x8009DE78` | `libspu_spu_1.c` | 640 | libspu `spu.o` | Resets the SPU. |
| `func_8009E0F8` | `0x8009E0F8` | `libspu_spu_1.c` | 448 | libspu `spu.o` `+0x280`, static | Writes to SPU RAM through the data port. |
| `_spu_FiDMA` | `0x8009E2B8` | `libspu_spu_2.c` | 188 | libspu `spu.o` `+0x440` | The SPU's DMA-end handler. |
| `_spu_t` | `0x8009E41C` | `libspu_spu_3.c` | 640 | libspu `spu.o` `+0x5A4` | One step of an SPU-RAM DMA transfer. |
| `_spu_Fw` | `0x8009E69C` | `libspu_spu_3.c` | 132 | libspu `spu.o` `+0x824` | Write to SPU RAM. |
| `_spu_FsetRXX` | `0x8009E784` | `libspu_spu_3.c` | 68 | libspu `spu.o` `+0x90C` | Writes one SPU register. |
| `_spu_FsetRXXa` | `0x8009E7C8` | `libspu_spu_3.c` | 164 | libspu `spu.o` `+0x950` | Writes a byte address to an SPU address register, in SPU units. |
| `func_8009E900` | `0x8009E900` | `libspu_spu_3.c` | 40 | libspu `spu.o` `+0xA88`, static | Bus timing for a write. |
| `func_8009E928` | `0x8009E928` | `libspu_spu_3.c` | 40 | libspu `spu.o` `+0xAB0`, static | Bus timing for a read. |
| `_spu_Fw1ts` | `0x8009E950` | `libspu_spu_3.c` | 92 | libspu `spu.o` `+0xAD8` | A short busy wait. |
| `_SpuDataCallback` | `0x8009E9B8` | `libspu_s_dcb.c` | 36 | libspu `s_dcb.o` | `DMACallback(4, func)`. |
| `SpuInitMalloc` | `0x8009E9E8` | `libspu_s_m_init.c` | 84 | libspu `s_m_init.o` | Starts the SPU-RAM allocation table. |
| `SpuMalloc` | `0x8009EA48` | `libspu_s_m_m.c` | 708 | libspu `s_m_m.o` | Allocates SPU RAM. |
| `_spu_gcSPU` | `0x8009ED18` | `libspu_s_m_int.c` | 768 | libspu `s_m_int.o` | Merges neighbouring free blocks, sorts the records by address, and folds free blocks before the tail into it. |
| `SpuFree` | `0x8009F018` | `libspu_s_m_f.c` | 124 | libspu `s_m_f.o` | Frees SPU RAM. |
| `SpuSetNoiseVoice` | `0x8009F098` | `libspu_s_snv.c` | 36 | libspu `s_snv.o` | Noise bits. |
| `_SpuSetAnyVoice` | `0x8009F0C8` | `libspu_s_sav.c` | 700 | libspu `s_sav.o` | Writes a voice bit mask to a register pair (reverb, noise). |
| `SpuGetNoiseVoice` | `0x8009F388` | `libspu_s_gnv.c` | 36 | libspu `s_gnv.o` | Reads the noise bits. |
| `_SpuGetAnyVoice` | `0x8009F3B8` | `libspu_s_gav.c` | 48 | libspu `s_gav.o` | Reads a register pair's voice bits. |
| `SpuSetNoiseClock` | `0x8009F3E8` | `libspu_s_snc.c` | 72 | libspu `s_snc.o` | Noise clock. |
| `SpuSetReverb` | `0x8009F438` | `libspu_s_sr.c` | 204 | libspu `s_sr.o` | Reverb on or off. |
| `_SpuIsInAllocateArea_` | `0x8009F588` | `libspu_s_m_util.c` | 140 | libspu `s_m_util.o` `+0x80` | Does an SPU address lie in allocated memory? |
| `SpuSetReverbModeParam` | `0x8009F618` | `libspu_s_srmp.c` | 1236 | libspu `s_srmp.o` | Reverb mode, depth, delay and feedback (the `SsUt*` path). |
| `_spu_setReverbAttr` | `0x8009FAF8` | `libspu_s_sra.c` | 1232 | libspu `s_sra.o` | Writes the reverb registers the entry selects (0: all 32). |
| `SpuSetReverbVoice` | `0x8009FFC8` | `libspu_s_srv.c` | 36 | libspu `s_srv.o` | Reverb bits. |
| `SpuGetReverbVoice` | `0x8009FFF8` | `libspu_s_grv.c` | 36 | libspu `s_grv.o` | Reads the reverb bits. |
| `SpuClearReverbWorkArea` | `0x800A0028` | `libspu_s_crwa.c` | 404 | libspu `s_crwa.o` | Zeroes a reverb mode's work area. |
| `WaitEvent` | `0x800A01C8` | `libapi_a10.c` | 12 | libapi `a10.o` | BIOS call stub. |
| `SpuSetKey` | `0x800A01D8` | `libspu_s_sk.c` | 444 | libspu `s_sk.o` | Key on and key off. |
| `SpuWrite` | `0x800A0398` | `libspu_s_r.c` | 92 | libspu `s_w.o` | Write to SPU RAM at the transfer address, at most 0x7EFF0 bytes. |
| `SpuSetTransferStartAddr` | `0x800A03F8` | `libspu_s_stsa.c` | 92 | libspu `s_stsa.o` | The transfer's start address. |
| `SpuSetTransferMode` | `0x800A0458` | `libspu_s_stm.c` | 48 | libspu `s_stm.o` | Transfer by DMA or by I/O. |
| `SpuIsTransferCompleted` | `0x800A0488` | `libspu_s_itc.c` | 164 | libspu `s_itc.o` | Has the transfer finished? |
| `TestEvent` | `0x800A0538` | `libapi_a11.c` | 12 | libapi `a11.o` | BIOS call stub. |
| `_spu_setInTransfer` | `0x800A0548` | `libspu_s_it.c` | 40 | libspu `s_it.o` | Sets the in-transfer flag. |
| `_spu_getInTransfer` | `0x800A0570` | `libspu_s_it.c` | 24 | libspu `s_it.o` `+0x28` | Reads the in-transfer flag. |
| `SpuSetVoiceAttr` | `0x800A0588` | `libspu_s_sva.c` | 1528 | libspu `s_sva.o` | Writes one voice's registers: volume, pitch, start and loop address, ADSR. |
| `_spu_note2pitch` | `0x800A0B88` | `libspu_s_n2p.c` | 208 | libspu `s_n2p.o` | Note to pitch for `SpuSetVoiceAttr`. |
| `SpuGetVoiceEnvelope` | `0x800A0D88` | `libspu_s_gvex.c` | 28 | libspu `s_gvex.o` | Reads a voice's envelope level, which `_SsVmFlush` uses to retire silent voices. |
| `SpuSetCommonMasterVolume` | `0x800A0DA8` | `libspu_s_scmv.c` | 28 | libspu `s_scmv.o` | Master volume (the game: 0x3F80 in `sound_init`, 0x3FFF in `cd_track_play` (`0x8001B684`)). |
| `SpuSetCommonCDVolume` | `0x800A0DC8` | `libspu_s_sccv.c` | 24 | libspu `s_sccv.o` | CD volume. |
| `SpuSetCommonCDReverb` | `0x800A0DE8` | `libspu_s_sccr.c` | 64 | libspu `s_sccr.o` | CD reverb. |
| `SpuSetCommonCDMix` | `0x800A0E28` | `libspu_s_sccm.c` | 64 | libspu `s_sccm.o` | CD mix. |
| `SpuSetReverbModeType` | `0x800A0E68` | `libspu_s_srmt.c` | 460 | libspu `s_srmt.o` | Reverb preset (the game: 4, STUDIO_C, with 0x100, clear the work area). |
| `SpuSetReverbModeDepth` | `0x800A1038` | `libspu_s_srmd.c` | 40 | libspu `s_srmd.o` | Reverb depth (the game ends on 0x2800 / 0x2800). |

## The call graph

From the game's `sound.c` layer down to the SPU registers. Game functions are
in [sound.md](../sound.md); the Sony functions' addresses are in the table
above.

### Boot

`main` (`0x80013D2C`) calls `system_init` (`0x8001EEB8`), which runs, in this
order: `ResetCallback`, `CdInit`, `SsInit`, `ResetGraph(0)`, `card_init`,
`card_port_set(0)`, `pad_start`, the file, memory and graphics setup,
`rng_seed`, then `sound_init`, and after it `xa_file_set` and the rest of the
start-up.

* `SsInit`: `ResetCallback`; `SpuInit` → `_SpuInit(0)`;
  `SpuClearReverbWorkArea(7)`; `_SsInit` → `_SsVmInit(24)`, VBLANK_MINUS = 60.
* `card_init` (`0x80024E3C`): `MemCardInit(0)`, `MemCardStart`, `_bu_init`.
  `MemCardInit` calls `InitCARD`, `StartCARD` and `_bu_init`. None of this is
  sound code.
* `sound_init` (`0x80023D54`):
  1. `SsSetTableSize(g_SoundScoreTable, 16, 1)`; `SsSetTickMode(1)`.
  2. `g_SoundBankCount` = 0, `g_SoundBufNext` = 0x801EB300, and the three
     12-byte bank records at `g_SoundBanks` emptied.
  3. `sound_seq_init` (`0x80023978`).
  4. `sound_reverb_set(4, 0x30)` (`0x800241F4`): `SpuSetReverbModeType(4 |
     0x100)`, `SpuSetReverbModeDepth(0x3000, 0xB000)`, `SpuSetReverb(1)`.
  5. `SsStart2` → `func_80098138(0)` → `VSyncCallback(tick_cb)`. The tick
     environment's callback is `SsSeqCalledTbyT` in the image's data.
  6. `SpuSetReverbModeDepth(0x2800, 0x2800)`;
     `SpuSetCommonMasterVolume(0x3F80, 0x3F80)`.

### Bank load (the sound-bank command list)

* `sound_vab_head_open` (`0x80024314`) → `SsVabOpenHead` →
  `_SsVabOpenHeadWithMode` with the allocator `func_8009D71C` (`SpuMalloc`).
* `sound_vab_body_transfer` (`0x800243D0`) → `SsVabTransBody`
  (`SpuSetTransferMode`, `SpuSetTransferStartAddr`, `SpuWrite`), and
  `SsVabTransCompleted`.
* `sound_seq_add` (`0x80024464`) adds a SEQ to the sound buffer.

### Play

`sound_seq_play(bank, k, slot)` (`0x80023B24`) has two callers:

* `sound_music_play` (`0x80023D0C`), which is `sound_seq_play(2, 0, 0)`;
  its callers are `MusicLoadStart` (`0x80055118`) and `MusicStart`
  (`0x800551C8`). `sound_music_load` (`0x80023E9C`) does not play: it calls
  `sound_music_index_get` (`0x80023E20`), `sound_bank_close_from(2)`
  (`0x800240EC`) and `sound_bank_load` (`0x80024138`).
* `sound_effect_play(ch, kind, n)` (`0x80024514`), from `SoundPlay`
  (`0x800555F0`), with `slot` = `ch`. Kind 1 plays from bank 0 with `n` below
  0x16, kind 2 from bank 1 with `n` below 0x60, kind 3 from bank 1 with `n`
  below 8.

`sound_seq_play` itself does nothing when `bank` is not below
`g_SoundBankCount` or `k` is not below the bank's score count. A negative
`slot` means the slot in `g_SoundSeqNextSlot`. Then:

1. `sound_seq_stop(slot)` (`0x80023A20`): if the slot holds a score,
   `SsSeqStop`, `SsSeqClose`, and the slot is emptied (-1).
2. `SsSeqOpen` (`_SsInitSoundSeq`; it fills `SsFCALL` with every `_SsCont*`,
   `_SsSetNrpnVabAttr*` and event handler).
3. `SsSeqSetVol(seq, 0x50, 0x50)`.
4. `SsSeqPlay(seq, 1, 1)` → `Snd_SetPlayMode`. The slot records the score
   and its bank.
5. Slot 0, the music: `sound_music_level_apply` (`0x800246B0`) →
   `sound_music_volume_set` (`0x80023C4C`): `SsSeqSetVol` on slot 0's
   score with both volumes `v = level * 0x50 / 0xFF`, at most 0x50, which
   reaches `_SsVmSetSeqVol` while the score plays.
6. A negative slot: `sound_seq_slot_step` (`0x800239D0`) moves
   `g_SoundSeqNextSlot` on, through 1 to 15 and round to 1. Only
   `sound_seq_rotate_play` (`0x80023CEC`), which is `sound_seq_play(bank, k,
   -1)`, asks for a negative slot, and nothing calls it.

Elsewhere: `sound_music_stop` (`0x80023D34`) is `sound_seq_stop(0)`;
`sound_music_play_check` (`0x80023CC0`) is `SsIsEos(slot 0's score, 0)`;
`SoundMonoSet` (`0x800550D4`) calls `SsSetMono` or `SsSetStereo` by the
option.

### The tick

On every vertical blank (59.94 Hz on NTSC): libetc's dispatcher
`func_80094E44` → the vertical-blank handler `func_800953C0` → its callback
slot 4, which `VSyncCallback` filled → `SsSeqCalledTbyT`:

1. `_SsVmFlush` first. It sends what the previous tick's events left in the
   voice manager's shadow registers, so a tick's events reach the SPU at the
   next tick's start.
   * `SpuGetVoiceEnvelope`: each voice's envelope level, to retire silent
     voices.
   * `SpuSetVoiceAttr` (→ `_spu_note2pitch`, `_spu_FsetRXXa`) for the dirty
     registers: volume, pitch, sample address, ADSR.
   * `SpuSetKey`: key off, then key on.
   * `SpuSetReverbVoice` and `SpuSetNoiseVoice` (→ `_SpuSetAnyVoice`), keeping
     the bits of voices the manager does not own as `SpuGetReverbVoice` and
     `SpuGetNoiseVoice` report them.
2. For each open score that is playing: `_SsSndPlay` → `_SsSeqPlay` →
   `_SsGetSeqData` (→ `_SsReadDeltaValue`; at the end of a track
   `_SsSeqGetEof` → `_SsSndNextSep`), which calls the handlers in `SsFCALL`:
   * `_SsNoteOn` → `_SsVmKeyOn` → `_SsVmVSetUp`, `_SsVmSelectToneAndVag`,
     `_SsVmAlloc`, `_SsVmDoAllocate`, `note2pitch` → `SsPitchFromNote`, and
     `_SsVmKeyOnNow` or `vmNoiseOn`. Velocity 0: `_SsVmKeyOff` →
     `_SsVmKeyOffNow`.
   * `_SsSetControlChange` → `_SsContMainVol`, `_SsContPanpot`,
     `_SsContExpression` → `_SsVmSetVol` (and `_SsVmSetProgVol` for CC11);
     `_SsContNrpn2` and `_SsContDataEntry` for loops.
   * `_SsSetPitchBend` → `_SsVmPitchBend` → `_SsVmPBVoice` → `note2pitch2` →
     `SsPitchFromNote`.
   * `_SsSetProgramChange`; `_SsGetMetaEvent` (tempo).
3. The flags: `_SsSndCrescendo` (0x10, 0x20), `_SsSndTempo` (0x40, 0x80),
   `_SsSndPause` (2), `_SsSndReplay` (8), `_SsSndStop` (4). Only `_SsSndTempo`
   touches flags 0x40 and 0x80, to clear them; the functions that set them
   (`SsSeqSetAccelerando`, `SsSeqSetRitardando`) are not linked, so the tempo
   ramp never runs in this game.

## What the game's SEQ data uses

Measured over every SEQ score in the game's sound banks (412 scores in 127
banks):

* Note events: 193,552, of which 96,776 are note-ons. Pitch bends: 9,823.
* Control changes: 3,367. CC7 2,028 (in 411 scores), CC10 1,101 (409),
  CC11 34 (2), CC99 136 (the loop markers 20 and 30, in 68 scores), CC6 68
  (each a loop count of 127, an endless loop).
* No CC0, CC64, CC91, CC98, CC100, CC101 or CC121, and no NRPN
  tone-attribute writes. The
  handlers for those are linked but the music never reaches them.
* Tone layering: 2,873 note-ons (in 32 banks) fall in the key range of two
  tones, and `_SsVmKeyOn` sounds both; 482 fall in none and sound nothing.
* The most voices one piece of music holds at once is 22; release tails and
  sound effects add to that.
