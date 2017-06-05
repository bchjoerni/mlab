#ifndef KEITHLEY2400WINDOW_H
#define KEITHLEY2400WINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "easylogging++.h"
#include "mlabwindow.h"
#include "keithley2400port.h"

namespace Ui
{
    class keithley2400Window;
}

class keithley2400Window : public mLabWindow
{
    Q_OBJECT

public:
    explicit keithley2400Window( QWidget *parent = 0 );
    ~keithley2400Window();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }
    void mLabSignal( const QString& cmd ) override;

private slots:
    void emergencyStop();
    void connectivityButtonPressed();
    void connectPort();
    void disconnectPort();
    void initFinished( const QString& idString );
    void resistanceUpdate( double resistance );
    void portError( QString error );
    void resetRefresh();
    void clearInfo();

private:
    void connectPortFunctions();
    void connectUiElements();
    void refreshPortList();
    void setPortEmits();

    Ui::keithley2400Window *_ui;
    keithley2400Port _port;

    const QString VOLTAGE               = "voltage";
    const QString CURRENT               = "current";
    const QString POWER                 = "power";
    const QString RESISTANCE            = "resistance";
    const QString POWER_BY_VOLTAGE      = "power (V)";
    const QString POWER_BY_CURRENT      = "power (I)";
    const QString RESISTANCE_BY_VOLTAGE = "resistance (V)";
    const QString RESISTANCE_BY_CURRENT = "resistance (I)";

    const QString UNIT_MILLIVOLT   = "mV";
    const QString UNIT_VOLT        = "V";
    const QString UNIT_MILLIAMPERE = "mA";
    const QString UNIT_AMPERE      = "A";
    const QString UNIT_WATT        = "W";
    const QString UNIT_MILLIWATT   = "mW";
    const QString UNIT_OHM         = "Ω";
};

#endif // KEITHLEY2400WINDOW_H
