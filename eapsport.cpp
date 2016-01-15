#include "eapsport.h"

eapsPort::eapsPort( QObject *parent ) : labPort( parent ), _id( 0 ),
    _maxVoltageOut( 0 ), _maxCurrentOut( 0 ),
    _maxVoltageIn( 1 ), _maxCurrentIn( 1 ),
    _initCount( 0 ),
    _maxVoltage( 16.0 ), _maxCurrent( 20.0 ),
    _lastVoltage( 0.0 ), _lastCurrent( 0.0 ),
    _setValueType( eapsSetValues::none ), _autoAdjust( false ),
    _emitVoltage( true ), _emitCurrent( true ), _emitPower( true )
{
    _port.setBaudRate( QSerialPort::Baud57600 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::NoParity );

    _port.setInitTimeoutMs( 1500 );
    _port.setNumInitValues( 4 );
    _port.setMinBytesRead( 24 );

    connect( &_port, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
}

void eapsPort::open( const QString& portName )
{
    LOG(INFO) << "open eapsPort ...";
    _initCount = 0;
    if( !_port.openPort( portName ) )
    {
        LOG(INFO) << "opening eapsPort failed";
        emit portError( "Error: unable to open port!" );
        return;
    }

    getIdn();
}

void eapsPort::getUpdateValues()
{
    if( _emitVoltage || _emitPower )
    {
        getVoltage();
    }
    if( _emitCurrent || _emitPower )
    {
        getCurrent();
    }

    if( _setValueType != eapsSetValues::none
            && _autoAdjust )
    {
        if( _setValueType == eapsSetValues::voltage )
        {
            setVoltage( _setVoltage - (_lastVoltage - _setVoltage)*0.25 );
            LOG(INFO) << "_setVoltage: " << _setVoltage
                      << "  _lastVoltage" << _lastVoltage;
        }
        else if( _setValueType == eapsSetValues::current )
        {
            setCurrent( _setCurrent - (_lastCurrent - _setCurrent)*0.25 );
        }
    }
}

void eapsPort::getInitValues()
{
    getIdn();
}

void eapsPort::setValue( eapsSetValues type, double value, bool autoAdjust )
{
    _setValueType = type;
    if( type == eapsSetValues::none )
    {
        return;
    }
    _autoAdjust = autoAdjust;

    if( type == eapsSetValues::voltage )
    {
        _setVoltage = value;
        _lastVoltage = value;
        setVoltage( value );
    }
    else if( type == eapsSetValues::current )
    {
        _setCurrent = value;
        _lastCurrent = value;
        setCurrent( value );
    }
}

void eapsPort::setVoltage( double voltage )
{
    LOG(INFO) << "set voltage";
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x1F;
    int value = static_cast<int>( voltage*_maxVoltageOut/_maxVoltage );
    msg[4] = value/256;
    msg[5] = value%256;
    setCheckBytes( msg );
    _port.sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::setCurrent( double current )
{
    LOG(INFO) << "set current";
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x3F;
    int value = static_cast<int>( current*_maxCurrentOut/_maxCurrent );
    msg[4] = value/256;
    msg[5] = value%256;
    setCheckBytes( msg );
    _port.sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::setRemoteControl( bool on )
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x5F;
    msg[4] = on ? 0x40 : 0;
    setCheckBytes( msg );
    _port.sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::getIdString()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x8F;
    setCheckBytes( msg );
    _port.sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::getVoltage()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x2F;
    setCheckBytes( msg );
    _port.sendMsg( ((char*) &msg), MESSAGE_LENGTH, true );
}

void eapsPort::getCurrent()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x4F;
    setCheckBytes( msg );
    _port.sendMsg( ((char*) &msg), MESSAGE_LENGTH, true );
}

void eapsPort::getIdn()
{
    LOG(INFO) << "getIdn()";
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[3] = 0xFF;
    setCheckBytes( msg );
    _port.sendMsg( ((char*) &msg), MESSAGE_LENGTH, true );
}

void eapsPort::getMaxValues()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x48;
    setCheckBytes( msg );
    _port.sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::setToPrototype( unsigned char *msg )
{
    for( int i = 0; i < MESSAGE_LENGTH; i++ )
    {
        msg[i] = MESSAGE_PROTOTYPE[i];
    }
}

void eapsPort::setCheckBytes( unsigned char* msg )
{
    int checksum = calcCheckBytes( msg );
    msg[MESSAGE_LENGTH - 2] = checksum/256;
    msg[MESSAGE_LENGTH - 1] = checksum%256;
}

int eapsPort::calcCheckBytes( unsigned char *msg )
{
    int checksum = 0;
    for( int i = 0; i < MESSAGE_LENGTH - 2; i++ )
    {
        checksum += msg[i];
    }
    return (256*256 - 1 - checksum);
}

bool eapsPort::checkMsgBytes( unsigned char *msg )
{
    return calcCheckBytes( msg )/256 == msg[MESSAGE_LENGTH - 2]
            && calcCheckBytes( msg )%256 == msg[MESSAGE_LENGTH - 1];
}

void eapsPort::receivedMsg( QByteArray msg )
{
    qApp->processEvents();
    if( msg.size() != MESSAGE_LENGTH )
    {
        emit portError( "Invalid answer length!" );
        return;
    }
    unsigned char msgValues[MESSAGE_LENGTH];
    for( int i = 0; i < MESSAGE_LENGTH; i++ )
    {
        msgValues[i] = (unsigned char) msg.at( i );
    }
    if( !checkMsgBytes( msgValues ) )
    {
        return;
    }

    LOG(INFO) << "command: " << static_cast<int>( msgValues[3] );
    switch( msgValues[3] )
    {
        case 0x1F:
        {
            break;
        }
        case 0x2F:
        {
            int value = msgValues[4]*256 + msgValues[5];
            _lastVoltage = _maxVoltage*value/_maxVoltageIn;
            emit newVoltage( _lastVoltage );
            break;
        }
        case 0x3F:
        {
            break;
        }
        case 0x4F:
        {
            int value = msgValues[4]*256 + msgValues[5];
            _lastCurrent = _maxCurrent*value/_maxCurrentIn;
            emit newCurrent( _lastCurrent );
            if( _emitPower )
            {
                emit newPower( _lastVoltage*_lastCurrent );
            }
            break;
        }
        case 0x5F:
        {
            emit initSuccessful( _idString );
            _initValueCounter++;
            break;
        }
        case 0x8F:
        {
            _idString = msg.mid( 4, MESSAGE_LENGTH-6 );
            int pos = _idString.indexOf( "30" );
            if( pos > 0 )
            {
                bool conversionSuccessful = false;
                int vmax = _idString.mid( pos+2, 2 ).toInt(
                            &conversionSuccessful );
                if( conversionSuccessful )
                {
                    _maxVoltage = vmax;
                }
                int imax = _idString.mid( pos+5, 2 ).toInt(
                            &conversionSuccessful );
                if( conversionSuccessful )
                {
                    _maxCurrent = imax;
                }
            }
            _idString = "ID string: " + _idString;
            _initValueCounter++;
            setRemoteControl( true );
            break;
        }
        case 0xAF:
        {
            emit portError( "Communication error! ("
                            + QString::number( msgValues[5] ) + ")" );
            break;
        }
        case 0xFF:
        {            
            _id = msgValues[4];
            _initValueCounter++;
            getMaxValues();
            break;
        }
        case 0x48:
        {
            _maxVoltageOut = msgValues[4]*256 + msgValues[5];
            _maxCurrentOut = msgValues[6]*256 + msgValues[7];
            _maxVoltageIn  = msgValues[8]*256 + msgValues[9];
            _maxCurrentIn = msgValues[10]*256 + msgValues[11];
            _initValueCounter++;
            getIdString();
            break;
        }
    default: emit portError( "Unknown answer!" );
            break;
    }
}
