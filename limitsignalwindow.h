#ifndef LIMITSIGNALWINDOW_H
#define LIMITSIGNALWINDOW_H

#include <QString>
#include <QFileDialog>
#include <QDateTime>

#include <fstream>
#include <vector>
#include <string>

#include "easylogging++.h"
#include "mlabwindow.h"


namespace Ui
{
    class limitSignalWindow;
}

class limitSignalWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit limitSignalWindow( QWidget *parent = 0 );
    ~limitSignalWindow();
    void doUpdate() override
    {
    }
    bool isReceiver() const override
    {
        return true;
    }
    void putValue( const QString& id, double value ) override;
    void mLabSignal( const QString& cmd ) override;

private slots:
    void startStop();

private:
    Ui::limitSignalWindow *_ui;

    const QString START = "start";
    const QString STOP  = "stop ";

    const QString RUNNING   = "running";
    const QString PAUSING   = "pausing";
};


#endif // LIMITSIGNALWINDOW_H
