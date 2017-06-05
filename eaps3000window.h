#ifndef EAPS3000WINDOW_H
#define EAPS3000WINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

#include "easylogging++.h"
#include "mlabwindow.h"
#include "eapsuta12port.h"

namespace Ui
{
    class eaps3000Window;
}

class eaps3000Window : public mLabWindow
{
    Q_OBJECT

public:
    explicit eaps3000Window( QWidget *parent = 0 );
    ~eaps3000Window();
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
    void setValue();
    void visibilitySelectionChanged();
    void changeVisibility();
    void setValueSelectionChanged();
    void initFinished( const QString& idString );
    void voltageUpdate( double voltage );
    void currentUpdate( double current );
    void powerUpdate( double power );
    void resistanceUpdate( double resistance );
    void portError( QString error );
    void resetRefresh();
    void clearInfo();
    void updateUnitRange();

private:
    void connectPortFunctions();
    void connectUiElements();
    void refreshPortList();
    void addItems();
    void setPortEmits();
    bool setResistanceConditionsMet();
    void showResistanceSetHint();

    Ui::eaps3000Window *_ui;
    eapsUta12Port _port;

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

#endif // EAPS3000WINDOW_H
