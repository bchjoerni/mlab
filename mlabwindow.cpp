#include "mlabwindow.h"

mLabWindow::mLabWindow( QWidget* parent ) : QWidget( parent )
{

}

void mLabWindow::putValue( const QString& id, double value )
{
    Q_UNUSED( id );
    Q_UNUSED( value );
}

void mLabWindow::closeEvent( QCloseEvent *event )
{
    Q_UNUSED( event );
    emit closing();
}

