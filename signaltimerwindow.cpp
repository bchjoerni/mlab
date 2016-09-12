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
            QString receiver = _ui->txt_window->text();
            if( _ui->rad_allWindows->isChecked() )
            {
                receiver = SIGNAL_RECEIVER_ALL;
            }
            emit newSignal( receiver,
                            static_cast<char>( _ui->spb_signal->value() ),
                            _ui->txt_cmd->text() );
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
    if( signal == SIGNAL_SHUTDOWN )
    {
        _ui->btn_startStop->setText( START );
        _ui->spb_ticks->setEnabled( true );
        _ui->spb_ticks->setStyleSheet( STYLE_NONE );
    }
    else if( signal == SIGNAL_STOP )
    {
        _ui->btn_startStop->setText( START );
        _ui->spb_ticks->setEnabled( true );
        _ui->spb_ticks->setStyleSheet( STYLE_NONE );
    }
}
