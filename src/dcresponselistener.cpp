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
    while (m_buffer.count() > 2) {
        quint16 val;
        val = (quint16) m_buffer.at(0);
        val |= ((quint16) m_buffer.at(1)) << 8;
        m_buffer.remove(0,2);
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
