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

#ifndef CAMMANAGER_H_
#define CAMMANAGER_H_

#include <qobject.h>
#include "TPG/TPG.h"

namespace Cam {

/**
 * The CamManager is the master of all knowledge in the Cam Workbench.
 *
 * It provides a single point of communication with all aspects of the
 * workbench.  It provides signals and slots so that the UI (though the
 * UIManager) can interact with the data.
 */
class CamExport CamManagerInst : public QObject {

	Q_OBJECT

protected:
	static CamManagerInst* _pcSingleton;

public:

	CamManagerInst();
	virtual ~CamManagerInst();

	// singleton manipators
	static CamManagerInst& instance(void);
	static void destruct (void);

	// CLI Internal API (i.e. Non-GUI)
	/**
	 * Updates the progress of TPG processing
	 */
	void updateProgress(QString tpgid, TPG::State state, int progress);

public Q_SLOTS:
//  void addTPG(Cam::TPGDescriptor *tpg);
//  void reloadTPGs();

Q_SIGNALS:
	/**
	 * Signalled when a TPG state changes (or progess changes).
	 *
	 * @param tpgid, QString, the id of the TPG that has changed
	 * @param state, TPG::State (enum), the new status
	 * @param progress, int, the total percentage progressed (0 <= progress <= 100)
	 */
	void updatedTPGState(QString tpgid, Cam::TPG::State state, int progress);
};

/// Get the global instance
inline CamManagerInst& CamManager(void)
{
    return CamManagerInst::instance();
}

} /* namespace Cam */
#endif /* CAMMANAGER_H_ */
