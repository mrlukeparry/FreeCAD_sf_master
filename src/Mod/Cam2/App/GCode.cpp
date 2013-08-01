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

#include <sstream>

// NOTE: These BOOST SPIRIT DEBUG macros MUST be defined BEFORE the include
// files.  Otherwise the default values, defined within the include files,
// will be used instead.

// Define an ostringstream to accumulate any debug output the grammar has
// to offer.  If we fail to parse the GCode, we will want to know why.
std::ostringstream GrammarDebugOutputBuffer;

#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_DEBUG_OUT GrammarDebugOutputBuffer
// #define BOOST_SPIRIT_DEBUG_PRINT_SOME 9999999	// We want as much debug output as possible.

/*
void BOOST_SPIRIT_DEBUG_TOKEN_PRINTER(std::ostringstream & o, std::string::value_type c)
{
	qDebug("%s", o.str().c_str());
	qDebug("%c", c);
}
*/


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "GCode.h"
using namespace Cam;
GCode::GCode()
{
}

GCode::~GCode()
{
}



void callback(char c, double f)
{
	int i = 3;
}

std::vector<double> doubles;
void add_argument(double const &value)
{
	doubles.push_back(value);
}


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

typedef std::string::iterator Iterator;
typedef std::pair<char, double> ArgumentData_t;
typedef double Double_t;
Double_t g_double;


template <typename Iterator>
struct point_double_grammar : boost::spirit::qi::grammar<Iterator, double()>
{
    point_double_grammar() : point_double_grammar::base_type(d)
    {
		d = boost::spirit::qi::double_ [ g_double = boost::spirit::qi::as<double>(boost::spirit::qi::_1) ];
    }
    boost::spirit::qi::rule<Iterator, double()> d;
};



namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace ascii = boost::spirit::ascii;

typedef qi::symbols<char, double> arguments_dictionary;
qi::rule<std::string::const_iterator, qi::space_type> GetRapid()
{
	return(qi::rule<std::string::const_iterator, qi::space_type>(qi::lexeme [+qi::char_("gG") >> *qi::char_("0") >> qi::char_("0")] >> (qi::lexeme [+qi::char_("xXyYzZ")] >> qi::double_)));
}


template <typename Iter, typename Skipper = qi::blank_type> 
	struct rs274 : qi::grammar<Iter, Skipper> 
{

	void Add(std::vector<char> c, double value)
	{
		if (c.size() == 1)
		{
			m_doubles.insert(std::make_pair(c[0], value));
		}
	}

	void Print()
	{
		for (DoubleMap_t::const_iterator itArg = m_doubles.begin(); itArg != m_doubles.end(); itArg++)
		{
			qDebug("%c=%lf\n", itArg->first, itArg->second);
		}
	}

	void SetLineNumber( const int value )
	{
		line_number = value;
	}

	void ProcessBlock()
	{
		qDebug("Processing block\n");
		int j=3;
		Print();
		m_doubles.clear();
	}

	typedef boost::proto::result_of::deep_copy<
				BOOST_TYPEOF(ascii::no_case[qi::lit(std::string())])
			>::type nocaselit_return_type;

	nocaselit_return_type nocaselit(const std::string& keyword)
	{
		return boost::proto::deep_copy(ascii::no_case[qi::lit(keyword)]);
	}

	rs274(arguments_dictionary &dict) : rs274::base_type(Start)
	{
		using phx::construct;
		using phx::val;

		using qi::lit;
        using qi::lexeme;
        using ascii::char_;
        using ascii::string;
        using namespace qi::labels;

        using phx::at_c;
        using phx::push_back;

		// Variables declared here are created and destroyed for each rule parsed.
		// Use member variables of the structure for long-lived variables instead.

		// N110 - i.e. use qi::lexeme to avoid qi::space_type skipper which allows the possibility of interpreting 'N 110'.  We don't want spaces here.
		// LineNumberRule = qi::lexeme[ qi::repeat(1,1)[qi::char_("nN")] >> qi::int_ ]
		LineNumberRule = (qi::repeat(1,1)[qi::char_("nN")] >> qi::int_)
			// [ phx::bind(&rs274<Iter, Skipper>::SetLineNumber, phx::ref(*this), qi::_2) ] // call this->SetLineNumber(_2);
		[ qi::_val = qi::_2 ]
			;

		// X 1.1 etc.
		MotionArgument = (qi::repeat(1,1)[qi::char_("xXyYzZ")] >> MathematicalExpression )
			[ phx::bind(&rs274<Iter, Skipper>::Add, phx::ref(*this), qi::_1, qi::_2) ] // call this->Add(_1, _2);
			;

		// g00 X <float> Y <float> etc.
		G00 = qi::lexeme[qi::repeat(1,1)[qi::char_("gG")] >> qi::repeat(1,2)[qi::char_("0")]] >> +(MotionArgument)
			[ phx::bind(&rs274<Iter, Skipper>::Print, phx::ref(*this) ) ]	// call this->Print()
			;

		// g01 X <float> Y <float> etc.
		G01 = ascii::no_case[qi::lit("G")] >> qi::repeat(0,1)[qi::char_("0")] >> qi::lit("1") >> +(MotionArgument)
			[ phx::bind(&rs274<Iter, Skipper>::Print, phx::ref(*this) ) ]	// call this->Print()
			;

		// EndOfBlock = (qi::no_skip[*qi::space >> qi::eol])
		EndOfBlock = qi::eol
			[ phx::bind(&rs274<Iter, Skipper>::ProcessBlock, phx::ref(*this) ) ]	// call this->EndOfBlock()
			;

		MotionCommand =	
					(LineNumberRule >> EndOfBlock)
				|	(LineNumberRule >> G01 >> EndOfBlock)
				|	(LineNumberRule >> G00 >> EndOfBlock)
					;

		MathematicalExpression = 
			  (qi::double_) [ qi::_val = qi::_1 ]

			// NOTE: We use _1 and _3 because qi::lit() does not produce an attribute and so it doesn't count as an argument.
			| (qi::lit("[") >> MathematicalExpression >> qi::char_("+") >> MathematicalExpression >> qi::lit("]"))
				[ qi::_val = qi::_1 + qi::_3 ]	

			| (qi::lit("[") >> MathematicalExpression >> qi::char_("-") >> MathematicalExpression >> qi::lit("]"))
				[ qi::_val = qi::_1 - qi::_3 ]	

			| (qi::lit("[") >> MathematicalExpression >> qi::char_("*") >> MathematicalExpression >> qi::lit("]"))
				[ qi::_val = qi::_1 * qi::_3 ]	

			| (qi::lit("[") >> MathematicalExpression >> qi::char_("/") >> MathematicalExpression >> qi::lit("]"))
				[ qi::_val = qi::_1 / qi::_3 ]	

			| (qi::lit("[") >> MathematicalExpression >> qi::lit("]")) [ qi::_val = qi::_1 ]
			;

		Start = +(MotionCommand)	// [ phx::bind(&Arguments_t::Print, phx::ref(arguments) ) ]
					;

		BOOST_SPIRIT_DEBUG_NODE(Start);
		BOOST_SPIRIT_DEBUG_NODE(MotionArgument);
		BOOST_SPIRIT_DEBUG_NODE(G01);
		BOOST_SPIRIT_DEBUG_NODE(G00);
		BOOST_SPIRIT_DEBUG_NODE(MotionCommand);
		BOOST_SPIRIT_DEBUG_NODE(LineNumberRule);
		BOOST_SPIRIT_DEBUG_NODE(EndOfBlock);
		BOOST_SPIRIT_DEBUG_NODE(MathematicalExpression);
	}

	public:
		qi::rule<Iter, Skipper> Start;
		qi::rule<Iter, Skipper> MotionArgument;
		qi::rule<Iter, Skipper> G01;
		qi::rule<Iter, Skipper> G00;		
		qi::rule<Iter, Skipper> MotionCommand;
		// qi::rule<Iter, boost::fusion::vector2<char,int>(), Skipper> LineNumberRule;
		qi::rule<Iter, int(), Skipper> LineNumberRule;
		qi::rule<Iter, Skipper> EndOfBlock;
		qi::rule<Iter, double(), Skipper> MathematicalExpression;

		// boost::fusion::vector2<char,int>		line_number;
		int		line_number;

		typedef std::map<char, double>	DoubleMap_t;
		DoubleMap_t	m_doubles;

		typedef std::map<char, int>	IntegerMap_t;
		IntegerMap_t	m_integers;
};

int CamExport wilma()
{
	// from http://stackoverflow.com/questions/12208705/add-to-a-spirit-qi-symbol-table-in-a-semantic-action
	// and http://stackoverflow.com/questions/9139015/parsing-mixed-values-and-key-value-pairs-with-boost-spirit

	arguments_dictionary arguments;
	rs274<std::string::const_iterator> linuxcnc(arguments);

	const std::string gcode = "N220 g0 X [1.0 + 0.1] Y [2.2 - 0.1] Z[[3.3 * 2]/7.0] \n"
							  "N230 g01 X 4.4 Y5.5\n";
	// const std::string gcode = "N220 G0 \n";

	std::string::const_iterator begin = gcode.begin();
	
	// Parse the GCode using the linuxcnc grammar.  The qi::blank skipper will skip all whitespace
	// except newline characters.  i.e. it allows newline characters to be included in the grammar
	// (which they need to be as they represent an 'end of block' marker)

	if (qi::phrase_parse(begin, gcode.end(), linuxcnc, qi::blank))
	{
		qDebug("last line number %d\n", linuxcnc.line_number );
		if (arguments.find("X")) qDebug("%lf\n", arguments.at("X"));
		if (arguments.find("Y")) qDebug("%lf\n", arguments.at("Y"));
		if (arguments.find("Z")) qDebug("%lf\n", arguments.at("Z"));
	}
	else
	{
		// qDebug("Parsing failed at %c%d\n", linuxcnc.line_number[0].first, linuxcnc.line_number[0].second);
		qDebug("%s\n", GrammarDebugOutputBuffer.str().c_str());
	}

	return(0);
}