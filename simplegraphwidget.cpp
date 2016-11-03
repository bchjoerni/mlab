#include "simplegraphwidget.h"

simpleGraphWidget::simpleGraphWidget( QWidget *parent ) : QWidget( parent )
{

}

void simpleGraphWidget::resizeEvent( QResizeEvent *event )
{
    update();
}

void simpleGraphWidget::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    drawAxes( &painter );

    painter.setPen( Qt::darkGray );
    std::vector<double>& x = _data[_xId];
    std::vector<double>& y = _data[_yId];
    if( x.size() > 1 && y.size() > 1 )
    {
        int textHeight = painter.fontMetrics().height();
        double xMin = *std::min_element( x.begin(), x.end() );
        double xMax = *std::max_element( x.begin(), x.end() );
        double yMin = *std::min_element( y.begin(), y.end() );
        double yMax = *std::max_element( y.begin(), y.end() );

        for( unsigned int i = 0; i < (x > y ? y.size()-1 : x.size()-1); i++ )
        {
            painter.drawLine( getXPosition( x[x.size()-1-i], xMin, xMax,
                              textHeight ),
                    getYPosition( y[y.size()-1-i], yMin, yMax, textHeight ),
                    getXPosition( x[x.size()-2-i], xMin, xMax, textHeight ),
                    getYPosition( y[y.size()-2-i], yMin, yMax, textHeight ) );
        }
    }
}

int simpleGraphWidget::getXPosition( double value, double min, double max,
                                  int textHeight )
{
    int distBorderLineLeft = DIST_LEFT_BORDER_TEXT + textHeight +
            DIST_LEFT_TEXT_LINE;

    if( min == max )
    {
        return distBorderLineLeft + (this->width() - distBorderLineLeft -
                                     DIST_RIGHT_BORDER_LINE)/2;
    }

    return static_cast<int>( distBorderLineLeft + 1 + (value - min)/
                             (max-min)*(this->width() - distBorderLineLeft
                                        - DIST_RIGHT_BORDER_LINE - 2) );
}

int simpleGraphWidget::getYPosition( double value, double min, double max,
                                  int textHeight)
{
    int distBorderLineBottom = DIST_BOTTOM_BORDER_TEXT + textHeight +
            DIST_BOTTOM_TEXT_LINE;

    if( min == max )
    {
        return DIST_TOP_BORDER_LINE + (this->height() - distBorderLineBottom -
                                       DIST_TOP_BORDER_LINE)/2;
    }

    return static_cast<int>( this->height() - distBorderLineBottom - 2 -
                             (value - min)/(max-min)*
                             (this->height() - DIST_TOP_BORDER_LINE
                                         - distBorderLineBottom - 2) );
}

void simpleGraphWidget::drawAxes( QPainter *painter )
{
    painter->fillRect( 0, 0, this->geometry().width(),
                       this->geometry().height(), Qt::white );

    QString value;
    painter->setPen( Qt::black );
    int textHeight = painter->fontMetrics().height();
    int distBorderLineLeft = DIST_LEFT_BORDER_TEXT + textHeight +
            DIST_LEFT_TEXT_LINE;
    int distBorderLineBottom = DIST_BOTTOM_BORDER_TEXT + textHeight +
            DIST_BOTTOM_TEXT_LINE;

    painter->drawLine( distBorderLineLeft,
                       this->height() - distBorderLineBottom,
                       this->width() - DIST_RIGHT_BORDER_LINE,
                       this->height() - distBorderLineBottom );
    painter->drawLine( distBorderLineLeft,
                       DIST_TOP_BORDER_LINE,
                       distBorderLineLeft,
                       this->height() - distBorderLineBottom );

    painter->drawText( distBorderLineLeft +
                       ((this->width() - distBorderLineLeft
                         - DIST_RIGHT_BORDER_LINE) -
                        painter->fontMetrics().width( _xId ) )/2,
                       this->height() - DIST_BOTTOM_BORDER_TEXT,
                       _xId );
    value = getMinValueString( _xId );
    painter->drawText( distBorderLineLeft,
                       this->height() - DIST_BOTTOM_BORDER_TEXT,
                       value );
    value = getMaxValueString( _xId );
    painter->drawText( this->width() - DIST_RIGHT_BORDER_LINE -
                       painter->fontMetrics().width( value ),
                       this->height() - DIST_BOTTOM_BORDER_TEXT,
                       value );

    painter->rotate( 270 );
    painter->drawText( -this->height() + distBorderLineBottom +
                       ((this->height() - DIST_TOP_BORDER_LINE -
                          distBorderLineBottom) -
                         painter->fontMetrics().width( _yId ))/2,
                       DIST_LEFT_BORDER_TEXT + textHeight, _yId );
    value = getMinValueString( _yId );
    painter->drawText( -(this->height() - distBorderLineBottom),
                       DIST_LEFT_BORDER_TEXT + textHeight, value );
    value = getMaxValueString( _yId );
    painter->drawText( -DIST_TOP_BORDER_LINE -
                       painter->fontMetrics().width( value ),
                       DIST_LEFT_BORDER_TEXT + textHeight, value );
    painter->rotate( -270 );
}

QString simpleGraphWidget::getMinValueString( const QString& id )
{
    if( _data[id].size() > 1 )
    {
        double minValue= *std::min_element( _data[id].begin(),
                                            _data[id].end() );
        return QString::number( minValue, 'g', PRECISION_MAX_VALUES );
    }
    return "";
}

QString simpleGraphWidget::getMaxValueString( const QString& id )
{
    if( _data[id].size() > 1 )
    {
        double maxValue= *std::max_element( _data[id].begin(),
                                            _data[id].end() );
        return QString::number( maxValue, 'g', PRECISION_MAX_VALUES );
    }
    return "";
}

void simpleGraphWidget::clearGraph()
{
    _data.clear();
    update();
}


void simpleGraphWidget::newValue( const QString& id, double value )
{
    _data[id].push_back( value );
}

void simpleGraphWidget::axisChanged( const QString& x, const QString& y )
{
    _xId = x;
    _yId = y;
}
