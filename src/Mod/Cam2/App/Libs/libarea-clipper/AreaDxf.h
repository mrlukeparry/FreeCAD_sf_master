// AreaDxf.h
// Copyright (c) 2011, Dan Heeks
// This program is released under the New BSD license. See the file COPYING for details.

#pragma once

#include <PreCompiled.h>

#include "dxf.h"

namespace area
{

class LibAreaExport CSketch;
class LibAreaExport CArea;
class LibAreaExport CCurve;

class LibAreaExport AreaDxfRead : public CDxfRead{
	void StartCurveIfNecessary(const double* s);

public:
	CArea* m_area;
	AreaDxfRead(CArea* area, const char* filepath);

	// AreaDxfRead's virtual functions
	void OnReadLine(const double* s, const double* e);
	void OnReadArc(const double* s, const double* e, const double* c, bool dir);
};

} // End namespace area

