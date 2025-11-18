@echo off
for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/scripts.env") do (
    set "%%A=%%B"
)

echo Running %MOSQUITTO_PATH% with config file %MOSQUITTO_CONF_PATH%
"%MOSQUITTO_PATH%" -c "%MOSQUITTO_CONF_PATH%" %*