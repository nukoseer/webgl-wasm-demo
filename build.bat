@echo off

IF NOT EXIST build mkdir build
pushd build

set debug_compiler_flags=
set release_compiler_flags=-flto -O3
set common_compiler_flags=--target=wasm32 -nostdlib -Wall -Werror -Wno-unused-function

set debug_linker_flags=
set release_linker_flags=-Wl,--lto-O3
set common_linker_flags=-Wl,--no-entry -Wl,--allow-undefined -Wl,--export=init

set debug=yes

if %debug%==yes (
   set common_compiler_flags=%common_compiler_flags% %debug_compiler_flags%
   set common_linker_flags=%common_linker_flags% %debug_linker_flags%
) else (
   set common_compiler_flags=%common_compiler_flags% %release_compiler_flags%
   set common_linker_flags=%common_linker_flags% %release_linker_flags%
)

clang %common_compiler_flags% %common_linker_flags% -o demo.wasm ../demo.c

popd
