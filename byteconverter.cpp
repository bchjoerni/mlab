#include "byteconverter.h"


byteConverter::byteConverter() : _buffer{ 0, 0, 0, 0, 0, 0, 0, 0 }
{
}

void byteConverter::resetBuffer()
{
    for( unsigned int i = 0; i < 8; i++ )
    {
        _buffer[i] = 0;
    }
}

std::string byteConverter::asciiToHexString( char binaryByte,
                                             unsigned int hexSize )
{
    std::stringstream ss;
    ss << std::hex << static_cast<int>(
              reinterpret_cast<unsigned char&>( binaryByte ) );
    std::string s = ss.str();

    while( s.size() < hexSize )
    {
        s = "0" + s;
    }

    return s;
}

std::string byteConverter::hexStringToString( const std::string& s, int base )
{
    std::string convertedString;
    for( unsigned int i = 0; i <= s.length()-base; i += base )
    {
        std::string hexChar = "0x" + s.substr( i, base );
        std::stringstream ss;
        ss << std::hex << hexChar;
        int value;
        ss >> value;
        convertedString += static_cast<char>( value );
    }
    return convertedString;
}

int byteConverter::bytesToInt( const std::string& s )
{
    stringToBuffer( s, sizeof( int ) - s.length() );
    int number = (*((int*) &_buffer));
    resetBuffer();
    return number;
}

float byteConverter::bytesToFloat( const std::string& s )
{
    stringToBuffer( s, sizeof( float ) - s.length() );
    float number = (*((float*) &_buffer));
    resetBuffer();
    return number;
}


bool byteConverter::littleEndian()
{
    unsigned int i = 1;
    char *c = (char*) &i;
    if( (*c) )
    {
        return true;
    }
    return false;
}

void byteConverter::stringToBuffer( const std::string& s, int offset )
{
    if( s.size() + offset > 8 )
    {
        return;
    }

    if( littleEndian() )
    {
        for( unsigned int i = 0; i < s.size(); i++ )
        {
            _buffer[i] = s[s.size()-i-1];
        }
    }
    else
    {
        for( unsigned int i = 0; i < s.size(); i++ )
        {
            _buffer[i+offset] = s[i];
        }
    }
}

std::string byteConverter::intToBytes( int param )
{
    char* bytes = (char*) &param;
    std::string intStr;

    if( littleEndian() )
    {
        intStr += bytes[1];
        intStr += bytes[0];
    }
    else
    {
        intStr += bytes[0];
        intStr += bytes[1];
    }

    return intStr;
}

std::string byteConverter::longToBytes( int param )
{
    char* bytes = (char*) &param;
    std::string intStr;

    if( littleEndian() )
    {
        intStr += bytes[3];
        intStr += bytes[2];
        intStr += bytes[1];
        intStr += bytes[0];
    }
    else
    {
        intStr += bytes[0];
        intStr += bytes[1];
        intStr += bytes[2];
        intStr += bytes[3];
    }

    return intStr;
}

std::string byteConverter::floatToBytes( float param )
{
    char* bytes = (char*) &param;
    std::string floatStr;

    if( littleEndian() )
    {
        floatStr += bytes[3];
        floatStr += bytes[2];
        floatStr += bytes[1];
        floatStr += bytes[0];
    }
    else
    {
        floatStr += bytes[0];
        floatStr += bytes[1];
        floatStr += bytes[2];
        floatStr += bytes[3];
    }

    return floatStr;
}

char byteConverter::hexChar( unsigned char c )
{
    switch( c )
    {
        case 0:  return '0';
        case 1:  return '1';
        case 2:  return '2';
        case 3:  return '3';
        case 4:  return '4';
        case 5:  return '5';
        case 6:  return '6';
        case 7:  return '7';
        case 8:  return '8';
        case 9:  return '9';
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
        default: return '?';
    }
}

std::string byteConverter::uCharToReadableHex( unsigned char uc )
{
    std::string hexStr = "";
    hexStr += hexChar( uc/16 );
    hexStr += hexChar( uc%16 );
    return hexStr;
}

