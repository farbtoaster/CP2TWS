#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ueberdialog.h"
#include "comdialog.h"
#include "veranstaltungsdialog.h"
#include "QtSerialPort/QSerialPortInfo"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QFileDialog>
#include <QSettings>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_serial(new QSerialPort(this)),
      m_networkManager(new QNetworkAccessManager(this))

{

    ui->setupUi(this);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onPostAnswer(QNetworkReply*)));
    connect(m_networkManager, SIGNAL(sslerror(QNetworkReply*)), SLOT(slotError(QNetworkReply*)));
    QEventLoop loop;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openSerialPort()
{
    m_serial->setPortName(ui->Comlabel->text());
    m_serial->setBaudRate(9600);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        ui->connectButton->setText("verbunden");
        ui->connectButton->setChecked(true);

    } else {
        QMessageBox::critical(this, tr("COM Fehler"), m_serial->errorString());

    }
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();

}

void MainWindow::readData()
{

    const QByteArray data = m_serial->readAll();

        charBuffer.append(data);
        if (charBuffer.contains("\r\n"))
        {
            //qInfo(charBuffer);
            charBuffer.chop(2);
            ui->UhrlistWidget->addItem(charBuffer);
            charBuffer.clear();
            ui->UhrlistWidget->setCurrentRow(0);
            ui->UhrlistWidget->item(0)->setBackground(Qt::lightGray);
            ui->StartNrlineEdit->setFocus();
        }
}

void MainWindow::on_connectButton_clicked()
{
    if (ui->connectButton->isChecked()) {
        ui->connectButton->setText("Uhr verbinden");
        ui->connectButton->setChecked(false);
        closeSerialPort();
    }
    else    {
        if (ui->Comlabel->text() != "Kein COM")  {
        openSerialPort();
        }
        else    {

            QMessageBox::information(
                    this,
                    tr("Hinweis"),
                    tr("kein COM Port ausgewählt!") );
        }
    }
}


void MainWindow::on_actionUeber_CP2TWS_triggered()
{
    ueberDialog myDialog;
    myDialog.exec();
}


void MainWindow::on_actionCOMselect_triggered()
{
    comDialog mycomdialog;  //Konstruktor comdialog
    QString comport;
    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo info, infoList) {
        comport = info.portName();
        mycomdialog.setComcomboBox( comport);
    }
    mycomdialog.exec();     //comdialog modal ausführen
    ui->Comlabel->setText(mycomdialog.getComport());    //update Com Label
}


void MainWindow::on_actionexit_triggered()
{
    closeSerialPort();  // Port schliessen
    this->close();  //Anwendung beenden
}



void MainWindow::on_sendButton_clicked()
{
    bool error = false;
    QString errortext;
    QString Startnummer = ui->StartNrlineEdit->text();  //Startnummer holen
    if (Startnummer.isEmpty())    {
        error = true;
        errortext = "keine Startnummer vergeben\n";
        }


    if (ui->UhrlistWidget->count() == 0)    {
            error = true;
            errortext = errortext + "keine Zeit vorhanden\n";
        }
    WP = ui->WPcomboBox->currentText();
    if (WP.isEmpty())    {
            error = true;
            errortext = errortext + "keine WP gewählt\n";
        }
    else    {
        WP.remove(0,3);
    }
    Zeitart = ui->ZeitartcomboBox->currentText();   // WP und Zeitart holen
    if (Zeitart.isEmpty())    {
            error = true;
            errortext = errortext + "keine Zeitart gewählt";
        }

    if (error)  {
        QMessageBox::information(
                this,
                tr("Hinweis"),
                (errortext) );
        }
    else    {
        QString Zeitstring = ui->UhrlistWidget->item(0)->text();    //oberste zeit holen
        // nettozeit erzeugen
        QString Zeit = Zeitstring.mid(16,12); // ab zeichen 16, 12 Zeichen ausschneiden

        int hours = Zeit.split(QLatin1Char(':'))[0].toInt();
        int minutes = Zeit.split(QLatin1Char(':'))[1].toInt();
        int seconds = Zeit.split(QLatin1Char(':'))[2].toInt();
        QString millis = Zeit.split(QLatin1Char('.'))[1];
        QString nettozeit = QString::number((hours * 3600) + (minutes * 60) + seconds);
        nettozeit = nettozeit.append(".");
        nettozeit = nettozeit.append(millis);
        if (Startnummer == "0")   {
            //nach rechts kopieren als gelöscht
            delete ui->UhrlistWidget->takeItem(0);
            //ui->SendlistWidget->addItem("gelöscht | " + Zeitstring);
            QString item = "gelöscht | " + Zeitstring;
            ui->SendlistWidget->insertItem(0,item);
            ui->StartNrlineEdit->clear();
            ui->StartNrlineEdit->setFocus();

        }
        else    {
            // Startnummer != 0 daten senden
            sendData(Startnummer,nettozeit,WP,Zeitart,Zeit);
        }
    }
}

void MainWindow::sendData(QString Startnummer,QString nettozeit,QString WP,QString Zeitart,QString Zeit)
{
    // url und apikey holen


    QUrl serviceUrl = QUrl(url);
    QByteArray postData;

    QUrlQuery query;
    query.addQueryItem("API_KEY",apikey);
    query.addQueryItem("ACTION","ZEIT_SPEICHERN");
    query.addQueryItem("WP_LAUF_NR",WP);
    query.addQueryItem("ART",Zeitart);
    query.addQueryItem("ZEIT",nettozeit);
    query.addQueryItem("STARTNUMMER",Startnummer);

    postData = query.toString(QUrl::FullyEncoded).toUtf8();

    //QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    //connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onPostAnswer(QNetworkReply*)));
    //connect(m_networkManager, SIGNAL(error(QNetworkReply*)), SLOT(slotError(QNetworkReply*)));

    QNetworkRequest networkRequest(serviceUrl);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    m_networkManager->post(networkRequest,postData);
    Zeitart = Zeitart.at(0);
    QString Zeitstring = ui->UhrlistWidget->item(0)->text();
    //ui->SendlistWidget->addItem( Zeitart + " | " + Startnummer + " | " + Zeitstring);
    QString item = Zeitart + " | " + Startnummer + " | " + Zeitstring;
    ui->SendlistWidget->insertItem(0,item);
    //delete networkManager;

}
void MainWindow::slotError(QNetworkReply::NetworkError)
{

    QMessageBox::information(
            this,
            tr("Hinweis"),
            tr("Fehler Netzwerk"));
}

void MainWindow::onPostAnswer(QNetworkReply* reply) //Aufruf nach request
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();


    QString content = QString::fromUtf8(reply->readAll());

    if (statusCode == 200)  {
        if (!content.isEmpty())    {
            QMessageBox::information(this,tr("Fehler"),tr("Status: %1 ").arg(statusCode) + content); //status 200 - Fehler im string
        }
        else    // status 200 ohne return - request ok
        {
            delete ui->UhrlistWidget->takeItem(0);
            //delete item
            ui->StartNrlineEdit->clear();
            ui->StartNrlineEdit->setFocus();
        }
    }
    else    {   //status nicht 200
       QMessageBox::information(this,tr("Fehler"),tr("Status: %1 ").arg(statusCode) + content);
    }



}

void MainWindow::on_actionVeranstaltung_triggered()
{
    veranstaltungsdialog myveranstaltungsdialog;
    if (!portalurl.isEmpty()) {
        myveranstaltungsdialog.setPortalurl(portalurl);
    }
    if (!folder.isEmpty()) {
        myveranstaltungsdialog.setVeranstaltung(folder);
    }
    if (!apikey.isEmpty()) {
        myveranstaltungsdialog.setapikey(apikey);
    }
    myveranstaltungsdialog.exec();
    portalurl = myveranstaltungsdialog.getPortalurl();
    folder = myveranstaltungsdialog.getVeranstaltung();
    apikey = myveranstaltungsdialog.getapiKey();
    QString filename = "/api.php";
    url = portalurl;
    url.append(folder);
    url.append(filename);
    ui->portallabel->setText(url);
    checkurl();


}

void MainWindow::checkurl()
{
    QString baseurl = portalurl;
    baseurl.append(folder);
    baseurl.append("/");
    QUrl testurl = QUrl(baseurl);
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onGetAnswer(QNetworkReply*)));
    QNetworkRequest testRequest(testurl);

    testRequest.setHeader(QNetworkRequest::UserAgentHeader, QVariant("Google Chrome "));


    networkManager->get(testRequest);


}

void MainWindow::onGetAnswer(QNetworkReply* reply) //Aufruf nach request
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();


    QString content = QString::fromUtf8(reply->readAll());

    if (statusCode == 200)  {
        ui->portallabel->setStyleSheet("background-color: lightgreen");
    }
    else    {   //status nicht 200
       QMessageBox::information(this,tr("Fehler"),tr("Status: %1 ").arg(statusCode) + content);
       ui->portallabel->setStyleSheet("background-color: red");
    }
    QObject *networkManager = sender();
    networkManager->deleteLater();
}
void MainWindow::on_StartNrlineEdit_returnPressed()
{
    on_sendButton_clicked();
}




void MainWindow::on_UhrlistWidget_itemDoubleClicked(QListWidgetItem *listWidgetItem)
{
    QString test = listWidgetItem->text();
    ui->UhrlistWidget->insertItem(0,test);
    ui->UhrlistWidget->repaint();
    delete ui->UhrlistWidget->takeItem(ui->UhrlistWidget->row(listWidgetItem));
    ui->UhrlistWidget->setCurrentRow(0);
}


void MainWindow::on_SendlistWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString selected = item->text();
    QStringList pieces = selected.split('|');
    selected = pieces[pieces.count() - 1];
    selected = selected.trimmed();
    ui->UhrlistWidget->insertItem(0,selected);
    ui->UhrlistWidget->repaint();
    delete ui->SendlistWidget->takeItem(ui->SendlistWidget->row(item));
    ui->UhrlistWidget->setCurrentRow(0);
}


void MainWindow::on_actionKonfiguration_laden_triggered()
{
    QString filter = "CP2TWS Konfiguration (*.konf)";
    QString configFile = QFileDialog::getOpenFileName(this, "Select a file...", QDir::homePath(), filter);
    if (!configFile.isEmpty()) {
        loadSettings(configFile);

    }
}

void MainWindow::loadSettings(QString file)
{
    // .ini format example
    QSettings settings(file, QSettings::IniFormat);

    QString portal = settings.value("OnlinePortal").toString();
    if (!portal.isEmpty())  {
        portalurl = portal;
    }
    QString name = settings.value("VerzeichnisName").toString();
    if (!name.isEmpty())    {
        folder = name;
    }
    QString api = settings.value("ApiKey").toString();
    if (!api.isEmpty()) {
        apikey = api;
    }
    QString filename = "/api.php";
    url = portalurl;
    url.append(folder);
    url.append(filename);
    ui->portallabel->setText(url);
    checkurl();

    QString wp = settings.value("WP").toString();
    if (!wp.isEmpty())  {
        WP = wp;
        ui->WPcomboBox->setCurrentText("WP " + WP);
    }
    QString zeitart = settings.value("Zeitart").toString();
    if (!zeitart.isEmpty()) {
        Zeitart = zeitart;
        ui->ZeitartcomboBox->setCurrentText(Zeitart);
    }

    // ...
}

void MainWindow::on_actionKonfiguration_speichern_triggered()
{
    //QString filter = "CP2TWS Konfiguration (*.konf)";
    QString configFile = QFileDialog::getSaveFileName(this,tr("Konfiguration speichern unter"), QDir::homePath(),tr("Konfiguration (*.konf);;All Files (*)"));
    QSettings settings(configFile, QSettings::IniFormat);
    settings.setValue("OnlinePortal",portalurl);
    settings.setValue("VerzeichnisName",folder);
    settings.setValue("ApiKey",apikey);
    WP = ui->WPcomboBox->currentText();
    QString wpNr = WP.remove(0,3);
    settings.setValue("WP",wpNr);
    Zeitart = ui->ZeitartcomboBox->currentText();
    settings.setValue("Zeitart",Zeitart);


}

