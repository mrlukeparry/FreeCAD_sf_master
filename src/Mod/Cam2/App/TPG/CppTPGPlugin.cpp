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
    getTPGPtr = NULL;
    descriptors = NULL;
}

CppTPGPlugin::~CppTPGPlugin() {
    if (descriptors != NULL)
        descriptors->release();
    if (library != NULL)
        close();
}

/**
 * A callback function to wrap each element in a wrapper.
 */
void* wrapfunc(TPGDescriptor* tpg, void* plugin) {
    return new CppTPGDescriptorWrapper(tpg, (CppTPGPlugin*)plugin);
}

/**
 * Returns a list of TPG's that this plugin provides
 * Note: the list is owned by the caller and must be released when finished
 * with it (including the referenced TPGDescriptors).
 */
Cam::TPGDescriptorCollection* CppTPGPlugin::getDescriptors() {
    if (isOpen()) {
        if (descriptors == NULL) {
            descriptors = getDescriptorsPtr();
            printf("%p\n", descriptors);
            descriptors->print();
        }
        if (descriptors != NULL) {
            // wrap the descriptors
            Cam::TPGDescriptorCollection* result = descriptors->clone();
            result->onEach(wrapfunc, (void*)this);
            return result;
        }
    }
    printf("Warning: NULL descriptors: %s\nError: %s\n", this->filename.toAscii().constData(), this->error.toAscii().constData());
    return NULL;
}

/**
 * Gets an instance of the TPG.
 * Note: the TPG returned is a wrapper around the implementation.  Release
 * it once you are finished with it and it will automatically delete the
 * implementation once all references are released
 */
TPG* CppTPGPlugin::getTPG(QString id) {
    if (isOpen()) {
        return getTPGPtr(id);
    }
    printf("Warning: NULL descriptors: %s\nError: %s\n", this->filename.toAscii().constData(), this->error.toAscii().constData());
    return NULL;
}

/**
 * Increases reference count
 */
CppTPGPlugin* CppTPGPlugin::grab() {
    refcnt++;
    return this;
}

/**
 * Decreases reference count and deletes self if no other references
 */
void CppTPGPlugin::release() {
    refcnt--;
    if (refcnt <= 0)
        delete this;
}

/**
 * Makes sure library is open, attempts to open it if it isn't.
 */
bool CppTPGPlugin::isOpen() {
    if (library == NULL) {
        library = dlopen(filename.toAscii(), RTLD_NOW);
        if (!library) {
            error = QString::fromAscii(dlerror());
            return false;
        }

        // open symbols as well
        dlerror();// reset errors
        getDescriptorsPtr = (getDescriptors_t*) dlsym(library, "getDescriptors");
        const char* dlsym_error = dlerror();
        if (dlsym_error != NULL) {error = QString::fromAscii(dlsym_error); close(); return false;}
        getTPGPtr = (getTPG_t*) dlsym(library, "getTPG");
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
//        // clear caches
//        if (descriptors != NULL)
//            delDescriptorsPtr(descriptors);

        dlclose(library);

        // cleanup
        library = NULL;
        getDescriptorsPtr = NULL;
//        delDescriptorsPtr = NULL;
        getTPGPtr = NULL;
//        delTPGPtr = NULL;
    }
}

} /* namespace CamGui */
