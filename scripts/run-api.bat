@echo off

for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/../run/local.env") do (
    set "%%A=%%B"
)

echo [%~n0]: Starting %GECKO_API_EXE_PATH%...
"%GECKO_API_EXE_PATH%" %*