#include "mdihandler.h"

mdiHandler::mdiHandler( QMdiArea* mdiArea, QObject *parent )
    : QObject( parent ), _mdiArea( mdiArea ), _counter( 0.0 )
{

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

void mdiHandler::addEapsWindow( const QString &title )
{
    LOG(INFO) << "add eapsWindow";
    eapsWindow* window = new eapsWindow;
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

void mdiHandler::addUiCharWindows()
{
    const int border = 1;

    const QString s1 = "UI Characteristic";
    const QString s2 = "Heating";
    const QString s3 = "Temperature Calculator";
    const QString s4 = "UI Value Saver";
    const QString s5 = "All Value Saver";
    const QString s6 = "Graph";

    QRect r1;
    QRect r2;
    QRect r3;
    QRect r4;
    QRect r5;
    QRect r6;

    addBopmgUICharWindow(     s1 );
    addEapsWindow(            s2 );
    addCalcTemperatureWindow( s3 );
    addPairSaveWindow(        s4 );
    addAllSaveWindow(         s5 );
    addSimpleGraph(           s6 );

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
            w->setGeometry( r3.x(), border + r4.y() + r4.height(),
                            w->geometry().width(), w->geometry().height() );
            r5 = w->geometry();
        }
        else if( w->windowTitle() == s6 )
        {
            w->setGeometry( border + r3.x() + r3.width(), border,
                            r1.width()*3/2, r1.width()*3/2 );
            r6 = w->geometry();
        }
    }
}

