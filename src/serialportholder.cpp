#include "serialportholder.h"
#include <QDebug>

SerialPortHolder::SerialPortHolder(QSerialPort *serial, QObject *parent) :
    ISerialIO(parent),
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
    qDebug() << tr("Data to send:");
    for (int i = 0; i < data.size(); ++i) {
        qDebug() << (quint8) data[i];
    }
}

void SerialPortHolder::writeAsyncByte(quint8 byte)
{
    QByteArray d;
    d.append((char) byte);
    writeAsync(d);
}

void SerialPortHolder::writeAsync2Bytes(quint16 byte)
{
    QByteArray d;
    d.append((char) byte);
    d.append((char) (byte >> 8));
    writeAsync(d);
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
    qDebug() << tr("Data recieved:");
    for (int i = 0; i < data.size(); ++i) {
        qDebug() << (quint8) data[i];
    }
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
