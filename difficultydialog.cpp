#include "difficultydialog.h"
#include <QHBoxLayout>

DifficultyDialog::DifficultyDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Select Difficulty");
    setModal(true);

    setStyleSheet(R"(
        QDialog {
            background-color: #f5f5dc; 
        }
        QLabel {
            font-family: "Garamond", serif;
            font-size: 18px;
            font-weight: bold;
            color: #5a4d41;
            margin-bottom: 15px;
        }
        QRadioButton {
            font-family: "Garamond", serif;
            font-size: 14px;
            color: #4b3832;
            margin-bottom: 5px;
        }
        QRadioButton::indicator {
            width: 15px;
            height: 15px;
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
        }
        QPushButton:hover { background-color: #cdab77; }
        QPushButton:pressed { background-color: #a08a6c; }
    )");


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QLabel* titleLabel = new QLabel("Choose Game Mode:", this);
    titleLabel->setAlignment(Qt::AlignCenter);

    rbEasy = new QRadioButton("Easy", this);
    rbMedium = new QRadioButton("Medium", this);
    rbHard = new QRadioButton("Hard", this);
    rbCustom = new QRadioButton("Custom Board", this);

    rbMedium->setChecked(true); // Default

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(rbEasy, 1); //1=Easy, 2=Medium, 3=Hard, 0=Custom
    buttonGroup->addButton(rbMedium, 2);
    buttonGroup->addButton(rbHard, 3);
    buttonGroup->addButton(rbCustom, 0);

    btnOk = new QPushButton("OK", this);
    btnCancel = new QPushButton("Cancel", this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnOk);
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addStretch(1);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(rbEasy);
    mainLayout->addWidget(rbMedium);
    mainLayout->addWidget(rbHard);
    mainLayout->addWidget(rbCustom);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    connect(btnOk, &QPushButton::clicked, this, &DifficultyDialog::acceptSelection);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void DifficultyDialog::acceptSelection() {
    selectedMode = buttonGroup->checkedId();
    accept();
}

int DifficultyDialog::getSelectedMode() const {
    return selectedMode;
}