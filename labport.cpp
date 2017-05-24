#include "labport.h"

labPort::labPort( QObject *parent ) : QObject( parent ),
    _initTimeoutMs( 0 ), _initValueCounter( 0 ), _numInitValues( 0 ),
    _minBytesRead( 0 ), _writingPauseMs( 100 ), _bytesError( 100 ),
    _inTimeValueCounter( 0 ), _numInTimeValues( 0 ),
    _closing( false ), _lastMsgSent( QTime::currentTime() )
{    
    connect( &_port, SIGNAL( readyRead() ), this, SLOT( read() ) );
    connect( &_port, SIGNAL( error( QSerialPort::SerialPortError ) ), this,
             SLOT( signalError( QSerialPort::SerialPortError ) ) );
    connect( &_sendTimer, SIGNAL( timeout() ), this, SLOT( timeToSendMsg() ) );
    connect( &_initTimer, SIGNAL( timeout() ), this, SLOT( initTimeout() ) );

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
             SLOT( logReadData( QByteArray ) ) );
}

void labPort::setBaudRate( QSerialPort::BaudRate baudRate )
{
    _port.setBaudRate( baudRate );
}

void labPort::setDataBits( QSerialPort::DataBits dataBits )
{
    _port.setDataBits( dataBits );
}

void labPort::setStopBits( QSerialPort::StopBits stopBits )
{
    _port.setStopBits( stopBits );
}

void labPort::setParity( QSerialPort::Parity parity )
{
    _port.setParity( parity );
}

void labPort::setFlowControl( QSerialPort::FlowControl flowControl )
{
    _port.setFlowControl( flowControl );
}

bool labPort::openPort( const QString& portName )
{
    if( _closeTimer.isActive() )
    {
        emit portError( "Port is still closing, please retry!" );
        return false;
    }

    if( _port.isOpen() )
    {
        _port.clear();
        _port.close();
    }    

    _port.clearError();
    _closing = false;
    _port.setPortName( portName );
    _msgToSend.clear();

    if( !_port.open( QSerialPort::ReadWrite ) )
    {
        emit portError( "Unable to open port!" );
        return false;
    }

    if( _initTimeoutMs > 0 )
    {
        _initTimer.start( _initTimeoutMs );
    }
    _initValueCounter = 0;
    getInitValues();
    return true;
}

bool labPort::isOpen()
{
    return _port.isOpen();
}

bool labPort::isRunning()
{
    return (_initValueCounter >= _numInitValues
            && _port.isOpen()
            && !_closing );
}

QString labPort::getPortName()
{
    return _port.portName();
}

void labPort::clearPortErrors()
{
    _port.clearError();
}

void labPort::sendMsg( const char* msg, int numChars, bool inTime )
{
    if( _closing )
    {
        return;
    }

    QByteArray bytes( msg, numChars );
    _msgToSend.push_back( bytes );

    if( !_sendTimer.isActive() )
    {
        if( _lastMsgSent.msecsTo( QTime::currentTime() ) > _writingPauseMs )
        {
            timeToSendMsg();
        }
        else
        {
            _sendTimer.start( _writingPauseMs );
        }
    }

    if( inTime )
    {
        _inTimeValueCounter++;
    }
}

void labPort::timeToSendMsg()
{
    if( _closing )
    {
        return;
    }

    if( !_port.isOpen() )
    {
        emit portError( "Port not open (writing failed)!" );
        return;
    }

    if( _msgToSend.size() > 0 )
    {
        LOG(INFO) << "write " << _msgToSend[0].size() << " bytes:";
        for( int i = 0; i < _msgToSend[0].size(); i++ )
        {
            char c = _msgToSend[0].at( i );
            LOG(INFO) << "write (dec): " << static_cast<int>(
                             reinterpret_cast<unsigned char&>( c ) );
        }
        _port.write( _msgToSend[0].data(), _msgToSend[0].size() );
        _msgToSend.erase( _msgToSend.begin() );
        _lastMsgSent = QTime::currentTime();
    }

    if( _msgToSend.size() > 0 )
    {
        _sendTimer.start( _writingPauseMs );
    }
    else
    {
        _sendTimer.stop();
    }
}

void labPort::closePort()
{
    _closing = true;
    _sendTimer.stop();
    _closeTimer.singleShot( _writingPauseMs*5/4, this, SLOT( finishPortClose() ) ); // interval > than writing pause
}

void labPort::finishPortClose()
{
    _msgToSend.clear();
    _port.close();
    _port.clearError();
}

void labPort::initTimeout()
{
    _initTimer.stop();
    if( _initValueCounter < _numInitValues )
    {
        emit portError( "Initialization timeout!" );
    }
}

void labPort::getInitValues()
{
    // specify in derived class!
}

void labPort::updateValues()
{
    // specify in derived class!
}

void labPort::checkInTimeCount()
{
    if( _closing )
    {
        return;
    }

    if( _inTimeValueCounter < _numInTimeValues )
    {
        emit portError( "Value not read!" );
    }
    _inTimeValueCounter = 0;
}

void labPort::read()
{
    if( _closing )
    {
        return;
    }

    if( _minBytesRead == -10 )
    {
        while( _port.canReadLine() )
        {
            emit dataReceived( _port.readLine() );
        }
    }
    else
    {
        while( _port.bytesAvailable() >= _minBytesRead
               && _port.bytesAvailable() > 0 )
        {
            if( _minBytesRead > 0 )
            {
                int bytesRead = _port.read( _buffer, _minBytesRead );
                emit dataReceived( QByteArray( _buffer, bytesRead ) );
            }
            else
            {
                emit dataReceived( _port.readAll() );
            }
        }
    }
}

void labPort::logReadData( const QByteArray& received )
{
    LOG(INFO) << "read " << received.size() << " bytes:";
    for( int i = 0; i < received.size(); i++ )
    {
        char c = received.at( i );
        LOG(INFO) << "read (dec): " << static_cast<int>(
                         reinterpret_cast<unsigned char&>( c ) );
    }
}

void labPort::signalError( const QSerialPort::SerialPortError& error )
{
    switch( error )
    {
        case 0: return; // no error
        case 1: emit portError( "Device not found!" );
            break;
        case 2: emit portError( "No Permission!" );
            break;
        case 3: emit portError( "Error when opening an already opened device!" );
            break;
        case 4: emit portError( "Parity error!" );
            break;
        case 5: emit portError( "Framing error!" );
            break;
        case 6: emit portError( "Break condition error!" );
            break;
        case 7: emit portError( "Writing error!" );
            break;
        case 8: emit portError( "Reading error!" );
            break;
        case 9: emit portError( "A resource is no longer available!" );
            break;
        case 10: emit portError( "Unsupported operation!" );
            break;
        case 11: emit portError( "Unknown error!" );
            break;
        case 12: emit portError( "Timeout!" );
            break;
        case 13: emit portError( "Device not open!" );
            break;
        default: emit portError( "Unspecified error!" );
            break;
    }
}


