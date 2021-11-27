#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QNetworkReply>
#include <QNetworkAccessManager>
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
    void sendData(QString Startnummer,QString nettozeit,QString WP,QString Zeitart,QString Zeit);
    void onPostAnswer(QNetworkReply* reply);
    void slotError(QNetworkReply::NetworkError);
    void on_sendButton_clicked();

    void on_actionVeranstaltung_triggered();
    void checkurl();
     void onGetAnswer(QNetworkReply* reply);
    void on_StartNrlineEdit_returnPressed();



    void on_UhrlistWidget_itemDoubleClicked(QListWidgetItem *listWidgetItem);

    void on_SendlistWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QSerialPort *m_serial = nullptr;
    QNetworkAccessManager *m_networkManager = nullptr;
    QByteArray charBuffer;
    QString url,apikey,folder,portalurl;


};
#endif // MAINWINDOW_H