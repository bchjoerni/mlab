#ifndef ELFLOWWINDOW_H
#define ELFLOWWINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "easylogging++.h"
#include "mlabwindow.h"
#include "elflowport.h"

namespace Ui
{
    class elFlowWindow;
}

class elFlowWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit elFlowWindow( QWidget *parent = 0 );
    ~elFlowWindow();
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
    void flowUpdate( double flow );
    void pressureUpdate( double pressure );
    void temperatureUpdate( double temperature );
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

    Ui::elFlowWindow *_ui;
    elFlowPort _port;

    const QString FLOW         = "flow";
    const QString PRESSURE     = "pressure";
    const QString TEMPERATURE  = "temperature";

    const QString UNIT_SCCM    = "sccm";
    const QString UNIT_PERCENT = "%";
    const QString UNIT_MBAR    = "mbar";
    const QString UNIT_DEGC    = "°C";
};

#endif // ELFLOWWINDOW_H
