#!/bin/bash

envmosquitto_loadenv() {
    mosquitto_env_file=$env_dir/mosquitto/.env.local
    [[ -f "$mosquitto_env_file" ]] && export $(grep -v "^[ \t]*#" "$mosquitto_env_file" | xargs) || {
        echo "[env.sh]: Failed to load environment from $mosquitto_env_file--have you run env.sh yet?"
        return 1
    }
}

envmosquitto_create() {
    envmosquitto_loadenv
    [[ ! $clean -eq 0 ]] && { rm "$env_dir/mosquitto/.env.local" 2> /dev/null; exit 0; }

    # Prompt
    if [[ $accept_defaults -eq 0 ]]; then
        [[ "$mosquitto_port" ]]        || read -p "Mosquitto port to listen on (press Enter for default: 3002): "                                             mosquitto_port
        [[ "$mosquitto_runtime_dir" ]] || read -p "Path to store mosquitto runtime / configuration data (press Enter for default: /var/lib/gecko/mosquitto):" mosquitto_runtime_dir

        read -p "Path to the mosquitto executable (press Enter to search filesystem): "                                     mosquitto_exe_path
        read -p "Path to the mosquitto_ctrl executable (press Enter to search filesystem): "                                mosquitto_ctrl_exe_path
        read -p "Path to the mosquitto_dynamic_security shared library (press Enter to search filesystem): "                mosquitto_dynsec_lib_path
    fi

    # Defaults
    [[ "$mosquitto_port" ]]        || mosquitto_port=3002
    [[ "$mosquitto_runtime_dir" ]] || mosquitto_runtime_dir=/var/lib/gecko/mosquitto

    # CMake Args
    cmake_args=()
    [[ "$mosquitto_port" ]]            && cmake_args+=("-DMOSQUITTO_PORT=$mosquitto_port")
    [[ "$mosquitto_runtime_dir" ]]     && cmake_args+=("-DMOSQUITTO_RUNTIME_DIR=$mosquitto_runtime_dir")
    [[ "$mosquitto_exe_path" ]]        && cmake_args+=("-DMOSQUITTO_EXE_PATH=$mosquitto_exe_path")
    [[ "$mosquitto_ctrl_exe_path" ]]   && cmake_args+=("-DMOSQUITTO_CTRL_EXE_PATH=$mosquitto_ctrl_exe_path")
    [[ "$mosquitto_dynsec_lib_path" ]] && cmake_args+=("-DMOSQUITTO_DYNSEC_LIB_PATH=$mosquitto_dynsec_lib_path")

    cmake "${cmake_args[@]}" -P "$env_dir/mosquitto/env.cmake" || exit 1
}