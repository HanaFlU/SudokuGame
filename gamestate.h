#pragma once
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QLineEdit>

const int BOARD_SIZE = 9;

class GameState {
public:
    GameState();

    // Save/Load functions
    bool saveGame(int board[BOARD_SIZE][BOARD_SIZE], int solution[BOARD_SIZE][BOARD_SIZE],
        QLineEdit* cells[BOARD_SIZE][BOARD_SIZE]);
    bool loadGame(int board[BOARD_SIZE][BOARD_SIZE], int solution[BOARD_SIZE][BOARD_SIZE],
        QJsonObject& gameState);

    // Helper functions
    QString getSaveFilePath();
    bool hasSavedGame();

private:
    QString saveFilePath;
};

#endif // GAMESTATE_H
