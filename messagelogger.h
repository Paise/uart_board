#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QObject>
#include <QPointer>
#include <QTextEdit>

class MessageLogger : public QObject
{
    Q_OBJECT
public:
    explicit MessageLogger(QObject *parent = 0);

    void setOutWidget(QTextEdit *edit);
    void log(const QByteArray &message);
    static MessageLogger* instance();

private:
    QPointer<QTextEdit> m_edit;
};

#endif // MESSAGELOGGER_H
