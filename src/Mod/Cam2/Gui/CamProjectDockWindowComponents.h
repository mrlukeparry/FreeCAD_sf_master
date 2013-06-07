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

#ifndef CAMPROJECTDOCKWINDOWCOMPONENTS_H_
#define CAMPROJECTDOCKWINDOWCOMPONENTS_H_

#include <QLineEdit>
#include <QFormLayout>
#include <QList>
#include <QWidget>
#include <QRadioButton>
#include <QMap>

#include "../App/TPG/TPGSettings.h"

namespace CamGui {

class CamGuiExport CamComponent {
protected:
    Cam::TPGSetting *tpgsetting;
    QFormLayout* form;
    QList<QWidget*> rootComponents;

public:
    CamComponent();
    virtual ~CamComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::TPGSetting *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool save();
};

// ----- CamTextBoxComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::TextBox setting
 */
class CamGuiExport CamTextBoxComponent: public CamComponent {
protected:
    QLineEdit *widget;

public:

    CamTextBoxComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::TPGSetting *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool save();

};

// ----- CamRadioComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::Radio setting
 */
class CamGuiExport CamRadioComponent: public CamComponent {
protected:
//    QList<QRadioButton *> radios;
    QMap<QString, QRadioButton*> radios;

public:

    CamRadioComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::TPGSetting *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool save();

};

} /* namespace Cam */
#endif /* CAMPROJECTDOCKWINDOWCOMPONENTS_H_ */
