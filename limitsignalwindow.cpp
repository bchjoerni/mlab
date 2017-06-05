#include "limitsignalwindow.h"
#include "ui_limitsignalwindow.h"


limitSignalWindow::limitSignalWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::limitSignalWindow )
{
    _ui->setupUi( this );

    _ui->btn_startStop->setText( START );

    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStop() ) );
}

limitSignalWindow::~limitSignalWindow()
{
    delete _ui;
}

void limitSignalWindow::startStop()
{
    if( _ui->btn_startStop->text() == START )
    {
        _ui->lbl_status->setStyleSheet( STYLE_OK );
        _ui->lbl_status->setText( RUNNING );
        _ui->btn_startStop->setText( STOP );
    }
    else
    {
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        _ui->lbl_status->setText( PAUSING );
        _ui->btn_startStop->setText( START );
    }
}

void limitSignalWindow::mLabSignal( const QString& cmd )
{
    QString cmdLower = cmd.toLower().trimmed();

    if( cmdLower == EMERGENCY_STOP.toLower() )
    {
        if( _ui->btn_startStop->text() == STOP )
        {
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->lbl_status->setText( PAUSING );
            _ui->btn_startStop->setText( START );
        }
    }
    else if( cmdLower == STOP_SIGNAL.toLower() )
    {
        if( _ui->btn_startStop->text() == STOP
                && _ui->chb_setZeroAtStopSignal->isChecked() )
        {
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->lbl_status->setText( PAUSING );
            _ui->btn_startStop->setText( START );
        }
    }
}

void limitSignalWindow::putValue( const QString &id, double value )
{
    if( _ui->btn_startStop->text() == STOP
            && _ui->cob_limit->currentText() == id )
    {
        if( _ui->rad_greater->isChecked()
                && value > _ui->dsb_limit->value() )
        {
            LOG(INFO) << "limit signal window emits: "
                      << _ui->txt_window->text().toStdString() << ", "
                      << _ui->txt_cmd->text().toStdString();
            emit newSignal( _ui->txt_window->text(), _ui->txt_cmd->text() );
        }
        else if( _ui->rad_lesser->isChecked()
                 && value < _ui->dsb_limit->value() )
        {
            LOG(INFO) << "limit signal window emits: "
                      << _ui->txt_window->text().toStdString() << ", "
                      << _ui->txt_cmd->text().toStdString();
            emit newSignal( _ui->txt_window->text(), _ui->txt_cmd->text() );
        }
    }

    bool inList = false;
    for( int i = 0; i < _ui->cob_limit->count(); i++ )
    {
        if( _ui->cob_limit->itemText( i ) == id )
        {
            inList = true;
            break;
        }
    }
    if( !inList )
    {
        _ui->cob_limit->addItem( id );
    }
}
