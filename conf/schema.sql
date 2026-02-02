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

CREATE TABLE IF NOT EXISTS Devices
(
    device_id INT PRIMARY KEY AUTO_INCREMENT,
    owner_id  INT NULL,
    `name`    INT NOT NULL,
    FOREIGN KEY (owner_id)
        REFERENCES Users(user_id)
);

CREATE TABLE IF NOT EXISTS SharedImages
(
    image_id            INT PRIMARY KEY AUTO_INCREMENT,
    image_blob_id       INT NOT NULL,
    sender_user_id      INT NOT NULL,
    recipient_device_id INT NOT NULL,
    FOREIGN KEY (image_blob_id)
        REFERENCES SharedImageBlobs(image_blob_id)
        ON DELETE CASCADE,
    FOREIGN KEY (sender_user_id)
        REFERENCES Users(user_id)
        ON DELETE CASCADE,
    FOREIGN KEY (recipient_device_id)
        REFERENCES Devices(device_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Friendships
(
    user_1 INT NOT NULL,
    user_2 INT NOT NULL,
    initiated_by INT NOT NULL,
    friendship_status ENUM('pending', 'accepted') NOT NULL,
    accepted_on DATE NULL,
    PRIMARY KEY (user_1, user_2),
    FOREIGN KEY (user_1) REFERENCES Users(user_id),
    FOREIGN KEY (user_2) REFERENCES Users(user_id),
    FOREIGN KEY (initiated_by) REFERENCES Users(user_id),
    CONSTRAINT canonical_ordering CHECK (user_1 < user_2),
    CONSTRAINT initiated_by_participant CHECK (initiated_by IN (user_1, user_2))
);

DELIMITER //
DROP TRIGGER IF EXISTS upd_friended_date_insert//
CREATE TRIGGER upd_friended_date_insert BEFORE INSERT ON Friendships
FOR EACH ROW
BEGIN
    IF NEW.friendship_status = 'pending' THEN
        SET NEW.accepted_on = NULL;
    ELSEIF NEW.friendship_status = 'accepted' THEN
        SET NEW.accepted_on = CURDATE();
    END IF;
END;//
DELIMITER ;

DELIMITER //
DROP TRIGGER IF EXISTS upd_friended_date_update//
CREATE TRIGGER upd_friended_date_update BEFORE UPDATE ON Friendships
FOR EACH ROW
BEGIN
    IF NEW.friendship_status = 'accepted' AND OLD.friendship_status <> 'accepted' THEN
        SET NEW.accepted_on = COALESCE(NEW.accepted_on, CURDATE());
    END IF;

    IF NEW.friendship_status <> 'accepted' THEN
        SET NEW.accepted_on = NULL;
    END IF;
END;//
DELIMITER ;