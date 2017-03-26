#include "serialportholder.h"
#include "messagelogger.h"
#include <QDebug>

SerialPortHolder::SerialPortHolder(QSerialPort *serial, QObject *parent) :
    AbstractSerialIO(parent),
    m_serialPort(serial)
{
    m_serialPort->setParent(this);

    connect(serial, &QSerialPort::bytesWritten, this, &SerialPortHolder::confirmDataWritten);
    connect(serial, &QSerialPort::readyRead, this, &SerialPortHolder::recieveData);
    connect(serial, &QSerialPort::aboutToClose, this, &SerialPortHolder::closed);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(logError(QSerialPort::SerialPortError)));
}

QSerialPort *SerialPortHolder::serialPort()
{
    return m_serialPort.data();
}

void SerialPortHolder::writeAsync(const QByteArray &data)
{
    m_serialPort->write(data);
    qDebug() << tr("Data to send: %0").arg(QString(data));
}

void SerialPortHolder::connectPort(const QString &name)
{
    if (!name.isEmpty()) {
        m_serialPort->setPortName(name);
    }
    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        return;
    }
    qDebug() << tr("Opened port \"%0\"").arg(m_serialPort->portName());
    emit opened();
}

void SerialPortHolder::disconnectPort()
{
    m_serialPort->close();
    qDebug() << tr("Closed port \"%0\"").arg(m_serialPort->portName());
}

void SerialPortHolder::confirmDataWritten(quint64 bytes)
{
    qDebug() << tr("%0 bytes of data succesfully sended").arg(bytes);
    emit dataWritten(bytes);
}

void SerialPortHolder::recieveData()
{
    QByteArray data = m_serialPort->readAll();
    qDebug() << tr("Data recieved: %0").arg(QString(data));
    emit dataRecieved(data);
}

void SerialPortHolder::logError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError || error == QSerialPort::NotOpenError)
        return;

    qDebug() << tr("Error: %0, (code: %1)").arg(m_serialPort->errorString()).arg(error);
    emit errorOccured(m_serialPort->errorString(), error);
    m_serialPort->clearError();
}
