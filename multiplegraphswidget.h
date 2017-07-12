#ifndef MULTIPLEGRAPHSWIDGET_H
#define MULTIPLEGRAPHSWIDGET_H


#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QString>

#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <cmath>

class multipleGraphsWidget : public QWidget
{
    Q_OBJECT

public:
    enum axisType { linear, log };

    explicit multipleGraphsWidget( QWidget *parent = 0 );
    void newValue( const QString& id, double value );
    void setNormalized( bool normalized );
    void setYAxis( axisType type );
    void clearData();
    void setRunning( bool running );
    void newSensor( const QString& name, bool draw );
    void setSensorDraw( const QString& name, bool draw );
    void deleteSensor( const QString& name );
    QColor getGraphColor( int index, int total );

protected:
    void paintEvent( QPaintEvent* event );
    void resizeEvent( QResizeEvent* event );

private:
    void drawAxes( QPainter *painter );
    void drawDataFromSensor( QPainter *painter, int sensorNum );
    QString getMinYValueString();
    QString getMaxYValueString();
    int getXPosition( double value, double min, double max, int textHeight );
    int getYPosition( double value, double min, double max, int textHeight );
    double getYAbsMax();
    double getYAbsMin();

    std::map<QString, std::vector<double> > _data;
    std::vector<std::pair<QString, bool> > _sensors;
    bool _normalized;
    axisType _yAxisType;

    int PRECISION_MAX_VALUES    = 4;
    int DIST_LEFT_BORDER_TEXT   = 1;
    int DIST_LEFT_TEXT_LINE     = 10;
    int DIST_BOTTOM_BORDER_TEXT = 10;
    int DIST_BOTTOM_TEXT_LINE   = 1;
    int DIST_RIGHT_BORDER_LINE  = 10;
    int DIST_TOP_BORDER_LINE    = 10;
};

#endif // MULTIPLEGRAPHSWIDGET_H
