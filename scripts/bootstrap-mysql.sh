#!/bin/bash

ENV_FILE="$(cd "$(dirname "$0")" && pwd)/../run/local.env"
[ -f "$ENV_FILE" ] && export $(grep -v '^#' "$ENV_FILE" | xargs)

if ! [ -d "$MYSQL_DATADIR" ]; then
    sudo mysqld_safe --defaults-file="$MYSQL_CONF_PATH" --initialize-insecure --user=mysql
fi

sudo chown -R mysql:mysql "$GECKO_USER_DIR/.mysql" && \
touch $GECKO_ROOT/run/.mysql/error.log             && \
exec sudo "$MYSQLD_EXE_PATH" --defaults-file=/mnt/b/gecko/run/.mysql/mysqld.cnf &