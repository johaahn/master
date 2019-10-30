/***********************************************************************
 ** qt.hh
 ***********************************************************************
 ** Copyright (c) SEAGNAL SAS
 **
 ** This library is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2.1 of the License, or (at your option) any later version.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 ** MA 02110-1301 USA
 **
 ***********************************************************************/

/* define against mutual inclusion */
#ifndef QT_HH_
#define QT_HH_

/**
 * @file qt.hh
 * QT HOST definition.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2012
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <host.hh>
#include <task.hh>
#include <node.hh>
#include <main_window.hh>
#include <qt_nogui.hh>
#include <QApplication>
#include <QScrollArea>
#include <QtWidgets>
#include <QTranslator>
#include <QSplashScreen>

#include <qtpropertymanager.h>
#include <qtvariantproperty.h>
#include <qttreepropertybrowser.h>


/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/
namespace Ui {
class form_task;
}


class CT_QT_HOST: public CT_QT_NOGUI_HOST {
	/* Translator */
	QTranslator * _pc_translator;

	/* Main window */
	CT_QT_MAIN_WINDOW * _pc_main_window;

	/* Splash screen */
	QSplashScreen * _pc_splash;
protected:
	int f_plugin_load(QFileInfo & in_c_module);

public:
	CT_QT_HOST(struct ST_HOST_ARGS * in_ps_args);
	~CT_QT_HOST();

	/* Virtual redefined function */
	int f_main_init(void);
	int f_main_loop(void);

	int f_state_loaded(void);

	master::core::CT_HOST_TASK_MANAGER * f_new_task_manager(void);
	CT_HOST_PROPERTY * f_new_property();

	QSplashScreen * f_get_splash_screen() { return _pc_splash; }
};
#if 0
class CT_QT_HOST_PROPERTY_GROUP: public CT_HOST_PROPERTY_GROUP {

	QtProperty * _pc_group;

public:
	CT_QT_HOST_PROPERTY_GROUP();
	~CT_QT_HOST_PROPERTY_GROUP();

};
#endif

class CT_QT_HOST_PROPERTY;

class CT_QT_HOST_PROPERTY_ELEMENT: public QObject, public CT_HOST_PROPERTY_ELEMENT {
	Q_OBJECT
	/* Owner info */
	//CT_QT_HOST_PROPERTY_ELEMENT *_pc_parent;
	//CT_QT_HOST_PROPERTY * _pc_property;
	/* Variant info */
	QtVariantProperty * _pc_item;

	/* Variant info */
	//QtVariantProperty * _pc_item;
public:
	CT_QT_HOST_PROPERTY_ELEMENT(CT_HOST_PROPERTY * in_pc_new,
			CT_HOST_PROPERTY_ELEMENT * in_pc_parent);
	~CT_QT_HOST_PROPERTY_ELEMENT();

	int f_configure(CT_NODE & in_c_node, int in_i_count);
	int f_data_update(CT_NODE & in_c_node);
	int f_data_update(CT_PORT_NODE & in_c_node);
	int f_data_update(void);
	int f_data_refresh(void);
	void f_remove(void);

	QtVariantProperty * f_get_property() {
		return _pc_item;
	}
};

class CT_QT_HOST_PROPERTY: public QWidget, public CT_HOST_PROPERTY {
Q_OBJECT
	QVBoxLayout* _pc_layout;
	QtVariantEditorFactory *_pc_variantfactory;
	QtTreePropertyBrowser *_pc_varianteditor;
	QtVariantPropertyManager *_pc_variantmanager;

private:
  bool _b_updated;

public slots:
	void on_property_updated(QtProperty*, QVariant);

signals:
	void updated(void);

public:
	CT_QT_HOST_PROPERTY();
	~CT_QT_HOST_PROPERTY();

  int f_set_from(CT_PORT_NODE & in_c_node);
  int f_set_from(CT_NODE & in_c_node);
	int f_set_dict(CT_NODE & in_c_node);
	CT_GUARD<CT_HOST_PROPERTY_ELEMENT> on_new_element(CT_HOST_PROPERTY * in_pc_new,
			CT_HOST_PROPERTY_ELEMENT * in_pc_parent);

	/*CT_HOST_PROPERTY_GROUP* on_new_group(
	 CT_HOST_PROPERTY * in_pc_property);*/

	QtVariantPropertyManager * f_get_property_manager() {
		return _pc_variantmanager;
	}
	QtTreePropertyBrowser * f_get_property_editor() {
		return _pc_varianteditor;
	}
};


class CT_QT_HOST_TASK: public QObject, public master::core::CT_TASK {

Q_OBJECT
	QWidget * _pc_widget;

public slots:
	void on_button_clicked(void);
	void on_signal_update(void);
signals:
void signal_update(void);

private:
	Ui::form_task *_pc_ui;

public:
	CT_QT_HOST_TASK(master::core::CT_TASK::task_key_t const & in_c_uuid);
	~CT_QT_HOST_TASK();

#if 0
	void f_set_value(int);
	void f_set_name(std::string const & in_str_title);
	void f_set_status(std::string const & in_str_title);
#endif
	QWidget * f_get_widget() {
		return _pc_widget;
	}
	void on_destroy(void);
	void on_init(void);
	void on_update(void);
	void on_delete(void);
};


class CT_QT_HOST_TASK_MANAGER: public QWidget,
		public master::core::CT_HOST_TASK_MANAGER {
Q_OBJECT

	QVBoxLayout * _pc_layout;
	QVBoxLayout * _pc_layout_top;

	QScrollArea * _pc_scroll;
	QWidget * _pc_scroll_contents;
	typedef std::map<master::core::CT_TASK::task_key_t const,
			CT_GUARD<master::core::CT_TASK> >::iterator _m_tasks_ui_iterator_t;
	std::map<master::core::CT_TASK::task_key_t const, CT_GUARD<master::core::CT_TASK> > _m_tasks_ui;

	/* delayed operation */
	CT_SPINLOCK _c_manager_lock;
	std::list<CT_GUARD<master::core::CT_TASK> > _l_new_task;
	std::list<CT_GUARD<master::core::CT_TASK> > _l_release_task;

	QTimer * _pc_timer;

private:
	void on_new(void);
	void on_release(void);

private slots:
	void on_timeout(void);

signals:
	void signal_task_release(void);
	void signal_task_new(void);

public:
	CT_QT_HOST_TASK_MANAGER();
	~CT_QT_HOST_TASK_MANAGER();

	CT_GUARD<master::core::CT_TASK> f_new_task(master::core::CT_TASK::task_key_t const & in_c_uuid);
	void on_new(CT_GUARD<master::core::CT_TASK> & in_pc_task);
	void on_release(CT_GUARD<master::core::CT_TASK> & in_pc_task);

	QVBoxLayout * f_get_layout();
};

CT_HOST * f_host_qt(struct ST_HOST_ARGS * in_ps_args);

#endif /* QT_HH_ */
