#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"
conf_dir="$(realpath "$script_dir/../conf")" 

source "$script_dir/_prompt.sh"
source "$script_dir/_envmysql.sh"

initmysql_kill() {
    envmysql_loadenv
    [[ -f "$MYSQL_PID_PATH" ]] && {
        pid=$(sudo cat "$MYSQL_PID_PATH")
        
        echo "[start.sh]: Killing MySQL process with PID $pid"
        sudo kill -9 $(sudo cat "$MYSQL_PID_PATH") 2> /dev/null || {
            echo "Couldn't kill MySQL (Already stopped?)"
        }
        
        sudo rm "$MYSQL_PID_PATH"
        test 1
    } || {
        echo "[start.sh]: No running instance of MySQL"
    }
}

initmysql_clean() {
    envmysql_loadenv
    [[ -f "$MYSQL_PID_PATH" ]] && {
        echo "[init.sh]: Could not complete the operation: MySQL seems to be running (kill with --kill)"
        return 1
    }

    [[ ! -f "$MYSQL_CONF_PATH" ]] && {
        echo "[init.sh]: Nothing to clean for MySQL"
        return 0
    }
    
    # Safety check: refuse to delete if MYSQL_DATA_DIR is empty, "/", or too short
    if [[ -z "$MYSQL_DATA_DIR" ]] || ((${#MYSQL_DATA_DIR} < 6)); then
        echo "[init.sh]: Refusing to force delete directory "$MYSQL_DATA_DIR" because it's suspiscious"
        return 1
    fi

    promptconfirm "Delete $MYSQL_DATA_DIR?"    && \
    promptconfirm "Delete $MYSQL_RUNTIME_DIR?" && {
        sudo rm -rf "$MYSQL_DATA_DIR"
        sudo rm -rf "$MYSQL_RUNTIME_DIR"
    }
}

initmysql_init() {
    envmysql_loadenv
    [[ -f "$MYSQL_PID_PATH" ]] && {
        echo "[init.sh]: Could not complete the operation: MySQL seems to be running (kill with --kill)"
        return 1
    }

    if [[ -d "$MYSQL_DATA_DIR" && -n "$(ls -A "$MYSQL_DATA_DIR" 2>/dev/null)" ]]; then
        echo "[init.sh]: MySQL already initialized (clean with --clean)"
        return 0
    fi

    promptpwd "MySQL user (root)" mysql_root_pwd || return 1

    mysql_schema_path="$conf_dir/schema.sql"

    echo "[init.sh]: Creating $MYSQL_RUNTIME_DIR"
    echo "[init.sh]: Creating $MYSQL_CONF_PATH"
    sudo mkdir -p "$MYSQL_RUNTIME_DIR"
    sudo mkdir -p "$MYSQL_DATA_DIR"
    sudo touch "$MYSQL_ERRORLOG_PATH"

    sudo cmake \
        -DIN_FILE_PATH="$conf_dir/mysql.cnf.in"      \
        -DOUT_FILE_PATH="$MYSQL_CONF_PATH"           \
        -DMYSQL_PORT="$MYSQL_PORT"                   \
        -DMYSQL_XAPI_PORT="$MYSQL_XAPI_PORT"         \
        -DMYSQL_PID_PATH="$MYSQL_PID_PATH"           \
        -DMYSQL_DATA_DIR="$MYSQL_DATA_DIR"           \
        -DMYSQL_ERRORLOG_PATH="$MYSQL_ERRORLOG_PATH" \
        -DMYSQL_SOCKET_PATH="$MYSQL_SOCKET_PATH"     \
        -P "$conf_dir/mysql.cnf.cmake"

    sudo chmod 600 "$MYSQL_CONF_PATH"
    sudo chown -R mysql:mysql "$MYSQL_RUNTIME_DIR"
    sudo chown -R mysql:mysql "$MYSQL_DATA_DIR"

    echo "[init.sh]: Initializing MySQL data directory at $MYSQL_DATA_DIR"
    sudo "$MYSQLD_EXE_PATH" --defaults-file="$MYSQL_CONF_PATH" --initialize-insecure || {
        echo "[init.sh]: Failed to initialize MySQL data directory."
        return 1
    }

    echo "[init.sh]: Starting MySQL server..."
    sudo "$MYSQLD_EXE_PATH" --defaults-file="$MYSQL_CONF_PATH" 2>&1 &

    echo "[init.sh]: Waiting for MySQL to become ready..."
    ready=0
    for (( i=0; i<60; ++i )); do
        sudo "$MYSQL_EXE_PATH" -u root -P "$MYSQL_PORT" -h 127.0.0.1 -e "SELECT 1;" >/dev/null 2>&1 && {
            ready=1
            break
        }
        sleep 1
    done

    if (( ready == 0 )); then
        echo "[init.sh]: MySQL did not start within 60 seconds."
        [[ -f "$MYSQL_PID_PATH" ]] && {
            pid="$(sudo cat "$MYSQL_PID_PATH")"
            sudo kill -9 "$pid" 2>/dev/null || true
            sudo rm "$MYSQL_PID_PATH" 2> /dev/null
        }
        return 1
    fi

    if [[ ! -f "$mysql_schema_path" ]]; then
        echo "[init.sh]: Could not find schema file at $mysql_schema_path"
        schema_status=1
    else
        echo "[init.sh]: Applying schema from $mysql_schema_path"
        "$MYSQL_EXE_PATH" -u root -P "$MYSQL_PORT" -h 127.0.0.1 < "$mysql_schema_path"
        schema_status=$?
    fi

    if (( schema_status == 0 )); then
        "$MYSQL_EXE_PATH" -u root -P "$MYSQL_PORT" -h 127.0.0.1 -e "CREATE DATABASE IF NOT EXISTS Gecko;"
        create_status=$?
        if (( create_status == 0 )); then
            "$MYSQL_EXE_PATH" -u root -P "$MYSQL_PORT" -h 127.0.0.1 -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '$mysql_root_pwd';"
            alter_status=$?
        else
            alter_status=1
        fi
    else
        create_status=1
        alter_status=1
    fi

    unset mysql_root_pwd

    if (( schema_status == 0 && create_status == 0 && alter_status == 0 )); then
        echo "[init.sh]: Initialized MySQL Server Successfully."
    else
        echo "[init.sh]: Failed to initialize MySQL Server."
    fi

    if [[ -f "$MYSQL_PID_PATH" ]]; then
        pid="$(sudo cat "$MYSQL_PID_PATH")"
        echo "[init.sh]: Stopping MySQL server with PID $pid"
        sudo kill "$pid" 2>/dev/null || true
        sleep 2
        sudo rm "$MYSQL_PID_PATH" 2> /dev/null
    fi
}
