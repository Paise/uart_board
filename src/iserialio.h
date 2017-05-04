#ifndef ABSTRACTSERIALIO_H
#define ABSTRACTSERIALIO_H

#include <QObject>

#define COMMAND_CLEAR_SCREEN (0xFFC1)

class ISerialIO : public QObject
{
    Q_OBJECT

public:
    explicit ISerialIO(QObject *parent) : QObject(parent) {}
    virtual ~ISerialIO() {}

    virtual void writeAsync(const QByteArray &data) =0;
    virtual void writeAsyncByte(quint8 byte) =0;
    virtual void writeAsync2Bytes(quint16 bytes) =0;

signals:
    void dataRecieved(const QByteArray &data);
    void dataWritten(qint64 bytes);

};

#endif // ABSTRACTSERIALIO_H
