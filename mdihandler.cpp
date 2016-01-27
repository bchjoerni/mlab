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
