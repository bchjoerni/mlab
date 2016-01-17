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
    if( _ui->chb_powerMeterTest->isChecked() )
    {
        _mdi->addPowerMeterTestWindow( _ui->txt_test->text() );
    }
    if( _ui->chb_pairSave->isChecked() )
    {
        _mdi->addPairSaveWindow( _ui->txt_pairSave->text() );
    }
    if( _ui->chb_eaps->isChecked() )
    {
        _mdi->addEapsWindow( _ui->txt_eaps->text() );
    }
    if( _ui->chb_bopMg->isChecked() )
    {
        _mdi->addBogMgWindow( _ui->txt_eaps->text() );
    }
    if( _ui->chb_bopmgUiChar->isChecked() )
    {
        _mdi->addBopmgUICharWindow( _ui->txt_bopmgUIChar->text() );
    }
    if( _ui->chb_simpleGraph->isChecked() )
    {
        _mdi->addSimpleGraph( _ui->txt_simpleGraph->text() );
    }

    accept();
}

void windowAdderWindow::cancel()
{
    reject();
}
