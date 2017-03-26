#include "rawdatasendwidget.h"
#include "ui_rawdatasendwidget.h"
#include "iserialio.h"
#include <QDebug>

RawDataSendWidget::RawDataSendWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawDataSendWidget)
{
    ui->setupUi(this);
    ui->asciiRadio->setChecked(true);
    ui->readEdit->setReadOnly(true);

    connect(ui->sendButton, &QPushButton::clicked, this, &RawDataSendWidget::sendData);
}

RawDataSendWidget::~RawDataSendWidget()
{
    delete ui;
}

void RawDataSendWidget::setSerialDevice(ISerialIO *serial)
{
    if (!m_serial.isNull()) {
        disconnect(m_serial.data(), &ISerialIO::dataRecieved, this, &RawDataSendWidget::recieveData);
    }
    m_serial = serial;
    connect(serial, &ISerialIO::dataRecieved, this, &RawDataSendWidget::recieveData);
}

void RawDataSendWidget::sendData()
{
    if (m_serial.isNull()) {
        qDebug() << tr("Cannot send data (no serialport instance provided)");
        return;
    }
    m_serial->writeAsync(ui->writeEdit->toPlainText().toUtf8());
}

void RawDataSendWidget::recieveData(const QByteArray &data)
{
    ui->readEdit->appendPlainText(QString(data));
}
