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

#ifndef MACHINEPROGRAM_H_
#define MACHINEPROGRAM_H_

#include <qstringlist.h>


namespace Cam {

class MachineProgram {
protected:
    QStringList *machineProgram;

    int refcnt;
    virtual ~MachineProgram();
public:
    MachineProgram();

    /**
     * Add a single Machine command to the Program
     */
    void addMachineCommand(QString mc);

    /**
     * Clear out the toolpath.
     */
    void clear();

    /**
     * Get the Machine Program as a list of strings
     */
    QStringList *getMachineProgram();

    /**
     * Increases reference count
     * Note: it returns a pointer to this for convenience.
     */
    MachineProgram *grab() {
        refcnt++;
        return this;
    }

    /**
     * Decreases reference count and deletes self if no other references
     */
    void release() {
        refcnt--;
        if (refcnt == 0)
            delete this;
    }
};

} /* namespace Cam */
#endif /* MACHINEPROGRAM_H_ */
