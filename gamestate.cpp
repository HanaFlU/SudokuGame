#include "gamestate.h"

GameState::GameState() {
    saveFilePath = getSaveFilePath();
}

QString GameState::getSaveFilePath() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return appDataPath + "/sudoku_save.json";
}

bool GameState::hasSavedGame() {
    QFile file(saveFilePath);
    return file.exists() && file.size() > 0;
}

bool GameState::saveGame(int board[BOARD_SIZE][BOARD_SIZE], int solution[BOARD_SIZE][BOARD_SIZE], QLineEdit* cells[BOARD_SIZE][BOARD_SIZE]) {
    qDebug() << "Saving game...";
    QJsonObject gameState;

	// Save board
    QJsonArray boardArray;
    for (int row = 0; row < BOARD_SIZE; row++) {
        QJsonArray rowArray;
        for (int col = 0; col < BOARD_SIZE; col++) {
            rowArray.append(board[row][col]);
        }
        boardArray.append(rowArray);
    }
    gameState["board"] = boardArray;

	// Save solution
    QJsonArray solutionArray;
    for (int row = 0; row < BOARD_SIZE; row++) {
        QJsonArray rowArray;
        for (int col = 0; col < BOARD_SIZE; col++) {
            rowArray.append(solution[row][col]);
        }
        solutionArray.append(rowArray);
    }
    gameState["solution"] = solutionArray;

    // Save inputs
    QJsonArray userInputsArray;
    for (int row = 0; row < BOARD_SIZE; row++) {
        QJsonArray rowArray;
        for (int col = 0; col < BOARD_SIZE; col++) {
            int value = 0;
            if (!cells[row][col]->isReadOnly()) {
                QString text = cells[row][col]->text();
                if (!text.isEmpty()) {
                    bool ok;
                    value = text.toInt(&ok);
                    if (!ok) value = 0;
                }
            }
            rowArray.append(value);
        }
        userInputsArray.append(rowArray);
    }
    gameState["userInputs"] = userInputsArray;

    // Save timestamp
    gameState["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    // Save to file
    QJsonDocument doc(gameState);
    QFile file(saveFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "Game saved to" << saveFilePath;
        return true;
    }
    else {
        qDebug() << "Error saving game to" << saveFilePath << ":" << file.errorString();
        return false;
    }
}

bool GameState::loadGame(int board[BOARD_SIZE][BOARD_SIZE], int solution[BOARD_SIZE][BOARD_SIZE], QJsonObject& gameState) {
    if (!hasSavedGame()) {
        qDebug() << "No saved game file found at" << saveFilePath;
        return false;
    }

    QFile file(saveFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open saved game file:" << file.errorString();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Invalid JSON format in save file.";
        return false;
    }

    gameState = doc.object();

    // Load board state (original puzzle)
    if (gameState.contains("board") && gameState["board"].isArray()) {
        QJsonArray boardArray = gameState["board"].toArray();
        if (boardArray.size() == BOARD_SIZE) {
            for (int row = 0; row < BOARD_SIZE; row++) {
                if (boardArray[row].isArray() && boardArray[row].toArray().size() == BOARD_SIZE) {
                    QJsonArray rowArray = boardArray[row].toArray();
                    for (int col = 0; col < BOARD_SIZE; col++) {
                        board[row][col] = rowArray[col].toInt(0); // Default to 0 if invalid
                    }
                }
                else { qDebug() << "Invalid row array size in saved board"; return false; }
            }
        }
        else { qDebug() << "Invalid board array size in save file"; return false; }
    }
    else { qDebug() << "Missing or invalid 'board' array in save file"; return false; }

    // Load solution
    if (gameState.contains("solution") && gameState["solution"].isArray()) {
        QJsonArray solutionArray = gameState["solution"].toArray();
        if (solutionArray.size() == BOARD_SIZE) {
            for (int row = 0; row < BOARD_SIZE; row++) {
                if (solutionArray[row].isArray() && solutionArray[row].toArray().size() == BOARD_SIZE) {
                    QJsonArray rowArray = solutionArray[row].toArray();
                    for (int col = 0; col < BOARD_SIZE; col++) {
                        solution[row][col] = rowArray[col].toInt(0);
                    }
                }
                else { qDebug() << "Invalid row array size in saved solution"; return false; }
            }
        }
        else { qDebug() << "Invalid solution array size in save file"; return false; }
    }
    else { qDebug() << "Missing or invalid 'solution' array in save file"; return false; }

    qDebug() << "Game data loaded successfully from" << saveFilePath;
    return true;
}
