#ifndef IRUNNABLEWIDGET_H
#define IRUNNABLEWIDGET_H

#include <QWidget>

class ISerialIO;

class IRunnableWidget : public QWidget
{
    Q_OBJECT

public:
    IRunnableWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~IRunnableWidget() {}

    virtual void setSerialDevice(ISerialIO *serial) =0;

public slots:
    virtual void run() =0;
    virtual void stop() =0;

signals:
    void completed();

};

#endif // IRUNNABLEWIDGET_H
