#include "customstatusbar.h"
#include <QPainter>

CustomStatusBar::CustomStatusBar(QWidget *parent) :
    QStatusBar(parent),
    m_color(Qt::red),
    m_permanentMessage(this),
    m_statusLabel(this)
{
    this->addPermanentWidget(&m_permanentMessage);
    this->addPermanentWidget(&m_statusLabel);
}

QString CustomStatusBar::permanentMessage() const
{
    return m_permanentMessage.text();
}

QColor CustomStatusBar::statusColor() const
{
    return m_color;
}

void CustomStatusBar::setPermanentMessage(const QString &text)
{
    m_permanentMessage.setText(text);
}

void CustomStatusBar::setStatusColor(const QColor &color)
{
    QPixmap pixmap( QSize(20,20) );
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);

    m_color = color;
    painter.setBrush( m_color );
    painter.drawEllipse(QPoint(10,10), 8, 8);
    m_statusLabel.setPixmap( pixmap );
}
