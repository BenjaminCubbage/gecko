#!/bin/bash

envapi_loadenv() {
    api_env_file=$env_dir/api/.env.local
    [[ -f "$api_env_file" ]] && export $(grep -v "^[ \t]*#" "$api_env_file" | xargs) || {
        echo "[env.sh]: Failed to load environment from $api_env_file--have you run env.sh yet?"
        return 1
    }
}

envapi_create() {
    envapi_loadenv
    [[ ! $clean -eq 0 ]] && { rm "$env_dir/api/.env.local" 2> /dev/null; exit 0; }

    # Prompt
    if [[ $accept_defaults -eq 0 ]]; then
        [[ "$gecko_api_backend_origin" ]]  || read -p "Origin of backend server (press Enter for default: https://localhost:3001):"                                     gecko_api_backend_origin
        [[ "$gecko_api_frontend_origin" ]] || read -p "Origin of frontend server (press Enter for default: https://localhost:3000):"                                    gecko_api_frontend_origin
        [[ "$gecko_api_port" ]]            || read -p "Port to listen on (press Enter for default: 3001):"                                                              gecko_api_port
        [[ "$mosquitto_port" ]]            || read -p "Mosquitto port to listen on (press Enter for default: 3002): "                                                   mosquitto_port
        [[ "$mysql_xapi_port" ]]           || read -p "MySQL XAPI port to connect to (press Enter for default: 3004): "                                                 mysql_xapi_port
        [[ "$mosquitto_cert_path" ]]       || read -p "Path to the mosquitto SSL certificate (press Enter to for default: /var/lib/gecko/mosquitto/secrets/cert.pem): " mosquitto_cert_path
        [[ "$gecko_api_runtime_dir" ]]     || read -p "Directory to store persistent data, like certificates (press Enter for default: /var/lib/gecko/api): "           gecko_api_runtime_dir
        [[ "$gecko_api_exe_path" ]]        || read -p "Path to the api executable (press Enter to search filesystem): "                                                 gecko_api_exe_path
    fi

    # Defaults
    [[ "$gecko_api_backend_origin" ]]  || gecko_api_backend_origin="https://localhost:3001"
    [[ "$gecko_api_frontend_origin" ]] || gecko_api_frontend_origin="https://localhost:3000"
    [[ "$gecko_api_port" ]]            || gecko_api_port=3001
    [[ "$mosquitto_port" ]]            || mosquitto_port=3002
    [[ "$mysql_xapi_port" ]]           || mysql_xapi_port=3004
    [[ "$gecko_api_runtime_dir" ]]     || gecko_api_runtime_dir="/var/lib/gecko/api"
    [[ "$mosquitto_cert_path" ]]       || mosquitto_cert_path="/var/lib/gecko/mosquitto/secrets/cert.pem"

    # CMake Args
    cmake_args=()
    [[ "$gecko_api_backend_origin" ]]  && cmake_args+=("-DGECKO_API_BACKEND_ORIGIN=$gecko_api_backend_origin")
    [[ "$gecko_api_frontend_origin" ]] && cmake_args+=("-DGECKO_API_FRONTEND_ORIGIN=$gecko_api_frontend_origin")
    [[ "$gecko_api_port" ]]            && cmake_args+=("-DGECKO_API_PORT=$gecko_api_port")
    [[ "$mosquitto_port" ]]            && cmake_args+=("-DMOSQUITTO_PORT=$mosquitto_port")
    [[ "$mysql_xapi_port" ]]           && cmake_args+=("-DMYSQL_XAPI_PORT=$mysql_xapi_port")
    [[ "$gecko_api_runtime_dir" ]]     && cmake_args+=("-DGECKO_API_RUNTIME_DIR=$gecko_api_runtime_dir")
    [[ "$mosquitto_cert_path" ]]       && cmake_args+=("-DMOSQUITTO_CERT_PATH=$mosquitto_cert_path")
    [[ "$gecko_api_exe_path" ]]        && cmake_args+=("-DGECKO_API_EXE_PATH=$gecko_api_exe_path")

    cmake "${cmake_args[@]}" -P "$env_dir/api/env.cmake" || exit 1
}