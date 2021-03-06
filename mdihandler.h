#ifndef MDIHANDLER_H
#define MDIHANDLER_H

#include <QObject>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QStringList>
#include <QIcon>
#include <QRect>

#include <vector>

#include "easylogging++.h"
#include "generalportwindow.h"
#include "bopmgwindow.h"
#include "powermetertestwindow.h"
#include "pairsavewindow.h"
#include "allsavewindow.h"
#include "eaps3000window.h"
#include "eaps3000uicharwindow.h"
#include "simplegraphwindow.h"
#include "multiplegraphswindow.h"
#include "bopmguicharwindow.h"
#include "keithley2400window.h"
#include "temperaturecalcwindow.h"
#include "tsh071window.h"
#include "elflowwindow.h"
#include "thermocouplewindow.h"
#include "scratchpadwindow.h"
#include "screenshotwindow.h"
#include "networkwindow.h"
#include "eaps8000usbwindow.h"
#include "eaps8000usbuicharwindow.h"
#include "ms8050window.h"
#include "tpg26xwindow.h"
#include "signaltimerwindow.h"
#include "limitsignalwindow.h"
#include "errorlogwindow.h"

const QString ERROR_LOG_WINDOW_TITLE = "error logger";

class mdiHandler : public QObject

{
    Q_OBJECT
public:
    explicit mdiHandler( QMdiArea* mdiArea, QObject *parent );
    void emergencyStop();
    void doUpdates();
    void addGeneralPortWindow( const QString& title );
    void addPowerMeterTestWindow( const QString& title );
    void addPairSaveWindow( const QString& title );
    void addAllSaveWindow( const QString& title );
    void addSimpleGraph( const QString& title );
    void addMultipleGraphs( const QString& title );
    void addCalcTemperatureWindow( const QString& title );
    void addBogMgWindow( const QString& title );
    void addKeithley2400Window( const QString& title );
    void addEaps3000Window( const QString& title );
    void addEaps3000UICharWindow( const QString& title );
    void addEaps8000UsbWindow( const QString& title );
    void addEaps8000UsbUICharWindow( const QString& title );
    void addBopmgUICharWindow( const QString& title );
    void addTsh071Window( const QString& title );
    void addFlowMeterWindow( const QString& title );
    void addThermocoupleWindow( const QString& title );
    void addMs8050Window( const QString& title );
    void addScratchpadWindow( const QString& title );
    void addScreenshotWindow( const QString& title );
    void addNetworkWindow( const QString& title );
    void addTpg26xWindow( const QString& title );
    void addSignalTimer( const QString& title );
    void addLimitSignalWindow( const QString& title );
    void addErrorLogWindow( const QString& title );
    QStringList getWindowNames();
    void addUICharWindowsNew();
    void addUICharWindowsOld();

public slots:
    void windowClosed();
    void windowError( const QString& errorMsg );
    void putValue( const QString& id, double value );
    void mLabSignal( const QString& receiver, const QString& cmd );
    void changeWindowState( const QString& id, bool okay );

signals:
    void windowNumberChanged( int change );

private:
    void addWindow( mLabWindow* window, Qt::WindowFlags flags,
                    const QString& title );
    void positionWindows( const std::vector<std::vector<QString> >& windows );

    QMdiArea* _mdiArea;
    double _counter;
};

#endif // MDIHANDLER_H
