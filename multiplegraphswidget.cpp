#include "multiplegraphswidget.h"

multipleGraphsWidget::multipleGraphsWidget( QWidget *parent )
    : QWidget( parent ), _normalized( false )
{

}

void multipleGraphsWidget::resizeEvent( QResizeEvent *event )
{
    update();
}

QColor multipleGraphsWidget::getGraphColor( int index, int total )
{
    if( total > 1 )
    {
        total--;
    }

    if( index > total )
    {
        index = total;
    }

    return QColor::fromRgb( 0, 255-255*index/total, 255*index/total );
}

void multipleGraphsWidget::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    drawAxes( &painter );

    double yAbsMax = 1.0;
    double yAbsMin = 0.0;

    if( !_normalized )
    {
        yAbsMax = getYAbsMax();
        yAbsMin = getYAbsMin();
    }


    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        painter.setPen( getGraphColor( i, _sensors.size() ) );

        if( _sensors[i].second )
        {
            if( _data.find( _sensors[i].first ) == _data.end() )
            {
                continue;
            }

            std::vector<double>& y = _data[_sensors[i].first];
            if( y.size() > 1 )
            {
                int textHeight = painter.fontMetrics().height();
                double xMin = 0;
                double xMax = y.size()-1;

                if( _normalized )
                {
                    double yMin = *std::min_element( y.begin(), y.end() );
                    double yMax = *std::max_element( y.begin(), y.end() );

                    for( unsigned int k = 0; k < y.size()-1; k++ )
                    {
                        painter.drawLine(
                            getXPosition( xMax-k, xMin, xMax, textHeight ),
                            getYPosition( (y[y.size()-1-k]-yMin)/std::abs(yMax-yMin), 0, 1, textHeight ),
                            getXPosition( xMax-k - 1, xMin, xMax, textHeight ),
                            getYPosition( (y[y.size()-2-k]-yMin)/std::abs(yMax-yMin), 0, 1, textHeight ) );
                    }
                }
                else
                {
                    for( unsigned int k = 0; k < y.size()-1; k++ )
                    {
                        painter.drawLine(
                            getXPosition( xMax-k, xMin, xMax, textHeight ),
                            getYPosition( y[y.size()-1-k], yAbsMin, yAbsMax, textHeight ),
                            getXPosition( xMax-k - 1, xMin, xMax, textHeight ),
                            getYPosition( y[y.size()-2-k], yAbsMin, yAbsMax, textHeight ) );
                    }
                }
            }
        }
    }
}

double multipleGraphsWidget::getYAbsMax()
{
    double absMaxValue = std::numeric_limits<double>::min();

    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        if( _sensors[i].second )
        {
            std::vector<double>& y = _data[_sensors[i].first];
            double yMax = *std::max_element( y.begin(), y.end() );

            if( yMax > absMaxValue )
            {
                absMaxValue = yMax;
            }
        }
    }

    if( absMaxValue > std::numeric_limits<double>::min() )
    {
        return absMaxValue;
    }

    return 1.0;
}

double multipleGraphsWidget::getYAbsMin()
{
    double absMinValue = std::numeric_limits<double>::max();

    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        if( _sensors[i].second )
        {
            std::vector<double>& y = _data[_sensors[i].first];
            double yMin = *std::min_element( y.begin(), y.end() );

            if( yMin < absMinValue )
            {
                absMinValue = yMin;
            }
        }
    }

    if( absMinValue < std::numeric_limits<double>::max() )
    {
        return absMinValue;
    }

    return 0.0;
}

int multipleGraphsWidget::getXPosition( double value, double min, double max,
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

int multipleGraphsWidget::getYPosition( double value, double min, double max,
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

void multipleGraphsWidget::drawAxes( QPainter *painter )
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
                        painter->fontMetrics().width( "counter" ) )/2,
                       this->height() - DIST_BOTTOM_BORDER_TEXT,
                       "counter" );
    /*
    painter->drawText( distBorderLineLeft,
                       this->height() - DIST_BOTTOM_BORDER_TEXT,
                       value );
    painter->drawText( this->width() - DIST_RIGHT_BORDER_LINE -
                       painter->fontMetrics().width( value ),
                       this->height() - DIST_BOTTOM_BORDER_TEXT,
                       value );
    */

    value = "values";
    if( _normalized )
    {
        value = "normalized values";
    }
    painter->rotate( 270 );
    painter->drawText( -this->height() + distBorderLineBottom +
                       ((this->height() - DIST_TOP_BORDER_LINE -
                          distBorderLineBottom) -
                         painter->fontMetrics().width( value ))/2,
                       DIST_LEFT_BORDER_TEXT + textHeight, value );
    value = _normalized ? "0" : getMinYValueString();
    painter->drawText( -(this->height() - distBorderLineBottom),
                       DIST_LEFT_BORDER_TEXT + textHeight, value );
    value = _normalized ? "1" : getMaxYValueString();
    painter->drawText( -DIST_TOP_BORDER_LINE -
                       painter->fontMetrics().width( value ),
                       DIST_LEFT_BORDER_TEXT + textHeight, value );
    painter->rotate( -270 );
}

QString multipleGraphsWidget::getMinYValueString()
{
    double minValue = std::numeric_limits<double>::max();

    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        if( _data[_sensors[i].first].size() > 1 )
        {
            double newMin = *std::min_element( _data[_sensors[i].first].begin(),
                                                _data[_sensors[i].first].end() );
            if( newMin < minValue )
            {
                minValue = newMin;
            }
        }
    }

    if( minValue < std::numeric_limits<double>::max() )
    {
        return QString::number( minValue, 'g', PRECISION_MAX_VALUES );
    }

    return "";
}

QString multipleGraphsWidget::getMaxYValueString()
{
    double maxValue = std::numeric_limits<double>::min();

    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        if( _data[_sensors[i].first].size() > 1 )
        {
            double newMax = *std::max_element( _data[_sensors[i].first].begin(),
                                                _data[_sensors[i].first].end() );
            if( newMax > maxValue )
            {
                maxValue = newMax;
            }
        }
    }

    if( maxValue > std::numeric_limits<double>::min() )
    {
        return QString::number( maxValue, 'g', PRECISION_MAX_VALUES );
    }

    return "";
}

void multipleGraphsWidget::clearData()
{
    _data.clear();
}


void multipleGraphsWidget::newValue( const QString& id, double value )
{
    bool known = false;
    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        if( _sensors[i].first == id )
        {
            known = true;
            break;
        }
    }

    if( known )
    {
        _data[id].push_back( value );
    }
}

void multipleGraphsWidget::newSensor( const QString& name, bool draw )
{
    bool known = false;
    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        if( _sensors[i].first == name )
        {
            known = true;
            break;
        }
    }

    if( !known )
    {
        _sensors.push_back( std::pair<QString, bool>( name, draw ) );
    }
}

void multipleGraphsWidget::setSensorDraw( const QString& name, bool draw )
{
    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        if( _sensors[i].first == name )
        {
            _sensors[i].second = draw;
            break;
        }
    }
}

void multipleGraphsWidget::deleteSensor( const QString& name )
{
    _data.erase( name );

    for( unsigned int i = 0; i < _sensors.size(); i++ )
    {
        if( _sensors[i].first == name )
        {
            _sensors.erase( _sensors.begin() + i );
            break;
        }
    }
}

void multipleGraphsWidget::setNormalized( bool normalized )
{
    _normalized = normalized;
    update();
}
