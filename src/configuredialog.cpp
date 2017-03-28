#include "configuredialog.h"
#include "ui_configuredialog.h"
#include <QSerialPortInfo>
#include <QStringList>
#include <QVariant>

#define PORT_SEARCH_PERIOD (1000)

ConfigureDialog::ConfigureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureDialog),
    m_portSearchTimer(this)
{
    ui->setupUi(this);
    m_portSearchTimer.setInterval(PORT_SEARCH_PERIOD);

    initAvailableValues();
    initDefaults();
    refreshPortsList();

    connect(&m_portSearchTimer, &QTimer::timeout, this, &ConfigureDialog::refreshPortsList);
    connect(ui->refreshButton, &QPushButton::clicked, this, &ConfigureDialog::refreshPortsList);
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &ConfigureDialog::processDialogButtonClick);
}

ConfigureDialog::~ConfigureDialog()
{
    delete ui;
}

QString ConfigureDialog::portName() const
{
    return ui->portsBox->currentText();
}

QSerialPort::BaudRate ConfigureDialog::baudRate() const
{
    return m_baudRates.value( ui->baudRateBox->currentText() );
}

QSerialPort::DataBits ConfigureDialog::dataBits() const
{
    return m_dataBits.value( ui->dataBitsBox->currentText() );
}

QSerialPort::Parity ConfigureDialog::parity() const
{
    return m_parities.value( ui->parityBox->currentText() );
}

QSerialPort::StopBits ConfigureDialog::stopBits() const
{
    return m_stopBits.value( ui->stopBitsBox->currentText() );
}

QSerialPort::FlowControl ConfigureDialog::flowControl() const
{
    return m_flowControls.value( ui->flowControlBox->currentText() );
}

void ConfigureDialog::initAvailableValues()
{
    QStringList baudRateNames;
    QStringList dataBitNames;
    QStringList parityNames;
    QStringList stopBitNames;
    QStringList flowControlNames;

    baudRateNames << "1200" << "2400" << "4800" << "9600" << "19200" << "38400" << "57600" << "115200";
    dataBitNames << "5" << "6" << "7" << "8";
    parityNames << tr("None" ) << tr("Even") << tr("Odd") << tr("Space") << tr("Mark");
    stopBitNames << "1" << "1.5" << "2";
    flowControlNames << tr("None") << tr("Hardware") << tr("Software");

    m_baudRates.insert( baudRateNames[0], QSerialPort::Baud1200 );
    m_baudRates.insert( baudRateNames[1], QSerialPort::Baud2400 );
    m_baudRates.insert( baudRateNames[2], QSerialPort::Baud4800 );
    m_baudRates.insert( baudRateNames[3], QSerialPort::Baud9600 );
    m_baudRates.insert( baudRateNames[4], QSerialPort::Baud19200 );
    m_baudRates.insert( baudRateNames[5], QSerialPort::Baud38400 );
    m_baudRates.insert( baudRateNames[6], QSerialPort::Baud57600 );
    m_baudRates.insert( baudRateNames[7], QSerialPort::Baud115200 );

    m_dataBits.insert( dataBitNames[0], QSerialPort::Data5 );
    m_dataBits.insert( dataBitNames[1], QSerialPort::Data6 );
    m_dataBits.insert( dataBitNames[2], QSerialPort::Data7 );
    m_dataBits.insert( dataBitNames[3], QSerialPort::Data8 );

    m_parities.insert( parityNames[0], QSerialPort::NoParity );
    m_parities.insert( parityNames[1], QSerialPort::EvenParity );
    m_parities.insert( parityNames[2], QSerialPort::OddParity );
    m_parities.insert( parityNames[3], QSerialPort::SpaceParity );
    m_parities.insert( parityNames[4], QSerialPort::MarkParity );

    m_stopBits.insert( stopBitNames[0], QSerialPort::OneStop );
    m_stopBits.insert( stopBitNames[1], QSerialPort::OneAndHalfStop );
    m_stopBits.insert( stopBitNames[2], QSerialPort::TwoStop );

    m_flowControls.insert( flowControlNames[0], QSerialPort::NoFlowControl );
    m_flowControls.insert( flowControlNames[1], QSerialPort::HardwareControl );
    m_flowControls.insert( flowControlNames[2], QSerialPort::SoftwareControl );

    ui->baudRateBox->addItems( baudRateNames );
    ui->dataBitsBox->addItems( dataBitNames );
    ui->parityBox->addItems( parityNames );
    ui->stopBitsBox->addItems( stopBitNames );
    ui->flowControlBox->addItems( flowControlNames );
}

void ConfigureDialog::updateEnabledStates(bool enabled)
{
    if (enabled) {
        m_portSearchTimer.stop();
    } else {
        m_portSearchTimer.start();
    }
    ui->settingsWidget->setEnabled(enabled);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
    ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(enabled);
}

void ConfigureDialog::initDefaults()
{
    ui->baudRateBox->setCurrentIndex(3);
    ui->dataBitsBox->setCurrentIndex(3);
    ui->parityBox->setCurrentIndex(0);
    ui->stopBitsBox->setCurrentIndex(1);
    ui->flowControlBox->setCurrentIndex(0);
}

void ConfigureDialog::refreshPortsList()
{
    ui->portsBox->clear();
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        if (info.isBusy()) {
            continue;
        }
        ui->portsBox->addItem(info.portName());
    }
    updateEnabledStates(ui->portsBox->count());
}

void ConfigureDialog::processDialogButtonClick(QAbstractButton *button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        initDefaults();
    }
}
