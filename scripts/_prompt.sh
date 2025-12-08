#!/bin/bash

promptpwd() {
    local what="$1"
    local varname="$2"
    local pwd
    local pwd_reenter

    while true; do
        read -s -p "Enter a password for $what: " pwd || { echo; return 1; }
        echo
        if [[ -z "$pwd" ]]; then
            echo "Password cannot be empty"
            echo
            continue
        fi

        read -s -p "Reenter your password: " pwd_reenter || { echo; return 1; }
        echo
        if [[ "$pwd" != "$pwd_reenter" ]]; then
            echo "Passwords do not match"
            echo
            continue
        fi

        printf -v "$varname" '%s' "$pwd"
        return 0
    done
}

promptnonempty() {
    local msg="$1"
    local varname="$2"
    local res

    while true; do
        read -p "$msg" res || { echo; return 1; }
        if [[ -z "$res" ]]; then
            echo "Response cannot be empty."
            echo
            continue
        fi

        printf -v "$varname" '%s' "$res"
        return 0
    done
}

promptconfirm() {
    local msg="$1"
    local res

    read -p "$msg (Y,n): " res || { echo; return 1; }

    [[ -z $res || confirm == "" || confirm == "Y" || confirm == "y" ]]
    return $?
}