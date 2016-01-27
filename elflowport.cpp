#include "elflowport.h"


elFlowPort::elFlowPort( QObject* parent ) : labPort( parent ),
    _id( 128 )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
}

void elFlowPort::setSerialValues()
{
    _port.setBaudRate( QSerialPort::Baud38400 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::NoParity );
}

void elFlowPort::setLabPortVariables()
{
    _initTimeoutMs      = 1500;
    _initValueCounter   = 0;
    _numInitValues      = 1;
    _minBytesRead       = 1;
    _writingPauseMs     = 100;
    _bytesError         = 50;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void elFlowPort::setId( int id )
{
    _id = id;
}

void elFlowPort::getInitValues()
{
}

void elFlowPort::updateValues()
{
}

void elFlowPort::setValue( setValueType type, double value )
{
    if( type == setValueType::setTypeNone )
    {
        return;
    }
    else
    {
        emit portError( "Unknown value to set!" );
    }
}

QString elFlowPort::idString() const
{
    return _idString;
}

void elFlowPort::receivedMsg( QByteArray msg )
{
}

