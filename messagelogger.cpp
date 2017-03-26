#include "messagelogger.h"

static MessageLogger *logger = NULL;

MessageLogger::MessageLogger(QObject *parent) :
    QObject(parent)
{
}

void MessageLogger::setOutWidget(QTextEdit *edit)
{
    m_edit = edit;
}

void MessageLogger::log(const QByteArray &message)
{
    if (m_edit.isNull()) {
        return;
    }
    m_edit->append(message);
}

MessageLogger *MessageLogger::instance()
{
    if (!logger) {
        logger = new MessageLogger();
    }
    return logger;
}
