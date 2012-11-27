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

#include "CppTPGPlugin.h"
#include "CppTPGDescriptorWrapper.h"

#include <dlfcn.h>

namespace Cam {

CppTPGPlugin::CppTPGPlugin(QString filename) {
    this->filename = filename;
    library = NULL;
    getDescriptorsPtr = NULL;
    delDescriptorsPtr = NULL;
    getTPGPtr = NULL;
    delTPGPtr = NULL;
    descriptors == NULL;
}

CppTPGPlugin::~CppTPGPlugin() {
}

/**
 * Returns a list of TPG's that this plugin provides
 * Note: the list is owned by the caller and must be deleted when finished
 * with it (including the referenced TPGDescriptors).
 */
std::vector<TPGDescriptor*>* CppTPGPlugin::getDescriptors() {
    if (isOpen()) {
        if (descriptors == NULL)
            descriptors = getDescriptorsPtr();
        if (descriptors != NULL) {
            printf("Descriptors: %p\n", descriptors);
            // wrap the descriptors
            std::vector<TPGDescriptor*>* result = new std::vector<TPGDescriptor*>();
            for (std::vector<TPGDescriptor*>::iterator it = descriptors->begin(); it != descriptors->end(); ++it)
            {
                printf("Descriptor: %p\n", it);
                if (*it != NULL)
                    result->push_back(new CppTPGDescriptorWrapper(*it, this));
            }
            return result;
        }
    }
    printf("Warning: NULL descriptors: %s\nError: %s\n", this->filename.toAscii().constData(), this->error.toAscii().constData());
    return NULL;
}

/**
 * Gets an instance of the TPG.
 * Note: the TPG returned is a wrapper around the implementation.  Delete
 * it once you are finished with it and it will automatically delete the
 * implementation using the delTPG(tpg) method below
 */
TPG* CppTPGPlugin::getTPG(QString id) {
    if (isOpen()) {
        return getTPGPtr(id);
    }
    printf("Warning: NULL descriptors: %s\nError: %s\n", this->filename.toAscii().constData(), this->error.toAscii().constData());
    return NULL;
}

/**
 * Used by the CppTPGWrapper to delete its implementation.
 */
void CppTPGPlugin::delTPG(TPG* tpg) {
    if (isOpen()) {
        if (tpg)
            delTPGPtr(tpg);
    }
    printf("Warning: NULL descriptors: %s\nError: %s\n", this->filename.toAscii().constData(), this->error.toAscii().constData());
}

/**
 * Makes sure library is open, attempts to open it if it isn't.
 */
bool CppTPGPlugin::isOpen() {
    if (library == NULL) {
        library = dlopen(filename.toAscii(), RTLD_LAZY);
        if (!library) {
            error = QString::fromAscii(dlerror());
            return false;
        }

        // open symbols as well
        dlerror();// reset errors
        getDescriptorsPtr = (getDescriptors_t*) dlsym(library, "getDescriptors");
        const char* dlsym_error = dlerror();
        if (dlsym_error != NULL) {error = QString::fromAscii(dlsym_error); close(); return false;}
        delDescriptorsPtr = (delDescriptors_t*) dlsym(library, "delDescriptors");
        dlsym_error = dlerror();
        if (dlsym_error != NULL) {error = QString::fromAscii(dlsym_error); close(); return false;}
        getTPGPtr = (getTPG_t*) dlsym(library, "getTPG");
        dlsym_error = dlerror();
        if (dlsym_error != NULL) {error = QString::fromAscii(dlsym_error); close(); return false;}
        delTPGPtr = (delTPG_t*) dlsym(library, "delTPG");
        dlsym_error = dlerror();
        if (dlsym_error != NULL) {error = QString::fromAscii(dlsym_error); close(); return false;}
    }
    return true;
}

/**
 * Close the library and cleanup pointers
 */
void CppTPGPlugin::close() {
    if (library != NULL) {
        // clear caches
        if (descriptors != NULL)
            delDescriptorsPtr(descriptors);

        dlclose(library);

        // cleanup
        library = NULL;
        getDescriptorsPtr = NULL;
        delDescriptorsPtr = NULL;
        getTPGPtr = NULL;
        delTPGPtr = NULL;
    }
    printf("Closing: %s\n", this->filename.toAscii().constData());
}

} /* namespace CamGui */
