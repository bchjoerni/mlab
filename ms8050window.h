#ifndef MS8050WINDOW_H
#define MS8050WINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>

#include "easylogging++.h"
#include "mlabwindow.h"
#include "ms8050port.h"

namespace Ui
{
    class ms8050Window;
}

class ms8050Window : public mLabWindow
{
    Q_OBJECT

public:
    explicit ms8050Window(QWidget *parent = 0);
    ~ms8050Window();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }

private slots:
    void connectivityButtonPressed();
    void connectPort();
    void disconnectPort();
    void initFinished( const QString& idString );
    void valueUpdate( double value, char unit );
    void holdUpdate( bool hold );
    void minMaxUpdate( char minMax );
    void relativeUpdate( bool relative );
    void portError( QString error );
    void resetInfo();

private:
    void connectPortFunctions();
    void connectUiElements();
    void refreshPortList();
    void setPortEmits();

    Ui::ms8050Window *_ui;
    ms8050Port _port;
    QString _unitStr[21];
    double _lastValue;
    char _lastUnit;
};

#endif // MS8050WINDOW_H
