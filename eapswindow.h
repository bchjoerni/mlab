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
    void resetInfoLabel();

private:
    void refreshPortList();
    void addItems();

    Ui::eapsWindow *_ui;
    eapsPort _port;

    const QString VOLTAGE = "voltage";
    const QString CURRENT = "current";
    const QString POWER   = "power";
    const QString UNIT_MILLIVOLTS = "mV";
    const QString UNIT_VOLTS = "V";
    const QString UNIT_MILLIAMPERE = "mA";
    const QString UNIT_AMPERE = "A";
};

#endif // EAPSWINDOW_H
