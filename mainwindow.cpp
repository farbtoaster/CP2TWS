#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ueberdialog.h"
#include "helpdialog.h"
#include "comdialog.h"
#include "veranstaltungsdialog.h"
#include "zeitdialog.h"
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
      m_serial(new QSerialPort(this))

{
    ui->setupUi(this);

    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(&worker,&Worker::checkFinished, this, &MainWindow::checkurl_success);
    connect(&worker,&Worker::isFinished, this, &MainWindow::post_success);

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
    qInfo() <<  "com select triggered";
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
        QString first = Zeit.split(QLatin1Char('.'))[0];
        QString millis = Zeit.split(QLatin1Char('.'))[1];
        int hours = first.split(QLatin1Char(':'))[0].toInt();
        int minutes = first.split(QLatin1Char(':'))[1].toInt();
        int seconds = first.split(QLatin1Char(':'))[2].toInt();
        // Zeit manipulieren wenn A-B
        if (Zeitart == "START A-B") {
            //Jumpstart? Checkboxen auslesen -> actions auslösen (nach zeitversand oder gleich?)
            if  (seconds >= 50) {
                //jumpstart true
                if (ui->msg_checkBox->isChecked()) sendMessage(Startnummer,WP);
                if (ui->tmpn_checkBox->isChecked()) sendTimePenalty(Startnummer,WP);
                minutes++;
                if (minutes == 60)   {
                    minutes = 00;
                    hours++;
                }
                if (hours == 23) hours = 00;
            }
            seconds = 00;
            millis = "0";
        }
        //Zeitart kürzen
        Zeitart = Zeitart.split(' ')[0];
        QString nettozeit = QString::number((hours * 3600) + (minutes * 60) + seconds);
        nettozeit = nettozeit.append(".");
        nettozeit = nettozeit.append(millis);

        if (Startnummer == "0")   {
            //nach rechts kopieren als gelöscht
            delete ui->UhrlistWidget->takeItem(0);
            QString item = "gelöscht | " + Zeitstring;
            ui->SendlistWidget->insertItem(0,item);
            ui->StartNrlineEdit->clear();
            ui->StartNrlineEdit->setFocus();

        }
        else    {
            // Startnummer != 0 daten senden
            sendData(Startnummer,nettozeit,WP,Zeitart);

        }
    }
}

void MainWindow::sendData(QString Startnummer,QString nettozeit,QString WP,QString Zeitart)
{
    QByteArray postData;

    QUrlQuery query;
    query.addQueryItem("API_KEY",apikey);
    query.addQueryItem("ACTION","ZEIT_SPEICHERN");
    query.addQueryItem("WP_LAUF_NR",WP);
    query.addQueryItem("ART",Zeitart);
    query.addQueryItem("ZEIT",nettozeit);
    query.addQueryItem("STARTNUMMER",Startnummer);
    postData = query.toString(QUrl::FullyEncoded).toUtf8();
    qInfo() << postData;
    worker.post(url,postData);
    Zeitart = Zeitart.at(0);
    QString Zeitstring = ui->UhrlistWidget->item(0)->text();
    QString item = Zeitart + " | " + Startnummer + " | " + Zeitstring;
    ui->SendlistWidget->insertItem(0,item);

}

void MainWindow::sendMessage(QString Startnummer,QString WP)
{
    QByteArray postData;

    QUrlQuery query;
    query.addQueryItem("API_KEY",apikey);
    query.addQueryItem("ACTION","LEITSTELLE_MELDUNG_SPEICHERN");
    query.addQueryItem("MELDUNGART_ID","1");
    query.addQueryItem("MELDUNG","Frühstart");
    query.addQueryItem("WP_LAUF_NR",WP);
    query.addQueryItem("STARTNUMMER",Startnummer);
    query.addQueryItem("KZ_OEFFENTLICH","0");
    postData = query.toString(QUrl::FullyEncoded).toUtf8();
    qInfo() << postData;
    worker.post(url,postData);
    QString Zeitstring = ui->UhrlistWidget->item(0)->text();
    QString item = "Msg | " + Startnummer + " | " + Zeitstring;
    ui->SendlistWidget->insertItem(0,item);


}

void MainWindow::sendTimePenalty(QString Startnummer,QString WP)
{
    QByteArray postData;
    //QUrlQuery query;
    //query.addQueryItem("API_KEY",apikey);
    //query.addQueryItem("ACTION","STRAFZEIT_SPEICHERN");
    //query.addQueryItem("WP_LAUF_NR",WP);
    //query.addQueryItem("ZEIT","10.0");
    //query.addQueryItem("GRUND","Frühstart");
    //query.addQueryItem("STARTNUMMER",Startnummer);
    //postData = query.toString(QUrl::FullyEncoded).toUtf8();
    QJsonObject obj;
    obj["APIKEY"] = apikey;
    obj["ACTION"] = "STRAFZEIT_SPEICHERN";
    QJsonDocument doc(obj);
    postData = doc.toJson();
    worker.post(url,postData);
    QString Zeitstring = ui->UhrlistWidget->item(0)->text();
    QString item = "TmPn | " + Startnummer + " | " + Zeitstring;
    ui->SendlistWidget->insertItem(0,item);


}

void MainWindow::post_success(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString content = QString::fromUtf8(reply->readAll());
    qInfo() <<  "post_success";
    qInfo() <<  statusCode;
    qInfo() <<  content;
    if (statusCode == 200)  {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(content.toUtf8());

        QJsonObject jsonObject = jsonResponse.object();

        int requeststatus = jsonObject["status"].toInt();
        QString msg = jsonObject["msg"].toString();

        if (requeststatus == 201)   {
            if (msg.left(4) == "Meld") {
                ui->SendlistWidget->item(1)->setBackground(Qt::green);

            } else  {
                ui->StartNrlineEdit->clear();
                ui->SendlistWidget->item(0)->setBackground(Qt::green);
                delete ui->UhrlistWidget->takeItem(0);
            }
            ui->StartNrlineEdit->setFocus();
        ui->statuslistWidget->insertItem(0,msg);
        }
    }
    else    {   //status nicht 200 oder request return
       QMessageBox::information(this,tr("Fehler"),tr("Status: %1 ").arg(statusCode) + content);
       // action request failed
       delete ui->SendlistWidget->takeItem(0);
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
    QByteArray postData;
    QUrlQuery query;
    query.addQueryItem("API_KEY",apikey);

    postData = query.toString(QUrl::FullyEncoded).toUtf8();
    worker.urlcheck(url,postData);

}

void MainWindow::checkurl_success(QNetworkReply* reply) //Aufruf nach checkurl request
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString content = QString::fromUtf8(reply->readAll());
    qInfo() <<  "checkurl_success";
    qInfo() <<  statusCode;
    qInfo() <<  content;

    if (statusCode == 200 && content.isEmpty())  {
        ui->portallabel->setStyleSheet("background-color: lightgreen");
    }
    else    {   //status nicht 200 oder request return
       QMessageBox::information(this,tr("Fehler"),tr("Status: %1 ").arg(statusCode) + content);
       ui->portallabel->setStyleSheet("background-color: red");
    }

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

}

void MainWindow::on_actionKonfiguration_speichern_triggered()
{
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


void MainWindow::on_actionHilfe_triggered()
{
    HelpDialog myDialog;
    myDialog.exec();
}


void MainWindow::on_pushButton_clicked()
{
    ZeitDialog myzeitDialog;
    myzeitDialog.exec();
    QTime test = myzeitDialog.getTime();
    if (!test.isNull()) {   // Zeit = 00:00:00,0
        QString newtime = "Zeiteingabe    "+test.toString();
        ui->UhrlistWidget->insertItem(0,newtime);
    }

}

