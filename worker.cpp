#include "worker.h"

Worker::Worker(QObject *parent)
    : QObject{parent}
{
    connect(&manager,&QNetworkAccessManager::authenticationRequired,this,&Worker::authenticationRequired);
    connect(&manager,&QNetworkAccessManager::encrypted,this,&Worker::encrypted);
    connect(&manager,&QNetworkAccessManager::finished,this,&Worker::finished);
    connect(&manager,&QNetworkAccessManager::preSharedKeyAuthenticationRequired,this,&Worker::preSharedKeyAuthenticationRequired);
    connect(&manager,&QNetworkAccessManager::proxyAuthenticationRequired,this,&Worker::proxyAuthenticationRequired);
    connect(&manager,&QNetworkAccessManager::sslErrors,this,&Worker::sslErrors);



}

void Worker::get(QString location)
{
    qInfo() << "get request";

    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(location)));
    connect(reply,&QNetworkReply::readyRead,this,&Worker::ReadyRead);

}

void Worker::urlcheck(QString location, QByteArray data)
{
    qInfo() << "urlcheck request";

    QNetworkRequest request = QNetworkRequest(QUrl(location));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    QNetworkReply* reply = manager.post(request,data);
    connect(reply,&QNetworkReply::readyRead,this,&Worker::urlcheckReady);

}

void Worker::urlcheckReady()
{
    qInfo() << "ReadyRead";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    emit checkFinished(reply);
    if (reply) qInfo() << reply->readAll();

}

void Worker::post(QString location, QByteArray data)
{
    qInfo() << "post request";

    QNetworkRequest request = QNetworkRequest(QUrl(location));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    QNetworkReply* reply = manager.post(request,data);
    connect(reply,&QNetworkReply::readyRead,this,&Worker::ReadyRead, Qt::QueuedConnection);

}


void Worker::ReadyRead()
{
    qInfo() << "ReadyRead";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    emit isFinished(reply);
    if (reply) qInfo() << reply->readAll();

}

void Worker::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    Q_UNUSED(authenticator);
    qInfo() << "authenticationRequired";
}

void Worker::encrypted(QNetworkReply *reply)
{
     Q_UNUSED(reply);
    qInfo() << "encrypted";
}

void Worker::finished(QNetworkReply *reply)
{
     Q_UNUSED(reply);
    qInfo() << "finished";
}

void Worker::preSharedKeyAuthenticationRequired(QNetworkReply *reply, QSslPreSharedKeyAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    Q_UNUSED(authenticator);
    qInfo() << "preSharedKeyAuthenticationRequired";
}

void Worker::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{

    Q_UNUSED(authenticator);
    qInfo() << "proxyAuthenticationRequired";
}

void Worker::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(reply);

    qInfo() << "sslErrors";
}

