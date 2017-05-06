#ifndef SERIALPORT_HOLDER_H
#define SERIALPORT_HOLDER_H

#include "iserialio.h"
#include <QSerialPort>
#include <QPointer>

/**
 * @brief Реализация интерфейса ISerialIO для последовательного порта.
 *
 * @details Данный класс, кроме реализации интерфейса, также осуществляет логгирование всех
 * действий с последовательным портом.
 */
class SerialPortHolder : public ISerialIO
{
    Q_OBJECT
public:
    explicit SerialPortHolder(QSerialPort *serial, QObject *parent = 0);

    QSerialPort* serialPort();
    void writeAsync(const QByteArray &data);
    void writeAsyncByte(quint8 byte);
    void writeAsync2Bytes(quint16 bytes);

private:
    QPointer<QSerialPort> m_serialPort;

public slots:
    /**
     * @brief Подключиться к порту
     * @param name Имя порта
     */
    void connectPort(const QString &name = QString());
    /**
     * @brief Разорвать текущее соединие
     */
    void disconnectPort();

private slots:
    /**
     * @brief Отправить сигнал об отправленных данных
     * @param bytes
     */
    void confirmDataWritten(quint64 bytes);
    /**
     * @brief Отправить сигнал о принятых данных
     */
    void recieveData();
    /**
     * @brief Логгировать ошибку
     * @param error Код ошибки
     */
    void logError(QSerialPort::SerialPortError error);

signals:
    /**
     * @brief Соединение разорвано
     */
    void closed();
    /**
     * @brief Соединение установлено
     */
    void opened();
    /**
     * @brief Сигнал ошибки в последовательном порта
     * @param msg Описание ошибки
     * @param error Код ошибки
     */
    void errorOccured(const QString &msg, QSerialPort::SerialPortError error);

};

#endif // SERIALPORT_HOLDER_H
