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

#ifndef CAM_GCODE_H
#define CAM_GCODE_H

#include <App/PropertyStandard.h>
#include <PreCompiled.h>

#include <Mod/Cam2/App/MachineProgram.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>


namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;


///////////////////////////////////////////////////////////////////////////////
int CamExport wilma();




/// Base class
namespace Cam
{

class CamExport GCode
{

public:
    GCode();
    ~GCode();

public:
	class Double { double _value; public: Double() : _value(0.0) {} Double(double value) { _value = value; } };

	
	typedef std::map<std::string, double> pairs_type;
	typedef std::pair<std::string, double> pair_type;
	 
	template <typename Iterator>
	struct Arguments
	  : qi::grammar<Iterator, pairs_type()>
	{
		Arguments()
		  : Arguments::base_type(pairs)
		{
			pairs =  pair >> pair | pair;
			pair  =  name >> value;
			name   =  qi::char_("xyzabcuvwXYZABCUVW");
			value = +qi::double_;
		}
	 
		qi::rule<Iterator, pairs_type()> pairs;
		qi::rule<Iterator, pair_type()> pair;
		qi::rule<Iterator, std::string()> name;
		qi::rule<Iterator, double()> value;
	};


	virtual int fred()
	{
		std::string kv("X1.234");

		std::string::iterator begin = kv.begin();
		std::string::iterator end = kv.end();

		// Arguments< std::string::iterator > p;
		pairs_type m;

		// qi::parse( begin, end, p, m );

		std::string number("1.2345");


		std::string input("1 2 3 4 5");
		std::string::const_iterator F(number.begin()), f(F), l(number.end());

		std::vector<double> floats;
		if (qi::phrase_parse(f = F, l, *qi::double_, qi::space, floats))
			std::cout << floats.size() << " floats parsed\n";

		std::vector<int> ints;
		if (qi::phrase_parse(f = F, l, *qi::int_, qi::space, ints))
			std::cout << ints.size() << " ints parsed\n";

		return(0);
	}




	
protected:
	/**
		Define boost::spirit::classic rules for each of the various 'tokens' that make up
		an RS274 GCode program.  Make them virtual so that this class can be overridden
		to make specific GCode parsers for different machine controllers.
	 */
	/*
	virtual rule<> line_number() { return ((chlit<>('N') | chlit<>('n')) >> uint_parser<>()); }

	virtual rule<> g00() { return ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('0'))); }
	virtual rule<> g01() { return ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('1'))); }
	virtual rule<> g02() { return ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('2'))); }

	// Variable can be #9999
	virtual rule<> variable() { return ((chlit<>('#') >> uint_parser<>())); }

	virtual rule<> end_of_block() { return eol_p; }

	// The work_offset rule changes between machine controllers so separate it out in its own rule.
	virtual rule<> work_offset() { return ((chlit<>('G') | chlit<>('g')) >> str_p<>("10") >> (chlit<>('L') | chlit<>('l')) >> str_p<>("20")); }
	*/
};

/*
class CamExport LinuxCNC : public GCode
{
public:
	LinuxCNC();
	~LinuxCNC();

protected:
	// Overload the work_offset rule because LinuxCNC uses G10 L20 for that.
	virtual rule<> work_offset() { return ((chlit<>('G') | chlit<>('g')) >> str_p<>("10") >> (chlit<>('L') | chlit<>('l')) >> str_p<>("20")); }

	// Variables can be #9999 or #<alpha> in LinuxCNC
	virtual rule<> variable() { return (GCode::variable() | (str_p<>("#<") >> +(char_("a-zA-Z0-9_")) >> str_p<>(">")) ); }

}; // End LinuxCNC GCode parser class definition.

class CamExport Mach3 : public GCode
{
public:
	Mach3();
	~Mach3();

protected:
	// Overload the work_offset rule because LinuxCNC uses G10 L2 for that.
	virtual rule<> work_offset() { return ((chlit<>('G') | chlit<>('g')) >> str_p<>("10") >> (chlit<>('L') | chlit<>('l')) >> str_p<>('2')); }

}; // End LinuxCNC GCode parser class definition.
*/

} //namespace Cam
#endif //CAM_GCODE_H
