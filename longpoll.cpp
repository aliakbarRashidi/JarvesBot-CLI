#include "longpoll.h"

using namespace QtJson;

void LongPoll::run()
{
    QByteArray answer ="dfgd";
    while( !(answer.isEmpty()) )
    {
        QUrlQuery request("https://api.telegram.org/bot"+token+"/getUpdates?offset="+offset);
        QString urlString = request.toString();
        QUrl url(urlString);
        answer = Network::sendGET(url);
        score++;

        QVariantList update = parse(answer).toMap().value("result").toList();

        if(update.size() < 1)
        {
            qDebug() << "Нет данных в ответе" << score;
            continue;
        }

        for(int i = 0; i < update.size(); i++)
        {
            QVariantMap item = update[i].toMap();
            QString up_id = item.value("update_id").toString();

            int tempId = up_id.toInt();
            tempId++;
            up_id = QString::number( tempId );

            offset = up_id;
        }

        emit nonEmptyAnswer( answer );

        QEventLoop loop;
        QTimer::singleShot(5000, &loop, SLOT(quit()));
        loop.exec();
    }
}

void LongPoll::workAnswer(QByteArray answer)
{
    QVariantList update = parse(answer).toMap().value("result").toList();

    for(int i = 0; i < update.size(); i++)
    {
        QVariantMap item = update[i].toMap();
        QString up_id = item.value("update_id").toString();
        QVariantMap message = item.value("message").toMap();
            QString mess_id = message.value("message_id").toString();
            QVariantMap from = message.value("from").toMap();
                QString from_id = from.value("id").toString();
                QString from_nick = from.value("username").toString();
                QString from_fname = from.value("first_name").toString();
                QString from_lname = from.value("last_name").toString();
            QVariantMap chat = message.value("chat").toMap();
                QString chat_id = chat.value("id").toString();
                QString chat_name = chat.value("title").toString();
            QString mess_text = message.value("text").toString();

        qDebug() << "Я получил такое сообщение:";
        qDebug() << "    From ID: " << from_id;
        qDebug() << "    From NICK: " << from_nick;
        qDebug() << "    From Last name: " << from_lname;
        qDebug() << "    From First name: " << from_fname;
        qDebug() << "    Message: " << mess_text;

        QString text_answ = getAnswerToUser(
                    mess_text,
                    from_id,
                    chat_id,
                    from_nick,
                    from_lname,
                    from_fname
        );

        QUrlQuery request("https://api.telegram.org/bot"+token+"/sendMessage?chat_id="+chat_id);
        request.addQueryItem("text", text_answ);
        QString urlString = request.toString();
        QUrl url(urlString);
        QByteArray callback = Network::sendGET( url );

        if( !(callback.isEmpty()) )
        {
            qDebug() << "Я ответил на него так:";
            qDebug() << "    Answer: " << text_answ;
        }
    }
}

QString LongPoll::getAnswerToUser(const QString &mess_text, const QString &user_id,
                                  const QString& chat_id, const QString &user_nick,
                                  const QString &user_lname, const QString &user_fname)
{
    QString botAnswer = "";

    if(mess_text == "start tv")
    {
        botAnswer = startTeamViewer();
    }
    else if(mess_text == "stop tv")
    {
        botAnswer = stopTeamViewer();
    }

    return botAnswer;
}

QString LongPoll::startTeamViewer()
{
    QString program = "/Applications/TeamViewer.app/Contents/MacOS/TeamViewer";
    QStringList arguments;
    //arguments << "-style" << "fusion";

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);

    return "TeamViewer starting";
}

QString LongPoll::stopTeamViewer()
{
    QString program = "/usr/bin/killall";
    QStringList arguments;
    arguments << "TeamViewer";

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);

    return "TeamViewer stoping";
}
