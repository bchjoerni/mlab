#ifndef POWERMETERTESTWINDOW_H
#define POWERMETERTESTWINDOW_H

#include <QSerialPortInfo>
#include <QDebug>

#include <random>
#include <memory>

#include "easylogging++.h"
#include "mlabwindow.h"

namespace Ui
{
    class powerMeterTestWindow;
}

class powerMeterTestWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit powerMeterTestWindow( QWidget *parent = 0 );
    ~powerMeterTestWindow();
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
    void clearInfo();

private:
    void refreshPortList();
    void addItems();
    void setInit( bool init )
    {
        _init = init;
    }
    bool getInit() const
    {
        return _init;
    }

    Ui::powerMeterTestWindow *_ui;
    QString _setValueStr;
    double _setValue;
    std::unique_ptr<std::mt19937> _randomGenerator;
    bool _init;

    const QString VOLTAGE = "voltage";
    const QString CURRENT = "current";
    const QString POWER   = "power";
};

#endif // POWERMETERTESTWINDOW_H
