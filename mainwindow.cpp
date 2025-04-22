#include "mainwindow.h"
#include "mainmenu.h"

#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>
#include <QIntValidator>
#include <QDebug>
#include <vector> 
#include <random> 
#include <algorithm>

// --- Constructor for New Game / Custom ---
MainWindow::MainWindow(int modeValue, QWidget* parent) : QMainWindow(parent), sudokuLogic(), gameState(), uiHelper()
{
    currentMode = (modeValue == 0) ? Mode::Custom : Mode::NewGame;
    initialDifficulty = (modeValue >= 1 && modeValue <= 3) ? modeValue : 2;
    isCustomMode = (currentMode == Mode::Custom);

    setupUI();

    if (currentMode == Mode::NewGame) {
        generateNewGameInternal(initialDifficulty);
    }
    else {
        startCustomGameInternal();
    }
}

// --- Constructor for Continue Game ---
MainWindow::MainWindow(Mode mode, QWidget* parent) : QMainWindow(parent), sudokuLogic(), gameState(), uiHelper()
{
    Q_ASSERT(mode == Mode::Continue);
    currentMode = Mode::Continue;
    isCustomMode = false;

    setupUI();
    continueGameInternal();
}

// --- Destructor ---
MainWindow::~MainWindow() {
    qDebug() << "MainWindow destroyed";
}

// --- Setup UI Structure ---
void MainWindow::setupUI() {
    setWindowTitle("Sudoku");
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setObjectName("centralWidget");

    setStyleSheet(R"(
        QMainWindow { background-color: #f0eadd; }
        QWidget#centralWidget { background-color: #f0eadd; }
        QGroupBox { font-family: "Garamond", serif; font-size: 16px; font-weight: bold; color: #5a4d41; border: 1px solid #d3c5b4; margin-top: 1ex; background-color: #e6dbc8; padding: 15px; border-radius: 0px; }
        QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 3px; left: 10px; background-color: #e6dbc8; }
        QLineEdit { background-color: #ffffff; border: 1px solid #cccccc; font-size: 20px; font-weight: bold; color: #333333; border-radius: 0px; min-width: 30px; min-height: 30px; }
        QLineEdit[readOnly="true"] { background-color: #e6dbc8; color: #5a4d41; font-weight: bold; border: 1px solid #b0a593; }
        QLineEdit.correct { background-color: #e0ffe0; color: #006400; }
        QLineEdit.incorrect { background-color: #ffe0e0; color: #a00000; }
        QLineEdit.solution { background-color: #f0f8ff; color: #4682b4; }
        QPushButton { background-color: #d2b48c; color: #4b3832; border: 1px solid #8b7e66; padding: 8px 12px; border-radius: 3px; font-family: "Garamond", serif; font-size: 14px; min-width: 100px; }
        QPushButton:hover { background-color: #c1a37c; }
        QPushButton:pressed { background-color: #a08a6c; }
        QPushButton:disabled { background-color: #e0d8cd; color: #888888; border-color: #c0b8ae; }
        QLabel#statusLabel { font-size: 14px; color: #4b3832; font-family: "Garamond", serif; font-weight: bold; margin-top: 10px; background-color: #e6dbc8; padding: 5px; border: 1px solid #d3c5b4; }
        QFrame#blockFrame { border: 2px solid #5a4d41; border-radius: 0px; background-color: transparent; }
        QFrame#sudokuFrame { border: 3px solid #3a2d21; background-color: #f0eadd; padding: 3px; }
    )");

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(20);

    // --- Sudoku Grid Setup ---
    QFrame* sudokuFrame = new QFrame();
    sudokuFrame->setObjectName("sudokuFrame");
    QVBoxLayout* sudokuFrameLayout = new QVBoxLayout(sudokuFrame);

    QGridLayout* mainGridLayout = new QGridLayout;
    mainGridLayout->setSpacing(0);
    mainGridLayout->setContentsMargins(0, 0, 0, 0);

    for (int blockRow = 0; blockRow < 3; ++blockRow) {
        for (int blockCol = 0; blockCol < 3; ++blockCol) {
            QFrame* blockFrame = new QFrame();
            blockFrame->setObjectName("blockFrame");
            blockFrame->setFrameShape(QFrame::Box);

            QGridLayout* blockLayout = new QGridLayout(blockFrame);
            blockLayout->setSpacing(1);
            blockLayout->setContentsMargins(1, 1, 1, 1);

            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    int globalRow = blockRow * 3 + row;
                    int globalCol = blockCol * 3 + col;

                    cells[globalRow][globalCol] = new QLineEdit(this);
                    cells[globalRow][globalCol]->setAlignment(Qt::AlignCenter);
                    cells[globalRow][globalCol]->setMaxLength(1);
                    cells[globalRow][globalCol]->setFixedSize(40, 40);
                    cells[globalRow][globalCol]->setValidator(new QIntValidator(1, 9, this));

                    connect(cells[globalRow][globalCol], &QLineEdit::textChanged, this, [this, globalRow, globalCol](const QString&) {
                        handleCellInput(globalRow, globalCol);
                    });

                    blockLayout->addWidget(cells[globalRow][globalCol], row, col);
                }
            }
            mainGridLayout->addWidget(blockFrame, blockRow, blockCol);
        }
    }
    sudokuFrameLayout->addLayout(mainGridLayout);

    // --- Control Panel Setup ---
    QGroupBox* controlFrame = new QGroupBox("Controls");
    QVBoxLayout* controlLayout = new QVBoxLayout;
    controlLayout->setSpacing(10);

    btnValidateCustom = uiHelper.createStyledButton("Validate & Play");
    btnHint = uiHelper.createStyledButton("Hint");
    btnSolve = uiHelper.createStyledButton("Show Solution");
    btnReset = uiHelper.createStyledButton("Reset Board");
    btnSaveGame = uiHelper.createStyledButton("Save Game");
    btnBackMenu = uiHelper.createStyledButton("Back to Menu");

    controlLayout->addWidget(btnValidateCustom);
    controlLayout->addWidget(btnHint);
    controlLayout->addWidget(btnSolve);
    controlLayout->addWidget(btnReset);
    controlLayout->addWidget(btnSaveGame);
    controlLayout->addStretch(1);
    controlLayout->addWidget(btnBackMenu);

    statusLabel = new QLabel("Welcome to Sudoku!");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    controlLayout->addWidget(statusLabel);

    controlFrame->setLayout(controlLayout);

    mainLayout->addWidget(sudokuFrame, 3);
    mainLayout->addWidget(controlFrame, 1);

    btnValidateCustom->setVisible(isCustomMode);
    btnHint->setEnabled(!isCustomMode);
    btnSolve->setEnabled(!isCustomMode);
    btnSaveGame->setEnabled(!isCustomMode);

    // Connect button signals to slots
    connect(btnHint, &QPushButton::clicked, this, &MainWindow::giveHint);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::resetBoard);
    connect(btnSolve, &QPushButton::clicked, this, &MainWindow::showSolution);
    connect(btnValidateCustom, &QPushButton::clicked, this, &MainWindow::validateCustomBoard);
    connect(btnSaveGame, &QPushButton::clicked, this, &MainWindow::saveGame);
    connect(btnBackMenu, &QPushButton::clicked, this, &MainWindow::backToMenu);
}

// --- Internal Game Initialization ---

void MainWindow::generateNewGameInternal(int difficulty) {
    isCustomMode = false;
    qDebug() << "Generating new game with difficulty:" << difficulty;

    for (int i = 0; i < SIZE; ++i) for (int j = 0; j < SIZE; ++j) board[i][j] = solution[i][j] = 0;

    if (!sudokuLogic.generateFullBoard(solution)) {
        QMessageBox::critical(this, "Error", "Failed to generate a full Sudoku board.");
        backToMenu();
        return;
    }
    qDebug() << "Full solution generated.";

    std::copy(&solution[0][0], &solution[0][0] + SIZE * SIZE, &board[0][0]);
    sudokuLogic.removeNumbers(board, difficulty);
    qDebug() << "Numbers removed.";

    uiHelper.updateBoardUI(board, cells, gameInProgress);

    btnValidateCustom->setVisible(false);
    btnHint->setEnabled(true);
    btnSolve->setEnabled(true);
    btnSaveGame->setEnabled(true);

    QString difficultyText;
    switch (difficulty) {
    case 1: difficultyText = "Easy"; break;
    case 2: difficultyText = "Medium"; break;
    case 3: difficultyText = "Hard"; break;
    default: difficultyText = "Unknown"; break;
    }
    statusLabel->setText("New " + difficultyText + " game started. Fill the empty cells!");
}

void MainWindow::startCustomGameInternal() {
    isCustomMode = true;
    gameInProgress = false;
    qDebug() << "Starting custom game setup.";

    clearBoardForCustom();

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            cells[row][col]->setText("");
            cells[row][col]->setReadOnly(false);
            uiHelper.applyCellStyle(cells[row][col], "default");
            cells[row][col]->setProperty("class", "");
        }
    }

    btnValidateCustom->setVisible(true);
    btnHint->setEnabled(false);
    btnSolve->setEnabled(false);
    btnSaveGame->setEnabled(false);

    statusLabel->setText("Custom Mode: Enter your puzzle numbers, then click 'Validate & Play'.");
}

void MainWindow::continueGameInternal() {
    isCustomMode = false;
    gameInProgress = false;
    qDebug() << "Attempting to continue saved game.";

    QJsonObject loadedGameState;
    if (!gameState.loadGame(board, solution, loadedGameState)) {
        QMessageBox::warning(this, "Load Error", "Could not load the saved game. Starting a new Medium game.");
        generateNewGameInternal(2);
        return;
    }

    uiHelper.updateBoardUI(board, cells, gameInProgress);

    if (loadedGameState.contains("userInputs") && loadedGameState["userInputs"].isArray()) {
        QJsonArray userInputsArray = loadedGameState["userInputs"].toArray();
        for (int row = 0; row < SIZE; row++) {
            if (row < userInputsArray.size()) {
                QJsonArray rowArray = userInputsArray[row].toArray();
                for (int col = 0; col < SIZE; col++) {
                    if (col < rowArray.size()) {
                        int value = rowArray[col].toInt();
                        if (board[row][col] == 0 && value != 0) {
                            cells[row][col]->setText(QString::number(value));
                        }
                        else if (board[row][col] == 0 && value == 0) {
                            cells[row][col]->setText("");
                        }
                    }
                }
            }
        }
    }

    btnValidateCustom->setVisible(false);
    btnHint->setEnabled(true);
    btnSolve->setEnabled(true);
    btnSaveGame->setEnabled(true);

    statusLabel->setText("Game loaded successfully. Continue playing!");
}

// --- Button Click Slots ---

void MainWindow::giveHint() {
    if (isCustomMode) return;

    std::vector<std::pair<int, int>> emptyEditableCells;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col] == 0 && cells[row][col]->text().isEmpty()) {
                emptyEditableCells.emplace_back(row, col);
            }
        }
    }

    if (!emptyEditableCells.empty()) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(emptyEditableCells.begin(), emptyEditableCells.end(), g);

        auto [row, col] = emptyEditableCells.front();
        cells[row][col]->setText(QString::number(solution[row][col]));

        statusLabel->setText(QString("Hint: Cell (%1, %2) is %3").arg(row + 1).arg(col + 1).arg(solution[row][col]));
        gameInProgress = true;
    }
    else {
        bool full = true;
        for (int r = 0; r < SIZE; ++r) for (int c = 0; c < SIZE; ++c) if (cells[r][c]->text().isEmpty()) full = false;

        if (full) {
            statusLabel->setText("Board is full. Check your answers or reset.");
        }
        else {
            statusLabel->setText("No more empty cells to give hints for!");
        }
    }
}

void MainWindow::showSolution() {
    if (isCustomMode) return;


    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col] == 0) {
                cells[row][col]->setText(QString::number(solution[row][col]));
                uiHelper.applyCellStyle(cells[row][col], "solution");
                cells[row][col]->setReadOnly(true);
            }
            else {
                uiHelper.applyCellStyle(cells[row][col], "readonly");
            }
        }
    }

    statusLabel->setText("Showing the solution. Start a new game to play again.");
    gameInProgress = false;
    btnHint->setEnabled(false);
    btnSaveGame->setEnabled(false);
    btnReset->setEnabled(false); 
    btnSolve->setEnabled(false);
}

void MainWindow::resetBoard() {
    qDebug() << "Resetting board. Custom mode:" << isCustomMode;
    if (isCustomMode) {
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                cells[row][col]->setText("");
                uiHelper.applyCellStyle(cells[row][col], "default");
                cells[row][col]->setProperty("class", "");
            }
        }
        statusLabel->setText("Custom board input cleared.");
    }
    else {
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (board[row][col] == 0) {
                    cells[row][col]->setText("");
                    uiHelper.applyCellStyle(cells[row][col], "default");
                    cells[row][col]->setProperty("class", "");
                }
                else {
                    uiHelper.applyCellStyle(cells[row][col], "readonly");
                }
            }
        }
        statusLabel->setText("Board reset to initial state.");
        gameInProgress = false;

        btnHint->setEnabled(true);
        btnSolve->setEnabled(true);
        btnSaveGame->setEnabled(true);

    }
}

void MainWindow::validateCustomBoard() {
    qDebug() << "Validating custom board...";
    int customBoard[SIZE][SIZE] = { 0 };
    bool hasInput = false;

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            QString text = cells[row][col]->text();
            if (!text.isEmpty()) {
                bool ok;
                int val = text.toInt(&ok);
                if (ok && val >= 1 && val <= 9) {
                    customBoard[row][col] = val;
                    hasInput = true;
                }
                else {
                    QMessageBox::warning(this, "Invalid Input", QString("Invalid value '%1' at row %2, col %3.").arg(text).arg(row + 1).arg(col + 1));
                    return;
                }
            }
        }
    }
    if (!hasInput) {
        QMessageBox::warning(this, "Empty Board", "Please enter some numbers.");
        return;
    }

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            int num = customBoard[row][col];
            if (num != 0) {
                int originalValue = customBoard[row][col];
                customBoard[row][col] = 0;
                bool placementValid = sudokuLogic.isValid(customBoard, row, col, originalValue);
                customBoard[row][col] = originalValue;
                if (!placementValid) {
                    QMessageBox::warning(this, "Invalid Board", QString("Initial board conflict at row %1, col %2.").arg(row + 1).arg(col + 1));
                    return;
                }
            }
        }
    }
    qDebug() << "Initial board conflicts check passed.";

    if (!sudokuLogic.hasUniqueSolution(customBoard, solution)) {
        QMessageBox::warning(this, "Invalid Board", "Puzzle must have exactly one unique solution.");
        for (int i = 0; i < SIZE; ++i) for (int j = 0; j < SIZE; ++j) solution[i][j] = 0;
        qDebug() << "Unique solution check failed.";
        return;
    }
    qDebug() << "Unique solution check passed.";

    std::copy(&customBoard[0][0], &customBoard[0][0] + SIZE * SIZE, &board[0][0]);
    uiHelper.updateBoardUI(board, cells, gameInProgress);

    isCustomMode = false;
    gameInProgress = false;
    btnValidateCustom->setVisible(false);
    btnHint->setEnabled(true);
    btnSolve->setEnabled(true);
    btnSaveGame->setEnabled(true);

    statusLabel->setText("Custom game validated! You can now play.");
    qDebug() << "Custom game validated and ready to play.";
}

void MainWindow::saveGame() {
    if (isCustomMode || !gameInProgress) {
        statusLabel->setText("Cannot save in custom setup or when no progress is made.");
        qDebug() << "Save aborted. isCustomMode:" << isCustomMode << "gameInProgress:" << gameInProgress;
        return;
    }

    if (gameState.saveGame(board, solution, cells)) {
        statusLabel->setText("Game saved successfully!");
        gameInProgress = false;
    }
    else {
        QMessageBox::warning(this, "Save Error", "Could not save the game state.");
    }
}

void MainWindow::backToMenu() {
    qDebug() << "Back to menu clicked. gameInProgress:" << gameInProgress;
    bool proceedToClose = true;

    if (gameInProgress && !isCustomMode) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Return to Menu", "Do you want to save your progress?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveGame();
        }
        else if (reply == QMessageBox::Cancel) {
            proceedToClose = false;
        }
    }

    if (proceedToClose) {
        this->setProperty("closeFromBackButton", true);
        emit gameClosed();
        this->close();
    }
}

void MainWindow::checkSolution() {
    if (isCustomMode) {
        statusLabel->setText("Cannot check solution in custom mode setup.");
        return;
    }
    if (!isBoardCompleteAndCorrect()) {
        statusLabel->setText("The board is not complete or contains errors. Keep trying!");
    }
    else {
        QMessageBox::information(this, "Congratulations!", "You solved the puzzle correctly!");
        gameInProgress = false;
        btnHint->setEnabled(false);
        btnSaveGame->setEnabled(false);
        btnSolve->setEnabled(false);
        statusLabel->setText("Puzzle Solved!");
    }
}

// --- Cell Input Handling ---

void MainWindow::handleCellInput(int row, int col) {
    if (cells[row][col]->isReadOnly()) {
        return;
    }

    gameInProgress = true;
    QString text = cells[row][col]->text();
    cells[row][col]->setProperty("class", "");
    uiHelper.applyCellStyle(cells[row][col], "default");

    if (text.isEmpty()) {
        return;
    }

    bool ok;
    int userInput = text.toInt(&ok);

    if (!ok || userInput < 1 || userInput > 9) {
        uiHelper.applyCellStyle(cells[row][col], "incorrect");
        statusLabel->setText("Invalid input (1-9).");
        return;
    }

    bool conflict = false;
    // Check row
    for (int c = 0; c < SIZE; ++c) if (c != col && cells[row][c]->text() == text) conflict = true;
    // Check column
    if (!conflict) for (int r = 0; r < SIZE; ++r) if (r != row && cells[r][col]->text() == text) conflict = true;
    // Check 3x3 block
    if (!conflict) {
        int startRow = (row / 3) * 3, startCol = (col / 3) * 3;
        for (int r = startRow; r < startRow + 3 && !conflict; ++r) {
            for (int c = startCol; c < startCol + 3; ++c) {
                if ((r != row || c != col) && cells[r][c]->text() == text) {
                    conflict = true;
                    break;
                }
            }
        }
    }

    if (conflict) {
        uiHelper.applyCellStyle(cells[row][col], "incorrect");
        statusLabel->setText("Number conflicts with another cell.");
    }
    else if (!isCustomMode && solution[row][col] != 0) {
        if (userInput == solution[row][col]) {
            uiHelper.applyCellStyle(cells[row][col], "correct");
            statusLabel->setText("Correct!");
            if (isBoardCompleteAndCorrect()) {
                checkSolution();
            }
        }
        else {
            uiHelper.applyCellStyle(cells[row][col], "incorrect");
            statusLabel->setText("Incorrect number for this cell.");
        }
    }
    else if (isCustomMode) {
        statusLabel->setText("Enter puzzle numbers or Validate.");
    }
    else {
        statusLabel->setText("Number placed (no conflicts).");
    }
}

// --- Helper Methods ---

void MainWindow::clearBoardForCustom() {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            board[row][col] = 0;
            solution[row][col] = 0;
        }
    }
}

bool MainWindow::isBoardCompleteAndCorrect() {
    if (isCustomMode) return false;

    QVector<QVector<QString>> cellTexts(SIZE, QVector<QString>(SIZE));
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            cellTexts[r][c] = cells[r][c]->text();
        }
    }

    return sudokuLogic.isBoardCompleteAndCorrect(board, solution, cellTexts);
}

// --- Window Event Handling ---

void MainWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "Close event triggered. gameInProgress:" << gameInProgress;

    bool fromBackButton = this->property("closeFromBackButton").toBool();

    bool needsPrompt = gameInProgress && !isCustomMode && !fromBackButton;
    bool accepted = true;

    if (needsPrompt) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Exit Game", "Do you want to save your progress before exiting?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveGame();
            event->accept();
        }
        else if (reply == QMessageBox::Discard) {
            event->accept();
        }
        else { // Cancel
            event->ignore();
            accepted = false;
        }
    }
    else {
        event->accept();
    }

    if (accepted && event->isAccepted() && !fromBackButton) {
        qDebug() << "Close event accepted, emitting gameClosed()";
        emit gameClosed();
    }

    this->setProperty("closeFromBackButton", false);
}