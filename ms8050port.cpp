#include "ms8050port.h"

ms8050Port::ms8050Port( QObject *parent ) : labPort( parent ),
    _idString( "MS 8050" ), _emitValue( false )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
}

void ms8050Port::setSerialValues()
{
    _port.setBaudRate( QSerialPort::Baud2400 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::EvenParity );
}

void ms8050Port::setLabPortVariables()
{
    _initTimeoutMs      = 700;
    _initValueCounter   = 0;
    _numInitValues      = 1;
    _minBytesRead       = 8;
    _writingPauseMs     = 150;
    _bytesError         = 20;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void ms8050Port::getInitValues()
{
    emit initSuccessful( _idString ); // if no value is received within init time --> error
}

void ms8050Port::updateValues()
{
    checkInTimeCount();
}

void ms8050Port::setEmitValue( bool emitValue )
{
    _emitValue = emitValue;
}

QString ms8050Port::idString() const
{
    return _idString;
}

void ms8050Port::receivedMsg( QByteArray msg )
{
    _bufferReceived.append( msg );

    for( int i = 0; i < msg.size()-7; i++ )
    {
        char startMarker = _bufferReceived.at( 0 );
        if( (reinterpret_cast<unsigned char&>( startMarker ) & 0xF0) == 0xA0 )
        {
            interpretMessage( _bufferReceived.left( 8 ) );
            _bufferReceived.clear();
        }
        else
        {
            _bufferReceived = _bufferReceived.right( _bufferReceived.size()-1 );
        }
    }
}

void ms8050Port::interpretMessage( const QByteArray &msg )
{
    double value;
    value = msg.at( 3 )*10000 + msg.at( 4 )*1000 + msg.at( 5 )*100
            + msg.at( 6 )*10 + msg.at( 7 );

    emit newMinMax( msg.at( 2 ) & 3 );
    emit newRelative( (msg.at( 2 ) & 4) == 4 );
    emit newHold( (msg.at( 1 ) & 64) == 64 );

    if( (msg.at( 2 ) & 32) == 32 )
    {
        value = -value;
    }
    char power = msg.at( 0 ) & 15;
    char unit = msg.at( 1 ) & 31;

    switch( unit )
    {
        case 0: value *= std::pow( 10, power-4 ); // V AC
            break;        
        case 1: value *= std::pow( 10, power-4 ); // V DC
            break;
        case 2: value *= std::pow( 10, power-4 ); // V AC+DC
            break;
        case 3: value *= std::pow( 10, power-3 ); // mV DC
            break;
        case 4: value *= std::pow( 10, power-3 ); // mV AC
            break;
        case 5: value *= std::pow( 10, power-3 ); // V AC+DC
            break;
        case 6: value *= std::pow( 10, power-3 ); // Hz
            break;
        case 7: value *= std::pow( 10, power-4 ); // V Diode
            break;
        case 8: value *= std::pow( 10, power-2 ); // Ohm
            break;
        case 9: value *= std::pow( 10, power-2 ); // continuity
            break;
        case 10: value *= std::pow( 10, power-8 ); // Capacitance
            break;
        case 11: value *= std::pow( 10, power-2 ); // uA DC
            break;
        case 12: value *= std::pow( 10, power-2 ); // uA AC
            break;
        case 13: value *= std::pow( 10, power-2 ); // uA AC+DC
            break;
        case 14: value *= std::pow( 10, power-3 ); // mA DC
            break;
        case 15: value *= std::pow( 10, power-3 ); // mA AC
            break;
        case 16: value *= std::pow( 10, power-3 ); // mA AC+DC
            break;
        case 17: value *= std::pow( 10, power-4 ); // A DC
            break;
        case 18: value *= std::pow( 10, power-4 ); // A AC
            break;
        case 19: value *= std::pow( 10, power-4 ); // A AC+DC
            break;

        default: emit portError( "Protocol error!" );
    }

    _initValueCounter++;
    emit newValue( value, unit );
}


