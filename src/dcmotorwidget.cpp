#include "dcmotorwidget.h"
#include "ui_dcmotorwidget.h"
#include "dcvectorsender.h"
#include "dcresponselistener.h"
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
    IRunnableWidget(parent),
    ui(new Ui::DCMotorWidget),
    m_sender(nullptr),
    m_listener(nullptr),
    m_chart(new QChart),
    m_series(new QLineSeries),
    m_recievedPoints(new QLineSeries),
    m_scatter(new QScatterSeries),
    m_sendedPoints(new QScatterSeries),
    m_sendInterval(INIT_INTERVAL)
{
    ui->setupUi(this);

    /* Init controls*/
    ui->intervalEdit->setValidator(new QIntValidator(1, MAX_INTERVAL));
    ui->intervalEdit->setPlaceholderText(QString("1-%0").arg(MAX_INTERVAL));
    ui->intervalEdit->setText(QString("%0").arg(INIT_INTERVAL));

    ui->ticksEdit->setValidator(new QIntValidator(0, 5));
    ui->ticksEdit->setPlaceholderText(QString("0-5"));
    ui->ticksEdit->setText(QString("0"));

    connect(ui->applyButton, &QPushButton::clicked, this, &DCMotorWidget::resetXAxisRange);
    connect(ui->applyButton, &QPushButton::clicked, this, &DCMotorWidget::changeXAxisMinorTicks);

    for (int i = 0; i < INIT_POINTS_COUNT; ++i) {
        addPoint(QPointF(i*INIT_INTERVAL,0));
    }
    /* Series Settings */
    m_scatter->setColor(Qt::green);
    m_sendedPoints->setColor(Qt::red);
    /* Chart Settings */
    m_chart->setTheme(QChart::ChartThemeDark);
    m_chart->legend()->hide();
    m_chart->addSeries(m_series);
    m_chart->addSeries(m_recievedPoints);
    m_chart->addSeries(m_scatter);
    m_chart->addSeries(m_sendedPoints);
    m_chart->createDefaultAxes();
    /* Axes Settings */
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

void DCMotorWidget::setSerialDevice(ISerialIO *serial)
{
    if (m_sender) return;

    m_sender = new DCVectorSender(serial, this);
    m_listener = new DCResponseListener(serial, this);

    connect(m_sender, &DCVectorSender::completed, this, &DCMotorWidget::completed);
    connect(m_sender, &DCVectorSender::completed, this, &DCMotorWidget::clearScreen);
    connect(m_sender, &DCVectorSender::completed, m_listener, &DCResponseListener::cancel);
    connect(m_sender, &DCVectorSender::started, m_listener, &DCResponseListener::listen);
    connect(m_sender, &DCVectorSender::sended, this, &DCMotorWidget::drawSendedPoint);
    connect(m_listener, &DCResponseListener::recieved, this, &DCMotorWidget::drawRecievedPoint);
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

void DCMotorWidget::run()
{
    if (!m_sender) return;
    m_sender->start(m_scatter->points(), m_sendInterval);
    ui->intervalEdit->setEnabled(false);
    ui->ticksEdit->setEnabled(false);
    ui->applyButton->setEnabled(false);
    m_chart->setEnabled(false);
}

void DCMotorWidget::stop()
{
    if (!m_sender) return;
    m_sender->cancel();
    ui->intervalEdit->setEnabled(true);
    ui->ticksEdit->setEnabled(true);
    ui->applyButton->setEnabled(true);
    m_chart->setEnabled(true);
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
    QList<QPointF> points;
    for (int i = 0; i < m_scatter->points().count(); ++i) {
        QPointF oldP = m_scatter->at(i);
        points << QPointF(i*interval, oldP.ry());
    }
    m_series->clear();
    m_scatter->clear();
    m_series->append(points);
    m_scatter->append(points);
}

void DCMotorWidget::changeXAxisMinorTicks()
{
    QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axisX());
    xAxis->setMinorTickCount(ui->ticksEdit->text().toInt());
    m_sendInterval = ui->intervalEdit->text().toInt() / (xAxis->minorTickCount() + 1);
}

void DCMotorWidget::drawRecievedPoint(int index, quint16 val)
{
    m_recievedPoints->append(m_sendInterval*index, val);
}

void DCMotorWidget::drawSendedPoint(int index, quint16 val)
{
    m_sendedPoints->clear();
    m_sendedPoints->append(m_sendInterval*index, val);
}

void DCMotorWidget::clearScreen()
{
    m_recievedPoints->clear();
    m_sendedPoints->clear();
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
