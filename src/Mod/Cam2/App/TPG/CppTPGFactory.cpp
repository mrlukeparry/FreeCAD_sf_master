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

#include "../PreCompiled.h"
#ifndef _PreComp_
#endif

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

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
}

/**
 * Searches the C++ TPG Plugin directory for Shared objects that implement the required API.
 * TODO: make this clear out libraries that aren't used and not reload libs that are in use
 * TODO: ultimately if the TPG lib file has changed this would delete all instances of the old TPG and recreate them from the new library.
 */
void CppTPGFactoryInst::scanPlugins() {
    std::string path = App::GetApplication().Config().at("AppHomePath");
    path.append("Mod/Cam/CppTPG/");
    const char *plugindir = path.c_str();
    unsigned int pathlen = path.size();
    char *pluginname = new char[pathlen+257];
    strcpy(pluginname, plugindir);
    printf("Scanning: %s\n", plugindir); //TODO: delete this once the workbench is more stable

    // open directory
    DIR *dir = opendir(plugindir);
    if (dir) {
        struct dirent *ent;
        struct stat info;

        // scan the directory
        while ((ent = readdir(dir)) != NULL) {
            strncpy(&pluginname[pathlen], ent->d_name, 256);
            if (stat(pluginname, &info) == 0) {

                // for files that are executable
                if (S_ISREG(info.st_mode) || S_ISLNK(info.st_mode)) {
                    if ((info.st_mode & S_IEXEC) != 0) {
                        int len = strlen(ent->d_name);

                        // and end with '.so'
                        if (len > 3 && ent->d_name[len - 3] == '.'
                                && ent->d_name[len - 2] == 's'
                                && ent->d_name[len - 1] == 'o') {

                            // make library pointer
                            QString lib = QString::fromAscii(pluginname);
                            plugins.push_back(new CppTPGPlugin(lib));
                            printf("CppPlugin: %s\n", lib.toAscii().constData()); //TODO: delete this once the workbench is more stable
                        }
                    }
                }
            }
        }
        closedir(dir);
        printf("Found %i CppPlugins\n", plugins.size()); //TODO: delete this once the workbench is more stable
    }
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
                descriptors->absorb(*tpgcollection);
                tpgcollection->release();
            }
        }
    }

    printf("Found %i CppTPGs\n", descriptors->size()); //TODO: delete this once the workbench is more stable

    // copy the tpg collection cache
    return descriptors->clone();
}

} /* namespace CamGui */
