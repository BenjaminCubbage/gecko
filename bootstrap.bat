@echo off

set INPUT_GENERATOR=

if not exist "%~dp0/build/CMakeCache.txt" (
    echo Please specify your CMake Generator: ^(ENTER for default^)
    set /p INPUT_GENERATOR=
)

if not defined INPUT_GENERATOR ( cmd /c "cd /d ""%~dp0"" && cmake -B build -S .                        -DCMAKE_BUILD_TYPE=Release && cmake --build build --target all_external_dependencies --config Release" )
if     defined INPUT_GENERATOR ( cmd /c "cd /d ""%~dp0"" && cmake -B build -S . -G "%INPUT_GENERATOR%" -DCMAKE_BUILD_TYPE=Release && cmake --build build --target all_external_dependencies --config Release" )
