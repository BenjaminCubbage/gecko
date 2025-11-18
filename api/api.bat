@echo off

for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/scripts.env") do (
    set "%%A=%%B"
)

"%CMAKE_RUNTIME_OUTPUT_DIRECTORY%/api"