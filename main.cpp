#include "mainwindow.h"
#include "messagelogger.h"
#include <QApplication>
#include <QDateTime>
#include <QtGlobal>

void customMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString date = QDateTime::currentDateTime().toString("hh:mm:ss::zzz");

    MessageLogger::instance()->log(QString("%0: %1").arg(date, msg));
    switch(type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), ctx.file, ctx.line, ctx.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), ctx.file, ctx.line, ctx.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), ctx.file, ctx.line, ctx.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), ctx.file, ctx.line, ctx.function);
        abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);
    MainWindow w;

    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);
    w.setWindowTitle(APPLICATION_NAME);
    w.show();

    return a.exec();
}
