#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include "messagelogger.h"
#include "configuredialog.h"
#include "customstatusbar.h"
#include "ext/QtAwesome/QtAwesome.h"
#include <QDebug>
#include <QState>
#include <QStyle>
#include <QFileDialog>
#include <QStandardPaths>

#define SEND_PERIOD (1000)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serial(this),
    m_sendTimer(this),
    m_stateMachine(this)
{
    ui->setupUi(this);
    ui->actionShow_Logs->setCheckable(true);
    ui->actionShow_Logs->setChecked(true);
    ui->logsEdit->setReadOnly(true);
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->mainToolBar->addAction( ui->actionStart );
    ui->mainToolBar->addAction( ui->actionStop );
    ui->mainToolBar->addAction( ui->actionDisconnect );
    m_sendTimer.setInterval(SEND_PERIOD);

    MessageLogger::instance()->setOutWidget(ui->logsEdit);
    initIcons();
    initStateMachine();

    connect(&m_serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(logPortError(QSerialPort::SerialPortError)));
    connect(&m_sendTimer, &QTimer::timeout, this, &MainWindow::sendData);
    connect(ui->actionConfigure_Port, &QAction::triggered, this, &MainWindow::configurePort);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionShow_Logs, &QAction::triggered, ui->logsDockWidget, &QDockWidget::setVisible);
    connect(ui->saveLogsButton, &QPushButton::clicked, this, &MainWindow::saveLogs);
    connect(ui->clearLogsButton, &QPushButton::clicked, ui->logsEdit, &QTextEdit::clear);
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

    ui->saveLogsButton->setIcon(QtAwesome::instance()->icon(fa::save));
    ui->clearLogsButton->setIcon(QtAwesome::instance()->icon(fa::eraser));
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
    if (!m_serial.open(QIODevice::ReadWrite)) {
        return;
    }
    emit portOpened();
    qDebug() << tr("Opened port \"%0\"").arg(m_serial.portName());
}

void MainWindow::disconnectPort()
{
    m_serial.close();
    qDebug() << tr("Closed port \"%0\"").arg(m_serial.portName());
}

void MainWindow::saveLogs()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Select a File"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QFile logsFile(filename);

    if (!logsFile.open(QFile::WriteOnly)) {
        qDebug() << tr("Failed to open file %0 to save logs").arg(filename);
        return;
    }
    logsFile.write(ui->logsEdit->toPlainText().toStdString().c_str());
    logsFile.close();
    qDebug() << tr("Log succesfully saved to %0").arg(filename);
}

void MainWindow::logPortError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError || error == QSerialPort::NotOpenError)
        return;

    qDebug() << tr("Error: %0, (code: %1)").arg(m_serial.errorString()).arg(error);
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
    qDebug() << tr("Start messaging");
}

void MainWindow::processStop()
{
    m_sendTimer.stop();
    qDebug() << tr("Stop messaging");
}
