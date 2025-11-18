@echo off

for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/scripts.env") do (
    set "%%A=%%B"
)

"%MOSQUITTO_CTRL_PATH%" %*