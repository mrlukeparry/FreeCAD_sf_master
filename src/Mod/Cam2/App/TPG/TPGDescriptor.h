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

#ifndef TPGDESCRIPTOR2_H_
#define TPGDESCRIPTOR2_H_

#include <qstring.h>

#include "TPG.h"

namespace Cam {

/**
 * A superclass for TPG Descriptors.  These describe the basic information
 * about a TPG and contain a method to create a new instance of this TPG.
 */
class TPGDescriptor {

protected:
    int refcnt;

    virtual ~TPGDescriptor() {
        printf("Deleted TPGDescriptor: %p\n", this);
    }

public:
    QString id;
    QString name;
    QString description;

    QString type; // subclasses should set this to their classname in the constructor

    TPGDescriptor(QString id, QString name, QString description, QString type) {
        this->id = id;
        this->name = name;
        this->description = description;
        this->type = type;
        refcnt = 1;
        printf("New      TPGDescriptor: %p (%s)\n", this, type.toAscii().constData());
    }
    /// Convenience method for using plain ascii string
    TPGDescriptor(const char *id, const char * name, const char * description,
            const char * type) {
        this->id = QString::fromAscii(id);
        this->name = QString::fromAscii(name);
        this->description = QString::fromAscii(description);
        this->type = QString::fromAscii(type);
        refcnt = 1;
        printf("New      TPGDescriptor: %p (%s)\n", this, type);
    }
    TPGDescriptor(const TPGDescriptor &copy) {
        this->id = copy.id;
        this->name = copy.name;
        this->description = copy.description;
        this->type = copy.type;
        refcnt = 1;
        printf("New      TPGDescriptor: %p (%s)\n", this, type.toAscii().constData());
    }
    TPGDescriptor(const TPGDescriptor *copy) {
        this->id = copy->id;
        this->name = copy->name;
        this->description = copy->description;
        this->type = copy->type;
        refcnt = 1;
        printf("New      TPGDescriptor: %p (%s)\n", this, type.toAscii().constData());
    }

    /**
     * Increases reference count
     */
    TPGDescriptor *grab() {
        refcnt++;
        printf("Grab     TPGDescriptor: %p (%i)\n", this, refcnt);
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

    /**
     * Creates a new instance of this TPG.  Sub-classes need to implement this
     */
    virtual Cam::TPG* make() = 0;
    virtual void print() {
        printf("- ('%s', '%s', '%s', '%s')\n", id.toAscii().constData(),
                name.toAscii().constData(), description.toAscii().constData(),
                type.toAscii().constData());
    }
};

} /* namespace Cam */
#endif /* TPGDESCRIPTOR2_H_ */
