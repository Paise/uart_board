#ifndef RAWDATASENDWIDGET_H
#define RAWDATASENDWIDGET_H

#include "irunnablewidget.h"
#include <QPointer>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QAction>
#include <QList>

class ISerialIO;
namespace Ui {
class RawDataSendWidget;
}

/**
 * @brief Виджет для посимвольной отправки данных по последовательному порту
 *
 * @details Доступна передача и прием данных в ASCII, Hex и Decimal форматах.
 */
class RawDataSendWidget : public IRunnableWidget
{
    Q_OBJECT

public:
    explicit RawDataSendWidget(QWidget *parent = 0);
    ~RawDataSendWidget();

    void setSerialDevice(ISerialIO *serial);

private:
    bool eventFilter(QObject *watched, QEvent *event);
    /**
     * @brief Переводит данные в текст, соответствующий текущему формату задания символов
     * @param data Данные для перевода
     * @return
     */
    QString toCurrentEncoding(const QByteArray &data);

    Ui::RawDataSendWidget *ui;
    QPointer<ISerialIO> m_serial;
    QIntValidator *m_intValidator;
    QRegExpValidator *m_hexValidator;
    QList<QAction*> m_lastSended;

public slots:
    /**
     * @brief Выполняетмых заданий нет
     */
    void run();
    /**
     * @brief ***
     */
    void stop();

private slots:
    /**
     * @brief Отправляет в порт текст, введенный пользователем
     */
    void sendData();
    /**
     * @brief Если введен текст, раблокируется кнопка передачи
     * @param text Текст в окне ввода
     */
    void enableSendButton(const QString &text);
    /**
     * @brief Вывести на экран принятые в порт данные
     * @param data Принятые данные
     */
    void recieveData(const QByteArray &data);

    /**
     * @brief Позволяет вводить только данные в десятичном формате
     */
    void setIntValidator();
    /**
     * @brief Позволяет вводить только данные в шестнадцатеричном формате
     */
    void setHexValidator();
    /**
     * @brief Позволяет вводить только данные в ASCII формате
     */
    void setAsciiValidator();
};

#endif // RAWDATASENDWIDGET_H
