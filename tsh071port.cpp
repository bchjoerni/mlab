#include "tsh071port.h"

tsh071Port::tsh071Port( QObject* parent ) : labPort( parent ),
    _id( 0 )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
}

void tsh071Port::setSerialValues()
{
    _port.setBaudRate( QSerialPort::Baud9600 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::NoParity );
}

void tsh071Port::setLabPortVariables()
{
    _initTimeoutMs      = 1500;
    _initValueCounter   = 0;
    _numInitValues      = 1;
    _minBytesRead       = 1;
    _writingPauseMs     = 100;
    _bytesError         = 20;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void tsh071Port::setId( int id )
{
    _id = id;
}

void tsh071Port::getInitValues()
{
    getDrvName();
}

void tsh071Port::updateValues()
{
    if( _emitRpm )
    {
        getRotation();
    }
    if( _emitPressure )
    {
        getPressure();
    }
}

void tsh071Port::startPump()
{
    std::string cmd = addZerosToSize( std::to_string( _id ), 3 ) +
            "1001006111111";
    cmd += addZerosToSize( std::to_string( calcCheckSum( cmd ) ), 3 );
    cmd += "\r";
    LOG(INFO) << "send: " << cmd;
    sendMsg( cmd.c_str(), cmd.size(), false );
}

void tsh071Port::stopPump()
{
    std::string cmd = addZerosToSize( std::to_string( _id ), 3 ) +
            "1001006000000";
    cmd += addZerosToSize( std::to_string( calcCheckSum( cmd ) ), 3 );
    cmd += "\r";
    LOG(INFO) << "send: " << cmd;
    sendMsg( cmd.c_str(), cmd.size(), false );
}

void tsh071Port::getDrvName()
{
    std::string cmd = addZerosToSize( std::to_string( _id ), 3 ) + "0034902=?";
    cmd += addZerosToSize( std::to_string( calcCheckSum( cmd ) ), 3 );
    cmd += "\r";
    sendMsg( cmd.c_str(), cmd.size(), false );
}

void tsh071Port::getRotation()
{
    std::string cmd = addZerosToSize( std::to_string( _id ), 3 ) + "0030902=?";
    cmd += addZerosToSize( std::to_string( calcCheckSum( cmd ) ), 3 );
    cmd += "\r";
    sendMsg( cmd.c_str(), cmd.size(), false );
}

void tsh071Port::getPressure()
{
    std::string cmd = addZerosToSize( std::to_string( _id ), 3 ) + "0034002=?";
    cmd += addZerosToSize( std::to_string( calcCheckSum( cmd ) ), 3 );
    cmd += "\r";
    sendMsg( cmd.c_str(), cmd.size(), false );
}

void tsh071Port::setEmitRpm( bool emitRpm )
{
    _emitRpm = emitRpm;
}

void tsh071Port::setEmitPressure( bool emitPressure )
{
    _emitPressure = emitPressure;
}

void tsh071Port::setValue( setValueType type, double value )
{
    if( type == setValueType::setTypeNone )
    {
        return;
    }
    else if( type == setValueType::setTypeRpm )
    {
        int rpm = static_cast<int>( value );
        if( rpm < 0 )
        {
            rpm = 0;
        }
        if( rpm > 1500 )
        {
            rpm = 1500;
        }
        std::string cmd = addZerosToSize( std::to_string( _id ), 3 )
                + "1030806";
        cmd += addZerosToSize( std::to_string( rpm ), 6 );
        cmd += addZerosToSize( std::to_string( calcCheckSum( cmd ) ), 3 );
        cmd += "\r";
        sendMsg( cmd.c_str(), cmd.size(), false );
    }
    else
    {
        emit portError( "Unknown value to set!" );
    }
}

QString tsh071Port::idString() const
{
    return _idString;
}

void tsh071Port::receivedMsg( QByteArray msg )
{
    char c = msg.at( 0 );

    if( c == '\r' )
    {
        if( _receivedMsg.size() > 12 )
        {
            interpretMsg();
        }
        _receivedMsg.clear();
    }
    else if( c >= 32 )
    {
        _receivedMsg += c;
    }
}

void tsh071Port::interpretMsg()
{
    if( !checkAnswerForErrors() )
    {
        return;
    }

    LOG(INFO) << "new msg: " << _receivedMsg.toStdString();

    QString cmd = _receivedMsg.mid( 5, 3 );
    if( cmd == "349" )
    {
        _idString = _receivedMsg.mid( 10, 6 );
        _initValueCounter++;
        emit initSuccessful( _idString );
    }
    else if( cmd == "309" )
    {
        bool conversionSuccessful = false;
        int rpm = _receivedMsg.mid( 10, 6 ).toInt( &conversionSuccessful );
        if( conversionSuccessful )
        {
            emit newRpm( rpm );
        }
        else
        {
            emit portError( "Invalid rotation speed!" );
        }
    }
    else if( cmd == "340" )
    {
        bool conversionSuccessful = false;
        double pressure = _receivedMsg.mid( 10, 6 ).toDouble(
                    &conversionSuccessful );
        if( conversionSuccessful )
        {
            emit newPressure( pressure );
        }
        else
        {
            emit portError( "Invalid pressure!" );
        }
    }
}

bool tsh071Port::checkAnswerForErrors()
{
    if( _receivedMsg.mid( 10, 6 ) == "NO_DEF" )
    {
        emit portError( "Invalid command!" );
        return false;
    }
    else if( _receivedMsg.mid( 10, 6 ) == "_RANGE" )
    {
        emit portError( "Range error!" );
        return false;
    }
    else if( _receivedMsg.mid( 10, 6 ) == "_LOGIC" )
    {
        emit portError( "Protocol error!" );
        return false;
    }

    return true;
}

int tsh071Port::calcCheckSum( const std::string& cmd )
{
    int checksum = 0;
    for( unsigned int i = 0; i < cmd.size(); i++ )
    {
        checksum += static_cast<int>( cmd[i] );
    }
    return checksum%256;
}

std::string tsh071Port::addZerosToSize( const std::string& s,
                                        unsigned int size )
{
    std::string str( s );

    while( str.size() < size )
    {
        str = "0" + str;
    }
    return str;
}
