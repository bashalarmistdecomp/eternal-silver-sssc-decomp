/* script.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef SCRIPT_H
#define SCRIPT_H

#include "layout.h"
#include "map.h"

/* One of the 18 rows of 0x30 bytes at the address in g_MoveChrRows
   (0x801C9F20; MoveChrInit stores it and zeroes the rows) that
   MoveChrCommandRun runs, the script's movers: row 0 the view, rows 1 to 6 the
   party's places 0 to 5, rows 7 to 0xA rows a command gives a map object, rows
   0xB to 0x10 rows with an actor of their own (g_MoveChrOwnActors), row 0x11
   the second view. A row runs its command list and walks its actor (or the
   view) to a goal, with its height, facing, speed and scale. */
typedef struct {
    /* +0x00 1 while the row is in use, beside its bit in g_MoveChrLiveBits, 0
       when it is let go. MoveChrRowInit sets 1; MoveChrFollowStart (rows 0,
       0x11, 2 to 6), MoveChrFollowRestart, MoveChrPartyJoin,
       MoveChrPartyLeave, ScriptCommandMoveChrRowsEnd,
       ScriptCommandMoveChrRowsAllEnd and MoveChrSecondViewFollowSet (row 0x11)
       set 0. MoveChrSpriteDataSet sets an own-actor row's actor's sprite data
       only while it is not 0. */
    u8 on;
    /* +0x01 the running byte: 1 while a command that takes several ticks (a
       walk, a wait) is under way. MoveCommandMoveTo and MoveCommandMoveBy set
       1 when they take their goal, MoveCommandWait when it starts its wait,
       MoveChrFollowStart and MoveChrFollowRestart with the goal they give;
       MoveChrWalkStep, MoveChrViewStep and MoveChrSecondViewStep set 0 at the
       goal, MoveCommandWait when the wait is over; MoveChrRowInit sets 0. */
    u8 running;
    /* +0x02 the actor's height above the ground in pixels: the actor's drawn y
       (sprite.y) is its ground y (sprite.depth) less it. MoveChrWalkStep steps
       it toward goal_height by height_step once a tick (value_goal_step) and
       writes the actor's drawn y from it; MoveCommandPlace sets the command's
       byte 3; MoveCommandMoveBy takes it + the command's signed byte 3 as the
       goal height; ScriptCommandMoveChrRowActorSet sets an own actor's from
       the command's byte 7; MoveChrPartyJoin and
       ScriptCommandMoveChrPartyFollowEnd set the ring entry's height,
       MoveChrPartyLeave copies the leaving member's row's; MoveChrRowInit sets
       0. */
    u8 height;
    /* +0x03 a union: what the row's actor is, a row number to the rows that
       hold a map object and an id to the others. object: rows 7 to 0xA (any
       row below 0xB a command gives an object): the object's row number in
       g_MapObjects (ScriptCommandMoveChrRowActorSet from MapObjectFind,
       MoveChrPartyLeave from MapObjectFreeFind); ScriptCommandMoveChrRowsEnd,
       ScriptCommandMoveChrRowsAllEnd, MoveChrPartyJoin and MoveChrRowIdGet
       index g_MapObjects with it. id: rows 1 to 6, the low byte of the
       member's id (MoveChrInit for row 1, MoveChrFollowRestart,
       MoveChrPartyJoin, ScriptCommandMoveChrPartyFollowEnd); rows 0xB to 0x10,
       the own actor's sprite data number (ScriptCommandMoveChrRowActorSet;
       MoveChrPartyLeave writes the member's id), which MoveChrSpriteDataSet
       sets the actor's sprite data from and MoveChrRowIdGet answers as the
       actor's id. */
    union {
        /* rows below 0xB that a command gives a map object (7 to 0xA): the
           object's row number in g_MapObjects */
        u8 object;
        /* rows 1 to 6: the low byte of the member's id; rows 0xB to 0x10: the
           own actor's sprite data number, which MoveChrRowIdGet answers as the
           actor's id */
        u8 id;
    } actor_number;
    /* +0x04 the address of the row's command list, the move commands (op bytes
       0xF0 to 0xF9; MoveChrCommandRun calls entry (op + 0x10) & 0xFF of
       D_800B45F8) the row runs from its start. ScriptCommandMoveChrCommandsSet
       stores the address of the list that follows its command;
       ScriptCommandMoveChrCommandsStart copies it into command for each live
       row that has one and no command, and sets the row's busy bit;
       MoveChrFollowStart and MoveChrFollowRestart point it at
       g_MoveChrMoveToList / g_MoveChrMoveToListCopy; MoveCommandEnd and
       MoveChrRowInit zero it. */
    u8 * command_list;
    /* +0x08 the command pointer: the command the row runs now
       (MoveChrCommandRun dispatches on its op byte for each busy row, once a
       frame). Each command moves it past itself: MoveCommandPoseSet,
       MoveCommandFacingSet, MoveCommandHeightStepSet and MoveCommandSpeedSet
       by 2, MoveCommandScale by 4, MoveCommandPlace by 5, MoveCommandWait by 2
       when its wait is over, MoveChrWalkStep, MoveChrViewStep and
       MoveChrSecondViewStep by 5 at the goal;
       ScriptCommandMoveChrCommandsStart, MoveChrFollowStart and
       MoveChrFollowRestart set it to the list's start; MoveCommandEnd and
       MoveChrRowInit zero it. The commands read their operand bytes through
       it. */
    u8 * command;
    /* +0x0C the address of the row's actor, the 0x3C-byte record the row
       moves: a party record (g_MapPartyRecords + place * 0x3C: MoveChrInit for
       row 1, MoveChrFollowRestart, MoveChrPartyJoin, MoveChrPartyLeave,
       ScriptCommandMoveChrPartyFollowEnd) or a map object's
       (ScriptCommandMoveChrRowActorSet: a row of g_MapObjects for rows below
       0xB, the row's own actor in g_MoveChrOwnActors for 0xB to 0x10;
       MoveChrPartyLeave for the member who stays behind), which begins with
       its record (MapObject.record). Rows 0 and 0x11 have none.
       MoveChrWalkStep writes the actor's sprite.x, sprite.depth, sprite.y
       (depth less the height) and the class bits 0x6000 of sprite.flags;
       MoveCommandPlace the same at once; MoveChrAction steps its animation and
       copies its sprite entry to entry row - 1 of the sprite list;
       MoveChrScaleSet and MoveChrScaleStep write its sprite.scaled, scale_x
       and scale_y; ScriptCommandMixGroupSet and ScriptCommandMixGroupClear its
       mix group bits (sprite.flags & 0x1C00); MoveCommandPoseSet,
       MoveCommandFacingSet and MoveChrWalkStep hand it to MapAnimationPoseSet;
       MoveChrPartyJoin copies it whole into a party record. MoveChrRowInit
       zeroes it. */
    MapRecord * actor;
    /* +0x10 the address of the actor's twin, a 0x3C-byte record
       (g_MapPartyTwins, g_MapObjectTwins or g_MoveChrOwnActorTwins, the
       actor's own number in it), set with actor by the same functions.
       MoveChrAction puts it on the actor's animation and copies its entry to
       entry row + 0x45 of the sprite list while its on byte is not 0;
       MoveCommandPlace puts it on the actor's x and y; MoveChrScaleSet,
       MoveChrScaleStep, ScriptCommandMixGroupSet and
       ScriptCommandMixGroupClear treat it as they treat the actor;
       MoveChrSpriteDataSet sets its sprite data; MoveChrPartyJoin copies it
       whole into a party twin. MoveChrRowInit zeroes it. */
    MapRecord * twin;
    /* +0x14 the address of the actor's box (MapBox, map.h): &object->box for a
       map object or an own actor (ScriptCommandMoveChrRowActorSet,
       MoveChrPartyLeave), the party's box at g_MapPartyBoxDx for a party row
       (MoveChrInit, MoveChrFollowRestart, MoveChrPartyJoin, MoveChrPartyLeave,
       ScriptCommandMoveChrPartyFollowEnd). MoveChrWalkStep hands it to
       MapSpotMoveTry and MapAttributeClassGet, MoveCommandPlace to
       MapAttributeClassGet. MoveChrRowInit zeroes it. */
    MapBox * box;
    /* +0x18 the x of the walk's goal in the map's pixels (rows 0 and 0x11: the
       view's x to reach). MoveCommandMoveTo stores the command's byte 1 * 8;
       MoveCommandMoveBy (the actor's x, or the view's, / 8 + the signed byte
       1) * 8; MoveChrFollowStart the x MapViewGoalGet, MapSecondViewGoalGet or
       the ring entry of the row's place gives, MoveChrFollowRestart the same
       for rows 0 and 2 to 6. MoveChrWalkStep aims at it (MapFacingTowardGet),
       MoveChrGoalCheck holds the step on it, MoveChrViewStep and
       MoveChrSecondViewStep move the view toward it (value_goal_step);
       MoveChrFollowStart reads row 0's to put the view there while it asks for
       the second view's goal. */
    s16 goal_x;
    /* +0x1A the ground y of the walk's goal (rows 0 and 0x11: the view's y),
       from the command's byte 2; written and read with goal_x by the same
       functions. */
    s16 goal_y;
    /* +0x1C a union: the height a walk ends at, or the frames a wait has left.
       goal_height: MoveCommandMoveTo stores the command's byte 3,
       MoveCommandMoveBy the height + the signed byte 3, MoveChrFollowStart and
       MoveChrFollowRestart the ring entry's height for rows 2 to 6;
       MoveChrWalkStep steps the height toward it and MoveChrGoalCheck (read
       unsigned) stops a walk only at it. wait_left: MoveCommandWait stores the
       command's byte 1 when it starts and counts it down once a frame, and the
       wait ends at 0. */
    union {
        /* a walk: the height it ends at */
        s16 goal_height;
        /* a wait: the frames it has left, from the command's byte 1 */
        s16 wait_left;
    } height_wait;
    /* +0x1E the pixels a tick moves: MoveChrWalkStep takes that many one-pixel
       steps a tick, MoveChrViewStep and MoveChrSecondViewStep move the view by
       it; MoveChrWalkStep and MoveCommandPlace hand it to MapAttributeClassGet
       as its step. MoveCommandSpeedSet sets g_MoveSpeedPixels at the command's
       byte 1, MoveChrFollowStart g_MoveSpeedPixels[3]; MoveChrRowInit sets 1. */
    u8 speed;
    /* +0x1F the pixels a tick moves the height toward goal_height
       (MoveChrWalkStep: value_goal_step(height, goal_height, height_step)).
       MoveCommandHeightStepSet sets the command's byte 1; MoveChrRowInit sets
       1. */
    u8 height_step;
    /* +0x20 the actor's layer word, 0 or 0x80, as MapObject.layer: the layer
       MapSpotMoveTry and MapAttributeClassGet are given for it
       (MoveChrWalkStep, MoveCommandPlace). ScriptCommandMoveChrRowActorSet
       copies an object's layer (+0x3C) or sets an own actor's and its own from
       the command's byte 9; MoveChrPartyLeave copies the leaving member's
       row's into the row and the object; MoveChrRowInit sets 0. */
    u8 layer;
    /* +0x21 the actor's facing, 1 to 8 (1 toward smaller y, then clockwise),
       given to MapAnimationPoseSet with each pose. MoveChrWalkStep keeps the
       facing each step took and at the goal takes facing_kept when that is not
       0; MoveCommandFacingSet sets the command's byte 1 (MoveCommandPlace
       through it); MoveCommandPoseSet passes it on.
       ScriptCommandMoveChrRowActorSet copies an object's (+0x3E) or sets an
       own actor's from the command's byte 8; MoveChrInit, MoveChrPartyJoin,
       MoveChrPartyLeave, MoveChrFollowRestart and
       ScriptCommandMoveChrPartyFollowEnd take the ring entry's facing,
       MoveChrPartyLeave copies a party row's into the row and the object.
       ScriptCommandMoveChrRowsEnd and ScriptCommandMoveChrRowsAllEnd give it
       back to the object's facing when the row lets the object go;
       MoveChrPartyLeave fills the ring with row 1's. MoveChrRowInit sets 0. */
    u8 facing;
    /* +0x22 the facing the walk heads along: MoveChrWalkStep re-aims it at the
       goal with MapFacingTowardGet while it equals facing and steps along it
       (MapSpotMoveTry), and MoveChrGoalCheck holds the step on the goal only
       when the step went along it. MoveCommandMoveTo, MoveCommandMoveBy and
       MoveChrFollowStart set it to facing when they take a goal;
       MoveChrFollowRestart sets it and facing from the ring entry.
       MoveChrRowInit leaves it. */
    u8 aim;
    /* +0x23 the facing a walk command shows while it walks and ends on, 0 for
       none (the command's byte 4: MoveCommandMoveTo, MoveCommandMoveBy;
       MoveChrFollowStart and MoveChrFollowRestart set 0), as
       MapObject.facing_kept is for a step list. MoveChrWalkStep hands it to
       MapPartyRingPush and MapAnimationPoseSet in place of facing while it is
       not 0, and copies it into facing at the goal. MoveChrRowInit sets 0. */
    u8 facing_kept;
    /* +0x24 the frames left before the row's next tick: MoveChrWalkStep counts
       it down each frame and at 0 loads it again from interval and ticks.
       MoveCommandSpeedSet and MoveChrFollowStart set it with interval;
       MoveChrRowInit sets 1. */
    u8 interval_left;
    /* +0x25 the frames from one tick of the row to the next:
       g_MoveSpeedIntervals at the speed command's byte 1
       (MoveCommandSpeedSet), g_MoveSpeedIntervals[3] (MoveChrFollowStart);
       MoveChrRowInit sets 1. MoveChrWalkStep reloads interval_left from it.
       The same table as MapObject.interval. */
    u8 interval;
    /* +0x26 the number in g_MoveChrScales of the actor's scale across (4 is
       0x100, the scale 1): MoveChrScaleSet writes the table's entry into the
       actor's and the twin's sprite.scale_x. MoveCommandScale stores the
       command's byte 1, after it has set the glide's direction against the old
       number; MoveChrRowInit sets 4. */
    u8 scale_x_number;
    /* +0x27 the number in g_MoveChrScales of the scale down, for scale_y; from
       the command's byte 2, written and read with scale_x_number. */
    u8 scale_y_number;
    /* +0x28 the direction of the glide of the scale across: 0 none, 1 down, 2
       up (MoveCommandScale for a command with a rate). MoveChrScaleStep moves
       both rows' scale_x by 0xC each period that way and sets 0 when
       scale_x_goal is reached; MoveCommandScale does nothing while it or
       scale_y_direction is not 0. MoveChrRowInit sets 0. */
    u8 scale_x_direction;
    /* +0x29 the direction of the glide of the scale down, for scale_y; written
       and read with scale_x_direction. */
    u8 scale_y_direction;
    /* +0x2A the frames between two steps of the glide: MoveCommandScale stores
       1 << (rate - 1), the command's byte 3; MoveChrScaleStep reloads
       scale_left from it after each step. */
    u8 scale_period;
    /* +0x2B the frames left before the glide's next step: MoveCommandScale
       sets 1, MoveChrScaleStep counts it down while a direction is not 0 and
       loads it from scale_period after each step. */
    u8 scale_left;
    /* +0x2C the scale across the glide ends on: g_MoveChrScales at the
       command's byte 1 (MoveCommandScale); MoveChrScaleStep compares the
       actor's scale_x with it. */
    s16 scale_x_goal;
    /* +0x2E the scale down the glide ends on: g_MoveChrScales at the command's
       byte 2 (MoveCommandScale); MoveChrScaleStep compares the actor's scale_y
       with it. */
    s16 scale_y_goal;
} MoveChrRow;

LAYOUT_SIZE(MoveChrRow, 0x30);
LAYOUT_AT(MoveChrRow, on, on, 0x00);
LAYOUT_AT(MoveChrRow, running, running, 0x01);
LAYOUT_AT(MoveChrRow, height, height, 0x02);
LAYOUT_AT(MoveChrRow, actor_number, actor_number, 0x03);
LAYOUT_AT(MoveChrRow, actor_number_object, actor_number.object, 0x03);
LAYOUT_AT(MoveChrRow, actor_number_id, actor_number.id, 0x03);
LAYOUT_AT(MoveChrRow, command_list, command_list, 0x04);
LAYOUT_AT(MoveChrRow, command, command, 0x08);
LAYOUT_AT(MoveChrRow, actor, actor, 0x0c);
LAYOUT_AT(MoveChrRow, twin, twin, 0x10);
LAYOUT_AT(MoveChrRow, box, box, 0x14);
LAYOUT_AT(MoveChrRow, goal_x, goal_x, 0x18);
LAYOUT_AT(MoveChrRow, goal_y, goal_y, 0x1a);
LAYOUT_AT(MoveChrRow, height_wait, height_wait, 0x1c);
LAYOUT_AT(MoveChrRow, height_wait_goal_height, height_wait.goal_height, 0x1c);
LAYOUT_AT(MoveChrRow, height_wait_wait_left, height_wait.wait_left, 0x1c);
LAYOUT_AT(MoveChrRow, speed, speed, 0x1e);
LAYOUT_AT(MoveChrRow, height_step, height_step, 0x1f);
LAYOUT_AT(MoveChrRow, layer, layer, 0x20);
LAYOUT_AT(MoveChrRow, facing, facing, 0x21);
LAYOUT_AT(MoveChrRow, aim, aim, 0x22);
LAYOUT_AT(MoveChrRow, facing_kept, facing_kept, 0x23);
LAYOUT_AT(MoveChrRow, interval_left, interval_left, 0x24);
LAYOUT_AT(MoveChrRow, interval, interval, 0x25);
LAYOUT_AT(MoveChrRow, scale_x_number, scale_x_number, 0x26);
LAYOUT_AT(MoveChrRow, scale_y_number, scale_y_number, 0x27);
LAYOUT_AT(MoveChrRow, scale_x_direction, scale_x_direction, 0x28);
LAYOUT_AT(MoveChrRow, scale_y_direction, scale_y_direction, 0x29);
LAYOUT_AT(MoveChrRow, scale_period, scale_period, 0x2a);
LAYOUT_AT(MoveChrRow, scale_left, scale_left, 0x2b);
LAYOUT_AT(MoveChrRow, scale_x_goal, scale_x_goal, 0x2c);
LAYOUT_AT(MoveChrRow, scale_y_goal, scale_y_goal, 0x2e);

/* One answer of the message window's prompts: a string id and a string's
   address, as font_string_put takes them. ScriptAnswerRun and ScriptPagesRun
   build two on their stack from the two strings of script command 7 and hand
   them to MessageAnswerRun or MessagePageRun. */
typedef struct {
    /* +0x00 the string's id in the string table, font_string_put's first
       argument; 0 takes the string at string (ScriptAnswerRun and
       ScriptPagesRun set 0 for both answers). */
    u16 string_id;
    /* +0x02..+0x03 nothing reads or writes them. */
    u8 pad_02[2];
    /* +0x04 the address of the string, used when string_id is 0:
       ScriptAnswerRun and ScriptPagesRun store g_ScriptCommandPointer at each
       of the command's two strings (stepped past with ScriptStringEndFind). */
    s32 string;
} MessageAnswer;

LAYOUT_SIZE(MessageAnswer, 0x08);
LAYOUT_AT(MessageAnswer, string_id, string_id, 0x00);
LAYOUT_AT(MessageAnswer, pad_02, pad_02, 0x02);
LAYOUT_AT(MessageAnswer, string, string, 0x04);

#endif
