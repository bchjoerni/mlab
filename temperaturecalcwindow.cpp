#include "temperaturecalcwindow.h"
#include "ui_temperaturecalcwindow.h"

temperatureCalcWindow::temperatureCalcWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::temperatureCalcWindow ),
    _running( false ), _lastResistance( 0.0 ),
    _linearInitTemperature( 300.0 ), _linearInitResistance( 0.0 )
{
    _ui->setupUi( this );

    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStopPressed() ) );
    connect( _ui->btn_measureInitValue, SIGNAL( clicked() ), this,
             SLOT( measureLinearInitValue() ) );
    connect( _ui->btn_applyInitValues, SIGNAL( clicked() ), this,
             SLOT( applyInitValues() ) );

    connect( _ui->chb_linear, SIGNAL( stateChanged( int ) ), this,
             SLOT( showShareChanged() ) );
    connect( _ui->chb_quadratic, SIGNAL( stateChanged( int ) ), this,
             SLOT( showShareChanged() ) );

    showShareChanged();
}

temperatureCalcWindow::~temperatureCalcWindow()
{
    delete _ui;
}

void temperatureCalcWindow::doUpdate()
{
    if( _running )
    {
        if( _ui->chb_linear->isChecked() )
        {
            double temperature = (_lastResistance/_linearInitResistance - 1.0)/
                    (_ui->dsb_alpha->value()*1E-3) + _linearInitTemperature;

            _ui->txt_linearCalcTemperatureCelsius->setText(
                        QString::number( temperature - 273.15) );
            _ui->txt_linearCalcTemperatureKelvin->setText(
                        QString::number( temperature ) );
            emit newValue( this->windowTitle() + ": temperature (linear)",
                           temperature );
        }
        if( _ui->chb_quadratic->isChecked() )
        {
            double temperature = -a1/(2.0*a2) + std::sqrt(
                        std::pow( a1/(2.0*a2), 2.0 ) - (a0 - _lastResistance*
                        (std::pow( _ui->dsb_diameter->value()*(1E-3)/2.0, 2.0 )*
                         M_PI/(_ui->dsb_length->value()*1E-3)))/a2 );

            _ui->txt_quadraticCalcTemperatureCelsius->setText(
                        QString::number( temperature - 273.15) );
            _ui->txt_quatraticCalcTemperatureKelvin->setText(
                        QString::number( temperature ) );
            emit newValue( this->windowTitle() + ": temperature (quadratic)",
                           temperature );
        }
    }
}

void temperatureCalcWindow::putValue( const QString& id, double value )
{
    if( id == _ui->cob_resistance->currentText() )
    {
        _lastResistance = value;
    }

    bool inList = false;
    for( int i = 0; i < _ui->cob_resistance->count(); i++ )
    {
        if( _ui->cob_resistance->itemText( i ) == id )
        {
            inList = true;
            break;
        }
    }
    if( !inList && id.contains( "resistance" ) )
    {
        _ui->cob_resistance->addItem( id );
    }
}

void temperatureCalcWindow::startStopPressed()
{
    if( _running )
    {
        _ui->btn_startStop->setText( START );
        _ui->lbl_status->setText( PAUSING );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );

        emit changeWindowState( this->windowTitle(), false );
    }
    else
    {
        if( _ui->cob_resistance->currentText().isEmpty() )
        {
            return;
        }

        _ui->btn_startStop->setText( STOP );
        _ui->lbl_status->setText( RUNNING );
        _ui->lbl_status->setStyleSheet( STYLE_OK );

        emit changeWindowState( this->windowTitle(), true );
    }

    _running = !_running;
}

void temperatureCalcWindow::showShareChanged()
{
    _ui->grp_linear->setVisible( _ui->chb_linear->isChecked() );
    _ui->grp_quadratic->setVisible( _ui->chb_quadratic->isChecked() );
}

void temperatureCalcWindow::measureLinearInitValue()
{
    _ui->dsp_initResistance->setValue( _lastResistance );
}

void temperatureCalcWindow::applyInitValues()
{
    double resistance = _ui->dsp_initResistance->value();
    if( resistance < 1E-10 )
    {
        _ui->lbl_status->setText( "Invalid resistance value!" );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        return;
    }

    _linearInitResistance = resistance;
    _linearInitTemperature = _ui->dsb_initTemperature->value();

    _ui->lbl_linearInitValues->setText( "init values: T = " +
            QString::number( _linearInitTemperature ) + "K, R = " +
            QString::number( _linearInitResistance ) + "Ω" );
}
