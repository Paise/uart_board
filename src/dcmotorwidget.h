#ifndef DCMOTORWIDGET_H
#define DCMOTORWIDGET_H

#include "irunnablewidget.h"
#include <QtCharts>

class DCVectorSender;
class DCResponseListener;
namespace Ui {
class DCMotorWidget;
}

/**
 * @brief Виджет для управления двигателем постоянного тока
 *
 * @details Пользователь может мышкой редактировать уставки, передаваемые на двигатель. Для этого используется набор точек,
 * задающих форму управляющей кривой. Можно задать определенное количество промежуточных точек между основными, их значения
 * будут вычислены перед передачей данных на двигатель.
 */
class DCMotorWidget : public IRunnableWidget
{
    Q_OBJECT

public:
    explicit DCMotorWidget(QWidget *parent = 0);
    ~DCMotorWidget();

    void setSerialDevice(ISerialIO *serial);
    /**
     * @brief Обрабатывает движение указателя по области графика
     *
     * @details В случае, если во время движения указателя по графику зажата точка графика, она передвигается
     * за указателем
     * @param e Событие движения мыши
     * @return Было ли обработано событие
     */
    bool processChartMouseMove(QMouseEvent *e);

    QList<QPointF> userPoints() const;
    QList<QPointF> recievedPoints() const;
    int minorTicks() const;
    void setUserPoints(const QList<QPointF> &points);
    void setRecievedPoints(const QList<QPointF> &points);
    void setMinorTicks(int ticks);


private:
    /**
     * @brief Формирует список точек, которые будут отправлены на двигатель
     *
     * @details Список точек состоит из точек, отображенных на графике, и промежуточных точек, значения которых
     * вычисляются в данной функции
     * @return Список точек
     */
    QList<QPointF> pointsToSend();

    qreal nextY(const qreal &ry);
    qreal nextX(const qreal &rx);
    void checkRemainder();

    /**
     * @brief Интервал межд точками на графике
     */
    int pointsInterval() const;
    /**
     * @brief Интервал между отправляемыми точками
     */
    int sendInterval() const;

    /**
     * @brief Реализация QChartView, переопределяющая обработку события движения мыши по графику
     */
    class ChartView : public QChartView {
    public:
        ChartView(QChart *chart, DCMotorWidget *widget);
    private:
        /**
         * @brief Передает событие движения мыши DCMotorWidget. Если событие не обработано DCMotorWidget, то вызывается реализация по умолчанию.
         * @param event Событие движения мыши
         */
        void mouseMoveEvent(QMouseEvent *event);
        DCMotorWidget *m_widget;
    };

    Ui::DCMotorWidget *ui;
    /**
     * @brief Объект, отправляющий данные на двигатель
     */
    DCVectorSender *m_sender;
    /**
     * @brief Объект, принимающий данные от двигателя
     */
    DCResponseListener *m_listener;
    /**
     * @brief График
     */
    QChart *m_chart;
    /**
     * @brief Отображение уставок сплошной линией
     */
    QLineSeries *m_series;
    /**
     * @brief Отображение принятых данных сплошной линией
     */
    QLineSeries *m_recievedPoints;
    /**
     * @brief Отображение уставок по точкам
     */
    QScatterSeries *m_scatter;
    /**
     * @brief Отображение текущей переданной уставки
     */
    QScatterSeries *m_sendedPoints;
    /**
     * @brief Текущая выделенная мышкой точка на графике
     */
    QPointF m_selected;

public slots:
    /**
     * @brief Начинает отправку уставок
     */
    void run();
    /**
     * @brief Останавливает отправку уставок
     */
    void stop();

private slots:
    /**
     * @brief Заменяет уставку на графике
     * @param oldp Старая уставка
     * @param newp Новая уставка
     */
    void replacePoint(const QPointF &oldp, const QPointF &newp);
    /**
     * @brief Задает точку как выделенную
     * @param p Выделенная точка
     */
    void selectPoint(const QPointF &p);
    /**
     * @brief Отменяет выделение точки
     * @param p Точка
     */
    void releasePoint(const QPointF &p);
    /**
     * @brief Обновляет отображение оси X при изменении интервала между точками
     */
    void rescalePointsForNewInterval();
    void changeXAxisRange();
    /**
     * @brief Обновляет отображение оси X при изменении количества промежуточных значений между точками
     */
    void changeXAxisMinorTicks();
    /**
     * @brief Добавляет на график принятую с двигателя точку
     * @param index Номер принятой точки
     * @param val Значение
     */
    void drawRecievedPoint(int index, quint16 val);
    /**
     * @brief Обновляет на графике текущую переданную точку
     * @param index Номер переданной точки
     * @param val Значение
     */
    void drawSendedPoint(int index, quint16 val);
    /**
     * @brief Удаляет с графика принятые и переданные точки
     */
    void clearScreen();
    /**
     * @brief Применяет значения интервала и числа значений между точками
     */
    void applySettings();
};

#endif // DCMOTORWIDGET_H
