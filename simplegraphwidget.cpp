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
        double xMin = *std::min_element( x.begin(), x.end() );
        double xMax = *std::max_element( x.begin(), x.end() );
        double yMin = *std::min_element( y.begin(), y.end() );
        double yMax = *std::max_element( y.begin(), y.end() );

        for( unsigned int i = 0; i < (x > y ? y.size()-1 : x.size()-1); i++ )
        {
            painter.drawLine( getXPosition( x[x.size()-1-i], xMin, xMax ),
                    getYPosition( y[y.size()-1-i], yMin, yMax ),
                    getXPosition( x[x.size()-2-i], xMin, xMax ),
                    getYPosition( y[y.size()-2-i], yMin, yMax ) );
        }
    }
}

int simpleGraphWidget::getXPosition( double value, double min, double max )
{
    if( min == max )
    {
        return LEFT_BORDER + (this->height() - LEFT_BORDER - RIGHT_BORDER)/2;
    }

    return static_cast<int>( LEFT_BORDER + 1 + (value - min)/
                             (max-min)*(this->width() - LEFT_BORDER
                                        - RIGHT_BORDER - 2) );
}

int simpleGraphWidget::getYPosition( double value, double min, double max )
{
    if( min == max )
    {
        return TOP_BORDER + (this->height() - BOTTOM_BORDER - TOP_BORDER)/2;
    }

    return static_cast<int>( this->height() - BOTTOM_BORDER - 2 - (value - min)/
                              (max-min)*(this->height() - TOP_BORDER
                                         - BOTTOM_BORDER - 2) );
}

void simpleGraphWidget::drawAxes( QPainter *painter )
{
    painter->fillRect( 0, 0, this->geometry().width(),
                       this->geometry().height(), Qt::white );

    QString value;
    painter->setPen( Qt::black );
    painter->drawLine( LEFT_BORDER,
                       this->height() - BOTTOM_BORDER,
                       this->width() - RIGHT_BORDER,
                       this->height() - BOTTOM_BORDER );
    painter->drawLine( LEFT_BORDER, TOP_BORDER, LEFT_BORDER,
                       this->height() - BOTTOM_BORDER );

    painter->drawText( this->width()/3,
                       this->height() - TEXT_DISTANCE_BOTTOM,
                       _xId );
    value = getMinValueString( _xId );
    painter->drawText( LEFT_BORDER, this->height() - TEXT_DISTANCE_BOTTOM,
                       value );
    value = getMaxValueString( _xId );
    painter->drawText( this->width() - RIGHT_BORDER - value.size()*CHAR_WIDTH,
                       this->height() - TEXT_DISTANCE_BOTTOM,
                       value );

    painter->rotate( 270 );
    painter->drawText( -(this->height() - BOTTOM_BORDER)*2/3,
                       TEXT_DISTANCE_LEFT, _yId );
    value = getMinValueString( _yId );
    painter->drawText( -(this->height() - BOTTOM_BORDER),
                       TEXT_DISTANCE_LEFT, value );
    value = getMaxValueString( _yId );
    painter->drawText( -TOP_BORDER - value.size()*CHAR_WIDTH,
                       TEXT_DISTANCE_LEFT, value );
    painter->rotate( -270 );
}

QString simpleGraphWidget::getMinValueString( const QString& id )
{
    if( _data[id].size() > 1 )
    {
        double minValue= *std::min_element( _data[id].begin(),
                                            _data[id].end() );
        return QString::number( minValue ).left( NUM_CHARS_MAX_VALUE );
    }
    return "";
}

QString simpleGraphWidget::getMaxValueString( const QString& id )
{
    if( _data[id].size() > 1 )
    {
        double maxValue= *std::max_element( _data[id].begin(),
                                            _data[id].end() );
        return QString::number( maxValue ).left( NUM_CHARS_MAX_VALUE );
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
