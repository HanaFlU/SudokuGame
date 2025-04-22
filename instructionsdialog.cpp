#include "instructionsdialog.h"

InstructionsDialog::InstructionsDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("How to Play Sudoku");
    setMinimumSize(400, 300);
    setModal(true);

    setStyleSheet(R"(
        QDialog {
            background-color: #f5f5dc;
        }
        QTextBrowser {
            background-color: #fff8e7;
            border: 1px solid #d3c5b4;
            font-family: "Garamond", serif;
            font-size: 14px;
            color: #4b3832;
        }
        QPushButton {
            background-color: #deb887;
            color: #4b3832;
            border: 2px solid #8b7e66;
            padding: 8px 15px;
            border-radius: 5px;
            font-family: "Garamond", serif;
            font-size: 14px;
            font-weight: bold;
            min-width: 80px;
            margin-top: 10px;
        }
        QPushButton:hover { background-color: #cdab77; }
        QPushButton:pressed { background-color: #a08a6c; }
    )");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    instructionsText = new QTextBrowser(this);
    instructionsText->setReadOnly(true);
    instructionsText->setHtml(
        "<h2>Sudoku Rules</h2>"
        "<p>The objective is to fill a 9x9 grid with digits so that each column, each row, and each of the nine 3x3 subgrids that compose the grid contain all of the digits from 1 to 9.</p>"
        "<h3>Basics:</h3>"
        "<ul>"
        "<li>The puzzle starts with some cells already filled with numbers (the 'givens').</li>"
        "<li>You need to fill the empty cells with numbers from 1 to 9.</li>"
        "</ul>"
        "<h3>Constraints:</h3>"
        "<ol>"
        "<li><b>Rows:</b> Each row must contain the numbers 1 through 9 exactly once.</li>"
        "<li><b>Columns:</b> Each column must contain the numbers 1 through 9 exactly once.</li>"
        "<li><b>3x3 Blocks:</b> Each of the nine 3x3 blocks (separated by thicker lines) must contain the numbers 1 through 9 exactly once.</li>"
        "</ol>"
        "<p>Good luck and have fun!</p>"
    );


    btnClose = new QPushButton("Close", this);

    mainLayout->addWidget(instructionsText);
    mainLayout->addWidget(btnClose, 0, Qt::AlignCenter);

    setLayout(mainLayout);

    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
}