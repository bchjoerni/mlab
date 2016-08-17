#ifndef ELFLOWPORT_H
#define ELFLOWPORT_H

#include <QObject>
#include <QString>
#include <QByteArray>

#include "easylogging++.h"
#include "labport.h"
#include "elflowprotocol.h"

class elFlowPort : public labPort
{
    Q_OBJECT

public:
    enum setValueType{ setTypeNone, setTypeFlow };

    explicit elFlowPort( QObject *parent = 0 );
    void updateValues() override;
    void setValue( setValueType type, double value, bool autoAdjust = false );
    void setId( int id );
    QString idString() const;
    double maxCapacity() const;
    void setEmitFlow( bool emitFlow );
    void setEmitPressure( bool emitPressure );
    void setEmitTemperature( bool emitTemperature );

signals:
    void newFlow( double flow );
    void newPressure( double pressure );
    void newTemperature( double temperature );

private slots:
    void receivedMsg( QByteArray msg );

private:
    void getInitValues() override;
    void setSerialValues();
    void setLabPortVariables();
    void sendCmd( const std::string& cmd );
    void getFlow();
    void getPressure();
    void getTemperature();
    void decodeMsg( const QByteArray& msg );
    void showParameter( int paramNum, double value, const std::string& text );

    elFlowProtocol _protocol;
    bool _inMsg;
    QByteArray _buffer;
    QString _idString;
    double _maxCapacity;
    setValueType _setValueType;
    double _setFlowPercent;
    bool _autoAdjust;
    bool _emitFlow;
    bool _emitPressure;
    bool _emitTemperature;
};

#endif // ELFLOWPORT_H
