#include "dcvectorsender.h"
#include <QDebug>

DCVectorSender::DCVectorSender(ISerialIO *serial, QObject *parent) :
    QObject(parent),
    m_serial(serial),
    m_currentIndex(0)
{
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &DCVectorSender::sendNext);
}

void DCVectorSender::clear()
{
    m_currentIndex = 0;
    m_points.clear();
    m_timer.stop();
}

void DCVectorSender::start(QList<QPointF> points, int interval)
{
    if (m_timer.isActive()) return;
    if (points.isEmpty()) return;

    m_currentIndex = 0;
    m_points = points;
    m_serial->writeAsync2Bytes(COMMAND_CLEAR_SCREEN);
    m_timer.start(interval);
}

void DCVectorSender::cancel()
{
    if (!m_timer.isActive()) return;

    clear();
    emit interrupted();
}

void DCVectorSender::sendNext()
{
    quint16 val = m_points.value(m_currentIndex).ry();
    m_serial->writeAsync2Bytes(val);
    emit sended(m_currentIndex);
    if (++m_currentIndex == m_points.count()) {
        clear();
        emit completed();
    }
}