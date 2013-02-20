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

#include "TPGDescriptorCollection.h"

namespace Cam {

TPGDescriptorCollection::TPGDescriptorCollection() {
    refcnt = 1;
    printf("New      TPGDescriptorCollection: %p\n", this);
}

TPGDescriptorCollection::~TPGDescriptorCollection() {
    printf("Deleting TPGDescriptorCollection: %p\n", this);
    //TODO: make this thread-safe
    for (size_t i = 0; i < descriptors.size(); i++)
        descriptors.at(i)->release();
    descriptors.empty();
    printf("Deleted  TPGDescriptorCollection: %p\n", this);
}

/**
 * Increases reference count.
 * Note: it returns a pointer to this for convenience.
 */
TPGDescriptorCollection *TPGDescriptorCollection::grab() {
    refcnt++;
    return this;
}

/**
 * Decreases reference count and deletes self if no other references
 */
void TPGDescriptorCollection::release() {
    refcnt--;
    if (refcnt == 0)
        delete this;
}

/**
 * Adds a TPG to the collection
 */
void TPGDescriptorCollection::add(TPGDescriptor* descriptor) {
    printf("TPGDescriptorCollection (%p): Adding %p\n", this, descriptor);
    descriptors.push_back(descriptor->grab());
}

/**
 * Absorbs the TPGs from the given collection.
 *
 * The other collection will be emptied but not released.
 */
void TPGDescriptorCollection::absorb(TPGDescriptorCollection *other) {
    //TODO: make this thread-safe
    size_t cnt = other->descriptors.size();
    for (size_t i = 0; i < cnt; i++) {
        descriptors.push_back(other->descriptors.at(i));
    }
//    printf("TPGDescriptorCollection::absorb: %p contains:\n", this);
//    for (size_t i = 0; i < descriptors.size(); i++)
//        printf("- %p\n", descriptors.at(i));
//    printf("TPGDescriptorCollection::absorb: %p other contained:\n", this);
//    for (size_t i = 0; i < other->descriptors.size(); i++)
//        printf("- %p\n", other->descriptors.at(i));
    other->descriptors.empty();
}

/**
 * Removes a TPG from the collection
 */
//void TPGDescriptorCollection::del(TPG* tpg);

/**
 * Get the number of items in this collection
 */
size_t TPGDescriptorCollection::size() {
    return descriptors.size();
}

/**
 * Get the TPG at the given position
 */
TPGDescriptor* TPGDescriptorCollection::at(size_t pos) {
    if (pos >= 0 && pos < descriptors.size())
        return descriptors.at(pos);
    return NULL;
}

/**
 * Makes a duplicate copy of this collection.  It will create a new object
 * and grab a new reference to each TPGDescriptor.
 *
 * Note: The returned reference is owned by the caller
 */
TPGDescriptorCollection* TPGDescriptorCollection::clone() {
    TPGDescriptorCollection* copy = new TPGDescriptorCollection();

    for (size_t i = 0; i < descriptors.size(); i++) {
        printf("TPGDescriptorCollection::clone(): 1\n");
        TPGDescriptor *desc = descriptors.at(i);
        printf("TPGDescriptorCollection::clone(): 2\n");
        copy->descriptors.push_back(desc->grab());
        printf("TPGDescriptorCollection::clone(): 3\n");
    }

    return copy;
}

/**
 * Calls a function for each element of in collection and assigns the element to is returned value
 */
void TPGDescriptorCollection::onEach(void *func(TPGDescriptor*, void*), void *param) {
    for (size_t i = 0; i < descriptors.size(); i++)
        descriptors[i] = (TPGDescriptor*) func(descriptors.at(i), param);
}

} /* namespace Cam */
