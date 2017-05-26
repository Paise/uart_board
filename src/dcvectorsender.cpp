#include "dcvectorsender.h"
#include <QDebug>

DCVectorSender::DCVectorSender(ISerialIO *serial, QObject *parent) :
    QObject(parent),
    m_serial(serial),
    m_currentIndex(0),
    m_skipNextAndStop(false)
{
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &DCVectorSender::sendNext);
}

void DCVectorSender::start(QList<QPointF> points, int interval)
{
    if (m_timer.isActive()) return;
    if (points.isEmpty()) {
        emit completed();
        return;
    }

    m_currentIndex = 0;
    m_skipNextAndStop = false;
    m_points = points;
    m_serial->writeAsyncByte(COMMAND_CLEAR_SCREEN);
    m_timer.start(interval);
    emit started();
}

void DCVectorSender::cancel()
{
    if (!m_timer.isActive()) return;

    m_currentIndex = 0;
    m_skipNextAndStop = false;
    m_points.clear();
    m_timer.stop();
    emit completed();
}

void DCVectorSender::sendNext()
{
    if (m_skipNextAndStop) {
        cancel();
        return;
    }

    quint16 val = m_points.value(m_currentIndex).ry();
    //m_serial->writeAsyncByte(quint8((val >> 8) / 2));
    m_serial->writeAsyncByte(val);
    emit sended(m_currentIndex, val);
    if (++m_currentIndex == m_points.count()) {
        m_skipNextAndStop = true;
    }
}
