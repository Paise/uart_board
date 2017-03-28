#ifndef ABSTRACTSERIALIO_H
#define ABSTRACTSERIALIO_H

#include <QObject>

class ISerialIO : public QObject
{
    Q_OBJECT

public:
    explicit ISerialIO(QObject *parent) : QObject(parent) {}
    virtual ~ISerialIO() {}

    virtual void writeAsync(const QByteArray &data) =0;

signals:
    void dataRecieved(const QByteArray &data);
    void dataWritten(qint64 bytes);

};

#endif // ABSTRACTSERIALIO_H
