#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"
conf_dir="$(realpath "$script_dir/../conf")" 

source "$script_dir/_prompt.sh"
source "$script_dir/_envmosquitto.sh"

initmosquitto_clean() {
    envmosquitto_loadenv
    [[ -f "$MOSQUITTO_PID_PATH" ]] && {
        echo "[init.sh]: Could not complete the operation: Mosquitto seems to be running (kill with --kill)"
        return 1
    }

    [[ ! -f "$MOSQUITTO_CONF_PATH" ]] && {
        echo "[init.sh]: Nothing to clean for Mosquitto"
        return 0
    }

    promptconfirm "Delete $MOSQUITTO_RUNTIME_DIR?" && sudo rm -r "$MOSQUITTO_RUNTIME_DIR"
}

initmosquitto_init() {
    envmosquitto_loadenv
    [[ -f "$MOSQUITTO_CONF_PATH" ]] && {
        echo "[init.sh]: Mosquitto already initialized (clean with --clean)"
        return 0
    }

    promptpwd "Mosquitto user (admin)" mosquitto_admin_pwd || exit 1
    promptpwd "Mosquitto user (root)"  mosquitto_root_pwd  || exit 1

    echo "[init.sh]: Creating $MOSQUITTO_CONF_PATH"
    sudo mkdir -p $(dirname $MOSQUITTO_CONF_PATH)
    sudo cmake \
        -DIN_FILE_PATH="$conf_dir/mosquitto.conf.in"               \
        -DOUT_FILE_PATH="$MOSQUITTO_CONF_PATH"                     \
        -DMOSQUITTO_PORT="$MOSQUITTO_PORT"                         \
        -DMOSQUITTO_PID_PATH="$MOSQUITTO_PID_PATH"                 \
        -DMOSQUITTO_DYNSEC_LIB_PATH="$MOSQUITTO_DYNSEC_LIB_PATH"   \
        -DMOSQUITTO_DYNSEC_CONF_PATH="$MOSQUITTO_DYNSEC_CONF_PATH" \
        -P "$conf_dir/mosquitto.conf.cmake"

    sudo "$MOSQUITTO_CTRL_EXE_PATH" dynsec init "$MOSQUITTO_DYNSEC_CONF_PATH" admin <<EOF
$mosquitto_admin_pwd
$mosquitto_admin_pwd
EOF

    # Mosquitto owns directory of the PID path because it needs create permissions on the directory.
    # The PID path should not be a directory Mosquitto shouldn't be able to access.
    sudo chown -R mosquitto "$MOSQUITTO_RUNTIME_DIR"

    sudo "$MOSQUITTO_EXE_PATH" -c "$MOSQUITTO_CONF_PATH" -d
    "$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $mosquitto_admin_pwd dynsec createClient root 2> /dev/null <<EOF
$mosquitto_root_pwd
$mosquitto_root_pwd
EOF

    "$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $mosquitto_admin_pwd dynsec createRole    root                                   2> /dev/null
    "$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $mosquitto_admin_pwd dynsec addRoleACL    root publishClientSend    \# allow 100 2> /dev/null
    "$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $mosquitto_admin_pwd dynsec addRoleACL    root publishClientReceive \# allow 100 2> /dev/null
    "$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $mosquitto_admin_pwd dynsec addRoleACL    root subscribePattern     \# allow 100 2> /dev/null
    "$MOSQUITTO_CTRL_EXE_PATH" -p $MOSQUITTO_PORT -u admin -P $mosquitto_admin_pwd dynsec addClientRole root root
    sudo kill -9 "$(cat $MOSQUITTO_PID_PATH)"
    sudo rm $MOSQUITTO_PID_PATH
}