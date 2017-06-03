#include "dcmotorwidget.h"
#include "ui_dcmotorwidget.h"
#include "dcvectorsender.h"
#include "dcresponselistener.h"
#include <QIntValidator>
#include <QtMath>

using namespace QtCharts;
#define INIT_POINTS_COUNT (8)
#define Y_MIN (0)
#define Y_MAX (60)
#define Y_STEP (5)
#define MAX_INTERVAL (5000)
#define INIT_INTERVAL (2000)
#define INIT_TICKS (16)

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
    m_pointsInterval(INIT_INTERVAL),
    m_sendInterval(m_pointsInterval)
{
    ui->setupUi(this);

    /* Init controls*/
    ui->intervalEdit->setValidator(new QIntValidator(1, MAX_INTERVAL));
    ui->intervalEdit->setPlaceholderText(QString("1-%0").arg(MAX_INTERVAL));
    ui->intervalEdit->setText(QString("%0").arg(INIT_INTERVAL));
    ui->ticksEdit->setText(QString("%0").arg(INIT_TICKS));

    connect(ui->applyButton, &QPushButton::clicked, this, &DCMotorWidget::applySettings);
    connect(ui->clearButton, &QPushButton::clicked, this, &DCMotorWidget::clearScreen);

    m_series->clear();
    m_scatter->clear();
    for (int i = 0; i < INIT_POINTS_COUNT; ++i) {
        addPoint(QPointF(i*INIT_INTERVAL,0));
    }
    /* Series Settings */
    m_scatter->setColor(Qt::green);
    m_sendedPoints->setColor(Qt::red);
    m_recievedPoints->setColor(Qt::yellow);
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
    yAxis->setRange(Y_MIN, Y_MAX);
    yAxis->setTickCount(Y_MAX/Y_STEP + 1);
    yAxis->setTitleText("rps");
    xAxis->setTickCount(INIT_POINTS_COUNT);
    xAxis->setTitleText("msec");

    ChartView *chartView = new ChartView(m_chart, this);
    ui->mainLayout->insertWidget(0, chartView);

    resetXAxisRange();
    changeXAxisMinorTicks();

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
    connect(m_sender, &DCVectorSender::completed, m_listener, &DCResponseListener::cancel);
    connect(m_sender, &DCVectorSender::started, m_listener, &DCResponseListener::listen);
    connect(m_sender, &DCVectorSender::sended, this, &DCMotorWidget::drawSendedPoint);
    connect(m_listener, &DCResponseListener::recieved, this, &DCMotorWidget::drawRecievedPoint);
}

bool DCMotorWidget::processChartMouseMove(QMouseEvent *e)
{
    if (m_selected.isNull()) return false;

    QPointF atMouse = m_chart->mapToValue(e->pos());

    QPointF next(nextX(atMouse.rx()), nextY(atMouse.ry()));
    replacePoint(m_selected, next);
    m_selected = next;
    e->accept();
    return true;
}

QList<QPointF> DCMotorWidget::pointsToSend()
{
    QList<QPointF> points;
    QList<QPointF> chartPoints = m_scatter->points();

    points.append(*chartPoints.begin());
    for (auto p = chartPoints.begin()+1; p != chartPoints.end(); ++p) {
        auto prev = p - 1;
        int stepsBetween = round((p->rx() - prev->rx()) / m_sendInterval);
        float mult = (p->ry() - prev->ry()) / (stepsBetween);
        for (int i = 1; i <= stepsBetween-1; ++i) {
            QPointF point;
            point.setX(prev->rx() + i*m_sendInterval);
            point.setY(prev->ry() + i*mult);
            points << point;
        }
        points << QPointF(*p);
    }

    return points;
}

qreal DCMotorWidget::nextY(const qreal &ry)
{
    qreal nextY = ((int) (ry/Y_STEP)) * Y_STEP;

    if (nextY < Y_MIN)
        nextY = Y_MIN;
    else if (nextY > Y_MAX)
        nextY = Y_MAX;

    return nextY;
}

qreal DCMotorWidget::nextX(const qreal &rx)
{
    QList<QPointF> points = m_scatter->points();
    int index = points.indexOf(m_selected);
    if (index == points.size()-1) return m_selected.rx();

    qreal minX = (index == 0) ? 0 : m_scatter->points().value(index-1).rx();
    qreal maxX = m_scatter->points().value(index+1).rx();
    qreal nextX = (floor(rx/m_sendInterval)) * m_sendInterval;

    minX += m_sendInterval;
    maxX -= m_sendInterval;
    if (nextX < minX)
        nextX = minX;
    else if (nextX > maxX)
        nextX = maxX;

    return nextX;
}

void DCMotorWidget::run()
{
    if (!m_sender) return;
    clearScreen();
    m_sender->start(pointsToSend(), m_sendInterval);
    ui->intervalEdit->setEnabled(false);
    ui->ticksEdit->setEnabled(false);
    ui->applyButton->setEnabled(false);
    ui->clearButton->setEnabled(false);
    m_chart->setEnabled(false);
}

void DCMotorWidget::stop()
{
    if (!m_sender) return;
    m_sender->cancel();
    ui->intervalEdit->setEnabled(true);
    ui->ticksEdit->setEnabled(true);
    ui->applyButton->setEnabled(true);
    ui->clearButton->setEnabled(true);
    m_chart->setEnabled(true);
    m_sendedPoints->clear();
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
    int prev_interval = m_pointsInterval;
    m_pointsInterval = ui->intervalEdit->text().toInt();
    if (!m_pointsInterval) {
        m_pointsInterval = INIT_INTERVAL;
        ui->intervalEdit->setText(QString("%0").arg(m_pointsInterval));
    }

    m_chart->axisX()->setRange(0, (m_series->count()-1)*m_pointsInterval);
    QList<QPointF> points;
    float scaler = (float) m_pointsInterval / (float) prev_interval;
    for (int i = 0; i < m_scatter->points().count(); ++i) {
        QPointF oldP = m_scatter->at(i);
        points << QPointF(oldP.rx()*scaler, oldP.ry());
    }
    m_series->replace(points);
    m_scatter->replace(points);
}

void DCMotorWidget::changeXAxisMinorTicks()
{
    /* To not allow send interval to be less than 1 ms */
    int maxTicks = m_pointsInterval - 1;
    ui->ticksEdit->setValidator(new QIntValidator(0, maxTicks));
    ui->ticksEdit->setPlaceholderText(QString("0-%0").arg(maxTicks));
    /* Check is new constraints are satisfied */
    if (ui->ticksEdit->text().toInt() > maxTicks) {
        ui->ticksEdit->setText(QString("%0").arg(maxTicks));
    }
    /* Apply ticks */
    QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axisX());
    xAxis->setMinorTickCount(ui->ticksEdit->text().toInt());
    m_sendInterval = m_pointsInterval / (xAxis->minorTickCount() + 1);
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
    ui->clearButton->setEnabled(false);
}

void DCMotorWidget::applySettings()
{
    clearScreen();
    if (m_sendInterval != ui->intervalEdit->text().toInt()) {
        resetXAxisRange();
    }
    changeXAxisMinorTicks();
    int ticks = ui->ticksEdit->text().toInt();
    if (ticks && (m_pointsInterval % ticks) != 0) {
        ui->warningsLabel->setText(tr("Warning: Interval cannot be divided to ticks without remainder"));
    } else {
        ui->warningsLabel->clear();
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
