#include "powermetertestwindow.h"
#include "ui_powermetertestwindow.h"

powerMeterTestWindow::powerMeterTestWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::powerMeterTestWindow ), _setValue( 0.01 ),
    _randomGenerator( new std::mt19937( std::random_device()() ) )
{
    _ui->setupUi( this );

    connect( _ui->cob_measuredValues, SIGNAL( currentTextChanged( QString ) ),
             this, SLOT( visibilitySelectionChanged() ) );
    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_setValue, SIGNAL( clicked() ), this, SLOT( setValue() ) );
    connect( _ui->btn_measuredValuesVisibility, SIGNAL( clicked() ), this,
             SLOT( changeVisibility() ) );

    addItems();
    refreshPortList();
    visibilitySelectionChanged();
}

powerMeterTestWindow::~powerMeterTestWindow()
{
    delete _ui;
}

void powerMeterTestWindow::addItems()
{
    _ui->cob_setValue->addItem( VOLTAGE );
    _ui->cob_setValue->addItem( CURRENT );
    _ui->cob_setValue->addItem( POWER   );

    _ui->cob_measuredValues->addItem( VOLTAGE );
    _ui->cob_measuredValues->addItem( CURRENT );
    _ui->cob_measuredValues->addItem( POWER   );

}

void powerMeterTestWindow::refreshPortList()
{
    _ui->cob_ports->clear();
    _ui->cob_ports->addItem( "testport" );
}

void powerMeterTestWindow::connectivityButtonPressed()
{
    if( _ui->btn_connect->text() == CONNECT_PORT )
    {
        connectPort();
    }
    else if( _ui->btn_connect->text() == DISCONNECT_PORT )
    {
        disconnectPort();
    }
}

void powerMeterTestWindow::connectPort()
{
    _ui->btn_setValue->setEnabled( true );
    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );
    _ui->lbl_info->setText( "testport" );
    setInit( true );
}

void powerMeterTestWindow::disconnectPort()
{
    _ui->btn_setValue->setEnabled( false );
    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );
    _ui->lbl_info->setText( "-" );
    setInit( false );
}

void powerMeterTestWindow::setValue()
{
    _setValueStr = _ui->cob_setValue->currentText();
    _setValue = _ui->dsp_setValue->value();
}

void powerMeterTestWindow::doUpdate()
{
    if( !getInit() )
    {
        return;
    }

    std::uniform_real_distribution<double> distribution( _setValue*0.95,
                                                         _setValue*1.05 );
    double power = distribution( (*_randomGenerator) );
    double voltage = power*2.0;
    double current = power/2.0;

    if( _setValueStr == VOLTAGE )
    {
        voltage = distribution( (*_randomGenerator) );
        current = power/voltage;
        _ui->txt_voltage->setText( QString::number( voltage ) + " V" );
        _ui->txt_current->setText( QString::number( current ) + " A" );
        _ui->txt_power->setText( QString::number( power ) );
    }
    else if( _setValueStr == CURRENT )
    {
        current = distribution( (*_randomGenerator) );
        voltage = power/current;
        _ui->txt_voltage->setText( QString::number( voltage ) + " V" );
        _ui->txt_current->setText( QString::number( current ) + " A" );
        _ui->txt_power->setText( QString::number( power ) );
    }
    else if( _setValueStr == POWER )
    {
        _ui->txt_voltage->setText( QString::number( voltage ) + " V" );
        _ui->txt_current->setText( QString::number( current ) + " A" );
        _ui->txt_power->setText( QString::number( power ) );
    }
    else
    {
        power = 0.0;
        voltage = 0.0;
        current = 0.0;

        _ui->txt_voltage->setText( QString::number( voltage ) + " V" );
        _ui->txt_current->setText( QString::number( current ) + " A" );
        _ui->txt_power->setText( QString::number( power ) );
    }

    if( _ui->chb_shareVoltage->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + VOLTAGE, voltage );
    }
    if( _ui->chb_shareCurrent->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + CURRENT, current );
    }
    if( _ui->chb_sharePower->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + POWER, power );
    }
}

void powerMeterTestWindow::visibilitySelectionChanged()
{
    QString text = _ui->cob_measuredValues->currentText();

    if( text == VOLTAGE )
    {
        if( _ui->frame_voltage->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
    else if( text == CURRENT )
    {
        if( _ui->frame_current->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
    else if( text == POWER )
    {
        if( _ui->frame_power->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
}

void powerMeterTestWindow::changeVisibility()
{
    QString text = _ui->cob_measuredValues->currentText();

    if( text == VOLTAGE )
    {
        _ui->frame_voltage->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_shareVoltage->setChecked( false );
    }
    else if( text == CURRENT )
    {
        _ui->frame_current->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_shareCurrent->setChecked( false );
    }
    else if( text == POWER )
    {
        _ui->frame_power->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_sharePower->setChecked( false );
    }
    visibilitySelectionChanged();
}
