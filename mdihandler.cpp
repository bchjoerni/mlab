#include "mdihandler.h"

mdiHandler::mdiHandler( QMdiArea* mdiArea, QObject *parent )
    : QObject( parent ), _mdiArea( mdiArea ), _counter( 0.0 )
{

}

void mdiHandler::emergencyStop()
{
    auto subWindows = _mdiArea->subWindowList();
    for( QMdiSubWindow* window : subWindows )
    {
        mLabWindow* labWindow = qobject_cast<mLabWindow*>( window->widget() );
        labWindow->mLabSignal( SIGNAL_SHUTDOWN, SIGNAL_CMD_VOID );
    }
}

void mdiHandler::doUpdates()
{
    auto subWindows = _mdiArea->subWindowList();
    for( QMdiSubWindow* window : subWindows )
    {
        mLabWindow* labWindow = qobject_cast<mLabWindow*>( window->widget() );
        labWindow->doUpdate();
        if( labWindow->isReceiver() )
        {
            labWindow->putValue( "counter", _counter );
        }
    }
    _counter++;
}

void mdiHandler::putValue( const QString &id, double value )
{
    auto subWindows = _mdiArea->subWindowList();
    for( QMdiSubWindow* window : subWindows )
    {
        mLabWindow* labWindow = qobject_cast<mLabWindow*>( window->widget() );
        if( labWindow->isReceiver() )
        {
            labWindow->putValue( id, value );
        }
    }
}

void mdiHandler::windowError( const QString &errorMsg )
{
    auto subWindows = _mdiArea->subWindowList();
    for( QMdiSubWindow* window : subWindows )
    {
        mLabWindow* labWindow = qobject_cast<mLabWindow*>( window->widget() );
        if( labWindow->getTitle().startsWith( ERROR_LOG_WINDOW_TITLE ) )
        {
            qobject_cast<errorLogWindow*>( labWindow )->mLabError(
                        labWindow->getTitle() + ": " + errorMsg );
        }
    }
}

void mdiHandler::mLabSignal( const QString& receiver, char signal,
                             const QString& cmd )
{
    auto subWindows = _mdiArea->subWindowList();
    for( QMdiSubWindow* window : subWindows )
    {
        mLabWindow* labWindow = qobject_cast<mLabWindow*>( window->widget() );

        if( receiver == SIGNAL_RECEIVER_ALL
                || receiver == labWindow->getTitle() )
        {
            labWindow->mLabSignal( signal, cmd );
        }
    }
}

void mdiHandler::changeWindowState( const QString &id, bool okay )
{
    auto subWindows = _mdiArea->subWindowList();
    for( QMdiSubWindow* window : subWindows )
    {
        if( window->windowTitle() == id )
        {
            if( okay )
            {
                window->setWindowIcon(
                            QIcon( ":/images/images/greenDot.png" ) );
            }
            else
            {
                window->setWindowIcon( QIcon( ":/images/images/redDot.png" ) );
            }
            break;
        }
    }
}

void mdiHandler::addGeneralPortWindow( const QString &title )
{
    LOG(INFO) << "add generalPortWindow";
    generalPortWindow* window = new generalPortWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addPowerMeterTestWindow( const QString &title )
{
    LOG(INFO) << "add powerMeterTestWindow";
    powerMeterTestWindow* window = new powerMeterTestWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addPairSaveWindow( const QString &title )
{
    LOG(INFO) << "add pairSaveWindow";
    pairSaveWindow* window = new pairSaveWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addAllSaveWindow( const QString &title )
{
    LOG(INFO) << "add allSaveWindow";
    allSaveWindow* window = new allSaveWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addSimpleGraph( const QString &title )
{
    LOG(INFO) << "add simpleGraph";
    simpleGraphWindow* window = new simpleGraphWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addCalcTemperatureWindow( const QString &title )
{
    LOG(INFO) << "add addCalcTemperature";
    temperatureCalcWindow* window = new temperatureCalcWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addKeithley2400Window( const QString& title )

{
    LOG(INFO) << "add keithley2400Window";
    keithley2400Window* window = new keithley2400Window;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addBogMgWindow( const QString& title )

{
    LOG(INFO) << "add bogMgWindow";
    bopmgWindow* window = new bopmgWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addBopmgUICharWindow( const QString& title )
{
    LOG(INFO) << "add bopmgUICharWindow";
    bopmgUICharWindow* window = new bopmgUICharWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addEaps3000Window( const QString &title )
{
    LOG(INFO) << "add eaps3000Window";
    eaps3000Window* window = new eaps3000Window;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addEaps3000UICharWindow( const QString &title )
{
    LOG(INFO) << "add eaps3000Window";
    eaps3000UICharWindow* window = new eaps3000UICharWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addEaps8000UsbWindow( const QString &title )
{
    LOG(INFO) << "add eaps8000UsbWindow";
    eaps8000UsbWindow* window = new eaps8000UsbWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addEaps8000UsbUICharWindow( const QString &title )
{
    LOG(INFO) << "add eapsWindow";
    eaps8000UsbUICharWindow* window = new eaps8000UsbUICharWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addTsh071Window( const QString &title )
{
    LOG(INFO) << "add tsh071Window";
    tsh071Window* window = new tsh071Window;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addFlowMeterWindow( const QString& title )
{
    LOG(INFO) << "add flowMeterWindow";
    elFlowWindow* window = new elFlowWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addThermocoupleWindow( const QString& title )
{
    LOG(INFO) << "add thermocoupleWindow";
    thermocoupleWindow* window = new thermocoupleWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addMs8050Window( const QString& title )
{
    LOG(INFO) << "add MS8050Window";
    ms8050Window* window = new ms8050Window;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addScratchpadWindow( const QString &title )
{
    LOG(INFO) << "add scratchpadWindow";
    scratchpadWindow* window = new scratchpadWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addScreenshotWindow( const QString &title )
{
    LOG(INFO) << "add screenshotWindow";
    screenshotWindow* window = new screenshotWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addNetworkWindow( const QString &title )
{
    LOG(INFO) << "add networkWindow";
    networkWindow* window = new networkWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addTpg26xWindow( const QString &title )
{
    LOG(INFO) << "add tpg26xWindow";
    tpg26xWindow* window = new tpg26xWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addSignalTimer( const QString &title )
{
    LOG(INFO) << "add signalTimerWindow";
    signalTimerWindow* window = new signalTimerWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addLimitSignalWindow( const QString &title )
{
    LOG(INFO) << "add limitSignalWindow";
    limitSignalWindow* window = new limitSignalWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addErrorLogWindow( const QString &title )
{
    LOG(INFO) << "add errorLogWindow";
    errorLogWindow* window = new errorLogWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addWindow( mLabWindow* window, Qt::WindowFlags flags,
                            const QString& title )
{
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowMaximizeButtonHint;

    _mdiArea->addSubWindow( window, flags );
    window->setWindowTitle( title );
    window->show();
    emit windowNumberChanged( 1 );
    connect( window, SIGNAL( closing() ), this, SLOT( windowClosed() ) );
    connect( window, SIGNAL( newValue( QString, double ) ), this,
             SLOT( putValue( QString, double ) ) );
    connect( window, SIGNAL( newSignal( QString, char, QString ) ), this,
             SLOT( mLabSignal( QString, char, QString ) ) );
    connect( window, SIGNAL( newError( QString ) ), this,
             SLOT( windowError( QString ) ) );
    connect( window, SIGNAL( changeWindowState( QString, bool ) ), this,
             SLOT( changeWindowState( QString, bool ) ) );
}

QStringList mdiHandler::getWindowNames()
{
    QStringList names;

    auto subWindows = _mdiArea->subWindowList();
    for( QMdiSubWindow* window : subWindows )
    {
        names.push_back( window->windowTitle() );
    }

    return names;
}

void mdiHandler::windowClosed()
{
    LOG(INFO) << "window closed";
    emit windowNumberChanged( -1 );
}

void mdiHandler::addUICharWindowsNew()
{
    const QString s11  = "UI Characteristic";
    const QString s12  = "Thermocouple 1";
    const QString s13  = "Thermocouple 2";
    const QString s21  = "Heating";
    const QString s22  = "MS 8050";
    const QString s23  = "Pressure Gauge";
    const QString s31  = "All value saver";
    const QString s32  = "Pair value saver";
    const QString s33  = "screenshot";
    const QString s34  = "scratchpad";
    const QString s41  = ERROR_LOG_WINDOW_TITLE;
    const QString s42  = "network remote";
    const QString s51  = "signal timer";
    const QString s52  = "simple graph";

    addEaps8000UsbUICharWindow( s11 );
    addThermocoupleWindow(      s12 );
    addThermocoupleWindow(      s13 );
    addEaps8000UsbWindow(       s21 );
    addMs8050Window(            s22 );
    addTpg26xWindow(            s23 );
    addAllSaveWindow(           s31 );
    addPairSaveWindow(          s32 );
    addScreenshotWindow(        s33 );
    addScratchpadWindow(        s34 );
    addErrorLogWindow(          s41 );
    addNetworkWindow(           s42 );
    addSignalTimer(             s51 );
    addSimpleGraph(             s52 );

    std::vector<QString> c1 { s11, s12, s13 };
    std::vector<QString> c2 { s21, s22, s23 };
    std::vector<QString> c3 { s31, s32, s33, s34 };
    std::vector<QString> c4 { s41, s42 };
    std::vector<QString> c5 { s51, s52 };
    std::vector<std::vector<QString> > windows { c1, c2, c3, c4, c5 };

    positionWindows( windows );
}



void mdiHandler::addUICharWindowsOld()
{
    const int border = 1;

    const QString s1  = "UI Characteristic";
    const QString s2  = "Heating";
    const QString s3  = "Thermocouple 1";
    const QString s4  = "Thermocouple 2";
    const QString s5  = "Temperature Calculator";
    const QString s6  = "UI Value Saver";
    const QString s7  = "All Value Saver";
    const QString s8  = "Screenshot";
    const QString s9  = "Scratchpad";
    const QString s10 = "Graph";

    QRect r1;
    QRect r2;
    QRect r3;
    QRect r4;
    QRect r5;
    QRect r6;
    QRect r7;
    QRect r8;
    QRect r9;
    QRect r10;

    addBopmgUICharWindow(     s1 );
    addEaps3000Window(        s2 );
    addThermocoupleWindow(    s3 );
    addThermocoupleWindow(    s4 );
    addCalcTemperatureWindow( s5 );
    addPairSaveWindow(        s6 );
    addAllSaveWindow(         s7 );
    addScreenshotWindow(      s8 );
    addScratchpadWindow(      s9 );
    addSimpleGraph(           s10 );

    auto subWindows = _mdiArea->subWindowList();
    for( QMdiSubWindow* w : subWindows )
    {
        if( w->windowTitle() == s1 )
        {
            w->setGeometry( border, border,
                            w->geometry().width(), w->geometry().height() );
            r1 = w->geometry();
        }
        else if( w->windowTitle() == s2 )
        {
            w->setGeometry( r1.x(), border*2 + r1.height(),
                            w->geometry().width(), w->geometry().height() );
            r2 = w->geometry();
        }
        else if( w->windowTitle() == s3 )
        {
            w->setGeometry( border*2 + (r1.width() > r2.width() ?
                                            r1.width() : r2.width()), border,
                            w->geometry().width(), w->geometry().height() );
            r3 = w->geometry();
        }
        else if( w->windowTitle() == s4 )
        {
            w->setGeometry( r3.x(), border*2 + r3.height(),
                            w->geometry().width(), w->geometry().height() );
            r4 = w->geometry();
        }
        else if( w->windowTitle() == s5 )
        {
            w->setGeometry( r3.x(), border*3 + r3.height() + r4.height(),
                            w->geometry().width(), w->geometry().height() );
            r5 = w->geometry();
        }
        else if( w->windowTitle() == s6 )
        {
            w->setGeometry( r3.x() + border +
                            (r3.width() > r5.width() ? r3.width() : r5.width()),
                            border,
                            w->geometry().width(), w->geometry().height() );
            r6 = w->geometry();
        }
        else if( w->windowTitle() == s7 )
        {
            w->setGeometry( r6.x(), border*2 + r6.height(),
                            w->geometry().width(), w->geometry().height() );
            r7 = w->geometry();
        }
        else if( w->windowTitle() == s8 )
        {
            w->setGeometry( r6.x(), border*3 + r6.height() + r7.height(),
                            w->geometry().width(), w->geometry().height() );
            r8 = w->geometry();
        }
        else if( w->windowTitle() == s9 )
        {
            w->setGeometry( r6.x(), r8.y() + r8.height() + border,
                            w->geometry().width(), w->geometry().height() );
            r9 = w->geometry();
        }
        else if( w->windowTitle() == s10 )
        {
            w->setGeometry( r6.x() + border + r6.width(), border,
                            r2.width(), r2.height() );
            r10 = w->geometry();
        }
    }
}

void mdiHandler::positionWindows( const std::vector<std::vector<QString> >&
                                   windows )
{
    int border = 1;
    int xPos = border;
    int yPos = border;
    int xPosChange = 0;
    auto subWindows = _mdiArea->subWindowList();

    for( unsigned int i = 0; i < windows.size(); i++ )
    {
        for( unsigned int k = 0; k < windows[i].size(); k++ )
        {
            for( QMdiSubWindow* w : subWindows )
            {
                if( w->windowTitle() == windows[i][k] )
                {
                    w->setGeometry( xPos, yPos, w->width(), w->height() );
                    if( w->width() > xPosChange )
                    {
                        xPosChange = w->width();
                    }
                    yPos += w->height() + border;
                }
            }
        }
        xPos += xPosChange + border;
        xPosChange = 0;
        yPos = border;
    }
}

