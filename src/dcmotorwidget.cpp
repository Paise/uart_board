#include "dcmotorwidget.h"
#include "ui_dcmotorwidget.h"

using namespace QtCharts;
#define POINTS_COUNT (8)

DCMotorWidget::DCMotorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DCMotorWidget),
    m_chart(new QChart),
    m_series(new QLineSeries),
    m_scatter(new QScatterSeries)
{
    ui->setupUi(this);

    for (int i = 0; i < 8; ++i) {
        addPoint(QPointF(i,0));
    }
    m_chart->legend()->hide();
    m_chart->addSeries(m_series);
    m_chart->addSeries(m_scatter);
    m_chart->createDefaultAxes();
    ChartView *chartView = new ChartView(m_chart, this);
    ui->mainLayout->insertWidget(0, chartView);

    connect(m_scatter, &QXYSeries::pressed, this, &DCMotorWidget::selectPoint);
    connect(m_scatter, &QXYSeries::released, this, &DCMotorWidget::releasePoint);
}

DCMotorWidget::~DCMotorWidget()
{
    delete ui;
}

bool DCMotorWidget::processChartMouseMove(QMouseEvent *e)
{
    if (m_selected.isNull()) {
        return false;
    }
    replacePoint(m_selected, m_chart->mapToValue(e->pos()));
    e->accept();
    return true;
}

void DCMotorWidget::addPoint(const QPointF &point)
{
    m_series->append(point);
    m_scatter->append(point);
}

void DCMotorWidget::replacePoint(const QPointF &oldp, const QPointF &newp)
{
    m_series->replace(oldp, newp);
    m_scatter->replace(oldp, newp);
    m_selected = newp;
}

void DCMotorWidget::selectPoint(const QPointF &point)
{
    if (!m_selected.isNull()) {
        qWarning() << "Previous point is unselected before select";
    }

    m_selected = point;
}

void DCMotorWidget::releasePoint(const QPointF &point)
{
    if (m_selected.isNull()) return;
    if (point != m_selected) {
        qWarning() << "Unselected point dont match with selected";
    }

    m_selected = QPointF();
}

DCMotorWidget::ChartView::ChartView(QChart *chart, DCMotorWidget *widget) :
    QChartView(chart),
    m_widget(widget)
{

}

void DCMotorWidget::ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_widget->processChartMouseMove(event)) {
        QChartView::mouseMoveEvent(event);
    }
}
