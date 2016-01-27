#include "simplegraphwindow.h"
#include "ui_simplegraphwindow.h"

simpleGraphWindow::simpleGraphWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::simpleGraphWindow )
{
    _ui->setupUi( this );

    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStop() ) );
    connect( _ui->btn_clearGraph, SIGNAL( clicked() ), this,
             SLOT( clearGraph() ) );
    connect( _ui->cob_x, SIGNAL( currentTextChanged( QString ) ), this,
             SLOT( axisChanged() ) );
    connect( _ui->cob_y, SIGNAL( currentTextChanged( QString ) ), this,
             SLOT( axisChanged() ) );
}

simpleGraphWindow::~simpleGraphWindow()
{
    delete _ui;
}

void simpleGraphWindow::doUpdate()
{
    if( _ui->btn_startStop->text() == STOP )
    {
        _ui->wgt_graph->update();
    }
}

void simpleGraphWindow::putValue( const QString& id, double value )
{
    if( _ui->chb_saveAllValues->isChecked() )
    {
        _ui->wgt_graph->newValue( id, value );
    }
    else
    {
        if( _ui->btn_startStop->text() == STOP
                && (_ui->cob_x->currentText() == id
                    || _ui->cob_y->currentText() == id) )
        {
           _ui->wgt_graph->newValue( id, value );
        }
    }

    bool inList = false;
    for( int i = 0; i < _ui->cob_x->count(); i++ )
    {
        if( _ui->cob_x->itemText( i ) == id )
        {
            inList = true;
            break;
        }
    }
    if( !inList )
    {
        _ui->cob_x->addItem( id );
        _ui->cob_y->addItem( id );
    }
}

void simpleGraphWindow::startStop()
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

void simpleGraphWindow::axisChanged()
{
    _ui->wgt_graph->axisChanged( _ui->cob_x->currentText(),
                                 _ui->cob_y->currentText() );
}

void simpleGraphWindow::clearGraph()
{
    _ui->wgt_graph->clearGraph();
}
