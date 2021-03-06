#ifndef BOPMGUICHARWINDOW_H
#define BOPMGUICHARWINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

#include <cmath>

#include "easylogging++.h"
#include "bopmgport.h"
#include "mlabwindow.h"


namespace Ui
{
    class bopmgUICharWindow;
}

class bopmgUICharWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit bopmgUICharWindow(QWidget *parent = 0);
    ~bopmgUICharWindow();
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

    Ui::bopmgUICharWindow *_ui;
    bopmgPort _port;
    int _tickCounter;
    int _loopCounter;
    bool _running;
    bool _increasing;
    double _setUiValue;
    double _lastMeasuredValue;

    const QString VOLTAGE          = "voltage";
    const QString CURRENT          = "current";

    const QString START            = "start";
    const QString STOP             = "stop";

    const QString UNIT_MILLIVOLT   = "mV";
    const QString UNIT_VOLT        = "V";
    const QString UNIT_MILLIAMPERE = "mA";
    const QString UNIT_AMPERE      = "A";
};


#endif // BOPMGUICHARWINDOW_H
