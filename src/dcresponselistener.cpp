#include "dcresponselistener.h"
#include "iserialio.h"

DCResponseListener::DCResponseListener(const ISerialIO *serial, QObject *parent) :
    QObject(parent),
    m_serial(serial),
    m_currentIndex(0),
    m_listening(false)
{

}

void DCResponseListener::recieveData(const QByteArray &array)
{
    if (!m_listening) return;

    m_buffer.append(array);
    while (m_buffer.count() >= 1) {
        quint16 val = 0;
        val |= ((quint8) m_buffer.at(0)) << 8;
        m_buffer.remove(0,1);
        emit recieved(m_currentIndex++, val);
    }
}

void DCResponseListener::listen()
{
    if (m_listening) return;

    m_buffer.clear();
    m_currentIndex = 0;
    m_listening = true;
    connect(m_serial, &ISerialIO::dataRecieved, this, &DCResponseListener::recieveData);
}

void DCResponseListener::cancel()
{
    if (!m_listening) return;

    m_buffer.clear();
    m_currentIndex = 0;
    m_listening = false;
    disconnect(m_serial, &ISerialIO::dataRecieved, this, &DCResponseListener::recieveData);
}
