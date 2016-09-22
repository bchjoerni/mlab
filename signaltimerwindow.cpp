#include "signaltimerwindow.h"
#include "ui_signaltimerwindow.h"

signalTimerWindow::signalTimerWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::signalTimerWindow ), _ticks( 60 )
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
        _ticks = _ui->spb_ticks->value();
    }
    else
    {
        _ui->spb_ticks->setStyleSheet( STYLE_NONE );
        _ui->btn_startStop->setText( START );
        _ui->spb_ticks->setEnabled( true );
        _ui->spb_ticks->setValue( _ticks );
    }
}

void signalTimerWindow::doUpdate()
{
    if( _ui->btn_startStop->text() == STOP )
    {
        if( _ui->spb_ticks->value() <= 1 )
        {
            LOG(INFO) << "signal timer emits: "
                      << _ui->txt_window->text().toStdString() << ", "
                      << _ui->spb_signal->value() << ", "
                      << _ui->txt_cmd->text().toStdString();
            emit newSignal( _ui->txt_window->text(),
                            static_cast<char>( _ui->spb_signal->value() ),
                            _ui->txt_cmd->text() );

            if( _ui->rad_singleShot->isChecked() )
            {
                _ui->btn_startStop->setText( START );
                _ui->spb_ticks->setEnabled( true );
                _ui->spb_ticks->setStyleSheet( STYLE_NONE );
            }
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
