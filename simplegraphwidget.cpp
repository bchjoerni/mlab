#include "simplegraphwidget.h"

simpleGraphWidget::simpleGraphWidget( QWidget *parent ) : QWidget( parent ),
    _xAxisType( axisType::axisLinear ), _yAxisType( axisType::axisLinear )
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

        if( _xAxisType == axisType::axisLog10 )
        {
            xMin = (xMin < std::numeric_limits<double>::epsilon() ?
                        0 : std::log10( xMin ));
            xMax = (xMax < std::numeric_limits<double>::epsilon() ?
                        0 : std::log10( xMax ));
        }
        if( _xAxisType == axisType::axisLn )
        {
            xMin = (xMin < std::numeric_limits<double>::epsilon() ?
                        0 : std::log( xMin ));
            xMax = (xMax < std::numeric_limits<double>::epsilon() ?
                        0 : std::log( xMax ));
        }

        if( _yAxisType == axisType::axisLog10 )
        {
            yMin = (yMin < std::numeric_limits<double>::epsilon() ?
                        0 : std::log10( yMin ));
            yMax = (yMax < std::numeric_limits<double>::epsilon() ?
                        0 : std::log10( yMax ));
        }
        if( _yAxisType == axisType::axisLn )
        {
            yMin = (yMin < std::numeric_limits<double>::epsilon() ?
                        0 : std::log( yMin ));
            yMax = (yMax < std::numeric_limits<double>::epsilon() ?
                        0 : std::log( yMax ));
        }

        for( unsigned int i = 0; i < (x > y ? y.size()-1 : x.size()-1); i++ )
        {
            double x1 = x[x.size()-1-i];
            double x2 = x[x.size()-2-i];

            double y1 = y[y.size()-1-i];
            double y2 = y[y.size()-2-i];

            if( _xAxisType == axisType::axisLog10 )
            {
                x1 = (x1 < std::numeric_limits<double>::epsilon() ?
                            0 : std::log10( x1 ));
                x2 = (x2 < std::numeric_limits<double>::epsilon() ?
                            0 : std::log10( x2 ));
            }
            if( _xAxisType == axisType::axisLn )
            {
                x1 = (x1 < std::numeric_limits<double>::epsilon() ?
                            0 : std::log( x1 ));
                x2 = (x2 < std::numeric_limits<double>::epsilon() ?
                            0 : std::log( x2 ));
            }

            if( _yAxisType == axisType::axisLog10 )
            {
                y1 = (y1 < std::numeric_limits<double>::epsilon() ?
                            0 : std::log10( y1 ));
                y2 = (y2 < std::numeric_limits<double>::epsilon() ?
                            0 : std::log10( y2 ));
            }
            if( _yAxisType == axisType::axisLn )
            {
                y1 = (y1 < std::numeric_limits<double>::epsilon() ?
                            0 : std::log( y1 ));
                y2 = (y2 < std::numeric_limits<double>::epsilon() ?
                            0 : std::log( y2 ));
            }

            painter.drawLine(
                        getXPosition( x1, xMin, xMax, textHeight ),
                        getYPosition( y1, yMin, yMax, textHeight ),
                        getXPosition( x2, xMin, xMax, textHeight ),
                        getYPosition( y2, yMin, yMax, textHeight ) );
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
                             std::abs(max-min)*(this->width() - distBorderLineLeft
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
                             (value - min)/std::abs(max-min)*
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

void simpleGraphWidget::setXAxisType( axisType type )
{
    _xAxisType = type;
}

void simpleGraphWidget::setYAxisType( axisType type )
{
    _yAxisType = type;
}
