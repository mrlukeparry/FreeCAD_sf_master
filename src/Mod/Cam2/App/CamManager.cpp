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

#include "PreCompiled.h"
#ifndef _PreComp_
#include <Python.h>
#endif

#include "CamManager.h"

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

}

CamManagerInst::~CamManagerInst() {
}

/**
 * Updates the progress of TPG processing
 */
void CamManagerInst::updateProgress(QString tpgid, TPG::State state, int progress) {

	Q_EMIT updatedTPGState(tpgid, state, progress);
}

} /* namespace Cam */

#include <Mod/Cam2/App/moc_CamManager.cpp>

