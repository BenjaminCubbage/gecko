#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"
conf_dir="$(realpath "$script_dir/../conf")" 

source "$script_dir/_initmosquitto.sh"
source "$script_dir/_initmysql.sh"
source "$script_dir/_initapi.sh"

source "$script_dir/_startmosquitto.sh"
source "$script_dir/_startmysql.sh"
source "$script_dir/_startapi.sh"

[[ -d "$env_dir" ]]  || { echo "[init.sh]: Could not find directory $env_dir";  exit 1; }
[[ -d "$conf_dir" ]] || { echo "[init.sh]: Could not find directory $conf_dir"; exit 1; }

usage() {
    echo "Usage: init.sh [options] <mosquitto|mysql|api|all>"
    echo "--clean|-c: Clean artifacts"
    echo "--kill|-K:  Force kill the application if it's running"
}

args=$(getopt -o cK -l clean,kill -- "$@") || { usage; exit 1; }
eval set -- "$args"

clean=0
kill=0
init=1

while true; do
    case "$1" in
        -c|--clean) init=0; clean=1; shift;;
        -K|--kill)  init=0; kill=1;  shift;;
        --) shift; break;;
        *) usage; exit 0;;
    esac
done

case "$1" in
    all|mosquitto)
        (( $kill  != 0 )) && startmosquitto_kill
        (( $clean != 0 )) && initmosquitto_clean
        (( $init  != 0 )) && initmosquitto_init
        ;;&

    all|mysql)
        (( $kill  != 0 )) && startmysql_kill
        (( $clean != 0 )) && initmysql_clean
        (( $init  != 0 )) && initmysql_init
        ;;&

    all|api)
        (( $kill  != 0 )) && startapi_kill
        (( $clean != 0 )) && initapi_clean
        (( $init  != 0 )) && initapi_init
        ;;&

    all|mosquitto|mysql|api) ;;
    *) usage; exit 1;;
esac