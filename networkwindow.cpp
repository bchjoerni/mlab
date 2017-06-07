#include "networkwindow.h"
#include "ui_networkwindow.h"


networkWindow::networkWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::networkWindow ),
    _networkManager( new QNetworkAccessManager( this ) ),
    _counterCommands( 0 ), _counterScreen( -1 ), _counterReset( 0 )
{
    _ui->setupUi( this );

    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStopPressed() ) );
    connect( _ui->spb_ticks, SIGNAL( valueChanged( int ) ), this,
             SLOT( commandTicksChanged() ) );

    _ui->lbl_status->setText( PAUSING );
}

networkWindow::~networkWindow()
{
    delete _networkManager;
    delete _ui;
}

void networkWindow::mLabSignal( const QString& cmd )
{

}

void networkWindow::doUpdate()
{
    if( _ui->btn_startStop->text() == STOP_REMOTE )
    {
        _counterCommands++;
        _ui->lbl_ticksToNext->setText(
            QString::number( _ui->spb_ticks->value() - _counterCommands ) );

        if( _ui->chb_reset->isChecked() )
        {
            _counterReset++;

            if( _counterReset >= _ui->spb_reset->value() )
            {
                delete _networkManager;
                _networkManager = new QNetworkAccessManager( this );
                _counterReset = 0;
            }
        }
    }

    if( _counterCommands >= _ui->spb_ticks->value() )
    {
        _counterCommands = 0;
        _counterScreen = 0;
        _ui->lbl_ticksToNext->setText( QString::number(
                                              _ui->spb_ticks->value() ) );
        if( _ui->chb_downloadCmds->isChecked() )
        {
            downloadCmd();
        }
    }

    if( _counterScreen >= 0 )
    {
        _counterScreen++;
    }
    if( _counterScreen-1 >= _ui->spb_ticksScreen->value() )
    {
        if( _ui->chb_uploadScreen->isChecked() )
        {
            uploadScreen();
        }
        _counterScreen = -1;
    }
}

void networkWindow::commandTicksChanged()
{
    _ui->lbl_ticksToNext->setText( QString::number(
                                             _ui->spb_ticks->value() ) );
    _ui->spb_ticksScreen->setMaximum( _ui->spb_ticks->value() );
}

void networkWindow::startStopPressed()
{
    if( _ui->btn_startStop->text() == START_REMOTE )
    {
        QUrl url( _ui->txt_url->text() );
        if( !url.isValid() && !url.isEmpty() )
        {
            _ui->lbl_info->setText( "Invalid cmd url!" );
            return;
        }

        if( _ui->txt_url->text().right( 1 ) != "/" )
        {
            _ui->txt_url->setText( _ui->txt_url->text() + "/" );
        }

        _ui->txt_url->setEnabled( false );
        _ui->txt_username->setEnabled( false );
        _ui->txt_password->setEnabled( false );

        _password = _ui->txt_password->text();
        _ui->txt_password->setText( "" );

        _counterCommands = _ui->spb_ticks->value();
        _counterScreen = -1;
        _ui->lbl_status->setText( RUNNING );
        _ui->lbl_status->setStyleSheet( STYLE_OK );
        _ui->btn_startStop->setText( STOP_REMOTE );
        LOG(INFO) << "networkwindow started, url="
                  << _ui->txt_url->text().toStdString();
    }
    else
    {
        _ui->txt_url->setEnabled( true );
        _ui->txt_username->setEnabled( true );
        _ui->txt_password->setEnabled( true );

        _ui->lbl_status->setText( PAUSING );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        _ui->btn_startStop->setText( START_REMOTE );
    }
}

void networkWindow::uploadScreen()
{
    LOG(INFO) << "start upload screen...";
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

    QUrl url( _ui->txt_url->text() + "screen.png" );
    url.setUserName( _ui->txt_username->text() );
    url.setPassword( _password );
    url.setPort( 21 );

    QNetworkRequest upload( url );
    QNetworkReply* reply = _networkManager->put( upload, _screenBytes );

    connect( reply, SIGNAL( uploadProgress( qint64, qint64 ) ), this,
             SLOT( uploadScreenProgress( qint64, qint64 ) ) );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
            this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
    connect( reply, SIGNAL( finished() ), this,
             SLOT( uploadScreenFinished() ) );
}

void networkWindow::networkError( QNetworkReply::NetworkError error )
{
    QString errorStr = "Network error: " + errorString( error )
            + " (" + QString::number( error ) + ")";
    LOG(INFO) << errorStr.toStdString();
    _ui->lbl_info->setText( errorStr );
    emit newError( this->windowTitle() + ": " + errorStr );
}

QString networkWindow::errorString( QNetworkReply::NetworkError error )
{
    switch( error )
    {
        case QNetworkReply::ConnectionRefusedError:            return "Connection refused!";
        case QNetworkReply::RemoteHostClosedError:             return "Remote host closed!";
        case QNetworkReply::HostNotFoundError:                 return "Host not found!";
        case QNetworkReply::TimeoutError:                      return "Timeout!";
        case QNetworkReply::OperationCanceledError:            return "Operation canceled!";
        case QNetworkReply::SslHandshakeFailedError:           return "SSL Handshake failed!";
        case QNetworkReply::TemporaryNetworkFailureError:      return "Temporary network failure!";
        case QNetworkReply::NetworkSessionFailedError:         return "Network session failed!";
        case QNetworkReply::BackgroundRequestNotAllowedError:  return "Background request not allowed";
    //    case QNetworkReply::TooManyRedirectsError:             return "Too many redirects!";
    //    case QNetworkReply::InsecureRedirectError:             return "Insecure redirect!";
        case QNetworkReply::ProxyConnectionRefusedError:       return "Proxy connection refused!";
        case QNetworkReply::ProxyConnectionClosedError:        return "Proxy connection closed!";
        case QNetworkReply::ProxyNotFoundError:                return "Proxy not found!";
        case QNetworkReply::ProxyTimeoutError:                 return "Proxy timeout";
        case QNetworkReply::ProxyAuthenticationRequiredError:  return "Proxy authentication required!";
        case QNetworkReply::ContentAccessDenied:               return "Content access denied!";
        case QNetworkReply::ContentOperationNotPermittedError: return "Content operation not permitted!";
        case QNetworkReply::ContentNotFoundError:              return "Content not found!";
        case QNetworkReply::AuthenticationRequiredError:       return "Authentication required!";
        case QNetworkReply::ContentReSendError:                return "Content resend!";
        case QNetworkReply::ContentConflictError:              return "Content conflict!";
        case QNetworkReply::ContentGoneError:                  return "Content gone!";
        case QNetworkReply::InternalServerError:               return "Internal server error!";
        case QNetworkReply::OperationNotImplementedError:      return "Operation not implemented";
        case QNetworkReply::ServiceUnavailableError:           return "Service unavailable!";
        case QNetworkReply::ProtocolUnknownError:              return "Protocol unknown";
        case QNetworkReply::ProtocolInvalidOperationError:     return "Protocol invalid operation!";
        case QNetworkReply::UnknownNetworkError:               return "Unknown network!";
        case QNetworkReply::UnknownProxyError:                 return "Unknown proxy!";
        case QNetworkReply::UnknownContentError:               return "Unknown content";
        case QNetworkReply::ProtocolFailure:                   return "Protocol failure";
        case QNetworkReply::UnknownServerError:                return "Unknown server!";
        default:                                               return "Unknown error!";
    }
}

void networkWindow::uploadScreenProgress( qint64 bytesSent, qint64 bytesTotal )
{
    if( bytesTotal > 0 )
    {
        _ui->lbl_info->setText( "Upload screen: "
                                + QString::number( bytesSent/bytesTotal )
                                + "%, " + QDateTime::currentDateTime()
                                .toString( "yyyy-MM-dd hh:mm:ss" ) );
    }
}

void networkWindow::uploadScreenFinished()
{
    if( !_ui->lbl_info->text().startsWith( "Network error" ) )
    {
        _ui->lbl_info->setText( "Screen upload finished. "
                                    + QDateTime::currentDateTime()
                                    .toString( "yyyy-MM-dd hh:mm:ss" ) );
        LOG(INFO) << "screen upload finished";
    }
}

void networkWindow::downloadCmdsProgress( qint64 bytesSent, qint64 bytesTotal )
{
    if( bytesTotal > 0 )
    {
        _ui->lbl_info->setText( "Download cmds: "
                                + QString::number( bytesSent/bytesTotal )
                                + "%" + QDateTime::currentDateTime()
                                .toString( "yyyy-MM-dd hh:mm:ss" ) );
    }
}

void networkWindow::downloadCmd()
{
    LOG(INFO) << "start download cmds";
    QNetworkRequest request( QUrl( _ui->txt_url->text().replace( "ftp", "http" )
                                   + "cmd.txt" ) );
    _downloadReply = _networkManager->get( request );

    connect( _downloadReply, SIGNAL( readyRead() ), this,
             SLOT( readCommands() ) );
    connect( _downloadReply, SIGNAL( downloadProgress( qint64, qint64 ) ), this,
             SLOT( downloadCmdsProgress( qint64, qint64 ) ) );
    connect( _downloadReply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
    connect( _downloadReply, SIGNAL( finished() ), this,
             SLOT( downloadCmdsFinished() ) );
}

void networkWindow::readCommands()
{
    while( _downloadReply->canReadLine() )
    {
        QString cmdLine = _downloadReply->readLine();
        LOG(INFO) << "network command read: " << cmdLine.toStdString();
        int firstSeperator = cmdLine.indexOf( "\t" );

        if( firstSeperator != -1 )
        {
            QString receiver = cmdLine.left( firstSeperator );
            QString cmd = cmdLine.mid( firstSeperator+1 );

            emit newSignal( receiver, cmd );
        }
    }
}

void networkWindow::downloadCmdsFinished()
{
    clearOnlineCmds();
    LOG(INFO) << "download cmds finished, clear online cmds";
}

void networkWindow::clearOnlineCmds()
{
    QUrl url( _ui->txt_url->text() + "cmd.txt" );
    url.setUserName( _ui->txt_username->text() );
    url.setPassword( _password );
    url.setPort( 21 );

    QNetworkRequest upload( url );
    QNetworkReply* reply = _networkManager->put( upload, "" );

    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
            this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
    connect( reply, SIGNAL( finished() ), this,
             SLOT( clearOnlineCmdsFinished() ) );
}

void networkWindow::clearOnlineCmdsFinished()
{
    LOG(INFO) << "clear online cmds finished";
}


