#include "rawdatasendwidget.h"
#include "ui_rawdatasendwidget.h"
#include "iserialio.h"
#include "ext/QtAwesome/QtAwesome.h"
#include <QDebug>
#include <QTextCursor>

RawDataSendWidget::RawDataSendWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawDataSendWidget)
{
    ui->setupUi(this);
    ui->asciiRadio->setChecked(true);
    ui->writeEdit->setReadOnly(true);
    ui->readEdit->setReadOnly(true);
    ui->clearWriteButton->setIcon(QtAwesome::instance()->icon(fa::trasho));
    ui->clearReadButton->setIcon(QtAwesome::instance()->icon(fa::trasho));

    connect(ui->sendButton, &QPushButton::clicked, this, &RawDataSendWidget::sendData);
    connect(ui->toSendLineEdit, &QLineEdit::returnPressed, ui->sendButton, &QPushButton::click);
    connect(ui->toSendLineEdit, &QLineEdit::textChanged, this, &RawDataSendWidget::enableSendButton);
    connect(ui->clearWriteButton, &QPushButton::clicked, ui->writeEdit, &QPlainTextEdit::clear);
    connect(ui->clearReadButton, &QPushButton::clicked, ui->readEdit, &QPlainTextEdit::clear);
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
    QString text = ui->toSendLineEdit->text();
    ui->toSendLineEdit->clear();
    if (text.isEmpty()) {
        return;
    }
    m_serial->writeAsync(text.toUtf8());
    ui->writeEdit->insertPlainText(text.append('\n'));
}

void RawDataSendWidget::enableSendButton(const QString &text)
{
    ui->sendButton->setEnabled(!text.isEmpty());
}

void RawDataSendWidget::recieveData(const QByteArray &data)
{
    ui->readEdit->appendPlainText(QString(data));
}
