#include "comdialog.h"
#include "ui_comdialog.h"

comDialog::comDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::comDialog)
{
    ui->setupUi(this);

}
void comDialog::setComcomboBox(const QString &comport)
{
    ui->ComcomboBox->addItem(comport);
}

QString comDialog::getComport()
{
    return(ui->ComcomboBox->currentText());
}

comDialog::~comDialog()
{
    delete ui;
}
