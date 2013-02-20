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

#ifndef TOOLPATH_H_
#define TOOLPATH_H_

#include <qstringlist.h>
#include <qstring.h>

namespace Cam {
class ToolPath;
}

#include "TPG.h"

namespace Cam {

/**
 * Stores the Tool Path output from a single TPG.
 */
class ToolPath {

protected:
    TPG *source;
    QStringList *toolpath;

public:
    ToolPath(TPG* source);
    virtual ~ToolPath();

    /**
     * Add a single toolpath command to the ToolPath
     */
    void addToolPath(QString tp);

    /**
     * Clear out the toolpath.
     */
    void clear();

    /**
     * Get the TPG that created this toolpath
     */
    TPG *getSource();

    /**
     * Get the Toolpath as strings
     */
    QStringList *getToolPath();
};

} /* namespace Cam */
#endif /* TOOLPATH_H_ */
