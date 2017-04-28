#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QSerialPort>
#include <QVector>

class Sender : public QObject
{
    Q_OBJECT
public:
    explicit Sender(QObject *parent = 0);

    void clearScreen();

private:
    QSerialPort port;
    QVector<quint16> sendData;
    QList<quint8> recievedData;

signals:

public slots:
    void send();

private slots:
    void readData();

};

#endif // SENDER_H
