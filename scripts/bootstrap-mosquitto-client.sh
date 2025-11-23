#!/bin/bash

ENV_FILE="$(cd "$(dirname "$0")" && pwd)/../run/local.env"
[ -f "$ENV_FILE" ] && export $(grep -v '^#' "$ENV_FILE" | xargs)

# If dynamic security config doesn’t exist, initialize it
if [ ! -f "$DYNAMIC_SECURITY_CONF_PATH" ]; then
    echo "[${0##*/}]: No dynamic security configuration found. Running mosquitto_ctrl dynsec init."

    sudo mkdir -p "$(dirname "$DYNAMIC_SECURITY_CONF_PATH")" && \
    sudo "$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT dynsec init "$DYNAMIC_SECURITY_CONF_PATH" admin
fi

echo "[${0##*/}]: Please enter your mosquitto admin password. This will be used to create a root Mosquitto MQTT client:"
read -s -p "Enter your mosquitto admin password: " MOSQUITTO_ADMIN_PASSWORD
echo

"$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $MOSQUITTO_ADMIN_PASSWORD dynsec createClient  root                                   
"$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $MOSQUITTO_ADMIN_PASSWORD dynsec createRole    root                                   
"$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $MOSQUITTO_ADMIN_PASSWORD dynsec addRoleACL    root publishClientSend    \# allow 100
"$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $MOSQUITTO_ADMIN_PASSWORD dynsec addRoleACL    root publishClientReceive \# allow 100
"$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $MOSQUITTO_ADMIN_PASSWORD dynsec addRoleACL    root subscribePattern     \# allow 100
"$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $MOSQUITTO_ADMIN_PASSWORD dynsec addClientRole root root                             

unset MOSQUITTO_ADMIN_PASSWORD