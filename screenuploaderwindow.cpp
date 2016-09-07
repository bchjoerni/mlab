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
    connect( _networkManager, SIGNAL( finished( QNetworkReply* ) ), this,
             SLOT( uploadFinished( QNetworkReply* ) ) );

    _ui->lbl_status->setText( PAUSING );
}

screenUploaderWindow::~screenUploaderWindow()
{
    delete _networkManager;
    delete _ui;
}

void screenUploaderWindow::mLabSignal( char signal, const QString& cmd )
{
    if( signal == SHUTDOWN_SIGNAL
            || (signal == STOP_SIGNAL) )
    {
        if( _ui->btn_startStop->text() == STOP_UPLOADING )
        {
            _ui->lbl_status->setText( PAUSING );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->lbl_info->setText( signal == SHUTDOWN_SIGNAL ?
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
    }

    if( _counter >= _ui->spb_ticks->value() )
    {
        _counter = 0;
        uploadScreen();
    }
}

void screenUploaderWindow::startStopPressed()
{
    if( _ui->btn_startStop->text() == START_UPLOADING )
    {
        _counter = 0;
        _ui->lbl_status->setText( RUNNING );
        _ui->lbl_status->setStyleSheet( STYLE_OK );
        _ui->btn_startStop->setText( STOP_UPLOADING );
    }
    else
    {
        _ui->lbl_status->setText( PAUSING );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        _ui->btn_startStop->setText( START_UPLOADING );
    }
}

void screenUploaderWindow::uploadScreen()
{
    QPixmap pixmap;
    QScreen *screen = QGuiApplication::primaryScreen();
    if( !screen )
    {
        _ui->lbl_info->setText( "Error: unable to get screen!" );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        return;
    }
    pixmap = screen->grabWindow( 0 );

    QByteArray screenBytes;
    QBuffer buffer( &screenBytes );
    buffer.open( QIODevice::WriteOnly );
    pixmap.save( &buffer, "PNG" );

    QUrl url( _ui->txt_url->text() );
    url.setUserName( _ui->txt_username->text() );
    url.setPassword( _ui->txt_password->text() );
    url.setPort( 21 );

    QNetworkRequest upload( url );
    QNetworkReply* reply = _networkManager->put( upload, screenBytes );

    connect( reply, SIGNAL( uploadProgress( qint64, qint64 ) ), this,
             SLOT( uploadProgress( qint64, qint64 ) ) );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
            this, SLOT( uploadError( QNetworkReply::NetworkError ) ) );
}

void screenUploaderWindow::uploadError( QNetworkReply::NetworkError error )
{
    _ui->lbl_info->setText( "Network error! (" + QString::number( error )
                            + ")" );
}

void screenUploaderWindow::uploadProgress( qint64 bytesSent, qint64 bytesTotal )
{
    _ui->lbl_info->setText( "Upload: " + QString::number( bytesSent/bytesTotal )
                            + "%" );
}

void screenUploaderWindow::uploadFinished( QNetworkReply *reply )
{
    _ui->lbl_info->setText( "Upload finished. " + QDateTime::currentDateTime()
                            .toString( "yyyy-MM-dd hh:mm:ss" ) );
}

