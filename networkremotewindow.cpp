#include "networkremotewindow.h"
#include "ui_networkremotewindow.h"


networkRemoteWindow::networkRemoteWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::networkRemoteWindow ),
    _networkManager( new QNetworkAccessManager( this ) ),
    _counterCommands( 0 ), _counterScreen( -1 )
{
    _ui->setupUi( this );

    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStopPressed() ) );
    connect( _ui->spb_ticksCmd, SIGNAL( valueChanged( int ) ), this,
             SLOT( commandTicksChanged() ) );

    _ui->lbl_status->setText( PAUSING );
}

networkRemoteWindow::~networkRemoteWindow()
{
    delete _networkManager;
    delete _ui;
}

void networkRemoteWindow::mLabSignal( char signal, const QString& cmd )
{
    if( signal == SIGNAL_SHUTDOWN
            || (signal == SIGNAL_STOP) )
    {
        if( _ui->btn_startStop->text() == STOP_REMOTE )
        {
            _ui->lbl_status->setText( PAUSING );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );
            _ui->lbl_info->setText( signal == SIGNAL_SHUTDOWN ?
                                        "Shutdown signal received!" :
                                        "Stop signal received!" );
            _ui->btn_startStop->setText( START_REMOTE );
        }
    }
}

void networkRemoteWindow::doUpdate()
{
    if( _ui->btn_startStop->text() == STOP_REMOTE )
    {
        _counterCommands++;
        _ui->lbl_ticksToNextCmd->setText(
            QString::number( _ui->spb_ticksCmd->value() - _counterCommands ) );
    }

    if( _counterCommands >= _ui->spb_ticksCmd->value() )
    {
        _counterCommands = 0;
        _counterScreen = 0;
        _ui->lbl_ticksToNextCmd->setText( QString::number(
                                              _ui->spb_ticksCmd->value() ) );
        downloadCmd();
    }

    if( _counterScreen >= 0 )
    {
        _counterScreen++;
    }
    if( _counterScreen-1 >= _ui->spb_ticksUpload->value() )
    {
        uploadScreen();
        _counterScreen = -1;
    }
}

void networkRemoteWindow::commandTicksChanged()
{
    _ui->lbl_ticksToNextCmd->setText( QString::number(
                                             _ui->spb_ticksCmd->value() ) );
    _ui->spb_ticksUpload->setMaximum( _ui->spb_ticksCmd->value() );
}

void networkRemoteWindow::startStopPressed()
{
    if( _ui->btn_startStop->text() == START_REMOTE )
    {
        QUrl urlCmd( _ui->txt_urlCmd->text() );
        if( !urlCmd.isValid() )
        {
            _ui->lbl_info->setText( "Invalid cmd url!" );
            return;
        }
        QUrl urlUpload( _ui->txt_urlUpload->text() );
        if( !urlUpload.isValid() )
        {
            _ui->lbl_info->setText( "Invalid upload url!" );
            return;
        }

        _ui->grp_screenUpload->setEnabled( false );
        _ui->grp_commands->setEnabled( false );
        _password = _ui->txt_passwordUpload->text();
        _ui->txt_passwordUpload->setText( "" );

        _counterCommands = 0;
        _counterScreen = -1;
        _ui->lbl_status->setText( RUNNING );
        _ui->lbl_status->setStyleSheet( STYLE_OK );
        _ui->btn_startStop->setText( STOP_REMOTE );
    }
    else
    {
        _ui->grp_screenUpload->setEnabled( true );
        _ui->grp_commands->setEnabled( true );

        _ui->lbl_status->setText( PAUSING );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        _ui->btn_startStop->setText( START_REMOTE );
    }
}

void networkRemoteWindow::uploadScreen()
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

    QUrl url( _ui->txt_urlUpload->text() );
    url.setUserName( _ui->txt_usernameUpload->text() );
    url.setPassword( _password );
    url.setPort( 21 );

    QNetworkRequest upload( url );
    QNetworkReply* reply = _networkManager->put( upload, _screenBytes );

    connect( reply, SIGNAL( uploadProgress( qint64, qint64 ) ), this,
             SLOT( uploadProgress( qint64, qint64 ) ) );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
            this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( uploadFinished() ) );
}

void networkRemoteWindow::networkError( QNetworkReply::NetworkError error )
{
    LOG(INFO) << "network error: " << error;
    _ui->lbl_info->setText( "Network error! (" + QString::number( error )
                            + ")" );
    emit newError( this->windowTitle() + ": network error "
                   + QString::number( error ) );
}

void networkRemoteWindow::uploadProgress( qint64 bytesSent, qint64 bytesTotal )
{
    if( bytesTotal > 0 )
    {
        _ui->lbl_info->setText( "Upload: "
                                + QString::number( bytesSent/bytesTotal )
                                + "%, " + QDateTime::currentDateTime()
                                .toString( "yyyy-MM-dd hh:mm:ss" ) );
    }
}

void networkRemoteWindow::uploadFinished()
{
    if( !_ui->lbl_info->text().startsWith( "Network error" ) )
    {
        _ui->lbl_info->setText( "Screen upload finished. "
                                    + QDateTime::currentDateTime()
                                    .toString( "yyyy-MM-dd hh:mm:ss" ) );
    }
}

void networkRemoteWindow::downloadProgress( qint64 bytesSent,
                                            qint64 bytesTotal )
{
    if( bytesTotal > 0 )
    {
        _ui->lbl_info->setText( "Download: "
                                + QString::number( bytesSent/bytesTotal )
                                + "%" + QDateTime::currentDateTime()
                                .toString( "yyyy-MM-dd hh:mm:ss" ) );
    }
}

void networkRemoteWindow::downloadCmd()
{
    QNetworkRequest request( QUrl( _ui->txt_urlCmd->text() ) );
    _downloadReply = _networkManager->get( request );

    connect( _downloadReply, SIGNAL( readyRead() ), this,
             SLOT( readCommands() ) );
    connect( _downloadReply, SIGNAL( downloadProgress( qint64, qint64 ) ), this,
             SLOT( downloadProgress( qint64, qint64 ) ) );
    connect( _downloadReply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
}

void networkRemoteWindow::readCommands()
{
    while( _downloadReply->canReadLine() )
    {
        QString cmdLine = _downloadReply->readLine();
        LOG(INFO) << "network command read: " << cmdLine.toStdString();
        int firstSeperator = cmdLine.indexOf( "\t" );
        int secondSeperator = cmdLine.indexOf( "\t", firstSeperator+1 );

        if( firstSeperator != -1
                && secondSeperator != -1 )
        {
            QString receiver = cmdLine.left( firstSeperator );
            bool conversionSuccessful = false;
            int temp = cmdLine.mid( firstSeperator+1,
                                    secondSeperator-firstSeperator-1 ).toInt(
                        &conversionSuccessful );
            if( conversionSuccessful
                    && temp < 256 )
            {
                char signal = static_cast<char>( temp );
                QString cmd = cmdLine.mid( secondSeperator+1 );
                _ui->lbl_info->setText( "signal " + QString::number( temp )
                                        + " emitted!" );
                emit newSignal( receiver, signal, cmd );
            }
        }
    }
}
