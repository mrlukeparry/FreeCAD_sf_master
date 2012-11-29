/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
 *   Copyright (c) 2012 Andrew Robinson  (andrewjrobinson@gmail.com)       *
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

#include <App/Application.h>
#include <Base/Console.h>
#include <Base/Sequencer.h>

#include <Python.h>

#include <QList>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include "TPG.h"
#include "TPGPython.h"
#include "TPGFactory.h"
#include "PyTPGFactory.h"
#include "CppTPGFactory.h"

using namespace Cam;


TPGFactoryInst* TPGFactoryInst::_pcSingleton = NULL;

TPGFactoryInst& TPGFactoryInst::instance(void)
{
    if (_pcSingleton == NULL) {
        _pcSingleton = new TPGFactoryInst;
    }

    return *_pcSingleton;
}

TPGFactoryInst::TPGFactoryInst(void)
{
    d = new TPGFactoryInstP;
}

TPGFactoryInst::~TPGFactoryInst(void)
{
    delete d;
}

void TPGFactoryInst::destruct (void)
{
    if (_pcSingleton != 0)
        delete _pcSingleton;
    _pcSingleton = 0;
}

void TPGFactoryInst::clearDescriptors()
{
    // Iterator through the TPGDescriptor List and delete all the entries
    for(std::vector<TPGDescriptor *>::iterator it = d->tpgList.begin(); it != d->tpgList.end(); ++it) {
        delete (*it);
        (*it) = 0;
    }     
    d->tpgList.empty();
}

TPG * TPGFactoryInst::getPlugin(QString id)
{
    for(std::vector<TPGDescriptor *>::iterator it = d->tpgList.begin(); it != d->tpgList.end(); ++it) {
        if ((*it)->id == id)
            return (*it)->make();
    }

    return 0;
}

/**
 * Scans for TPG's and updates internal cache of known tpgs
 *
 * @see getDescriptors to get a copy of these.
 */
void TPGFactoryInst::scanPlugins(short tpgtype /*= ALL_TPG*/) {

    // scan for plugins
    if (tpgtype & PYTHON_TPG > 0)
        Cam::PyTPGFactory().scanPlugins();
    if (tpgtype & CPP_TPG > 0)
        Cam::CppTPGFactory().scanPlugins();

    // clear my cache (if any)
    clearDescriptors();

    // update cache
    std::vector<TPGDescriptor*>* t = Cam::PyTPGFactory().getDescriptors();
    for (std::vector<TPGDescriptor*>::iterator it = t->begin(); it != t->end(); ++it)
        d->tpgList.push_back(*it);
    delete t;
    t = Cam::CppTPGFactory().getDescriptors();
    for (std::vector<TPGDescriptor*>::iterator it = t->begin(); it != t->end(); ++it)
        d->tpgList.push_back(*it);
    delete t;
}

/**
 * Get a vector of all C++ TPG's that are known about
 */
std::vector<TPGDescriptor*>* TPGFactoryInst::getDescriptors()
{
    // cache a copy of the descriptors
    if (d->tpgList.size() == 0)
        this->scanPlugins();

    // copy the tpg list cache
    std::vector<TPGDescriptor*> *result = new std::vector<TPGDescriptor*>();
    for (std::vector<TPGDescriptor*>::iterator it = d->tpgList.begin(); it != d->tpgList.end(); ++it)
        result->push_back(*it);

    return result;
}