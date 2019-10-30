
#include <main_window.hh>
#include "ui_main_window.h"

CT_QT_MAIN_WINDOW::CT_QT_MAIN_WINDOW(QWidget *parent)
    : QMainWindow(parent), _pc_ui(new Ui::MainWindow)
{
	_pc_ui->setupUi(this);

}


CT_QT_MAIN_WINDOW::~CT_QT_MAIN_WINDOW()
{
    delete _pc_ui;
}
