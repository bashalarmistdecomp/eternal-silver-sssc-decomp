# Map engine — where it is documented

The field's map engine is spread over several pages. This one says which
holds what.

| what | where |
|---|---|
| The map resource: one list of commands that `resource_command_run` (`0x800202FC`, resource_load.c) walks; command 3 the texture pages, 4 the palettes, 5 the tile layout, 6 the attribute plane, 7 the second layer's cells, 9 the scene table and the moves inside a map, 16 the images of the animated tiles | [graphics.md](../../graphics.md) §"World Map Arrangement Data" |
| Drawing: the tile word, `bg_first_draw` (`0x80014058`), the block cache `bg_block_load` (`0x80014F3C`), the second layer `bg_second_draw` (`0x800148E0`), the walk-behind copy and its depth grid, what shows through a tile's transparent texels, the run-time tile writes, the animated tiles and palettes | [rendering.md](rendering.md) |
| The field's frame in order, the walk, collision, the camera, scene set-up, doors, fades | [field-frame.md](field-frame.md) |
| The map objects, their think pass, the touch scan, the map-cell events, the script interpreter, the mover slots | [npc-scripting.md](npc-scripting.md) |
| The enemies on a map: how they roam, see the party, chase it and start a fight | [map-enemies.md](map-enemies.md) |
| The area records: kind, place name, music | [text.md](../../text.md) §"Locations" |
| The party's field sprites | [assets/sprites.md](../../assets/sprites.md) §"The party's field sheets" |
