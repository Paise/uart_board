#ifndef DCMOTORWIDGET_H
#define DCMOTORWIDGET_H

#include "irunnablewidget.h"
#include <QtCharts>

class DCVectorSender;
class DCResponseListener;
namespace Ui {
class DCMotorWidget;
}

class DCMotorWidget : public IRunnableWidget
{
    Q_OBJECT

public:
    explicit DCMotorWidget(QWidget *parent = 0);
    ~DCMotorWidget();

    void setSerialDevice(ISerialIO *serial);
    bool processChartMouseMove(QMouseEvent *e);

private:
    class ChartView : public QChartView {
    public:
        ChartView(QChart *chart, DCMotorWidget *widget);
    private:
        void mouseMoveEvent(QMouseEvent *event);
        DCMotorWidget *m_widget;
    };

    Ui::DCMotorWidget *ui;
    DCVectorSender *m_sender;
    DCResponseListener *m_listener;
    QChart *m_chart;
    QLineSeries *m_series;
    QLineSeries *m_recievedPoints;
    QScatterSeries *m_scatter;
    QScatterSeries *m_sendedPoints;
    QPointF m_selected;

public slots:
    void run();
    void stop();

private slots:
    void addPoint(const QPointF &);
    void replacePoint(const QPointF &oldp, const QPointF &newp);
    void selectPoint(const QPointF &);
    void releasePoint(const QPointF &);
    void resetXAxisRange();
    void drawRecievedPoint(int index, quint16 val);
    void drawSendedPoint(int index, quint16 val);
    void clearScreen();
};

#endif // DCMOTORWIDGET_H
