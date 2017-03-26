#ifndef SERIALPORT_HOLDER_H
#define SERIALPORT_HOLDER_H

#include <QSerialPort>
#include <QPointer>

class SerialPortHolder : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortHolder(QSerialPort *serial, QObject *parent = 0);

    QSerialPort* serialPort();
    void writeAsync(const QByteArray &data);

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
    void dataRecieved(const QByteArray &data);
    void dataWritten(qint64 bytes);

};

#endif // SERIALPORT_HOLDER_H
