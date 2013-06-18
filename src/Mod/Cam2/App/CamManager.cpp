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

#include "PreCompiled.h"
#ifndef _PreComp_
#include <Python.h>
#endif

#include <App/Application.h>
#include <App/DocumentObject.h>

#include <Base/Console.h>

#include "CamManager.h"

#include "Features/CamFeature.h"

namespace Cam {
CamManagerInst* CamManagerInst::_pcSingleton = NULL;

CamManagerInst& CamManagerInst::instance()
{
    if (_pcSingleton == NULL)
        _pcSingleton = new CamManagerInst();

    return *_pcSingleton;
}
void CamManagerInst::destruct (void)
{
    if (_pcSingleton != NULL)
        delete _pcSingleton;
    _pcSingleton = NULL;
}

CamManagerInst::CamManagerInst() {
	tpgRunnerKeepAlive = true;
	tpgRunnerThread = NULL;
}

CamManagerInst::~CamManagerInst() {
	tpgRunnerQueueMutex.lock();
	if (tpgRunnerThread != NULL) {
		tpgRunnerKeepAlive = false;
		tpgRunnerQueueMutex.unlock();
		tpgRunnerThread->join();
	}
	else
		tpgRunnerQueueMutex.unlock();
}


/**
 * Adds a TPG to the processing queue
 */
bool CamManagerInst::runTPGByName(const char *FeatName, App::Document* document /*= NULL */) {

	// check/get document
	App::Document* doc = document;
	if (doc == NULL)
		doc = App::GetApplication().getActiveDocument();
	if (doc == NULL) {
		Base::Console().Error("TPG run error: Document not provided and none currently active!\n", FeatName);
		return false;
	}

	// get TPGFeature
	App::DocumentObject *docObj = doc->getObject(FeatName);
	if(docObj && docObj->isDerivedFrom(Cam::TPGFeature::getClassTypeId())) {
		Cam::TPGFeature *tpgFeature = dynamic_cast<Cam::TPGFeature *>(docObj);
		if (tpgFeature) {

			// queue the TPG to run
			return queueTPGRun(tpgFeature->getTPG(), tpgFeature->getTPGSettings());
		}
	}
	Base::Console().Error("TPG run error: Feature '%s' does not appear to be a TPGFeature!\n", FeatName);
	return false;
}

/**
 * Add a TPG to the TPG Runner Queue.  First call will start the tpgRunnerThread.
 */
bool CamManagerInst::queueTPGRun(TPG* tpg, TPGSettings* settings) {

	tpgRunnerQueueMutex.lock(); // exploit this lock to make starting runner thread 'thread-safe'.
	// start the thread if needed
	if (tpgRunnerThread == NULL) {
		tpgRunnerThread = new boost::thread(boost::bind(&CamManagerInst::tpgRunnerThreadMain, this));
	}

	// add the tpg to the processing queue
	if (tpg != NULL && settings != NULL) {
		TPGRunnerItem* tpgRun = new TPGRunnerItem(tpg, settings->clone());
		tpgRunnerQueue.push(tpgRun);
		tpgRunnerQueueMutex.unlock();
		return true;
	}
	tpgRunnerQueueMutex.unlock();
	return false;
}

/**
 * The main function used for the tpgRunnerThread
 */
void CamManagerInst::tpgRunnerThreadMain() {

	Base::Console().Message("TPG Runner thread starting\n");

	// loop until instructed to stop
	while (tpgRunnerKeepAlive) {
		TPGRunnerItem* tpgRun = NULL;

		// Get a TPG to process
		tpgRunnerQueueMutex.lock();
		if (tpgRunnerQueue.size() > 0) {
			tpgRun = tpgRunnerQueue.front();
			tpgRunnerQueue.pop();
		}
		tpgRunnerQueueMutex.unlock();

		// run the next TPG
		if (tpgRun != NULL) {
			Base::Console().Message("Running TPG: %s\n", tpgRun->tpg->getName().toAscii().constData());
			tpgRun->tpg->run(tpgRun->settings, QString::fromAscii("default"));
			Base::Console().Message("Finished TPG: %s\n", tpgRun->tpg->getName().toAscii().constData());
		}
		else {
			sleep(1); //TODO: make this a bit smarter so it can be interupted to stop quickly.
		}
	}
	Base::Console().Message("TPG Runner thread stopping\n");
}

/**
 * Updates the progress of TPG processing
 */
void CamManagerInst::updateProgress(QString tpgid, TPG::State state, int progress) {

	Q_EMIT updatedTPGState(tpgid, state, progress);
}

} /* namespace Cam */

#include <Mod/Cam2/App/moc_CamManager.cpp>

