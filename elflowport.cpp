#include "elflowport.h"


elFlowPort::elFlowPort( QObject* parent ) : labPort( parent ), _inMsg( false ),
    _maxCapacity( 0.0 ), _setValueType( setValueType::setTypeNone ),
    _setFlowPercent( 0.0 ), _autoAdjust( false ),
    _emitFlow( false ), _emitPressure( false ), _emitTemperature( false )
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
    _protocol.setNode( id );
}

void elFlowPort::getInitValues()
{
    sendCmd( _protocol.getListenToRS232Cmd() );
//    sendCmd( _protocol.getFluidNameCmd() );
//    sendCmd( _protocol.getCapacityUnitCmd() );
    sendCmd( _protocol.getCapacityValueCmd() );
    sendCmd( _protocol.getIdStringCmd() );
}

void elFlowPort::updateValues()
{
    // checkInTimeCount();

    if( _emitFlow )
    {
        getFlow();
    }
    if( _emitPressure )
    {
        getPressure();
    }
    if( _emitTemperature )
    {
        getTemperature();
    }
    // updateNumInTimeValues();

    if( _setValueType != setValueType::setTypeNone
            && _autoAdjust )
    {
        // adjustValues();
    }
}

void elFlowPort::setValue( setValueType type, double value, bool autoAdjust )
{
    _autoAdjust = autoAdjust;

    if( type == setValueType::setTypeNone )
    {
        return;
    }
    else if( type == setValueType::setTypeFlow )
    {
        sendCmd( _protocol.getFlowValueSetCmd( value ) );
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

double elFlowPort::maxCapacity() const
{
    return _maxCapacity;
}

void elFlowPort::sendCmd( const std::string& cmd )
{
    sendMsg( cmd.c_str(), cmd.size(), false );
}

void elFlowPort::receivedMsg( QByteArray msg )
{
    while( msg.size() > 0 &&
           (_inMsg || msg.contains( ":" )) )
    {
        int index = 0;
        if( !_inMsg )
        {
            index = msg.indexOf( ":" );
            if( index < 0 )
            {
                return;
            }
            _inMsg = true;
        }

        for( ; index < msg.size(); index++ )
        {
            if( msg.at( index ) == '\r' )
            {
                decodeMsg( _buffer );
                _inMsg = false;
                _buffer.clear();
                break;
            }
            _buffer.append( msg.at( index) );
        }
        msg = msg.mid( index+1 );
    }
}

void elFlowPort::setEmitFlow( bool emitFlow )
{
    _emitFlow = emitFlow;
}

void elFlowPort::setEmitPressure( bool emitPressure )
{
    _emitPressure = emitPressure;
}

void elFlowPort::setEmitTemperature( bool emitTemperature )
{
    _emitTemperature = emitTemperature;
}

void elFlowPort::getFlow()
{
    sendCmd( _protocol.getFlowValueCmd() );
}

void elFlowPort::getPressure()
{
    LOG(INFO) << "getpressure: " << _protocol.getPressureCmd();
    sendCmd( _protocol.getPressureCmd() );
}

void elFlowPort::getTemperature()
{
    sendCmd( _protocol.getTemperatureCmd() );
}

void elFlowPort::decodeMsg( const QByteArray& msg )
{
    elFlowAnswer answer = _protocol.interpretAnswer( msg.toStdString() );

    if( answer.type == elFlowAnswerType::elFlowError )
    {
        emit portError( "Invalid answer!" );
    }
    else if( answer.type == elFlowAnswerType::protocolError )
    {
        emit portError( "Protocol error!" );
    }
    else if( answer.type == elFlowAnswerType::elFlowStatus )
    {
        if( answer.text.find( "no error" ) == std::string::npos )
        {
            emit portError( QString::fromStdString( answer.text ) );
        }
    }
    else if( answer.type == elFlowAnswerType::elFlowValue )
    {
        showParameter( answer.paramNum, answer.value, answer.text );
    }
    else
    {
        emit portError( "Warning: unhandled answer type!" );
    }
}

void elFlowPort::showParameter( int paramNum, double value,
                                const std::string& text )
{
    switch( paramNum )
    {
        case 1: _initValueCounter++;
                emit initSuccessful( QString::fromStdString( text ) );
            break;

        case 8: emit newFlow( value/320.0 );
            break;

        case 21: _maxCapacity = value;
            break;

        case 107: emit newPressure( value );
            break;

        case 142: emit newTemperature( value );
            break;
        case 143: emit newPressure( value );
            break;
    }
}
