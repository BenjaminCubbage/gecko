#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"
conf_dir="$(realpath "$script_dir/../conf")"

[[ -d "$env_dir" ]] || {
    echo "[env.sh]: Couldn't find directory $env_dir"
    exit 1
}

source "$script_dir/env/_envmosquitto.sh"
source "$script_dir/env/_envmysql.sh"
source "$script_dir/env/_envapi.sh"

usage() {
    echo "Usage: env.sh [options] <mosquitto|mysql|api|all>"
    echo "--clean|-c:                    Clean artifacts"
    echo "--accept-defaults|-d:          Accept all unsupplied defaults (no prompts)"
    echo "--mosquitto-port <port>:       Mosquitto port to bind to"
    echo "--mysql-port <port>:           MySQL port to bind to"
    echo "--mysql-xapi-port <port>:      MySQL XAPI port to bind to"
    echo "--gecko-api-port <port>:       Main API port to listen on"
    echo "--mysql-runtime-dir <dir>:     Directory to store MySQL data / config"
    echo "--mosquitto-runtime-dir <dir>: Directory to store Mosquitto data / config"
    echo "--gecko-api-runtime-dir <dir>: Directory to store API runtime dependencies, including secrets"
}

# parse options with GNU getopt
args=$(getopt -o c,d -l \
clean,\
accept-defaults,\
mosquitto-port:,\
mysql-port:,\
mysql-xapi-port:,\
gecko-api-port:,\
mysql-runtime-dir:,\
mosquitto-runtime-dir:,\
gecko-api-runtime-dir: -- "$@") || { usage; exit 1; }
eval set -- "$args"

clean=0
accept_defaults=0

while true; do
    case "$1" in
        -c|--clean)              clean=1;                  shift;;
        -d|--accept-defaults)    accept_defaults=1;        shift;;
        --mosquitto-port)        mosquitto_port=$2;        shift 2;;
        --mysql-port)            mysql_port=$2;            shift 2;;
        --mysql-xapi-port)       mysql_xapi_port=$2;       shift 2;;
        --gecko-api-port)        gecko_api_port=$2;        shift 2;;
        --mysql-runtime-dir)     mysql_runtime_dir=$2;     shift 2;;
        --mosquitto-runtime-dir) mosquitto_runtime_dir=$2; shift 2;;
        --gecko-api-runtime-dir) gecko_api_runtime_dir=$2; shift 2;;
        --) shift; break;;
        *) usage; exit 0;;
    esac
done

case $1 in
    all|mosquitto)
        (( $clean != 0 )) && sudo rm "$env_dir/mosquitto/.env.local" 2> /dev/null
        (( $clean == 0 )) && envmosquitto_create
        ;;&
    all|mysql)
        (( $clean != 0 )) && sudo rm "$env_dir/mysql/.env.local" 2> /dev/null
        (( $clean == 0 )) && envmysql_create
        ;;&
    all|api)
        (( $clean != 0 )) && sudo rm "$env_dir/api/.env.local" 2> /dev/null
        (( $clean == 0 )) && envapi_create
        ;;&

    all|mosquitto|mysql|api) ;;
    *) usage; exit 1;;
esac
