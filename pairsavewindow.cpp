#include "pairsavewindow.h"
#include "ui_pairsavewindow.h"

pairSaveWindow::pairSaveWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::pairSaveWindow ), _intervalCounter( 0 ), _savedCounter( 0 ),
    _recording( false ), _dataRecorded( false )
{
    _ui->setupUi( this );

    connect( _ui->btn_selectFile, SIGNAL( clicked() ), this,
             SLOT( selectFile() ) );
    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStopPressed() ) );
    connect( _ui->btn_resetCounter, SIGNAL( clicked() ), this,
             SLOT( resetCounter() ) );

    addDelimiters();
}

pairSaveWindow::~pairSaveWindow()
{
    delete _ui;
}

void pairSaveWindow::mLabSignal( const QString& cmd )
{    
    QString cmdLower = cmd.toLower();

    if( cmdLower == STOP_SIGNAL
            && _ui->chb_setZeroAtStopSignal->isChecked() )
    {
        if( _recording )
        {
            _recording = false ;
            _ui->btn_startStop->setText( START_RECORDING );
            _ui->lbl_status->setText( STOP_INFO_TEXT );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->btn_selectFile->setEnabled( true );
            _intervalCounter = 0;
            emit changeWindowState( this->windowTitle(), false );
        }
    }
    else if( cmdLower == "btn_startstop\tpress" )
    {
        startStopPressed();
    }
    else if( cmdLower == "btn_startrecording\tpress" )
    {
        if( _ui->btn_startStop->text() == START_RECORDING )
        {
            startStopPressed();
        }
    }
    else if( cmdLower == "btn_stoprecording\tpress" )
    {
        if( _ui->btn_startStop->text() == STOP_RECORDING )
        {
            startStopPressed();
        }
    }
    else if( cmdLower == "btn_resetcounter\tpress" )
    {
        resetCounter();
    }
}

void pairSaveWindow::doUpdate()
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

        _fileStream << _x << getDelimiter().toStdString() << _y << std::endl;
        _fileStream.close();

        _dataRecorded = true;
        _savedCounter++;
        _ui->lbl_numSaved->setText( QString::number( _savedCounter ) );
        _intervalCounter = 0;
    }
    _intervalCounter++;
}

void pairSaveWindow::putValue( const QString& id, double value )
{
    if( id == _ui->cob_x->currentText() )
    {
        _x = value;
    }
    if( id == _ui->cob_y->currentText() )
    {
        _y = value;
    }

    bool inList = false;
    for( int i = 0; i < _ui->cob_x->count(); i++ )
    {
        if( _ui->cob_x->itemText( i ) == id )
        {
            inList = true;
            break;
        }
    }
    if( !inList )
    {
        _ui->cob_x->addItem( id );
        _ui->cob_y->addItem( id );
    }
}

void pairSaveWindow::selectFile()
{
    QString name = "mlab_ui_" +
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

void pairSaveWindow::startStopPressed()
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
    }
    else
    {
        _ui->btn_startStop->setText( START_RECORDING );
        _ui->lbl_status->setText( PAUSING );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    }

    _ui->btn_selectFile->setEnabled( !start );
    _ui->cob_delimiter->setEnabled( !start );
    _ui->cob_x->setEnabled( !start );
    _ui->cob_y->setEnabled( !start );

    _intervalCounter = 0;
    _recording = start;

    emit changeWindowState( this->windowTitle(), start );
}

void pairSaveWindow::addDelimiters()
{
    _ui->cob_delimiter->clear();

    _ui->cob_delimiter->addItem( DELIMITER_TABSTOP );
    _ui->cob_delimiter->addItem( DELIMITER_KOMMA );
    _ui->cob_delimiter->addItem( DELIMITER_SEMIKOLON );
    _ui->cob_delimiter->addItem( DELIMITER_WHITESPACE );
}

QString pairSaveWindow::getDelimiter()
{
    if( _ui->cob_delimiter->currentText() == DELIMITER_KOMMA )
    {
        return ",";
    }
    else if( _ui->cob_delimiter->currentText() == DELIMITER_SEMIKOLON )
    {
        return ";";
    }
    else if( _ui->cob_delimiter->currentText() == DELIMITER_TABSTOP )
    {
        return "\t";
    }
    else
    {
        return " ";
    }
}

void pairSaveWindow::resetCounter()
{
    _savedCounter = 0;
    _ui->lbl_numSaved->setText( "0" );
}
