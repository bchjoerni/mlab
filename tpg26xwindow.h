#ifndef TPG26XWINDOW_H
#define TPG26XWINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>

#include "easylogging++.h"
#include "mlabwindow.h"
#include "tpg26xport.h"

namespace Ui
{
    class tpg26xWindow;
}

class tpg26xWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit tpg26xWindow(QWidget *parent = 0);
    ~tpg26xWindow();
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
    void initFinished( const QString& idString );
    void unitUpdate( const QString& unit );
    void pressureGauge1Update( double mbarPressure );
    void pressureGauge2Update( double mbarPressure );
    void portError( QString error );
    void resetRefresh();
    void clearInfo();

private:
    void connectPortFunctions();
    void connectUiElements();
    void refreshPortList();
    void setPortEmits();

    Ui::tpg26xWindow *_ui;
    tpg26xPort _port;
    QString _unit;

    QString PR_ERRORS[8] = { "No error", "underrange!", "overrange!",
                             "sensor error!", "turned off!", "no gauge!",
                             "id error!", "unknown error!" };
    const QString PRESSURE_GAUGE_1 = "pressure gauge 1";
    const QString PRESSURE_GAUGE_2 = "pressure gauge 2";
};

#endif // TPG26XWINDOW_H
