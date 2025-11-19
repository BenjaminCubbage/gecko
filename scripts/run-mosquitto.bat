@echo off

for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/../run/local.env") do (
    set "%%A=%%B"
)

if not exist "%DYNAMIC_SECURITY_CONF_PATH%" (
    echo [%~n0]: No dynamic security configuration found. Running mosquitto_ctrl dynsec init.

    for %%i in ("%DYNAMIC_SECURITY_CONF_PATH%") do ( mkdir %%~dpi )

    "%MOSQUITTO_CTRL_EXE_PATH%" dynsec init "%DYNAMIC_SECURITY_CONF_PATH%" admin
    echo [%~n0]: Initialized dynamic security at %DYNAMIC_SECURITY_CONF_PATH%
)

if exist "%MOSQUITTO_PID_PATH%" (
    for /f %%A in ('"%CMAKE_COMMAND%" -E cat %MOSQUITTO_PID_PATH%') do (
        echo [%~n0]: Killing old mosquitto.exe instance with PID %%A. This required administrator priviledges.
        taskkill /F /PID %%A
    )
)

:: This version of Mosquitto (2.0.18) seems to have a bug where the dynamic security plugin doesn't correctly
:: copy its updated temporary dynamic security config file to the original on exit. This is a hack to simulate
:: that behavior, except reversed: we copy it over before starting, rather than after stopping.

:: That said, it's impossible to cover all edge-cases here without knowing what they really are. Is dynsec.json.new
:: always going to hold actually new information? I don't know. I'm not a Mosquitto contributor.

:: To account for this, I also create a backup to dynsec.json.old here before overwriting dynsec.json with the new
:: data. That way, If dynsec.json.new ever contains "dirty" data, or is empty, the config can be recovered manually.
:: I don't expect this to happen, it's just a precaution.
if exist "%DYNAMIC_SECURITY_CONF_PATH%.new" (
    echo [%~n0]: Storing backup at %DYNAMIC_SECURITY_CONF_PATH% -^> %DYNAMIC_SECURITY_CONF_PATH%.old
    echo [%~n0]: Copying %DYNAMIC_SECURITY_CONF_PATH%.new       -^> %DYNAMIC_SECURITY_CONF_PATH%
    "%CMAKE_COMMAND%" -E copy_if_different "%DYNAMIC_SECURITY_CONF_PATH%"     "%DYNAMIC_SECURITY_CONF_PATH%.old"
    "%CMAKE_COMMAND%" -E copy_if_different "%DYNAMIC_SECURITY_CONF_PATH%.new" "%DYNAMIC_SECURITY_CONF_PATH%"
)

echo [%~n0]: Starting %MOSQUITTO_EXE_PATH% with configuration file %MOSQUITTO_CONF_PATH%
"%MOSQUITTO_EXE_PATH%" -c %MOSQUITTO_CONF_PATH% %*
