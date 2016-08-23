#ifndef TPG26XPORT_H
#define TPG26XPORT_H

#include <QObject>

#include "easylogging++.h"
#include "labport.h"

class tpg26xPort : public labPort
{
    Q_OBJECT

public:
    explicit tpg26xPort( QObject *parent = 0 );
    void updateValues() override;
    void setEmitGauge1( bool emitGauge1 );
    void setEmitGauge2( bool emitGauge2 );
    QString idString() const;

signals:
    void newPressureUnit( QString unit );
    void newPressureGauge1( double pressure );
    void newPressureGauge2( double pressure );

private slots:
    void receivedMsg( QByteArray msg );
    void nextMsg();
    void waitForSendingTimeout();

private:
    void setSerialValues();
    void setLabPortVariables();
    void getInitValues() override;
    void getId();
    void getUnit();
    void getPressure();
    void sendTpg26xCmd( QString cmd );
    void sendNextMessage();

    std::vector<QString> _msgToSend;
    QString _expectedAnswer;
    QTimer  _checkForAnswerTimer;
    QString _idString;
    QString _unit;

    bool _readyToSend;
    bool _emitGauge1;
    bool _emitGauge2;

    const QString CMD_ID       = "TID";
    const QString CMD_UNIT     = "UNI";
    const QString CMD_PRESSURE = "PRX";
    const QString CMD_OTHER    = "other";
};

#endif // TPG26XPORT_H
