#!/bin/bash

script_dir="$(cd "$(dirname "$0")" && pwd)"
env_dir="$(realpath "$script_dir/../env")"
conf_dir="$(realpath "$script_dir/../conf")"

source "$script_dir/_prompt.sh"
source "$script_dir/_envapi.sh"

initapi_clean() {
    envapi_loadenv || exit 1

    # If nothing exists, say so and exit.
    if [[ ! -d "$GECKO_API_RUNTIME_DIR" ]]; then
        echo "[init.sh]: Nothing to clean for Gecko API"
        return 0
    fi

    promptconfirm "Delete $GECKO_API_RUNTIME_DIR?" && sudo rm -r $GECKO_API_RUNTIME_DIR
}

initapi_init() {
    envapi_loadenv || exit 1

    if [[ -d "$GECKO_API_RUNTIME_DIR" ]]; then
        echo "[init.sh]: Gecko API already initialized (clean with --clean)"
        return 0
    fi

    sudo mkdir -p "$GECKO_API_RUNTIME_DIR"
    sudo mkdir -p "$(dirname "$GECKO_API_OAUTH_CLIENTID_PATH")"
    sudo mkdir -p "$(dirname "$GECKO_API_OAUTH_CLIENTSECRET_PATH")"

    echo "Never use production secrets in development"
    promptnonempty "Enter your OAuth client ID: "     oauth_clientid
    promptnonempty "Enter your OAuth client Secret: " oauth_clientsecret

    printf %s $oauth_clientid     | sudo tee "$GECKO_API_OAUTH_CLIENTID_PATH"     1> /dev/null && \
    printf %s $oauth_clientsecret | sudo tee "$GECKO_API_OAUTH_CLIENTSECRET_PATH" 1> /dev/null
    (( $? )) && return 1

    echo "Wrote client id to $GECKO_API_OAUTH_CLIENTID_PATH"
    echo "Wrote client secret to $GECKO_API_OAUTH_CLIENTSECRET_PATH"

    if [[ ! -f "$GECKO_API_TLS_PKEY_PATH" ]]; then
        sudo mkdir -p "$(dirname "$GECKO_API_TLS_PKEY_PATH")"      && \
        sudo "$OPENSSL_EXE_PATH" genpkey                              \
            -algorithm ec                                             \
            -pkeyopt ec_paramgen_curve:P-256                          \
            -out "$GECKO_API_TLS_PKEY_PATH"                        && \
        echo "Created TLS private key at $GECKO_API_TLS_PKEY_PATH" || \
        echo "Failed to create TLS private key at $GECKO_API_TLS_PKEY_PATH"
    fi

    if [[ -f "$GECKO_API_TLS_PKEY_PATH" && ! -f "$GECKO_API_TLS_CERT_PATH" ]]; then
        sudo mkdir -p "$(dirname "$GECKO_API_TLS_CERT_PATH")"      && \
        sudo "$OPENSSL_EXE_PATH" req                                  \
            -new                                                      \
            -x509                                                     \
            -key "$GECKO_API_TLS_PKEY_PATH"                           \
            -subj "/CN=localhost"                                     \
            -addext "subjectAltName=DNS:localhost"                    \
            -days 3650                                                \
            -out "$GECKO_API_TLS_CERT_PATH"                        && \
        echo "Created TLS certificate at $GECKO_API_TLS_CERT_PATH" || \
        echo "Failed to create TLS certificate $GECKO_API_TLS_CERT_PATH from private key $GECKO_API_TLS_PKEY_PATH"
    fi

    if [[ ! -f "$GECKO_API_JWT_PKEY_PATH" ]]; then
        sudo mkdir -p "$(dirname "$GECKO_API_JWT_PKEY_PATH")"      && \
        sudo "$OPENSSL_EXE_PATH" genpkey                              \
            -algorithm ec                                             \
            -pkeyopt ec_paramgen_curve:P-256                          \
            -out "$GECKO_API_JWT_PKEY_PATH"                        && \
        echo "Created JWT private key at $GECKO_API_JWT_PKEY_PATH" || \
        echo "Failed to create JWT private key at $GECKO_API_JWT_PKEY_PATH"
    fi

    if [[ -f "$GECKO_API_JWT_PKEY_PATH" && ! -f "$GECKO_API_JWT_PUBKEY_PATH" ]]; then
        sudo mkdir -p "$(dirname "$GECKO_API_JWT_PUBKEY_PATH")"    && \
        sudo "$OPENSSL_EXE_PATH" pkey                                 \
            -in "$GECKO_API_JWT_PKEY_PATH"                            \
            -pubout                                                   \
            -out "$GECKO_API_JWT_PUBKEY_PATH"                      && \
        echo "JWT public key outputted to $GECKO_API_JWT_PUBKEY_PATH" || \
        echo "Failed to create JWT public key $GECKO_API_JWT_PUBKEY_PATH from private key $GECKO_API_JWT_PKEY_PATH"
    fi
}