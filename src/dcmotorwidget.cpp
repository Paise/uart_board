#include "dcmotorwidget.h"
#include "ui_dcmotorwidget.h"
#include <QIntValidator>
#include <QtMath>

using namespace QtCharts;
#define INIT_POINTS_COUNT (8)
#define Y_MIN (0)
#define Y_MAX (0xFFFF)
#define Y_STEP (0x1FFF)
#define MAX_INTERVAL (5000)
#define INIT_INTERVAL (200)

DCMotorWidget::DCMotorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DCMotorWidget),
    m_chart(new QChart),
    m_series(new QLineSeries),
    m_scatter(new QScatterSeries)
{
    ui->setupUi(this);

    /* Init controls*/
    ui->intervalEdit->setValidator(new QIntValidator(1, MAX_INTERVAL));
    ui->intervalEdit->setPlaceholderText(QString("1-%0").arg(MAX_INTERVAL));
    ui->intervalEdit->setText(QString("%0").arg(INIT_INTERVAL));
    connect(ui->intervalEdit, &QLineEdit::returnPressed, this, &DCMotorWidget::resetXAxisRange);
    connect(ui->intervalApplyButton, &QPushButton::clicked, this, &DCMotorWidget::resetXAxisRange);

    for (int i = 0; i < INIT_POINTS_COUNT; ++i) {
        addPoint(QPointF(i*INIT_INTERVAL,0));
    }
    m_chart->setTheme(QChart::ChartThemeDark);
    m_chart->legend()->hide();
    m_chart->addSeries(m_series);
    m_chart->addSeries(m_scatter);
    m_chart->createDefaultAxes();
    /* Axes setting */
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(m_chart->axisY());
    QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axisX());
    yAxis->setRange(0, 0xFFFF);
    yAxis->setTickCount(Y_MAX/Y_STEP + 1);
    resetXAxisRange();
    xAxis->setTickCount(INIT_POINTS_COUNT);

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
    if (m_selected.isNull()) return false;

    QPointF atMouse = m_chart->mapToValue(e->pos());
    qreal nextY = ((int) (atMouse.ry()/Y_STEP)) * Y_STEP;
    if (nextY < Y_MIN)
        nextY = Y_MIN;
    else if (nextY > Y_MAX)
        nextY = Y_MAX;

    QPointF next(m_selected.rx(), nextY);
    replacePoint(m_selected, next);
    m_selected = next;
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

void DCMotorWidget::resetXAxisRange()
{
    int interval = ui->intervalEdit->text().toInt();
    m_chart->axisX()->setRange(0, (INIT_POINTS_COUNT-1)*interval);
    for (int i = 0; i < m_scatter->points().count(); ++i) {
        QPointF oldP = m_scatter->at(i);
        QPointF newP(i*interval, oldP.ry());
        replacePoint(oldP, newP);
    }
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
