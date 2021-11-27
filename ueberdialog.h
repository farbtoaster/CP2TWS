#ifndef UEBERDIALOG_H
#define UEBERDIALOG_H

#include <QDialog>

namespace Ui {
class ueberDialog;
}

class ueberDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ueberDialog(QWidget *parent = nullptr);
    ~ueberDialog();

private:
    Ui::ueberDialog *ui;
};

#endif // UEBERDIALOG_H
