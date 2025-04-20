#ifndef DIFFICULTYDIALOG_H
#define DIFFICULTYDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QLabel>

class DifficultyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DifficultyDialog(QWidget* parent = nullptr);
    int getSelectedMode() const;

private slots:
    void acceptSelection();

private:
    QRadioButton* rbEasy;
    QRadioButton* rbMedium;
    QRadioButton* rbHard;
    QRadioButton* rbCustom;
    QButtonGroup* buttonGroup;
    QPushButton* btnOk;
    QPushButton* btnCancel;
    int selectedMode = 2; // Default to Medium
};

#endif // DIFFICULTYDIALOG_H
