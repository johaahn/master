/***********************************************************************
 ** file_property.cc
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

/**
 * @file qt.cc
 * QT Host definition.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/


#include "qt_property.hh"
#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>
#include <QFocusEvent>

CT_QT_FILE_PROPERTY::CT_QT_FILE_PROPERTY(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    _pc_edit = new QLineEdit(this);
    _pc_edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    QToolButton *button = new QToolButton(this);
    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    button->setText(QLatin1String("..."));
    layout->addWidget(_pc_edit);
    layout->addWidget(button);
    setFocusProxy(_pc_edit);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled);
  //  connect(_pc_edit, SIGNAL(textEdited(const QString &)),
    //            this, SIGNAL(filePathChanged(const QString &)));
    connect(_pc_edit, SIGNAL(returnPressed()),
                   this, SLOT(returnPressed()));
    connect(button, SIGNAL(clicked()),
                this, SLOT(buttonClicked()));
}

void CT_QT_FILE_PROPERTY::buttonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Choose a file"), _pc_edit->text(), _str_filter);
    if (filePath.isNull())
        return;
    _pc_edit->setText(filePath);
    emit filePathChanged(filePath);
}

void CT_QT_FILE_PROPERTY::returnPressed()
{
    QString filePath =  _pc_edit->text();
    emit filePathChanged(filePath);
}

void CT_QT_FILE_PROPERTY::focusInEvent(QFocusEvent *e)
{
	_pc_edit->event(e);
    if (e->reason() == Qt::TabFocusReason || e->reason() == Qt::BacktabFocusReason) {
    	_pc_edit->selectAll();
    }
    QWidget::focusInEvent(e);
}

void CT_QT_FILE_PROPERTY::focusOutEvent(QFocusEvent *e)
{
	_pc_edit->event(e);
    QWidget::focusOutEvent(e);
}

void CT_QT_FILE_PROPERTY::keyPressEvent(QKeyEvent *e)
{
	_pc_edit->event(e);
}

void CT_QT_FILE_PROPERTY::keyReleaseEvent(QKeyEvent *e)
{
	_pc_edit->event(e);
}


CT_QT_VARIANT_MANAGER::CT_QT_VARIANT_MANAGER(QObject *parent)
    : QtVariantPropertyManager(parent)
        { }


class FilePathPropertyType
{
};

Q_DECLARE_METATYPE(FilePathPropertyType)

int CT_QT_VARIANT_MANAGER::filePathTypeId()
{
    return qMetaTypeId<FilePathPropertyType>();
}

bool CT_QT_VARIANT_MANAGER::isPropertyTypeSupported(int propertyType) const
{
    if (propertyType == filePathTypeId())
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

int CT_QT_VARIANT_MANAGER::valueType(int propertyType) const
{
    if (propertyType == filePathTypeId())
        return QVariant::String;
    return QtVariantPropertyManager::valueType(propertyType);
}

QVariant CT_QT_VARIANT_MANAGER::value(const QtProperty *property) const
{
    if (_m_values.contains(property))
        return _m_values[property].str_value;
    return QtVariantPropertyManager::value(property);
}

QStringList CT_QT_VARIANT_MANAGER::attributes(int propertyType) const
{
    if (propertyType == filePathTypeId()) {
        QStringList attr;
        attr << QLatin1String("filter");
        return attr;
    }
    return QtVariantPropertyManager::attributes(propertyType);
}

int CT_QT_VARIANT_MANAGER::attributeType(int propertyType, const QString &attribute) const
{
    if (propertyType == filePathTypeId()) {
        if (attribute == QLatin1String("filter"))
            return QVariant::String;
        return 0;
    }
    return QtVariantPropertyManager::attributeType(propertyType, attribute);
}

QVariant CT_QT_VARIANT_MANAGER::attributeValue(const QtProperty *property, const QString &attribute)
{
    if (_m_values.contains(property)) {
        if (attribute == QLatin1String("filter"))
            return _m_values[property].str_filter;
        return QVariant();
    }
    return QtVariantPropertyManager::attributeValue(property, attribute);
}

QString CT_QT_VARIANT_MANAGER::valueText(const QtProperty *property) const
{
    if (_m_values.contains(property))
        return _m_values[property].str_value;
    return QtVariantPropertyManager::valueText(property);
}

void CT_QT_VARIANT_MANAGER::setValue(QtProperty *property, const QVariant &val)
{
    if (_m_values.contains(property)) {
        if (val.type() != QVariant::String && !val.canConvert(QVariant::String))
            return;
        QString str = val.value<QString>();
        ST_DATA d = _m_values[property];
        if (d.str_value == str)
            return;
        d.str_value = str;
        _m_values[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, str);
        return;
    }
    QtVariantPropertyManager::setValue(property, val);
}

void CT_QT_VARIANT_MANAGER::setAttribute(QtProperty *property,
                const QString &attribute, const QVariant &val)
{
    if (_m_values.contains(property)) {
        if (attribute == QLatin1String("filter")) {
            if (val.type() != QVariant::String && !val.canConvert(QVariant::String))
                return;
            QString str = val.value<QString>();
            ST_DATA d = _m_values[property];
            if (d.str_filter == str)
                return;
            d.str_filter = str;
            _m_values[property] = d;
            emit attributeChanged(property, attribute, str);
        }
        return;
    }
    QtVariantPropertyManager::setAttribute(property, attribute, val);
}

void CT_QT_VARIANT_MANAGER::initializeProperty(QtProperty *property)
{
    if (propertyType(property) == filePathTypeId())
    	_m_values[property] = ST_DATA();
    QtVariantPropertyManager::initializeProperty(property);
}

void CT_QT_VARIANT_MANAGER::uninitializeProperty(QtProperty *property)
{
	_m_values.remove(property);
    QtVariantPropertyManager::uninitializeProperty(property);
}

CT_QT_VARIANT_FACTORY::CT_QT_VARIANT_FACTORY(QObject *parent) {

}

CT_QT_VARIANT_FACTORY::~CT_QT_VARIANT_FACTORY()
{
    QList<CT_QT_FILE_PROPERTY *> editors = _m_properties.keys();
    QListIterator<CT_QT_FILE_PROPERTY *> it(editors);
    while (it.hasNext())
        delete it.next();
}

void CT_QT_VARIANT_FACTORY::connectPropertyManager(QtVariantPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QVariant &)));
    connect(manager, SIGNAL(attributeChanged(QtProperty *, const QString &, const QVariant &)),
                this, SLOT(slotPropertyAttributeChanged(QtProperty *, const QString &, const QVariant &)));
    QtVariantEditorFactory::connectPropertyManager(manager);
}

QWidget *CT_QT_VARIANT_FACTORY::createEditor(QtVariantPropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    if (manager->propertyType(property) == CT_QT_VARIANT_MANAGER::filePathTypeId()) {
    	CT_QT_FILE_PROPERTY *editor = new CT_QT_FILE_PROPERTY(parent);
        editor->setFilePath(manager->value(property).toString());
        editor->setFilter(manager->attributeValue(property, QLatin1String("filter")).toString());
        _m_editors[property].append(editor);
        _m_properties[editor] = property;

        connect(editor, SIGNAL(filePathChanged(const QString &)),
                this, SLOT(slotSetValue(const QString &)));
        connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
        return editor;
    }
    return QtVariantEditorFactory::createEditor(manager, property, parent);
}

void CT_QT_VARIANT_FACTORY::disconnectPropertyManager(QtVariantPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QVariant &)));
    disconnect(manager, SIGNAL(attributeChanged(QtProperty *, const QString &, const QVariant &)),
                this, SLOT(slotPropertyAttributeChanged(QtProperty *, const QString &, const QVariant &)));
    QtVariantEditorFactory::disconnectPropertyManager(manager);
}

void CT_QT_VARIANT_FACTORY::slotPropertyChanged(QtProperty *property,
                const QVariant &value)
{
    if (!_m_editors.contains(property))
        return;

    QList<CT_QT_FILE_PROPERTY *> editors = _m_editors[property];
    QListIterator<CT_QT_FILE_PROPERTY *> itEditor(editors);
    while (itEditor.hasNext())
        itEditor.next()->setFilePath(value.toString());
}

void CT_QT_VARIANT_FACTORY::slotPropertyAttributeChanged(QtProperty *property,
            const QString &attribute, const QVariant &value)
{
    if (!_m_editors.contains(property))
        return;

    if (attribute != QLatin1String("filter"))
        return;

    QList<CT_QT_FILE_PROPERTY *> editors = _m_editors[property];
    QListIterator<CT_QT_FILE_PROPERTY *> itEditor(editors);
    while (itEditor.hasNext())
        itEditor.next()->setFilter(value.toString());
}

void CT_QT_VARIANT_FACTORY::slotSetValue(const QString &value)
{
    QObject *object = sender();
    QMap<CT_QT_FILE_PROPERTY *, QtProperty *>::ConstIterator itEditor =
                _m_properties.constBegin();
    while (itEditor != _m_properties.constEnd()) {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtVariantPropertyManager *manager = propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
        itEditor++;
    }
}

void CT_QT_VARIANT_FACTORY::slotEditorDestroyed(QObject *object)
{
    QMap<CT_QT_FILE_PROPERTY *, QtProperty *>::ConstIterator itEditor =
                _m_properties.constBegin();
    while (itEditor != _m_properties.constEnd()) {
        if (itEditor.key() == object) {
        	CT_QT_FILE_PROPERTY *editor = itEditor.key();
            QtProperty *property = itEditor.value();
            _m_properties.remove(editor);
            _m_editors[property].removeAll(editor);
            if (_m_editors[property].isEmpty())
                _m_editors.remove(property);
            return;
        }
        itEditor++;
    }
}
