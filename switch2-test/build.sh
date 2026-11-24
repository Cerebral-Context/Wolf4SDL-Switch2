#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"
CC="$DEVKITPRO/devkitA64/bin/aarch64-none-elf-gcc"
CXX="$DEVKITPRO/devkitA64/bin/aarch64-none-elf-g++"
ELF2NRO="$DEVKITPRO/tools/bin/elf2nro"

BUILD="gpl-build"

mkdir -p "$BUILD/compat"

COMMON_FLAGS=(
    -mcpu=cortex-a78c+crypto
    -mtp=soft
    -fPIE
    -O2
    -ffunction-sections
    -fdata-sections
    -D__SWITCH__
    -D__SWITCH2__
    -Icompat
    -Igame-src
    -I.
    -I"$DEVKITPRO/libounce/include"
    -I"$DEVKITPRO/portlibs/switch2/include"
)

OBJECTS=()

echo "=== Compiling SDL compatibility layer ==="

for source in compat/*.c; do
    object="$BUILD/compat/$(basename "${source%.c}").o"
    echo "Compiling $source"
    "$CC" "${COMMON_FLAGS[@]}" -std=gnu11 \
        -c "$source" -o "$object"
    OBJECTS+=("$object")
done

echo "=== Compiling video backend ==="

"$CC" "${COMMON_FLAGS[@]}" -std=gnu11 \
    -c wolf_video.c -o "$BUILD/wolf_video.o"

OBJECTS+=("$BUILD/wolf_video.o")

echo "=== Compiling Wolf4SDL engine ==="

for source in game-src/*.cpp \
              game-src/dosbox/dbopl.cpp; do

    object="$BUILD/$(basename "${source%.cpp}").o"

    echo "Compiling $source"

    "$CXX" "${COMMON_FLAGS[@]}" \
        -std=gnu++11 -fpermissive -include compat/wolf_switch2_input.h \
        -c "$source" -o "$object"

    OBJECTS+=("$object")
done

echo "=== Linking Wolfenstein 3D ==="

"$CXX" \
    -mcpu=cortex-a78c+crypto \
    -mtp=soft \
    -fPIE \
    -specs="$DEVKITPRO/libounce/switch.specs" \
    -L"$DEVKITPRO/libounce/lib" \
    -L"$DEVKITPRO/portlibs/switch2/lib" \
    "${OBJECTS[@]}" \
    -lounce -lm \
    -o "$BUILD/Wolf3d.elf"

"$ELF2NRO" \
    "$BUILD/Wolf3d.elf" \
    "$BUILD/Wolf3d.nro"

echo "=== BUILD SUCCESSFUL ==="
ls -lh "$BUILD/Wolf3d.nro"
