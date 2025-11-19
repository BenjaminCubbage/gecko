@echo off

for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/../run/local.env") do (
    set "%%A=%%B"
)

if not exist "%DYNAMIC_SECURITY_CONF_PATH%" (
    echo [%~n0]: No dynamic security configuration found. Running mosquitto_ctrl dynsec init.
    "%MOSQUITTO_CTRL_EXE_PATH%" dynsec init "%DYNAMIC_SECURITY_CONF_PATH%" admin
)

if exist "%MOSQUITTO_PID_PATH%" (
    for /f %%A in ('"%CMAKE_COMMAND%" -E cat %MOSQUITTO_PID_PATH%') do (
        echo [%~n0]: Killing old mosquitto.exe instance with PID %%A. This required administrator priviledges.
        taskkill /F /PID %%A
    )
)

echo [%~n0]: Starting %MOSQUITTO_EXE_PATH% with configuration file %MOSQUITTO_CONF_PATH%
"%MOSQUITTO_EXE_PATH%" -c %MOSQUITTO_CONF_PATH% %*