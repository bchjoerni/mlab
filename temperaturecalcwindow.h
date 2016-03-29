#ifndef TEMPERATURECALCWINDOW_H
#define TEMPERATURECALCWINDOW_H

#include <cmath>

#include "easylogging++.h"
#include "mlabwindow.h"

namespace Ui
{
    class temperatureCalcWindow;
}

class temperatureCalcWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit temperatureCalcWindow( QWidget *parent = 0 );
    ~temperatureCalcWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return true;
    }
    void putValue( const QString& id, double value );

private slots:
    void startStopPressed();
    void showShareChanged();
    void measureLinearInitValue();
    void applyInitValues();

private:
    Ui::temperatureCalcWindow *_ui;
    bool _running;
    double _lastResistance;
    double _linearInitTemperature;
    double _linearInitResistance;

    const double a0 = -2.797452189966926E-8;
    const double a1 = 2.593428237203974E-10;
    const double a2 = 1.890812006620829E-14;

    const QString START = "run ";
    const QString STOP  = "stop";

    const QString RUNNING = "running";
    const QString PAUSING = "paused ";
};

#endif // TEMPERATURECALCWINDOW_H
