#include "sender.h"
#include <QDebug>

Sender::Sender(QObject *parent) :
    QObject(parent)
{
    port.setPortName("COM5");
    port.setBaudRate(QSerialPort::Baud9600);
    port.setParity(QSerialPort::NoParity);

    if (!port.open(QIODevice::ReadWrite)) {
        qDebug() << "not opened";
    }
    connect(&port, &QIODevice::readyRead, this, &Sender::readData);
    sendData = QVector<quint16>(128);
    sendData[0] = 0x4000;
    for (int i = 1; i < sendData.size(); ++i) {
        if (i > 0 && i < 26) {
            sendData[i] = sendData[i-1];
        } else if (i >= 26 && i < 52) {
            sendData[i] = sendData[i-1] + 0x6C5;
        } else if (i >= 52 && i < 78) {
            sendData[i] = sendData[i-1];
        } else if (i >= 78 && i < 104) {
            sendData[i] = sendData[i-1] - 0x3C5;
        } else {
            sendData[i] = sendData[i-1];
        }
    }
}

void Sender::clearScreen()
{
    char command[2];
    command[0] = 0xC1;
    command[1] = 0xFF;
    port.write(command, 2);
}

void Sender::send()
{
    if (sendData.isEmpty()) {
        qDebug() << "recieved: ";
        qDebug() << recievedData;
        exit(0);
    }
    quint16 n = sendData.takeFirst();
    char d[2];
    d[0] = (char) n;
    d[1] = (char) (n>>8);
    //qDebug() << "d[1]=" << (quint8) d[0];
    //qDebug() << "d[2]=" << (quint8) d[1];
    port.write(d, 2);
}

void Sender::readData()
{
    if (port.bytesAvailable() == 0) return;

    QByteArray d = port.readAll();
    for (int i = 0; i < d.count(); ++i) {
        qDebug() << (quint8) d[i];
        recievedData.append((quint8) d[i]);
    }
}
