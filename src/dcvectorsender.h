#ifndef DCVECTORSENDER_H
#define DCVECTORSENDER_H

#include "iserialio.h"
#include <QObject>
#include <QTimer>
#include <QList>
#include <QPointF>

class DCVectorSender : public QObject
{
    Q_OBJECT
public:
    explicit DCVectorSender(ISerialIO *serial, QObject *parent = 0);

private:
    ISerialIO *m_serial;
    QList<QPointF> m_points;
    QTimer m_timer;
    uint32_t m_currentIndex;

signals:
    void sended(int index, quint16 val);
    void started();
    void completed();

public slots:
    void start(QList<QPointF> points, int interval);
    void cancel();

private slots:
    void sendNext();

};

#endif // DCVECTORSENDER_H
