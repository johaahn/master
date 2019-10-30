#ifndef MAIN_WINDOW_HH_
#define MAIN_WINDOW_HH_

#include <QtCore/QCoreApplication>
#include <QtWidgets>
#if 1
namespace Ui {
	class MainWindow;
}
#endif
//#include "ui_main_window.h"
class CT_QT_MAIN_WINDOW : public QMainWindow {

	Ui::MainWindow * _pc_ui;

public:
	CT_QT_MAIN_WINDOW(QWidget *parent);
	~CT_QT_MAIN_WINDOW();
};

#endif /* MAIN_WINDOW_HH_ */
