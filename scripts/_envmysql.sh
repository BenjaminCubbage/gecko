#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"

envmysql_loadenv() {
    mysql_env_file=$env_dir/mysql/.env.local
    [[ -f "$mysql_env_file" ]] && export $(grep -v "^[ \t]*#" "$mysql_env_file" | xargs) || {
        echo "[env.sh]: Failed to load environment from $mysql_env_file--have you run env.sh yet?"
        return 1
    }
}

envmysql_create() {
    envmysql_loadenv

    # Prompt
    if [[ $accept_defaults -eq 0 ]]; then
        [[ "$mysql_port" ]]        || read -p "MySQL port to listen on (press Enter for default: 3003): "                               mysql_port
        [[ "$mysql_xapi_port" ]]   || read -p "MySQL XAPI port to listen on (press Enter for default: 3004): "                          mysql_xapi_port
        [[ "$mysql_runtime_dir" ]] || read -p "Root path to store mysql config / data (press Enter for default: /var/lib/gecko/mysql):" mysql_runtime_dir
        [[ "$just_find_them" ]]    || read -p "Path to the mysql executable (press Enter to search filesystem): "                       mysql_exe_path
        [[ "$just_find_them" ]]    || read -p "Path to the mysqld_safe executable (press Enter to search filesystem): "                 mysqld_exe_path
    fi

    # Defaults
    [[ "$mysql_port" ]]        || mysql_port=3003
    [[ "$mysql_xapi_port" ]]   || mysql_xapi_port=3004
    [[ "$mysql_runtime_dir" ]] || mysql_runtime_dir=/var/lib/gecko/mysql

    # CMake Args
    cmake_args=()
    [[ "$mysql_port" ]]        && cmake_args+=("-DMYSQL_PORT=$mysql_port")
    [[ "$mysql_xapi_port" ]]   && cmake_args+=("-DMYSQL_XAPI_PORT=$mysql_xapi_port")
    [[ "$mysql_runtime_dir" ]] && cmake_args+=("-DMYSQL_RUNTIME_DIR=$mysql_runtime_dir")
    [[ "$mysql_exe_path" ]]    && cmake_args+=("-DMYSQL_EXE_PATH=$mysql_exe_path")
    [[ "$mysqld_exe_path" ]]   && cmake_args+=("-DMYSQL_EXE_PATH=$mysql_exe_path")

    cmake "${cmake_args[@]}" -P "$env_dir/mysql/env.cmake" || exit 1
}