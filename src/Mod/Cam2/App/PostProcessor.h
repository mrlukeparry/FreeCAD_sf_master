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

#ifndef POSTPROCESSOR_H_
#define POSTPROCESSOR_H_

#include <cstdlib>

#include "TPG/ToolPath.h"
#include "ItemList.h"
#include "MachineProgram.h"

namespace Cam {

class PostProcessorInst {

protected:
    static PostProcessorInst* _pcSingleton;

    PostProcessorInst();
    virtual ~PostProcessorInst();

public:
    static PostProcessorInst& instance(void);
    static void destruct (void);


    Cam::ItemList *getPostProcessorList(bool rescan);
    Cam::MachineProgram *postProcess(Cam::ToolPath *toolpath, Cam::Item *postprocessor);

};

/// Get the global instance
inline PostProcessorInst& PostProcessor(void)
{
    return PostProcessorInst::instance();
}

} /* namespace Cam */
#endif /* POSTPROCESSOR_H_ */
