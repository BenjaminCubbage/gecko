#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"

source "$script_dir/_prompt.sh"
source "$script_dir/_envapi.sh"

# Eventually we may make the backend produce its own PID file.
# For now kill()ing the API does nothing, and certificate creation
# won't try to ensure the API process is stopped. This is fine for the
# interim because the API loads certificates once at startup and then 
# keeps them in-mem.
initapi_kill() { :; }

startapi_start() {
    envapi_loadenv

    mosquitto_password_path=$GECKO_API_MOSQUITTO_ROOT_PASSWORD_PATH
    mysql_password_path=$GECKO_API_MYSQL_ROOT_PASSWORD_PATH

    if [[ ! -f "$mosquitto_password_path" ]] || [[ $(cat "$mosquitto_password_path") == "" ]] ||
       [[ ! -f "$mysql_password_path"     ]] || [[ $(cat "$mysql_password_path")     == "" ]] 
    then
        promptnonempty "Enter the path to a text file containing your root password for Mosquitto: " mosquitto_password_path
        promptnonempty "Enter the path to a text file containing your root password for MySQL: "     mysql_password_path
    fi

    echo "[start.sh]: Starting up API..."
    sudo "$GECKO_API_EXE_PATH" "$env_dir/api/.env.local" "$mosquitto_password_path" "$mysql_password_path"
}