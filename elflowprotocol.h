#ifndef ELFLOWPROTOCOL_H
#define ELFLOWPROTOCOL_H

#include <string>
#include <stdexcept>

#include "byteconverter.h"

class elFlowProtocolException : public std::runtime_error
{
public:
    elFlowProtocolException( const std::string& msg )
        : std::runtime_error( msg )
    {
    }
};

class elFlowProtocol
{
public:
    elFlowProtocol( int node = 128 );
    void setNode( int node );

    inline int baudRate() { return 38400; }
    inline int dataBits() { return 8; }
    inline int parity()   { return 0; } // no parity
    inline int stopBits() { return 1; } // one stop bit

    std::string getStringInterpretation( const std::string& answer,
                                         int paramNum );

    std::string getStatusCmd();
    std::string getFlowValueCmd();
    std::string getCapacityValueCmd();
    std::string getCapacityUnitCmd();
    std::string getFluidNameCmd();
    std::string getAliasCmd();

    std::string getFlowValueSetCmd( float percent );


    std::string getSingleGetParamCmd( int paramNum );

    std::string getSingleSendCharCmd(   int paramNum, char param  );
    std::string getSingleSendIntCmd(    int paramNum, int param   );
    std::string getSingleSendLongCmd(   int paramNum, int param   );
    std::string getSingleSendFloatCmd(  int paramNum, float param );
    std::string getSingleSendStringCmd( int paramNum,
                                        const std::string& param  );

    bool getSingleCharFromAnswer(   char* param, int paramNum,
                                    const std::string& answer,
                                    std::string* errorMsg );
    bool getSingleIntFromAnswer(    int* param, int paramNum,
                                    const std::string& answer,
                                    std::string* errorMsg );
    bool getSingleLongFromAnswer(   int* param, int paramNum,
                                    const std::string& answer,
                                    std::string* errorMsg );
    bool getSingleFloatFromAnswer(  float* param, int paramNum,
                                    const std::string& answer,
                                    std::string* errorMsg );
    bool getSingleStringFromAnswer( std::string* param, int paramNum,
                                    const std::string& answer,
                                    std::string* errorMsg );

private:
    int getProcess( int paramNum );
    int getFBnr( int paramNum );
    char getParamType( int paramNum );
    int getParamTypeHexCode( char type );
    int getStringLength( int paramNum );
    std::string getErrorMsg( int errorCode );
    std::string getStatusMessage( int statusCode );
    bool checkAnswerForErrors( const std::string& answer,
                               std::string* errorMsg );
    bool getTypeFromAnswer( char answerChar, char& type, int expectedParam,
                            std::string* errorMsg );
    std::string getValueString( const std::string& hexString, bool* ok );
    std::string getHexAnswer( int paramNum, const std::string& answer,
                              std::string* errorMsg );

    int _node;
};

#endif // ELFLOWPROTOCOL_H
