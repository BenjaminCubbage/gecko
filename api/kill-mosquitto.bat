@echo off

for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/scripts.env") do (
    set "%%A=%%B"
)

:: Kill whatever process id was stored at "mosquitto-pid.temp"
"%CMAKE_COMMAND%" -E touch "%MOSQUITTO_PID_PATH%"
for /f %%p in ('"%CMAKE_COMMAND%" -E cat %MOSQUITTO_PID_PATH%') do (
    taskkill /F /PID %%p
)
exit /b 0