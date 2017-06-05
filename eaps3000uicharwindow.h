#ifndef EAPS3000UICHARWINDOW_H
#define EAPS3000UICHARWINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

#include <cmath>

#include "easylogging++.h"
#include "eapsuta12port.h"
#include "mlabwindow.h"


namespace Ui
{
    class eaps3000UICharWindow;
}

class eaps3000UICharWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit eaps3000UICharWindow(QWidget *parent = 0);
    ~eaps3000UICharWindow();
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
    void powerUpdate( double power );
    void resistanceUpdate( double resistance );
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

    Ui::eaps3000UICharWindow *_ui;
    eapsUta12Port _port;
    int _tickCounter;
    int _loopCounter;
    bool _running;
    bool _increasing;
    double _setUiValue;
    double _lastMeasuredValue;

    const QString VOLTAGE          = "voltage";
    const QString CURRENT          = "current";
    const QString POWER            = "power";
    const QString RESISTANCE       = "resistance";

    const QString START            = "start";
    const QString STOP             = "stop";

    const QString UNIT_MILLIVOLT   = "mV";
    const QString UNIT_VOLT        = "V";
    const QString UNIT_MILLIAMPERE = "mA";
    const QString UNIT_AMPERE      = "A";
    const QString UNIT_WATT        = "W";
    const QString UNIT_MILLIWATT   = "mW";
    const QString UNIT_RESISTANCE  = "Ω";
};

#endif // EAPS3000UICHARWINDOW_H
