#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"

source "$script_dir/_envmosquitto.sh"

startmosquitto_kill() {
    envmosquitto_loadenv

    [[ -f "$MOSQUITTO_PID_PATH" ]] && {
        pid=$(sudo cat "$MOSQUITTO_PID_PATH")
        
        echo "[start.sh]: Killing Mosquitto process with PID $pid"
        sudo kill -9 $(sudo cat "$MOSQUITTO_PID_PATH") 2> /dev/null || {
            echo "Couldn't kill Mosquitto (Already stopped?)"
        }
        
        sudo rm "$MOSQUITTO_PID_PATH"
        test 1
    } || {
        echo "[start.sh]: No running instance of Mosquitto"
    }
}

startmosquitto_start() {
    envmosquitto_loadenv
    
    if [[ -f "$MOSQUITTO_PID_PATH" ]]; then
        echo "[start.sh]: Couldn't start Mosquitto because it's already running (kill with --kill)"
        exit 0
    fi

    echo "[start.sh]: Using Mosquitto configuration at $MOSQUITTO_CONF_PATH"
    echo "[start.sh]: Starting Mosquitto as daemon"
    sudo "$MOSQUITTO_EXE_PATH" -c "$MOSQUITTO_CONF_PATH"
}