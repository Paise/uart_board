#ifndef RAWDATASENDWIDGET_H
#define RAWDATASENDWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QIntValidator>
#include <QRegExpValidator>

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
    QString toCurrentEncoding(const QByteArray &data);

    Ui::RawDataSendWidget *ui;
    QPointer<ISerialIO> m_serial;
    QIntValidator *m_intValidator;
    QRegExpValidator *m_hexValidator;

private slots:
    void sendData();
    void enableSendButton(const QString &text);
    void recieveData(const QByteArray &data);

    void setIntValidator();
    void setHexValidator();
    void setAsciiValidator();
};

#endif // RAWDATASENDWIDGET_H
