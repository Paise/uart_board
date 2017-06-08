#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include "messagelogger.h"
#include "configuredialog.h"
#include "customstatusbar.h"
#include "serialportholder.h"
#include "irunnablewidget.h"
#include "ext/QtAwesome/QtAwesome.h"
#include <QDebug>
#include <QState>
#include <QStyle>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSettings>

//Save|load project
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serial(new SerialPortHolder(new QSerialPort, this)),
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
    ui->mainToolBar->addAction( ui->actionConfigure_Port );

    ui->mainToolBar->addAction( ui->actionLoad );
    ui->mainToolBar->addAction( ui->actionSave );

    ui->rawTab->setSerialDevice(m_serial);
    ui->DCTab->setSerialDevice(m_serial);
    ui->controlTab->setSerialDevice(m_serial);

    MessageLogger::instance()->setOutWidget(ui->logsEdit);
    initIcons();
    initStateMachine();

    connect(ui->actionConfigure_Port, &QAction::triggered, this, &MainWindow::configurePort);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionShow_Logs, &QAction::triggered, ui->logsDockWidget, &QDockWidget::setVisible);
    connect(ui->logsDockWidget, &QDockWidget::visibilityChanged, ui->actionShow_Logs, &QAction::setChecked);
    connect(ui->saveLogsButton, &QPushButton::clicked, this, &MainWindow::saveLogs);
    connect(ui->clearLogsButton, &QPushButton::clicked, ui->logsEdit, &QTextEdit::clear);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveGraph);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::loadGraph);

}

MainWindow::~MainWindow()
{
    m_serial->disconnectPort();
    delete ui;
}

void MainWindow::showEvent(QShowEvent *)
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    this->restoreGeometry(settings.value("geometry").toByteArray());
    this->restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("state", this->saveState());
    settings.endGroup();
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
    ui->clearLogsButton->setIcon(QtAwesome::instance()->icon(fa::trasho));
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
    stateDisconnected->assignProperty(ui->rawTab, "enabled", false);
    stateDisconnected->assignProperty(ui->statusBar, "statusColor", QColor(Qt::red));
    stateDisconnected->assignProperty(ui->tabWidget->tabBar(), "enabled", true);

    stateConnected->assignProperty(ui->actionStart, "enabled", true);
    stateConnected->assignProperty(ui->actionStop, "enabled", false);
    stateConnected->assignProperty(ui->actionDisconnect, "enabled", true);
    stateConnected->assignProperty(ui->actionConfigure_Port, "enabled", true);
    stateConnected->assignProperty(ui->rawTab, "enabled", true);
    stateConnected->assignProperty(ui->statusBar, "statusColor", QColor(Qt::yellow));
    stateConnected->assignProperty(ui->tabWidget->tabBar(), "enabled", true);

    stateRunnig->assignProperty(ui->actionStart, "enabled", false);
    stateRunnig->assignProperty(ui->actionStop, "enabled", true);
    stateRunnig->assignProperty(ui->actionDisconnect, "enabled", false);
    stateRunnig->assignProperty(ui->actionConfigure_Port, "enabled", false);
    stateRunnig->assignProperty(ui->rawTab, "enabled", true);
    stateRunnig->assignProperty(ui->statusBar, "statusColor", QColor(Qt::green));
    stateRunnig->assignProperty(ui->tabWidget->tabBar(), "enabled", false);

    stateDisconnected->addTransition(m_serial, SIGNAL(opened()), stateConnected);
    stateConnected->addTransition(ui->actionStart, SIGNAL(triggered()), stateRunnig);
    stateConnected->addTransition(ui->actionDisconnect, SIGNAL(triggered()), stateDisconnected);
    stateConnected->addTransition(m_serial, SIGNAL(closed()), stateDisconnected);
    stateConnected->addTransition(m_serial, SIGNAL(errorOccured(const QString &, QSerialPort::SerialPortError)), stateDisconnected);
    stateRunnig->addTransition(ui->actionStop, SIGNAL(triggered()), stateConnected);
    stateRunnig->addTransition(m_serial, SIGNAL(closed()), stateDisconnected);
    stateRunnig->addTransition(m_serial, SIGNAL(errorOccured(const QString &, QSerialPort::SerialPortError)), stateDisconnected);
    stateRunnig->addTransition(ui->rawTab, SIGNAL(completed()), stateConnected);
    stateRunnig->addTransition(ui->DCTab, SIGNAL(completed()), stateConnected);

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

void MainWindow::saveGraph()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Select a File"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (filename.isEmpty()) return;
    QFile graphFile(filename);

    if (!graphFile.open(QFile::WriteOnly)) {
        qDebug() << tr("Failed to open file %0 to save graph points").arg(filename);
        return;
    }

    QDataStream out(&graphFile);
    out << ui->DCTab->minorTicks() << ui->DCTab->userPoints() << ui->DCTab->recievedPoints();

    graphFile.close();
    qDebug() << tr("Graph info succesfully saved to %0").arg(filename);
}

void MainWindow::loadGraph()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open a File"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (filename.isEmpty()) return;
    QFile graphFile(filename);

    if (!graphFile.open(QFile::ReadOnly)) {
        qDebug() << tr("Failed to open file %0 to load graph points").arg(filename);
        return;
    }

    QDataStream in(&graphFile);
    int ticks;
    QList<QPointF> points;
    QList<QPointF> pointsRec;
    in >> ticks >> points >> pointsRec;
    ui->DCTab->setUserPoints(points);
    ui->DCTab->setMinorTicks(ticks);
    ui->DCTab->setRecievedPoints(pointsRec);

    graphFile.close();
    qDebug() << tr("Graph info successfully loaded from %0").arg(filename);
}

void MainWindow::configurePort()
{
    ConfigureDialog dialog(this);

    dialog.setWindowFlags(Qt::Window);
    dialog.setWindowModality(Qt::ApplicationModal);
    if (dialog.exec() == QDialog::Accepted) {
        m_serial->disconnectPort();
        m_serial->serialPort()->setPortName( dialog.portName() );
        m_serial->serialPort()->setBaudRate( dialog.baudRate() );
        m_serial->serialPort()->setDataBits( dialog.dataBits() );
        m_serial->serialPort()->setParity( dialog.parity() );
        m_serial->serialPort()->setStopBits( dialog.stopBits() );
        m_serial->serialPort()->setFlowControl( dialog.flowControl() );
        m_serial->connectPort();
    }
}

void MainWindow::processDisconnectState()
{
    m_serial->disconnectPort();
    ui->statusBar->setPermanentMessage(tr("Disconected"));
}

void MainWindow::processConnectState()
{
    ui->statusBar->setPermanentMessage(tr("Connected to %0").arg(m_serial->serialPort()->portName()));
}

void MainWindow::processRunningState()
{
    qDebug() << tr("Start messaging");
    ui->statusBar->setPermanentMessage(tr("Running on %0").arg(m_serial->serialPort()->portName()));
    IRunnableWidget *currentWidget = dynamic_cast<IRunnableWidget*>(ui->tabWidget->currentWidget());
    if (currentWidget) currentWidget->run();
}

void MainWindow::processStop()
{
    qDebug() << tr("Stop messaging");
    IRunnableWidget *currentWidget = dynamic_cast<IRunnableWidget*>(ui->tabWidget->currentWidget());
    if (currentWidget) currentWidget->stop();
}
