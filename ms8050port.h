#ifndef MS8050PORT_H
#define MS8050PORT_H

#include <QObject>

#include "easylogging++.h"
#include "labport.h"

class ms8050Port : public labPort
{
    Q_OBJECT

public:
    explicit ms8050Port( QObject *parent = 0 );
    void updateValues() override;
    void setEmitValue( bool emitValue );
    QString idString() const;

signals:
    void newValue( double value, char unit );
    void newHold( bool hold );
    void newMinMax( char minMax );
    void newRelative( bool relative );

private slots:
    void receivedMsg( QByteArray msg );

private:
    void setSerialValues();
    void setLabPortVariables();
    void getInitValues() override;
    void interpretMessage( const QByteArray& msg );

    QString _idString;
    bool _emitValue;
};

#endif // MS8050PORT_H
