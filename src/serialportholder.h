#ifndef SERIALPORT_HOLDER_H
#define SERIALPORT_HOLDER_H

#include "iserialio.h"
#include <QSerialPort>
#include <QPointer>

class SerialPortHolder : public ISerialIO
{
    Q_OBJECT
public:
    explicit SerialPortHolder(QSerialPort *serial, QObject *parent = 0);

    QSerialPort* serialPort();
    void writeAsync(const QByteArray &data);
    void writeAsyncByte(quint8 byte);
    void writeAsync2Bytes(quint16 bytes);

private:
    QPointer<QSerialPort> m_serialPort;

public slots:
    void connectPort(const QString &name = QString());
    void disconnectPort();

private slots:
    void confirmDataWritten(quint64 bytes);
    void recieveData();
    void logError(QSerialPort::SerialPortError);

signals:
    void closed();
    void opened();
    void errorOccured(const QString &msg, QSerialPort::SerialPortError);

};

#endif // SERIALPORT_HOLDER_H
