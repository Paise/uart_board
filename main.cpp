#include "mainwindow.h"
#include "messagelogger.h"
#include <QApplication>
#include <QtGlobal>

void customMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    MessageLogger::instance()->log(localMsg);
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

    w.setWindowTitle("Uart Board");
    w.show();

    return a.exec();
}
