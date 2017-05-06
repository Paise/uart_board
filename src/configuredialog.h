#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>
#include <QMap>
#include <QTimer>
#include <QSerialPort>
#include <QAbstractButton>

namespace Ui {
class ConfigureDialog;
}

/**
 * @brief Диалог настройки последовательного порта
 */
class ConfigureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureDialog(QWidget *parent = 0);
    ~ConfigureDialog();

    /**
     * @brief Имя заданного порта
     * @return Имя порта
     */
    QString portName() const;
    /**
     * @brief Скорость работы порта
     * @return Скорость работы порта
     */
    QSerialPort::BaudRate baudRate() const;
    /**
     * @brief Количество бит даных
     * @return Количество бит даных
     */
    QSerialPort::DataBits dataBits() const;
    /**
     * @brief Бит паритета
     * @return Бит паритета
     */
    QSerialPort::Parity parity() const;
    /**
     * @brief Наличие стоп-бита
     * @return Наличие стоп-бита
     */
    QSerialPort::StopBits stopBits() const;
    /**
     * @brief Контроль потока
     * @return Контроль потока
     */
    QSerialPort::FlowControl flowControl() const;

private:
    /**
     * @brief Заполняет словари воможных значений параметров
     */
    void initAvailableValues();
    /**
     * @brief Блокирует кнопки диалога
     * @param enabled Раблокировать или разблокировать кнопки
     */
    void updateEnabledStates(bool enabled);

    Ui::ConfigureDialog *ui;
    QMap<QString, QSerialPort::BaudRate> m_baudRates;
    QMap<QString, QSerialPort::DataBits> m_dataBits;
    QMap<QString, QSerialPort::Parity> m_parities;
    QMap<QString, QSerialPort::StopBits> m_stopBits;
    QMap<QString, QSerialPort::FlowControl> m_flowControls;
    /**
     * @brief Таймер, проверяющий наличие доступных портов
     */
    QTimer m_portSearchTimer;

private slots:
    /**
     * @brief Променить настройки порта по-умолчанию
     */
    void initDefaults();
    /**
     * @brief Поиск доступных портов и обновление списка портов
     */
    void refreshPortsList();
    /**
     * @brief Обрабатывает нажатие диалогойвой кнопки
     * @param button Нажатая кнопка
     */
    void processDialogButtonClick(QAbstractButton *button);
};

#endif // CONFIGUREDIALOG_H
