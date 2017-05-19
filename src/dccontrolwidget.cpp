#include "dccontrolwidget.h"
#include "ui_dccontrolwidget.h"
#include "iserialio.h"

DCControlWidget::DCControlWidget(QWidget *parent) :
    IRunnableWidget(parent),
    ui(new Ui::DCControlWidget)
{
    ui->setupUi(this);

    ui->spinBoxKp->setRange(0.0, 2.55);
    ui->spinBoxKp->setSingleStep(0.01);
    ui->spinBoxKp->setValue(0.50);
    ui->spinBoxKi->setRange(0.0, 2.55);
    ui->spinBoxKi->setSingleStep(0.01);
    ui->spinBoxKi->setValue(0.10);
    ui->spinBoxKd->setRange(0.0, 2.55);
    ui->spinBoxKd->setSingleStep(0.01);
    ui->spinBoxKd->setValue(0.10);

    ui->radio4->setChecked(true);

    connect(ui->clearScreenButton, &QPushButton::clicked, this, &DCControlWidget::clearScreen);
    connect(ui->sendSettingsButton, &QPushButton::clicked, this, &DCControlWidget::sendSettings);
}

DCControlWidget::~DCControlWidget()
{
    delete ui;
}

void DCControlWidget::setSerialDevice(ISerialIO *serial)
{
    m_serial = serial;
}

void DCControlWidget::run()
{
    connect(ui->plusButton, &QPushButton::clicked, this, &DCControlWidget::increaseSpeed);
    connect(ui->minusButton, &QPushButton::clicked, this, &DCControlWidget::decreaseSpeed);
    connect(ui->spinBoxKp, SIGNAL(valueChanged(double)), this, SLOT(sendKp(double)));
    connect(ui->spinBoxKd, SIGNAL(valueChanged(double)), this, SLOT(sendKd(double)));
    connect(ui->spinBoxKi, SIGNAL(valueChanged(double)), this, SLOT(sendKi(double)));
    connect(ui->radio4, &QRadioButton::clicked, this, &DCControlWidget::setTiming4);
    connect(ui->radio8, &QRadioButton::clicked, this, &DCControlWidget::setTiming8);
    connect(ui->radio16, &QRadioButton::clicked, this, &DCControlWidget::setTiming16);
    connect(ui->radio32, &QRadioButton::clicked, this, &DCControlWidget::setTiming32);
}

void DCControlWidget::stop()
{
    disconnect(ui->plusButton, &QPushButton::clicked, this, &DCControlWidget::increaseSpeed);
    disconnect(ui->minusButton, &QPushButton::clicked, this, &DCControlWidget::decreaseSpeed);
    disconnect(ui->spinBoxKp, SIGNAL(valueChanged(double)), this, SLOT(sendKp(double)));
    disconnect(ui->spinBoxKd, SIGNAL(valueChanged(double)), this, SLOT(sendKd(double)));
    disconnect(ui->spinBoxKi, SIGNAL(valueChanged(double)), this, SLOT(sendKi(double)));
    disconnect(ui->radio4, &QRadioButton::clicked, this, &DCControlWidget::setTiming4);
    disconnect(ui->radio8, &QRadioButton::clicked, this, &DCControlWidget::setTiming8);
    disconnect(ui->radio16, &QRadioButton::clicked, this, &DCControlWidget::setTiming16);
    disconnect(ui->radio32, &QRadioButton::clicked, this, &DCControlWidget::setTiming32);
}

void DCControlWidget::clearScreen(){
    m_serial->writeAsyncByte(COMMAND_CLEAR_SCREEN);
    return;
}

void DCControlWidget::increaseSpeed(){
    m_serial->writeAsyncByte(COMMAND_INC_SPEED);
    return;
}

void DCControlWidget::decreaseSpeed(){
    m_serial->writeAsyncByte(COMMAND_DEC_SPEED);
    return;
}

void DCControlWidget::sendKp(double d){
    m_serial->writeAsyncByte(COMMAND_SET_PID_P);
    m_serial->writeAsyncByte((quint8)(d*100));
    return;
}

void DCControlWidget::sendKd(double d){
    m_serial->writeAsyncByte(COMMAND_SET_PID_D);
    m_serial->writeAsyncByte((quint8)(d*100));
    return;
}

void DCControlWidget::sendKi(double d){
    m_serial->writeAsyncByte(COMMAND_SET_PID_I);
    m_serial->writeAsyncByte((quint8)(d*100));
    return;
}

void DCControlWidget::setTiming4(){
    m_serial->writeAsyncByte(COMMAND_SET_TIMING_4);
    return;
}

void DCControlWidget::setTiming8(){
    m_serial->writeAsyncByte(COMMAND_SET_TIMING_8);
    return;
}

void DCControlWidget::setTiming16(){
    m_serial->writeAsyncByte(COMMAND_SET_TIMING_16);
    return;
}

void DCControlWidget::setTiming32(){
    m_serial->writeAsyncByte(COMMAND_SET_TIMING_32);
    return;
}

void DCControlWidget::sendSettings(){
    sendKp(ui->spinBoxKp->value());
    sendKi(ui->spinBoxKi->value());
    sendKd(ui->spinBoxKd->value());
    if (ui->radio4->isChecked()) setTiming4();
    else if (ui->radio8->isChecked()) setTiming8();
    else if (ui->radio16->isChecked()) setTiming16();
    else if (ui->radio32->isChecked()) setTiming32();

    return;
}

