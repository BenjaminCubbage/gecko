#!/bin/bash

# Load key=value pairs from ../run/local.env (relative to this script)
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ENV_FILE="$SCRIPT_DIR/../run/local.env"
[ -f "$ENV_FILE" ] && export $(grep -v '^#' "$ENV_FILE" | xargs)

# If dynamic security config doesn’t exist, initialize it
if [ ! -f "$DYNAMIC_SECURITY_CONF_PATH" ]; then
    echo "[${0##*/}]: No dynamic security configuration found. Running mosquitto_ctrl dynsec init."

    mkdir -p "$(dirname "$DYNAMIC_SECURITY_CONF_PATH")"
    "$MOSQUITTO_CTRL_EXE_PATH" dynsec init "$DYNAMIC_SECURITY_CONF_PATH" admin

    echo "[${0##*/}]: Initialized dynamic security at $DYNAMIC_SECURITY_CONF_PATH"
fi

# If an old Mosquitto PID exists, kill the process
if [ -f "$MOSQUITTO_PID_PATH" ]; then
    PID="$(cat "$MOSQUITTO_PID_PATH")"
    echo "[${0##*/}]: Killing old mosquitto instance with PID $PID. This may require sudo."
    kill -9 "$PID" 2>/dev/null || true
fi

# Mosquitto dynsec bug workaround: copy *.new → original, backup original → *.old
if [ -f "${DYNAMIC_SECURITY_CONF_PATH}.new" ]; then
    echo "[${0##*/}]: Storing backup at ${DYNAMIC_SECURITY_CONF_PATH}.old"
    echo "[${0##*/}]: Copying ${DYNAMIC_SECURITY_CONF_PATH}.new → ${DYNAMIC_SECURITY_CONF_PATH}"

    cp -n "$DYNAMIC_SECURITY_CONF_PATH" "${DYNAMIC_SECURITY_CONF_PATH}.old" 2>/dev/null || true
    cp -f "${DYNAMIC_SECURITY_CONF_PATH}.new" "$DYNAMIC_SECURITY_CONF_PATH"
fi

echo "[${0##*/}]: Starting $MOSQUITTO_EXE_PATH with configuration file $MOSQUITTO_CONF_PATH"
exec "$MOSQUITTO_EXE_PATH" -c "$MOSQUITTO_CONF_PATH" "$@"