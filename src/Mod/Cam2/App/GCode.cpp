/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
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

#include "GCode.h"
using namespace Cam;
GCode::GCode()
{
}

GCode::~GCode()
{
}

std::map<std::string, double> the_dictionary;
void insert(std::pair<std::string, double>& p)
{
	the_dictionary[p.first] = p.second;
}
 
namespace qi = boost::spirit::qi;


int wilma()
{
	std::map<std::string, double> arguments;

	typedef std::string::iterator iterator;
	qi::rule<iterator, std::string()> identifier = qi::char_("xyzabcuvwXYZABCUVW");
	qi::rule<iterator, double()> value = qi::double_;

	qi::rule<iterator, std::pair<std::string, double >()> assignment;
	qi::rule<iterator, std::map<std::string, double >()> assignments;
	assignment = identifier >> '=' >> value >> ';';
	assignments = +(identifier >> '=' >> value >> ';');
	 
	std::string input("x=1.1;y=2.2;");
	// qi::parse(input.begin(), input.end(), assignment[&insert]);
	qi::parse(input.begin(), input.end(), assignments, arguments);

	return(0);
}