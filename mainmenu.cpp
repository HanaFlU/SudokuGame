#include "mainmenu.h"
#include "mainwindow.h"
#include "difficultydialog.h"
#include "instructionsdialog.h"

MainMenu::MainMenu(QWidget* parent) : QWidget(parent)
{
    setupUI();
    // Check if saved game exists and enable/disable continue button
    btnContinueGame->setEnabled(hasSavedGame());

    gameWindow = nullptr;       
}

MainMenu::~MainMenu()
{
    // Clean up dialogs if they were created
    if (difficultyDialog) delete difficultyDialog;
    if (instructionsDialog) delete instructionsDialog;
    // gameWindow is handled by Qt's parent-child mechanism or closed separately
}

void MainMenu::setupUI() {
    setWindowTitle("Sudoku Classic");
    setMinimumSize(350, 400); // Adjusted size

    // Apply a classic stylesheet
    setStyleSheet(R"(
        MainMenu {
            background-color: #f5f5dc; /* Beige background */
        }
        QLabel#titleLabel {
            font-family: "Times New Roman", Times, serif;
            font-size: 36px;
            font-weight: bold;
            color: #5a4d41; /* Dark brown */
            margin-bottom: 30px;
        }
        QPushButton {
            background-color: #deb887; /* BurlyWood */
            color: #4b3832; /* Darker brown text */
            border: 2px solid #8b7e66; /* Darker border */
            padding: 12px 20px;
            border-radius: 5px;
            font-family: "Garamond", serif;
            font-size: 16px;
            font-weight: bold;
            min-width: 180px; /* Ensure buttons have a good width */
            margin-top: 10px;
        }
        QPushButton:hover {
            background-color: #cdab77; /* Slightly darker hover */
        }
        QPushButton:pressed {
            background-color: #a08a6c; /* Darker pressed */
        }
        QPushButton:disabled {
            background-color: #d3c5b4; /* Lighter, disabled look */
            color: #888888;
            border-color: #b0a593;
        }
    )");


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setAlignment(Qt::AlignCenter);

    titleLabel = new QLabel("Sudoku", this);
	titleLabel->setObjectName("titleLabel"); // Set object name for styling
    titleLabel->setAlignment(Qt::AlignCenter);

    btnNewGame = new QPushButton("New Game", this);
    btnContinueGame = new QPushButton("Continue Game", this);
    btnInstructions = new QPushButton("How to Play", this);
    btnExit = new QPushButton("Exit", this);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(btnNewGame);
    mainLayout->addWidget(btnContinueGame);
    mainLayout->addWidget(btnInstructions);
    mainLayout->addStretch(1);
    mainLayout->addWidget(btnExit);
    mainLayout->addStretch(1);

    setLayout(mainLayout);

    // Connections
    connect(btnNewGame, &QPushButton::clicked, this, &MainMenu::startNewGame);
    connect(btnContinueGame, &QPushButton::clicked, this, &MainMenu::continueGame);
    connect(btnInstructions, &QPushButton::clicked, this, &MainMenu::showInstructions);
    connect(btnExit, &QPushButton::clicked, this, &MainMenu::exitApplication);
}

void MainMenu::startNewGame() {
    if (!difficultyDialog) {
        difficultyDialog = new DifficultyDialog(this);
    }

    if (difficultyDialog->exec() == QDialog::Accepted) {
        int mode = difficultyDialog->getSelectedMode(); // 0: Custom, 1: Easy, 2: Medium, 3: Hard

        if (gameWindow) {
            gameWindow->close();
            delete gameWindow;   // Delete properly
        }
        gameWindow = new MainWindow(mode);

        connect(gameWindow, &MainWindow::gameClosed, this, &MainMenu::handleGameFinished);

        gameWindow->show();
        this->hide();
    }
}

void MainMenu::continueGame() {
    if (gameWindow) {
        gameWindow->close();
        delete gameWindow;
    }
    gameWindow = new MainWindow(MainWindow::Mode::Continue);

    connect(gameWindow, &MainWindow::gameClosed, this, &MainMenu::handleGameFinished);
    
    gameWindow->show();
    this->hide();
}

void MainMenu::showInstructions() {
    if (!instructionsDialog) {
        instructionsDialog = new InstructionsDialog(this);
    }
    instructionsDialog->exec();
}

void MainMenu::exitApplication() {
    QApplication::quit();
}

void MainMenu::handleGameFinished() {
    if (gameWindow) {
        delete gameWindow;
        gameWindow = nullptr;
    }

    btnContinueGame->setEnabled(hasSavedGame());
    this->show(); 
}

QString MainMenu::getSaveFilePath() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return appDataPath + "/sudoku_save.json";
}

bool MainMenu::hasSavedGame() {
    QFile file(getSaveFilePath());
    return file.exists() && file.size() > 0;
}