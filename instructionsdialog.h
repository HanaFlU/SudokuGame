#ifndef INSTRUCTIONSDIALOG_H
#define INSTRUCTIONSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextBrowser> // Use QTextBrowser for better text formatting

class InstructionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstructionsDialog(QWidget* parent = nullptr);

private:
    QTextBrowser* instructionsText;
    QPushButton* btnClose;
};

#endif // INSTRUCTIONSDIALOG_H