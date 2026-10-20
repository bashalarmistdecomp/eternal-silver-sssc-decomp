# The SDK 4.5+ library compiler: a GCC mainline snapshot of 1997-03-18
# (between the 2.7.2 and 2.8.0 releases), built the way decompals/old-gcc
# builds gcc-2.7.2-psx. compile.py's `cc97_lib` recipe runs it; build.sh
# stages this context and has the measurements that date the snapshot.
#
# The context holds: the GNU 2.7.2 tarball, old-gcc's patches/ and tests/,
# and snap/ — the snapshot's gcc/ directory with its parsers generated
# (genparsers.py). A version-control snapshot lacks what a release tarball
# adds (configure, version.c, obstack, the host config headers of the
# day); those come from the 2.7.2 release, never overwriting a file the
# snapshot has. ONE deliberate edit to the compiler proper, checked below:
# mips_split_addresses stays 0. This snapshot splits a symbol address into
# %hi/%lo whenever the target assembler is GAS, with no switch to turn it
# off (-mno-split-addresses is later); Sony's objects have the assembler's
# own unsplit `lw $2,sym($3)` forms.
# 2.7.2's hand-written configure predates two Makefile.in conventions of
# the snapshot (fragment paths in the Makefile dependency, target_alias);
# the generated Makefile is fixed up after configure.
FROM ubuntu:focal AS build
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y build-essential gcc gcc-multilib

WORKDIR /work
COPY gcc-2.7.2.tar.gz /work/
RUN tar xzf gcc-2.7.2.tar.gz
COPY patches /work/patches
COPY snap /work/snap

WORKDIR /work/snap
RUN for f in ../gcc-2.7.2/*; do b=$(basename $f); case $b in \
      c-parse.c|c-parse.y|c-parse.h|cexp.c|bi-parser.c|bi-parser.h|objc-parse.c|objc-parse.y|objc|*.info*|ChangeLog*|TAGS) continue;; esac; \
      [ -e $b ] || cp -r $f $b; done \
 && cp ../gcc-2.7.2/config.sub config.sub \
 && cp -rn ../gcc-2.7.2/config/. config/
RUN sed -i -- 's/include <varargs.h>/include <stdarg.h>/g' *.c

RUN patch -u -p1 obstack.h -i ../patches/obstack-2.7.2.h.patch
RUN patch -u -p1 configure -i ../patches/configure.patch
RUN patch -u -p1 config.sub -i ../patches/config.sub.patch
RUN patch -u -p1 config/mips/mips.h -i ../patches/mipsel-2.7.patch
RUN patch -su -p1 < ../patches/psx-2.5.7.patch
RUN sed -i 's/mips_split_addresses = 1;/mips_split_addresses = 0;/' config/mips/mips.c \
 && test "$(grep -c 'mips_split_addresses = 0;' config/mips/mips.c)" = 2

RUN ./configure \
    --target=mips-sony-psx \
    --prefix=/opt/cross \
    --with-endian-little \
    --with-gnu-as \
    --disable-gprof \
    --disable-gdb \
    --disable-werror \
    --host=i386-pc-linux \
    --build=i386-pc-linux
RUN sed -i 's|^   $(xmake_file) $(tmake_file) \\$|   \\|' Makefile \
 && sed -i 's|^target_alias=.*|target_alias=mips-sony-psx|' Makefile \
 && ! grep -q "configure' substitutes" Makefile

# Serial on purpose: this Makefile.in does not order every object after
# the generated insn-*.h it includes, and a parallel build loses the race.
RUN make cc1 LANGUAGES=c CFLAGS="-std=gnu89 -m32 -static"

COPY tests /work/tests
RUN ./cc1 -quiet -O2 /work/tests/little_endian.c && grep -E 'lbu\s\$2,0\(\$4\)' /work/tests/little_endian.s
RUN ./cc1 -quiet -help</dev/null 2>&1 | grep -- -msoft-float
# the recipe's own flags, and the property the mips.c edit is there for
RUN printf 'extern int g[];\nint f(int i){return g[i];}\n' > /work/t.c \
 && ./cc1 -quiet -O2 -G8 -mhard-float -fno-exceptions /work/t.c -o /work/t.s \
 && grep -E 'lw\s\$2,g\(\$4\)' /work/t.s

RUN mkdir /build && cp cc1 /build/

FROM scratch AS export
COPY --from=build /build/cc1 .
