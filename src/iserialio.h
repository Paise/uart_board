#ifndef ABSTRACTSERIALIO_H
#define ABSTRACTSERIALIO_H

#include <QObject>

#define COMMAND_CLEAR_SCREEN (0x80)
#define COMMAND_INC_SPEED (0x81)
#define COMMAND_DEC_SPEED (0x82)
#define COMMAND_INC_TIMING (0x83)
#define COMMAND_DEC_TIMING (0x84)
#define COMMAND_SET_SETPOINT (0xD0)
#define COMMAND_SET_TIMING (0xD1)
#define COMMAND_SET_PID_P (0xD2)
#define COMMAND_SET_PID_I (0xD3)
#define COMMAND_SET_PID_D (0xD4)

/**
 * @brief Интерфейс для передачи и приема данных по последовательному порту без доступа к его настройкам
 */
class ISerialIO : public QObject
{
    Q_OBJECT

public:
    explicit ISerialIO(QObject *parent) : QObject(parent) {}
    virtual ~ISerialIO() {}

    /**
     * @brief Передать произвольный набор данных
     * @param data Данные
     */
    virtual void writeAsync(const QByteArray &data) =0;
    /**
     * @brief Передать байт данных
     * @param byte Байт
     */
    virtual void writeAsyncByte(quint8 byte) =0;
    /**
     * @brief Передать два байта данных
     * @param bytes 2 Байта
     */
    virtual void writeAsync2Bytes(quint16 bytes) =0;

signals:
    /**
     * @brief В буффере приема появились данные
     * @param data Принятые данные
     */
    void dataRecieved(const QByteArray &data);
    /**
     * @brief Данные переданы
     * @param bytes Переданные данные
     */
    void dataWritten(qint64 bytes);

};

#endif // ABSTRACTSERIALIO_H
