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

#include <Base/Console.h>

#include "CamProjectDockWindowComponents.h"

namespace CamGui {


// ----- CamComponent ---------------------------------------------------------
CamComponent::CamComponent() {
    form = NULL;
    tpgsetting = NULL;
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
        delete *it;
    rootComponents.clear();
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamComponent::makeUI(Cam::TPGSettingDefinition *tpgsetting, QFormLayout* form) {
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
		QString qvalue = widget->text();
		if (!tpgsetting->setValue(qvalue))
			Base::Console().Error("Saving failed: '%s'\n", tpgsetting->name.toStdString().c_str());
	}
	else
		Base::Console().Error("Saving a setting failed!\n");
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamTextBoxComponent::makeUI(Cam::TPGSettingDefinition *tpgsetting, QFormLayout* form) {
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

            // make the edit box
            widget = new QLineEdit(parent);
            widget->setObjectName(qname);
            widget->setText(tpgsetting->getValue());
            widget->setToolTip(tpgsetting->helptext);
            form->setWidget(row, QFormLayout::FieldRole, widget);

            // connect events
        	QObject::connect(widget, SIGNAL(editingFinished()), this,
        			SLOT(editingFinished()));

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


// ----- CamRadioComponent ---------------------------------------------------------

CamRadioComponent::CamRadioComponent()
: CamComponent() {
}

/**
 * Creates the UI for this component and loads the initial value
 */
bool CamRadioComponent::makeUI(Cam::TPGSettingDefinition *tpgsetting, QFormLayout* form) {

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
            QList<Cam::TPGSettingOption*>::iterator it = tpgsetting->options.begin();
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


#include "moc_CamProjectDockWindowComponents.cpp"

} /* namespace Cam */
