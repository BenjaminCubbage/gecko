CREATE DATABASE IF NOT EXISTS Gecko;
USE Gecko;

CREATE TABLE IF NOT EXISTS Users
(
    user_id  INT         PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(36) NOT NULL,
    oidc_iss VARCHAR(36) NOT NULL,
    oidc_sub VARCHAR(36) NOT NULL,
    UNIQUE(username),
    UNIQUE(oidc_iss, oidc_sub)
);

CREATE TABLE IF NOT EXISTS SharedImageBlobs
(
    image_blob_id   INT        PRIMARY KEY AUTO_INCREMENT,
    idempotency_key CHAR(36)   UNIQUE NOT NULL,
    bytes           MEDIUMBLOB NOT NULL
);

CREATE TABLE IF NOT EXISTS SharedImages
(
    image_id      INT PRIMARY KEY AUTO_INCREMENT,
    image_blob_id INT NOT NULL,
    sender_id     INT NOT NULL,
    receiver_id   INT NOT NULL,
    FOREIGN KEY (image_blob_id)
        REFERENCES SharedImageBlobs(image_blob_id)
        ON DELETE CASCADE,
    FOREIGN KEY (sender_id)
        REFERENCES Users(user_id)
        ON DELETE CASCADE,
    FOREIGN KEY (receiver_id)
        REFERENCES Users(user_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Devices
(
    device_id INT PRIMARY KEY AUTO_INCREMENT,
    owner_id INT NULL,
    FOREIGN KEY (owner_id)
        REFERENCES Users(user_id)
);