/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
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

using namespace Cam;


//TYPESYSTEM_SOURCE_ABSTRACT(Cam::TPGFactoryInst, Base::Factory)

TPGFactoryInst* TPGFactoryInst::_pcSingleton = NULL;

TPGFactoryInst& TPGFactoryInst::instance(void)
{
    if (_pcSingleton == NULL) {
        _pcSingleton = new TPGFactoryInst;
    }

    return *_pcSingleton;
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

//    std::map<const std::string, Base::AbstractProducer*>::const_iterator it;
//    it = _mpcProducers.find(id.toStdString().c_str());
//    if (it != _mpcProducers.end()) {
//        //it->second->setValue(v);
//        return static_cast<TPG *>(it->second->Produce());
//    }


    // Try loading python modules
    return 0;
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
