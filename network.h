#ifndef NETWORK_H
#define NETWORK_H

#include <QByteArray>
#include <QUrlQuery>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>


class Network
{
public:
    Network(){}

    static QByteArray sendGET(QUrl uri) // Отправка GET запросов
    {
        QNetworkAccessManager* manager = new QNetworkAccessManager();
        QNetworkReply* reply = manager->get(QNetworkRequest(uri));
        QEventLoop wait;
        QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
        QTimer::singleShot(300000, &wait, SLOT(quit()));
        QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));
        wait.exec();
        QByteArray answer = reply->readAll();
        reply->deleteLater();

        return answer;
    }
};

#endif // NETWORK_H
