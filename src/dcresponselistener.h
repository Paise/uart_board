#ifndef DCRESPONSELISTENER_H
#define DCRESPONSELISTENER_H

#include <QObject>

class ISerialIO;

/**
 * @brief Принимает данные от двигателя в определенном формате и передает сообщение о полученных данных
 */
class DCResponseListener : public QObject
{
    Q_OBJECT
public:
    explicit DCResponseListener(const ISerialIO *serial, QObject *parent = 0);

private:
    const ISerialIO *m_serial;
    /**
     * @brief Буффер, содержащий принятые данные, которые еще не были переданы сигналом recieved
     *
     * @details Данные передаются двигателем в виде двухбайтовых значений. Если было получено нечетное количество байт, то последный байт нужно
     * сохранить до следующего приема.
     */
    QByteArray m_buffer;
    /**
     * @brief Счетчик принятых байтов
     */
    int m_currentIndex;
    /**
     * @brief Текущее состояние
     */
    bool m_listening;

signals:
    /**
     * @brief Сигнал о принятом от двигателя значении
     * @param index Номер значения
     * @param value Значение
     */
    void recieved(int index, quint16 value);

private slots:
    /**
     * @brief Обработчик, вызываемый при получении сигнала о доступных в порте данных
     * @param array Принятые данные
     */
    void recieveData(const QByteArray &array);

public slots:
    /**
     * @brief Запуск прослушивания порта
     */
    void listen();
    /**
     * @brief Остановка прослушивания порта
     */
    void cancel();

};

#endif // DCRESPONSELISTENER_H
