#ifndef SIGNALTIMERWINDOW_H
#define SIGNALTIMERWINDOW_H

#include "easylogging++.h"
#include "mlabwindow.h"


namespace Ui
{
    class signalTimerWindow;
}

class signalTimerWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit signalTimerWindow( QWidget *parent = 0 );
    ~signalTimerWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }
    void mLabSignal( char signal ) override;

private slots:
    void startStop();

private:
    Ui::signalTimerWindow *_ui;

    const QString START = "start";
    const QString STOP  = "stop";
};

#endif // SIGNALTIMERWINDOW_H
