#ifndef ELFLOWPORT_H
#define ELFLOWPORT_H

#include <QObject>
#include <QString>

#include "easylogging++.h"
#include "labport.h"

class elFlowPort : public labPort
{
    Q_OBJECT

public:
    enum setValueType{ setTypeNone };

    explicit elFlowPort( QObject *parent = 0 );
    void updateValues() override;
    void setValue( setValueType type, double value );
    void setId( int id );
    QString idString() const;

signals:
    void initSuccessful( QString idString );

private slots:
    void receivedMsg( QByteArray msg );

private:
    void getInitValues() override;
    void setSerialValues();
    void setLabPortVariables();

    QString _idString;
    int _id;
};

#endif // ELFLOWPORT_H
