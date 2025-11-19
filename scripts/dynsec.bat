@echo off
setlocal enabledelayedexpansion

for /f "usebackq tokens=1,2 delims==" %%A in ("%~dp0/../run/local.env") do (
    set "%%A=%%B"
)

if "%1"=="create-server-client" (
    echo [%~n0]: Creating server client. This requires a mosquitto instance to already be running.
    echo [%~n0]: To start up the mosquitto server, run the `run-mosquitto` script from another console.
    echo(
    echo [%~n0]: Please enter your mosquitto admin password. This will be used to create a new priviledged `server` MQTT client:
    set /p MOSQUITTO_ADMIN_PWD=

    "%MOSQUITTO_CTRL_EXE_PATH%" -u admin -P !MOSQUITTO_ADMIN_PWD! dynsec createClient  server
    "%MOSQUITTO_CTRL_EXE_PATH%" -u admin -P !MOSQUITTO_ADMIN_PWD! dynsec createRole    server
    "%MOSQUITTO_CTRL_EXE_PATH%" -u admin -P !MOSQUITTO_ADMIN_PWD! dynsec addRoleACL    server publishClientSend    # allow 100
    "%MOSQUITTO_CTRL_EXE_PATH%" -u admin -P !MOSQUITTO_ADMIN_PWD! dynsec addRoleACL    server publishClientReceive # allow 100
    "%MOSQUITTO_CTRL_EXE_PATH%" -u admin -P !MOSQUITTO_ADMIN_PWD! dynsec addRoleACL    server subscribePattern     # allow 100
    "%MOSQUITTO_CTRL_EXE_PATH%" -u admin -P !MOSQUITTO_ADMIN_PWD! dynsec addClientRole server server

    set MOSQUITTO_ADMIN_PWD=
    exit /b 0
)

"%MOSQUITTO_CTRL_EXE_PATH%" -u admin dynsec %*