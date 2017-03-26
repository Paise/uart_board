#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>

class CustomSerialPort;
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
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void initIcons();
    void initStateMachine();

    Ui::MainWindow *ui;
    CustomSerialPort *m_serial;
    QStateMachine m_stateMachine;

private slots:
    void about();
    void saveLogs();
    void configurePort();

    void processDisconnectState();
    void processConnectState();
    void processRunningState();
    void processStop();
};

#endif // MAINWINDOW_H
