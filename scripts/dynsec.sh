#!/bin/bash

# Load key=value pairs from ../run/local.env (relative to this script)
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ENV_FILE="$SCRIPT_DIR/../run/local.env"
[ -f "$ENV_FILE" ] && export $(grep -v '^#' "$ENV_FILE" | xargs)

if [ "$1" = "create-server-client" ]; then
    echo "[${0##*/}]: Creating server client. This requires a mosquitto instance to already be running."
    echo "[${0##*/}]: To start up the mosquitto server, run the \`run-mosquitto\` script from another console, or as a daemon with -v."
    echo
    printf "[${0##*/}]: Please enter your mosquitto admin password. This will be used to create a new privileged \`server\` MQTT client:\n"
    
    read -s -p "Mosquitto Admin Password: " MOSQUITTO_ADMIN_PASSWORD
    echo

    "$MOSQUITTO_CTRL_EXE_PATH" -u admin -P "$MOSQUITTO_ADMIN_PASSWORD" dynsec createClient  server
    "$MOSQUITTO_CTRL_EXE_PATH" -u admin -P "$MOSQUITTO_ADMIN_PASSWORD" dynsec createRole    server
    "$MOSQUITTO_CTRL_EXE_PATH" -u admin -P "$MOSQUITTO_ADMIN_PASSWORD" dynsec addRoleACL    server publishClientSend    \# allow 100
    "$MOSQUITTO_CTRL_EXE_PATH" -u admin -P "$MOSQUITTO_ADMIN_PASSWORD" dynsec addRoleACL    server publishClientReceive \# allow 100
    "$MOSQUITTO_CTRL_EXE_PATH" -u admin -P "$MOSQUITTO_ADMIN_PASSWORD" dynsec addRoleACL    server subscribePattern     \# allow 100
    "$MOSQUITTO_CTRL_EXE_PATH" -u admin -P "$MOSQUITTO_ADMIN_PASSWORD" dynsec addClientRole server server

    unset MOSQUITTO_ADMIN_PASSWORD
    exit 0
fi

"$MOSQUITTO_CTRL_EXE_PATH" -u admin dynsec "$@"