#include "rawdatasendwidget.h"
#include "ui_rawdatasendwidget.h"
#include "iserialio.h"
#include "ext/QtAwesome/QtAwesome.h"
#include <QDebug>
#include <QTextCursor>
#include <QScrollBar>
#include <QMenu>

#define MAX_LAST_BYTES (10)

//TODO: scroll to new
RawDataSendWidget::RawDataSendWidget(QWidget *parent) :
    IRunnableWidget(parent),
    ui(new Ui::RawDataSendWidget),
    m_intValidator(new QIntValidator(0, 0xFF, this)),
    m_hexValidator(new QRegExpValidator(QRegExp("(\\d|[a-f]){1,2}"), this))
{
    ui->setupUi(this);
    ui->writeEdit->setReadOnly(true);
    ui->readEdit->setReadOnly(true);
    ui->clearWriteButton->setIcon(QtAwesome::instance()->icon(fa::trasho));
    ui->clearReadButton->setIcon(QtAwesome::instance()->icon(fa::trasho));
    ui->toSendLineEdit->installEventFilter(this);

    connect(ui->sendButton, &QPushButton::clicked, this, &RawDataSendWidget::sendData);
    connect(ui->toSendLineEdit, &QLineEdit::returnPressed, ui->sendButton, &QPushButton::click);
    connect(ui->toSendLineEdit, &QLineEdit::textChanged, this, &RawDataSendWidget::enableSendButton);
    connect(ui->clearWriteButton, &QPushButton::clicked, ui->writeEdit, &QPlainTextEdit::clear);
    connect(ui->clearReadButton, &QPushButton::clicked, ui->readEdit, &QPlainTextEdit::clear);
    connect(ui->asciiRadio, &QRadioButton::clicked, this, &RawDataSendWidget::setAsciiValidator);
    connect(ui->decRadio, &QRadioButton::clicked, this, &RawDataSendWidget::setIntValidator);
    connect(ui->hexRadio, &QRadioButton::clicked, this, &RawDataSendWidget::setHexValidator);
    ui->hexRadio->setChecked(true);
    setHexValidator();
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

bool RawDataSendWidget::eventFilter(QObject *watched, QEvent *e)
{
    if (watched == ui->toSendLineEdit) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *ke = dynamic_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Up) {
                QMenu menu;
                QPoint pos = ui->toSendLineEdit->rect().topLeft();
                pos = ui->toSendLineEdit->mapToGlobal(pos);
                menu.addActions(m_lastSended);
                QAction *ret = menu.exec(pos);
                if (ret) {
                    ui->toSendLineEdit->setText(ret->text());
                    sendData();
                    e->accept();
                    return true;
                }
            }
        }
    }

    return QWidget::eventFilter(this, e);
}

QString RawDataSendWidget::toCurrentEncoding(const QByteArray &data)
{
    QString text;

    if (ui->asciiRadio->isChecked()) {
        text = QString("|ascii\t|") + QString::fromUtf8(data);
    } else if (ui->decRadio->isChecked()) {
        QString hex(data.toHex());
        bool ok;
        text = QString("|dec\t|%0").arg(hex.toInt(&ok, 16));
    } else {
        text = QString("|0x\t|") + QString(data.toHex());
    }

    return text;
}

void RawDataSendWidget::run()
{
    emit completed();
}

void RawDataSendWidget::stop()
{
    // Do nothing
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
    if (ui->asciiRadio->isChecked()) {
        m_serial->writeAsync(text.toUtf8());
    } else if (ui->decRadio->isChecked()) {
        m_serial->writeAsyncByte(text.toInt());
    } else {
        bool ok;
        m_serial->writeAsyncByte(text.toInt(&ok, 16));
    }
    ui->writeEdit->insertPlainText(text.append('\n'));
    m_lastSended.prepend(new QAction(text));
    if (m_lastSended.size() > MAX_LAST_BYTES)
        delete m_lastSended.takeLast();
}

void RawDataSendWidget::enableSendButton(const QString &text)
{
    ui->sendButton->setEnabled(!text.isEmpty());
}

void RawDataSendWidget::recieveData(const QByteArray &data)
{
    ui->readEdit->appendPlainText(toCurrentEncoding(data));
}

void RawDataSendWidget::setIntValidator()
{
    ui->toSendLineEdit->setValidator(m_intValidator);
    ui->toSendLineEdit->setPlaceholderText("0-255");
    qDebug() << tr("Switched to decimal");
    m_lastSended.clear();
}

void RawDataSendWidget::setHexValidator()
{
    ui->toSendLineEdit->setValidator(m_hexValidator);
    ui->toSendLineEdit->setPlaceholderText("0-FF");
    qDebug() << tr("Switched to hex");
    m_lastSended.clear();
}

void RawDataSendWidget::setAsciiValidator()
{
    ui->toSendLineEdit->setValidator(NULL);
    ui->toSendLineEdit->setPlaceholderText("a-z");
    qDebug() << tr("Switched to ascii");
    m_lastSended.clear();
}
