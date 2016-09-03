#ifndef LONGPOLL_H
#define LONGPOLL_H

#include <QThread>
#include <QString>
#include <QByteArray>

#include <QUrlQuery>
#include <QUrl>


#include <QMap>
#include <QList>

#include <QVariantMap>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>

#include <QFile>
#include <QDataStream>

#include <QDebug>

#include "Json/json.h"
#include "network.h"

class LongPoll : public QThread
{
    Q_OBJECT
public:
    LongPoll( const QString& _token, QString& _offset, int _score ):token(_token),offset(_offset),score(_score)
    {
        connect(this, SIGNAL( nonEmptyAnswer(QByteArray) ), this, SLOT( workAnswer(QByteArray) ) );
    }
    void run();
signals:
    void nonEmptyAnswer( QByteArray );

private slots:
    void workAnswer( QByteArray answer );

private:
    QString getAnswerToUser( const QString& mess_text, const QString& user_id, const QString& chat_id, const QString& user_nick, const QString& user_lname, const QString& user_fname );
    QString getInfoToday();
    QString setInfo( const QString& info );

private:
    QString token;
    QString& offset;
    int score;
};

#endif // LONGPOLL_H
