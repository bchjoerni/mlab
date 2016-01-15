#include "labport.h"

labPort::labPort( QObject *parent ) : QObject( parent ),    
    _initValueCounter( 0 ), _realtimeValueCounter( 0 ),
    _initTimeoutMs( 0 ), _numInitValues( 0 ), _minBytesRead( 0 ),
    _writingPauseMs( 100 ), _bytesError( 100 )
{    
    connect( &_port, SIGNAL( readyRead() ), this, SLOT( read() ) );
    connect( &_port, SIGNAL( error( QSerialPort::SerialPortError ) ), this,
             SLOT( signalError( QSerialPort::SerialPortError ) ) );
    connect( &_sendTimer, SIGNAL( timeout() ), this, SLOT( timeToSendMsg() ) );
    connect( &_initTimer, SIGNAL( timeout() ), this, SLOT( initTimeout() ) );
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

void labPort::setInitTimeoutMs( int initTimeoutMs )
{
    _initTimeoutMs = initTimeoutMs;
}

void labPort::setNumInitValues( int numInitValues )
{
    _numInitValues = numInitValues;
}

void labPort::setMinBytesRead( int minBytesRead )
{
    _minBytesRead = minBytesRead;
}

void labPort::setWritingPauseMs( int writingPauseMs )
{
    _writingPauseMs = writingPauseMs;
}

void labPort::setBytesError( int bytesError )
{
    _bytesError = bytesError;
}

bool labPort::openPort( const QString& portName )
{
    LOG(INFO) << "open port ...";
    if( _port.isOpen() )
    {
        _port.close();
    }

    _port.setPortName( portName );

    if( !_port.open( QSerialPort::ReadWrite ) )
    {
        LOG(INFO) << "opening port failed";
        emit portError( "Unable to open port!" );
        return false;
    }

    if( _initTimeoutMs > 0 )
    {
        _initTimer.start( _initTimeoutMs );
    }
    getInitValues();
    return true;
}

bool labPort::isOpen()
{
    return _port.isOpen();
}

bool labPort::isRunning()
{
    return (_initValueCounter >= _numInitValues);
}

void labPort::setRunning( bool running )
{
    _initValueCounter = running ? _numInitValues + 1 : _numInitValues - 1;
}

QString labPort::getPortName()
{
    return _port.portName();
}

void labPort::clearErrors()
{
    _port.clearError();
}

void labPort::sendMsg( char* msg, int numChars, bool realtime )
{
    LOG(INFO) << "sendMsg (" << numChars << " bytes)";
    std::unique_ptr<char[]> up( new char[numChars] );
    for( int i = 0; i < numChars; i++ )
    {
        up[i] = msg[i];
    //    LOG(INFO) << static_cast<int>( (unsigned char) msg[i] );
    }
    _msgToSend.push_back( std::move( up ) );

    if( _msgToSend.size() == 1 )
    {
        _sendTimer.start( _writingPauseMs );
    }

    if( realtime )
    {
        _realtimeValueCounter++;
    }
}

void labPort::timeToSendMsg()
{
    if( !_port.isOpen() )
    {
        emit portError( "Could not write to device, device not open!" );
        return;
    }

    _port.write( _msgToSend[0].get(), 24 );
/*
    LOG(INFO) << "WRITE:";
    for( int i = 0; i < 24; i++ )
    {
        LOG(INFO) << static_cast<int>( (unsigned char) _msgToSend[0].get()[i] );
    }
*/
    _msgToSend.erase( _msgToSend.begin() );

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
    _port.clear();
    _port.clearError();
    _port.close();
}

void labPort::initTimeout()
{
    if( _initValueCounter < _numInitValues )
    {
        emit portError( "Initialization failed!" );
    }
}

void labPort::getInitValues()
{
    // specify in derived class!
}

void labPort::getUpdateValues()
{
    // specify in derived class!
}

void labPort::updateValues()
{
    if( _initValueCounter >= _numInitValues)
    {
        if( _realtimeValueCounter > 0 )
        {
            emit portError( "Asyncronous/timeout!" );
        }
    }
}

void labPort::read()
{
    while( _port.bytesAvailable() >= _minBytesRead )
    {
        LOG(INFO) << "available before reading: " << _port.bytesAvailable();
        if( _realtimeValueCounter > 0 )
        {
            _realtimeValueCounter--;
        }

        if( _minBytesRead > 0 )
        {
            int bytesRead = _port.read( _buffer, _minBytesRead );
            /*
            for( int i = 0; i < MIN_BYTES_READ; i++ )
            {
                LOG(INFO) << static_cast<int>( (unsigned char) _buffer[i] );
            }
            */
            emit dataReceived( QByteArray( _buffer, bytesRead ) );
        }
        else
        {
            emit dataReceived( QByteArray( _port.readAll() ) );
        }
        LOG(INFO) << "available after reading: " << _port.bytesAvailable();
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


