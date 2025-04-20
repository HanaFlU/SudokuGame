#pragma once
#ifndef UIHELPER_H
#define UIHELPER_H

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QIntValidator>

const int UI_SIZE = 9;

class UIHelper {
public:
    UIHelper();

    // UI helper functions
    void updateBoardUI(int board[UI_SIZE][UI_SIZE], QLineEdit* cells[UI_SIZE][UI_SIZE], bool& gameInProgress);
    QPushButton* createStyledButton(const QString& text);
    void applyCellStyle(QLineEdit* cell, const QString& styleClass);

    // Style constants
    static const QString STYLE_DEFAULT;
    static const QString STYLE_READONLY;
    static const QString STYLE_CORRECT;
    static const QString STYLE_INCORRECT;
    static const QString STYLE_SOLUTION;
};

#endif // UIHELPER_H
