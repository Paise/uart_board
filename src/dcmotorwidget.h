#ifndef DCMOTORWIDGET_H
#define DCMOTORWIDGET_H

#include "irunnablewidget.h"
#include <QtCharts>

namespace Ui {
class DCMotorWidget;
}

class DCMotorWidget : public IRunnableWidget
{
    Q_OBJECT

public:
    explicit DCMotorWidget(QWidget *parent = 0);
    ~DCMotorWidget();

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
    QChart *m_chart;
    QLineSeries *m_series;
    QScatterSeries *m_scatter;
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
};

#endif // DCMOTORWIDGET_H
