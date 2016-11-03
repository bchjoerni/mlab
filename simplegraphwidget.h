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
    int getXPosition( double value, double min, double max, int textHeight );
    int getYPosition( double value, double min, double max, int textHeight );

    std::map<QString, std::vector<double> > _data;
    QString _xId;
    QString _yId;

    int PRECISION_MAX_VALUES    = 4;
    int DIST_LEFT_BORDER_TEXT   = 1;
    int DIST_LEFT_TEXT_LINE     = 10;
    int DIST_BOTTOM_BORDER_TEXT = 10;
    int DIST_BOTTOM_TEXT_LINE   = 1;
    int DIST_RIGHT_BORDER_LINE  = 10;
    int DIST_TOP_BORDER_LINE    = 10;
};

#endif // SIMPLEGRAPHWIDGET_H
