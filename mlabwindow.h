#ifndef MLABWINDOW_H
#define MLABWINDOW_H

#include <QWidget>
#include <QString>
#include <QCloseEvent>

#include "easylogging++.h"

const QString SIGNAL_CMD_VOID = "";
const QString SIGNAL_RECEIVER_ALL = "__ALL__";

class mLabWindow : public QWidget
{
    Q_OBJECT

public:
    explicit mLabWindow( QWidget* parent = 0 );
    virtual void doUpdate() = 0;
    virtual bool isReceiver() const = 0;
    virtual void putValue( const QString& id, double value );
    virtual void mLabSignal( const QString& cmd )
    {
        Q_UNUSED( cmd )
    }
    QString getTitle() const;

protected:
    const QString EMERGENCY_STOP     = "btn_emergencystop\tpress";
    const QString STOP_SIGNAL        = "stop_signal";
    const QString STOP_INFO_TEXT     = "stop signal received";
    const QString NO_PORTS_AVAILABLE = "No ports available!";
    const QString SHOW               = "show";
    const QString HIDE               = "hide";
    const QString CONNECT_PORT       = "connect";
    const QString DISCONNECT_PORT    = "disconnect";
    const QString CONNECTED          = "connected";
    const QString NOT_CONNECTED      = "not connected";
    const QString STYLE_NONE         = "";
    const QString STYLE_OK           = "color: green;";
    const QString STYLE_ERROR        = "color: red;";

    void closeEvent( QCloseEvent* event );

signals:
    void newValue( QString id, double value );
    void newString( QString id, QString string );
    void newSignal( QString receiver, QString cmd );
    void newError( QString errorMsg );
    void closing();
    void changeWindowState( QString id, bool okay );
};

#endif // MLABWINDOW_H
