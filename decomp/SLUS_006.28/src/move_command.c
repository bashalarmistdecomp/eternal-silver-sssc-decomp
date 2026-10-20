/* Lunar SSSC — move_command: the rows' move commands, the ten entries of
   the table at 0x800B45F8 that MoveChrCommandRun calls (pose, facing,
   height, move to and by, speed, wait, place, scale, end). The first split
   had it in scripting_d.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "script.h"
#include "map.h"
#include "sprite.h"

/* at the top of the file: MoveChrSecondViewStep's prototype, well above
   its definition, names MoveChrRow */
s32 MapPartyRingEntryGet(s32, s32 *);
s32 ScriptErrorReport();
extern u16 g_ScriptCommandWord;
void MapAnimationPoseSet(s32, s32, s32);
extern u8 g_MoveSpeedPixels[];
extern u8 g_MoveSpeedIntervals[];
extern s16 g_MapSecondViewX;
extern s16 g_MapSecondViewY;
extern u8 g_MoveChrPartyFollow;
extern u8 g_MoveChrSecondViewFollow;
extern s16 g_ScriptErrorNumber;
extern s32 g_MoveChrBusyBits;
s32 MoveChrError11Report(s32 unused, u16 row_number);  /* TU defines this below */
s32 MoveCommandFacingSet(void *row);  /* TU defines this below */
void MoveCommandHeightStepSet(void *row);  /* TU defines this below */
void MoveCommandSpeedSet(void *row);  /* TU defines this below */
void MoveCommandWait(void *row);  /* TU defines this below */
void MoveCommandEnd(void *row, s32 row_number);  /* TU defines this below */
s32 MapPartyRingFollow(void);
s32 MapAttributeClassGet();
extern s16 g_MapViewX;
extern s16 g_MapViewY;
void MoveChrScaleSet(u8 *row);
void MapPartyRingFill();
extern s16 g_ScriptErrorNumber;
extern s32 g_MoveChrBusyBits;
void MoveCommandPoseSet(void *row);
void MoveChrWalkStep(MoveChrRow *row, u16 row_number);  /* extern */

s32 MoveChrError11Report(s32 unused, u16 row_number);  /* TU defines this below */
s32 MoveCommandFacingSet(void *row);  /* TU defines this below */
void MoveCommandHeightStepSet(void *row);  /* TU defines this below */
void MoveCommandSpeedSet(void *row);  /* TU defines this below */
void MoveCommandWait(void *row);  /* TU defines this below */
void MoveCommandEnd(void *row, s32 row_number);  
/* far_syms: g_MoveChrScales (target reaches it %hi/%lo) */
extern const u16 g_MoveChrScales[];

/* func_80062F04 */
void MoveChrScaleSet(u8 *row)
{
	u8 *row_copy = row;

	if ((AS(u32, ((MoveChrRow *)row_copy)->interval_left) & 0xFFFF0000) == 0x4040000) {
		((u16 *)AS(u32, ((MoveChrRow *)row_copy)->actor))[0x10] = 0;
		((u16 *)AS(u32, ((MoveChrRow *)row_copy)->twin))[0x10] = 0;
	} else {
		((u16 *)AS(u32, ((MoveChrRow *)row_copy)->actor))[0x10] = 1;
		((u16 *)AS(u32, ((MoveChrRow *)row_copy)->twin))[0x10] = 1;
	}

	((u16 *)AS(u32, ((MoveChrRow *)row_copy)->actor))[0x12] = ((u16 *)g_MoveChrScales)[((MoveChrRow *)row_copy)->scale_x_number];
	((u16 *)AS(u32, ((MoveChrRow *)row_copy)->actor))[0x13] = ((u16 *)g_MoveChrScales)[((MoveChrRow *)row_copy)->scale_y_number];
	((u16 *)AS(u32, ((MoveChrRow *)row_copy)->twin))[0x12] = ((u16 *)g_MoveChrScales)[((MoveChrRow *)row_copy)->scale_x_number];
	((u16 *)AS(u32, ((MoveChrRow *)row_copy)->twin))[0x13] = ((u16 *)g_MoveChrScales)[((MoveChrRow *)row_copy)->scale_y_number];
}

/* func_80062FC8 */
s32 MoveChrError11Report(s32 unused, u16 row_number)
{
    g_ScriptCommandWord = row_number;
    g_ScriptErrorNumber = 0x11;
    return ScriptErrorReport();
}

/* func_80062FF4 */
void MoveCommandPoseSet(void *row)
{
  void *command_then_row;
  s32 pose;
  u8 *command_pointer;
  unsigned int operand_then_facing_offset;
  command_pointer = ((u8 *) row) + 0x8;
  command_then_row = *((s32 *) command_pointer);
  operand_then_facing_offset = 0x1;
  *((s32 *) command_pointer) = (u32) (command_then_row + 1);
  pose = *((u8 *) (((u8 *) command_then_row) + operand_then_facing_offset));
  *((s32 *) command_pointer) = (u32) (command_then_row + 2);
  operand_then_facing_offset = 0x21;
  command_then_row = (u8 *) row;
  MapAnimationPoseSet(AS(s32, ((MoveChrRow *)command_then_row)->actor), pose, (*((u8 *) (((u8 *) row) + operand_then_facing_offset))) & 0xFF);
}

/* func_80063038 */
s32 MoveCommandFacingSet(void *row)
{
  void *command_then_row;
  s32 facing;
  u8 *command_pointer;
  u8 *row_facing;
  unsigned int operand_then_facing_offset;
  command_pointer = ((u8 *) row) + 0x8;
  command_then_row = *((s32 *) command_pointer);
  operand_then_facing_offset = 0x1;
  *((s32 *) command_pointer) = (u32) (command_then_row + 1);
  facing = *((u8 *) (((u8 *) command_then_row) + operand_then_facing_offset));
  *((s32 *) command_pointer) = (u32) (command_then_row + 2);
  row_facing = (u8 *) (((u8 *) row) + (operand_then_facing_offset = 0x21));
  *row_facing = facing;
  command_then_row = (u8 *) row;
  MapAnimationPoseSet(AS(s32, ((MoveChrRow *)command_then_row)->actor), -1, (*((u8 *) (((u8 *) ((u8 *) row)) + operand_then_facing_offset))) & 0xFF);
}

/* func_80063080 */
void MoveCommandHeightStepSet(void *row) {
    void *command;

    command = (void *)(AS(s32, ((MoveChrRow *)row)->command));
    AS(s32, ((MoveChrRow *)row)->command) = (s32)(void *) (command + 1);
    ((MoveChrRow *)row)->height_step = (u8) (*(u8 *)((u8 *)command + 0x1));
    AS(s32, ((MoveChrRow *)row)->command) = (s32)(void *) (command + 2);
}
/* MoveCommandMoveTo -- Move-script opcode 0xF3
   (g_MoveChrCommandFunctions[3]), MOVE TO: the five-byte command `F3 x y z facing`, the
   absolute twin of 0xF4 (MoveCommandMoveBy). While the slot is not running
   (+1 == 0) the goal is taken straight from the command: +0x18 = x << 3,
   +0x1A = y << 3, goal height +0x1C = z (three UNSIGNED bytes, no range
   check and so no error path), facing override +0x23 = the facing byte,
   +0x22 = +0x21, +1 = 1; then, and on every later call, MoveChrWalkStep(slot,
   n) steps the mover. It is the opcode of the built-in scripts g_MoveChrMoveToList /
   g_MoveChrMoveToListCopy (F3 00 00 00 00 F9), where MoveChrFollowStart has already
   filled the goal and set +1, so there it only ticks.

   The source order of the three byte stores is MoveCommandMoveBy's
   (+0x23, +0x22, +1 -- the order MoveChrFollowStart fills a slot in); the
   target EMITS them +1, +0x22, +0x23 with both loads ahead of them.
   Every script byte is read through the slot (`((u8 *)row->w8)[k]`), no
   pointer local: with the stores written in the emitted order it is
   24-27/34. */
/* func_800630A4 */
void MoveCommandMoveTo(MoveChrRow *row, u16 row_number)
{
    if (row->running == 0) {
        AS(u16, row->goal_x) = ((u8 *)AS(s32, row->command))[1] << 3;
        AS(u16, row->goal_y) = ((u8 *)AS(s32, row->command))[2] << 3;
        row->height_wait.goal_height = ((u8 *)AS(s32, row->command))[3];
        row->facing_kept = ((u8 *)AS(s32, row->command))[4];
        row->aim = row->facing;
        row->running = 1;
    }
    MoveChrWalkStep(row, row_number);
}

/* Move-script opcode 0xF4 -- g_MoveChrCommandFunctions[4], called by MoveChrCommandRun as
   handler(slot, n) for each busy mover slot (a 0x30-byte g_MoveChrRows row)
   whose current script byte is 0xF4. MOVE BY: a five-byte command
   `F4 dx dy dz facing`, the relative twin of opcode 0xF3 (MoveCommandMoveTo,
   `F3 x y z facing`). dx, dy and dz are SIGNED bytes, dx and dy in
   8-pixel units, counted from where the mover is now.

   While the slot is not running (+1 == 0) the goal is set up first:
     n == 0     the camera: x = g_MapViewX / 8 + dx, y = g_MapViewY / 8 + dy,
                z = 0.
     n == 0x11  the second scroll: the same from g_MapSecondViewX / g_MapSecondViewY.
     others     x = the actor row's (+0xC) x (+6) / 8 + dx, y = its ground
                y (+8) / 8 + dy, z = the slot's height (+2) + dz.
   x, y and z must each be 0..0xFF. If one is not,
   MoveChrError11Report reports error 0x11 for slot n (g_ScriptCommandWord = n,
   g_ScriptErrorNumber = 0x11, ScriptErrorReport) and the handler returns: nothing is
   stored, the slot is not stepped, +1 stays 0 and the script pointer
   stays on the command.
   In range: goal +0x18 / +0x1A = x * 8 / y * 8, goal height +0x1C = z,
   facing override +0x23 = the command's facing byte, +0x22 = +0x21,
   +1 = 1 (running) -- the order MoveChrFollowStart fills a slot in.
   Then, and on every later call while +1 is set, MoveChrWalkStep(slot, n)
   steps the mover; its steppers clear +1 and move the script pointer
   five bytes on when the goal is reached.

   Shape: the positions are read as s16 and divided signed, and each sum
   is written position first (the position is loaded before the script
   pointer). The script pointer is read through the slot every time. x, y
   and z are s16: of the six-term range test only the first pair folds
   into one unsigned 16-bit compare. The goal is `x * 8`: stored to a
   16-bit field, `x << 3` is narrowed to a 16-bit shift (no sign
   extension, 103/107), the multiply is done on the sign-extended value.
   n is zero-extended for MoveChrError11Report too (its only call
   site), which is what types that function's second parameter u16. */
/* func_8006312C */
void MoveCommandMoveBy(MoveChrRow *row, u16 row_number)
{
    s16 x;
    s16 y;
    s16 z;

    if (row->running == 0) {
        if (row_number == 0) {
            x = g_MapViewX / 8 + (s8)((u8 *)AS(s32, row->command))[1];
            y = g_MapViewY / 8 + (s8)((u8 *)AS(s32, row->command))[2];
            z = 0;
        } else if (row_number == 0x11) {
            x = g_MapSecondViewX / 8 + (s8)((u8 *)AS(s32, row->command))[1];
            y = g_MapSecondViewY / 8 + (s8)((u8 *)AS(s32, row->command))[2];
            z = 0;
        } else {
            x = (s16)AS(u16, row->actor->sprite.x) / 8 + (s8)((u8 *)AS(s32, row->command))[1];
            y = (s16)AS(u16, row->actor->sprite.depth) / 8 + (s8)((u8 *)AS(s32, row->command))[2];
            z = row->height + (s8)((u8 *)AS(s32, row->command))[3];
        }
        if (x < 0 || x >= 0x100 || y < 0 || y >= 0x100 || z < 0 || z >= 0x100) {
            MoveChrError11Report((s32)row, row_number);
            return;
        }
        AS(u16, row->goal_x) = x * 8;
        AS(u16, row->goal_y) = y * 8;
        row->height_wait.goal_height = z;
        row->facing_kept = ((u8 *)AS(s32, row->command))[4];
        row->aim = row->facing;
        row->running = 1;
    }
    MoveChrWalkStep(row, row_number);
}

/* func_800632D8 */
void MoveCommandSpeedSet(void *row)
{
  unsigned char speed;
  u8 interval;
  void *command;
  command = AS(s32, ((MoveChrRow *)row)->command);
  AS(s32, ((MoveChrRow *)row)->command) = (u32) (command + 1);
  speed = (*((u8 *) (((u8 *) command) + 0x1))) & 0xFF;
  AS(s32, ((MoveChrRow *)row)->command) = (u32) (command + 2);
  ((MoveChrRow *)row)->speed = (u8) (*(speed + g_MoveSpeedPixels));
  interval = *(speed + g_MoveSpeedIntervals);
  ((MoveChrRow *)row)->interval = interval;
  ((MoveChrRow *)row)->interval_left = interval;
}

/* func_80063330 */
void MoveCommandWait(void *row) {
    u16 wait_left;

    if (((MoveChrRow *)row)->running == 0) {
        ((MoveChrRow *)row)->height_wait.wait_left = (u16) (*(u8 *)((u8 *)(AS(s32, ((MoveChrRow *)row)->command)) + 0x1));
        ((MoveChrRow *)row)->running = 1U;
    }
    wait_left = ((MoveChrRow *)row)->height_wait.wait_left - 1;
    ((MoveChrRow *)row)->height_wait.wait_left = wait_left;
    if (!(wait_left & 0xFFFF)) {
        ((MoveChrRow *)row)->running = 0U;
        AS(s32, ((MoveChrRow *)row)->command) = (u32)(AS(s32, ((MoveChrRow *)row)->command) + 2);
    }
}

/* Move-script opcode 0xF7 -- g_MoveChrCommandFunctions[7], called by MoveChrCommandRun as
   handler(slot, n) for each busy mover slot (a 0x30-byte g_MoveChrRows row)
   whose current script byte is 0xF7. PLACE: a five-byte command
   `F7 x y z facing` that puts the slot's actor on a spot at once, x and y
   in 8-pixel units. Nothing waits; the slot's script pointer (+8) ends
   five bytes on.

     n == 0     the camera: g_MapViewX / g_MapViewY = x * 8 / y * 8, then
                MapViewScrollSet(g_MoveChrSecondViewFollow). z and the facing are skipped.
     n == 0x11  the second scroll: g_MapSecondViewX / g_MapSecondViewY the same, then
                MapSecondViewScrollSet().
     others     the actor row (+0xC) gets x (+6) = x * 8 and ground y (+8)
                = y * 8; the slot's height (+2) = z; the row's draw y
                (+0xA) = ground y - z.
                Slot 1 (the lead): MapPartyRingFill((s16)x, (s16)y, z, facing
                as it stands in +0x21), then party spot 0 is read back
                (MapPartyRingEntryGet) and its +6 word becomes the row's 0x6000
                bits of +0xE. With g_MoveChrPartyFollow set, MapPartyRingFollow() runs
                and the command ends there: the last two bytes are skipped
                and the facing byte is NOT applied.
                Any other slot: MapAttributeClassGet(+0x1E, 0x20, x, draw y, y,
                box +0x14, layer +0x20) classifies the footprint into the
                row's 0x6000 bits.
                Then a live twin row (+0x10, first byte non-zero) gets the
                same x, and y * 8 as BOTH its ground y and its draw y (no
                height taken off), and MoveCommandFacingSet(slot, n) -- opcode
                0xF1's handler -- reads the facing byte into +0x21 and
                dispatches the actor's animation with it.

   Shape: the script pointer is read ONCE into `p` and every store back is
   `p + k` (a store through +0xC would force a reload of +8 otherwise),
   but the height is read through the slot again (`*row->w8`, after the
   byte store to +2 -- the target reloads +8 there). x and y are u16
   locals; (s16)x / (s16)y at MapPartyRingFill is what makes cse fold the
   sign extension into the byte's own shift (`p[1] << 19 >> 16`), so the
   two byte temporaries live into that arm (combine leaves `a0 = byte <<
   3`, `a1 = byte << 3` there) and, after register allocation, the
   recomputed arguments come out as copies of the registers that already
   hold them (`move a0,a2`, and nothing at all for a1). Passed plain they
   are copies of the saved registers instead (129/133). The draw y is an s16
   local read with *row->w8 FIRST in the target's load order.
   MoveCommandFacingSet is defined above with one parameter; the handler table
   passes two, hence the cast. */
/* func_8006338C */
void MoveCommandPlace(MoveChrRow *row, u16 row_number)
{
    MapPartyRingEntry spot;
    u8 *p;
    u16 x;
    u16 y;
    s16 dy;
    s32 flag;

    p = row->command;
    row->command = p + 1;
    if (row_number == 0) {
        g_MapViewX = p[1] << 3;
        row->command = p + 2;
        g_MapViewY = p[2] << 3;
        row->command = p + 5;
        MapViewScrollSet(g_MoveChrSecondViewFollow);
        return;
    }
    if (row_number == 0x11) {
        g_MapSecondViewX = p[1] << 3;
        row->command = p + 2;
        g_MapSecondViewY = p[2] << 3;
        row->command = p + 5;
        MapSecondViewScrollSet();
        return;
    }
    x = p[1] << 3;
    AS(u16, row->actor->sprite.x) = x;
    row->command = p + 2;
    y = p[2] << 3;
    AS(u16, row->actor->sprite.depth) = y;
    row->command = p + 3;
    row->height = p[3];
    dy = AS(u16, row->actor->sprite.depth) - *row->command;
    row->actor->sprite.y = dy;
    if (row_number == 1) {
        MapPartyRingFill((s16)x, (s16)y, row->height, row->facing);
        MapPartyRingEntryGet(0, (s32 *)&spot);
        flag = spot.class;
    } else {
        flag = MapAttributeClassGet(row->speed, 0x20, (s16)x, dy, y, AS(void *, row->box), row->layer);
    }
    row->actor->sprite.flags = flag | (row->actor->sprite.flags & 0x9FFF);
    if (row_number == 1 && g_MoveChrPartyFollow != 0) {
        MapPartyRingFollow();
        row->command += 2;
        return;
    }
    if (row->twin->on_kind.on != 0) {
        AS(u16, row->twin->sprite.x) = x;
        row->twin->sprite.y = AS(u16, row->twin->sprite.depth) = y;
    }
    ((void (*)())MoveCommandFacingSet)(row, row_number);
}
/* Move-script opcode 0xF8 -- g_MoveChrCommandFunctions[8], called by MoveChrCommandRun as
   handler(slot, n) for each busy mover slot (a 0x30-byte g_MoveChrRows row)
   whose current script byte is 0xF8. SCALE: a four-byte command
   `F8 ix iy rate`. ix and iy index the scale table g_MoveChrScales (0, 0x40,
   0x80, 0xC0, 0x100, 0x180, 0x200, 0x300, 0x400; 0x100 is 1:1) for the
   x and the y scale of the slot's actor row and its twin.

   While a glide is still running (a direction byte, +0x28 or +0x29, is
   non-zero) the handler does nothing and the script pointer stays on the
   command, so a second F8 waits for the first one's glide to end.

     rate == 0  at once: the slot's scale indices +0x26 / +0x27 = ix / iy,
                then MoveChrScaleSet(slot) writes the table values to both
                rows (+0x24 / +0x26; their +0x20, the "scaled" word, is 0
                when both indices are 4 and 1 otherwise).
     rate != 0  a glide: MoveChrScaleSet(slot) first, with the indices as
                they stand; both rows' +0x20 = 1; the goals +0x2C / +0x2E
                = the table values of ix / iy; the period +0x2A =
                1 << (rate - 1) frames (stored as a byte) and the countdown
                +0x2B = 1; the directions +0x28 (x) / +0x29 (y)
                = 0 when the new index equals the slot's, 1 when it is
                below it (shrink), 2 when above (grow); last, +0x26 /
                +0x27 = ix / iy. MoveChrScaleStep, which MoveChrCommandRun
                calls after every handler, then moves the rows' scale by
                0xC a period towards the goals and ends on
                MoveChrScaleSet(slot).

   Either way the script pointer (+8) ends four bytes on: the glide does
   not hold the script, only the next F8. n is not used.

   Shape: the table is a CONST array and is indexed directly, so its two
   loads are "unchanging" and the scheduler lifts both above the +0x2C /
   +0x2E stores (declared non-const, or read through a cast pointer, the
   second load stays behind the first store: 63/81). The two direction
   bytes are tested as members, `b28 == 0 && b29 == 0`; fold merges the
   adjacent bytes into the one `lhu` of +0x28. The period is stored before
   the countdown (the other order: 78/81). sx and sy are u8 locals (ints:
   62/81), and each direction is an if / else-if / else chain (the y chain
   as a ?: 77/81) -- the x chain's three stores stay apart because its
   1 is the register that already holds the rows' flag value, the y
   chain's 1 and 2 share one store. */
/* func_800635A0 */
void MoveCommandScale(MoveChrRow *row, u16 unused)
{
    u8 sx;
    u8 sy;

    if (row->scale_x_direction == 0 && row->scale_y_direction == 0) {
        if (((u8 *)AS(s32, row->command))[3] == 0) {
            row->scale_x_number = ((u8 *)AS(s32, row->command))[1];
            row->scale_y_number = ((u8 *)AS(s32, row->command))[2];
            MoveChrScaleSet((u8 *)row);
        } else {
            MoveChrScaleSet((u8 *)row);
            ((u16 *)row->actor)[0x10] = 1;
            ((u16 *)AS(void *, row->twin))[0x10] = 1;
            sx = ((u8 *)AS(s32, row->command))[1];
            sy = ((u8 *)AS(s32, row->command))[2];
            AS(u16, row->scale_x_goal) = g_MoveChrScales[sx];
            AS(u16, row->scale_y_goal) = g_MoveChrScales[sy];
            row->scale_period = 1 << (((u8 *)AS(s32, row->command))[3] - 1);
            row->scale_left = 1;
            if (sx == row->scale_x_number) {
                row->scale_x_direction = 0;
            } else if (sx < row->scale_x_number) {
                row->scale_x_direction = 1;
            } else {
                row->scale_x_direction = 2;
            }
            if (sy == row->scale_y_number) {
                row->scale_y_direction = 0;
            } else if (sy < row->scale_y_number) {
                row->scale_y_direction = 1;
            } else {
                row->scale_y_direction = 2;
            }
            row->scale_x_number = sx;
            row->scale_y_number = sy;
        }
        AS(s32, row->command) += 4;
    }
}

/* func_800636E4 */
void MoveCommandEnd(void *row, s32 row_number)
{
  s32 busy_bits;
  busy_bits = *(&g_MoveChrBusyBits);
  AS(s32, ((MoveChrRow *)row)->command_list) = 0;
  AS(s32, ((MoveChrRow *)row)->command) = 0;
  g_MoveChrBusyBits = busy_bits & (~(1 << row_number));
}
