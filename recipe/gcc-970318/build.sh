#!/usr/bin/env bash
# Build the SDK 4.5+ library compiler -> toolchain/gcc-970318-psx/cc1.
#
#   recipe/gcc-970318/build.sh        (recipe/toolchain.sh runs it)
#
# compile.py's `cc97_lib` recipe runs this compiler (with -mhard-float
# -fno-exceptions and the library post-passes); nothing else does, and that
# recipe refuses loudly when this cc1 is missing.
#
# WHAT IT IS. Not a release: the GCC development tree as of 1997-03-18
# (gcc-mirror/gcc commit 2b3600ac), between the 2.7.2 and 2.8.0 releases.
# Dated by measurement on libmcrd's _card_create2 (LOW.OBJ, first shipped
# in SDK 4.5) — the same plain C, compiled by four snapshots of the tree:
#
#   4757e6a4  1997-02-05 00:29   400/429   one commit BEFORE cabcf079
#   cabcf079  1997-02-05 16:20   MATCH     "Don't always align reload stack
#                                           slots to BIGGEST_ALIGNMENT"
#   2b3600ac  1997-03-18 18:38   MATCH     one commit before 38b610ed
#   f75487ff  1997-03-18 20:00   396/429   38b610ed "Add a REG_EQUIV note
#                                           when a register parameter has a
#                                           stack location" is in
#
# so Sony's compiler was cut from that six-week window: 4-byte reload
# slots already, a spilled register parameter still homed in the callee's
# frame. Its loop.c is the 1996 one (Wilson's maybe_multiple fix of
# 1996-01-24, Rennecke's "DEST_REG giv with its only use" benefit of
# 1996-09/10), which no 2.7.2.x release has: 2.7.2 strength-reduces
# index loops this compiler leaves alone, and the reverse.
#
# Inputs (fetched into toolchain/gcc-970318-src/ when absent): the
# snapshot tree, the GNU 2.7.2 tarball for the files only a release
# carries, and decompals/old-gcc at a pinned commit for its psx patches.
# Needs docker (BuildKit) and, on the host, bison 3 (genparsers.py).
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../.." && pwd)
tc=${TOOLCHAIN_DIR:-$root/toolchain}
src="$tc/gcc-970318-src"
out="$tc/gcc-970318-psx"
snap=2b3600acb4eceda3000312002ec78fecd5a39d30
tar_sha=7cd8bce5c3aeec59a72ecc2d3d5123864a817b14cdbd0680b1a969c3bccc5da5
# sha256 over `sha256sum` of every file of the snapshot's gcc/ directory,
# in C-locale path order — the tree itself, not GitHub's packaging of it.
tree_sha=00695436c7ca42871205b8f566ddf15bf4fd5890e03570b4b9a0ba0173efa71b
# decompals/old-gcc as it stood when the measurements above were made
oldgcc=b74211c9d959e9724802f3177c8229cd67202c87
# The cc1 every measurement above was made with (parsers by bison 3.8.2).
# A rebuild that differs is not necessarily wrong (another bison, image
# drift) but must be re-proven before use — see below.
measured_sha=61bf7d9dae9809c54fdf2c5beced57a65c9e27dfe6453bb42f2790303deb8217

command -v bison >/dev/null || { echo "bison not found (genparsers.py)" >&2; exit 1; }
command -v docker >/dev/null || { echo "docker not found" >&2; exit 1; }

mkdir -p "$src"
if [ ! -f "$src/gcc-2.7.2.tar.gz" ]; then
  curl -fsSL https://ftp.gnu.org/old-gnu/gcc/gcc-2.7.2.tar.gz \
    -o "$src/gcc-2.7.2.tar.gz"
fi
echo "$tar_sha  $src/gcc-2.7.2.tar.gz" | sha256sum -c --quiet
if [ ! -f "$src/gcc-$snap.tar.gz" ]; then
  curl -fsSL "https://github.com/gcc-mirror/gcc/archive/$snap.tar.gz" \
    -o "$src/gcc-$snap.tar.gz"
fi
if [ ! -d "$src/old-gcc/patches" ]; then
  git init -q "$src/old-gcc"
  git -C "$src/old-gcc" remote add origin https://github.com/decompals/old-gcc.git
  git -C "$src/old-gcc" fetch -q --depth 1 origin "$oldgcc"
  git -C "$src/old-gcc" checkout -q FETCH_HEAD
fi

ctx=$(mktemp -d)
trap 'rm -rf "$ctx"' EXIT
mkdir "$ctx/tree"
tar xzf "$src/gcc-$snap.tar.gz" -C "$ctx/tree" --strip-components=1
got=$(cd "$ctx/tree/gcc" && find . -type f | LC_ALL=C sort | xargs sha256sum | sha256sum | cut -d' ' -f1)
if [ "$got" != "$tree_sha" ]; then
  echo "snapshot tree $got is not the measured $tree_sha" >&2; exit 1
fi
mv "$ctx/tree/gcc" "$ctx/snap"
rm -rf "$ctx/tree"
python3 "$here/genparsers.py" "$ctx/snap"
cp "$src/gcc-2.7.2.tar.gz" "$ctx/"
cp -r "$src/old-gcc/patches" "$ctx/patches"
cp -r "$src/old-gcc/tests" "$ctx/tests"
cp "$here/Dockerfile" "$ctx/"

mkdir -p "$out"
DOCKER_BUILDKIT=1 docker build --target export \
  -o "type=local,dest=$out" "$ctx"
chmod +x "$out/cc1"
got=$(sha256sum "$out/cc1" | cut -d' ' -f1)
echo "$got  $out/cc1"
if [ "$got" = "$measured_sha" ]; then
  echo "identical to the measured cc1"
else
  echo "DIFFERS from the measured cc1 ($measured_sha): another bison or a" \
       "drifted base image can do that. The build is the judge — run" \
       "\`make\` and trust the compiler only if it ends with OK." >&2
fi
