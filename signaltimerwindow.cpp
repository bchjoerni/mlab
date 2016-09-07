#include "signaltimerwindow.h"
#include "ui_signaltimerwindow.h"

signalTimerWindow::signalTimerWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::signalTimerWindow )
{
    _ui->setupUi( this );

    _ui->btn_startStop->setText( START );

    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStop() ) );
}

signalTimerWindow::~signalTimerWindow()
{
    delete _ui;
}

void signalTimerWindow::startStop()
{
    if( _ui->btn_startStop->text() == START )
    {
        _ui->spb_ticks->setStyleSheet( "background-color: orange;" );
        _ui->spb_ticks->setEnabled( false );
        _ui->btn_startStop->setText( STOP );
    }
    else
    {
        _ui->spb_ticks->setStyleSheet( STYLE_NONE );
        _ui->btn_startStop->setText( START );
        _ui->spb_ticks->setEnabled( true );
    }
}

void signalTimerWindow::doUpdate()
{
    if( _ui->btn_startStop->text() == STOP )
    {
        if( _ui->spb_ticks->value() <= 1 )
        {
            LOG(INFO) << "timer emits signal:" << _ui->spb_signal->value();
            emit newSignal( static_cast<char>( _ui->spb_signal->value() ),
                            SIGNALCMD_VOID );
            _ui->btn_startStop->setText( START );
            _ui->spb_ticks->setEnabled( true );
            _ui->spb_ticks->setStyleSheet( STYLE_NONE );
        }
        else
        {
            _ui->spb_ticks->setValue( _ui->spb_ticks->value()-1 );
        }
    }
}

void signalTimerWindow::mLabSignal( char signal, const QString& cmd )
{
    if( signal == SHUTDOWN_SIGNAL )
    {
        _ui->btn_startStop->setText( START );
        _ui->spb_ticks->setEnabled( true );
        _ui->spb_ticks->setStyleSheet( STYLE_NONE );
    }
    else if( signal == STOP_SIGNAL )
    {
        _ui->btn_startStop->setText( START );
        _ui->spb_ticks->setEnabled( true );
        _ui->spb_ticks->setStyleSheet( STYLE_NONE );
    }
}
