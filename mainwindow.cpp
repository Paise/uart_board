#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include "configuredialog.h"
#include "customstatusbar.h"
#include "ext/QtAwesome/QtAwesome.h"
#include <QDebug>
#include <QState>
#include <QStyle>

#define SEND_PERIOD (1000)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serial(this),
    m_sendTimer(this),
    m_stateMachine(this)
{
    ui->setupUi(this);
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->mainToolBar->addAction( ui->actionStart );
    ui->mainToolBar->addAction( ui->actionStop );
    ui->mainToolBar->addAction( ui->actionDisconnect );
    m_sendTimer.setInterval(SEND_PERIOD);

    initIcons();
    initStateMachine();

    connect(&m_sendTimer, &QTimer::timeout, this, &MainWindow::sendData);
    connect(ui->actionConfigure_Port, &QAction::triggered, this, &MainWindow::configurePort);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
}

MainWindow::~MainWindow()
{
    disconnectPort();
    delete ui;
}

void MainWindow::initIcons()
{
    ui->actionConfigure_Port->setIcon(QtAwesome::instance()->icon(fa::cog));
    ui->actionDisconnect->setIcon(QtAwesome::instance()->icon(fa::unlink));
    ui->actionLoad->setIcon(QtAwesome::instance()->icon(fa::folderopeno));
    ui->actionSave->setIcon(QtAwesome::instance()->icon(fa::save));
    QVariantMap playOptions;
    playOptions["color"] = QColor(Qt::green);
    playOptions["color-active"] = QColor(Qt::green);
    playOptions["color-selected"] = QColor(Qt::green);
    ui->actionStart->setIcon(QtAwesome::instance()->icon(fa::playcircleo, playOptions));
    QVariantMap stopOptions;
    stopOptions["color"] = QColor(Qt::red);
    playOptions["color-active"] = QColor(Qt::red);
    playOptions["color-selected"] = QColor(Qt::red);
    ui->actionStop->setIcon(QtAwesome::instance()->icon(fa::stopcircleo, stopOptions));
}

void MainWindow::initStateMachine()
{
    QState *stateDisconnected = new QState;
    QState *stateConnected = new QState;
    QState *stateRunnig = new QState;

    stateDisconnected->assignProperty(ui->actionStart, "enabled", false);
    stateDisconnected->assignProperty(ui->actionStop, "enabled", false);
    stateDisconnected->assignProperty(ui->actionDisconnect, "enabled", false);
    stateDisconnected->assignProperty(ui->actionConfigure_Port, "enabled", true);
    stateDisconnected->assignProperty(ui->statusBar, "statusColor", QColor(Qt::red));

    stateConnected->assignProperty(ui->actionStart, "enabled", true);
    stateConnected->assignProperty(ui->actionStop, "enabled", false);
    stateConnected->assignProperty(ui->actionDisconnect, "enabled", true);
    stateConnected->assignProperty(ui->actionConfigure_Port, "enabled", true);
    stateConnected->assignProperty(ui->statusBar, "statusColor", QColor(Qt::yellow));

    stateRunnig->assignProperty(ui->actionStart, "enabled", false);
    stateRunnig->assignProperty(ui->actionStop, "enabled", true);
    stateRunnig->assignProperty(ui->actionDisconnect, "enabled", false);
    stateRunnig->assignProperty(ui->actionConfigure_Port, "enabled", false);
    stateRunnig->assignProperty(ui->statusBar, "statusColor", QColor(Qt::green));

    stateDisconnected->addTransition(this, SIGNAL(portOpened()), stateConnected);
    stateConnected->addTransition(ui->actionStart, SIGNAL(triggered()), stateRunnig);
    stateConnected->addTransition(ui->actionDisconnect, SIGNAL(triggered()), stateDisconnected);
    stateConnected->addTransition(&m_serial, SIGNAL(aboutToClose()), stateDisconnected);
    stateConnected->addTransition(&m_serial, SIGNAL(error(QSerialPort::SerialPortError)), stateDisconnected);
    stateRunnig->addTransition(ui->actionStop, SIGNAL(triggered()), stateConnected);
    stateRunnig->addTransition(&m_serial, SIGNAL(aboutToClose()), stateDisconnected);
    stateRunnig->addTransition(&m_serial, SIGNAL(error(QSerialPort::SerialPortError)), stateDisconnected);

    connect(stateDisconnected, &QState::entered, this, &MainWindow::processDisconnectState);
    connect(stateConnected, &QState::entered, this, &MainWindow::processConnectState);
    connect(stateRunnig, &QState::entered, this, &MainWindow::processRunningState);
    connect(stateRunnig, &QState::exited, this, &MainWindow::processStop);

    m_stateMachine.addState(stateDisconnected);
    m_stateMachine.addState(stateConnected);
    m_stateMachine.addState(stateRunnig);
    //m_stateMachine.setErrorState();
    m_stateMachine.setInitialState(stateDisconnected);
    m_stateMachine.start();
}

void MainWindow::about()
{
    QDialog aboutDialog;
    Ui::AboutWidget ui;
    ui.setupUi(&aboutDialog);
    aboutDialog.exec();
}

void MainWindow::sendData()
{
    m_serial.write("hell", 4);
}

void MainWindow::configurePort()
{
    ConfigureDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        disconnectPort();
        m_serial.setPortName( dialog.portName() );
        m_serial.setBaudRate( dialog.baudRate() );
        m_serial.setDataBits( dialog.dataBits() );
        m_serial.setParity( dialog.parity() );
        m_serial.setStopBits( dialog.stopBits() );
        m_serial.setFlowControl( dialog.flowControl() );
        connectPort();
    }
}

void MainWindow::connectPort()
{
    if (m_serial.open(QIODevice::ReadWrite)) {
        emit portOpened();
    }
}

void MainWindow::disconnectPort()
{
    m_serial.close();
}

void MainWindow::processDisconnectState()
{
    disconnectPort();
    ui->statusBar->setPermanentMessage(tr("Disconected"));
}

void MainWindow::processConnectState()
{
    ui->statusBar->setPermanentMessage(tr("Connected to %0").arg(m_serial.portName()));
}

void MainWindow::processRunningState()
{
    m_sendTimer.start();
    ui->statusBar->setPermanentMessage(tr("Running on %0").arg(m_serial.portName()));
}

void MainWindow::processStop()
{
    m_sendTimer.stop();
}
