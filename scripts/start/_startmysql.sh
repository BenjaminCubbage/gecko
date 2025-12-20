#!/bin/bash

source "$script_dir/env/_envmysql.sh"

startmysql_kill() {
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

startmysql_start() {
    envmysql_loadenv
    
    if [[ -f "$MYSQL_PID_PATH" ]]; then
        echo "[start.sh]: Couldn't start MySQL because it's already running (kill with --kill)"
        exit 0
    fi

    echo "[start.sh]: Using MySQL configuration at $MYSQL_CONF_PATH"
    echo "[start.sh]: Starting MySQL as a detached process"

    # I'm swallowing output here. This isn't great if starting mysqld_safe fails to start,
    # so this should probably fixed at some point. That said, it's not entirely clear how
    # to deal with this in a coherent way.
    sudo "$MYSQLD_EXE_PATH" --defaults-file="$MYSQL_CONF_PATH" > /dev/null &
}