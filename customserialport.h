#ifndef CUSTOMSERIALPORT_H
#define CUSTOMSERIALPORT_H

#include <QSerialPort>

class CustomSerialPort : public QSerialPort
{
    Q_OBJECT
public:
    explicit CustomSerialPort(QObject *parent = 0);

private:

public slots:
    void connectPort();
    void disconnectPort();

private slots:
    void logError(QSerialPort::SerialPortError);

signals:
    void opened();

};

#endif // CUSTOMSERIALPORT_H
