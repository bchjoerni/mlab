#ifndef EAPSWINDOW_H
#define EAPSWINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "easylogging++.h"
#include "mlabwindow.h"
#include "eapsport.h"

namespace Ui
{
    class eapsWindow;
}

class eapsWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit eapsWindow( QWidget *parent = 0 );
    ~eapsWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }

private slots:
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
    void portError( QString error );
    void resetInfo();
    void updateUnitRange();
    void emitVoltageChanged();
    void emitCurrentChanged();
    void emitPowerChanged();

private:
    void connectPortFunctions();
    void connectUiElements();
    void refreshPortList();
    void addItems();    

    Ui::eapsWindow *_ui;
    eapsPort _port;

    const QString VOLTAGE          = "voltage";
    const QString CURRENT          = "current";
    const QString POWER            = "power";
    const QString POWER_BY_VOLTAGE = "power (V)";
    const QString POWER_BY_CURRENT = "power (I)";

    const QString UNIT_MILLIVOLT   = "mV";
    const QString UNIT_VOLT        = "V";
    const QString UNIT_MILLIAMPERE = "mA";
    const QString UNIT_AMPERE      = "A";
    const QString UNIT_WATT        = "W";
    const QString UNIT_MILLIWATT   = "mW";
};

#endif // EAPSWINDOW_H
