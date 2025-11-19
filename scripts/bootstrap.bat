@echo off

set DPRODUCTION=
if "%1"=="production"  ( set DPRODUCTION="-DPRODUCTION=ON")
if "%1"=="prod"        ( set DPRODUCTION="-DPRODUCTION=ON")
if "%1"=="development" ( set DPRODUCTION="-DPRODUCTION=OFF")
if "%1"=="dev"         ( set DPRODUCTION="-DPRODUCTION=OFF")

set INPUT_GENERATOR=
if not defined INPUT_GENERATOR (
    if not exist "%~dp0/../build/CMakeCache.txt" (
        echo Please specify your CMake Generator: ^(ENTER for default^)
        set /p INPUT_GENERATOR=
    )
)

if not defined PERSISTENT_DIR (
    set PERSISTENT_DIR="%~dp0/../run"
)

:: Bash is a great scripting la-
if not defined INPUT_GENERATOR ( cmd /c "cd /d ""%~dp0/.."" && cmake -B build -S .                        -DCMAKE_BUILD_TYPE=Release %DPRODUCTION% && cmake --build build --target all_external_dependencies --config Release" )
if     defined INPUT_GENERATOR ( cmd /c "cd /d ""%~dp0/.."" && cmake -B build -S . -G "%INPUT_GENERATOR%" -DCMAKE_BUILD_TYPE=Release %DPRODUCTION% && cmake --build build --target all_external_dependencies --config Release" )

:: Build primary gecko target
cmd /c "cd /d ""%~dp0/.."" && cmake --build build --target gecko --config Release"

:: Populate the environment variables written by the CMake build to /run/local.env
:: The local.env.in template is at /cmake/
for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/../run/local.env") do (
    set "%%A=%%B"
)