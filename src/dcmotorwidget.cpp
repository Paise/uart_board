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
    m_sendedPoints(new QScatterSeries)
{
    ui->setupUi(this);

    /* Init controls*/
    ui->intervalEdit->setValidator(new QIntValidator(1, MAX_INTERVAL));
    ui->intervalEdit->setPlaceholderText(QString("1-%0").arg(MAX_INTERVAL));
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
    xAxis->setTitleText("msec");
    /* Create initial points */
    QList<QPointF> initPoints;
    for (int i = 0; i < INIT_POINTS_COUNT; ++i) {
        initPoints.append(QPointF(i*INIT_INTERVAL,0));
    }
    setUserPoints(initPoints);
    setMinorTicks(INIT_TICKS);
    /* Add chart on widget */
    ChartView *chartView = new ChartView(m_chart, this);
    ui->mainLayout->insertWidget(0, chartView);

    connect(m_scatter, &QXYSeries::pressed, this, &DCMotorWidget::selectPoint);
    connect(m_scatter, &QXYSeries::released, this, &DCMotorWidget::releasePoint);
    connect(ui->intervalEdit, &QLineEdit::returnPressed, this, &DCMotorWidget::rescalePointsForNewInterval);
    connect(ui->ticksEdit, &QLineEdit::returnPressed, this, &DCMotorWidget::changeXAxisMinorTicks);
    connect(ui->applyButton, &QPushButton::clicked, this, &DCMotorWidget::applySettings);
    connect(ui->clearButton, &QPushButton::clicked, this, &DCMotorWidget::clearScreen);
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

QList<QPointF> DCMotorWidget::userPoints() const
{
    return m_scatter->points();
}

QList<QPointF> DCMotorWidget::recievedPoints() const
{
    return m_recievedPoints->points();
}

int DCMotorWidget::minorTicks() const
{
    QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axisX());
    return xAxis->minorTickCount();
}

void DCMotorWidget::setUserPoints(const QList<QPointF> &points)
{
    m_scatter->clear();
    m_series->clear();
    m_scatter->append(points);
    m_series->append(points);
    QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axisX());
    xAxis->setTickCount(points.count());
    ui->intervalEdit->setText(QString("%0").arg(pointsInterval()));
    changeXAxisRange();
}

void DCMotorWidget::setRecievedPoints(const QList<QPointF> &points)
{
    clearScreen();
    m_recievedPoints->append(points);
}

void DCMotorWidget::setMinorTicks(int ticks)
{
    ui->ticksEdit->setText(QString("%0").arg(ticks));
    changeXAxisMinorTicks();
}

QList<QPointF> DCMotorWidget::pointsToSend()
{
    QList<QPointF> points;
    QList<QPointF> chartPoints = m_scatter->points();

    points.append(*chartPoints.begin());
    for (auto p = chartPoints.begin()+1; p != chartPoints.end(); ++p) {
        auto prev = p - 1;
        int stepsBetween = round((p->rx() - prev->rx()) / sendInterval());
        float mult = (p->ry() - prev->ry()) / (stepsBetween);
        for (int i = 1; i <= stepsBetween-1; ++i) {
            QPointF point;
            point.setX(prev->rx() + i*sendInterval());
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
    qreal nextX = (floor(rx/sendInterval())) * sendInterval();

    minX += sendInterval();
    maxX -= sendInterval();
    if (nextX < minX)
        nextX = minX;
    else if (nextX > maxX)
        nextX = maxX;

    return nextX;
}

void DCMotorWidget::checkRemainder()
{
    int ticks = ui->ticksEdit->text().toInt();
    if (ticks && (pointsInterval() % ticks) != 0) {
        ui->warningsLabel->setText(tr("Warning: Interval cannot be divided to ticks without remainder"));
    } else {
        ui->warningsLabel->clear();
    }
}

int DCMotorWidget::pointsInterval() const
{
    if (m_scatter->count() == 0) return 0;

    if (m_scatter->count() == 1) {
        return m_scatter->points().last().rx();
    } else {
        return m_scatter->points().last().rx() / (m_scatter->count() - 1);
    }
}

int DCMotorWidget::sendInterval() const
{
    QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axisX());
    return pointsInterval() / (xAxis->minorTickCount() + 1);
}

void DCMotorWidget::run()
{
    if (!m_sender) return;
    clearScreen();
    m_sender->start(pointsToSend(), sendInterval());
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

void DCMotorWidget::rescalePointsForNewInterval()
{
    int new_interval = ui->intervalEdit->text().toInt();
    if (new_interval == pointsInterval()) {
        return;
    }
    clearScreen();
    changeXAxisRange();

    QList<QPointF> points;
    float scaler = (float) new_interval / (float) pointsInterval();
    for (int i = 0; i < m_scatter->points().count(); ++i) {
        QPointF oldP = m_scatter->at(i);
        points << QPointF(floor(oldP.rx()*scaler), oldP.ry());
    }
    m_series->replace(points);
    m_scatter->replace(points);
    checkRemainder();
}

void DCMotorWidget::changeXAxisRange()
{
    m_chart->axisX()->setRange(0, (m_series->count()-1)*ui->intervalEdit->text().toInt());
}

void DCMotorWidget::changeXAxisMinorTicks()
{
    /* To not allow send interval to be less than 1 ms */
    int maxTicks = pointsInterval() - 1;
    ui->ticksEdit->setValidator(new QIntValidator(0, maxTicks));
    ui->ticksEdit->setPlaceholderText(QString("0-%0").arg(maxTicks));
    /* Check is new constraints are satisfied */
    if (ui->ticksEdit->text().toInt() > maxTicks) {
        ui->ticksEdit->setText(QString("%0").arg(maxTicks));
    }
    /* Apply ticks */
    QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axisX());
    xAxis->setMinorTickCount(ui->ticksEdit->text().toInt());
    checkRemainder();
}

void DCMotorWidget::drawRecievedPoint(int index, quint16 val)
{
    m_recievedPoints->append(sendInterval()*index, val);
}

void DCMotorWidget::drawSendedPoint(int index, quint16 val)
{
    m_sendedPoints->clear();
    m_sendedPoints->append(sendInterval()*index, val);
}

void DCMotorWidget::clearScreen()
{
    m_recievedPoints->clear();
    m_sendedPoints->clear();
    ui->clearButton->setEnabled(false);
}

void DCMotorWidget::applySettings()
{
    rescalePointsForNewInterval();
    changeXAxisMinorTicks();
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
