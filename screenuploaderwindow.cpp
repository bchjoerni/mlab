#include "screenuploaderwindow.h"
#include "ui_screenuploaderwindow.h"

screenUploaderWindow::screenUploaderWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::screenUploaderWindow ),
    _networkManager( new QNetworkAccessManager( this ) ),
    _counter( 0 )
{
    _ui->setupUi( this );

    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStopPressed() ) );
    connect( _ui->spb_ticks, SIGNAL( valueChanged( int ) ), this,
             SLOT( updateTickCounter() ) );

    _ui->lbl_status->setText( PAUSING );
}

screenUploaderWindow::~screenUploaderWindow()
{
    delete _networkManager;
    delete _ui;
}

void screenUploaderWindow::mLabSignal( char signal, const QString& cmd )
{
    if( signal == SIGNAL_SHUTDOWN
            || (signal == SIGNAL_STOP) )
    {
        if( _ui->btn_startStop->text() == STOP_UPLOADING )
        {
            _ui->lbl_status->setText( PAUSING );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->lbl_info->setText( signal == SIGNAL_SHUTDOWN ?
                                        "Shutdown signal received!" :
                                        "Stop signal received!" );
            _ui->btn_startStop->setText( START_UPLOADING );
        }
    }
}

void screenUploaderWindow::doUpdate()
{
    if( _ui->btn_startStop->text() == STOP_UPLOADING )
    {
        _counter++;
        _ui->lbl_ticksToNextScreen->setText( QString::number(
                                                 _ui->spb_ticks->value() -
                                                 _counter ) );

        if( _counter >= _ui->spb_ticks->value() )
        {
            _ui->lbl_ticksToNextScreen->setText( QString::number(
                                                     _ui->spb_ticks->value() ) );
            _counter = 0;
            uploadScreen();
        }
    }
}

void screenUploaderWindow::updateTickCounter()
{
    _ui->lbl_ticksToNextScreen->setText( QString::number(
                                             _ui->spb_ticks->value() ) );
}

void screenUploaderWindow::startStopPressed()
{
    if( _ui->btn_startStop->text() == START_UPLOADING )
    {
        QUrl url( _ui->txt_url->text() );
        if( !url.isValid() )
        {
            _ui->lbl_info->setText( "Invalid URL!" );
            return;
        }
        _ui->frame_parameter->setEnabled( false );
        _username = _ui->txt_username->text();
        _password = _ui->txt_password->text();
        _ui->txt_password->setText( "" );

        _counter = 0;
        _ui->lbl_status->setText( RUNNING );
        _ui->lbl_status->setStyleSheet( STYLE_OK );
        _ui->btn_startStop->setText( STOP_UPLOADING );        
    }
    else
    {
        _ui->frame_parameter->setEnabled( true );

        _ui->lbl_status->setText( PAUSING );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        _ui->btn_startStop->setText( START_UPLOADING );
    }
}

void screenUploaderWindow::uploadScreen()
{
    LOG(INFO) << "upload screen";
    QPixmap pixmap;
    QScreen *screen = QGuiApplication::primaryScreen();
    if( !screen )
    {
        LOG(INFO) << "ERROR: Unable to get screen!";
        _ui->lbl_info->setText( "Error: unable to get screen!" );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        return;
    }
    pixmap = screen->grabWindow( 0 );

    QBuffer buffer( &_screenBytes );
    buffer.open( QIODevice::WriteOnly );
    pixmap.save( &buffer, "PNG" );

    QUrl url( _ui->txt_url->text() );
    url.setUserName( _username );
    url.setPassword( _password );
    url.setPort( 21 );

    QNetworkRequest upload( url );
    QNetworkReply* reply = _networkManager->put( upload, _screenBytes );

    connect( reply, SIGNAL( uploadProgress( qint64, qint64 ) ), this,
             SLOT( uploadProgress( qint64, qint64 ) ) );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
            this, SLOT( uploadError( QNetworkReply::NetworkError ) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( uploadFinished() ) );
    connect( reply, SIGNAL( finished() ), reply, SLOT( deleteLater() ) );
}

void screenUploaderWindow::uploadError( QNetworkReply::NetworkError error )
{
    LOG(INFO) << "upload screen error: " << error;
    _ui->lbl_info->setText( "Network error! (" + QString::number( error )
                            + ")" );
}

void screenUploaderWindow::uploadProgress( qint64 bytesSent, qint64 bytesTotal )
{
    if( bytesTotal > 0 )
    {
        _ui->lbl_info->setText( "Upload: "
                                + QString::number( bytesSent/bytesTotal )
                                + "%" );
    }
}

void screenUploaderWindow::uploadFinished()
{
    if( !_ui->lbl_info->text().startsWith( "Network error" ) )
    {
        _ui->lbl_info->setText( "Upload finished. "
                                + QDateTime::currentDateTime()
                                .toString( "yyyy-MM-dd hh:mm:ss" ) );
    }
}

