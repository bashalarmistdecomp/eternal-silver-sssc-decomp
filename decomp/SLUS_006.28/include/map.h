/* map.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef MAP_H
#define MAP_H

#include "layout.h"
#include "sprite.h"

/* MapRecord: one of the 0x3C-byte animated records outside the battle: the
   party's records on the map and their twins (g_MapPartyRecords,
   g_MapPartyTwins), the map objects' twins (g_MapObjectTwins), the script
   actors' twins (g_MoveChrOwnActorTwins), the shop's kept records and objects
   (g_ShopKeptRecords, g_ShopObjects), the menu's records (g_MenuRecords), the
   message arrow's record (g_MessageArrowRecord) and the menu use's show
   records (g_MenuUseShowRecords); and the first 0x3C bytes of every 0x60-byte
   map object row (g_MapObjects) and script actor row (g_MoveChrOwnActors). Two
   bytes that say what the record is to its user, the sprite entry its stepper
   copies to the sprite list each frame, and the animation's words. */
typedef struct {
    /* +0x00 what the record is, 0 when it is off; a union because the same
       byte is an on byte to the actors' code and a kind to the map objects'
       code. on: every stepper and finder takes the record as off while it is 0
       (MapPartyAnimationStep, MapObjectsAnimationStep, MoveChrAction for the
       twins, ShopAnimationStep, OutlineMapStep in mode 1, MapPartyRingFollow,
       MapPartyTwinSpotSet, MoveChrFollowRestart, MoveChrSpriteDataSet,
       MapObjectFind, MapObjectFreeFind, ShopListSet). A party record holds the
       low byte of its member's id (MapPartyRecordInit, MapPartyRecordSpotSet,
       ScriptCommandChr1ChrBChange, MenuCardListClose, MenuOptionClose,
       MenuChrRecordDisp, MenuChrRecordClose), 1 when the menu puts it back
       (MenuOpen, MenuClose), 0 for an empty or hidden place
       (MapPartyRecordsInit, MoveChrFollowStart, MoveChrFollowRestart,
       MoveChrPartyLeave, ScriptCommandMapPartyKindSet, MenuClose, ShopClose,
       MenuCardListOpen, MenuOptionOpen, MapBtlRun); the twins, the shop's
       objects, the menu use's records and the script actors' records hold 1 or
       0 (MapPartyRecordInit and MenuClose for the twins, ShopOpen, ShopClose,
       ShopBuySellOpen, ShopMemberDisp, ShopMemberStep, ShopItemLineUpSet,
       ShopMemberLineUpSet, ShopItemStep, ShopItemDisp, ShopListSet,
       ShopObjectInit, MenuUseShowPlacesPlay, ScriptCommandMoveChrRowActorSet,
       ScriptCommandMoveChrRowsEnd). kind: MapObjectInit writes (definition +0
       & 0x3F) + 1, MapObjectRestore the map list's byte;
       ScriptCommandMoveChrRowActorSet sets bit 0x80 when a script slot takes
       the object and ScriptCommandMoveChrRowsAllEnd and
       ScriptCommandMoveChrRowsEnd clear it (the latter copies the byte to the
       twin); MoveChrPartyLeave writes 0x81 for a member who stays behind as an
       object; ScriptCommandMapObjectsRemove and MapBtlRun clear it.
       MapObjectsThink (0 and 7 skipped, below 4 thinks one frame in four),
       MapObjectMoveSet (4 to 6), MapObjectTurnStep (3), MapPartyTalkCheck (4
       to 6, 7), MapObjectTouchFind (4 to 6), ScriptCommandMapObjectsRemove
       (below 4) and MapObjectsSpriteDataSet (7: no sprite data) read it as the
       kind, and MapObjectRestore and MapObjectInit pass it to MapRecordInit as
       the kind for the row and its twin. The menu's records and the message
       arrow's record never set it (their clears leave 0). */
    union {
        /* on: used by actors, and, finders, for, kept, menu, objects, only,
           party, records, s, script and others */
        u8 on;
        /* kind: used by MapObjectInit, MapObjectMoveSet, MapObjectOverlapFind,
           MapObjectRestore, MapObjectTouchFind, MapObjectTurnStep,
           MapObjectsAnimationStep, MapObjectsSpriteDataSet, MapObjectsThink,
           MapPartyTalkCheck, MoveChrPartyLeave, ScriptCommandMapObjectsRemove
           and others */
        u8 kind;
    } on_kind;
    /* +0x01 a union: a hold flag to the party's code, a definition number to
       the map objects' code. hold: while it is not 0 MapPartyAnimationStep
       does not count the party record's timer (the record is still drawn, on
       its step). The menu and the save hold the members they line up and the
       ones they take out of the party for their duration, with bit 0x8000 of
       sprite.flags to shade them (MenuRun, MenuSaveRun, MenuSaveWrite),
       MenuLineUpDisp holds every member but the one picked; the shop holds the
       extra member it lines up last (ShopOpen); MenuRun, MenuSaveRun,
       MenuSaveWrite, MenuLineUpDisp and MapPartyRecordInit set 0, and
       MoveChrPartyJoin sets 0 right after it copies an object's row into a
       party record (which brings the definition number with it).
       ShopBuySellDisp sets it 1 on the shop object not chosen and 0 on the
       other, with bit 0x8000 of their flags, but ShopAnimationStep does not
       test it: nothing reads a shop object's hold. definition: MapObjectInit
       writes the definition's number, MapObjectRestore the map list's byte +1,
       MoveChrPartyLeave the definition of a member who stays behind as an
       object; the twins get it with the row's first 0x3C bytes. MapObjectFind
       matches it, MoveChrRowIdGet, MoveChrPartyJoin,
       ScriptCommandMoveChrRowsEnd, ScriptCommandMapObjectsRemove,
       MapObjectsSpriteDataSet, MapPartyTalkCheck, MapObjectTouchFind and the
       behaviours of map_look.c (MapObjectPartySeenStep,
       MapObjectPartyLook03Step, MapObjectPartyLook0AStep,
       MapObjectPartySeen01Step to MapObjectPartySeen06Step) index
       g_MapObjectDefinitions with it. */
    union {
        /* hold: used by MapPartyAnimationStep, ShopBuySellDisp, and, it, menu,
           nothing, objects, party, reads, records, s, save and others */
        u8 hold;
        /* definition: used by MapObjectFind, MapObjectInit,
           MapObjectPartyLook, MapObjectPartyLook03Step,
           MapObjectPartyLook0AStep, MapObjectPartySeen,
           MapObjectPartySeen01Step, MapObjectPartySeen06Step,
           MapObjectPartySeenStep, MapObjectRestore, MapObjectTouchFind,
           MapObjectsSpriteDataSet and others */
        u8 definition;
    } hold_definition;
    /* +0x02..+0x03 nothing reads or writes them on their own; they go with the
       whole-record copies and the clears (MapRecordsClear, MapObjectsDataLoad,
       MoveChrInit, MenuRecordsInit, MessageRecordsInit). ShopObjectInit's own
       comment notes it leaves +1..+3 alone. */
    u8 pad_02[2];
    /* +0x04..+0x27 the record's sprite entry (SpriteEntry, sprite.h): the 0x24
       bytes each stepper copies whole to its entry of the sprite list every
       frame, then sets the copy's pose from the step list
       (MapPartyAnimationStep: entries 0 to 5 and the twins 0x46 to 0x4B;
       MapObjectsAnimationStep: from 0x10 and the twins from 0x56;
       MoveChrAction: 6 to 0x10 and 0x4C to 0x56; ShopAnimationStep: the
       caller's run, 0 up for the lined-up party, 0x72 to 0x78 the shop
       objects, 0x6E to 0x73 the menu use's shows; MenuRecordsAnimationStep:
       0x95 down to 0x8F). On the map sprite.x is the x on the map in pixels,
       sprite.depth the ground y (the sort key) and sprite.y the y drawn, the
       ground y less the height (MoveChrWalkStep, MapPartyRingFollow,
       MoveChrPartyJoin, ScriptCommandMoveChrRowActorSet), so a twin, given the
       record's ground y for both (MoveChrAction), stands on the ground under a
       raised actor; the menu puts the depth at y + 0xE0 (MenuOpen, MenuClose,
       MenuLineUpMove, MenuLineUpSpotsMove, MenuBtlPlacesStep) and the shop at
       0x1000 (ShopOpen, ShopObjectInit) to draw in front. sprite.on switches
       the picture alone (MapRow2Change, MapFadePartyIn, MapPartyOnPlay,
       ScriptCommandMessageTimedSet, MessageArrowStep's test, the menu's pages,
       MenuBtlPlacesClose and MenuPresetClose write the guest's id 0xC there);
       sprite.flags is set by MapRecordInit from the kind and mode (0x2000,
       0x2100, 0x2200, 0x2370, with 0x800 or 0xC00), its bits 0x6000 are the
       class MapAttributeClassGet gives the record's place (MapObjectRestore,
       MapObjectInit, ScriptCommandMoveChrRowActorSet, MoveChrWalkStep,
       MapPartyRingFollow, MapPartyTwinSpotSet, MapFadePartyOut and
       MapFadePartyIn, MenuClose), bit 0x8000 shades a held record (MenuRun,
       MenuSaveRun, MenuSaveWrite, ShopOpen, ShopBuySellDisp); sprite.attr is
       0x20 or 0 with 0x8000 for mode 3 (MapRecordInit); sprite.scaled, scale_x
       and scale_y are the script's scale (MoveChrScaleSet, MoveChrScaleStep).
       It is a SpriteEntry, embedded as in BtlRecord by the owner's ruling of
       2026-10-14. */
    SpriteEntry sprite;
    /* +0x28 the address of the animation table: one 4-byte row for each
       animation number, a 16-bit count of steps (top bit: hold on the last
       step) and the 16-bit number of the first step in the step list, as in
       BtlRecord. Written by MapRecordSpriteDataSet from the sprite data's
       header word 0 (for MapRecordInit, MapPartySpriteDataSet,
       MapObjectsSpriteDataSet, MoveChrSpriteDataSet, MenuOpen, MenuClose,
       ScriptCommandChr1ChrBChange, ScriptCommandMapPartyKindSet), and from the
       bank g_SpriteData0's word 0 by ShopObjectInit, MenuRecordsInit,
       MessageRecordsInit and MenuUseShowPlacesPlay. Read by MapAnimationStart
       and ShopAnimationSet for the animation's row, and by the steppers for
       the row's top bit (MapPartyAnimationStep, MapObjectsAnimationStep,
       MoveChrAction, MenuRecordsAnimationStep); MenuRecordsInit reads row 0xE
       of it. */
    s32 animation_table;
    /* +0x2C the address of the step list: 4-byte steps, a pose and a length in
       frames (the battle's AnimStep). Written by the same functions as
       animation_table, from the sprite data's header word 1. Read by
       MapAnimationStart and ShopAnimationSet for the first step's length, by
       the steppers for each step's pose (the sprite-list copy's) and length,
       and by MenuBtlPlacesOpen (the pose of the current step for its copies in
       the list) and MenuRecordsInit. */
    s32 step_list;
    /* +0x30 the number of the animation the record is on. MapAnimationSet
       stores a new number here (keeping the old one in animation_before) and
       starts it with MapAnimationStart, restarting the timer unless only the
       facing column of a pose group 1..60 changed; MapAnimationPoseSet picks
       the number from a pose and a facing (pose < 0 keeps this one's group).
       ShopAnimationSet stores it and starts the animation always;
       MapRecordInit sets 0xFFFF so that the next MapAnimationSet starts
       whatever it names; ShopObjectInit stores its animation,
       MenuUseShowPlacesPlay 0xFF before ShopAnimationSet. Read by the steppers
       (the row's hold bit; the twins are put on the record's number),
       MapAnimationStart, and OutlineMapStep, which keeps the outline's counter
       still while bit 0 is set. */
    u16 animation;
    /* +0x32 the animation number before the last change: MapAnimationSet
       stores the old animation here and reads it back to see whether the
       change is only a facing column of one pose group (then the timer and
       step length stay); ShopAnimationSet stores the old number too.
       ShopObjectInit stores 1 for animation 0 and 0 for any other,
       MenuRecordsInit and MessageRecordsInit 0xFFFF, MenuUseShowPlacesPlay
       0xFF: a number unlike the first one set. */
    u16 animation_before;
    /* +0x34 frames spent on the current step: the steppers count it up to
       step_length (MapPartyAnimationStep unless the record is held,
       MapObjectsAnimationStep unless the object's behaviour holds it,
       MoveChrAction, ShopAnimationStep, MenuRecordsAnimationStep) and set 0
       when a step begins; MapAnimationStart sets 0 on a restart,
       ShopAnimationSet always; MapPartySpriteDataSet sets it to step_length so
       that the next frame steps on. A twin takes its record's
       (MapPartyAnimationStep, MapObjectsAnimationStep, MoveChrAction);
       ShopListSet gives each list mark the other's. */
    u16 timer;
    /* +0x36 the length in frames of the current step, read from the step list
       when a step begins (the steppers, MapAnimationStart on a restart,
       ShopAnimationSet). A twin takes its record's; ShopListSet gives each
       list mark the other's. */
    u16 step_length;
    /* +0x38 the number of the current step in the step list: MapAnimationStart
       and ShopAnimationSet put it on the animation's first step, the steppers
       move it on (and back by one at the end when the hold bit holds the last
       step). A twin is put as far from its own end as its record is from the
       record's (MapPartyAnimationStep, MapObjectsAnimationStep,
       MoveChrAction); ShopListSet does the same between the two list marks.
       Read also by MenuUseShowPlacesPlay (it waits for the last step),
       MapObjectTurnStep (a kind 3 object turns only after its last step) and
       MenuBtlPlacesOpen (the pose of the step). */
    u16 step;
    /* +0x3A one past the animation's last step. MapAnimationStart stores first
       step + (count & 0x7FFF): the steppers it serves (MapPartyAnimationStep,
       MapObjectsAnimationStep, MoveChrAction, MenuRecordsAnimationStep)
       compare the step with it and test the hold bit in the animation table's
       row. ShopAnimationSet stores first step + the raw count, so the count's
       top bit stays in bit 15, which ShopAnimationStep reads as 'hold the last
       step' (end & 0x7FFF the end); MenuUseShowPlacesPlay waits for step (end
       & 0x7FFF) - 1. Read also by MapObjectTurnStep, ShopListSet and the
       twins' sync. */
    u16 end;
} MapRecord;

LAYOUT_SIZE(MapRecord, 0x3c);
LAYOUT_AT(MapRecord, on_kind, on_kind, 0x00);
LAYOUT_AT(MapRecord, on_kind_on, on_kind.on, 0x00);
LAYOUT_AT(MapRecord, on_kind_kind, on_kind.kind, 0x00);
LAYOUT_AT(MapRecord, hold_definition, hold_definition, 0x01);
LAYOUT_AT(MapRecord, hold_definition_hold, hold_definition.hold, 0x01);
LAYOUT_AT(MapRecord, hold_definition_definition, hold_definition.definition, 0x01);
LAYOUT_AT(MapRecord, pad_02, pad_02, 0x02);
LAYOUT_AT(MapRecord, sprite, sprite, 0x04);
LAYOUT_AT(MapRecord, sprite_on, sprite.on, 0x04);
LAYOUT_AT(MapRecord, sprite_x, sprite.x, 0x06);
LAYOUT_AT(MapRecord, sprite_depth, sprite.depth, 0x08);
LAYOUT_AT(MapRecord, sprite_y, sprite.y, 0x0a);
LAYOUT_AT(MapRecord, sprite_attr, sprite.attr, 0x0c);
LAYOUT_AT(MapRecord, sprite_flags, sprite.flags, 0x0e);
LAYOUT_AT(MapRecord, sprite_texel_records, sprite.texel_records, 0x10);
LAYOUT_AT(MapRecord, sprite_pose_table, sprite.pose_table, 0x14);
LAYOUT_AT(MapRecord, sprite_piece_table, sprite.piece_table, 0x18);
LAYOUT_AT(MapRecord, sprite_pose, sprite.pose, 0x1c);
LAYOUT_AT(MapRecord, sprite_pad_1a, sprite.pad_1a, 0x1e);
LAYOUT_AT(MapRecord, sprite_scaled, sprite.scaled, 0x20);
LAYOUT_AT(MapRecord, sprite_pad_1e, sprite.pad_1e, 0x22);
LAYOUT_AT(MapRecord, sprite_scale_x, sprite.scale_x, 0x24);
LAYOUT_AT(MapRecord, sprite_scale_y, sprite.scale_y, 0x26);
LAYOUT_AT(MapRecord, animation_table, animation_table, 0x28);
LAYOUT_AT(MapRecord, step_list, step_list, 0x2c);
LAYOUT_AT(MapRecord, animation, animation, 0x30);
LAYOUT_AT(MapRecord, animation_before, animation_before, 0x32);
LAYOUT_AT(MapRecord, timer, timer, 0x34);
LAYOUT_AT(MapRecord, step_length, step_length, 0x36);
LAYOUT_AT(MapRecord, step, step, 0x38);
LAYOUT_AT(MapRecord, end, end, 0x3a);

/* MapBox: 0x4 bytes. */
typedef struct {
    /* +0x00 (in a MapObject +0x50) the box's middle less x, in pixels, signed:
       the callers add it to x before MapAttributeFreeCheck or MapBoxTouchGet
       (which reads only the two half sizes). The party's box has 0
       (MapPartyBoxSet). */
    s8 dx;
    /* +0x01 (in a MapObject +0x51) the box's middle less the ground y, in
       pixels, signed (the party's box: 0, or -1 for a party kind other than
       0). */
    s8 dy;
    /* +0x02 (in a MapObject +0x52) half the box's width in pixels; 0 makes
       MapBoxTouchGet answer 0, so such a box never touches (the party's box:
       6, or 8). */
    u8 half_width;
    /* +0x03 (in a MapObject +0x53) half the box's height in pixels (the
       party's box: 4, or 5). */
    u8 half_height;
} MapBox;

LAYOUT_SIZE(MapBox, 0x04);
LAYOUT_AT(MapBox, dx, dx, 0x00);
LAYOUT_AT(MapBox, dy, dy, 0x01);
LAYOUT_AT(MapBox, half_width, half_width, 0x02);
LAYOUT_AT(MapBox, half_height, half_height, 0x03);

/* MapObject: one of the map's objects, 0x60 bytes: its animated record (the
   first 0x3C bytes, the same record as a party record or a twin) and then what
   it thinks, sees and moves by. The map's 0x80 live at g_MapObjects
   (0x801D6B80, set by GameStart; their twins, plain 0x3C-byte records, at
   g_MapObjectTwins), and the script rows' six own actors at g_MoveChrOwnActors
   (0x801C9B78, set by MoveChrInit; twins at g_MoveChrOwnActorTwins). */
typedef struct {
    /* +0x00..+0x3B the object's animated record, the same 0x3C bytes as a
       party record or a twin: the on byte, the sprite entry at +4 that
       MapObjectsAnimationStep copies to the sprite list, the animation words
       at +0x28..+0x3B. The object does not point at a record: it begins with
       one, and the record functions are given the object's own address
       (MapRecordInit, MapRecordSpriteDataSet, MapAnimationPoseSet,
       MapAnimationSet, MapAnimationStart; a script row's +0xC holds it as its
       actor). MapObjectInit, MapObjectRestore and
       ScriptCommandMoveChrRowActorSet copy it whole to the object's twin;
       MoveChrPartyLeave copies a party record whole into it, MoveChrPartyJoin
       an object's whole into a party record. In an object, record byte 0 is
       the kind (record.on_kind.kind), (definition byte 0 & 0x3F) + 1, 1 to 7,
       0 for a free row, with bit 0x80 while a script row holds the object;
       byte 1 (record.hold_definition.definition) is the number of the
       0x26-byte definition at g_MapObjectDefinitions the object was made from.
       A MapRecord (map.h). */
    MapRecord record;
    /* +0x3C the object's layer word, 0 or 0x80 (bit 0x80 of its definition's
       byte 0). It is the layer MapAttributeFreeCheck, MapSpotMoveTry and
       MapAttributeClassGet are given (at 0 every attribute with a bit of
       0x3800 stops the box; at 0x80 only those that also have bit 0x4000), and
       the flag MapObjectTouchFind gives MapAttributeLineCheck. Written by
       MapObjectInit and MapObjectRestore (from the definition, not from the
       save), MoveChrPartyLeave (the leaving member's script row +0x20) and
       ScriptCommandMoveChrRowActorSet (an own actor: the command's byte 9); a
       script row that takes the object copies it to its +0x20
       (ScriptCommandMoveChrRowActorSet). */
    u8 layer;
    /* +0x3D the object's number. Its low six bits pick the entry of the nine
       think functions at 0x800B4658 (vtbl_4658_table) that MapObjectsThink
       calls: 0 MapObjectWanderStep, 1 and 2 StepCommandRun (the step list,
       +0x5C), 3 and 4 func_80067584 (MapObjectsThink passes over both; 3 is
       also not animated by MapObjectsAnimationStep, not touched by
       MapObjectTouchFind, not taken by MapObjectCursorAdvance), 5
       MapObjectTurnStep, 6 MapObjectPartyLookStep, 7 MapObjectPartySeenStep, 8
       MapObjectPartyAwayStep. Bit 0x40: the object turns one facing at a time
       (MapObjectFacingTurnSet, tested by MapObjectRandomWalkStart,
       MapObjectFacingSet, MapObjectPartyLook02/03/04Step). Bit 0x80 is kept
       with 0x40 whenever the number changes; nothing found tests it. Written
       by MapObjectInit ((definition byte 0 & 0x40) | definition byte 0xC),
       MapObjectRestore (the save), MapObjectNumberReset (definition byte 0xC),
       MapObjectPartySeenCheck (7 or 8 when the object sees the party),
       MapObjectPartySeen03Step (4, for the object that stopped its walk), and
       2 for the own actors (MoveChrPartyLeave,
       ScriptCommandMoveChrRowActorSet). */
    u8 number;
    /* +0x3E the object's facing, 1 to 8 (1 toward smaller y, then clockwise; 0
       for the wander stands still), given to MapAnimationPoseSet with each
       pose. For a chest (kind 7) 5 to 8 is shut and 1 to 4 open: MapObjectInit
       takes 4 off when the chest's story bit is set, MapObjectKind7Run takes 4
       off when it opens and gives them back when nobody had room. Written by
       MapObjectInit (definition byte 6 + 1), MapObjectRestore (the save),
       MapObjectWanderStep and MapObjectTurnStep (rng_draw() % 8 + 1),
       StepWalkStep (toward its goal, then +0x4A), StepCommandFacingSet (the
       step's byte 1), the looks' walks and turns (MapObjectTowardWalk,
       MapObjectAwayWalk, MapObjectRandomWalkStart, MapObjectFacingTurnStep,
       MapObjectFacingSet, MapObjectPartyLook02/03/04/05/06/0AStep,
       MapObjectPartySeen04/05Step), MapObjectTouchApproach (its steps toward
       the party), MapBtlRun (back from g_MapTouchedObjectKeptFacing after a
       fight that answered 2), MoveChrPartyLeave and
       ScriptCommandMoveChrRowActorSet (an own actor: the command's byte 8),
       and ScriptCommandMoveChrRowsEnd and ScriptCommandMoveChrRowsAllEnd (back
       from the script row's +0x21 when the row lets the object go). */
    u8 facing;
    /* +0x3F the pixels the object moves in one think: MapObjectWanderStep
       takes that many one-pixel steps, StepWalkStep and the looks' walks
       (MapObjectTowardWalk, MapObjectAwayWalk, MapObjectFacingWalk) move that
       far, and a walk's length becomes thinks as value_div(pixels, +0x3F)
       (MapObjectRandomWalkStart, MapObjectFacingTurnStep,
       MapObjectPartyLook04Step, MapObjectPartySeen04Step). Written by
       MapObjectMoveSet (g_MoveSpeedPixels at the definition's byte 0xD + 2 for
       kinds 4 to 6, + 3 for the others; 1 for a step list), MapObjectRestore
       (the save), StepCommandSpeedSet (g_MoveSpeedPixels at the step's byte 1)
       and MapObjectPartySeenCheck (g_MoveSpeedPixels at the definition's byte
       0x22 + 2 when it sees the party). */
    u8 speed;
    /* +0x40 the passes of MapObjectsThink left before the object's next think:
       MapObjectsThink counts it down at each pass that reaches the object and
       at 0 loads it again from +0x41 and calls the object's think. Set to
       +0x41 by MapObjectMoveSet and by MapObjectPartySeenCheck. Not saved
       (MenuSaveWrite skips it): after MapObjectRestore it holds what
       MapObjectMoveSet set from the definition. */
    u8 interval_left;
    /* +0x41 the object's interval: the passes of MapObjectsThink from one of
       its thinks to the next (g_MoveSpeedIntervals). The looks turn a wait of
       N passes into thinks with value_div(N, +0x41). Written by
       MapObjectMoveSet (g_MoveSpeedIntervals at the same place as +0x3F for
       numbers 0 and 6, 1 for any other number), MapObjectRestore (the save),
       StepCommandSpeedSet (g_MoveSpeedIntervals at the step's byte 1) and
       MapObjectPartySeenCheck (at the definition's byte 0x22 + 2). */
    u8 interval;
    /* +0x42 a count of thinks toward a new facing. MapObjectTurnStep (number
       5) counts every think and at 0x10 draws a facing (a kind 3 waits there
       for its animation's last step) and zeroes it. MapObjectWanderStep
       (number 0) counts each think another object blocks it, sets 0x10 at once
       when a wall or its bounds stop it, draws a facing and zeroes it once it
       is 0x10 or more, and zeroes it after a walk that went its whole way.
       Zeroed by MapObjectInit, MoveChrPartyLeave and
       ScriptCommandMoveChrRowActorSet; MapObjectRestore takes the save's. */
    u8 turn_count;
    /* +0x43 the object's look, for number 6: the low seven bits pick the entry
       of the 14 at D_800B5B3C that MapObjectPartyLookStep calls
       (MapObjectPartyLook00Step to MapObjectPartyLook0CStep), and at 0
       MapObjectPartySeenCheck does not look for the party;
       MapObjectsAnimationStep does not step the animation of a number 6 object
       on look 1 (standing) whose +0x46 is 0. Bit 0x80 is kept by
       MapObjectPartyLook03Step, which puts the object on look 2 when it is
       home again; nothing found tests it. The first of the five bytes
       MapObjectMoveSet copies from the definition (+0x14, or +0x19 for the
       definition's second set); MapObjectRestore takes the save's. */
    u8 look;
    /* +0x44 how wide a number 6 object sees: MapObjectPartySeenCheck looks for
       the party's box in a box ahead along the facing that reaches +0x44 * 4
       to either side, or, when the definition's byte 0x1E has bit 0x80, in a
       square round the object +0x44 * 8 each way (number 7 takes the
       definition's bytes 0x1F and 0x20 instead, number 8 a square of 0x60).
       Copied by MapObjectMoveSet from the definition's +0x15 (or +0x1A);
       MapObjectRestore takes the save's. */
    u8 sight_width;
    /* +0x45 how far a number 6 object sees: the box ahead of
       MapObjectPartySeenCheck is +0x45 * 4 half long, so it reaches +0x45 * 8
       pixels ahead along the facing (a strip along the diagonal for the even
       facings, MapBoxDiagonalTouchGet). Copied by MapObjectMoveSet from the
       definition's +0x16 (or +0x1B); MapObjectRestore takes the save's. */
    u8 sight_length;
    /* +0x46 the length the look's walks or waits take in place of a drawn one
       (rng_range_draw's fixed value; 0 draws one): a walk in units of 8 pixels
       (8 to 0x10 drawn; MapObjectRandomWalkStart, MapObjectFacingTurnStep,
       MapObjectPartyLook04Step), or a wait in units of 0x1E passes (1 to 3
       drawn; MapObjectPartyLook02Step, MapObjectPartyLook0BStep). On look 1
       (standing) a 0 also keeps MapObjectsAnimationStep from stepping the
       animation. Copied by MapObjectMoveSet from the definition's +0x17 (or
       +0x1C); MapObjectRestore takes the save's. */
    u8 length;
    /* +0x47 the look's second value in place of a drawn one (rng_range_draw's
       fixed value; 0 draws): the turns about after a wait (3 to 5 drawn;
       MapObjectPartyLook02Step, MapObjectPartyLook04Step,
       MapObjectPartyLook07Step), the length of a stop in units of 0x1E passes
       (1 to 3 drawn; MapObjectPartyLook06Step, so looks 6, 7, 9 and 0xC), and
       on looks 5 and 8 whether a stopped walk always turns about
       (MapObjectSideFacingGet's reverse, given it by
       MapObjectPartyLook05Step). Copied by MapObjectMoveSet from the
       definition's +0x18 (or +0x1D); MapObjectRestore takes the save's. */
    u8 count;
    /* +0x48 where the object's look or sight is: 0 to begin (zeroed by
       MapObjectMoveSet, MapObjectNumberReset and MapObjectPartySeenCheck);
       0xFF walking (MapObjectRandomWalkStart) or, on numbers 7 and 8, seeing
       the party; 0xFE turning in steps before a walk, 0xFD turning for
       MapObjectFacingSet, 0xFC waiting before a walk
       (MapObjectPartySeen03Step), each ended by MapObjectFacingTurnStep or the
       wait; 0x80 the party is not seen any more (MapObjectPartySeenCheck,
       which puts 0x80 to 0xEF back to 0xFF while it is seen); 0x90 and 0xA0
       the stops of looks 6 and 0xB; 1 the wait after the party went
       (MapObjectPartySeen01/03/05/06Step); below 0x80, on looks 2, 4 and 7,
       the turns about left. Written and tested by the look and seen steps
       (map_look.c); MapObjectTouchRemove reads it signed; MapObjectRestore
       takes the save's. */
    u8 state;
    /* +0x49 the thinks left of the look's walk, wait or turn:
       MapObjectRandomWalkStart sets value_div(length * 8, +0x3F) for a walk,
       the waits value_div(N passes, +0x41), MapObjectFacingSet and the turns
       in steps 1, MapObjectFacingTurnStep value_div(5, +0x41) between two
       places of a turn; the looks, the seen steps and MapObjectFacingTurnStep
       count it down at each think. MapObjectRestore takes the save's. */
    u8 thinks_left;
    /* +0x4A a facing kept aside, 0 for none. On a step list: the facing a walk
       step (StepCommandMoveTo, StepCommandMoveBy: the step's byte 4) shows
       while it walks and ends on (StepWalkStep). On the looks: the facing of
       the walk under way, 0 once a walk has been stopped (MapObjectTowardWalk
       keeps the facing toward its goal, MapObjectAwayWalk the facing away,
       MapObjectFacingWalk the facing after a whole step), and the facing a
       look turns back to after its turns or wait
       (MapObjectPartyLook02/03/04/07Step and MapObjectPartySeen01/03/05Step
       keep +0x3E there). Zeroed by MapObjectMoveSet; MapObjectRestore takes
       the save's. */
    u8 facing_kept;
    /* +0x4B a turn in steps under way: the places left + 1, with bit 0x80 for
       clockwise; 0 none. MapObjectFacingTurnSet plans it (for an object with
       bit 0x40 of +0x3D), MapObjectFacingTurnStep carries it out one place
       every value_div(5, +0x41) thinks and zeroes it at the end,
       MapObjectPartyLook04Step sets 3 or 0x83 (two places), MapObjectMoveSet,
       MapObjectPartySeen04Step and MapObjectPartySeenCheck zero it. While it
       is not 0, numbers 6 to 8 (MapObjectPartyLookStep,
       MapObjectPartySeenStep, MapObjectPartyAwayStep) call
       MapObjectFacingTurnStep in place of their look. MapObjectRestore takes
       the save's. */
    u8 turn_plan;
    /* +0x4C..+0x4F what the look's walk is about. spot: x and y in the map's
       pixels of a spot the object walks to (MapObjectPartyLook03Step: its
       definition's place, bytes 4 and 5 * 8; MapObjectPartySeen01Step: the
       first party record's place while it sees the party;
       MapObjectPartySeen02Step: the definition's bytes 0x23 and 0x24 * 8) or
       away from (MapObjectPartySeen06Step: the first party record's place).
       overlapped, in the first half, on seen 3: the object that stopped the
       walk, as MapObjectOverlapFind answers it (its row number + 2), set by
       MapObjectPartySeen03Step when that object's definition is the one the
       definition's byte 0x24 names, and taken off the map and zeroed by
       MapObjectOverlappedRemove (MapObjectPartySeen03Step,
       MapObjectTouchRemove). MapObjectRestore and MenuSaveWrite copy the two
       halves as 16-bit words. */
    union {
        /* +0x4C, +0x4E the spot the look walks to or away from, x and y in the
           map's pixels */
        struct {
            s16 x;
            s16 y;
        } spot;
        /* +0x4C on seen 3: the row number + 2 of the object that stopped the
           walk (MapObjectOverlapFind's answer), 0 for none */
        s16 overlapped;
    } target;
    /* +0x50..+0x53 the object's box on the map: a signed dx and dy from x and
       the ground y, a half width and a half height (the glossary's box). The
       attribute map is tested under it (MapAttributeFreeCheck,
       MapAttributeClassGet, MapSpotMoveTry, MapAttributeFind) and the objects
       and the party against one another with it (MapBoxTouchGet:
       MapObjectOverlapFind, MapPartyObjectsFreeCheck, MapPartyTalkCheck; a
       copy grown by the definition's byte 0x13 * 8 in MapObjectTouchFind, by 1
       in MapObjectTouchApproach and MapObjectTouchWait); a half width of 0
       never touches. Copied whole by MapObjectInit and MapObjectRestore (the
       definition's bytes 7 to 0xA, not the save), MoveChrPartyLeave (the
       party's box, g_MapPartyBoxDx to g_MapPartyBoxHalfHeight) and
       ScriptCommandMoveChrRowActorSet (the command's bytes 0xB to 0xE); a
       script row keeps its address at +0x14 for MoveChrWalkStep. */
    MapBox box;
    /* +0x54..+0x5B what the object moves inside or by, as its number says.
       bounds, numbers 0 and 6 to 8 (the wander, the looks and seen steps):
       left <= x < right and top <= y < bottom, which the walks do not leave;
       MapObjectMoveSet sets them round where the object stands from the
       definition's signed bytes 0xE to 0x11 * 8 (left and top at least 0,
       right and bottom at most 0x7FF) and keeps them when the number is reset.
       step, numbers 1 and 2 (a step list): the step under way, a walk step's
       goal x and y (StepCommandMoveTo, StepCommandMoveBy) or, at +0x54, a wait
       step's thinks left (StepCommandWait); busy, 1 while a step takes more
       than one think; and the cursor, the place in the step list (+0x5C) of
       the step to run, which each step moves past itself (MapObjectMoveSet
       sets 2, past the list's two first bytes). MapObjectRestore and
       MenuSaveWrite copy the four words whole. */
    union {
        /* numbers 0 and 6 to 8: left <= x < right, top <= ground y < bottom
           (right and bottom are outside), as g_MapLimitLeft to
           g_MapLimitBottom bound the map */
        struct {
            s16 left;
            s16 right;
            s16 top;
            s16 bottom;
        } bounds;
        /* numbers 1 and 2: a walk step's goal (x, ground y), 1 while a step
           that takes several thinks is under way, and the place in the step
           list of the step to run */
        struct {
            s16 goal_x;
            s16 busy;
            s16 goal_y;
            s16 cursor;
        } step;
        /* +0x54 for a wait step: its thinks left, from the step's byte 1,
           counted down to 0 (StepCommandWait) */
        s16 wait_left;
    } move;
    /* +0x5C the address of the object's step list, for numbers 1 and 2:
       MapObjectMoveSet points it at g_MapObjectStepLists + the 16-bit offset
       in that table's entry the definition's byte 0x12 picks. Byte 1 of the
       list is the pose MapObjectMoveSet answers (MapObjectInit starts the
       object in it); from the cursor (+0x5A) on, steps of an op byte and its
       operands, which StepCommandRun runs through the ten handlers at
       D_800B4620 (ops 0xF0 to 0xF9). Read by StepCommandRun and the step
       handlers (StepCommandPoseSet, StepCommandFacingSet, StepCommandMoveTo,
       StepCommandMoveBy, StepCommandSpeedSet, StepCommandWait,
       StepCommandPlace). MenuSaveWrite saves the address itself and
       MapObjectRestore puts it back. */
    s32 step_list;
} MapObject;

LAYOUT_SIZE(MapObject, 0x60);
LAYOUT_AT(MapObject, record, record, 0x00);
LAYOUT_AT(MapObject, record_on_kind, record.on_kind, 0x00);
LAYOUT_AT(MapObject, record_on_kind_on, record.on_kind.on, 0x00);
LAYOUT_AT(MapObject, record_on_kind_kind, record.on_kind.kind, 0x00);
LAYOUT_AT(MapObject, record_hold_definition, record.hold_definition, 0x01);
LAYOUT_AT(MapObject, record_hold_definition_hold, record.hold_definition.hold, 0x01);
LAYOUT_AT(MapObject, record_hold_definition_definition, record.hold_definition.definition, 0x01);
LAYOUT_AT(MapObject, record_pad_02, record.pad_02, 0x02);
LAYOUT_AT(MapObject, record_sprite, record.sprite, 0x04);
LAYOUT_AT(MapObject, record_sprite_on, record.sprite.on, 0x04);
LAYOUT_AT(MapObject, record_sprite_x, record.sprite.x, 0x06);
LAYOUT_AT(MapObject, record_sprite_depth, record.sprite.depth, 0x08);
LAYOUT_AT(MapObject, record_sprite_y, record.sprite.y, 0x0a);
LAYOUT_AT(MapObject, record_sprite_attr, record.sprite.attr, 0x0c);
LAYOUT_AT(MapObject, record_sprite_flags, record.sprite.flags, 0x0e);
LAYOUT_AT(MapObject, record_sprite_texel_records, record.sprite.texel_records, 0x10);
LAYOUT_AT(MapObject, record_sprite_pose_table, record.sprite.pose_table, 0x14);
LAYOUT_AT(MapObject, record_sprite_piece_table, record.sprite.piece_table, 0x18);
LAYOUT_AT(MapObject, record_sprite_pose, record.sprite.pose, 0x1c);
LAYOUT_AT(MapObject, record_sprite_pad_1a, record.sprite.pad_1a, 0x1e);
LAYOUT_AT(MapObject, record_sprite_scaled, record.sprite.scaled, 0x20);
LAYOUT_AT(MapObject, record_sprite_pad_1e, record.sprite.pad_1e, 0x22);
LAYOUT_AT(MapObject, record_sprite_scale_x, record.sprite.scale_x, 0x24);
LAYOUT_AT(MapObject, record_sprite_scale_y, record.sprite.scale_y, 0x26);
LAYOUT_AT(MapObject, record_animation_table, record.animation_table, 0x28);
LAYOUT_AT(MapObject, record_step_list, record.step_list, 0x2c);
LAYOUT_AT(MapObject, record_animation, record.animation, 0x30);
LAYOUT_AT(MapObject, record_animation_before, record.animation_before, 0x32);
LAYOUT_AT(MapObject, record_timer, record.timer, 0x34);
LAYOUT_AT(MapObject, record_step_length, record.step_length, 0x36);
LAYOUT_AT(MapObject, record_step, record.step, 0x38);
LAYOUT_AT(MapObject, record_end, record.end, 0x3a);
LAYOUT_AT(MapObject, layer, layer, 0x3c);
LAYOUT_AT(MapObject, number, number, 0x3d);
LAYOUT_AT(MapObject, facing, facing, 0x3e);
LAYOUT_AT(MapObject, speed, speed, 0x3f);
LAYOUT_AT(MapObject, interval_left, interval_left, 0x40);
LAYOUT_AT(MapObject, interval, interval, 0x41);
LAYOUT_AT(MapObject, turn_count, turn_count, 0x42);
LAYOUT_AT(MapObject, look, look, 0x43);
LAYOUT_AT(MapObject, sight_width, sight_width, 0x44);
LAYOUT_AT(MapObject, sight_length, sight_length, 0x45);
LAYOUT_AT(MapObject, length, length, 0x46);
LAYOUT_AT(MapObject, count, count, 0x47);
LAYOUT_AT(MapObject, state, state, 0x48);
LAYOUT_AT(MapObject, thinks_left, thinks_left, 0x49);
LAYOUT_AT(MapObject, facing_kept, facing_kept, 0x4a);
LAYOUT_AT(MapObject, turn_plan, turn_plan, 0x4b);
LAYOUT_AT(MapObject, target, target, 0x4c);
LAYOUT_AT(MapObject, target_spot, target.spot, 0x4c);
LAYOUT_AT(MapObject, target_spot_x, target.spot.x, 0x4c);
LAYOUT_AT(MapObject, target_spot_y, target.spot.y, 0x4e);
LAYOUT_AT(MapObject, target_overlapped, target.overlapped, 0x4c);
LAYOUT_AT(MapObject, box, box, 0x50);
LAYOUT_AT(MapObject, box_dx, box.dx, 0x50);
LAYOUT_AT(MapObject, box_dy, box.dy, 0x51);
LAYOUT_AT(MapObject, box_half_width, box.half_width, 0x52);
LAYOUT_AT(MapObject, box_half_height, box.half_height, 0x53);
LAYOUT_AT(MapObject, move, move, 0x54);
LAYOUT_AT(MapObject, move_bounds, move.bounds, 0x54);
LAYOUT_AT(MapObject, move_bounds_left, move.bounds.left, 0x54);
LAYOUT_AT(MapObject, move_bounds_right, move.bounds.right, 0x56);
LAYOUT_AT(MapObject, move_bounds_top, move.bounds.top, 0x58);
LAYOUT_AT(MapObject, move_bounds_bottom, move.bounds.bottom, 0x5a);
LAYOUT_AT(MapObject, move_step, move.step, 0x54);
LAYOUT_AT(MapObject, move_step_goal_x, move.step.goal_x, 0x54);
LAYOUT_AT(MapObject, move_step_busy, move.step.busy, 0x56);
LAYOUT_AT(MapObject, move_step_goal_y, move.step.goal_y, 0x58);
LAYOUT_AT(MapObject, move_step_cursor, move.step.cursor, 0x5a);
LAYOUT_AT(MapObject, move_wait_left, move.wait_left, 0x54);
LAYOUT_AT(MapObject, step_list, step_list, 0x5c);

/* An entry of the party's ring, the 0x60 entries of 8 bytes at the address in
   g_MapPartyRingEntries (0x801EA000, MapRecordsClear) with the head
   g_MapPartyRingHead: a spot the party's first record stood on, with the
   height, the facing and the class it had there; the record of place k stands
   on the entry 16 * k behind the head. MapPartyRingEntryGet copies one out
   into an 8-byte local of its caller and MapPartyRingEntrySet one in;
   MapPartyRingFill and MapPartyRingPush build one on their stack; the spot
   MapPartySpotSet, MapPartyRecordSpotSet and MapPartyTwinSpotSet take, and the
   party's place in a saved game (GameMapRecord +0xA), is one. */
typedef struct {
    /* +0x00 x in the map's pixels: MapPartyRingFill and MapPartyRingPush store
       their argument; MapPartyRingFollow, MapPartyTwinSpotSet,
       MapPartyRecordSpotSet and MapPartySpotSet put a record on it, as
       MoveChrWalkStep, MoveChrPartyJoin, MapFadePartyIn and MenuClose do with
       an entry they took; MoveChrWalkStep builds one from a row's actor for
       MapPartyTwinSpotSet, MoveChrPartyLeave one from the first record's x for
       MapPartyRingFill; MoveChrFollowStart and MoveChrFollowRestart make it a
       row's goal x. */
    u16 x;
    /* +0x02 the ground y in the map's pixels, written and read with x: a
       record put on the entry gets it as its sprite.depth and it less the
       height as its sprite.y (MapPartyRingFollow, MapPartyRecordSpotSet,
       MoveChrWalkStep, MoveChrPartyJoin, MenuClose), a twin gets it as both
       (MapPartyTwinSpotSet, MapPartyRecordSpotSet). */
    u16 y;
    /* +0x04 the height above the ground in pixels: 0 from MapPartyWalkStep's
       push and MapPartySpotSet's fill, the row's height from MoveChrWalkStep
       (MapPartyRingPush, and the entry it builds for MapPartyTwinSpotSet) and
       from MoveCommandPlace (MapPartyRingFill). Taken off the y for a record's
       drawn y (above), and as a row's height or goal height (MoveChrPartyJoin,
       MoveChrFollowStart, MoveChrFollowRestart,
       ScriptCommandMoveChrPartyFollowEnd, MoveChrPartyLeave). */
    u8 height;
    /* +0x05 the facing, 1 to 8, the record had there: given to
       MapAnimationPoseSet with pose 1 (MapPartyRingFollow,
       MapPartyTwinSpotSet, MapPartyRecordSpotSet) or with a show's pose
       (MapFadePartyIn, MapRow2Change, MapPartyPose4Play, MapPartyOnPlay,
       MapObjectKind7Run, MenuClose, MoveChrFollowStart, MoveChrFollowRestart);
       MapPartyWalkStep and MapPartyTalkCheck write and read the head entry's
       at +5 directly; MapPartySpotSet reads only x, y and this byte of the
       spot it is given (MapChange and MapPartyPlace put their row's facing
       byte + 1 there). */
    u8 facing;
    /* +0x06 the class MapAttributeClassGet answered for the party's box on the
       spot, 0x2000 or 0x4000 (MapPartyRingFill, MapPartyRingPush;
       MoveChrWalkStep for the entry it builds): put in bits 0x6000 of a
       record's sprite.flags (MapPartyRingFollow, MapPartyTwinSpotSet,
       MoveChrWalkStep, MoveCommandPlace, MapFadePartyIn, MapFadePartyOut,
       MenuClose). Not set by MapChange and MapPartyPlace, whose 6-byte spots
       stop before it. */
    u16 class;
} MapPartyRingEntry;

LAYOUT_SIZE(MapPartyRingEntry, 0x08);
LAYOUT_AT(MapPartyRingEntry, x, x, 0x00);
LAYOUT_AT(MapPartyRingEntry, y, y, 0x02);
LAYOUT_AT(MapPartyRingEntry, height, height, 0x04);
LAYOUT_AT(MapPartyRingEntry, facing, facing, 0x05);
LAYOUT_AT(MapPartyRingEntry, class, class, 0x06);

/* A spot, an x and a y in the map's pixels (the glossary's spot):
   MapViewGoalGet and MapSecondViewGoalGet write the view's goal into one
   (MapViewStep, MapViewScrollSet and MoveChrFollowStart hand them one on their
   stack), and MapAttributeLineCheck takes the two ends of its line as two (its
   callers hand it the address of a record's x, whose ground y follows). */
typedef struct {
    /* +0x00 x in the map's pixels: MapSecondViewGoalGet stores the second
       view's goal x by the effect's mode (the view, half or twice the view, or
       the second view + the view's move + its drift), MapViewGoalGet the
       view's x that puts the first party record 0xA0 across, held in the
       limits. MapAttributeLineCheck reads the two ends' x. */
    s16 x;
    /* +0x02 y in the map's pixels (for a record, the ground y at +8 after x at
       +6), written and read with x. */
    s16 y;
} MapSpot;

LAYOUT_SIZE(MapSpot, 0x04);
LAYOUT_AT(MapSpot, x, x, 0x00);
LAYOUT_AT(MapSpot, y, y, 0x02);

/* One of the map's object definitions, 0x80 of 0x26 bytes at the address in
   g_MapObjectDefinitions (0x801E8900, GameStart), which MapObjectsDataLoad
   unpacks from a set of map objects' resource: what a map object is made from
   (MapObjectInit; MapObjectRestore for a saved game), and what its thinks, its
   talk, its touch and the script read again through the object's definition
   number (record.hold_definition.definition). The step lists at
   g_MapObjectStepLists follow the 0x80 definitions. */
typedef struct {
    /* +0x00 bits 0x3F the object's kind less 1 (MapObjectInit and
       MapObjectsInit make the kind (byte & 0x3F) + 1; MoveChrPartyJoin and
       MoveChrPartyLeave test that below 4 for the story bit's form;
       MapObjectDefinitionSpotFind answers it); bit 0x40 the object turns one
       facing at a time, which MapObjectInit puts into its number; bit 0x80 the
       object's layer (MapObjectInit and MapObjectRestore: layer = byte &
       0x80). */
    u8 kind;
    /* +0x01 the object's sprite data number, 0 for no object (MapObjectsInit
       passes over it): MapObjectInit and MapObjectRestore set the object's
       sprite data from it (0 instead for kind 7), MapObjectsSpriteDataSet
       again; a number below 0xF has a twin with g_MapTwinSpriteDataNumbers[it
       - 1]. For a member who stays behind as an object it is the member's id:
       MoveChrPartyLeave matches it with g_PartyIds, MoveChrRowIdGet answers it
       as a row's object's id. MapObjectDefinitionSpotFind looks for it;
       MapObjectPartySeenCheck plays sound D_800B5B8C[it - 0x1F];
       MapObjectPartySeen03Step compares another object's with
       seen_target.overlap.sprite_data. */
    u8 sprite_data;
    /* +0x02 the object's story bit, 0 for none: for kinds 1 to 3 bit 0x580 +
       it, for the others bit (definition number + 0x680 + ((g_MapNumber -
       0x65) << 7)); while it is set the object is not made (MapObjectsInit,
       MapObjectDefinitionSpotFind). MapObjectOverlappedRemove sets it when it
       takes the object off; MoveChrPartyJoin sets and MoveChrPartyLeave clears
       the first form. */
    u8 story_bit;
    /* +0x03 MapRecordInit's mode for the object and its twin (1: bit 0x800 in
       sprite.flags, 2: 0xC00, 3: flags 0x2000 and attr bit 0x8000):
       MapObjectInit, MapObjectRestore. ScriptCommandMixGroupClear reads it
       again to put bits 0x800 or 0xC00 back into a row's actor's flags when it
       takes the row out of a mix group. */
    u8 mode;
    /* +0x04 the object's x in units of 8 pixels: MapObjectInit makes sprite.x
       it * 8; MapObjectPartyLook03Step walks the object back to it * 8;
       MapObjectDefinitionSpotFind answers it. */
    u8 x;
    /* +0x05 its y in units of 8 pixels: MapObjectInit makes both y words it *
       8; read with x. */
    u8 y;
    /* +0x06 the object's facing less 1: MapObjectInit makes the facing it + 1
       (and turns the twin to it + 1, as MapObjectRestore does);
       MapObjectPartyLook03Step turns the object back to it + 1 at home;
       MapObjectPartyLook0AStep tests its bit 0 (an even facing). */
    u8 facing;
    /* +0x07..+0x0A the object's box (MapBox, map.h), copied whole into the
       object's box (+0x50) by MapObjectInit and MapObjectRestore (the
       definition's, not the save's). At the odd +7, which is why MapBox has
       byte members only (alignment 1). */
    MapBox box;
    /* +0x0B a union: what the object has for the party, by its kind; 0 for
       nothing. label: kinds 1 to 3, talked to (MapPartyTalkCheck turns the
       object to the party and runs ScriptLabelRun(it)), and kind 4, touched
       (MapObjectTouchRun runs ScriptLabelRun(it) after
       MapObjectTouchApproach). btl_number: kinds 5 and 6, touched:
       MapObjectTouchRun runs the battle numbered it (MapBtlRun(it, 0, 0, 0, 1,
       1, 1)). contents: kind 7, through MapPartyTalkCheck and
       MapObjectKind7Run: 0 nothing (message 0xF64C), 1 to 0xB6 the item given
       (ScriptCommandChrItemGive), 0xB7 to 0xEF the money of entry it - 0xB7 of
       D_800B4F60 (ScriptCommandMoneyAdd), 0xF0 and up the script label it +
       0x230. */
    union {
        /* kinds 1 to 3 (talked to) and 4 (touched): the script label run */
        u8 label;
        /* kinds 5 and 6 (touched): the number of the battle run */
        u8 btl_number;
        /* kind 7: an item, an amount of money or a label */
        u8 contents;
    } event;
    /* +0x0C the low six bits of the object's number, its think: MapObjectInit
       makes the number (kind & 0x40) | it; MapObjectNumberReset puts it back
       into the low six bits (bits 0xC0 kept) when a look or a seen step ends. */
    u8 number;
    /* +0x0D the object's speed class: for numbers 0 and 6 MapObjectMoveSet
       takes g_MoveSpeedPixels and g_MoveSpeedIntervals at it + 2 for kinds 4
       to 6 and at it + 3 for the others into the object's speed and interval. */
    u8 speed;
    /* +0x0E the left bound from the object's x, in units of 8 pixels, signed:
       MapObjectMoveSet (numbers 0 and 6, first set) makes move.bounds.left x +
       it * 8, not below 0. */
    s8 bound_left;
    /* +0x0F the right bound: move.bounds.right = x + (it + 1) * 8, at most
       0x7FF (MapObjectMoveSet). */
    u8 bound_right;
    /* +0x10 the top bound from the object's drawn y, signed: move.bounds.top =
       sprite.y + it * 8, not below 0 (MapObjectMoveSet). */
    s8 bound_top;
    /* +0x11 the bottom bound: move.bounds.bottom = sprite.y + (it + 1) * 8, at
       most 0x7FF (MapObjectMoveSet). */
    u8 bound_bottom;
    /* +0x12 for numbers 1 and 2, the number of the object's step list in the
       table of 16-bit places at g_MapObjectStepLists: MapObjectMoveSet points
       the object's step_list at the list it picks. */
    u8 step_list;
    /* +0x13 how far beyond its box the object touches the party, in units of 8
       pixels: MapObjectTouchFind grows both half sizes of a copy of the
       object's box by it * 8. */
    u8 touch_reach;
    /* +0x14..+0x18 the five bytes MapObjectMoveSet copies, for numbers 0 and 6
       with its flag 0 (MapObjectInit, MapObjectRestore), into the object's
       look, sight_width, sight_length, length and count (+0x43..+0x47), read
       one after another through a byte pointer. */
    u8 look_set[5];
    /* +0x19..+0x1D the second set of the five, which MapObjectMoveSet copies
       with its flag 1 (MapObjectNumberReset, when a look or a seen step ends),
       the bounds kept. */
    u8 second_look_set[5];
    /* +0x1E what the object does once it has seen the party: bits 0x7F the
       seen step, 1 to 6 (MapObjectPartySeenStep calls entry it - 1 of
       D_800B5B74, MapObjectPartySeen01Step to MapObjectPartySeen06Step), 0 for
       an object that does not look for the party (MapObjectPartySeenCheck), 6
       also puts it on number 8; bit 0x80 it sees in a square round itself
       instead of a box ahead (MapObjectPartySeenCheck). */
    u8 seen;
    /* +0x1F for number 7, how wide it sees, in units of 4 pixels (with bit
       0x80 of seen, the square's half size in units of 8):
       MapObjectPartySeenCheck, as the object's sight_width serves number 6. */
    u8 seen_sight_width;
    /* +0x20 for number 7, how far it sees, in units of 4 pixels
       (MapObjectPartySeenCheck), as the object's sight_length serves number 6. */
    u8 seen_sight_length;
    /* +0x21 the seen steps' length in place of a drawn one (rng_range_draw's
       fixed value; 0 draws one): a wait in units of 0x1E passes (0 to 2 drawn:
       MapObjectPartySeen01Step, MapObjectPartySeen03Step,
       MapObjectPartySeen05Step, MapObjectPartySeen06Step) or a walk in units
       of 8 pixels (8 to 0x10 drawn: MapObjectPartySeen04Step), as the object's
       length serves the looks. */
    u8 seen_length;
    /* +0x22 the speed class once the object has seen the party:
       MapObjectPartySeenCheck sets its speed, interval and interval_left from
       g_MoveSpeedPixels and g_MoveSpeedIntervals at it + 2. */
    u8 seen_speed;
    /* +0x23..+0x24 a union, by the seen step. goal, seen 2
       (MapObjectPartySeen02Step): the spot the object walks to, x and y in
       units of 8 pixels (target.spot = them * 8); x not 0 also has it run the
       label of a kind 4 attribute under its box at the goal
       (MapAttributeFind). overlap, seen 3 (MapObjectPartySeen03Step): action,
       what is done to an object that stops the walk, when that object's
       definition's sprite_data is sprite_data (1 puts it on number 4, 2 turns
       it to facing 1; 0 nothing), and MapObjectOverlappedRemove later takes it
       off. */
    union {
        /* seen 2: the spot walked to, in units of 8 pixels */
        struct {
            u8 x;
            u8 y;
        } goal;
        /* seen 3: what is done to the object that stops the walk, and the
           sprite data number its definition must have */
        struct {
            u8 action;
            u8 sprite_data;
        } overlap;
    } seen_target;
    /* +0x25 the party's least level the object stands up to:
       MapObjectPartySeenCheck puts an object that sees the party on number 8
       (MapObjectPartyAwayStep, away from the party) when it is below
       g_PartyLevelLeast or seen & 0x7F is 6, on number 7 otherwise. */
    u8 level;
} MapObjectDefinition;

LAYOUT_SIZE(MapObjectDefinition, 0x26);
LAYOUT_AT(MapObjectDefinition, kind, kind, 0x00);
LAYOUT_AT(MapObjectDefinition, sprite_data, sprite_data, 0x01);
LAYOUT_AT(MapObjectDefinition, story_bit, story_bit, 0x02);
LAYOUT_AT(MapObjectDefinition, mode, mode, 0x03);
LAYOUT_AT(MapObjectDefinition, x, x, 0x04);
LAYOUT_AT(MapObjectDefinition, y, y, 0x05);
LAYOUT_AT(MapObjectDefinition, facing, facing, 0x06);
LAYOUT_AT(MapObjectDefinition, box, box, 0x07);
LAYOUT_AT(MapObjectDefinition, box_dx, box.dx, 0x07);
LAYOUT_AT(MapObjectDefinition, box_dy, box.dy, 0x08);
LAYOUT_AT(MapObjectDefinition, box_half_width, box.half_width, 0x09);
LAYOUT_AT(MapObjectDefinition, box_half_height, box.half_height, 0x0a);
LAYOUT_AT(MapObjectDefinition, event, event, 0x0b);
LAYOUT_AT(MapObjectDefinition, event_label, event.label, 0x0b);
LAYOUT_AT(MapObjectDefinition, event_btl_number, event.btl_number, 0x0b);
LAYOUT_AT(MapObjectDefinition, event_contents, event.contents, 0x0b);
LAYOUT_AT(MapObjectDefinition, number, number, 0x0c);
LAYOUT_AT(MapObjectDefinition, speed, speed, 0x0d);
LAYOUT_AT(MapObjectDefinition, bound_left, bound_left, 0x0e);
LAYOUT_AT(MapObjectDefinition, bound_right, bound_right, 0x0f);
LAYOUT_AT(MapObjectDefinition, bound_top, bound_top, 0x10);
LAYOUT_AT(MapObjectDefinition, bound_bottom, bound_bottom, 0x11);
LAYOUT_AT(MapObjectDefinition, step_list, step_list, 0x12);
LAYOUT_AT(MapObjectDefinition, touch_reach, touch_reach, 0x13);
LAYOUT_AT(MapObjectDefinition, look_set, look_set, 0x14);
LAYOUT_AT(MapObjectDefinition, second_look_set, second_look_set, 0x19);
LAYOUT_AT(MapObjectDefinition, seen, seen, 0x1e);
LAYOUT_AT(MapObjectDefinition, seen_sight_width, seen_sight_width, 0x1f);
LAYOUT_AT(MapObjectDefinition, seen_sight_length, seen_sight_length, 0x20);
LAYOUT_AT(MapObjectDefinition, seen_length, seen_length, 0x21);
LAYOUT_AT(MapObjectDefinition, seen_speed, seen_speed, 0x22);
LAYOUT_AT(MapObjectDefinition, seen_target, seen_target, 0x23);
LAYOUT_AT(MapObjectDefinition, seen_target_goal, seen_target.goal, 0x23);
LAYOUT_AT(MapObjectDefinition, seen_target_goal_x, seen_target.goal.x, 0x23);
LAYOUT_AT(MapObjectDefinition, seen_target_goal_y, seen_target.goal.y, 0x24);
LAYOUT_AT(MapObjectDefinition, seen_target_overlap, seen_target.overlap, 0x23);
LAYOUT_AT(MapObjectDefinition, seen_target_overlap_action, seen_target.overlap.action, 0x23);
LAYOUT_AT(MapObjectDefinition, seen_target_overlap_sprite_data, seen_target.overlap.sprite_data, 0x24);
LAYOUT_AT(MapObjectDefinition, level, level, 0x25);

/* An entry of the sprite data table, 0x100 entries of 0x10 bytes at the
   address in g_MapSpriteDataEntries (0x801EA300, GameStart), one for each
   sprite data number: the address of that number's unpacked sprite data, 0 for
   none, 1 while it waits to be loaded again. Entry 0 is g_SpriteData1
   (MapObjectsDataLoad). */
typedef struct {
    /* +0x00 the address of the number's unpacked sprite data, 0 for none, 1
       between MapSpriteDataReload's first loop and its last for one to load
       again. MapPartySpriteDataLoad and MapObjectsDataLoad store the block
       xmalloc_heap_alloc gave and inflate_buffer filled (MapObjectsDataLoad
       also g_SpriteData1 in entry 0); MapPartySpriteDataReload and
       MapObjectsSpriteDataReload do so for the entries at 1;
       MapSpriteDataReload marks the loaded ones 1 and zeroes those still 1
       after; MapSpriteDataFree frees every loaded one whose mark is set and
       zeroes it; MoveChrEnd zeroes the entries whose mark is 1;
       MapRecordsClear zeroes the table. MapRecordSpriteDataSet takes a
       record's five addresses from it; MapSpriteDataLoadedMark,
       MapPartySpriteDataMissingCount, MapPartyImagesSend,
       ScriptCommandMapObjectsAdd and ScriptCommandMoveChrRowActorSet read it. */
    s32 data;
    /* +0x04..+0x07 nothing reads or writes it but MapRecordsClear's clear of
       the table. */
    u8 pad_04[4];
    /* +0x08 MoveChrEnd zeroes it with +0 and +0xC for each number whose mark
       is 1; nothing reads it and nothing else writes it. */
    s32 unk_08;
    /* +0x0C zeroed with +8 by MoveChrEnd; nothing reads it. */
    s32 unk_0c;
} MapSpriteDataEntry;

LAYOUT_SIZE(MapSpriteDataEntry, 0x10);
LAYOUT_AT(MapSpriteDataEntry, data, data, 0x00);
LAYOUT_AT(MapSpriteDataEntry, pad_04, pad_04, 0x04);
LAYOUT_AT(MapSpriteDataEntry, unk_08, unk_08, 0x08);
LAYOUT_AT(MapSpriteDataEntry, unk_0c, unk_0c, 0x0c);

/* A row of the list of sprite data in a set of map objects' resource (the
   resource the set's 16-bit word of D_800B4E2C names; the list's place is the
   resource's second word): a sprite data number, its packed data's length and
   its place, ended by a number of 0. MapObjectsDataLoad unpacks each number
   not yet loaded into the sprite data table; MapObjectsSpriteDataReload those
   marked 1. */
typedef struct {
    /* +0x00 the sprite data number, 0 to end the list: MapObjectsDataLoad
       loads it when its entry of g_MapSpriteDataEntries is 0 and it is below
       0x100; MapObjectsSpriteDataReload when the entry is 1 (numbers below
       0x1F passed over for skip_low). */
    u16 sprite_data;
    /* +0x02 the unpacked data's length in units of 4 bytes
       (xmalloc_heap_alloc(it * 4)). */
    u16 length;
    /* +0x04 the packed data's place in units of 4 bytes from the list's start
       (g_ResourceReadBuffer + hdr[1] + it * 4). */
    u16 place;
} MapObjectsSpriteDataRow;

LAYOUT_SIZE(MapObjectsSpriteDataRow, 0x06);
LAYOUT_AT(MapObjectsSpriteDataRow, sprite_data, sprite_data, 0x00);
LAYOUT_AT(MapObjectsSpriteDataRow, length, length, 0x02);
LAYOUT_AT(MapObjectsSpriteDataRow, place, place, 0x04);

/* A map's 16-byte record, row (D_800B5198[map]) of the 124 at D_800B5238 that
   MapTblGet gives for a map's number: what the map is and what its loading
   sets up. tbl is the developers' word for it (MapEffectSet's line prints its
   address under tbl=). MapSet copies it to its stack and changes the music and
   script there; MapLoad and the developers' MapEffectSet read it; MapSeek and
   MapTblByte7Get read it too. */
typedef struct {
    /* +0x00 the map's party kind, which MapSet makes g_MapPartyKind (0: the
       party's records take sprite data number + 0xF; 2: every party record on
       place 0's ring entry); MapSet and MapSeek take music 7 for music 6 when
       it is 0 and story bit 0x3C1 is set. */
    u8 party_kind;
    /* +0x01..+0x03 nothing reads or writes them on their own; they go with
       MapSet's and MapSeek's copies of the record. */
    u8 pad_01[3];
    /* +0x04 the map's tile animation set, which MapLoad hands to
       tile_animation_set. */
    u8 tile_animation;
    /* +0x05 the map's palette animation set, which MapLoad hands to
       palette_animation_set. */
    u8 palette_animation;
    /* +0x06 the number of the map's place name, which MapSet makes
       g_MapPlaceName, setting story bit it + 0x28A for 0x51 to 0x64. */
    u8 place_name;
    /* +0x07 read only by MapTblByte7Get, which nothing calls; what it holds
       was not read. */
    u8 unk_07;
    /* +0x08 the number of the map's script: MapSet puts its argument there
       when that is not 0, and MapLoad hands it to ScriptChange when sw is 1. */
    u8 script;
    /* +0x09 nothing reads or writes it on its own. */
    u8 pad_09;
    /* +0x0A not 0: MapSet sets story bit 0x2D3, 0: it clears it (what the bit
       means was not read). */
    u8 story_bit_2d3;
    /* +0x0B the map's effect flags, the developers' mapeffect: MapEffectSet
       makes g_MapEffect of it when sw is 1 (bits 3 the second view's mode, 0xC
       a row of D_80012C98, 0x30 the colour, 0x40 the bg's mode 1, 0x80 layer
       4's mix); MapLoad hands it to bg_effect_flags_set, a store nothing
       reads. */
    u8 effect;
    /* +0x0C for effect mode 3, the second view's drift: the high four bits the
       x speed and the low four the y, as entries of D_80012C8C (-8 to 8) for
       g_MapSecondViewSpeedX and g_MapSecondViewSpeedY (MapEffectSet). */
    u8 second_view_speeds;
    /* +0x0D bits 0 to 4 the row of D_80012C98 for effect bits 0xC
       (MapEffectSet keeps it in D_800B8D1E); bits 5 to 7 for effect bit 0x80,
       layer 4's mix level over 4 (g_MapLayer4Mix = (it >> 5) << 2). */
    u8 effect_row_mix;
    /* +0x0E for effect bits 0x30, the colour: for 0x10 MapColSet(bits 4 to 6,
       the low four bits), a row of D_800B3300 and a scale, unless the row is
       0; for 0x20 and 0x30 ColMix(the low four bits, 0). */
    u8 col;
    /* +0x0F the map's music: MapSet puts its argument there when that is not
       0xFF and makes g_MusicNumber of it (7 for 6 with party kind 0 and story
       bit 0x3C1); MapSeek takes it for a music of 0xFF. */
    u8 music;
} MapTbl;

LAYOUT_SIZE(MapTbl, 0x10);
LAYOUT_AT(MapTbl, party_kind, party_kind, 0x00);
LAYOUT_AT(MapTbl, pad_01, pad_01, 0x01);
LAYOUT_AT(MapTbl, tile_animation, tile_animation, 0x04);
LAYOUT_AT(MapTbl, palette_animation, palette_animation, 0x05);
LAYOUT_AT(MapTbl, place_name, place_name, 0x06);
LAYOUT_AT(MapTbl, unk_07, unk_07, 0x07);
LAYOUT_AT(MapTbl, script, script, 0x08);
LAYOUT_AT(MapTbl, pad_09, pad_09, 0x09);
LAYOUT_AT(MapTbl, story_bit_2d3, story_bit_2d3, 0x0a);
LAYOUT_AT(MapTbl, effect, effect, 0x0b);
LAYOUT_AT(MapTbl, second_view_speeds, second_view_speeds, 0x0c);
LAYOUT_AT(MapTbl, effect_row_mix, effect_row_mix, 0x0d);
LAYOUT_AT(MapTbl, col, col, 0x0e);
LAYOUT_AT(MapTbl, music, music, 0x0f);

/* A row of the map's enter table, the 11-byte rows at the address in
   g_MapEnterTable (0x801E6B00, GameStart; resource_map_enter_table_set fills
   it from the map's resource when a map is entered) by which a map is entered
   from another: MapChange takes row n of it (or, for 0, a row
   ScriptCommandMapChange builds on its stack from its command). */
typedef struct {
    /* +0x00 the number of the map entered (MapSeek, MapSet). MapSet tests the
       table's byte 0x16, row 2's map number, for story bit 0x2D2. */
    u8 map_number;
    /* +0x01 the number of the set of map objects the map is entered with
       (MapObjectsInit). */
    u8 object_set;
    /* +0x02 the fade out: 1 fades to MapFadeOut's target -2 instead of -1, 2
       sets g_MapFadeState to 2 (the party's own fade, MapFadePartyOut and
       MapFadePartyIn). */
    u8 fade;
    /* +0x03 a sound played first (SoundPlay(it, 3)), 0 for none. */
    u8 sound;
    /* +0x04 the view's left limit in units of 8 pixels (g_MapLimitLeft = it *
       8). */
    u8 limit_left;
    /* +0x05 the view's right limit, the last column (g_MapLimitRight = (it +
       1) * 8). */
    u8 limit_right;
    /* +0x06 the view's top limit (g_MapLimitTop = it * 8). */
    u8 limit_top;
    /* +0x07 the view's bottom limit, the last row (g_MapLimitBottom = (it + 1)
       * 8). */
    u8 limit_bottom;
    /* +0x08 the x of the spot the party is put on, in units of 8 pixels
       (MapPartySpotSet). */
    u8 x;
    /* +0x09 the spot's y in units of 8 pixels. */
    u8 y;
    /* +0x0A the party's facing there, less 1 (the spot's facing = it + 1). */
    u8 facing;
} MapEnterRow;

LAYOUT_SIZE(MapEnterRow, 0x0b);
LAYOUT_AT(MapEnterRow, map_number, map_number, 0x00);
LAYOUT_AT(MapEnterRow, object_set, object_set, 0x01);
LAYOUT_AT(MapEnterRow, fade, fade, 0x02);
LAYOUT_AT(MapEnterRow, sound, sound, 0x03);
LAYOUT_AT(MapEnterRow, limit_left, limit_left, 0x04);
LAYOUT_AT(MapEnterRow, limit_right, limit_right, 0x05);
LAYOUT_AT(MapEnterRow, limit_top, limit_top, 0x06);
LAYOUT_AT(MapEnterRow, limit_bottom, limit_bottom, 0x07);
LAYOUT_AT(MapEnterRow, x, x, 0x08);
LAYOUT_AT(MapEnterRow, y, y, 0x09);
LAYOUT_AT(MapEnterRow, facing, facing, 0x0a);

/* A row of the 9-byte rows at the address in D_800B8D00 (0x801E7080,
   GameStart) by which MapPartyPlace puts the party on another spot of the map
   it is on, with the draw area closed before and opened after (or, for row 0,
   a row ScriptCommandMapPartyPlace builds on its stack from its command):
   MapChange's row without the map. */
typedef struct {
    /* +0x00 a byte MapPartyPlace only hands on, as the third argument of
       MapDrawAreaClose and MapDrawAreaOpen, which do not read it. */
    u8 unk_00;
    /* +0x01 a sound played first (SoundPlay(it, 3)), 0 for none. */
    u8 sound;
    /* +0x02 the view's left limit in units of 8 pixels. */
    u8 limit_left;
    /* +0x03 the right limit, the last column ((it + 1) * 8). */
    u8 limit_right;
    /* +0x04 the top limit (it * 8). */
    u8 limit_top;
    /* +0x05 the bottom limit, the last row ((it + 1) * 8). */
    u8 limit_bottom;
    /* +0x06 the x of the spot the party is put on, in units of 8 pixels
       (MapPartySpotSet). */
    u8 x;
    /* +0x07 the spot's y in units of 8 pixels. */
    u8 y;
    /* +0x08 the party's facing there, less 1. */
    u8 facing;
} MapPartyPlaceRow;

LAYOUT_SIZE(MapPartyPlaceRow, 0x09);
LAYOUT_AT(MapPartyPlaceRow, unk_00, unk_00, 0x00);
LAYOUT_AT(MapPartyPlaceRow, sound, sound, 0x01);
LAYOUT_AT(MapPartyPlaceRow, limit_left, limit_left, 0x02);
LAYOUT_AT(MapPartyPlaceRow, limit_right, limit_right, 0x03);
LAYOUT_AT(MapPartyPlaceRow, limit_top, limit_top, 0x04);
LAYOUT_AT(MapPartyPlaceRow, limit_bottom, limit_bottom, 0x05);
LAYOUT_AT(MapPartyPlaceRow, x, x, 0x06);
LAYOUT_AT(MapPartyPlaceRow, y, y, 0x07);
LAYOUT_AT(MapPartyPlaceRow, facing, facing, 0x08);

/* A row of the map's bg change table (the glossary's bg change), at the
   address in g_MapBgChangeTable (0x801E7500, GameStart) after the table's
   16-bit words stored high byte first (u16_bytes_swap): word n, when it is not
   0, is the place of change n's row counted in 16-bit words from the table's
   start. A rectangle of the map's bg and attributes, in units of 8 pixels,
   that bg_both_copy copies over from each of the row's sources in turn, some
   frames apart. MapBgChangeStart plays a row from its first source,
   MapBgChangeStoryBitsCopy copies the last source of a row whose story bit is
   set, and MapBgChangeRowSet copies the rectangle into the record
   (MapBgChange). Nothing writes a row: the table is the map's resource. */
typedef struct {
    /* +0x00 the destination's x, the record's dx (MapBgChangeRowSet). */
    u8 dx;
    /* +0x01 the destination's y, the record's dy (MapBgChangeRowSet). */
    u8 dy;
    /* +0x02 the rectangle's width, the record's width (MapBgChangeRowSet). */
    u8 width;
    /* +0x03 the rectangle's height, the record's height
       (MapBgChangeRowSet). */
    u8 height;
    /* +0x04 the change's story bit less 0x480, 0 for none: MapBgChangeStart
       sets bit it + 0x480 with StoryBitSet and starts nothing when it was set
       already, so such a change runs once; MapBgChangeStoryBitsCopy copies the
       last source of a row whose bit is set (StoryBitCheck). */
    u8 story_bit;
    /* +0x05 the number of sources that follow at +8: the record's
       source_count (MapBgChangeRowSet); MapBgChangeStoryBitsCopy finds the
       last source at row + 6 + it * 2 and makes the record's copies it - 1. */
    u8 source_count;
    /* +0x06 the frames between two copies, the record's interval
       (MapBgChangeStart). */
    u8 interval;
    /* +0x07 a sound played when the copies begin, 0 for none: the record's
       sound (MapBgChangeStart). */
    u8 sound;
    /* +0x08.. the sources, source_count pairs of bytes, an x and a y each:
       the record's source points into them (MapBgChangeStart at the first,
       MapBgChangeStoryBitsCopy at the last, MapBgChangeStep moves it on by
       2), and MapBgChangeRowSet and MapBgChangeStep take the pair there as the
       record's sx and sy. The row's length is the count's: the array has no
       size of its own. */
    u8 sources[0];
} MapBgChangeRow;

LAYOUT_SIZE(MapBgChangeRow, 0x08);
LAYOUT_AT(MapBgChangeRow, dx, dx, 0x00);
LAYOUT_AT(MapBgChangeRow, dy, dy, 0x01);
LAYOUT_AT(MapBgChangeRow, width, width, 0x02);
LAYOUT_AT(MapBgChangeRow, height, height, 0x03);
LAYOUT_AT(MapBgChangeRow, story_bit, story_bit, 0x04);
LAYOUT_AT(MapBgChangeRow, source_count, source_count, 0x05);
LAYOUT_AT(MapBgChangeRow, interval, interval, 0x06);
LAYOUT_AT(MapBgChangeRow, sound, sound, 0x07);
LAYOUT_AT(MapBgChangeRow, sources, sources, 0x08);

/* The bg change's record (the glossary's), g_MapBgChange (0x800E08F8): the one
   bg change that runs, played from a row of the bg change table
   (MapBgChangeRow). MapBgChangeStart fills it, MapBgChangeStep counts its
   frames once a frame and moves it to the next source, MapBgChangeCopy makes
   the copy that is due with bg_both_copy, and MapBgChangeStoryBitsCopy fills
   it with a row's last source for a change made before. 0x16 bytes used; the
   next global (g_RngNumbers) is at +0x18. */
typedef struct {
    /* +0x00 the number of the change that runs + 1, 0 for none:
       MapBgChangeStart stores it, MapBgChangeStep zeroes it after the last
       copy, MapChange, GameEnter and MenuUseMoviePlay zero it
       (MapBgChangeStoryBitsCopy leaves it). MapBgChangeStep does nothing while
       it is 0 and keeps it - 1 in g_MapBgChangeNumber when the delay runs out;
       MapBgChangeEndWait waits for 0; MapAttributeRun starts a change only
       when it is 0 or the delay is not. */
    u8 number;
    /* +0x01 1 while a copy is due: MapBgChangeStart sets it for a delay of 0,
       MapBgChangeStep when the delay runs out and at each next source,
       MapBgChangeStoryBitsCopy for its copy; MapBgChangeCopy makes the copy
       and zeroes it, and MapChange, GameEnter and MenuUseMoviePlay zero it.
       MapBgChangeStep does nothing while it is set. */
    u8 copy_due;
    /* +0x02 the row's number of sources (MapBgChangeRowSet): the change is
       over when copies reaches it (MapBgChangeStep). */
    u8 source_count;
    /* +0x03 the copies made: 0 from MapBgChangeStart, source_count - 1 from
       MapBgChangeStoryBitsCopy; MapBgChangeStep adds 1 at each copy. */
    u8 copies;
    /* +0x04 the frames between two copies, the row's interval
       (MapBgChangeStart): MapBgChangeStep moves on once frames reaches it. */
    u8 interval;
    /* +0x05 the frames counted since the last copy: zeroed by
       MapBgChangeStart and by MapBgChangeStep at each copy, counted by
       MapBgChangeStep. */
    u8 frames;
    /* +0x06 the rectangle's height, the row's (MapBgChangeRowSet):
       bg_both_copy's h (MapBgChangeCopy). */
    u8 height;
    /* +0x07 the rectangle's width, the row's (MapBgChangeRowSet):
       bg_both_copy's w. */
    u8 width;
    /* +0x08 the destination's x, the row's dx (MapBgChangeRowSet):
       bg_both_copy's dx. */
    u16 dx;
    /* +0x0A the destination's y, the row's dy: bg_both_copy's dy. */
    u16 dy;
    /* +0x0C the current source's x, the first byte of the pair at source:
       MapBgChangeRowSet, and MapBgChangeStep for the next source (a signed
       16-bit store); bg_both_copy's sx. */
    u16 sx;
    /* +0x0E the current source's y, the pair's second byte (the same
       writers): bg_both_copy's sy. */
    u16 sy;
    /* +0x10 the address of the current source in the row's sources:
       MapBgChangeStart stores the first, MapBgChangeStoryBitsCopy the last
       (row + 6 + source_count * 2), MapBgChangeStep moves it on by 2;
       MapBgChangeRowSet and MapBgChangeStep read the pair there. */
    u8 * source;
    /* +0x14 the row's sound (MapBgChangeStart), 0 for none: SoundPlay(it, 3)
       when the copies begin (MapBgChangeStart for a delay of 0,
       MapBgChangeStep when the delay runs out). */
    u8 sound;
    /* +0x15 the frames before the copies begin, MapBgChangeStart's delay:
       MapBgChangeStep counts it down; MapAttributeRun reads it (see
       number). */
    u8 delay;
    /* +0x16..+0x17 nothing reads or writes them. */
    u8 pad_16[2];
} MapBgChange;

LAYOUT_SIZE(MapBgChange, 0x18);
LAYOUT_AT(MapBgChange, number, number, 0x00);
LAYOUT_AT(MapBgChange, copy_due, copy_due, 0x01);
LAYOUT_AT(MapBgChange, source_count, source_count, 0x02);
LAYOUT_AT(MapBgChange, copies, copies, 0x03);
LAYOUT_AT(MapBgChange, interval, interval, 0x04);
LAYOUT_AT(MapBgChange, frames, frames, 0x05);
LAYOUT_AT(MapBgChange, height, height, 0x06);
LAYOUT_AT(MapBgChange, width, width, 0x07);
LAYOUT_AT(MapBgChange, dx, dx, 0x08);
LAYOUT_AT(MapBgChange, dy, dy, 0x0a);
LAYOUT_AT(MapBgChange, sx, sx, 0x0c);
LAYOUT_AT(MapBgChange, sy, sy, 0x0e);
LAYOUT_AT(MapBgChange, source, source, 0x10);
LAYOUT_AT(MapBgChange, sound, sound, 0x14);
LAYOUT_AT(MapBgChange, delay, delay, 0x15);
LAYOUT_AT(MapBgChange, pad_16, pad_16, 0x16);

/* The script command MapObjectKind7Run builds on its stack and points
   g_ScriptCommandPointer at, to have a kind 7 object give what it holds
   through the script's own handlers: the money add (command word 0x1C,
   ScriptCommandMoneyAdd) or the item give (0x1F, ScriptCommandChrItemGive),
   laid out as those handlers read their command. */
typedef struct {
    /* +0x00 the command's word: 0x1C for money, 0x1F for an item (the
       handlers' entries 28 and 31 of the script's table at 0x800B333C); the
       handlers step past it. */
    u16 word;
    /* +0x02 a union, by the command: notice, for 0x1C, the notice's kind k (0
       here: the notice with string 0x65A + 1); item, for 0x1F, the item's
       number (the object's contents). */
    union {
        /* command 0x1C: the notice's kind */
        u8 notice;
        /* command 0x1F: the item given */
        u8 item;
    } notice_item;
    /* +0x03 for 0x1F, the notice's kind k (0 here); ScriptCommandMoneyAdd does
       not read it (its amount starts at the next multiple of 4). */
    u8 item_notice;
    /* +0x04 for 0x1C, the amount of money: entry ev - 0xB7 of the 16-bit table
       D_800B4F60; ScriptCommandMoneyAdd takes the 32-bit word at the next
       multiple of 4. */
    s32 amount;
} MapObjectKind7Command;

LAYOUT_SIZE(MapObjectKind7Command, 0x08);
LAYOUT_AT(MapObjectKind7Command, word, word, 0x00);
LAYOUT_AT(MapObjectKind7Command, notice_item, notice_item, 0x02);
LAYOUT_AT(MapObjectKind7Command, notice_item_notice, notice_item.notice, 0x02);
LAYOUT_AT(MapObjectKind7Command, notice_item_item, notice_item.item, 0x02);
LAYOUT_AT(MapObjectKind7Command, item_notice, item_notice, 0x03);
LAYOUT_AT(MapObjectKind7Command, amount, amount, 0x04);

/* The map's colour: three signed 16-bit numbers, the scalars g_MapCol,
   g_MapColSecond and g_MapColThird, which the code also copies as one 6-byte
   block (through the alias g_MapCol_blk in menu_card.c and menu_list.c, through
   g_MapCol itself in map_change.c) and which the layer code takes as an array
   of three (layer_col_set, layer_col_fade_start get &g_MapCol). */
typedef struct {
    /* +0x00 the colour's first number (g_MapCol): layer_col_apply turns it
       into g_LayerColByte2; MapColSet sets it, MapEffectSet zeroes it,
       GameEnter puts the saved one back, MapBtlRun and MapFrameUpdate set
       -0x38 and MenuUseDrawAreaOpen -100 for a while. Which colour it is was
       not read, so it keeps its place (as the register rows do). */
    s16 first;
    /* +0x02 the second number (g_MapColSecond): layer_col_apply turns it into
       g_LayerColByte1; written with the first by the same functions. */
    s16 second;
    /* +0x04 the third number (g_MapColThird): layer_col_apply turns it into
       g_LayerColByte0; written with the first by the same functions. */
    s16 third;
} MapCol;

LAYOUT_SIZE(MapCol, 0x06);
LAYOUT_AT(MapCol, first, first, 0x00);
LAYOUT_AT(MapCol, second, second, 0x02);
LAYOUT_AT(MapCol, third, third, 0x04);

/* A saved game's entry for one map object, 0x28 bytes: what MenuSaveWrite
   keeps of a MapObject row in use, packed into the save after its 0x80 flag
   bytes at +0xE00 (entries from +0xF80); MenuLoadRead lays them out again, one
   for each of the 0x80 rows (0 for a free row), 0xC00 bytes into its buffer
   (0x801D0700 for 0x801CFB00), and GameEnter hands that list to
   MapObjectsRestore, whose MapObjectRestore makes each row again from its
   entry. The glossary's saved game's entry. */
typedef struct {
    /* +0x00 the object's kind (record.on_kind.kind); MapObjectsRestore passes
       an entry of 0 over (a free row). */
    u8 kind;
    /* +0x01 the number of the object's definition
       (record.hold_definition.definition). */
    u8 definition;
    /* +0x02 the object's x (record.sprite.x). */
    u16 x;
    /* +0x04 the object's ground y (record.sprite.depth). */
    u16 depth;
    /* +0x06 the object's drawn y (record.sprite.y). */
    u16 y;
    /* +0x08 the object's animation number (record.animation); MapObjectRestore
       makes the pose (animation - 1) / 4 + 1 from it for MapAnimationPoseSet. */
    u16 animation;
    /* +0x0A the object's number (MapObject +0x3D). */
    u8 number;
    /* +0x0B the object's facing (+0x3E). */
    u8 facing;
    /* +0x0C the object's speed (+0x3F). */
    u8 speed;
    /* +0x0D the object's interval (+0x41; interval_left at +0x40 is not
       saved). */
    u8 interval;
    /* +0x0E the object's turn_count (+0x42). */
    u8 turn_count;
    /* +0x0F the object's look (+0x43). */
    u8 look;
    /* +0x10 the object's sight_width (+0x44). */
    u8 sight_width;
    /* +0x11 sight_length (+0x45). */
    u8 sight_length;
    /* +0x12 length (+0x46). */
    u8 length;
    /* +0x13 count (+0x47). */
    u8 count;
    /* +0x14 state (+0x48). */
    u8 state;
    /* +0x15 thinks_left (+0x49). */
    u8 thinks_left;
    /* +0x16 facing_kept (+0x4A). */
    u8 facing_kept;
    /* +0x17 turn_plan (+0x4B). */
    u8 turn_plan;
    /* +0x18..+0x1B the object's target words (+0x4C, +0x4E: the spot, or the
       overlapped object in the first), copied as two 16-bit words. */
    u16 target[2];
    /* +0x1C..+0x23 the object's four move words (+0x54 to +0x5A: the bounds,
       or the step under way), copied as 16-bit words. */
    u16 move[4];
    /* +0x24 the address of the object's step list (+0x5C), saved as the
       address itself. */
    s32 step_list;
} MapObjectSaveEntry;

LAYOUT_SIZE(MapObjectSaveEntry, 0x28);
LAYOUT_AT(MapObjectSaveEntry, kind, kind, 0x00);
LAYOUT_AT(MapObjectSaveEntry, definition, definition, 0x01);
LAYOUT_AT(MapObjectSaveEntry, x, x, 0x02);
LAYOUT_AT(MapObjectSaveEntry, depth, depth, 0x04);
LAYOUT_AT(MapObjectSaveEntry, y, y, 0x06);
LAYOUT_AT(MapObjectSaveEntry, animation, animation, 0x08);
LAYOUT_AT(MapObjectSaveEntry, number, number, 0x0a);
LAYOUT_AT(MapObjectSaveEntry, facing, facing, 0x0b);
LAYOUT_AT(MapObjectSaveEntry, speed, speed, 0x0c);
LAYOUT_AT(MapObjectSaveEntry, interval, interval, 0x0d);
LAYOUT_AT(MapObjectSaveEntry, turn_count, turn_count, 0x0e);
LAYOUT_AT(MapObjectSaveEntry, look, look, 0x0f);
LAYOUT_AT(MapObjectSaveEntry, sight_width, sight_width, 0x10);
LAYOUT_AT(MapObjectSaveEntry, sight_length, sight_length, 0x11);
LAYOUT_AT(MapObjectSaveEntry, length, length, 0x12);
LAYOUT_AT(MapObjectSaveEntry, count, count, 0x13);
LAYOUT_AT(MapObjectSaveEntry, state, state, 0x14);
LAYOUT_AT(MapObjectSaveEntry, thinks_left, thinks_left, 0x15);
LAYOUT_AT(MapObjectSaveEntry, facing_kept, facing_kept, 0x16);
LAYOUT_AT(MapObjectSaveEntry, turn_plan, turn_plan, 0x17);
LAYOUT_AT(MapObjectSaveEntry, target, target, 0x18);
LAYOUT_AT(MapObjectSaveEntry, move, move, 0x1c);
LAYOUT_AT(MapObjectSaveEntry, step_list, step_list, 0x24);

/* MapObjectSprites: the map objects' stretch of the sprite list, from its
   entry 0x10 (g_SpriteList + 0x240): the objects' entries and then their
   twins'. MapObjectsAnimationStep copies each object's sprite entry into
   object[n] and each twin's into twin[m]. */
typedef struct {
    /* +0x000 the entries of the map objects, 0x46 of them, one per object row,
       written by MapObjectsAnimationStep. */
    SpriteEntry object[70];
    /* +0x9D8 the entries of the objects' twins, 0x10 of them, written by
       MapObjectsAnimationStep (with g_MapObjectsSpriteBits in their flags). */
    SpriteEntry twin[16];
} MapObjectSprites;

LAYOUT_SIZE(MapObjectSprites, 0xc18);
LAYOUT_AT(MapObjectSprites, object, object, 0x00);
LAYOUT_AT(MapObjectSprites, object_0__on, object[0].on, 0x00);
LAYOUT_AT(MapObjectSprites, object_0__x, object[0].x, 0x02);
LAYOUT_AT(MapObjectSprites, object_0__depth, object[0].depth, 0x04);
LAYOUT_AT(MapObjectSprites, object_0__y, object[0].y, 0x06);
LAYOUT_AT(MapObjectSprites, object_0__attr, object[0].attr, 0x08);
LAYOUT_AT(MapObjectSprites, object_0__flags, object[0].flags, 0x0a);
LAYOUT_AT(MapObjectSprites, object_0__texel_records, object[0].texel_records, 0x0c);
LAYOUT_AT(MapObjectSprites, object_0__pose_table, object[0].pose_table, 0x10);
LAYOUT_AT(MapObjectSprites, object_0__piece_table, object[0].piece_table, 0x14);
LAYOUT_AT(MapObjectSprites, object_0__pose, object[0].pose, 0x18);
LAYOUT_AT(MapObjectSprites, object_0__pad_1a, object[0].pad_1a, 0x1a);
LAYOUT_AT(MapObjectSprites, object_0__scaled, object[0].scaled, 0x1c);
LAYOUT_AT(MapObjectSprites, object_0__pad_1e, object[0].pad_1e, 0x1e);
LAYOUT_AT(MapObjectSprites, object_0__scale_x, object[0].scale_x, 0x20);
LAYOUT_AT(MapObjectSprites, object_0__scale_y, object[0].scale_y, 0x22);
LAYOUT_AT(MapObjectSprites, twin, twin, 0x9d8);
LAYOUT_AT(MapObjectSprites, twin_0__on, twin[0].on, 0x9d8);
LAYOUT_AT(MapObjectSprites, twin_0__x, twin[0].x, 0x9da);
LAYOUT_AT(MapObjectSprites, twin_0__depth, twin[0].depth, 0x9dc);
LAYOUT_AT(MapObjectSprites, twin_0__y, twin[0].y, 0x9de);
LAYOUT_AT(MapObjectSprites, twin_0__attr, twin[0].attr, 0x9e0);
LAYOUT_AT(MapObjectSprites, twin_0__flags, twin[0].flags, 0x9e2);
LAYOUT_AT(MapObjectSprites, twin_0__texel_records, twin[0].texel_records, 0x9e4);
LAYOUT_AT(MapObjectSprites, twin_0__pose_table, twin[0].pose_table, 0x9e8);
LAYOUT_AT(MapObjectSprites, twin_0__piece_table, twin[0].piece_table, 0x9ec);
LAYOUT_AT(MapObjectSprites, twin_0__pose, twin[0].pose, 0x9f0);
LAYOUT_AT(MapObjectSprites, twin_0__pad_1a, twin[0].pad_1a, 0x9f2);
LAYOUT_AT(MapObjectSprites, twin_0__scaled, twin[0].scaled, 0x9f4);
LAYOUT_AT(MapObjectSprites, twin_0__pad_1e, twin[0].pad_1e, 0x9f6);
LAYOUT_AT(MapObjectSprites, twin_0__scale_x, twin[0].scale_x, 0x9f8);
LAYOUT_AT(MapObjectSprites, twin_0__scale_y, twin[0].scale_y, 0x9fa);

#endif
