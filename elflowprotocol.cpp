#include "elflowprotocol.h"

elFlowProtocol::elFlowProtocol( int node ) :
    _node( node )
{
}

void elFlowProtocol::setNode( int node )
{
    _node = node;
}

std::string elFlowProtocol::getStatusCmd()
{
    return ":02" + byteConverter::asciiToHexString( _node, 2 ) + "01\r\n";
}

std::string elFlowProtocol::getFlowValueCmd()
{
    return getSingleGetParamCmd( 8 );
}

std::string elFlowProtocol::getCapacityValueCmd()
{
    return getSingleGetParamCmd( 21 );
}

std::string elFlowProtocol::getCapacityUnitCmd()
{
    return getSingleGetParamCmd( 15 );
}

std::string elFlowProtocol::getFluidNameCmd()
{
    return getSingleGetParamCmd( 25 );
}

std::string elFlowProtocol::getAliasCmd()
{
    return getSingleGetParamCmd( 115 );
}

std::string elFlowProtocol::getFlowValueSetCmd( float percent )
{
    return getSingleSendIntCmd( 9, static_cast<int>( percent*320 ) );
}


std::string elFlowProtocol::getSingleGetParamCmd( int paramNum )
{
    bool isString = false;
    char cmd[12];
    cmd[0] = 0x3A;
    cmd[1] = 6;
    cmd[2] = _node;
    cmd[3] = 0x04;
    cmd[4] = getProcess( paramNum );
    cmd[5] = getParamTypeHexCode( getParamType( paramNum ) )
            + getFBnr( paramNum );
    cmd[6] = getProcess( paramNum );
    cmd[7] = getParamTypeHexCode( getParamType( paramNum ) )
            + getFBnr( paramNum );
    if( getParamType( paramNum ) == 's' )
    {
        cmd[8] = getStringLength( paramNum );
        isString = true;
        cmd[1] = 7;
    }
    cmd[9] = 0x0D;
    cmd[10] = 0x0A;

    std::string cmdStr{ cmd[0] };
    for( int i = 1; i < 8; i++ )
    {
        cmdStr += byteConverter::asciiToHexString( cmd[i], 2 );
    }
    if( isString )
    {
        cmdStr += byteConverter::asciiToHexString( cmd[8], 2 );
    }
    cmdStr += cmd[9];
    cmdStr += cmd[10];

    return cmdStr;
}

int elFlowProtocol::getProcess( int paramNum )
{
    switch( paramNum )
    {
        case  1: return 0;
        case  2: return 0;
        case  3: return 0;
        case  4: return 0;
        case  5: return 0;
        case  6: return 0;
        case  7: return 0;
        case  8: return 1;
        case  9: return 1;
        case 10: return 1;
        case 11: return 1;
        case 12: return 1;
        case 13: return 1;
        case 14: return 1;
        case 15: return 1;
        case 16: return 1;
        case 17: return 1;
        case 18: return 1;
        case 19: return 1;
        case 20: return 1;
        case 21: return 1;
        case 22: return 1;
        case 23: return 1;
        case 24: return 1;
        case 25: return 1;
        case 26: return 1;
        case 27: return 1;
        case 28: return 1;

        case 115: return 113;
        default: throw elFlowProtocolException(
                    "Process number of parameter not found!" );
    }
}

int elFlowProtocol::getFBnr( int paramNum )
{
    switch( paramNum )
    {
        case  1: return 0;
        case  2: return 1;
        case  3: return 2;
        case  4: return 3;
        case  5: return 4;
        case  6: return 5;
        case  7: return 10;
        case  8: return 0;
        case  9: return 1;
        case 10: return 2;
        case 11: return 3;
        case 12: return 4;
        case 13: return 5;
        case 14: return 6;
        case 15: return 7;
        case 16: return 8;
        case 17: return 9;
        case 18: return 10;
        case 19: return 11;
        case 20: return 12;
        case 21: return 13;
        case 22: return 14;
        case 23: return 15;
        case 24: return 16;
        case 25: return 17;
        case 26: return 18;
        case 27: return 19;
        case 28: return 20;

        case 115: return 6;
        default: throw elFlowProtocolException(
                    "FB number of parameter not found!" );
    }
}

char elFlowProtocol::getParamType( int paramNum )
{
    switch( paramNum )
    {
        case  1: return 's';
        case  2: return 'c';
        case  3: return 'c';
        case  4: return 'c';
        case  5: return 'c';
        case  6: return 'c';
        case  7: return 'c';
        case  8: return 'i';
        case  9: return 'i';
        case 10: return 'i';
        case 11: return 'i';
        case 12: return 'c';
        case 13: return 'f';
        case 14: return 'f';
        case 15: return 'f';
        case 16: return 'f';
        case 17: return 'f';
        case 18: return 'f';
        case 19: return 'f';
        case 20: return 'f';
        case 21: return 'f';
        case 22: return 'c';
        case 23: return 'c';
        case 24: return 'c';
        case 25: return 's';
        case 26: return 'c';
        case 27: return 'c';
        case 28: return 'c';

        case 115: return 's';
        default: throw elFlowProtocolException(
                    "FB number of parameter not found!" );
    }
}

int elFlowProtocol::getParamTypeHexCode( char type )
{
    switch( type )
    {
        case 'c': return 0x00;
        case 'i': return 0x20;
        case 'l': return 0x40;
        case 'f': return 0x40;
        case 's': return 0x60;
        default: throw elFlowProtocolException(
                    "Hex code of type not found!" );
    }
}

int elFlowProtocol::getStringLength( int paramNum )
{
    switch( paramNum )
    {
        case 1: return 0;
        case 25: return 10;
        case 115: return 0;
        default: throw elFlowProtocolException(
                    "String length of parameter not found!" );
    }
}

std::string elFlowProtocol::getErrorMsg( int errorCode )
{
    switch( errorCode )
    {
        case 1: return "Error: general error!";
        case 2: return "Error: general error!";
        case 3: return "Error: protocol error!";
        case 4: return "Error: protocol error!";
        case 5: return "Error: wrong node!";
        case 8: return "Error: general error!";
        case 9: return "Error: timeout error!";
        default: throw elFlowProtocolException( "Unknown error message!" );
    }
}

std::string elFlowProtocol::getStatusMessage( int statusCode )
{
    switch( statusCode )
    {
        case 0: return "Status: no error.";

        case 22: return "Status: protocol error!";

        default: throw elFlowProtocolException( "Unknown status message!" );
    }
}

bool elFlowProtocol::checkAnswerForErrors( const std::string& answer,
                                            std::string* errorMsg )
{
    if( answer[0] != 0x3A )
    {
        (*errorMsg) = "Invalid answer!";
        return false;
    }

    if( answer.substr( 1, 2 ) == "01" )
    {
        (*errorMsg) = getErrorMsg( std::stoi( answer.substr( 3, 2 ) ) );
        return false;
    }

    if( answer.substr( 1, 2 ) == "04"
            && answer.substr( 5, 2 ) == "00" )
    {
        (*errorMsg) = getStatusMessage( std::stoi( answer.substr( 3, 2 ) ) );
        return false;
    }

    if( answer.substr( 5, 2 ) != "02" )
    {
        (*errorMsg) = "Unknown answer type!";
        return false;
    }

    return true;
}

bool elFlowProtocol::getTypeFromAnswer( char answerChar, char& type,
                                         int expectedParam,
                                         std::string* errorMsg )
{
    if( (answerChar & 0x60) == 0x60 )
    {
        type = 's';
    }
    else if( (answerChar & 0x40) == 0x40 )
    {
        if( getParamType( expectedParam ) == 'f' )
        {
            type = 'f';
        }
        else if( getParamType( expectedParam ) == 'l' )
        {
            type = 'l';
        }
        else
        {
            (*errorMsg) = "Error: Wrong answer!";
            return false;
        }
    }
    else if( (answerChar & 0x20) == 0x20 )
    {
        type = 'i';
    }
    else
    {
        type = 'c';
    }

    if( answerChar%32 != getFBnr( expectedParam ) )
    {
        (*errorMsg) = "Error: Wrong answer!";
        return false;
    }

    return true;
}

std::string elFlowProtocol::getValueString( const std::string& hexString,
                                             bool* ok )
{
    (*ok) = true;
    char zero = 0x0;

    if( hexString[0] == zero )
    {
        unsigned int stringEnd; // std::string::find takes 0x30 as 0x00
        for( stringEnd = 1; stringEnd < hexString.size(); stringEnd++ )
        {
            if( hexString[stringEnd] == zero )
            {
                break;
            }
        }
        if( stringEnd < hexString.size() )
        {
            return hexString.substr( 1, stringEnd-1 );
        }
        else
        {
            (*ok) = false;
            return "Format error!";
        }
    }

    if( hexString[0] > hexString.size()-1 )
    {
        (*ok) = false;
        return "Length error!";
    }

    return hexString.substr( 1, hexString[0] );
}

std::string elFlowProtocol::getSingleSendCharCmd( int paramNum, char param )
{
    char cmd[9];
    cmd[0] = 0x3A;
    cmd[1] = 5;
    cmd[2] = _node;
    cmd[3] = 0x01;
    cmd[4] = getProcess( paramNum );
    cmd[5] = getParamTypeHexCode( getParamType( paramNum ) )
            + getFBnr( paramNum );
    cmd[6] = param;
    cmd[7] = 0x0D;
    cmd[8] = 0x0A;

    std::string cmdStr{ cmd[0] };
    for( int i = 1; i < 7; i++ )
    {
        cmdStr += byteConverter::asciiToHexString( cmd[i], 2 );
    }
    cmdStr += cmd[7];
    cmdStr += cmd[8];

    return cmdStr;
}

std::string elFlowProtocol::getSingleSendIntCmd( int paramNum, int param )
{
    char cmd[10];
    cmd[0] = 0x3A;
    cmd[1] = 6;
    cmd[2] = _node;
    cmd[3] = 0x01;
    cmd[4] = getProcess( paramNum );
    cmd[5] = getParamTypeHexCode( getParamType( paramNum ) )
            + getFBnr( paramNum );

    byteConverter conv;
    cmd[6] = conv.intToBytes( param )[0];
    cmd[7] = conv.intToBytes( param )[1];
    cmd[8] = 0x0D;
    cmd[9] = 0x0A;

    std::string cmdStr{ cmd[0] };
    for( int i = 1; i < 8; i++ )
    {
        cmdStr += byteConverter::asciiToHexString( cmd[i], 2 );
    }
    cmdStr += cmd[8];
    cmdStr += cmd[9];

    return cmdStr;
}

std::string elFlowProtocol::getSingleSendLongCmd( int paramNum, int param )
{
    byteConverter conv;
    std::string longStr = conv.longToBytes( param );

    char cmd[12];
    cmd[0] = 0x3A;
    cmd[1] = 8;
    cmd[2] = _node;
    cmd[3] = 0x01;
    cmd[4] = getProcess( paramNum );
    cmd[5] = getParamTypeHexCode( getParamType( paramNum ) )
            + getFBnr( paramNum );
    cmd[6] = longStr[0];
    cmd[7] = longStr[1];
    cmd[8] = longStr[2];
    cmd[9] = longStr[3];
    cmd[10] = 0x0D;
    cmd[11] = 0x0A;

    std::string cmdStr{ cmd[0] };
    for( int i = 1; i < 10; i++ )
    {
        cmdStr += byteConverter::asciiToHexString( cmd[i], 2 );
    }
    cmdStr += cmd[10];
    cmdStr += cmd[11];

    return cmdStr;
}

std::string elFlowProtocol::getSingleSendFloatCmd( int paramNum, float param )
{
    byteConverter conv;
    std::string floatStr = conv.floatToBytes( param );

    char cmd[12];
    cmd[0] = 0x3A;
    cmd[1] = 8;
    cmd[2] = _node;
    cmd[3] = 0x01;
    cmd[4] = getProcess( paramNum );
    cmd[5] = getParamTypeHexCode( getParamType( paramNum ) )
            + getFBnr( paramNum );
    cmd[6] = floatStr[0];
    cmd[7] = floatStr[1];
    cmd[8] = floatStr[2];
    cmd[9] = floatStr[3];
    cmd[10] = 0x0D;
    cmd[11] = 0x0A;

    std::string cmdStr{ cmd[0] };
    for( int i = 1; i < 10; i++ )
    {
        cmdStr += byteConverter::asciiToHexString( cmd[i], 2 );
    }
    cmdStr += cmd[10];
    cmdStr += cmd[11];

    return cmdStr;
}

std::string elFlowProtocol::getSingleSendStringCmd( int paramNum,
                                                     const std::string& param )
{
    unsigned int stringLength = getStringLength( paramNum );
    bool definedSize = (stringLength != 0);

    char cmd[10];
    cmd[0] = 0x3A;
    if( definedSize )
    {
        cmd[1] = stringLength + 5;
    }
    else
    {
        cmd[1] = param.size() + 6;
    }
    cmd[2] = _node;
    cmd[3] = 0x01;
    cmd[4] = getProcess( paramNum );
    cmd[5] = getParamTypeHexCode( getParamType( paramNum ) )
            + getFBnr( paramNum );
    if( definedSize )
    {
        cmd[6] = stringLength;
    }
    else
    {
        cmd[6] = 0x00;
    }
    cmd[7] = 0x0D;
    cmd[8] = 0x0A;

    std::string cmdStr{ cmd[0] };
    for( int i = 1; i < 7; i++ )
    {
        cmdStr += byteConverter::asciiToHexString( cmd[i], 2 );
    }
    for( unsigned int i = 0; i < param.size(); i++ )
    {
        cmdStr += byteConverter::asciiToHexString( param[i], 2 );
    }
    if( definedSize )
    {
       for( unsigned int i = param.size(); i < stringLength; i++ )
       {
           cmdStr += "20"; // add spaces
       }
    }
    else
    {
        cmdStr += "00";
    }
    cmdStr += cmd[7];
    cmdStr += cmd[8];

    return cmdStr;
}

std::string elFlowProtocol::getHexAnswer( int paramNum,
                                           const std::string& answer,
                                           std::string* errorMsg )
{
    (*errorMsg) = "";
    if( !checkAnswerForErrors( answer, errorMsg ) )
    {
        return "";
    }

    std::string hexAnswer = ":" + byteConverter::hexStringToString(
                answer.substr( 1, answer.length()-1 ) );
    char type;
    if( !getTypeFromAnswer( hexAnswer[5], type, paramNum, errorMsg ) )
    {
        return "";
    }
    if( type != getParamType( paramNum ) )
    {
        (*errorMsg) = "Wrong type!";
        return "";
    }

    return hexAnswer;
}

bool elFlowProtocol::getSingleCharFromAnswer( char* param, int paramNum,
                                               const std::string& answer,
                                               std::string* errorMsg )
{
    std::string hexAnswer = getHexAnswer( paramNum, answer, errorMsg );
    if( hexAnswer == "" )
    {
        return false;
    }
    (*param) = hexAnswer[6];
    return true;
}

bool elFlowProtocol::getSingleIntFromAnswer( int* param, int paramNum,
                                              const std::string& answer,
                                              std::string* errorMsg )
{
    std::string hexAnswer = getHexAnswer( paramNum, answer, errorMsg );
    if( hexAnswer == "" )
    {
        return false;
    }
    byteConverter conv;
    (*param) = conv.bytesToInt( hexAnswer.substr( 6, 2 ) );
    return true;
}

bool elFlowProtocol::getSingleLongFromAnswer( int* param, int paramNum,
                                               const std::string& answer,
                                               std::string* errorMsg )
{
    std::string hexAnswer = getHexAnswer( paramNum, answer, errorMsg );
    if( hexAnswer == "" )
    {
        return false;
    }
    byteConverter conv;
    (*param) = conv.bytesToInt( hexAnswer.substr( 6, 4 ) );
    return true;
}

bool elFlowProtocol::getSingleFloatFromAnswer( float* param, int paramNum,
                                                const std::string& answer,
                                                std::string* errorMsg )
{
    std::string hexAnswer = getHexAnswer( paramNum, answer, errorMsg );
    if( hexAnswer == "" )
    {
        return false;
    }
    byteConverter conv;
    (*param) = conv.bytesToFloat( hexAnswer.substr( 6, 4 ) );
    return true;
}

bool elFlowProtocol::getSingleStringFromAnswer( std::string* param,
                                                 int paramNum,
                                                 const std::string& answer,
                                                 std::string* errorMsg )
{
    std::string hexAnswer = getHexAnswer( paramNum, answer, errorMsg );
    if( hexAnswer == "" )
    {
        return false;
    }

    bool ok = false;
    (*param) = getValueString( hexAnswer.substr( 6 ), &ok );
    if( !ok )
    {
        (*errorMsg) = (*param);
        (*param) = "";
    }

    return ok;
}

std::string elFlowProtocol::getStringInterpretation( const std::string& answer,
                                                      int paramNum )
{
    char c;
    int i;
    float f;
    std::string s;

    std::string error = "";
    switch( getParamType( paramNum ) )
    {
        case 'c': getSingleCharFromAnswer( &c, paramNum, answer, &error );
                  s = std::to_string( c );
            break;
        case 'i': getSingleIntFromAnswer( &i, paramNum, answer, &error );
                  s = std::to_string( i );
            break;
        case 'l': getSingleLongFromAnswer( &i, paramNum, answer, &error );
                  s = std::to_string( i );
            break;
        case 'f': getSingleFloatFromAnswer( &f, paramNum, answer, &error );
                  s = std::to_string( f );
            break;
        case 's': getSingleStringFromAnswer( &s, paramNum, answer, &error );
            break;
        default: error = "Unknown type!";
    }

    if( error != "" )
    {
        return error;
    }
    return s;
}
