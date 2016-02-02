#include "labport.h"

labPort::labPort( QObject *parent ) : QObject( parent ),
    _initTimeoutMs( 0 ), _initValueCounter( 0 ), _numInitValues( 0 ),
    _minBytesRead( 0 ), _writingPauseMs( 100 ), _bytesError( 100 ),
    _inTimeValueCounter( 0 ), _numInTimeValues( 0 )
{    
    connect( &_port, SIGNAL( readyRead() ), this, SLOT( read() ) );
    connect( &_port, SIGNAL( error( QSerialPort::SerialPortError ) ), this,
             SLOT( signalError( QSerialPort::SerialPortError ) ) );
    connect( &_sendTimer, SIGNAL( timeout() ), this, SLOT( timeToSendMsg() ) );
    connect( &_initTimer, SIGNAL( timeout() ), this, SLOT( initTimeout() ) );
}

bool labPort::openPort( const QString& portName )
{
    if( _port.isOpen() )
    {
        _port.close();
    }
    _port.setPortName( portName );

    if( !_port.open( QSerialPort::ReadWrite ) )
    {
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
    return (_initValueCounter >= _numInitValues
            && _port.isOpen());
}

QString labPort::getPortName()
{
    return _port.portName();
}

void labPort::clearPort()
{
    if( _port.isOpen() )
    {
        _port.clear();
    }
    _port.clearError();
}

void labPort::sendMsg( const char* msg, int numChars, bool inTime )
{
    QByteArray bytes( msg, numChars );
    _msgToSend.push_back( bytes );

    if( !_sendTimer.isActive() )
    {
        timeToSendMsg();
        _sendTimer.start( _writingPauseMs );
    }

    if( inTime )
    {
        _inTimeValueCounter++;
    }
}

void labPort::timeToSendMsg()
{
    if( !_port.isOpen() )
    {
        emit portError( "Port not open (writing failed)!" );
        return;
    }

    if( _msgToSend.size() > 0 )
    {
        _port.write( _msgToSend[0].data(), _msgToSend[0].size() );
        _msgToSend.erase( _msgToSend.begin() );
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
    _sendTimer.stop();

    _port.clear();
    _port.clearError();
    _port.close();
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
    if( _inTimeValueCounter < _numInTimeValues )
    {
        emit portError( "Value not read!" );
    }
    _inTimeValueCounter = 0;
}

void labPort::read()
{
    if( _minBytesRead == -10 )
    {
        if( _port.canReadLine() )
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
                emit dataReceived( QByteArray( _port.readAll() ) );
            }
        }
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


