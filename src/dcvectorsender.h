#ifndef DCVECTORSENDER_H
#define DCVECTORSENDER_H

#include "iserialio.h"
#include <QObject>
#include <QTimer>
#include <QList>
#include <QPointF>

/**
 * @brief Передает массив значений на двигатель через заданный интервал
 */
class DCVectorSender : public QObject
{
    Q_OBJECT
public:
    explicit DCVectorSender(ISerialIO *serial, QObject *parent = 0);

private:
    ISerialIO *m_serial;
    /**
     * @brief Массив значений
     */
    QList<QPointF> m_points;
    /**
     * @brief Таймер, управляющий передачей
     */
    QTimer m_timer;
    /**
     * @brief Счетчик переданных значений
     */
    uint32_t m_currentIndex;
    /**
     * @brief Флаг, устанавливаемый после передачи последнего значения
     *
     * @details Требуется для того, чтобы получить последний ответ от двигателя перед тем, как закончить сеанс передачи
     */
    bool m_skipNextAndStop;

signals:
    /**
     * @brief Сигнал о том, чтобы передано значение
     * @param index Номер значения
     * @param val Значение
     */
    void sended(int index, quint16 val);
    /**
     * @brief Начата передача данных
     */
    void started();
    /**
     * @brief Передача данных завершена
     */
    void completed();

public slots:
    /**
     * @brief Начать передачу данных
     * @param points Массив значений
     * @param interval Интервал между значениями
     */
    void start(QList<QPointF> points, int interval);
    /**
     * @brief Остановить передачу
     */
    void cancel();

private slots:
    /**
     * @brief Отправляет слудющее значение
     */
    void sendNext();

};

#endif // DCVECTORSENDER_H
