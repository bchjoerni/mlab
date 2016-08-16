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
    _initTimeoutMs      = 1500;
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
    // wait for data to be sent
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
    if( msg.isNull() )
    {
        return;
    }

    if( msg.isEmpty() )
    {
        return;
    }

    char first = msg.at( 0 );
    if( (reinterpret_cast<unsigned char&>( first ) & 0xA0) != 0xA0 )
    {
        emit portError( "Wrong start marker!" );
    }
    else
    {
        qApp->processEvents();
        interpretMessage( msg );
    }
}

void ms8050Port::interpretMessage( const QByteArray &msg )
{
    double value;
    value = (msg.at( 3 ) - 0x30)*10000 + (msg.at( 4 ) - 0x30)*1000
            + (msg.at( 5 ) - 0x30)*100 + (msg.at( 6 ) - 0x30)*10
            + (msg.at( 7 ) - 0x30);

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
        case 1: value *= std::pow( 10, power-2 ); // dBm
            break;
        case 2: value *= std::pow( 10, power-4 ); // V DC
            break;
        case 3: value *= std::pow( 10, power-4 ); // V AC+DC
            break;
        case 4: value *= std::pow( 10, power-4 ); // mV DC
            break;
        case 5: value *= std::pow( 10, power-4 ); // mV AC
            break;
        case 6: value *= std::pow( 10, power-7 ); // V AC+DC
            break;
        case 7: value *= std::pow( 10, power-3 ); // Hz
            break;
        case 8: value *= std::pow( 10, power-2 ); // Duty
            break;
        case 9: value *= std::pow( 10, power-2 ); // Ohm
            break;
        case 10: value *= std::pow( 10, power-2 ); // continuity
            break;
        case 11: value *= std::pow( 10, power-8 ); // Capacitance
            break;
        case 12: value *= std::pow( 10, power-2 ); // uA DC
            break;
        case 13: value *= std::pow( 10, power-2 ); // uA AC
            break;
        case 14: value *= std::pow( 10, power-2 ); // uA AC+DC
            break;
        case 15: value *= std::pow( 10, power-3 ); // mA DC
            break;
        case 16: value *= std::pow( 10, power-3 ); // mA AC
            break;
        case 17: value *= std::pow( 10, power-4 ); // mA AC+DC
            break;
        case 18: value *= std::pow( 10, power-4 ); // A DC
            break;
        case 19: value *= std::pow( 10, power-4 ); // A AC
            break;
        case 20: value *= std::pow( 10, power-4 ); // A AC+DC
            break;

        default: emit portError( "Protocol error!" );
    }

    emit newValue( value, unit );
}

