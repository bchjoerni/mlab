#include "multiplegraphswindow.h"
#include "ui_multiplegraphswindow.h"


multipleGraphsWindow::multipleGraphsWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::multipleGraphsWindow )
{
    _ui->setupUi( this );

    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStop() ) );
    connect( _ui->btn_clearData, SIGNAL( clicked() ), this,
             SLOT( clearData() ) );
    connect( _ui->btn_addSensor, SIGNAL( clicked() ), this,
             SLOT( addSensor() ) );
    connect( _ui->btn_removeSensors, SIGNAL( clicked() ), this,
             SLOT( deleteSensors() ) );
    connect( _ui->chb_normalize, SIGNAL( clicked() ), this,
             SLOT( normalizedChanged() ) );
    connect( _ui->cob_yAxis, SIGNAL( currentTextChanged( QString ) ), this,
             SLOT( yAxisChanged() ) );
    connect( _ui->list_sensors, SIGNAL( itemSelectionChanged() ), this,
             SLOT( drawSettingsChanged() ) );

    _ui->cob_yAxis->addItem( AXIS_LINEAR );
    _ui->cob_yAxis->addItem( AXIS_LOG );
}

multipleGraphsWindow::~multipleGraphsWindow()
{
    delete _ui;
}


void multipleGraphsWindow::mLabSignal( const QString& cmd )
{
    QString cmdLower = cmd.toLower().trimmed();

    if( cmdLower == STOP_SIGNAL
            && _ui->chb_setZeroAtStopSignal->isChecked() )
    {
        if( _ui->btn_startStop->text() == STOP )
        {
            _ui->btn_startStop->setText( START );
            _ui->lbl_status->setText( STOP_INFO_TEXT  );
            _ui->lbl_status->setStyleSheet( STYLE_ERROR );

            emit changeWindowState( this->windowTitle(), false );
        }
    }
}

void multipleGraphsWindow::doUpdate()
{
    if( _ui->btn_startStop->text() == STOP )
    {
        _ui->wgt_graph->update();
    }
}

void multipleGraphsWindow::putValue( const QString& id, double value )
{
    _ui->wgt_graph->newValue( id, value );

    bool inList = false;
    for( int i = 0; i < _ui->cob_sensors->count(); i++ )
    {
        if( _ui->cob_sensors->itemText( i ) == id )
        {
            inList = true;
            break;
        }
    }
    if( !inList )
    {
        _ui->cob_sensors->addItem( id );
    }
}

void multipleGraphsWindow::startStop()
{
    if( _ui->btn_startStop->text() == START )
    {
        _ui->btn_startStop->setText( STOP );
        _ui->lbl_status->setText( RUNNING );
        _ui->lbl_status->setStyleSheet( STYLE_OK );

        emit changeWindowState( this->windowTitle(), true );
    }
    else if( _ui->btn_startStop->text() == STOP )
    {
        _ui->btn_startStop->setText( START );
        _ui->lbl_status->setText( PAUSING );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );

        emit changeWindowState( this->windowTitle(), false );
    }
}

void multipleGraphsWindow::clearData()
{
    _ui->wgt_graph->clearData();
}

void multipleGraphsWindow::addSensor()
{
    _ui->wgt_graph->newSensor( _ui->cob_sensors->currentText(), true );

    bool inList = false;
    for( int i = 0; i < _ui->list_sensors->count(); i++ )
    {
        if( _ui->list_sensors->item( i )->text() == _ui->cob_sensors->currentText() )
        {
            inList = true;
            break;
        }
    }
    if( !inList )
    {
        _ui->list_sensors->addItem( _ui->cob_sensors->currentText() );
        updateSensorColors();
    }
}

void multipleGraphsWindow::deleteSensors()
{
    for( int i = 0; i < _ui->list_sensors->count(); i++ )
    {
        if( _ui->list_sensors->item( i )->isSelected() )
        {
            _ui->wgt_graph->deleteSensor( _ui->list_sensors->item( i )->text() );
            delete _ui->list_sensors->item( i );
            i--;
        }
    }

    updateSensorColors();
}

void multipleGraphsWindow::drawSettingsChanged()
{
    for( int i = 0; i < _ui->list_sensors->count(); i++ )
    {
        _ui->wgt_graph->setSensorDraw( _ui->list_sensors->item( i )->text(),
                                       !_ui->list_sensors->item( i )->isSelected() );
    }
}

void multipleGraphsWindow::normalizedChanged()
{
    _ui->wgt_graph->setNormalized( _ui->chb_normalize->isChecked() );
}

void multipleGraphsWindow::yAxisChanged()
{
    if( _ui->cob_yAxis->currentText() == AXIS_LOG )
    {
        _ui->wgt_graph->setYAxis( multipleGraphsWidget::axisType::log );
    }
    else
    {
        _ui->wgt_graph->setYAxis( multipleGraphsWidget::axisType::linear );
    }
}

void multipleGraphsWindow::updateSensorColors()
{
    for( int i = 0; i < _ui->list_sensors->count(); i++ )
    {
        _ui->list_sensors->item( i )->setForeground(
                    _ui->wgt_graph->getGraphColor(
                        i, _ui->list_sensors->count() ) );
    }
}
