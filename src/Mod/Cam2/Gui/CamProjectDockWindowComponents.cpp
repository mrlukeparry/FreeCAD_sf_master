/***************************************************************************
 *   Copyright (c) 2013 Andrew Robinson <andrewjrobinson@gmail.com>        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include <PreCompiled.h>
#ifndef _PreComp_
#endif

#include <cstdlib>
#include <QLabel>
#include <QIntValidator>
#include <QMessageBox>
#include <QComboBox>
#include <QFileDialog>
#include <QColor>
#include <QColorDialog>
#include <QPalette>

#include <Base/Console.h>

#include "CamProjectDockWindowComponents.h"

namespace CamGui {


// ----- CamComponent ---------------------------------------------------------
CamComponent::CamComponent() {
    form = NULL;
    tpgsetting = NULL;
	validator = NULL;
}

CamComponent::~CamComponent() {

    // release my hold on the settings object
    if (tpgsetting != NULL) {
        tpgsetting->release();
        tpgsetting = NULL;
    }

    // Remove my components
    QList<QWidget*>::iterator it = rootComponents.begin();
    for (; it != rootComponents.end(); ++it)
	{
        delete *it;
	}
    rootComponents.clear();

	// NOTE: There is no need to explicitly delete the Validator object as the deletion of
	// the QWidget implicitly deletes the Validator object for us.
}

QValidator::State CamComponent::Validator::validate(QString & input, int & position) const
{
	switch(this->setting_definition->validate(input, position))
	{
	case Cam::Settings::Definition::Acceptable:
		return(QValidator::Acceptable);

	case Cam::Settings::Definition::Intermediate:
		return(QValidator::Intermediate);

	default:
	case Cam::Settings::Definition::Invalid:
		return(QValidator::Invalid);
	}
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {
    Base::Console().Log("Warning: Unimplemented makeUI() method or unnecessary call to base method.");
    return false;
}

/**
 * Saves the values on the UI to the TPGSetting instance
 */
bool CamComponent::close() {
    Base::Console().Log("Warning: Unimplemented save() method or unnecessary call to base method.");
    return false;
}


// ----- CamTextBoxComponent ---------------------------------------------------------


CamTextBoxComponent::CamTextBoxComponent()
: QObject(), CamComponent() {
    this->widget = NULL;
}

/**
 * Slot to receive messages when the user changes the text value
 */
void CamTextBoxComponent::editingFinished() {
	if (widget != NULL && tpgsetting != NULL) {
		switch (tpgsetting->type)
		{
		case Cam::Settings::Definition::SettingType_Double:
			{
				Cam::Settings::Double *double_setting = (Cam::Settings::Double *) tpgsetting;
				if (double_setting)
				{
					double value;
					if (double_setting->Evaluate(widget->text().toAscii().constData(), &value))
					{
						double_setting->set(value);
					}
				}
			}
			break;

		default:
			{
				QString qvalue = widget->text();
				if (!tpgsetting->setValue(qvalue))
				{
					Base::Console().Error("Saving failed: '%s'\n", tpgsetting->name.toStdString().c_str());
					widget->setText(tpgsetting->getValue());
				}
			}
			break;
		}
	}
	else
		Base::Console().Error("Saving a setting failed!\n");
}

void CamLineEdit::focusOutEvent ( QFocusEvent * e )
{
	if (this->hasAcceptableInput() == false)
	{
		QString message = QString::fromAscii("Value for ");
		message += this->tpgSetting->name;
		message += QString::fromAscii(" has been rejected as invalid");

		QMessageBox message_box;
		message_box.setText(message);
		message_box.setInformativeText(QString::fromAscii("Do you want to keep the value entered or revert to the previous value?"));
		message_box.setStandardButtons( QMessageBox::Save | QMessageBox::Discard );
		message_box.setDefaultButton( QMessageBox::Discard );
		int response = message_box.exec();
		if (response == QMessageBox::Discard)
		{
			this->setText(this->tpgSetting->getValue());
		}
		else
		{
			QLineEdit::focusOutEvent(e);
		}
	}
	else
	{
		QLineEdit::focusOutEvent(e);
	}
}

void CamTextBoxComponent::handleAddObjectNameButton()
{
	int j=3;
	int k=j;
}

void CamTextBoxComponent::handleOKButton()
{
	int j=3;
	int k=j;
	close();
}

void CamTextBoxComponent::handleButton()
{
	if (this->tpgsetting->type == Cam::Settings::Definition::SettingType_ObjectNamesForType)
	{
		boost::scoped_ptr<QDialog> dialog(new QDialog);
		boost::scoped_ptr<QVBoxLayout> layout(new QVBoxLayout);
		boost::scoped_ptr<QPushButton> add_button(new QPushButton);
		boost::scoped_ptr<QPushButton> ok_button(new QPushButton);
		boost::scoped_ptr<QLineEdit> dummy_edit(new QLineEdit);

		add_button->setText(QString::fromAscii("Add"));
		ok_button->setText(QString::fromAscii("OK"));
		dummy_edit->setText(QString::fromAscii("One day this dialog will present a list of all objects whose types match this setting and allow the object names to be selected"));

		add_button->connect( add_button.get(), SIGNAL(clicked()), this, SLOT(handleAddObjectNameButton()));
		layout->addWidget(add_button.get());

		ok_button->connect( ok_button.get(), SIGNAL(clicked()), this, SLOT(handleOKButton()));
		layout->addWidget(ok_button.get());

		layout->addWidget(dummy_edit.get());

		dialog->setGeometry(0,0,200,100);
		QString title = QString::fromAscii("Edit object names list");
		dialog->setWindowTitle(title);
		dialog->setLayout(layout.get());
		dialog->setWindowFlags(Qt::Dialog);		

		dialog->show();
		dialog->exec();
	}
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamTextBoxComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {
    if (tpgsetting != NULL)
    {
        // grab a copy of the setting so we can save it later
        this->tpgsetting = tpgsetting->grab();

        // construct the ui
        QWidget *parent = dynamic_cast<QWidget*>(form->parent());
        if (parent != NULL) {
            int row = form->rowCount();
            QString qname = tpgsetting->getFullname();

            // make the label
            QWidget *labelWidget = new QLabel(tpgsetting->label, parent);
			labelWidget->setObjectName(qname + QString::fromUtf8("Label"));
            form->setWidget(row, QFormLayout::LabelRole, labelWidget);
            labelWidget->setToolTip(tpgsetting->helptext);

			// make the container
            QWidget *widget = new QWidget(parent);
            QHBoxLayout *layout = new QHBoxLayout(widget);
            layout->setContentsMargins(0,0,0,0);
            widget->setLayout(layout);
            widget->setObjectName(qname);
            widget->setToolTip(tpgsetting->helptext);
			form->setWidget(row, QFormLayout::FieldRole, widget);

            // make the edit box
            CamLineEdit *camLineEdit = new CamLineEdit(parent, tpgsetting);
            camLineEdit->setObjectName(qname);

			switch(tpgsetting->type)
			{
			case Cam::Settings::Definition::SettingType_Double:
				{
					Cam::Settings::Double *double_setting = dynamic_cast<Cam::Settings::Double *>(tpgsetting);
					std::ostringstream ossValue;
					ossValue << double_setting->get();
					camLineEdit->setText(QString::fromStdString(ossValue.str()));
				}
				break;

			case Cam::Settings::Definition::SettingType_Text:
			case Cam::Settings::Definition::SettingType_ObjectNamesForType:
			case Cam::Settings::Definition::SettingType_Integer:
			default:
				if (tpgsetting->getValue() != QString::null)
				{
					camLineEdit->setText(tpgsetting->getValue());
				}
				else
				{
					camLineEdit->setText(tpgsetting->defaultvalue);
				}
			}

            camLineEdit->setToolTip(tpgsetting->helptext);
			camLineEdit->setPlaceholderText(tpgsetting->helptext);

			this->validator = new Validator(tpgsetting->grab(), camLineEdit);
			camLineEdit->setValidator(validator);

            form->setWidget(row, QFormLayout::FieldRole, camLineEdit);

            // connect events
        	QObject::connect(widget, SIGNAL(editingFinished()), this,
        			SLOT(editingFinished()));

			layout->addWidget(camLineEdit);

			if (this->tpgsetting->type == Cam::Settings::Definition::SettingType_ObjectNamesForType)
			{
				// make the push button
				QString qvalue = tpgsetting->getValue();
				QToolButton *button = new QToolButton(widget);
				button->setObjectName(QString::fromAscii("SelectFile"));
				button->setText(QString::fromAscii("..."));
				// connect events
        		QObject::connect(button, SIGNAL(pressed()), this,
        				SLOT(handleButton()));
				layout->addWidget(button);
				rootComponents.push_back(button);
			}

            // keep reference to widgets for later cleanup
            rootComponents.push_back(labelWidget);
            rootComponents.push_back(widget);
			

            return true;
        }
        Base::Console().Warning("Warning: Unable to find parent widget for (%p)\n", form->parent());
    }

    Base::Console().Warning("Warning: Not given a TPGSettingDefinition\n");
    return false;
}

/**
 * Saves the values on the UI to the TPGSetting instance
 */
bool CamTextBoxComponent::close() {
//    if (widget != NULL && tpgsetting != NULL) {
//    	QString qvalue = widget->text();
//        return tpgsetting->setValue(qvalue);
//    }
    return true;
}


CamLengthComponent::CamLengthComponent()
: QObject(), CamComponent() {
    this->camLineEdit = NULL;

	this->values.push_back( std::make_pair(QString::fromAscii("mm"), QString::fromAscii("mm")) );
	this->values.push_back( std::make_pair(QString::fromAscii("inch"), QString::fromAscii("inch")) );
}

/**
 * Slot to receive messages when the user changes the text value
 */
void CamLengthComponent::editingFinished() {
	if (camLineEdit != NULL && tpgsetting != NULL) {
		QString qvalue = camLineEdit->text();
		Cam::Settings::Length *length = dynamic_cast<Cam::Settings::Length *>(this->tpgsetting);
		double value;
		if (length->Evaluate(camLineEdit->text().toAscii().constData(), &value))
		{
			length->set( value );
			std::ostringstream ossValue;
			ossValue << length->get(length->getUnits());
			camLineEdit->setText(QString::fromStdString(ossValue.str()));
		}
	}
	else
		Base::Console().Error("Saving a setting failed!\n");
}

/**
 * Apply the changed setting to the TPGFeature so that any interested parties are notified of the change.
 */
void CamLengthComponent::currentIndexChanged ( int current_index )
{
	if ((current_index >= 0) && (current_index < int(values.size())))
	{
		Cam::Settings::Length *length = dynamic_cast<Cam::Settings::Length *>(this->tpgsetting);

		if (values[current_index].first == QString::fromAscii("inch"))
		{
			length->setUnits(Cam::Settings::Definition::Imperial);
			std::ostringstream ossValue;
			ossValue << length->get(length->getUnits());
			camLineEdit->setText(QString::fromStdString(ossValue.str()));
		}
		else
		{
			length->setUnits(Cam::Settings::Definition::Metric);
			std::ostringstream ossValue;
			ossValue << length->get(length->getUnits());
			camLineEdit->setText(QString::fromStdString(ossValue.str()));
		}
	}
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamLengthComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {
    if (tpgsetting != NULL)
    {
        // grab a copy of the setting so we can save it later
        this->tpgsetting = tpgsetting->grab();

        // construct the ui
        QWidget *parent = dynamic_cast<QWidget*>(form->parent());
        if (parent != NULL) {
            int row = form->rowCount();
            QString qname = tpgsetting->getFullname();

            // make the label
            QWidget *labelWidget = new QLabel(tpgsetting->label, parent);
			labelWidget->setObjectName(qname + QString::fromUtf8("Label"));
            form->setWidget(row, QFormLayout::LabelRole, labelWidget);
            labelWidget->setToolTip(tpgsetting->helptext);

            // make the container
            QWidget *widget = new QWidget(parent);
            QHBoxLayout *layout = new QHBoxLayout(widget);
            layout->setContentsMargins(0,0,0,0);
            widget->setLayout(layout);
            widget->setObjectName(qname);
            widget->setToolTip(tpgsetting->helptext);
			form->setWidget(row, QFormLayout::FieldRole, widget);

			// make the edit box
            camLineEdit = new CamLineEdit(parent, tpgsetting);
            camLineEdit->setObjectName(qname);

			Cam::Settings::Length *length = (Cam::Settings::Length *) this->tpgsetting;
			std::ostringstream ossValue;
			ossValue << length->get( length->getUnits() );
			camLineEdit->setText(QString::fromStdString(ossValue.str()));
            camLineEdit->setToolTip(tpgsetting->helptext);
			camLineEdit->setPlaceholderText(tpgsetting->helptext);
			this->validator = new Validator(tpgsetting->grab(), camLineEdit);
			camLineEdit->setValidator(validator);
            // connect events
        	QObject::connect(camLineEdit, SIGNAL(editingFinished()), this,
        			SLOT(editingFinished()));
			layout->addWidget(camLineEdit);

            QComboBox *combo_box = new QComboBox(parent);
			int index = 0;
			for (std::vector< std::pair< Id_t, Label_t > >::const_iterator itValue = values.begin(); itValue != values.end(); itValue++)
			{
				combo_box->addItem(itValue->first);
				if (this->tpgsetting->units == itValue->first)
				{
					combo_box->setCurrentIndex(index);
				}
				index++;
			}

            combo_box->setObjectName(qname);
            combo_box->setToolTip(tpgsetting->helptext);

			this->validator = new Validator(tpgsetting->grab(), combo_box);
			combo_box->setValidator(validator);

            form->setWidget(row, QFormLayout::FieldRole, combo_box);

            // connect events
        	QObject::connect(combo_box, SIGNAL(currentIndexChanged(int)), this,
        			SLOT(currentIndexChanged(int)));
            layout->addWidget(combo_box);

            // keep reference to widgets for later cleanup
            rootComponents.push_back(labelWidget);
			rootComponents.push_back(camLineEdit);
			rootComponents.push_back(combo_box);

            return true;
        }
        Base::Console().Warning("Warning: Unable to find parent widget for (%p)\n", form->parent());
    }

    Base::Console().Warning("Warning: Not given a TPGSettingDefinition\n");
    return false;
}

/**
 * Saves the values on the UI to the TPGSetting instance
 */
bool CamLengthComponent::close() {
//    if (widget != NULL && tpgsetting != NULL) {
//    	QString qvalue = widget->text();
//        return tpgsetting->setValue(qvalue);
//    }
    return true;
}




//--- CamRateComponent ---

CamRateComponent::CamRateComponent()
: QObject(), CamComponent() {
    this->camLineEdit = NULL;

	this->values.push_back( std::make_pair(QString::fromAscii("mm/min"), QString::fromAscii("mm/min")) );
	this->values.push_back( std::make_pair(QString::fromAscii("inch/min"), QString::fromAscii("inch/min")) );
}

/**
 * Slot to receive messages when the user changes the text value
 */
void CamRateComponent::editingFinished() {
	if (camLineEdit != NULL && tpgsetting != NULL) {
		QString qvalue = camLineEdit->text();
		Cam::Settings::Rate *rate = dynamic_cast<Cam::Settings::Rate *>(this->tpgsetting);
		double value;
		if (rate->Evaluate(camLineEdit->text().toAscii().constData(), &value))
		{
			rate->set( value );
			std::ostringstream ossValue;
			ossValue << rate->get(rate->getUnits());
			camLineEdit->setText(QString::fromStdString(ossValue.str()));
		}
	}
	else
		Base::Console().Error("Saving a setting failed!\n");
}

/**
 * Apply the changed setting to the TPGFeature so that any interested parties are notified of the change.
 */
void CamRateComponent::currentIndexChanged ( int current_index )
{
	if ((current_index >= 0) && (current_index < int(values.size())))
	{
		Cam::Settings::Rate *rate = dynamic_cast<Cam::Settings::Rate *>(this->tpgsetting);

		if (values[current_index].first == QString::fromAscii("inch/min"))
		{
			rate->setUnits(Cam::Settings::Definition::Imperial);
			std::ostringstream ossValue;
			ossValue << rate->get(rate->getUnits());
			camLineEdit->setText(QString::fromStdString(ossValue.str()));
		}
		else
		{
			rate->setUnits(Cam::Settings::Definition::Metric);
			std::ostringstream ossValue;
			ossValue << rate->get(rate->getUnits());
			camLineEdit->setText(QString::fromStdString(ossValue.str()));
		}
	}
}


/**
 * Creates the UI for this component and loads the initial value
 */
bool CamRateComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {
    if (tpgsetting != NULL)
    {
        // grab a copy of the setting so we can save it later
        this->tpgsetting = tpgsetting->grab();

        // construct the ui
        QWidget *parent = dynamic_cast<QWidget*>(form->parent());
        if (parent != NULL) {
            int row = form->rowCount();
            QString qname = tpgsetting->getFullname();

            // make the label
            QWidget *labelWidget = new QLabel(tpgsetting->label, parent);
			labelWidget->setObjectName(qname + QString::fromUtf8("Label"));
            form->setWidget(row, QFormLayout::LabelRole, labelWidget);
            labelWidget->setToolTip(tpgsetting->helptext);

            // make the container
            QWidget *widget = new QWidget(parent);
            QHBoxLayout *layout = new QHBoxLayout(widget);
            layout->setContentsMargins(0,0,0,0);
            widget->setLayout(layout);
            widget->setObjectName(qname);
            widget->setToolTip(tpgsetting->helptext);
			form->setWidget(row, QFormLayout::FieldRole, widget);

			// make the edit box
            camLineEdit = new CamLineEdit(parent, tpgsetting);
            camLineEdit->setObjectName(qname);

			Cam::Settings::Rate *rate = dynamic_cast<Cam::Settings::Rate *>(this->tpgsetting);
			std::ostringstream ossValue;
			ossValue << rate->get( rate->getUnits() );
			camLineEdit->setText(QString::fromStdString(ossValue.str()));
            camLineEdit->setToolTip(tpgsetting->helptext);
			camLineEdit->setPlaceholderText(tpgsetting->helptext);
			this->validator = new Validator(tpgsetting->grab(), camLineEdit);
			camLineEdit->setValidator(validator);
            // connect events
        	QObject::connect(camLineEdit, SIGNAL(editingFinished()), this,
        			SLOT(editingFinished()));
			layout->addWidget(camLineEdit);

            QComboBox *combo_box = new QComboBox(parent);
			int index = 0;
			for (std::vector< std::pair< Id_t, Label_t > >::const_iterator itValue = values.begin(); itValue != values.end(); itValue++)
			{
				combo_box->addItem(itValue->first);
				if (this->tpgsetting->units == itValue->first)
				{
					combo_box->setCurrentIndex(index);
				}
				index++;
			}

            combo_box->setObjectName(qname);
            combo_box->setToolTip(tpgsetting->helptext);

			this->validator = new Validator(tpgsetting->grab(), combo_box);
			combo_box->setValidator(validator);

            form->setWidget(row, QFormLayout::FieldRole, combo_box);

            // connect events
        	QObject::connect(combo_box, SIGNAL(currentIndexChanged(int)), this,
        			SLOT(currentIndexChanged(int)));
            layout->addWidget(combo_box);

            // keep reference to widgets for later cleanup
            rootComponents.push_back(labelWidget);
			rootComponents.push_back(camLineEdit);
			rootComponents.push_back(combo_box);

            return true;
        }
        Base::Console().Warning("Warning: Unable to find parent widget for (%p)\n", form->parent());
    }

    Base::Console().Warning("Warning: Not given a TPGSettingDefinition\n");
    return false;
}

/**
 * Saves the values on the UI to the TPGSetting instance
 */
bool CamRateComponent::close() {
//    if (widget != NULL && tpgsetting != NULL) {
//    	QString qvalue = widget->text();
//        return tpgsetting->setValue(qvalue);
//    }
    return true;
}



// ----- CamRadioComponent ---------------------------------------------------------

CamRadioComponent::CamRadioComponent()
: CamComponent() {
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamRadioComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {

    if (tpgsetting != NULL) {
        // grab a copy of the setting so we can save it later
        this->tpgsetting = tpgsetting->grab();

        // construct the ui
        QWidget *parent = dynamic_cast<QWidget*>(form->parent());
        if (parent != NULL) {
            int row = form->rowCount();
            QString qname = tpgsetting->getFullname();

            // make the label
            QWidget *labelWidget = new QLabel(tpgsetting->label, parent);
			labelWidget->setObjectName(qname + QString::fromUtf8("Label"));
            form->setWidget(row, QFormLayout::SpanningRole, labelWidget);
            labelWidget->setToolTip(tpgsetting->helptext);

            // make the container
            QWidget *widget = new QWidget(parent);
            QHBoxLayout *layout = new QHBoxLayout(widget);
            layout->setContentsMargins(0,0,0,0);
            widget->setLayout(layout);
            widget->setObjectName(qname);
            widget->setToolTip(tpgsetting->helptext);
            form->setWidget(row + 1, QFormLayout::SpanningRole, widget);

            // make the radio buttons
            QString qvalue = tpgsetting->getValue();
            QList<Cam::Settings::Option*>::iterator it = tpgsetting->options.begin();
            for (; it != tpgsetting->options.end(); ++it) {
                QRadioButton *btn = new QRadioButton(widget);
                btn->setObjectName(qname + (*it)->id);
                if (qvalue.compare((*it)->id) == 0)
                    btn->setChecked(true);
                btn->setText((*it)->label);
                layout->addWidget(btn);
				radios.insert((*it)->id, btn);
            }

            // keep reference to widgets for later cleanup
            rootComponents.push_back(labelWidget);
            rootComponents.push_back(widget);

            return true;
        }
        Base::Console().Warning("Warning: Unable to find parent widget for (%p)", form->parent());
    }

    Base::Console().Warning("Warning: Not given a TPGSettingDefinition");
    return false;
}

/**
 * Saves the values on the UI to the TPGSetting instance
 */
bool CamRadioComponent::close() {

	//TODO: make this happen when radio buttons lose focus or different one is selected
	QMap<QString, QRadioButton*>::const_iterator it = radios.constBegin();
	for (; it != radios.constEnd(); ++it) {
		if (it.value()->isChecked()) {
			QString qvalue = it.key();
			return tpgsetting->setValue(qvalue);
		}
	}

    return false;
}




// ----- CamComboBoxComponent ---------------------------------------------------------

CamComboBoxComponent::CamComboBoxComponent()
: CamComponent() {
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamComboBoxComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {

    if (tpgsetting != NULL) {
        // grab a copy of the setting so we can save it later
        this->tpgsetting = tpgsetting->grab();

        // construct the ui
        QWidget *parent = dynamic_cast<QWidget*>(form->parent());
        if (parent != NULL) {
            int row = form->rowCount();
            QString qname = tpgsetting->getFullname();

            // make the label
            QWidget *labelWidget = new QLabel(tpgsetting->label, parent);
			labelWidget->setObjectName(qname + QString::fromUtf8("Label"));
			form->setWidget(row, QFormLayout::LabelRole, labelWidget);
            labelWidget->setToolTip(tpgsetting->helptext);

            // make the container
			// Keep our own copy of the id/label pairs in a vector so that we're sure to
			// associate the 'index' of the combo-box with the correct options in both
			// this method (which defines them) and in the currentIndexChanged() (which
			// looks for which one was chosen).
			QComboBox *combo_box = new QComboBox(parent);
			int index = 0;
			for (QList<Cam::Settings::Option *>::const_iterator itOption = this->tpgsetting->options.begin(); itOption != this->tpgsetting->options.end(); itOption++)
			{
				combo_box->addItem((*itOption)->label);
				if (this->tpgsetting->getValue() == (*itOption)->id)
				{
					combo_box->setCurrentIndex(index);
				}
				values.push_back( std::make_pair( (*itOption)->id, (*itOption)->label ) );
				index++;
			}

            combo_box->setObjectName(qname);
            combo_box->setToolTip(tpgsetting->helptext);

			this->validator = new Validator(tpgsetting->grab(), combo_box);
			combo_box->setValidator(validator);

            form->setWidget(row, QFormLayout::FieldRole, combo_box);

            // connect events
        	QObject::connect(combo_box, SIGNAL(currentIndexChanged(int)), this,
        			SLOT(currentIndexChanged(int)));

            // keep reference to widgets for later cleanup
            rootComponents.push_back(labelWidget);
            rootComponents.push_back(combo_box);

            return true;
        }
        Base::Console().Warning("Warning: Unable to find parent widget for (%p)", form->parent());
    }

    Base::Console().Warning("Warning: Not given a TPGSettingDefinition");
    return false;
}

/**
 * Apply the changed setting to the TPGFeature so that any interested parties are notified of the change.
 */
void CamComboBoxComponent::currentIndexChanged ( int current_index )
{
	if ((current_index >= 0) && (current_index < int(values.size())))
	{
		this->tpgsetting->setValue(values[current_index].first);
	}
}

/**
 * Saves the values on the UI to the TPGSetting instance.
 * NOTE: We don't save it here as the value will already have been set in the currentIndexChanged() method.
 */
bool CamComboBoxComponent::close() {
	
    return true;	// Assume all is well.
}







CamFilenameComponent::CamFilenameComponent()
: QObject(), CamComponent() {
    this->camLineEdit = NULL;
	this->button = NULL;
}

/**
 * Slot to receive messages when the user changes the text value
 */
void CamFilenameComponent::editingFinished() {
	if (camLineEdit != NULL && tpgsetting != NULL) {
		QString qvalue = camLineEdit->text();
		if (!tpgsetting->setValue(qvalue))
		{
			Base::Console().Error("Saving failed: '%s'\n", tpgsetting->name.toStdString().c_str());
			camLineEdit->setText(tpgsetting->getValue());
		}
	}
	else
		Base::Console().Error("Saving a setting failed!\n");
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamFilenameComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {
    if (tpgsetting != NULL)
    {
        // grab a copy of the setting so we can save it later
        this->tpgsetting = tpgsetting->grab();

        // construct the ui
        QWidget *parent = dynamic_cast<QWidget*>(form->parent());
        if (parent != NULL) {
            int row = form->rowCount();
            QString qname = tpgsetting->getFullname();

            // make the label
            QWidget *labelWidget = new QLabel(tpgsetting->label, parent);
			labelWidget->setObjectName(qname + QString::fromUtf8("Label"));
            form->setWidget(row, QFormLayout::LabelRole, labelWidget);
            labelWidget->setToolTip(tpgsetting->helptext);

            // make the container
            QWidget *widget = new QWidget(parent);
            QHBoxLayout *layout = new QHBoxLayout(widget);
            layout->setContentsMargins(0,0,0,0);
            widget->setLayout(layout);
            widget->setObjectName(qname);
            widget->setToolTip(tpgsetting->helptext);
			form->setWidget(row, QFormLayout::FieldRole, widget);

			// make the edit box
            camLineEdit = new CamLineEdit(parent, tpgsetting);
            camLineEdit->setObjectName(qname);
            camLineEdit->setText(tpgsetting->getValue());
            camLineEdit->setToolTip(tpgsetting->helptext);
			camLineEdit->setPlaceholderText(tpgsetting->helptext);
			this->validator = new Validator(tpgsetting->grab(), camLineEdit);
			camLineEdit->setValidator(validator);
            // connect events
        	QObject::connect(camLineEdit, SIGNAL(editingFinished()), this,
        			SLOT(editingFinished()));
			layout->addWidget(camLineEdit);

            // make the push button
            QString qvalue = tpgsetting->getValue();
            button = new QToolButton(widget);
			button->setObjectName(QString::fromAscii("SelectFile"));
            button->setText(QString::fromAscii("..."));
			// connect events
        	QObject::connect(button, SIGNAL(pressed()), this,
        			SLOT(handleButton()));
            layout->addWidget(button);

            // keep reference to widgets for later cleanup
            rootComponents.push_back(labelWidget);
            rootComponents.push_back(camLineEdit);
			rootComponents.push_back(button);
			rootComponents.push_back(widget);

            return true;
        }
        Base::Console().Warning("Warning: Unable to find parent widget for (%p)\n", form->parent());
    }

    Base::Console().Warning("Warning: Not given a TPGSettingDefinition\n");
    return false;
}

/**
 * Saves the values on the UI to the TPGSetting instance
 */
bool CamFilenameComponent::close() {
//    if (widget != NULL && tpgsetting != NULL) {
//    	QString qvalue = widget->text();
//        return tpgsetting->setValue(qvalue);
//    }
    return true;
}

void CamFilenameComponent::handleButton()
{
	QString caption = QString::fromAscii("Caption");
	QString directory = QString::fromAscii("c:\\david");
	QString filter = QString::fromAscii("*.*");

	QString filename = QFileDialog::getOpenFileName(NULL, caption, directory, filter);
	if (filename.isNull() == false)
	{
		this->camLineEdit->setText(filename);
		this->tpgsetting->setValue(filename);
	}
}








CamDirectoryComponent::CamDirectoryComponent()
: QObject(), CamComponent() {
    this->camLineEdit = NULL;
	this->button = NULL;
}

/**
 * Slot to receive messages when the user changes the text value
 */
void CamDirectoryComponent::editingFinished() {
	if (camLineEdit != NULL && tpgsetting != NULL) {
		QString qvalue = camLineEdit->text();
		if (!tpgsetting->setValue(qvalue))
		{
			Base::Console().Error("Saving failed: '%s'\n", tpgsetting->name.toStdString().c_str());
			camLineEdit->setText(tpgsetting->getValue());
		}
	}
	else
		Base::Console().Error("Saving a setting failed!\n");
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamDirectoryComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {
    if (tpgsetting != NULL)
    {
        // grab a copy of the setting so we can save it later
        this->tpgsetting = tpgsetting->grab();

        // construct the ui
        QWidget *parent = dynamic_cast<QWidget*>(form->parent());
        if (parent != NULL) {
            int row = form->rowCount();
            QString qname = tpgsetting->getFullname();

            // make the label
            QWidget *labelWidget = new QLabel(tpgsetting->label, parent);
			labelWidget->setObjectName(qname + QString::fromUtf8("Label"));
            form->setWidget(row, QFormLayout::LabelRole, labelWidget);
            labelWidget->setToolTip(tpgsetting->helptext);

            // make the container
            QWidget *widget = new QWidget(parent);
            QHBoxLayout *layout = new QHBoxLayout(widget);
            layout->setContentsMargins(0,0,0,0);
            widget->setLayout(layout);
            widget->setObjectName(qname);
            widget->setToolTip(tpgsetting->helptext);
			form->setWidget(row, QFormLayout::FieldRole, widget);

			// make the edit box
            camLineEdit = new CamLineEdit(parent, tpgsetting);
            camLineEdit->setObjectName(qname);
            camLineEdit->setText(tpgsetting->getValue());
            camLineEdit->setToolTip(tpgsetting->helptext);
			camLineEdit->setPlaceholderText(tpgsetting->helptext);
			this->validator = new Validator(tpgsetting->grab(), camLineEdit);
			camLineEdit->setValidator(validator);
            // connect events
        	QObject::connect(camLineEdit, SIGNAL(editingFinished()), this,
        			SLOT(editingFinished()));
			layout->addWidget(camLineEdit);

            // make the push button
            QString qvalue = tpgsetting->getValue();
            button = new QToolButton(widget);
			button->setObjectName(QString::fromAscii("SelectFile"));
            button->setText(QString::fromAscii("..."));
			// connect events
        	QObject::connect(button, SIGNAL(pressed()), this,
        			SLOT(handleButton()));
            layout->addWidget(button);

            // keep reference to widgets for later cleanup
            rootComponents.push_back(labelWidget);
			rootComponents.push_back(camLineEdit);
			rootComponents.push_back(button);
            rootComponents.push_back(widget);

            return true;
        }
        Base::Console().Warning("Warning: Unable to find parent widget for (%p)\n", form->parent());
    }

    Base::Console().Warning("Warning: Not given a TPGSettingDefinition\n");
    return false;
}

/**
 * Saves the values on the UI to the TPGSetting instance
 */
bool CamDirectoryComponent::close() {
//    if (widget != NULL && tpgsetting != NULL) {
//    	QString qvalue = widget->text();
//        return tpgsetting->setValue(qvalue);
//    }
    return true;
}

void CamDirectoryComponent::handleButton()
{
	QString caption = QString::fromAscii("Caption");
	QString directory = QString::fromAscii("c:\\david");
	QString filter = QString::fromAscii("*.*");

	QString dir = QFileDialog::getExistingDirectory(NULL, caption, directory, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isNull() == false)
	{
		this->camLineEdit->setText(dir);
		this->tpgsetting->setValue(dir);
	}
}






CamColorComponent::CamColorComponent()
: QObject(), CamComponent() {
	this->button = NULL;
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamColorComponent::makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form) {
    if (tpgsetting != NULL)
    {
        // grab a copy of the setting so we can save it later
        this->tpgsetting = tpgsetting->grab();

        // construct the ui
        QWidget *parent = dynamic_cast<QWidget*>(form->parent());
        if (parent != NULL) {
            int row = form->rowCount();
            QString qname = tpgsetting->getFullname();

            // make the label
            QWidget *labelWidget = new QLabel(tpgsetting->label, parent);
			labelWidget->setObjectName(qname + QString::fromUtf8("Label"));
            form->setWidget(row, QFormLayout::LabelRole, labelWidget);
            labelWidget->setToolTip(tpgsetting->helptext);

            // make the container
            QWidget *widget = new QWidget(parent);

            // make the push button
            QString qvalue = tpgsetting->getValue();
            button = new QPushButton(widget);
			button->setObjectName(QString::fromAscii("SelectFile"));
            button->setText(QString::fromAscii("   "));

			int red, green, blue, alpha;
			Cam::Settings::Color *pColorSetting = (Cam::Settings::Color *) this->tpgsetting;
			pColorSetting->get( red, green, blue, alpha );
			QColor color(red, green, blue, alpha);
			QPalette palette(color);
			button->setPalette(palette);
			button->setAutoFillBackground(true);
			button->setFlat(true);

			// connect events
        	QObject::connect(button, SIGNAL(pressed()), this,
        			SLOT(handleButton()));
			form->setWidget(row, QFormLayout::FieldRole, widget);

            // keep reference to widgets for later cleanup
            rootComponents.push_back(labelWidget);
            rootComponents.push_back(widget);

            return true;
        }
        Base::Console().Warning("Warning: Unable to find parent widget for (%p)\n", form->parent());
    }

    Base::Console().Warning("Warning: Not given a TPGSettingDefinition\n");
    return false;
}

/**
 * Saves the values on the UI to the TPGSetting instance
 */
bool CamColorComponent::close() {
//    if (widget != NULL && tpgsetting != NULL) {
//    	QString qvalue = widget->text();
//        return tpgsetting->setValue(qvalue);
//    }
    return true;
}

void CamColorComponent::handleButton()
{
	Cam::Settings::Color *pColorSetting = (Cam::Settings::Color *) this->tpgsetting;

	int red, green, blue, alpha;
	pColorSetting->get(red, green, blue, alpha);
	QColor initial(red, green, blue, alpha);

	QColor color = QColorDialog::getColor(initial, NULL);
	if (color.isValid())
	{
		QPalette palette(color);
		this->button->setPalette(palette);
		this->button->setAutoFillBackground(true);
		this->button->setFlat(true);

		// Cast the setting pointer to a TPGColorSettingDefinition so that we can generate
		// the verbose (string) represenation of the QColor for storage in the PropTPGSettings property
		// within the TPGFeature.

		
		pColorSetting->set( color.red(), color.green(), color.blue(), color.alpha() );
	}
	
}




#include "moc_CamProjectDockWindowComponents.cpp"

} /* namespace Cam */
