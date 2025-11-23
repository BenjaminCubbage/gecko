#!/bin/bash

ENV_FILE="$(cd "$(dirname "$0")" && pwd)/../run/local.env"
[ -f "$ENV_FILE" ] && export $(grep -v '^#' "$ENV_FILE" | xargs)

echo "[${0##*/}]: Creating MySQL root user..."
read -s -p "Enter a new password for 'root'@'localhost':" MYSQL_PASSWORD                                      && \
"MYSQL_EXE_PATH" -u root -P $MYSQL_PORT -h 127.0.0.1 -e "CREATE DATABASE IF NOT EXISTS Gecko;"                           && \
"MYSQL_EXE_PATH" -u root -P $MYSQL_PORT -h 127.0.0.1 -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '$MYSQL_PASSWORD';" && \
echo "[${0##*/}]: Initialized MySQL Server Successfully."                                                     || \
echo "[${0##*/}]: Failed to initialize MySQL Server."

unset MYSQL_PASSWORD