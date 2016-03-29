#include "windowadderwindow.h"
#include "ui_windowadderwindow.h"

windowAdderWindow::windowAdderWindow( mdiHandler* mdi, QWidget *parent ) :
    QDialog( parent ),
    _ui( new Ui::windowAdderWindow ),
    _mdi( mdi )
{
    _ui->setupUi( this );

    connect( _ui->btn_ok, SIGNAL( clicked() ), this, SLOT( ok() ) );
    connect( _ui->btn_cancel, SIGNAL( clicked() ), this, SLOT( cancel() ) );
}

windowAdderWindow::~windowAdderWindow()
{
    delete _ui;
}

void windowAdderWindow::ok()
{
    if( !checkWindowNames() )
    {
        return;
    }

    if( _ui->chb_powerMeterTest->isChecked() )
    {
        _mdi->addPowerMeterTestWindow( _ui->txt_test->text() );
    }
    if( _ui->chb_pairSave->isChecked() )
    {
        _mdi->addPairSaveWindow( _ui->txt_pairSave->text() );
    }
    if( _ui->chb_allSave->isChecked() )
    {
        _mdi->addAllSaveWindow( _ui->txt_allSave->text() );
    }
    if( _ui->chb_eaps->isChecked() )
    {
        _mdi->addEapsWindow( _ui->txt_eaps->text() );
    }
    if( _ui->chb_bopMg->isChecked() )
    {
        _mdi->addBogMgWindow( _ui->txt_bogMg->text() );
    }
    if( _ui->chb_bopmgUiChar->isChecked() )
    {
        _mdi->addBopmgUICharWindow( _ui->txt_bopmgUIChar->text() );
    }
    if( _ui->chb_simpleGraph->isChecked() )
    {
        _mdi->addSimpleGraph( _ui->txt_simpleGraph->text() );
    }
    if( _ui->chb_calcTemperature->isChecked() )
    {
        _mdi->addCalcTemperatureWindow( _ui->txt_calcTemperature->text() );
    }
    if( _ui->chb_tsh071->isChecked() )
    {
        _mdi->addTsh071Window( _ui->txt_tsh071->text() );
    }
    if( _ui->chb_flowMeter->isChecked() )
    {
        _mdi->addFlowMeterWindow( _ui->txt_flowMeter->text() );
    }

    accept();
}

void windowAdderWindow::cancel()
{
    reject();
}

bool windowAdderWindow::checkWindowNames()
{
    QStringList names;

    if( _ui->chb_powerMeterTest->isChecked() )
    {
        names.push_back( _ui->txt_test->text() );
    }
    if( _ui->chb_pairSave->isChecked() )
    {
        names.push_back( _ui->txt_pairSave->text() );
    }
    if( _ui->chb_allSave->isChecked() )
    {
        names.push_back( _ui->txt_allSave->text() );
    }
    if( _ui->chb_eaps->isChecked() )
    {
        names.push_back( _ui->txt_eaps->text() );
    }
    if( _ui->chb_bopMg->isChecked() )
    {
        names.push_back( _ui->txt_bogMg->text() );
    }
    if( _ui->chb_bopmgUiChar->isChecked() )
    {
        names.push_back( _ui->txt_bopmgUIChar->text() );
    }
    if( _ui->chb_simpleGraph->isChecked() )
    {
        names.push_back( _ui->txt_simpleGraph->text() );
    }
    if( _ui->chb_calcTemperature->isChecked() )
    {
        names.push_back( _ui->txt_calcTemperature->text() );
    }
    if( _ui->chb_tsh071->isChecked() )
    {
        names.push_back( _ui->txt_tsh071->text() );
    }
    if( _ui->chb_flowMeter->isChecked() )
    {
        names.push_back( _ui->txt_flowMeter->text() );
    }

    return namesValid( names );
}

bool windowAdderWindow::namesValid( QStringList names )
{
    if( names.isEmpty() )
    {
        return true;
    }

    for( QString name : names )
    {
        if( name.isEmpty() )
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle( "error" );
            msgBox.setText( "Window names must not be empty!" );
            msgBox.exec();

            return false;
        }
    }

    for( int i = 0; i < names.size(); i++ )
    {
        for( int k = 0; k < names.size(); k++ )
        {
            if( i != k && names.at( i ) == names.at( k ) )
            {
                QMessageBox msgBox;
                msgBox.setWindowTitle( "error" );
                msgBox.setText( "Window names have to be unique!\n"
                                "('" + names.at( i ) + "' is not!)" );
                msgBox.exec();

                return false;
            }
        }
    }

    QStringList existingNames = _mdi->getWindowNames();
    if( !existingNames.isEmpty() )
    {
        for( QString name : names )
        {
            for( QString existingName : existingNames )
            {
                if( name == existingName )
                {
                    QMessageBox msgBox;
                    msgBox.setWindowTitle( "error" );
                    msgBox.setText( "Window name '" + name + "' is already "
                                                             "used!"  );
                    msgBox.exec();

                    return false;
                }
            }
        }
    }

    return true;
}
