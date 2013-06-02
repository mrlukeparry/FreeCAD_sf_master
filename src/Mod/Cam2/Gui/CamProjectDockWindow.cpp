/***************************************************************************
 *   Copyright (c) 2012 Andrew Robinson <andrewjrobinson@gmail.com>        *
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
#endif
#include <qlist.h>

#include <Base/Console.h>

#include "ui_CamProjectDockWindow.h"
#include "CamProjectDockWindow.h"
#include "UIManager.h"

namespace CamGui {

CamProjectDockWindow::CamProjectDockWindow(Gui::Document* pcDocument,
		QWidget *parent) :
		DockWindow(pcDocument, parent), ui(new Ui_CamProjectDockWindow) {
	setWindowTitle(tr("Cam Project"));

	ui->setupUi(this);

	currentSettings = NULL;

	QObject::connect(&UIManager(), SIGNAL(updatedTPGSelection(Cam::TPG*)), this,
			SLOT(updatedTPGSelection(Cam::TPG*)));

	// receive tpg running state changes from Cam layer.
	QObject::connect(&CamGui::UIManager(), SIGNAL(updatedTPGStateSig(QString, Cam::TPG::State, int)),
			this, SLOT(updatedTPGState(QString, Cam::TPG::State, int)));
}

CamProjectDockWindow::~CamProjectDockWindow()
{
}

/**
 * Causes the setting area to display a form for editing the selected
 * setting.  The form is populated with the current values from the setting;
 * Caller needs to set the value to defaultvalue if that is desired.
 *
 * Provide a NULL newSettings to stop edit any settings.
 */
bool CamProjectDockWindow::editSettings(Cam::TPGSettings* newSettings, bool saveOld /*=true*/)
{
    // save old settings
    if (saveOld && ! saveSettings()) {
        //TODO: tell the user saving settings failed
        return false;
    }

    // clear old form components
    QList<CamComponent*>::iterator it = components.begin();
    for (; it != components.end(); ++it)
        delete *it;
    components.clear();

    // create the new form
    if (newSettings != NULL)
    {
        currentSettings = newSettings->grab();
        CamComponent *comp = NULL;
        bool failure = false;
        std::vector<Cam::TPGSetting*> settings = newSettings->getSettings();
        std::vector<Cam::TPGSetting*>::iterator it = settings.begin();
        QString camTextBox = "Cam::TextBox";
        QString camRadio = "Cam::Radio";
        for (; it != settings.end(); ++it)
        {
            Cam::TPGSetting* setting = *it;
            if (setting != NULL)
            {
                if (camTextBox.compare(setting->type) == 0)
                    comp = new CamTextBoxComponent();
                else if (camRadio.compare(setting->type) == 0)
                    comp = new CamRadioComponent();
                else
                    continue;
                components.append(comp);
                if (!comp->makeUI(setting,(QFormLayout*)ui->SettingsWidget->layout()))
                    failure = true;
            }
            else
                Base::Console().Log("Warning: NULL setting provided");
        }
        return !failure;
    }

    return true;
}

/**
 * Saves the current values from the UI into the settings objects.
 *
 * Note: this is called by default on when changing settings (if saveOld is
 * true).
 */
bool CamProjectDockWindow::saveSettings()
{
    bool failure = false;
    QList<CamComponent*>::iterator it = components.begin();
    for (; it != components.end(); ++it)
        if (!(*it)->save())
            failure = true;
    return !failure;
}

/**
 * Slot that is called from the UIManager when the selection is changed.
 */
void CamProjectDockWindow::updatedTPGSelection(Cam::TPG* tpg) {

    if (tpg != NULL) {
        QString action = "default";
		Cam::TPGSettings *settings = tpg->getSettings(action);
		if (!this->editSettings(settings))
			Base::Console().Error("Failed to edit settings for '%s'\n", tpg->getName().toAscii().constData());
    }
    else
    	this->editSettings(NULL);
}


/**
 * Receive messages to update a progress bar
 */
void CamProjectDockWindow::updatedTPGState(QString tpgid, Cam::TPG::State state, int progress) {

	ProgressBar *pb = NULL;
	if (!progressBars.contains(tpgid)) {
		pb = new ProgressBar(this->ui->scrollAreaWidgetContents, tpgid);
		progressBars.insert(tpgid, pb);
	} else {
		pb = progressBars[tpgid];
	}
	pb->updateState(state, progress);

	if (state == Cam::TPG::FINISHED) {
		progressBars.remove(tpgid);

		delete pb; //TODO: delay this by a timer (maybe 10 seconds)
	}
}

#include "moc_CamProjectDockWindow.cpp"

} /* namespace CamGui */
