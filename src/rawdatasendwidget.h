#ifndef RAWDATASENDWIDGET_H
#define RAWDATASENDWIDGET_H

#include <QWidget>
#include <QPointer>

class ISerialIO;
namespace Ui {
class RawDataSendWidget;
}

class RawDataSendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RawDataSendWidget(QWidget *parent = 0);
    ~RawDataSendWidget();

    void setSerialDevice(ISerialIO *serial);

private:
    Ui::RawDataSendWidget *ui;
    QPointer<ISerialIO> m_serial;

private slots:
    void sendData();
    void recieveData(const QByteArray &data);
};

#endif // RAWDATASENDWIDGET_H
