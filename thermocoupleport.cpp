#include "thermocoupleport.h"

thermocouplePort::thermocouplePort( QObject *parent ) : labPort( parent ),
    _sendCounter( 0 ), _answerPending( -1 ),
    _emitProbeTemperature( false ), _emitAmbientTemperature( false ),
    _idStringSet( false ), _probeTemperatureOnly( true )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
    connect( &_checkForAnswerTimer, SIGNAL( timeout() ), this,
             SLOT( nextMsg() ) );
}

void thermocouplePort::setSerialValues()
{
    _port.setBaudRate( QSerialPort::Baud38400 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::NoParity );
}

void thermocouplePort::setLabPortVariables()
{
    _initTimeoutMs      = 1500;
    _initValueCounter   = 0;
    _numInitValues      = 2;
    _minBytesRead       = -10;
    _writingPauseMs     = 150;
    _bytesError         = 20;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void thermocouplePort::getInitValues()
{
    getId();
}

void thermocouplePort::updateValues()
{
    checkInTimeCount();
    if( _expectedAnswer.size() != 0 )
    {
        _expectedAnswer.clear();
        if( !_closing )
        {
            emit portError( "Answer missing!" );
        }
    }

    if( _emitProbeTemperature || _emitAmbientTemperature )
    {
        getTemperatures();
    }
}

void thermocouplePort::setEmitProbeTemperature( bool emitProbeTemperature )
{
    _emitProbeTemperature = emitProbeTemperature;
}

void thermocouplePort::setEmitAmbientTemperature( bool emitAmbientTemperature )
{
    _emitAmbientTemperature = emitAmbientTemperature;
}

void thermocouplePort::getId()
{
    _idStringSet = false;
    sendUtcCmd( CMD_ID, 2 );
}

QString thermocouplePort::idString() const
{
    return _idString;
}

void thermocouplePort::getTemperatures()
{
    if( _probeTemperatureOnly )
    {
        sendUtcCmd( CMD_PROBE_TEMPERATURE, 1 );
    }
    else
    {
        sendUtcCmd( CMD_BOTH_TEMPERATURES, 1 );
    }
}

void thermocouplePort::sendUtcCmd( QString cmd, int numAnswers )
{
    for( int i = 0; i < numAnswers; i++ )
    {
        if( cmd != CMD_ID
                && cmd != CMD_PROBE_TEMPERATURE
                && cmd != CMD_BOTH_TEMPERATURES )
        {
            _expectedAnswer.push_back( CMD_OTHER );
        }
        else
        {
            _expectedAnswer.push_back( cmd );
        }
    }

    _msgToSend.push_back( (numAnswers > 0 ? "y" : "n") + cmd + "\r\n" );

    if( !_checkForAnswerTimer.isActive() )
    {
        _answerPending = -1;
        nextMsg();
    }
}

void thermocouplePort::nextMsg()
{
    if( _answerPending == 1 )
    {
        noAnswerReceived();
    }
    else
    {
        if( _msgToSend.size() == 0 )
        {
            _checkForAnswerTimer.stop();
        }
        sendNextMessage();
    }
}

void thermocouplePort::noAnswerReceived()
{
    if( _msgToSend.size() == 0 )
    {
        return;
    }

    if( _sendCounter < TRIES_SEND_MSG )
    {
        _sendCounter++;
        sendMsg( _msgToSend[0].toStdString().c_str(), _msgToSend[0].size(),
                 false );
        _checkForAnswerTimer.start( _writingPauseMs*2 );
    }
    else
    {
        if( !_closing )
        {
            emit portError( "No answer after several requests!" );
        }
        _sendCounter = 0;
        _msgToSend.erase( _msgToSend.begin() );
        _expectedAnswer.erase( _expectedAnswer.begin() );
        _answerPending = -1;
        if( _msgToSend.size() > 0 )
        {
            _checkForAnswerTimer.start( _writingPauseMs*2 );
        }
    }
}

void thermocouplePort::sendNextMessage()
{
    _sendCounter = 0;
    if( _answerPending != -1 && _msgToSend.size() > 0 )
    {
        _msgToSend.erase( _msgToSend.begin() );
    }

    if( _msgToSend.size() > 0 )
    {
        _answerPending = (_msgToSend[0].at( 0 ) == 'n') ? 0 : 1;
        _msgToSend[0] = _msgToSend[0].mid( 1, -1 );
        sendMsg( _msgToSend[0].toStdString().c_str(), _msgToSend[0].size(),
                 false );
        _checkForAnswerTimer.start( _writingPauseMs*2 );
    }
    else
    {
        _checkForAnswerTimer.stop();
    }
}

void thermocouplePort::receivedMsg( QByteArray msg )
{
    if( msg.isNull() )
    {
        return;
    }
    msg.replace( 0x0A, "" ).replace( 0x0B, "" ).replace( 0x0D, "" )
            .replace( 0x11, "" ).replace( 0x13, "" )
            .replace( 0x3C, "" ).replace( 0x3E, "" ); // < and > - dont know why this is sent ...
    if( msg.isEmpty() )
    {
        return;
    }

    if( _expectedAnswer.size() == 0 )
    {
        if( _sendCounter != 0 )
        {
            return;
        }
        emit portError( "Unexpected answer!" );
        return;
    }
    _answerPending = 0;

    if( _expectedAnswer[0] == CMD_ID )
    {
        if( _idStringSet )
        {
            bool conversionSuccessful = false;
            int version = msg.toInt( &conversionSuccessful );
            if( conversionSuccessful && version > 90615 )
            {
                _probeTemperatureOnly = false;
            }
            _initValueCounter++;
            emit initSuccessful( _idString );
        }
        else
        {
            _idStringSet = true;
            _idString = msg;
            _initValueCounter++;
        }
    }
    else if( _expectedAnswer[0] == CMD_PROBE_TEMPERATURE )
    {
        bool conversionSuccessful = false;

        double probeTemperatureCelsius = msg.toDouble( &conversionSuccessful );

        if( conversionSuccessful )
        {
            if( _emitProbeTemperature )
            {
                if( probeTemperatureCelsius > MAX_TEMPERATURE
                        || probeTemperatureCelsius < -MAX_TEMPERATURE )
                {
                    emit portError( "Out of range!" );
                }
                else
                {
                    emit newProbeTemperature( probeTemperatureCelsius );
                }
            }
        }
        else
        {
            emit portError( "Could not get temperature from answer!" );
        }
    }
    else if( _expectedAnswer[0] == CMD_BOTH_TEMPERATURES )
    {
        bool conversion1Successful = false;
        bool conversion2Successful = false;

        double probeTemperatureFahrenheit = msg.left( msg.indexOf( "," ) )
                .toDouble( &conversion1Successful );
        double ambientTemperatureFahrenheit = msg.mid( msg.indexOf( "," ) + 1 )
                .toDouble( &conversion2Successful );

        if( conversion1Successful && conversion2Successful )
        {
            int tProbeCelsius = fahrenheitToCelsius(
                        probeTemperatureFahrenheit );
            int tAmbientCelsius = fahrenheitToCelsius(
                        ambientTemperatureFahrenheit );
            if( _emitProbeTemperature )
            {
                if( tProbeCelsius > MAX_TEMPERATURE
                        || tProbeCelsius < -MAX_TEMPERATURE )
                {
                    emit portError( "Out of range!" );
                }
                else
                {
                    emit newProbeTemperature( tProbeCelsius );
                }
            }
            if( _emitAmbientTemperature )
            {
                if( tAmbientCelsius > MAX_TEMPERATURE
                        || tAmbientCelsius < -MAX_TEMPERATURE )
                {
                    emit portError( "Out of range!" );
                }
                else
                {
                    emit newAmbientTemperature( tAmbientCelsius );
                }
            }
        }
        else
        {
            emit portError( "Could not get temperatures from answer!" );
        }
    }
    else if( _expectedAnswer[0] == CMD_OTHER )
    {
    }
    else
    {
        emit portError( "Undefined answer!" );
    }

    _expectedAnswer.erase( _expectedAnswer.begin() );
}

double thermocouplePort::fahrenheitToCelsius( double temperatureFahrentheit )
{
    return (static_cast<int>( (temperatureFahrentheit - 32.0)*5.0/9.0*100 ))
            /100.0; // return with max two decimals
}

