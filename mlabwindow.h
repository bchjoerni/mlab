#ifndef MLABWINDOW_H
#define MLABWINDOW_H

#include <QWidget>
#include <QString>
#include <QCloseEvent>

#include "easylogging++.h"

class mLabWindow : public QWidget
{
    Q_OBJECT

public:
    explicit mLabWindow( QWidget* parent = 0 );
    virtual void doUpdate() = 0;
    virtual bool isReceiver() const = 0;
    virtual void putValue( const QString& id, double value );
    virtual void mLabSignal( char signal )
    {
    }

protected:
    const char SHUTDOWN_SIGNAL = 0; // is hard coded in mdi handler !!!
    const char STOP_SIGNAL     = 1;

    const QString EMERGENCY_STOP     = "Emergency stop!";
    const QString STOP_RECEIVED      = "stop signal";
    const QString NO_PORTS_AVAILABLE = "No ports available!";
    const QString SHOW               = "show";
    const QString HIDE               = "hide";
    const QString CONNECT_PORT       = "connect";
    const QString DISCONNECT_PORT    = "disconnect";
    const QString CONNECTED          = "connected";
    const QString NOT_CONNECTED      = "not connected";
    const QString STYLE_OK           = "color: green;";
    const QString STYLE_ERROR        = "color: red;";

    void closeEvent( QCloseEvent* event );

signals:
    void newValue( QString id, double value );
    void newString( QString id, QString string );
    void newSignal( char signal );
    void closing();
    void changeWindowState( QString id, bool okay );
};

#endif // MLABWINDOW_H
