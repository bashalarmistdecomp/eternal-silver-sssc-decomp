# The title logo

This page covers where the title screen's logo lies in video memory, how
the title draws it, and what the rectangle it draws holds. The whole title
screen is in
[../code/subsystems/menu-ui.md](../code/subsystems/menu-ui.md)
§"The title screen's data and draw".

## Where the logo is

The logo is on the title's "logo" page: LUNADATA **resource 3**, which
`StartResourceLoad` (`0x8004409C`) runs, holds a type-2 TIM at VRAM
(384, 0). `StartScreenDraw` (`0x80043848`) draws the logo as one sprite
(`packet_sprite_add`): the 0xF0 x 0x58 rectangle at (u, v) = (0, 0x30) of
the 8-bit texture page at (0x180, 0), onto screen (0x2D, 0x34). Its palette
word is `0x7E00`, row 504, except while the logo fades: in phase 1 the
word is `((frame + 0xE0) << 6) + 0x20` and in phase 2
`((0xFF - frame) << 6) + 0x20`, so each frame of the fade takes its 256
colours from x 512 of another row of video memory. Those rows are the ones
`StartScreenInit` makes: over the 0x20 frames of phase 1 the logo goes to
white, and over those of phase 2 it comes back (the register's reading of
`StartScreenDraw`).

Inside the rectangle (rows, from the logo's top):

| rows | what |
|---|---|
| 1..52 | the LUNAR lettering, its drop shadow, the ® (at the right, rows 44..52) |
| 53..66 | the R's leg tip and shadow, running down and right (x 212..243) |
| 56..66 | the silver plate with "SILVER STAR STORY" (x 2..230) |
| 72..87 | "COMPLETE" |

