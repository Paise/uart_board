#include "dccontrolwidget.h"
#include "ui_dccontrolwidget.h"

DCControlWidget::DCControlWidget(QWidget *parent) :
    IRunnableWidget(parent),
    ui(new Ui::DCControlWidget)
{
    ui->setupUi(this);

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

}

void DCControlWidget::stop()
{

}
