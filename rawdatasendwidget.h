#ifndef RAWDATASENDWIDGET_H
#define RAWDATASENDWIDGET_H

#include <QWidget>

namespace Ui {
class RawDataSendWidget;
}

class RawDataSendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RawDataSendWidget(QWidget *parent = 0);
    ~RawDataSendWidget();

private:
    Ui::RawDataSendWidget *ui;
};

#endif // RAWDATASENDWIDGET_H
