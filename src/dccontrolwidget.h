#ifndef DCCONTROLWIDGET_H
#define DCCONTROLWIDGET_H

#include "irunnablewidget.h"

namespace Ui {
class DCControlWidget;
}

class DCControlWidget : public IRunnableWidget
{
    Q_OBJECT

public:
    explicit DCControlWidget(QWidget *parent = 0);
    ~DCControlWidget();

    void setSerialDevice(ISerialIO *serial);

private:
    Ui::DCControlWidget *ui;
    ISerialIO *m_serial;


public slots:
    void run();
    void stop();
    void clearScreen();
    void increaseSpeed();
    void decreaseSpeed();
    void sendKp(double d);
    void sendKi(double d);
    void sendKd(double d);
    void setTiming4();
    void setTiming8();
    void setTiming16();
    void setTiming32();
    void sendSettings();
};

#endif // DCCONTROLWIDGET_H
