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

#include <PreCompiled.h>
#ifndef _PreComp_
#endif

#include "PostProcessor.h"
#include <Base/Interpreter.h>	// for the Python runtime

namespace Cam {

PostProcessorInst* PostProcessorInst::_pcSingleton = NULL;

PostProcessorInst& PostProcessorInst::instance(void)
{
    if (_pcSingleton == NULL) {
        _pcSingleton = new PostProcessorInst;
    }

    return *_pcSingleton;
}

PostProcessorInst::PostProcessorInst() {

}

PostProcessorInst::~PostProcessorInst() {
}

/**
	This method accepts a ToolPath object (i.e. a Python program) and executes
	it as such.  The output from this Python program is expected to be GCode
	which is returned within the MachineProgram object.
 */
MachineProgram *PostProcessorInst::postProcess(ToolPath *toolpath, Item *postprocessor)
{
	MachineProgram *machine_program = new MachineProgram;

	QStringList *lines = toolpath->getToolPath();
	if (lines != NULL)
	{
		for (QStringList::size_type i=0; i<lines->size(); i++)
		{
			const char *line = lines->at(i).toAscii().constData();
			int result = Base::Interpreter().runCommandLine( lines->at(i).toAscii().constData() );
		} // End for
	} // End if - then

	return(machine_program);
}


} /* namespace Cam */
