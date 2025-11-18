@echo off
setlocal enabledelayedexpansion

for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/scripts.env") do (
    set "%%A=%%B"
)

set FILESIZE=0
if exist "%~dp0/dynsec.json" (
    for %%A in ("%~dp0/dynsec.json") do set FILESIZE=%%~zA

    if not !FILESIZE!==0 (
        echo Dynsec config file at %DYNAMIC_SECURITY_CONF_PATH% already exists!
        exit /b 0
    )
)

call "%~dp0/mosquitto-ctrl" dynsec init "%~dp0/dynsec.json" admin