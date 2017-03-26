#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QStateMachine>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initStateMachine();

    Ui::MainWindow *ui;
    QSerialPort m_serial;
    QTimer m_sendTimer;
    QStateMachine m_stateMachine;

private slots:
    void about();
    void sendData();
    void startMessaging();
    void stopMessaging();
    void configurePort();
    void connectPort();
    void disconnectPort();

    void processDisconnectState();
    void processConnectState();
    void processRunningState();

signals:
    void portOpened();
};

#endif // MAINWINDOW_H
