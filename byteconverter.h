#ifndef BYTECONVERTER_H
#define BYTECONVERTER_H

#include <string>
#include <sstream>

class byteConverter
{
public:
    byteConverter();

    static std::string asciiToHexString( char binaryByte,
                                         unsigned int hexSize = 2 );
    static std::string hexStringToString( const std::string& s, int base = 2 );
    static std::string uCharToReadableHex( unsigned char uc );
    static char hexChar( unsigned char c );

    int   bytesToInt(   const std::string& s );
    float bytesToFloat( const std::string& s );

    std::string intToBytes(   int   param );
    std::string longToBytes(  int   param );
    std::string floatToBytes( float param );

private:
    bool littleEndian();
    void resetBuffer();
    void stringToBuffer( const std::string& s, int offset );

    char _buffer[8];
};
#endif // BYTECONVERTER_H
