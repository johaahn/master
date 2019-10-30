/***********************************************************************
 ** file_property.hh
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
#ifndef QT_PROPERTY_HH_
#define QT_PROPERTY_HH_

/**
 * @file file_property.hh
 * QT File property definition.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2012
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#include <QLineEdit>
#include <qtpropertymanager.h>
#include <qtvariantproperty.h>
#include <qttreepropertybrowser.h>
/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/


class CT_QT_FILE_PROPERTY : public QWidget
{
    Q_OBJECT
public:
    CT_QT_FILE_PROPERTY(QWidget *parent = 0);
    void setFilePath(const QString &in_str_filePath) { if (_pc_edit->text() != in_str_filePath) _pc_edit->setText(in_str_filePath); }
    QString filePath() const { return _pc_edit->text(); }
    void setFilter(const QString &in_str_filter) { _str_filter = in_str_filter; }
    QString filter() const { return _str_filter; }
signals:
    void filePathChanged(const QString &filePath);
protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
private slots:
    void buttonClicked();
    void returnPressed();
private:
    QLineEdit *_pc_edit;
    QString _str_filter;
};


class CT_QT_VARIANT_MANAGER : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    CT_QT_VARIANT_MANAGER(QObject *parent = 0);

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    virtual QStringList attributes(int propertyType) const;
    virtual int attributeType(int propertyType, const QString &attribute) const;
    virtual QVariant attributeValue(const QtProperty *property, const QString &attribute);


    static int filePathTypeId();
public slots:
    virtual void setValue(QtProperty *property, const QVariant &val);
    virtual void setAttribute(QtProperty *property,
                const QString &attribute, const QVariant &value);
protected:
    virtual QString valueText(const QtProperty *property) const;
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);
private:
    struct ST_DATA {
        QString str_value;
        QString str_filter;
    };
    QMap<const QtProperty *, ST_DATA> _m_values;
};

class CT_QT_VARIANT_FACTORY : public QtVariantEditorFactory
{
    Q_OBJECT
public:
    CT_QT_VARIANT_FACTORY(QObject *parent = 0);
    virtual ~CT_QT_VARIANT_FACTORY();
protected:
    virtual void connectPropertyManager(QtVariantPropertyManager *manager);
    virtual QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    virtual void disconnectPropertyManager(QtVariantPropertyManager *manager);
private slots:
    void slotPropertyChanged(QtProperty *property, const QVariant &value);
    void slotPropertyAttributeChanged(QtProperty *property, const QString &attribute, const QVariant &value);
    void slotSetValue(const QString &value);
    void slotEditorDestroyed(QObject *object);
private:
    QMap<QtProperty *, QList<CT_QT_FILE_PROPERTY *> > _m_editors;
    QMap<CT_QT_FILE_PROPERTY *, QtProperty *> _m_properties;
};


#endif
