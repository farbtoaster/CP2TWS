#ifndef COMDIALOG_H
#define COMDIALOG_H

#include <QDialog>

namespace Ui {
class comDialog;
}

class comDialog : public QDialog
{
    Q_OBJECT

public:
    explicit comDialog(QWidget *parent = nullptr);
    ~comDialog();
    void setComcomboBox(const QString &comport);
    QString getComport();

private:
    Ui::comDialog *ui;
};

#endif // COMDIALOG_H
