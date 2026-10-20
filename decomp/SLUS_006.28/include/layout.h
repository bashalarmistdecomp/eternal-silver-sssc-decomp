/* layout.h: compile-time checks of a struct's layout. A header states its
   struct's size and each field's offset with these; a wrong one stops the
   build (an array of size -1), and no code or data comes of them.
   (docs/naming.md, "Structs and their fields") */
#ifndef LAYOUT_H
#define LAYOUT_H

#define LAYOUT_OFFSET(type, field) ((int)&((type *)0)->field)
#define LAYOUT_SIZE(type, size) typedef char type##_layout_sizeof[(sizeof(type) == (size)) ? 1 : -1]
#define LAYOUT_AT(type, name, field, offset) \
    typedef char type##_layout_##name[(LAYOUT_OFFSET(type, field) == (offset)) ? 1 : -1]

/* A field read or written as another type than its own, where the match needs
   it: AS(u16, record->sprite.depth), AS(u8 *, record->sprite.pose_table).
   The access stays a member access (GCC 2.8 schedules a member access and a
   plain pointer access differently against the stores round it), and only
   its type changes. */
#define AS(type, field) (((struct { type v; } *)&(field))->v)

#endif
