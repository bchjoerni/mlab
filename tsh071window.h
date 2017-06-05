#ifndef TSH071WINDOW_H
#define TSH071WINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>

#include "easylogging++.h"
#include "mlabwindow.h"
#include "tsh071port.h"


namespace Ui
{
    class tsh071Window;
}

class tsh071Window : public mLabWindow
{
    Q_OBJECT

public:
    explicit tsh071Window( QWidget *parent = 0 );
    ~tsh071Window();
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
    void setRotation();
    void visibilitySelectionChanged();
    void changeVisibility();
    void initFinished( const QString& idString );
    void portError( QString error );
    void resetRefresh();
    void clearInfo();
    void rpmUpdate( int rpm );
    void pressureUpdate( double pressure );
    void startPump();
    void stopPump();

private:
    void connectPortFunctions();
    void connectUiElements();
    void refreshPortList();
    void addItems();
    void setPortEmits();

    Ui::tsh071Window *_ui;
    tsh071Port _port;

    const QString ROTATION = "rotation";
    const QString PRESSURE = "pressure";
};

#endif // TSH071WINDOW_H
