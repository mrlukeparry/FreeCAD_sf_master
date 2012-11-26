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

#ifndef CPPTPG_H_
#define CPPTPG_H_

#include <vector>

/// Macros to make it simpler to define a new CppTPG ///
/**
 * TPG Plugins that implement a single plugin should add this macro to its
 * source file
 */
#define CPPTPG_API_SOURCE(_type_, _id_, _name_, _desc_)\
extern "C" std::vector<Cam::TPGDescriptor*>* getDescriptors() {\
    std::vector<Cam::TPGDescriptor*>* descriptors = new std::vector<Cam::TPGDescriptor*>();\
    descriptors->push_back(new Cam::CppTPGDescriptor(QString::fromAscii(_id_),\
            QString::fromAscii(_name_),\
            QString::fromAscii(_desc_)));\
    return descriptors;\
}\
extern "C" void delDescriptors(std::vector<Cam::TPGDescriptor*>* descriptors) {\
    std::vector<Cam::TPGDescriptor*>::iterator itt = descriptors->begin();\
    for (;itt != descriptors->end(); ++itt)\
        delete *itt;\
    descriptors->clear();\
    delete descriptors;\
}\
extern "C" Cam::TPG* getTPG(QString id) {\
    if (id == QString::fromAscii(_id_))\
        return new Cam::CppExampleTPG();\
    return NULL;\
}\
extern "C" void delTPG(Cam::TPG* tpg) {\
    if (tpg != NULL)\
        delete tpg;\
}

/**
 * TPG Plugins that implement a single plugin should add this macro to its
 * header file
 */
#define CPPTPG_API_HEADER()\
extern "C" std::vector<Cam::TPGDescriptor*>* getDescriptors();\
extern "C" void delDescriptors(std::vector<Cam::TPGDescriptor*>*);\
extern "C" Cam::TPG* getTPG(QString);\
extern "C" void delTPG(Cam::TPG*);

namespace Cam {

/**
 * This file contains support types for the functions that the TPG Library
 * files export.
 */

// Descriptor create/delete function types
typedef std::vector<TPGDescriptor*>* getDescriptors_t();
typedef void delDescriptors_t(std::vector<TPGDescriptor*>*);

// TPG create/delete function types
typedef TPG* getTPG_t(QString);
typedef void delTPG_t(TPG*);


}



#endif /* CPPTPG_H_ */
