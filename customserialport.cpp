#include "customserialport.h"
#include "messagelogger.h"
#include <QTimer>
#include <QDebug>

CustomSerialPort::CustomSerialPort(QObject *parent) :
    QSerialPort(parent)
{

    connect(this, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(logPortError(QSerialPort::SerialPortError)));
}

void CustomSerialPort::connectPort()
{
    if (!this->open(QIODevice::ReadWrite)) {
        return;
    }
    emit portOpened();
    qDebug() << tr("Opened port \"%0\"").arg(this->portName());
}

void CustomSerialPort::disconnectPort()
{
    this->close();
    qDebug() << tr("Closed port \"%0\"").arg(this->portName());
}

void CustomSerialPort::logPortError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError || error == QSerialPort::NotOpenError)
        return;

    qDebug() << tr("Error: %0, (code: %1)").arg(this->errorString()).arg(error);
}
