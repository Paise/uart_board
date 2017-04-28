#include <QCoreApplication>
#include <QTimer>
#include "sender.h"
#include <QByteArray>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Sender s;
    QTimer t;
    t.setTimerType(Qt::PreciseTimer);

    s.connect(&t, &QTimer::timeout, &s, &Sender::send);
    s.clearScreen();
    t.start(128);

    return a.exec();
}
