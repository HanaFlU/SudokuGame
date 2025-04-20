#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class MainWindow;
class DifficultyDialog;
class InstructionsDialog;

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget* parent = nullptr);
    ~MainMenu();

private slots:
    void startNewGame();
    void continueGame();
    void showInstructions();
    void exitApplication();
    void handleGameFinished();

private:
    void setupUI();
    bool hasSavedGame();
    QString getSaveFilePath();

    QLabel* titleLabel;
    QPushButton* btnNewGame;
    QPushButton* btnContinueGame;
    QPushButton* btnInstructions;
    QPushButton* btnExit;

    MainWindow* gameWindow = nullptr;
    DifficultyDialog* difficultyDialog = nullptr;
    InstructionsDialog* instructionsDialog = nullptr;
};

#endif // MAINMENU_H
