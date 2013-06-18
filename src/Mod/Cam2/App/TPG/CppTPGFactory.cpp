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

#include <PreCompiled.h>
#ifndef _PreComp_
#endif

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <QDir>

#include <App/Application.h>

#include "CppTPGFactory.h"

namespace Cam {
CppTPGFactoryInst* CppTPGFactoryInst::_pcSingleton = NULL;

CppTPGFactoryInst& CppTPGFactoryInst::instance(void)
{
    if (_pcSingleton == NULL)
        _pcSingleton = new CppTPGFactoryInst();

    return *_pcSingleton;
}

CppTPGFactoryInst::CppTPGFactoryInst() {
    descriptors = new TPGDescriptorCollection();
}

CppTPGFactoryInst::~CppTPGFactoryInst() {
	for (Plugins_t::iterator itPlugin = plugins.begin(); itPlugin != plugins.end(); itPlugin++)
	{
		(*itPlugin)->release();
	}
	plugins.clear();
}

/**
 * Searches the C++ TPG Plugin directory for Shared objects that implement the required API.
 * TODO: make this clear out libraries that aren't used and not reload libs that are in use
 * TODO: ultimately if the TPG lib file has changed this would delete all instances of the old TPG and recreate them from the new library.
 */
void CppTPGFactoryInst::scanPlugins() 
{
	QString path = QString::fromAscii(App::GetApplication().Config()["AppHomePath"].c_str());
	path.append(QString::fromAscii("Mod/Cam/CppTPG/"));

	/*
	// NOTE: We cannot delete the plugins here because pointers to them are still held by
	// parent objects (CppTPGDescriptorWrapper).  As an alternative, we're going to check each file by name and only
	// load those that we have not previously seen.

    // cleanout old plugins
    // printf("Releasing old Plugins:\n");
    for (size_t i = 0; i < plugins.size(); i++) {
        CppTPGPlugin* plugin = plugins.at(i);
        // printf(" - %p, %i\n", plugin, plugin->refcnt);
        plugin->release();
    }
    plugins.clear();
	*/

	qDebug("Scanning: %s\n", path.toAscii().constData());

	QDir dir(path);
	dir.setFilter(QDir::Files);
	QFileInfoList file_info_list = dir.entryInfoList();
	for (QFileInfoList::ConstIterator itFileInfo = file_info_list.begin(); itFileInfo != file_info_list.end(); itFileInfo++)
	{
		if (QLibrary::isLibrary(itFileInfo->fileName()))
		{
			// Look to see if we already have this plugin loaded.  If so, skip it.
			bool found = false;
			for (Plugins_t::const_iterator itPlugin = plugins.begin(); (! found) && (itPlugin != plugins.end()); itPlugin++)
			{
				if ((*itPlugin)->FileName() == itFileInfo->fileName()) found = true;
			}

			if (! found)
			{
				 // make library pointer
				QString lib = itFileInfo->absoluteFilePath();
				plugins.push_back(new CppTPGPlugin(lib));
				qDebug("CppPlugin: %s\n", lib.toAscii().constData()); //TODO: delete this once the workbench is more stable
			}
			else
			{
				qDebug("CppPlugin::scanPlugins() Skipping over %s as we already have it loaded\n", itFileInfo->fileName().toAscii().constData());
			}
		} // End if - then
	} // End for
}

/**
 * Get a vector of all C++ TPG's that are known about
 */
TPGDescriptorCollection* CppTPGFactoryInst::getDescriptors()
{
    // cache a copy of the descriptors
    if (descriptors->size() == 0) { //TODO: allow this to be done after each reload
        for (std::vector<CppTPGPlugin*>::iterator it = plugins.begin(); it != plugins.end(); ++it) {
            Cam::TPGDescriptorCollection* tpgcollection = (*it)->getDescriptors();
            if (tpgcollection != NULL) {
                descriptors->absorb(tpgcollection);
                tpgcollection->release();
            }
        }
    }

    qDebug("Found %i CppTPGs\n", int(descriptors->size())); //TODO: delete this once the workbench is more stable

    // copy the tpg collection cache
    return descriptors->clone();
}

} /* namespace CamGui */
