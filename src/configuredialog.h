#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>
#include <QMap>
#include <QTimer>
#include <QSerialPort>
#include <QAbstractButton>

namespace Ui {
class ConfigureDialog;
}

class ConfigureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureDialog(QWidget *parent = 0);
    ~ConfigureDialog();

    QString portName() const;
    QSerialPort::BaudRate baudRate() const;
    QSerialPort::DataBits dataBits() const;
    QSerialPort::Parity parity() const;
    QSerialPort::StopBits stopBits() const;
    QSerialPort::FlowControl flowControl() const;

private:
    void initAvailableValues();
    void updateEnabledStates(bool enabled);

    Ui::ConfigureDialog *ui;
    QMap<QString, QSerialPort::BaudRate> m_baudRates;
    QMap<QString, QSerialPort::DataBits> m_dataBits;
    QMap<QString, QSerialPort::Parity> m_parities;
    QMap<QString, QSerialPort::StopBits> m_stopBits;
    QMap<QString, QSerialPort::FlowControl> m_flowControls;
    QTimer m_portSearchTimer;

private slots:
    void initDefaults();
    void refreshPortsList();
    void processDialogButtonClick(QAbstractButton *button);
};

#endif // CONFIGUREDIALOG_H
