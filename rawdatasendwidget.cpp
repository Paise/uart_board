#include "rawdatasendwidget.h"
#include "ui_rawdatasendwidget.h"

RawDataSendWidget::RawDataSendWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawDataSendWidget)
{
    ui->setupUi(this);
    ui->asciiRadio->setChecked(true);
    ui->readEdit->setReadOnly(true);
}

RawDataSendWidget::~RawDataSendWidget()
{
    delete ui;
}
