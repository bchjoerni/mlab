#ifndef MDIHANDLER_H
#define MDIHANDLER_H

#include <QObject>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QStringList>
#include <QIcon>
#include <QRect>

#include "easylogging++.h"
#include "bopmgwindow.h"
#include "powermetertestwindow.h"
#include "pairsavewindow.h"
#include "allsavewindow.h"
#include "eapswindow.h"
#include "simplegraphwindow.h"
#include "bopmguicharwindow.h"
#include "temperaturecalcwindow.h"
#include "tsh071window.h"
#include "elflowwindow.h"

class mdiHandler : public QObject
{
    Q_OBJECT
public:
    explicit mdiHandler( QMdiArea* mdiArea, QObject *parent );
    void doUpdates();
    void addPowerMeterTestWindow( const QString& title );
    void addPairSaveWindow( const QString& title );
    void addAllSaveWindow( const QString& title );
    void addSimpleGraph( const QString& title );
    void addCalcTemperatureWindow( const QString& title );
    void addBogMgWindow( const QString& title );
    void addEapsWindow( const QString& title );
    void addBopmgUICharWindow( const QString& title );
    void addTsh071Window( const QString& title );
    void addFlowMeterWindow( const QString& title );
    QStringList getWindowNames();
    void addUiCharWindows();

public slots:
    void windowClosed();
    void putValue( const QString& id, double value );
    void changeWindowState( const QString& id, bool okay );

signals:
    void windowNumberChanged( int change );

private:
    void addWindow( mLabWindow* window, Qt::WindowFlags flags,
                    const QString& title );

    QMdiArea* _mdiArea;
    double _counter;
};

#endif // MDIHANDLER_H
