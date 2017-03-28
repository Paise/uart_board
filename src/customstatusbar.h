#ifndef CUSTOMSTATUSBAR_H
#define CUSTOMSTATUSBAR_H

#include <QStatusBar>
#include <QLabel>

class CustomStatusBar : public QStatusBar
{
    Q_OBJECT
    Q_PROPERTY(QColor statusColor READ statusColor WRITE setStatusColor)

public:
    explicit CustomStatusBar(QWidget *parent = 0);

    QString permanentMessage() const;
    QColor statusColor() const;
    void setPermanentMessage(const QString &text);
    void setStatusColor(const QColor &color);

private:
    QColor m_color;
    QLabel m_permanentMessage;
    QLabel m_statusLabel;
};

#endif // CUSTOMSTATUSBAR_H
