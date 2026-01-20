#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"
conf_dir="$(realpath "$script_dir/../conf")"

[[ -d "$env_dir" ]]  || { echo "[init.sh]: Could not find directory $env_dir";  exit 1; }

source "$script_dir/start/_startmosquitto.sh"
source "$script_dir/start/_startmysql.sh"
source "$script_dir/start/_startapi.sh"

usage() {
    echo "Usage: start.sh <mosquitto|mysql|api>"
    echo "--kill|-K: Force kill the process if it's currently running"
}

args=$(getopt -o K -l kill -- "$@") || { usage; exit 1; }
eval set -- "$args"

kill=0
start=1

while true; do
    case "$1" in
        -K|--kill)  kill=1; start=0;  shift;;
        --) shift; break;;
        *) usage; exit 0;;
    esac
done

case "$1" in
    mosquitto)
        (( $kill  != 0 )) && startmosquitto_kill
        (( $start != 0 )) && startmosquitto_start
        ;;

    mysql)
        (( $kill  != 0 )) && startmysql_kill
        (( $start != 0 )) && startmysql_start
        ;;

    api)
        (( $kill != 0 ))  && startapi_kill
        (( $start != 0 )) && startapi_start
        ;;

    *) usage; exit 1;;
esac
