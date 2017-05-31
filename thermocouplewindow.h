#ifndef THERMOCOUPLEWINDOW_H
#define THERMOCOUPLEWINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>

#include "easylogging++.h"
#include "mlabwindow.h"
#include "thermocoupleport.h"

namespace Ui
{
    class thermocoupleWindow;
}

class thermocoupleWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit thermocoupleWindow(QWidget *parent = 0);
    ~thermocoupleWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }
    void mLabSignal( char signal, const QString& cmd ) override;

private slots:
    void connectivityButtonPressed();
    void connectPort();
    void disconnectPort();
    void initFinished( const QString& idString );
    void ambientTemperatureUpdate( double degreeCelsius );
    void probeTemperatureUpdate( double degreeCelsius );
    void portError( QString error );
    void resetInfo();

private:
    void connectPortFunctions();
    void connectUiElements();
    void refreshPortList();
    void setPortEmits();

    Ui::thermocoupleWindow *_ui;
    thermocouplePort _port;

    const QString TEMPERATURE_AMBIENT = "ambient temperature";
    const QString TEMPERATURE_PROBE   = "probe temperature";
    const QString UNIT_CELSIUS = "°C";
};

#endif // THERMOCOUPLEWINDOW_H
