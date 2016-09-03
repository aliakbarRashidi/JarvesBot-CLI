#include <QCoreApplication>
#include "longpoll.h"
#include "network.h"
#include "Json/json.h"

using namespace QtJson;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString token = "106636311:AAHnALgQ-34yTf-5QgQjut8FMPrTpCQ1Ta4";
    QString offset = "0";
    int score = 0;

    LongPoll * longPoll = new LongPoll( token, offset, score );

    QUrlQuery request("https://api.telegram.org/bot"+token+"/getMe");
    QString urlString = request.toString();
    QUrl url(urlString);
    QByteArray answer = Network::sendGET(url);
    qDebug() << answer;

    longPoll->start();

    return a.exec();
}
