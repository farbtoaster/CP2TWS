#include "zeitdialog.h"
#include "ui_zeitdialog.h"

ZeitDialog::ZeitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZeitDialog)
{
    ui->setupUi(this);
}

ZeitDialog::~ZeitDialog()
{
    delete ui;
}

QTime ZeitDialog::getTime()
{
    return(ui->timeEdit->time());
}



