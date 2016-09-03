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

        QVariantList update = parse(answer).toMap().value("result").toList();

        if(update.size() < 1)
        {
            qDebug() << "Нет данных в ответе" << score;
            score++;
        }
        else
        {
            score++;

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
        }

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

        QString text_answ = getAnswerToUser( mess_text, from_id, chat_id, from_nick, from_lname, from_fname );

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

QString LongPoll::getAnswerToUser(const QString &mess_text, const QString &user_id, const QString& chat_id, const QString &user_nick, const QString &user_lname, const QString &user_fname)
{
    QString botAnswer = "";

    if( mess_text == "/start" )
        botAnswer = "Добро пожаловать! Тебя приветствует Telegram Bot Jarves. "
                    "У меня нет конкретной цели и задачи. Я просто буду тебе помогать.";
    else if( mess_text == "/rand")
        botAnswer = QString::number( rand()%100 );
    else if( mess_text == "Привет")
        botAnswer = "Привет, " + user_fname;
    else if(mess_text == "Погода" || mess_text == "/weather")
    {
        QUrlQuery request("http://shotinleg.xyz/api/getWeather.php");
        QString urlString = request.toString();
        QUrl url(urlString);
        QByteArray answer = Network::sendGET(url);
        qDebug() << answer;
        QVariantMap info = parse(answer).toMap().value("result").toMap();
        QString city = info.value("city").toString();
        QString degr = info.value("degr").toString();
        QString osad = info.value("osad").toString();

        botAnswer = "Погода в городе "+city+": \n" + degr + "\n" + osad;
    }
    else if(mess_text == "Новости" || mess_text == "/newshitech")
    {
        QUrlQuery request("http://shotinleg.xyz/api/getNews.php");
        QString urlString = request.toString();
        QUrl url(urlString);
        QByteArray answer = Network::sendGET(url);
        qDebug() << answer;
        QVariantList info = parse(answer).toMap().value("result").toList();

        for(int i = 0; i < info.size(); i++)
        {
            QVariantMap item = info[i].toMap();
            QString title = item.value("title").toString();
            QString link = item.value("link").toString();
            QString desc = item.value("desc").toString();
            QString time = item.value("time").toString();

            botAnswer += title + "\n\n" +desc + "\n\nВремя: " + time + "\n******************************\n";
        }
    }
    else if(mess_text == "Расписание Адамант" || mess_text == "/listadamant")
    {
        QUrlQuery request("http://shotinleg.xyz/api/getAdamant.php");
        QString urlString = request.toString();
        QUrl url(urlString);
        QByteArray answer = Network:: sendGET(url);
        qDebug() << answer;
        QVariantList info = parse(answer).toMap().value("result").toList();

        for(int i = 0; i < info.size(); i++)
        {
            QVariantMap item = info[i].toMap();
            QString name = item.value("name").toString();
            QString time = item.value("time").toString();
            QString place = item.value("place").toString();

            botAnswer += name + "\n" + time + "\n" + place + "\n******************************\n";
        }
    }

    if( user_id == "56854054" )
    {
        if( mess_text == "/today" )
            botAnswer = getInfoToday();
        else if( mess_text.contains( QRegExp( "^[0-9]" ) ) )
            botAnswer = setInfo( mess_text );
    }

    return botAnswer;
}

QString LongPoll::getInfoToday()
{
    QString infoToday = "";

    QFile file ("work.txt");

    QDate date;
    int today = date.currentDate().year() * 10000;
    today += date.currentDate().month() * 100;
    today += date.currentDate().day();

    QString s_today = QString::number( today );

    if( file.open(QIODevice::ReadWrite) )
    {
        QVariantMap parametrs;
        QDataStream stream(&file);

        stream >> parametrs;
        file.close();

        if( parametrs.contains(s_today) )
        {
            infoToday = parametrs.value( s_today ).toString();
        }
    }

    return "Информация на сегодня: \n\n" + infoToday;
}

QString LongPoll::setInfo(const QString &info)
{
    QString botAnswer = "Не удалось записать.";

    QFile file ("work.txt");

    QString date = info.split("=")[0];
    QString infor = info.split("=")[1];


    if( file.open(QIODevice::ReadWrite) )
    {
        QVariantMap parametrs;
        QDataStream stream(&file);

        parametrs.insert(date, infor);

        stream << parametrs;

        botAnswer = "Записанно";

        file.close();
    }

    return botAnswer;
}


//"{\"ok\":true,\"result\":[{\"update_id\":840858210,\n\"message\":{\"message_id\":565,\"from\":{\"id\":56854054,\"first_name\":\"\\u0410\\u043d\\u0442\\u043e\\u043d\",\"last_name\":\"\\u0417\\u0432\\u043e\\u043d\\u0430\\u0440\\u0435\\u0432\",\"username\":\"shotInLeg\"},\"chat\":{\"id\":56854054,\"first_name\":\"\\u0410\\u043d\\u0442\\u043e\\u043d\",\"last_name\":\"\\u0417\\u0432\\u043e\\u043d\\u0430\\u0440\\u0435\\u0432\",\"username\":\"shotInLeg\",\"type\":\"private\"},\"date\":1472914022,\"text\":\"\\u041f\\u0440\\u0438\\u0432\\u0435\\u0442\"}}]}"
