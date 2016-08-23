#include "tpg26xport.h"

tpg26xPort::tpg26xPort( QObject *parent ) : labPort( parent ),
    _readyToSend( true ),
    _emitGauge1( false ), _emitGauge2( false )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
    connect( &_checkForAnswerTimer, SIGNAL( timeout() ), this,
             SLOT( waitForSendingTimeout() ) );
}

void tpg26xPort::setSerialValues()
{
    _port.setBaudRate( QSerialPort::Baud9600 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::NoParity );
}

void tpg26xPort::setLabPortVariables()
{
    _initTimeoutMs      = 1500;
    _initValueCounter   = 0;
    _numInitValues      = 1;
    _minBytesRead       = -10;
    _writingPauseMs     = 200;
    _bytesError         = 40;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void tpg26xPort::getInitValues()
{
    getUnit();
    getId();
}

void tpg26xPort::updateValues()
{
    checkInTimeCount();
    if( !_expectedAnswer.isEmpty() )
    {
        _expectedAnswer = "";
        if( !_closing )
        {
            emit portError( "Answer missing!" );
        }
    }

    if( _emitGauge1 || _emitGauge2 )
    {
        getPressure();
    }
}

void tpg26xPort::setEmitGauge1( bool emitGauge1 )
{
    _emitGauge1 = emitGauge1;
}

void tpg26xPort::setEmitGauge2( bool emitGauge2 )
{
    _emitGauge2 = emitGauge2;
}

QString tpg26xPort::idString() const
{
    return _idString;
}

void tpg26xPort::getId()
{
    sendTpg26xCmd( CMD_ID );
}

void tpg26xPort::getUnit()
{
    sendTpg26xCmd( CMD_UNIT );
}

void tpg26xPort::getPressure()
{
    sendTpg26xCmd( CMD_PRESSURE );
}

void tpg26xPort::sendTpg26xCmd( QString cmd )
{
    _msgToSend.push_back( cmd );

    if( _readyToSend )
    {
        nextMsg();
    }
    else
    {
        _checkForAnswerTimer.start( (_expectedAnswer.size() + 1)
                                    *_writingPauseMs );
    }
}

void tpg26xPort::nextMsg()
{
    if( _msgToSend.size() > 0 )
    {
        QString msg = _msgToSend[0] + "\r\n";
        _expectedAnswer = _msgToSend[0];
        sendMsg( msg.toStdString().c_str(), msg.size(), false );
        _msgToSend.erase( _msgToSend.begin() );
    }
    else
    {
        _readyToSend = true;
        _expectedAnswer = "";
    }
}


void tpg26xPort::waitForSendingTimeout()
{
    _checkForAnswerTimer.stop();

    if( _msgToSend.size() > 0 )
    {
        emit portError( "Sending timeout!" );
        _msgToSend.clear();
        _readyToSend = true;
    }
}

void tpg26xPort::receivedMsg( QByteArray msg )
{
    if( msg.isNull() )
    {
        return;
    }
    if( msg.isEmpty() )
    {
        return;
    }

    if( msg.startsWith( 0x06 ) )
    {
        char c = 5;
        sendMsg( &c, 1, false );
        return;
    }
    else if( msg.startsWith( 0x15 ) )
    {
        emit portError( "Negative ack.!" );
        return;
    }

    if( _expectedAnswer.isEmpty() )
    {
        emit portError( "Unexpected answer!" );
        return;
    }

    if( _expectedAnswer == CMD_ID )
    {
        _idString = msg;
        _initValueCounter++;
        emit initSuccessful( msg );
    }
    else if( _expectedAnswer == CMD_UNIT )
    {
        if( msg.at( 0 ) == '0' )
        {
            _unit = "mbar";
        }
        else if( msg.at( 0 ) == '1' )
        {
            _unit = "Torr";
        }
        else if( msg.at( 0 ) == '2' )
        {
            _unit = "Pa";
        }
        else
        {
            _unit = msg;
        }
        _initValueCounter++;
        emit newPressureUnit( _unit );
    }
    else if( _expectedAnswer == CMD_PRESSURE )
    {
        if( msg.size() < 27 || !msg.contains( "," ) || !msg.contains( "E" ) )
        {
            emit portError( "Wrong answer format!" );
        }
        else
        {
            bool conversion1Successful = false;
            bool conversion2Successful = false;

            double pressureGauge1 = msg.mid( 2, 11 ).toDouble(
                        &conversion1Successful );
            double pressureGauge2 = msg.mid( 16, 11 ).toDouble(
                        &conversion2Successful );

            switch(  msg.at( 0 ) )
            {
                case 0: if( _emitGauge1 )
                {
                    if( conversion1Successful )
                    {
                        emit newPressureGauge1( pressureGauge1 );
                    }
                    else
                    {
                        emit portError( "Invalid data! (G1)" );
                    }
                }
                break;

                default: if( _emitGauge1 )
                {
                    emit newPressureGauge1( -msg.at( 0 ) );
                }
                break;
            }

            switch(  msg.at( 14 ) )
            {
                case 0: if( _emitGauge2 )
                {
                    if( conversion2Successful )
                    {
                        emit newPressureGauge2( pressureGauge2 );
                    }
                    else
                    {
                        emit portError( "Invalid data! (G2)" );
                    }
                }
                break;

                default: if( _emitGauge2 )
                {
                    emit newPressureGauge2( -msg.at( 14 ) );
                }
                break;
            }
        }
    }
    else if( _expectedAnswer == CMD_OTHER )
    {
    }
    else
    {
        emit portError( "Undefined answer!" );
    }
    nextMsg();
}
