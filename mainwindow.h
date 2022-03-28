#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <worker.h>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_connectButton_clicked();
    void openSerialPort();
    void closeSerialPort();
    void readData();
    void on_actionUeber_CP2TWS_triggered();
    void on_actionCOMselect_triggered();
    void on_actionexit_triggered();
    void sendData(QString Startnummer,QString nettozeit,QString WP,QString Zeitart);
    void sendMessage(QString Startnummer,QString WP);
    void sendTimePenalty(QString Startnummer,QString WP);
    void on_sendButton_clicked();
    void on_actionVeranstaltung_triggered();
    void checkurl();
    void checkurl_success(QNetworkReply *reply);
    void post_success(QNetworkReply *reply);
    void on_StartNrlineEdit_returnPressed();
    void loadSettings(QString file);
    void on_UhrlistWidget_itemDoubleClicked(QListWidgetItem *listWidgetItem);
    void on_SendlistWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_actionKonfiguration_laden_triggered();
    void on_actionKonfiguration_speichern_triggered();
    void on_actionHilfe_triggered();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *m_serial = nullptr;
    QByteArray charBuffer;
    QString url,apikey,folder,portalurl,WP,Zeitart;
    Worker worker;


};
#endif // MAINWINDOW_H
