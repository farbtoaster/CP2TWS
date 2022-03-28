#ifndef ZEITDIALOG_H
#define ZEITDIALOG_H

#include <QDialog>

namespace Ui {
class ZeitDialog;
}

class ZeitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ZeitDialog(QWidget *parent = nullptr);
    ~ZeitDialog();
    QTime getTime();

private:
    Ui::ZeitDialog *ui;
};

#endif // ZEITDIALOG_H
