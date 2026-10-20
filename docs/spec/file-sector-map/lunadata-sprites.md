# LUNADATA.FIL sectors 15571–17559: the map object sets

This page covers the run of LUNADATA.FIL that holds the map object sets:
the people, enemies and chests a map shows, with their sprite data and
their texture pages. It says which resources lie there and how a set is
laid out. What a set's parts mean is in
[../assets/sprites.md](../assets/sprites.md) §"Map object sets"; the
palettes they are drawn with are in
[../formats/palettes.md](../formats/palettes.md).

## Location

- **File**: LUNADATA.FIL
- **Resources**: `0x1E9` to `0x249` (97 sets), one for each object set:
  `MapObjectsDataLoad` (`0x80065418`) reads
  `g_MapObjectsDataResourceNumbers[set]` (`0x800B4E2C`), whose 97 non-zero
  words are exactly these 97 resources.
- **Sectors**: 15571–17559 (1989 sectors), by the EXE's resource table
  `g_ResourceSectorPairs` (`0x800AE904`). Resources `0x1E9`–`0x1EB` lie at
  15571–15597; `0x1EC` begins at 15598.

The address `0x800AF0B4` is not a second sector table: it is
`g_ResourceSectorPairs + 0x7B0`, the pair of resource `0x1EC`. Reading from
there, index i is resource `0x1EC + i`.

## Block inventory

Pairs `[start_sector:u16, size_sectors:u16]` from `g_ResourceSectorPairs`,
for the first and last resources of the run:

| Resource | Sector | Size | Bytes | Notes |
|-----|--------|------|-------|-------|
| `0x1EC` | 15598 | 3 | 6KB | |
| `0x1ED` | 15601 | 11 | 22KB | |
| `0x1EE` | 15612 | 11 | 22KB | |
| `0x1EF` | 15623 | 9 | 18KB | first page inflates to 65,536 bytes |
| `0x1F0` | 15632 | 40 | 80KB | first page inflates to 65,536 bytes |
| `0x1F1` | 15672 | 44 | 88KB | first page inflates to 65,536 bytes |
| ... | ... | ... | ... | |
| `0x248` | 17532 | 16 | 32KB | first page inflates to 65,536 bytes |
| `0x249` | 17548 | 12 | 24KB | the last set; the bromides follow at 17560 |

The 47 sets, by first sector, whose first texture page inflates to exactly
65,536 bytes (a full 256-line page):
15623, 15632, 15672, 15753, 15793, 15819, 15847, 15875, 15915, 15949, 15975, 16083, 16131, 16163, 16204, 16298, 16343, 16696, 16877, 16922, 16946, 16967, 16981, 16999, 17009, 17030, 17045, 17067, 17086, 17118, 17163, 17206, 17223, 17263, 17274, 17319, 17345, 17363, 17390, 17403, 17411, 17429, 17462, 17471, 17480, 17501, 17532

The other sets are not a different kind of block: a set has 0 to 5 pages,
and a page stream may hold fewer than 256 lines (98 of the 227 pages in the
97 sets do), so a set with no page, or whose first page is short, is
missing from that list.

## A set's layout

The resource begins with 32-bit words, each a byte offset from the
resource's start (`MapObjectsDataLoad`, `sprite_page_list_load`
(`0x800182B8`)):

| word | what |
|---|---|
| `[0]` | a packed stream: the definition block (the 128 object definitions of 0x26 bytes, then the step lists) |
| `[1]` | the sprite data list: 6-byte rows `MapObjectsSpriteDataRow` `{u16 sprite_data, u16 length, u16 place}` (4-byte units, the place counted from the list's start), ended by a number of 0, followed by the packed sprite data |
| `[2..]` | the packed texture pages, one per word, ended by a word of 0 |

Every packed stream is unpacked by `inflate_buffer` (`0x80021D6C`)
([../formats/compression.md](../formats/compression.md)). Page n goes to
sprite page slot n (`g_SpritePageCorners[n]`), 0x80 halfwords by 256 lines.
