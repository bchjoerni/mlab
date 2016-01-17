#ifndef SIMPLEGRAPHWIDGET_H
#define SIMPLEGRAPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QString>

#include <QDebug>

#include <vector>
#include <map>
#include <algorithm>

class simpleGraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit simpleGraphWidget(QWidget *parent = 0);
    void newValue( const QString& id, double value );
    void axisChanged( const QString& x, const QString& y );
    void clearGraph();
    void setRunning( bool running );

protected:
    void paintEvent( QPaintEvent* event );
    void resizeEvent( QResizeEvent* event );

private:
    void drawAxes( QPainter *painter );
    QString getMinValueString( const QString& id );
    QString getMaxValueString( const QString& id );
    int getXPosition( double value, double min, double max );
    int getYPosition( double value, double min, double max );

    std::map<QString, std::vector<double> > _data;
    QString _xId;
    QString _yId;

    const int LEFT_BORDER   = 30;
    const int BOTTOM_BORDER = 30;
    const int RIGHT_BORDER  = 10;
    const int TOP_BORDER    = 10;
    const int TEXT_DISTANCE_LEFT   = 20;
    const int TEXT_DISTANCE_BOTTOM = 10;
    const int TEXT_START_FACTOR    = 3;
    const int NUM_CHARS_MAX_VALUE  = 6;
    const int CHAR_WIDTH           = 6;
};

#endif // SIMPLEGRAPHWIDGET_H
