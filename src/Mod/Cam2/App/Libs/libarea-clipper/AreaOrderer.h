// AreaOrderer.h
// Copyright (c) 2010, Dan Heeks
// This program is released under the New BSD license. See the file COPYING for details.

#pragma once

#include <PreCompiled.h>

#include <list>
#include <set>

namespace area
{

class LibAreaExport CArea;
class LibAreaExport CCurve;

class LibAreaExport CAreaOrderer;

class LibAreaExport CInnerCurves
{
	CInnerCurves* m_pOuter;
	const area::CCurve* m_curve; // always empty if top level
	std::set<CInnerCurves*> m_inner_curves;
	CArea *m_unite_area; // new curves made by uniting are stored here

public:
	static CAreaOrderer* area_orderer;
	CInnerCurves(CInnerCurves* pOuter, const area::CCurve* curve);
	~CInnerCurves();

	void Insert(const area::CCurve* pcurve);
	void GetArea(CArea &area, bool outside = true, bool use_curve = true)const;
	void Unite(const CInnerCurves* c);
};

class LibAreaExport CAreaOrderer
{
public:
	CInnerCurves* m_top_level;

	CAreaOrderer();

	void Insert(area::CCurve* pcurve);
	CArea ResultArea()const;
};

} // End namespace area

