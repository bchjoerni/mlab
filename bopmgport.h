#ifndef BOPMGPORT_H
#define BOPMGPORT_H

#include <QString>

#include <vector>
#include <string>

#include "easylogging++.h"
#include "labport.h"

class bopmgPort : public labPort
{
    Q_OBJECT

public:
    enum setValueType{ setTypeNone, setTypeVoltage, setTypeCurrent,
                      setTypePowerByVoltage, setTypePowerByCurrent };

    explicit bopmgPort( QObject *parent = 0 );
    void updateValues() override;
    void setValue( setValueType type, double value, bool autoAdjust = false );
    double minVoltage() const;
    double maxVoltage() const;
    double minCurrent() const;
    double maxCurrent() const;
    void setEmitVoltage( bool emitVoltage );
    void setEmitCurrent( bool emitCurrent );
    void setEmitPower( bool power );
    QString idString() const;

signals:
    void newVoltage( double volt );
    void newCurrent( double current );
    void newPower( double power );
    void initSuccessful( QString idString );

private slots:
    void receivedMsg( QByteArray msg );

private:
    void setSerialValues();
    void setLabPortVariables();
    void setCls();
    void setOutput( bool on );
    void setRemoteControl( bool on );
    void getIdn();
    void getMinVoltage();
    void getMaxVoltage();
    void getMinCurrent();
    void getMaxCurrent();
    void getInitValues() override;
    void getVoltage();
    void getCurrent();
    double calcAdjustedValue( double setValue, double lastValue );
    void adjustValues();
    void setVoltage( double voltage );
    void setCurrent( double current );
    void sendBopmgCmd( QString cmd, bool inTime = false );
    void updateNumInTimeValues();

    std::vector<QString> _expectedAnswer;
    QString _idString;
    double _minVoltage;
    double _maxVoltage;
    double _minCurrent;
    double _maxCurrent;
    double _lastVoltage;
    double _lastCurrent;
    double _lastPower;
    setValueType _setValueType;
    double _setVoltage;
    double _setCurrent;
    double _setPower;
    bool _autoAdjust;
    bool _emitVoltage;
    bool _emitCurrent;
    bool _emitPower;

    const double adjustmentFactor = 0.3;
    const QString CMD_IDN         = "*IDN?";
    const QString CMD_VOLTAGE_MIN = "VOLT?MIN";
    const QString CMD_VOLTAGE_MAX = "VOLT?MAX";
    const QString CMD_CURRENT_MIN = "CURR?MIN";
    const QString CMD_CURRENT_MAX = "CURR?MAX";
    const QString CMD_VOLTAGE     = "MEAS:VOLT?";
    const QString CMD_CURRENT     = "MEAS:CURR?";
    const QString CMD_OTHER       = "other";
};

#endif // BOPMGPORT_H
