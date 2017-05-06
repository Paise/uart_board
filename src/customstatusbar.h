#ifndef CUSTOMSTATUSBAR_H
#define CUSTOMSTATUSBAR_H

#include <QStatusBar>
#include <QLabel>

/**
 * @brief Реализация класса QStatusBar с добавленными виждетами состояния последовательного порта
 */
class CustomStatusBar : public QStatusBar
{
    Q_OBJECT
    Q_PROPERTY(QColor statusColor READ statusColor WRITE setStatusColor)

public:
    explicit CustomStatusBar(QWidget *parent = 0);

    /**
     * @brief Текущее сообщение
     * @return Текст сообщения
     */
    QString permanentMessage() const;
    /**
     * @brief Цвет индикатора состояний порта
     * @return Цвет индикатора
     */
    QColor statusColor() const;
    /**
     * @brief Задать текущее сообщение
     * @param text Текст сообщения
     */
    void setPermanentMessage(const QString &text);
    /**
     * @brief Задать цвет индикатора состояния порта
     * @param color Цвет индикатора
     */
    void setStatusColor(const QColor &color);

private:
    /**
     * @brief Цыет индикатора последовательного порта
     */
    QColor m_color;
    /**
     * @brief Текущее сообщение
     */
    QLabel m_permanentMessage;
    /**
     * @brief Виджет, содержащий индикатор состояния порта
     */
    QLabel m_statusLabel;
};

#endif // CUSTOMSTATUSBAR_H
