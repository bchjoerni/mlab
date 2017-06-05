#ifndef EAPS8000USBUICHARWINDOW_H
#define EAPS8000USBUICHARWINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

#include <cmath>

#include "easylogging++.h"
#include "eaps8000usbport.h"
#include "mlabwindow.h"


namespace Ui
{
    class eaps8000UsbUICharWindow;
}

class eaps8000UsbUICharWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit eaps8000UsbUICharWindow(QWidget *parent = 0);
    ~eaps8000UsbUICharWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }
    void mLabSignal( const QString& cmd ) override;

private slots:
    void connectivityButtonPressed();
    void connectPort();
    void disconnectPort();
    void setValueSelectionChanged();
    void initFinished( const QString& idString );
    void voltageUpdate( double voltage );
    void currentUpdate( double current );
    void powerUpdate( double power);
    void portError( QString error );
    void resetRefresh();
    void clearInfo();
    void updateUnitRange();
    void fixStepSizeChanged();
    void calculateRemainingTicks();
    void startStop();
    void emergencyStop();

private:
    void connectPortFunctions();
    void connectUiElements();
    void refreshPortList();
    void addItems();
    void setValues();
    void uiCharFinished();
    bool endOfLoop();
    bool inLoopInterval();

    Ui::eaps8000UsbUICharWindow *_ui;
    eaps8000UsbPort _port;
    int _tickCounter;
    int _loopCounter;
    bool _running;
    bool _increasing;
    double _setUiValue;
    double _lastMeasuredValue;

    const QString VOLTAGE          = "voltage";
    const QString CURRENT          = "current";
    const QString POWER            = "power";

    const QString START            = "start";
    const QString STOP             = "stop";

    const QString UNIT_MILLIVOLT   = "mV";
    const QString UNIT_VOLT        = "V";
    const QString UNIT_MILLIAMPERE = "mA";
    const QString UNIT_AMPERE      = "A";
    const QString UNIT_WATT        = "W";
    const QString UNIT_MILLIWATT   = "mW";
};

#endif // EAPS8000USBUICHARWINDOW_H
