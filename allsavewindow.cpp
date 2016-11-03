#include "allsavewindow.h"
#include "ui_allsavewindow.h"

allSaveWindow::allSaveWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::allSaveWindow ), _intervalCounter( 0 ), _savedCounter( 0 ),
    _recording( false ), _dataRecorded( false )
{
    _ui->setupUi( this );

    connect( _ui->btn_selectFile, SIGNAL( clicked() ), this,
             SLOT( selectFile() ) );
    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStopPressed() ) );
    connect( _ui->btn_resetCounter, SIGNAL( clicked() ), this,
             SLOT( resetCounter() ) );
}

allSaveWindow::~allSaveWindow()
{
    delete _ui;
}

void allSaveWindow::mLabSignal( char signal, const QString& cmd )
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
            _intervalCounter = 0;
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
        resetCounter();
    }
}

void allSaveWindow::doUpdate()
{
    if( _recording && _intervalCounter%_ui->spb_ticks->value() == 0 )
    {
        _fileStream.open( _fileName.toStdString().c_str(), std::ios_base::app );

        if( !_fileStream.is_open() )
        {
            _ui->lbl_status->setText( "ERROR!" );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            emit newError( this->windowTitle() + ": unable to open file!" );
            return;
        }

        _fileStream << "{" << std::endl;
        for( std::string s : _data )
        {
            _fileStream << s << std::endl;
        }
        _fileStream << "}" << std::endl;
        _fileStream.close();

        _dataRecorded = true;
        _savedCounter++;
        _ui->lbl_numSaved->setText( QString::number( _savedCounter ) );
        _intervalCounter = 0;
    }
    _data.clear();
    _intervalCounter++;
}

void allSaveWindow::putValue( const QString& id, double value )
{
    _data.push_back( id.toStdString() + ": " + std::to_string( value ) );
}

void allSaveWindow::selectFile()
{
    QString name = "mlab_all_" +
        QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh_mm_ss" ) + ".txt";
    _fileName = QFileDialog::getSaveFileName( this, "Select file", name,
                                              "text files (*.txt)" );
    if( !_fileName.isEmpty() )
    {
        _ui->lbl_fileName->setText( _fileName );
        _ui->btn_startStop->setEnabled( true );
        _dataRecorded = false;
    }
}

void allSaveWindow::startStopPressed()
{
    bool start = (_ui->btn_startStop->text() == START_RECORDING);

    if( start )
    {
        if( _dataRecorded )
        {
            if( QMessageBox::warning( this, "Warning", "Old data will be "
                                      "overwritten!\nContinue?",
                                      QMessageBox::StandardButtons(
                                          QMessageBox::Yes | QMessageBox::No ),
                                      QMessageBox::StandardButton::No )
                    == QMessageBox::StandardButton::No )
            {
                return;
            }
        }

        _fileStream.open( _fileName.toStdString().c_str(), std::ios_base::out );
        if( !_fileStream.is_open() )
        {
            _ui->lbl_status->setText( "ERROR!" );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            emit newError( this->windowTitle() + ": unable to open file!" );
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
    _intervalCounter = 0;
    _recording = start;
}

void allSaveWindow::resetCounter()
{
    _savedCounter = 0;
    _ui->lbl_numSaved->setText( "0" );
}
