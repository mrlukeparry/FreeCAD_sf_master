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

#include <PreCompiled.h>

#include <QStringList>
#include <QString>

#include <Area.h>

namespace Cam {
class ToolPath;
}

#include "TPG.h"

namespace Cam {

/**
 * Stores the Tool Path output from a single TPG.
 */
class CamExport ToolPath {

protected:
    TPG *source;
    QStringList *toolpath;
	QString line_buffer;		// an accumulation of multiple operator<< calls for a single line until we find a newline character at the end.

    int refcnt;
    virtual ~ToolPath();

public:
    ToolPath(TPG* source);

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

    /**
     * Increases reference count
     * Note: it returns a pointer to this for convenience.
     */
    ToolPath *grab() {
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

public:
	QString PythonString( const QString value ) const;
	QString PythonString( const char * value ) const;
	QString PythonString( const double value ) const;

	// The operator<< methods all allow the accumulation of parts of a single line
	// of Python script.  Each one will look to see if the accumulated line ends
	// with a newline character ('\n').  If it does then the line will be
	// added to the list of lines (this->toolpath) and the buffer (this->line_buffer)
	// will be cleared ready to start accumulating the next line.  They're just
	// utility methods to help make the construction of the Python program
	// a little easier to read.

	ToolPath & operator<< ( const ToolPath & value );
	ToolPath & operator<< ( const double value );
	ToolPath & operator<< ( const float value );
	ToolPath & operator<< ( const QString value );
	ToolPath & operator<< ( const char *value );
	ToolPath & operator<< ( const int value );

	const unsigned int RequiredDecimalPlaces() const;

private:
	double Round(double number,int place) const;
	double ToolPath::round(double r) const;
	unsigned int Precision( const double value ) const;

public:
	friend QString CamExport operator<< ( QString & buf, const ToolPath & tool_path );

};

} /* namespace Cam */
#endif /* TOOLPATH_H_ */
