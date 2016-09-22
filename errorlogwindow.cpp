#include "errorlogwindow.h"
#include "ui_errorlogwindow.h"


errorLogWindow::errorLogWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::errorLogWindow ), _recording( false )
{
    _ui->setupUi( this );

    connect( _ui->btn_selectFile, SIGNAL( clicked() ), this,
             SLOT( selectFile() ) );
    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStopPressed() ) );
    connect( _ui->btn_clearLog, SIGNAL( clicked() ), this,
             SLOT( clearLog() ) );
}

errorLogWindow::~errorLogWindow()
{
    delete _ui;
}

void errorLogWindow::mLabSignal( char signal, const QString& cmd )
{
    if( signal == SIGNAL_SHUTDOWN
            || (signal == SIGNAL_STOP &&
                _ui->chb_setZeroAtStopSignal->isChecked()) )
    {
        if( _recording )
        {
            _recording = false ;
            _ui->btn_startStop->setText( START_RECORDING );
            _ui->lbl_status->setText( signal == SIGNAL_SHUTDOWN ?
                                          EMERGENCY_STOP : STOP_RECEIVED );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->btn_selectFile->setEnabled( true );
            emit changeWindowState( this->windowTitle(), false );
        }

    }
    else if( signal == 10 )
    {
        if( _ui->btn_startStop->text() == STOP_RECORDING )
        {
            startStopPressed();
        }
    }
    else if( signal == 11 )
    {
        if( _ui->btn_startStop->text() == START_RECORDING )
        {
            startStopPressed();
        }
    }
    else if( signal == 19 )
    {
        clearLog();
    }
}

void errorLogWindow::mLabError( const QString& errorMsg )
{
    if( _recording )
    {
        _fileStream.open( _fileName.toStdString().c_str(), std::ios_base::app );

        if( !_fileStream.is_open() )
        {
            _ui->lbl_status->setText( "ERROR!" );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->txt_errorLog->append( QDateTime::currentDateTime().toString(
                                           "yyyy-MM-dd_hh:mm:ss: " ) +
                                       "Error: unable to open file!" );
        }
        else
        {
            _fileStream << QDateTime::currentDateTime().toString(
                               "yyyy-MM-dd_hh:mm:ss: " ).toStdString()
                        << errorMsg.toStdString() << std::endl;
            _fileStream.close();
        }
    }

    _ui->txt_errorLog->append( QDateTime::currentDateTime().toString(
                                   "yyyy-MM-dd_hh:mm:ss: " ) + errorMsg );
}

void errorLogWindow::selectFile()
{
    QString name = "mlab_errorlog_" +
        QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh_mm_ss" ) + ".txt";
    _fileName = QFileDialog::getSaveFileName( this, "Select file", name,
                                              "text files (*.txt)" );
    if( !_fileName.isEmpty() )
    {
        _ui->lbl_fileName->setText( _fileName );
        _ui->btn_startStop->setEnabled( true );
    }
}

void errorLogWindow::startStopPressed()
{
    bool start = (_ui->btn_startStop->text() == START_RECORDING);

    if( start )
    {
        _fileStream.open( _fileName.toStdString().c_str(), std::ios_base::out );
        if( !_fileStream.is_open() )
        {
            _ui->lbl_status->setText( "ERROR!" );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->txt_errorLog->append( QDateTime::currentDateTime().toString(
                                           "yyyy-MM-dd_hh:mm:ss: " ) +
                                       "Error: unable to open file!" );
            return;
        }
        _fileStream.close();
        _ui->lbl_status->setText( RECORDING );
        _ui->lbl_status->setStyleSheet( STYLE_OK );
        _ui->btn_startStop->setText( STOP_RECORDING );

        emit changeWindowState( this->windowTitle(), true );
    }
    else
    {
        _ui->btn_startStop->setText( START_RECORDING );
        _ui->lbl_status->setText( PAUSING );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );

        emit changeWindowState( this->windowTitle(), false );
    }

    _ui->btn_selectFile->setEnabled( !start );
    _recording = start;
}

void errorLogWindow::clearLog()
{
    _ui->txt_errorLog->clear();
}
