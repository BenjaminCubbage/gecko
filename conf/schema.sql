CREATE DATABASE IF NOT EXISTS Gecko;
USE Gecko;

CREATE TABLE IF NOT EXISTS Users
(
    user_id  INT         PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(36) NOT NULL,
    oidc_iss VARCHAR(36) NOT NULL,
    oidc_sub VARCHAR(36) NOT NULL,
    UNIQUE(username),
    UNIQUE(oidc_iss, oidc_sub),
    INDEX oidc_identity(oidc_iss, oidc_sub)
);