PRAGMA foreign_keys = off;

-- Table: GamePlayerBind
CREATE TABLE IF NOT EXISTS GamePlayerBind (
    playerId   INTEGER  NOT NULL
                        REFERENCES GamePlayer (playerId),
    bindObject VARCHAR  NOT NULL,
    bindTime   DATETIME NOT NULL,
    PRIMARY KEY (
        playerId,
        bindObject
    )
);


-- Table: GamePlayerItems
CREATE TABLE IF NOT EXISTS GamePlayerItems (
    playerId     INTEGER REFERENCES GamePlayer (playerId) 
                         NOT NULL,
    itemId       INTEGER REFERENCES BasicItems
                         NOT NULL,
    itemAmount   INTEGER NOT NULL,
    itemLocation INTEGER NOT NULL,
    PRIMARY KEY (
        playerId,
        itemId
    )
);


-- Table: GamePlayer
CREATE TABLE IF NOT EXISTS GamePlayer (
    playerId      INTEGER  PRIMARY KEY AUTOINCREMENT,
    playerName    VARCHAR  NOT NULL,
    playerRegdate DATETIME NOT NULL
);


-- Table: GameItems
CREATE TABLE IF NOT EXISTS GameItems (
    itemId         INTEGER PRIMARY KEY AUTOINCREMENT
                           NOT NULL
                           UNIQUE,
    itemBaseId     INTEGER REFERENCES BasicItems (itemBaseId) 
                           NOT NULL,
    itemName       VARCHAR,
    itemProperties VARCHAR
);


-- Table: BasicItems
CREATE TABLE IF NOT EXISTS BasicItems (
    itemBaseId     INTEGER PRIMARY KEY AUTOINCREMENT
                           UNIQUE
                           NOT NULL,
    itemName       VARCHAR NOT NULL,
    itemDescribe   VARCHAR,
    itemCateogory  VARCHAR,
    itemProperties VARCHAR
);


-- View: GamePlayerItemsView
CREATE VIEW IF NOT EXISTS GamePlayerItemsView AS
    SELECT GamePlayerItems.playerId AS playerId,
           GamePlayerItems.itemId AS itemId,
           GamePlayerItems.itemAmount AS itemAmount,
           GamePlayerItems.itemLocation AS itemLocation,
           GameItemsView.itemBaseId AS itemBaseId,
           GameItemsView.itemCateogory AS itemCateogory,
           GameItemsView.itemDescribe AS itemDescribe,
           GameItemsView.itemName AS itemName,
           GameItemsView.itemProperties AS itemProperties
      FROM GamePlayerItems,
           GameItemsView
     WHERE GamePlayerItems.itemId = GameItemsView.itemId;


-- View: GameItemsView
CREATE VIEW IF NOT EXISTS GameItemsView AS
    SELECT GameItems.itemId AS itemId,
           BasicItems.itemBaseId AS itemBaseId,
           CASE WHEN (GameItems.itemName IS NOT NULL) THEN GameItems.itemName ELSE BasicItems.itemName END AS itemName,
           BasicItems.itemCateogory AS itemCateogory,
           BasicItems.itemDescribe AS itemDescribe,
           CASE WHEN (GameItems.itemProperties IS NOT NULL) THEN GameItems.itemProperties ELSE BasicItems.itemProperties END AS itemProperties
      FROM BasicItems,
           GameItems
     WHERE BasicItems.itemBaseId = GameItems.itemBaseId;


PRAGMA foreign_keys = on;
