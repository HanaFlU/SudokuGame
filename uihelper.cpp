#include "uihelper.h"

const QString UIHelper::STYLE_DEFAULT = "background-color: #ffffff; color: #333333;";
const QString UIHelper::STYLE_READONLY = "background-color: #e6dbc8; color: #5a4d41; font-weight: bold; border: 1px solid #b0a593;";
const QString UIHelper::STYLE_CORRECT = "background-color: #e0ffe0; color: #006400;";
const QString UIHelper::STYLE_INCORRECT = "background-color: #ffe0e0; color: #a00000;";
const QString UIHelper::STYLE_SOLUTION = "background-color: #f0f8ff; color: #4682b4;";

UIHelper::UIHelper() {
    // Constructor
}

void UIHelper::updateBoardUI(int board[UI_SIZE][UI_SIZE], QLineEdit* cells[UI_SIZE][UI_SIZE], bool& gameInProgress) {
    gameInProgress = false;
    for (int row = 0; row < UI_SIZE; row++) {
        for (int col = 0; col < UI_SIZE; col++) {
            cells[row][col]->setProperty("class", "");

            if (board[row][col] == 0) {
                cells[row][col]->setText("");
                cells[row][col]->setReadOnly(false);
                cells[row][col]->setStyleSheet(STYLE_DEFAULT);
            }
            else {
                cells[row][col]->setText(QString::number(board[row][col]));
                cells[row][col]->setReadOnly(true);
                cells[row][col]->setStyleSheet(STYLE_READONLY);
            }

            if (!cells[row][col]->validator()) {
                cells[row][col]->setValidator(new QIntValidator(1, 9, cells[row][col]));
            }
        }
    }
}

QPushButton* UIHelper::createStyledButton(const QString& text) {
    QPushButton* btn = new QPushButton(text);
    return btn;
}

void UIHelper::applyCellStyle(QLineEdit* cell, const QString& styleClass) {
    if (styleClass == "default") {
        cell->setStyleSheet(STYLE_DEFAULT);
    }
    else if (styleClass == "readonly") {
        cell->setStyleSheet(STYLE_READONLY);
    }
    else if (styleClass == "correct") {
        cell->setStyleSheet(STYLE_CORRECT);
    }
    else if (styleClass == "incorrect") {
        cell->setStyleSheet(STYLE_INCORRECT);
    }
    else if (styleClass == "solution") {
        cell->setStyleSheet(STYLE_SOLUTION);
    }
}
