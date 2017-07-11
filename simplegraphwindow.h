#ifndef SIMPLEGRAPHWINDOW_H
#define SIMPLEGRAPHWINDOW_H

#include <QString>

#include <vector>
#include <map>

#include "mlabwindow.h"
#include "simplegraphwidget.h"

namespace Ui
{
    class simpleGraphWindow;
}

class simpleGraphWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit simpleGraphWindow( QWidget *parent = 0 );
    ~simpleGraphWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return true;
    }
    void putValue( const QString& id, double value ) override;
    void mLabSignal( const QString& cmd ) override;

private slots:
    void startStop();
    void clearGraph();
    void axisChanged();
    void xAxisTypeChanged();
    void yAxisTypeChanged();

private:
    void addAxisTypes();

    Ui::simpleGraphWindow *_ui;

    const QString RUNNING = "drawing";
    const QString PAUSING = "pausing";

    const QString START = "start drawing";
    const QString STOP  = "stop drawing";

    const QString AXIS_LINEAR = "linear";
    const QString AXIS_LOG10  = "log_10";
    const QString AXIS_LN     = "ln";
};

#endif // SIMPLEGRAPHWINDOW_H
