/***************************************************************************
 *   Copyright (c) 2010 J�rgen Riegel <juergen.riegel@web.de>              *
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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <QRegExp>
#endif

#include "DlgSettingsUnitsImp.h"
#include "NavigationStyle.h"
#include "PrefWidgets.h"
#include <App/Application.h>
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/UnitsApi.h>

using namespace Gui::Dialog;
using namespace Base;

/* TRANSLATOR Gui::Dialog::DlgSettingsUnitsImp */

/**
 *  Constructs a DlgSettingsUnitsImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgSettingsUnitsImp::DlgSettingsUnitsImp(QWidget* parent)
    : PreferencePage( parent )
{
    this->setupUi(this);
    //this->setEnabled(false);
    retranslate();

    //fillUpListBox();

    QObject::connect(comboBox_ViewSystem, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));

	tableWidget->setVisible(false);

}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsUnitsImp::~DlgSettingsUnitsImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgSettingsUnitsImp::fillUpListBox()
{
    //tableWidget->setRowCount(10);
    //for (int i = 0 ; i<9;i++) {
    //    QTableWidgetItem *newItem = new QTableWidgetItem(UnitsApi::getQuantityName((Base::QuantityType)i));
    //    tableWidget->setItem(i, 0, newItem);
    //    
    //    newItem = new QTableWidgetItem(UnitsApi::getPrefUnitOf((Base::QuantityType)i));
    //    tableWidget->setItem(i, 1, newItem);
    //}
}

void DlgSettingsUnitsImp::currentIndexChanged(int index)
{
    if (index < 0)
        return; // happens when clearing the combo box in retranslateUi()
    assert(index>-1 && index <3);

    UnitsApi::setSchema((UnitSystem)index);

    fillUpListBox();
}

void DlgSettingsUnitsImp::saveSettings()
{
    // must be done as very first because we create a new instance of NavigatorStyle
    // where we set some attributes afterwards
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath
        ("User parameter:BaseApp/Preferences/Units");
    hGrp->SetInt("UserSchema", comboBox_ViewSystem->currentIndex());
}

void DlgSettingsUnitsImp::loadSettings()
{
    
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath
        ("User parameter:BaseApp/Preferences/Units");
    comboBox_ViewSystem->setCurrentIndex(hGrp->GetInt("UserSchema",0));
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsUnitsImp::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi(this);
        retranslate();
    }
    else {
        QWidget::changeEvent(e);
    }
}

void DlgSettingsUnitsImp::retranslate()
{
}

#include "moc_DlgSettingsUnitsImp.cpp"
