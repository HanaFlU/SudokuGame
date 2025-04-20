#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>
#include <QIntValidator>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QCloseEvent>

#include "sudokulogic.h"
#include "gamestate.h"
#include "uihelper.h"

class MainMenu;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    enum class Mode { NewGame, Custom, Continue };

    explicit MainWindow(int modeValue, QWidget* parent = nullptr); // modeValue: 0=Custom, 1=Easy, 2=Medium, 3=Hard
    explicit MainWindow(Mode mode, QWidget* parent = nullptr); // Constructor for Continue mode
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

signals:
    void gameClosed();

private slots:
    void checkSolution();
    void giveHint();
    void showSolution();
    void resetBoard();
    void validateCustomBoard();
    void saveGame();
    void handleCellInput(int row, int col);
    void backToMenu();

private:
    int board[SIZE][SIZE] = { 0 };
    int solution[SIZE][SIZE] = { 0 };
    QLineEdit* cells[SIZE][SIZE];
    QPushButton* btnHint, * btnSolve, * btnReset, * btnBackMenu, * btnValidateCustom;
    QPushButton* btnSaveGame;
    QLabel* statusLabel;
    QGridLayout* gridLayout;
    QWidget* centralWidget;

    bool isCustomMode = false;
    bool gameInProgress = false; // Flag to track if changes were made / game is active

    // Initialization modes
    int initialDifficulty = 2; // Default if modeValue constructor is used
    Mode currentMode;

    // Helper classes
    SudokuLogic sudokuLogic;
    GameState gameState;
    UIHelper uiHelper;

    void setupUI();
    void generateNewGameInternal(int difficulty);
    void startCustomGameInternal();
    void continueGameInternal();

    // Custom game functions
    void clearBoardForCustom();

    // Helper functions
    bool isBoardCompleteAndCorrect();
    QString getSaveFilePath();
    bool hasSavedGame();
};

#endif // MAINWINDOW_H
