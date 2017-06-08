#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>

class SerialPortHolder;
namespace Ui {
class MainWindow;
}

/**
 * @brief Главное окно приложения
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    /**
     * @brief При открытии окна восстанавливаются настройки, сохраненные в прошлой сессии
     */
    void showEvent(QShowEvent *);
    /**
     * @brief При закрытии окна автоматически сохраняется текущее состояние окна
     */
    void closeEvent(QCloseEvent *);
    /**
     * @brief Инициализирует иконки виджетов
     */
    void initIcons();
    /**
     * @brief Инициализирует машину состояний, управляющуюю поведением MainWindow
     */
    void initStateMachine();

    Ui::MainWindow *ui;
    /**
     * @brief Объект для работы с последовательным портом
     */
    SerialPortHolder *m_serial;
    /**
     * @brief Машина состояний управляет поведением и отображением главного окна
     *
     * @details Приложение может находиться в трех состояниях - порт не сконфиурирован (Disconnected), порт сконфигурирован (Connected)
     * и запущено выполнение программы (Running). В зависимости от текущего состояния меняются состояния виджетов главного окна.
     */
    QStateMachine m_stateMachine;

public slots:
    /**
     * @brief Открыть окно настройки последовательного порта
     */
    void configurePort();

private slots:
    /**
     * @brief Показать окно About
     */
    void about();
    /**
     * @brief Сохранить логи приложения в файл
     */
    void saveLogs();
    /**
      * @brief Сохранить точки из графика в файл
      */
    void saveGraph();
    /**
      * @brief Загрузить точки для графика из файла
      */
    void loadGraph();

    /**
     * @brief Вызывается при переходе в состояние Disconnected
     */
    void processDisconnectState();
    /**
     * @brief Вызывается при переходе в состояние Connected
     */
    void processConnectState();
    /**
     * @brief Вызывается при переходе в состояние Running
     */
    void processRunningState();
    /**
     * @brief Вызывается при выходе из состояния Running
     */
    void processStop();
};

#endif // MAINWINDOW_H
