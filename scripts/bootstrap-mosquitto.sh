#!/bin/bash

ENV_FILE="$(cd "$(dirname "$0")" && pwd)/../run/local.env"
[ -f "$ENV_FILE" ] && export $(grep -v '^#' "$ENV_FILE" | xargs)

# If an old Mosquitto PID exists, kill the process
if [ -f "$MOSQUITTO_PID_PATH" ]; then
    PID="$(cat "$MOSQUITTO_PID_PATH")"
    echo "[${0##*/}]: Killing old mosquitto instance with PID $PID."
    sudo kill -9 "$PID" 2>/dev/null && \
    echo "[${0##*/}]: Successfully killed old mosquitto instance."
fi

echo "[${0##*/}]: Starting $MOSQUITTO_EXE_PATH with configuration file $MOSQUITTO_CONF_PATH"
exec sudo chown -R mosquitto "$GECKO_USER_DIR/.mosquitto"
exec sudo "$MOSQUITTO_EXE_PATH" -c "$MOSQUITTO_CONF_PATH" -d "$@"