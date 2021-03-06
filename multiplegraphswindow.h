#ifndef MULTIPLEGRAPHSWINDOW_H
#define MULTIPLEGRAPHSWINDOW_H

#include <QString>
#include <QColor>

#include <vector>
#include <map>

#include "mlabwindow.h"
#include "multiplegraphswidget.h"


namespace Ui
{
    class multipleGraphsWindow;
}

class multipleGraphsWindow  : public mLabWindow
{
    Q_OBJECT

public:
    explicit multipleGraphsWindow( QWidget *parent = 0 );
    ~multipleGraphsWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return true;
    }
    void putValue( const QString& id, double value ) override;
    void mLabSignal( const QString& cmd ) override;

private slots:
    void startStop();
    void clearData();
    void addSensor();
    void deleteSensors();
    void drawSettingsChanged();
    void normalizedChanged();
    void yAxisChanged();

private:
    void updateSensorColors();

    Ui::multipleGraphsWindow *_ui;

    const QString RUNNING = "drawing";
    const QString PAUSING = "pausing";

    const QString START = "start drawing";
    const QString STOP  = "stop drawing";

    const QString AXIS_LINEAR = "linear";
    const QString AXIS_LOG    = "logarithmic";
};

#endif // MULTIPLEGRAPHSWINDOW_H
