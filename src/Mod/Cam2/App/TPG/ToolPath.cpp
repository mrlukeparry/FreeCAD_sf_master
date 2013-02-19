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
#include <cstring>
#endif

#include "ToolPath.h"

namespace Cam {

ToolPath::ToolPath(TPG* source) {
    this->source = source;
    this->toolpath = new QStringList();
}

ToolPath::~ToolPath() {
    if (this->toolpath != NULL)
        delete this->toolpath;
}

/**
 * Add a single toolpath command to the ToolPath
 */
void ToolPath::addToolPath(QString tp) {
    if (this->toolpath == NULL)
        this->toolpath = new QStringList();
    this->toolpath->push_back(tp);
}

/**
 * Clear out the toolpath.
 */
void ToolPath::clear() {
    if (this->toolpath != NULL)
        this->toolpath->clear();
}

/**
 * Get the TPG that created this toolpath
 */
TPG *ToolPath::getSource() {
    return this->source;
}

/**
 * Get the Toolpath as strings
 */
QStringList *ToolPath::getToolPath() {
    return this->toolpath;
}

} /* namespace Cam */
