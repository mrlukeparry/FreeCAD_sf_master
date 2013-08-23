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
#include <Python.h>
#endif

#include <App/Application.h>
#include <App/DocumentObject.h>

#include <Base/Console.h>

#include "CamManager.h"

#include "Features/CamFeature.h"
#include "PostProcessor.h"
#include "Graphics/Paths.h"

#include "LinuxCNC.h"	// For DEBUG only.  We need to move the GCode parsing out to somewhere else.

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

	/*
	{
		Part::Feature *gcFeature = (Part::Feature *)doc->addObject("Part::Feature", doc->getUniqueObjectName("GCodeFeature").c_str());
		TopoDS_Edge line = Cam::Edge( Cam::Point(0.0, 0.0, 0.0), Cam::Point(100.0, 0.0, 0.0) );
		gcFeature->Shape.setValue(line);
	}

	{
		Part::Feature *gcFeature = (Part::Feature *)doc->addObject("Part::Feature", doc->getUniqueObjectName("GCodeFeature").c_str());
		TopoDS_Edge line = Cam::Edge( Cam::Point(0.0, 0.0, 0.0), Cam::Point(100.0, 0.0, 0.0) );
		gcFeature->Shape.setValue(line);
	}
	*/

	// get TPGFeature
	App::DocumentObject *docObj = doc->getObject(FeatName);
	if(docObj && docObj->isDerivedFrom(Cam::TPGFeature::getClassTypeId())) {
		Cam::TPGFeature *tpgFeature = dynamic_cast<Cam::TPGFeature *>(docObj);
		if (tpgFeature) {
			// queue the TPG to run
			return queueTPGRun(tpgFeature->getTPG(), tpgFeature->getTPGSettings(), tpgFeature);
		}
	}
	Base::Console().Error("TPG run error: Feature '%s' does not appear to be a TPGFeature!\n", FeatName);
	return false;
}



bool CamManagerInst::runPostProcessByName(const char *FeatName, App::Document* document /*= NULL */) {

	// check/get document
	App::Document* doc = document;
	if (doc == NULL)
		doc = App::GetApplication().getActiveDocument();
	if (doc == NULL) {
		Base::Console().Error("TPG run error: Document not provided and none currently active!\n", FeatName);
		return false;
	}

	/*
	{
		Part::Feature *gcFeature = (Part::Feature *)doc->addObject("Part::Feature", doc->getUniqueObjectName("GCodeFeature").c_str());
		TopoDS_Edge line = Cam::Edge( Cam::Point(0.0, 0.0, 0.0), Cam::Point(100.0, 0.0, 0.0) );
		gcFeature->Shape.setValue(line);
	}

	{
		Part::Feature *gcFeature = (Part::Feature *)doc->addObject("Part::Feature", doc->getUniqueObjectName("GCodeFeature").c_str());
		TopoDS_Edge line = Cam::Edge( Cam::Point(0.0, 0.0, 0.0), Cam::Point(100.0, 0.0, 0.0) );
		gcFeature->Shape.setValue(line);
	}
	*/

	// get TPGFeature
	App::DocumentObject *docObj = doc->getObject(FeatName);
	if(docObj && docObj->isDerivedFrom(Cam::TPGFeature::getClassTypeId())) {
		Cam::TPGFeature *tpgFeature = dynamic_cast<Cam::TPGFeature *>(docObj);
		if (tpgFeature) {
			TPG* tpg = tpgFeature->getTPG();
			if (tpg)
			{
				// Just playing.  This part needs to go away later on.
				// I would expect that all the tpgRun->tpg->run() threads will return before any of their
				// ToolPath objects are executed by the Python interpreter.  I'm just doing it here
				// for now because I don't know how to work around the user interface stuff well enough
				// to actually do this.

				ToolPath *toolpath = tpg->getToolPath();	// copy (and add reference counter to) the ToolPath object.
				if (! toolpath)
				{
					// We must not have executed the TPG already.  Do so now.
					tpg->run(tpgFeature->getTPGSettings(), QString::fromAscii("default"));
					toolpath = tpg->getToolPath();
				}

				if (toolpath)
				{
					// Just log the whole program here for interest sake.  Not for real use.
					QString python_program;
					python_program << *toolpath;
					qDebug("%s\n", python_program.toAscii().constData());

					// Execute the Python program contained in the ToolPath object to produce GCode.
					Cam::MachineProgram *machine_program = Cam::PostProcessor().postProcess(toolpath, (Cam::Item *) NULL);
					
					// The only indication as to whether errors occured is if the getErrors() method returns
					// a QStringList that is not empty.
					if ((machine_program->getErrors()) && (machine_program->getErrors()->size() > 0))
					{
						// Something went wrong.  Present the errors to the operator.
						// TODO - This needs to appear in a popup dialog or similar so the operator knows
						// someting went wrong.
						for (QStringList::size_type i=0; i<machine_program->getErrors()->size(); i++)
						{
							qCritical("%s\n", machine_program->getErrors()->at(i).toAscii().constData());
						}
					}
					else
					{
						// There were no errors so the GCode must have been generated OK.
						// Log it here for now but it will really need to go somewhere more useful at some point.

						QString gcode;
						gcode << *machine_program;
						qDebug("%s\n", gcode.toAscii().constData());

						LinuxCNC parser(machine_program, tpgFeature);
						if (parser.Parse())
						{
							qDebug("GCode parsed OK\n");

							// And release both the toolpath and the machine_program objects now.  The toolpath probably
							// won't go away at this point but it's important that we let it know that we're finished with
							// it.  The machine_program, on the other hand, probably will go away as we allocated space for it
							// during the Cam::PostProcessor().postProcess() method so we're probably the only ones who
							// know it exists.

							toolpath->release();
							machine_program->release();
							Base::Console().Error("Machine Program from feature '%s' parsed OK\n", FeatName);
							return(true);
						}
						else
						{
							qDebug("GCode failed to parse\n");
						}
					}

					// And release both the toolpath and the machine_program objects now.  The toolpath probably
					// won't go away at this point but it's important that we let it know that we're finished with
					// it.  The machine_program, on the other hand, probably will go away as we allocated space for it
					// during the Cam::PostProcessor().postProcess() method so we're probably the only ones who
					// know it exists.

					toolpath->release();
					machine_program->release();
				}
			}
		}
	}
	Base::Console().Error("TPG run error: Feature '%s' does not appear to be a TPGFeature!\n", FeatName);
	return false;
}


/**
 * Add a TPG to the TPG Runner Queue.  First call will start the tpgRunnerThread.
 */
bool CamManagerInst::queueTPGRun(TPG* tpg, TPGSettings* settings, Cam::TPGFeature *tpgFeature) {

	tpgRunnerQueueMutex.lock(); // exploit this lock to make starting runner thread 'thread-safe'.
	// start the thread if needed
	if (tpgRunnerThread == NULL) {
		tpgRunnerThread = new boost::thread(boost::bind(&CamManagerInst::tpgRunnerThreadMain, this));
	}

	// add the tpg to the processing queue
	if (tpg != NULL && settings != NULL) {
		TPGRunnerItem* tpgRun = new TPGRunnerItem(tpg, settings->clone(), tpgFeature);
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
			updatedTPGState(tpgRun->tpg->getName(), Cam::TPG::RUNNING, 0);

			Base::Console().Message("Running TPG: %s\n", tpgRun->tpg->getName().toAscii().constData());
			tpgRun->tpg->run(tpgRun->settings, QString::fromAscii("default"));
			Base::Console().Message("Finished TPG: %s\n", tpgRun->tpg->getName().toAscii().constData());

			updatedTPGState(tpgRun->tpg->getName(), Cam::TPG::FINISHED, 100);

			

			// The tpgRun object was created by us and holds just enough information to allow the
			// toolpath to be generated.  We must release this memory here.
			delete tpgRun;
			tpgRun = NULL;
			// TODO Signal the main instance that we've completed the toolpath generation. Only when
			// this is done can we post process them into actual GCode.
		}
		else {
			#ifdef WIN32
				Sleep(1 * 1000);
			#else
				sleep(1); //TODO: make this a bit smarter so it can be interupted to stop quickly.
			#endif
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
