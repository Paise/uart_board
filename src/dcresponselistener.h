#ifndef DCRESPONSELISTENER_H
#define DCRESPONSELISTENER_H

#include <QObject>

class ISerialIO;

class DCResponseListener : public QObject
{
    Q_OBJECT
public:
    explicit DCResponseListener(const ISerialIO *serial, QObject *parent = 0);

private:
    const ISerialIO *m_serial;
    QByteArray m_buffer;
    int m_currentIndex;
    bool m_listening;

signals:
    void recieved(int index, quint16 value);

private slots:
    void recieveData(const QByteArray &array);

public slots:
    void listen();
    void cancel();

};

#endif // DCRESPONSELISTENER_H
