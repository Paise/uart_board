#ifndef IRUNNABLEWIDGET_H
#define IRUNNABLEWIDGET_H

#include <QWidget>

class ISerialIO;

/**
 * @brief Интерфейс виджета, управляющего объектами, которые могут быть запущенными на выполнение кнопкой Start
 */
class IRunnableWidget : public QWidget
{
    Q_OBJECT

public:
    IRunnableWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~IRunnableWidget() {}

    /**
     * @brief Для выполнения задания должен быть использован последовательный порт
     * @param serial Экземплят последовательного порта
     */
    virtual void setSerialDevice(ISerialIO *serial) =0;

public slots:
    /**
     * @brief Запустить выполнение
     */
    virtual void run() =0;
    /**
     * @brief Остановить выполнение
     */
    virtual void stop() =0;

signals:
    /**
     * @brief Выполенение завершено
     */
    void completed();

};

#endif // IRUNNABLEWIDGET_H
