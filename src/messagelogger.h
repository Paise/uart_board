#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QObject>
#include <QPointer>
#include <QTextEdit>

/**
 * @brief Класс, управляющий отображением логов на QTextEdit
 */
class MessageLogger : public QObject
{
    Q_OBJECT
public:
    explicit MessageLogger(QObject *parent = 0);

    /**
     * @brief Задать виджет, отображающий логи
     * @param edit Виджет логов
     */
    void setOutWidget(QTextEdit *edit);
    /**
     * @brief Логгировать сообщение
     * @param message Сообщение
     */
    void log(const QString &message);
    static MessageLogger* instance();

private:
    QPointer<QTextEdit> m_edit;
};

#endif // MESSAGELOGGER_H
