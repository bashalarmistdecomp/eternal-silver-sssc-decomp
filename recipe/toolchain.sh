#!/usr/bin/env bash
# Put the toolchain the build needs into toolchain/ (not in git).
#
#   recipe/toolchain.sh             everything the build needs that is missing
#   recipe/toolchain.sh check       say what is there; build nothing
#   recipe/toolchain.sh <piece>...  binutils | gcc | gcc-970318 | maspsx | psyq
#   recipe/toolchain.sh sdk         the optional piece: Sony's SDK libraries
#
# Every piece is fetched from where its authors publish it, at a pinned
# version, and checked against a hash recorded here before it is used:
#
#   binutils/         GNU binutils 2.42 for mipsel-linux-gnu, built from
#                     source: the assembler, the linker, objcopy.
#   gcc-2.8.1-psx/    the game's compiler, GCC 2.8.1 as decompals/old-gcc
#   gcc-2.7.2-psx/    builds it, and 2.7.2 for the SDK library objects that
#                     were compiled with the older one. The 2.8.1 cc1 then
#                     gets one 8-byte fix (recipe/patch_cc1.py says why).
#   gcc-970318-psx/   the compiler of the SDK 4.5+ libraries: a GCC snapshot
#                     of 1997-03-18, built in docker (recipe/gcc-970318/).
#   maspsx/           mkst/maspsx, which adapts the old compilers' output
#                     to GNU as the way Sony's ASPSX assembled it.
#   psyq/4.4/         Sony's PSY-Q SDK headers (release 4.7, in a folder
#                     named 4.4 where they come from), which the C includes.
#                     They are Sony's; this repository does not carry them.
#                     Set PSYQ_INCLUDE to a copy of your own to skip the
#                     fetch (the build reads the same variable).
#
# One more piece is fetched only when asked for (`sdk`), because only
# `make CODE=sdk` reads it:
#
#   psyq-4.5/lib/    five files of Sony's PSY-Q 4.5, the release these
#                     programs link, as Sony shipped them (libapi.lib,
#                     libc2.lib, libcard.lib, libgte.lib and the start-up
#                     object 2mbyte.obj). The routines of
#                     the programs that were assembly in the SDK are in them
#                     byte for byte, so the build can take them from here
#                     instead of from your disc. They are Sony's; this
#                     repository does not carry them. The stubs under
#                     decomp/<serial>/from-sdk/ name offsets inside these
#                     exact files, so a copy of your own (PSYQ_SDK) must be
#                     these same five files; each is checked by its hash.
#
# Needs: curl, tar, git, python3, make and a C compiler (binutils); docker
# with BuildKit and bison 3 (gcc-970318). The compilers are static 32-bit
# x86 programs: the kernel must run those (CONFIG_IA32_EMULATION).
#
# TOOLCHAIN_DIR puts all of it somewhere else; the Makefile and the build
# scripts read the same variable.
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
tc=${TOOLCHAIN_DIR:-$root/toolchain}
case "$tc" in /*) ;; *) tc="$root/$tc" ;; esac

BINUTILS_VERSION=2.42
# identical from sourceware and kernel.org (2026-10-06)
BINUTILS_SHA256=f6e4d41fd5fc778b06b7891457b3620da5ecea1006c6a4a41ae998109f85a800
OLDGCC_RELEASE=0.17
GCC281_TAR_SHA256=f6f6e883942d4d3289d048236c672e71ed410e546aaae8ff655952f1567e1be0
GCC281_CC1_SHA256=1a48ec5ac87a146a074a9d66d7a67734a4d1ad56553d02d91e8b71c7fecc2d52
GCC281_CC1_PATCHED_SHA256=bae32a66a51f24650db47f3bca1871b78f1fe7a9a2f9345d3d3fb7eef05d3125
GCC272_TAR_SHA256=500a459b3485e885a8d302cac23c2a4632f3900e03a09153f6190699fd723571
GCC272_CC1_SHA256=0359379289db8e3904b8ed3b25b422ca631c8d71ab2f543d533c39ed2967764f
GCC970318_CC1_SHA256=61bf7d9dae9809c54fdf2c5beced57a65c9e27dfe6453bb42f2790303deb8217
MASPSX_COMMIT=42b862c988fe7a13fe4e7ac0ebec90ed6b9fb763
# Zackmon/lunar2-psx-decomp, the public tree the headers were taken from
PSYQ_REPO=https://github.com/Zackmon/lunar2-psx-decomp
PSYQ_COMMIT=3798b47d77a4e4e8008102345ba1cd6aa0f661bb
# sha256 over `sha256sum` of the 56 files of include/psyq4.4, C-locale order
PSYQ_TREE_SHA256=a661d7154d08ef7bdce85a2b5f6e5dbd459b69130aa1b44547a25a475983d518
# Five files of PSY-Q 4.5, in Sony's own formats, from the public tree
# FoxdieTeam/psyq_sdk (its psyq_4.5/lib): four libraries and the start-up
# object. 4.5 is the release these programs link (docs/naming.md says how
# that was measured). name:sha256
SDK_REPO=https://github.com/FoxdieTeam/psyq_sdk
SDK_COMMIT=91719fa5bdca0b7e55c5c4b9e045da407db93510
SDK_FILES="
libapi.lib:efc557a4cc234fe07f01798f61a2511f556614a1ea56039b4fa225b46c8df33d
libc2.lib:1fd54df981ceb368d00e2e5db4a180b40513ef3ba7b21bbd99d0134ce55e860c
libcard.lib:f0c6be3ec56a4ffc8fb2c2e44d0af95f7e04d2e1ad1f4bb8e848096cd58ca79d
libgte.lib:2bfe86a7170d832c75403406b394ac0a75fd0ecf19e0e0d70b1e0b2e68ae1300
2mbyte.obj:b4ed8b2e76bc841f7a81132906d846877aefa8f6233fb738600c8b1ebd032a46
"
sdk=${PSYQ_SDK:-$tc/psyq-4.5}

sha() { sha256sum "$1" | cut -d' ' -f1; }
tree_sha() { (cd "$1" && find . -type f | LC_ALL=C sort | xargs sha256sum | sha256sum | cut -d' ' -f1); }
say() { printf '%s\n' "$*"; }
die() { printf 'toolchain.sh: %s\n' "$*" >&2; exit 1; }

have_binutils() { [ -x "$tc/binutils/bin/mipsel-linux-gnu-as" ] \
  && "$tc/binutils/bin/mipsel-linux-gnu-as" --version 2>/dev/null | grep -q "Binutils) $BINUTILS_VERSION"; }
have_gcc() { [ -f "$tc/gcc-2.8.1-psx/cc1" ] && [ -f "$tc/gcc-2.7.2-psx/cc1" ] \
  && [ "$(sha "$tc/gcc-2.8.1-psx/cc1")" = "$GCC281_CC1_PATCHED_SHA256" ] \
  && [ "$(sha "$tc/gcc-2.7.2-psx/cc1")" = "$GCC272_CC1_SHA256" ]; }
have_gcc970318() { [ -x "$tc/gcc-970318-psx/cc1" ]; }
have_maspsx() { [ -f "$tc/maspsx/maspsx.py" ] \
  && [ "$(git -C "$tc/maspsx" rev-parse HEAD 2>/dev/null)" = "$MASPSX_COMMIT" ]; }
have_psyq() { [ -n "${PSYQ_INCLUDE:-}" ] && [ -f "$PSYQ_INCLUDE/libgpu.h" ] && return 0
  [ -f "$tc/psyq/4.4/libgpu.h" ] && [ "$(tree_sha "$tc/psyq/4.4")" = "$PSYQ_TREE_SHA256" ]; }
have_sdk() { local e; for e in $SDK_FILES; do
    [ -f "$sdk/lib/${e%%:*}" ] && [ "$(sha "$sdk/lib/${e%%:*}")" = "${e##*:}" ] || return 1
  done; }

do_binutils() {
  have_binutils && { say "binutils: present"; return; }
  say "binutils: building $BINUTILS_VERSION for mipsel-linux-gnu (a few minutes)"
  local work t="binutils-$BINUTILS_VERSION.tar.xz"
  work=$(mktemp -d); mkdir -p "$tc"
  # Mirrors in order, so no single host is load-bearing.
  for m in https://sourceware.org/pub/binutils/releases \
           https://mirrors.kernel.org/gnu/binutils \
           https://ftp.gnu.org/gnu/binutils; do
    curl -sSfL --connect-timeout 20 -o "$work/$t" "$m/$t" && break || rm -f "$work/$t"
  done
  [ -f "$work/$t" ] || die "could not download $t"
  echo "$BINUTILS_SHA256  $work/$t" | sha256sum -c --quiet
  tar xJf "$work/$t" -C "$work"
  mkdir "$work/build"
  # -march=x86-64: a distribution whose compiler defaults to a newer x86-64
  # level stamps the binaries with it, and they then refuse to start on any
  # older CPU. MAKEINFO=true: the tarball tries to regenerate bfd.info and
  # dies without texinfo; the manuals are not part of the toolchain.
  ( cd "$work/build" \
    && CFLAGS="-O2 -std=gnu17 -march=x86-64 -mtune=generic" \
       CXXFLAGS="-O2 -march=x86-64 -mtune=generic" \
       "../binutils-$BINUTILS_VERSION/configure" \
         --target=mipsel-linux-gnu --prefix="$tc/binutils" \
         --disable-nls --disable-werror --disable-gdb --disable-gdbserver \
         --disable-sim --disable-gprofng >configure.log 2>&1 \
    && make -j"$(nproc)" MAKEINFO=true >make.log 2>&1 \
    && make install MAKEINFO=true >install.log 2>&1 ) \
    || die "binutils build failed; logs are in $work/build"
  rm -rf "$work"
  have_binutils || die "binutils built but does not run"
  say "binutils: $("$tc/binutils/bin/mipsel-linux-gnu-as" --version | head -1)"
}

fetch_oldgcc() {  # name, tarball sha256, cc1 sha256
  local name=$1 tar_sha=$2 cc1_sha=$3 t
  [ -f "$tc/$name/cc1" ] && return 0
  t=$(mktemp)
  curl -fsSL -o "$t" \
    "https://github.com/decompals/old-gcc/releases/download/$OLDGCC_RELEASE/$name.tar.gz"
  echo "$tar_sha  $t" | sha256sum -c --quiet
  mkdir -p "$tc/$name"
  tar xzf "$t" -C "$tc/$name"
  rm -f "$t"
  [ "$(sha "$tc/$name/cc1")" = "$cc1_sha" ] || die "$name/cc1 is not the released one"
}

do_gcc() {
  have_gcc && { say "gcc 2.8.1 + 2.7.2: present"; return; }
  say "gcc: fetching decompals/old-gcc $OLDGCC_RELEASE (2.8.1 and 2.7.2)"
  fetch_oldgcc gcc-2.8.1-psx "$GCC281_TAR_SHA256" "$GCC281_CC1_SHA256"
  fetch_oldgcc gcc-2.7.2-psx "$GCC272_TAR_SHA256" "$GCC272_CC1_SHA256"
  TOOLCHAIN_DIR="$tc" python3 "$here/patch_cc1.py" --apply
  have_gcc || die "gcc: the compilers on disk are not the pinned ones"
}

do_gcc970318() {
  have_gcc970318 && { say "gcc-970318: present"; return; }
  say "gcc-970318: building the 1997-03-18 snapshot in docker (several minutes)"
  TOOLCHAIN_DIR="$tc" "$here/gcc-970318/build.sh"
}

do_maspsx() {
  have_maspsx && { say "maspsx: present"; return; }
  say "maspsx: fetching mkst/maspsx at ${MASPSX_COMMIT:0:8}"
  rm -rf "$tc/maspsx"; mkdir -p "$tc"
  git init -q "$tc/maspsx"
  git -C "$tc/maspsx" remote add origin https://github.com/mkst/maspsx.git
  git -C "$tc/maspsx" fetch -q --depth 1 origin "$MASPSX_COMMIT"
  git -C "$tc/maspsx" checkout -q FETCH_HEAD
  have_maspsx || die "maspsx: checkout failed"
}

do_psyq() {
  if [ -n "${PSYQ_INCLUDE:-}" ]; then
    [ -f "$PSYQ_INCLUDE/libgpu.h" ] || die "PSYQ_INCLUDE=$PSYQ_INCLUDE has no libgpu.h"
    say "psyq: using your copy at $PSYQ_INCLUDE"; return
  fi
  have_psyq && { say "psyq headers: present"; return; }
  say "psyq: fetching the PSY-Q headers from $PSYQ_REPO at ${PSYQ_COMMIT:0:8}"
  local work; work=$(mktemp -d)
  curl -fsSL -o "$work/src.tar.gz" "$PSYQ_REPO/archive/$PSYQ_COMMIT.tar.gz"
  tar xzf "$work/src.tar.gz" -C "$work" --strip-components=1 \
      --wildcards '*/include/psyq4.4/*'
  [ "$(tree_sha "$work/include/psyq4.4")" = "$PSYQ_TREE_SHA256" ] \
    || die "psyq: the fetched headers are not the ones the match was proven with"
  rm -rf "$tc/psyq/4.4"; mkdir -p "$tc/psyq"
  mv "$work/include/psyq4.4" "$tc/psyq/4.4"
  rm -rf "$work"
}

do_sdk() {
  if [ -n "${PSYQ_SDK:-}" ]; then
    have_sdk || die "PSYQ_SDK=$PSYQ_SDK does not hold lib/libapi.lib, libc2.lib, libcard.lib, libgte.lib and 2mbyte.obj of PSY-Q 4.5, the files the stubs were written against"
    say "sdk: using your copy at $PSYQ_SDK"; return
  fi
  have_sdk && { say "sdk libraries: present"; return; }
  say "sdk: fetching five PSY-Q 4.5 files from $SDK_REPO"
  local work e name; work=$(mktemp -d)
  for e in $SDK_FILES; do
    name=${e%%:*}
    curl -fsSL -o "$work/$name" "${SDK_REPO/github.com/raw.githubusercontent.com}/$SDK_COMMIT/psyq_4.5/lib/$name" \
      || die "sdk: download of $name failed; set PSYQ_SDK to a folder with a copy of your own in lib/"
    echo "${e##*:}  $work/$name" | sha256sum -c --quiet \
      || die "sdk: $name is not the file the stubs were written against"
  done
  rm -rf "$sdk"; mkdir -p "$sdk/lib"
  mv "$work"/* "$sdk/lib/"
  rm -rf "$work"
}

check() {
  local bad=0
  line() { if "$1"; then say "  ok       $2"; else say "  MISSING  $2"; bad=1; fi; }
  say "toolchain at $tc:"
  line have_binutils  "binutils $BINUTILS_VERSION (mipsel-linux-gnu)"
  line have_gcc       "gcc 2.8.1 (patched) and gcc 2.7.2"
  line have_gcc970318 "gcc snapshot 1997-03-18"
  line have_maspsx    "maspsx ${MASPSX_COMMIT:0:8}"
  line have_psyq      "PSY-Q headers${PSYQ_INCLUDE:+ (your copy: $PSYQ_INCLUDE)}"
  if have_sdk; then say "  ok       PSY-Q 4.5 library files, for \`make CODE=sdk\`${PSYQ_SDK:+ (your copy: $PSYQ_SDK)}"
  else say "  -        PSY-Q 4.5 library files: not there; only \`make CODE=sdk\` needs them (recipe/toolchain.sh sdk)"; fi
  if have_gcc970318 && [ "$(sha "$tc/gcc-970318-psx/cc1")" != "$GCC970318_CC1_SHA256" ]; then
    say "  note     gcc-970318-psx/cc1 is not byte-identical to the measured build;"
    say "           \`make\` ending with OK is what proves it"
  fi
  if have_gcc && ! printf 'int f(int a){return a+1;}\n' \
       | "$tc/gcc-2.8.1-psx/cc1" -quiet -O2 -G8 -o /dev/null >/dev/null 2>&1; then
    say "  BROKEN   the compilers do not run here: they are static 32-bit x86"
    say "           programs and need a kernel with IA32 emulation"
    bad=1
  fi
  return $bad
}

[ $# -gt 0 ] || set -- all
for piece in "$@"; do
  case "$piece" in
    check)      check; exit $? ;;
    all)        do_binutils; do_gcc; do_gcc970318; do_maspsx; do_psyq ;;
    binutils)   do_binutils ;;
    gcc)        do_gcc ;;
    gcc-970318) do_gcc970318 ;;
    maspsx)     do_maspsx ;;
    psyq)       do_psyq ;;
    sdk)        do_sdk ;;
    *)          die "unknown piece '$piece' (binutils gcc gcc-970318 maspsx psyq sdk check)" ;;
  esac
done
check
