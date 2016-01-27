#ifndef TSH071PORT_H
#define TSH071PORT_H

#include <QString>
#include <QDebug>

#include <string>

#include "easylogging++.h"
#include "labport.h"


class tsh071Port : public labPort
{
    Q_OBJECT

public:
    enum setValueType{ setTypeNone, setTypeRpm };

    explicit tsh071Port( QObject *parent = 0 );
    void updateValues() override;
    void startPump();
    void stopPump();
    void setValue( setValueType type, double value, bool autoAdjust = false );
    void setEmitRpm( bool emitRpm );
    void setEmitPressure( bool emitPressure );
    QString idString() const;

signals:
    void initSuccessful( QString idString );
    void newRpm( int rpm );
    void newPressure( double pressure );

private slots:
    void receivedMsg( QByteArray msg );

private:
    void setSerialValues();
    void setLabPortVariables();
    void getInitValues() override;
    int calcCheckSum( const std::string& cmd );
    std::string addZerosToSize( const std::string& s, unsigned int size );
    void getDrvName();
    void getRotation();
    void getPressure();
    void interpretMsg();
    bool checkAnswerForErrors();

    QString _idString;
    QString _receivedMsg;
    int _id;
    bool _emitRpm;
    bool _emitPressure;
};

#endif // TSH071PORT_H
