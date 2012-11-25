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

#include "CppTPGFactory.h"

namespace Cam {

CppTPGFactory::CppTPGFactory() {
}

CppTPGFactory::~CppTPGFactory() {
}



/**
 * Get a vector of all C++ TPG's that are known about
 */
std::vector<TPGDescriptor*>* CppTPGFactory::getDescriptors()
{
    // cache a copy of the descriptors
    if (tpgs.size() == 0) //TODO: allow this to be done after each reload
    {
        std::vector<CppTPGPlugin*>::iterator it = plugins.begin();
        for (; it != plugins.end(); ++it)
        {
            std::vector<TPGDescriptor*>*tpglist = (*it)->getDescriptors();
            if (tpglist != NULL) {
                std::vector<TPGDescriptor*>::iterator itt = tpglist->begin();
                for (;itt != tpglist->end(); ++itt)
                    tpgs.push_back(*itt);
                delete tpglist;
            }
        }
    }

    printf("Found %i CppTPG's\n", tpgs.size());

    // copy the tpg list cache
    std::vector<TPGDescriptor*> *result = new std::vector<TPGDescriptor*>();
    std::vector<TPGDescriptor*>::iterator itt = tpgs.begin();
    for (;itt != tpgs.end(); ++itt)
        result->push_back(*itt);

    return result;
}

/**
 * Searches the C++ TPG Plugin directory for Shared objects that implement the required API.
 * TODO: make this clear out libraries that aren't used and not reload libs that are in use
 * TODO: ultimately if the TPG lib file has changed this would delete all instances of the old TPG and recreate them from the new library.
 */
void CppTPGFactory::scanPlugins() {
    char *plugindir = "./Mod/Cam/CppTPG";

    // open directory
    DIR *dir = opendir(plugindir);
    if (dir) {
        struct dirent *ent;
        struct stat info;

        // scan the directory
        while ((ent = readdir(dir)) != NULL) {
            if (stat(plugindir, &info) == 0) {

                // for files that are executable
                if (S_ISREG(info.st_mode) || S_ISLNK(info.st_mode)) {
                    if ((info.st_mode & S_IEXEC) != 0) {
                        int len = strlen(ent->d_name);

                        // and end with '.so'
                        if (len > 3 && ent->d_name[len - 3] == '.'
                                && ent->d_name[len - 2] == 's'
                                && ent->d_name[len - 1] == 'o') {

                            // make library pointer
                            QString lib = QString::fromAscii(plugindir);
                            lib.append(QString::fromAscii(ent->d_name));
                            plugins.push_back(new CppTPGPlugin(lib));
                            printf("CppPlugin: %s\n", lib.toAscii().constData());
                        }
                    }
                }
            }
        }
        closedir(dir);
        printf("Found %i CppPlugins\n", plugins.size());
    }
}

} /* namespace CamGui */
