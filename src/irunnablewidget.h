#ifndef IRUNNABLEWIDGET_H
#define IRUNNABLEWIDGET_H

#include <QWidget>

class IRunnableWidget : public QWidget
{
    Q_OBJECT

public:
    IRunnableWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~IRunnableWidget() {}

public slots:
    virtual void run() =0;
    virtual void stop() =0;

};

#endif // IRUNNABLEWIDGET_H
