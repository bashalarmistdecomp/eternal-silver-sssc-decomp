/* pad.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef PAD_H
#define PAD_H

#include "layout.h"

/* A pad record: the buttons of one owner's looks at the pad, which
   pad_record_init makes ready and pad_record_update brings up to date at each
   look. Four live: g_BtlPadRecord, g_BtlMenuPadRecord, g_MenuPadRecord,
   g_MessagePadRecord. The glossary's pad record. */
typedef struct {
    /* +0x00 the buttons held now (pad_buttons_get at the last update;
       pad_record_init sets its held argument). */
    u16 held;
    /* +0x02 the buttons that went down at the last update.
       g_MenuPadRecordPressed (0x800E47DA) names g_MenuPadRecord's. */
    u16 pressed;
    /* +0x04 the buttons that went down or repeat: a button is set when it goes
       down and again each time its timer runs out while it stays held.
       g_MenuPadRecordRepeated (0x800E47DC) names g_MenuPadRecord's. */
    u16 repeated;
    /* +0x06 the buttons held at the update before. */
    u16 held_before;
    /* +0x08 held for the second word of buttons, g_PadSecondHeldButtons. */
    u16 held_2;
    /* +0x0A pressed for the second word. */
    u16 pressed_2;
    /* +0x0C repeated for the second word. */
    u16 repeated_2;
    /* +0x0E held_before for the second word. */
    u16 held_before_2;
    /* +0x10..+0x1F one timer a button bit: the updates left before the bit
       repeats (delay when it goes down, delay / 2 after a repeat, 0 while it
       is up; pad_record_init sets delay / 2 for the held bits). */
    u8 timers[16];
    /* +0x20..+0x2F the timers of the second word's bits. */
    u8 timers_2[16];
} PadRecord;

LAYOUT_SIZE(PadRecord, 0x30);
LAYOUT_AT(PadRecord, held, held, 0x00);
LAYOUT_AT(PadRecord, pressed, pressed, 0x02);
LAYOUT_AT(PadRecord, repeated, repeated, 0x04);
LAYOUT_AT(PadRecord, held_before, held_before, 0x06);
LAYOUT_AT(PadRecord, held_2, held_2, 0x08);
LAYOUT_AT(PadRecord, pressed_2, pressed_2, 0x0a);
LAYOUT_AT(PadRecord, repeated_2, repeated_2, 0x0c);
LAYOUT_AT(PadRecord, held_before_2, held_before_2, 0x0e);
LAYOUT_AT(PadRecord, timers, timers, 0x10);
LAYOUT_AT(PadRecord, timers_2, timers_2, 0x20);

#endif
