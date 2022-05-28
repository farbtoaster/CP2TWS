#ifndef WORKER_H
#define WORKER_H

#include <QtCore/QObject>
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QAuthenticator>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);


signals:
    void isFinished(QNetworkReply* reply);
    void checkFinished(QNetworkReply* reply);
    void timeRequestFinished(QNetworkReply* reply);

public slots:
    void    get(QString location);
    void    put(QString location, QByteArray data);
    void    post(QString location, QByteArray data);
    void    urlcheck(QString location, QByteArray data);
    void    ReadyRead();
    void    urlcheckReady();
    void    TimeRequestReady();
private slots:

    void 	authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void 	encrypted(QNetworkReply *reply);
    void 	finished(QNetworkReply *reply);
    void 	preSharedKeyAuthenticationRequired(QNetworkReply *reply, QSslPreSharedKeyAuthenticator *authenticator);
    void 	proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
    void 	sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
private:
    QNetworkAccessManager manager;
};

#endif // WORKER_H
