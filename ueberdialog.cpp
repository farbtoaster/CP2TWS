#include "ueberdialog.h"
#include "ui_ueberdialog.h"


ueberDialog::ueberDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ueberDialog)
{
    ui->setupUi(this);
}

ueberDialog::~ueberDialog()
{
    delete ui;
}
