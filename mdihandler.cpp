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

void mdiHandler::addPowerMeterTestWindow( const QString &title )
{
    powerMeterTestWindow* pmtw = new powerMeterTestWindow;
    addWindow( pmtw, pmtw->windowFlags(), title );
}

void mdiHandler::addPairSaveWindow( const QString &title )
{
    pairSaveWindow* psw = new pairSaveWindow;
    addWindow( psw, psw->windowFlags(), title );
}

void mdiHandler::addBogMgWindow( const QString& title )
{
    bopMgWindow* window = new bopMgWindow;
    addWindow( window, window->windowFlags(), title );
}

void mdiHandler::addEapsWindow( const QString &title )
{
    eapsWindow* window = new eapsWindow;
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
}

void mdiHandler::windowClosed()
{
    emit windowNumberChanged( -1 );
}
