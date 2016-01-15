#ifndef EAPSPORT_H
#define EAPSPORT_H

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QTimer>
#include <QTime>
#include <QApplication>

#include "easylogging++.h"
#include "labport.h"

class eapsPort : public labPort
{
    Q_OBJECT

public:
    enum eapsSetValues{ none, voltage, current };

    explicit eapsPort( QObject *parent = 0 );
    void open( const QString& portName );
    void getUpdateValues() override;
    void setValue( eapsSetValues type, double value, bool autoAdjust );
    void setEmitVoltage( bool emitVoltage );
    void setEmitCurrent( bool emitCurrent );
    void setEmitPower( bool power );

signals:
    void newVoltage( double volt );
    void newCurrent( double current );
    void newPower( double power );
    void initSuccessful( QString idString );

private slots:
    void receivedMsg( QByteArray msg );

private:
    void getIdn();
    void getMaxValues();
    void getIdString();
    void setRemoteControl( bool on = true);
    void getInitValues() override;
    void getVoltage();
    void getCurrent();
    void setVoltage( double voltage );
    void setCurrent( double current );
    void setToPrototype( unsigned char* msg );
    void setCheckBytes( unsigned char* msg );
    int calcCheckBytes( unsigned char* msg );
    bool checkMsgBytes( unsigned char* msg );

    labPort _port;
    QString _idString;
    int _id;
    int _maxVoltageOut;    
    int _maxCurrentOut;
    int _maxVoltageIn;
    int _maxCurrentIn;
    int _initCount;
    double _maxVoltage;
    double _maxCurrent;
    double _lastVoltage;
    double _lastCurrent;
    eapsSetValues _setValueType;
    double _setVoltage;
    double _setCurrent;
    bool _autoAdjust;
    bool _emitVoltage;
    bool _emitCurrent;
    bool _emitPower;

    const static int MESSAGE_LENGTH = 24;
    const unsigned char MESSAGE_PROTOTYPE[MESSAGE_LENGTH]
            = { 170, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
};

#endif // EAPSPORT_H
