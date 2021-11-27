#include "veranstaltungsdialog.h"
#include "ui_veranstaltungsdialog.h"

veranstaltungsdialog::veranstaltungsdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::veranstaltungsdialog)
{
    ui->setupUi(this);
}

veranstaltungsdialog::~veranstaltungsdialog()
{
    delete ui;
}

QString veranstaltungsdialog::getPortalurl()
{
    QString serverurl = ui->servercomboBox->currentText();
    return serverurl;;
}
QString veranstaltungsdialog::getVeranstaltung()
{

    QString folder = ui->verzeichnisslineEdit->text();
    return(folder);
}

QString veranstaltungsdialog::getapiKey()
{
    QString apikey = ui->apikeylineEdit->text();
    return(apikey);
}

void veranstaltungsdialog::setPortalurl(const QString &portalurl)
{
    ui->servercomboBox->setCurrentText(portalurl);
}

void veranstaltungsdialog::setVeranstaltung(const QString &veranstaltung)
{
    ui->verzeichnisslineEdit->setText(veranstaltung);
}

void veranstaltungsdialog::setapikey(const QString &apikey)
{
    ui->apikeylineEdit->setText(apikey);
}
