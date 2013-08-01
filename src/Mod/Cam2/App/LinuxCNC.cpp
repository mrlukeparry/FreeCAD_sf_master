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
#include <math.h>

// NOTE: These BOOST SPIRIT DEBUG macros MUST be defined BEFORE the include
// files.  Otherwise the default values, defined within the include files,
// will be used instead.

#include "LinuxCNC.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace Cam;
LinuxCNC::LinuxCNC()
{
}

LinuxCNC::~LinuxCNC()
{
}


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

typedef std::string::iterator Iterator;
typedef std::pair<char, double> ArgumentData_t;
typedef double Double_t;
Double_t g_double;


namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace ascii = boost::spirit::ascii;

template <typename Iter, typename Skipper = qi::blank_type> 
	struct linuxcnc_grammar : qi::grammar<Iter, Skipper> 
{

	void Add(std::vector<char> c, double value)
	{
		if (c.size() == 1)
		{
			m_doubles.insert(std::make_pair(c[0], value));
		}
	}

	void ABS(double & result, double value)		{ result = ::abs(value); }
	void ACOS(double & result, double value)	{ result = ::acos(value); }
	void ASIN(double & result, double value)	{ result = ::asin(value); }
	void ATAN(double & result, double lhs, double rhs)	{ result = ::atan2(lhs,rhs); }
	void COS(double & result, double value)	{ result = ::cos(value); }
	void EXP(double & result, double value)	{ result = ::exp(value); }
	void FIX(double & result, double value)	{ result = double(int(::floor(value))); }
	void FUP(double & result, double value)	{ result = double(int(::ceil(value))); }
	void ROUND(double & result, double value)	{ result = double(int(value)); }
	void LN(double & result, double value)	{ result = ::log(value); }
	void SIN(double & result, double value)	{ result = ::sin(value); }
	void SQRT(double & result, double value)	{ result = ::sqrt(value); }
	void TAN(double & result, double value)	{ result = ::tan(value); }

	void Print()
	{
		for (DoubleMap_t::const_iterator itArg = m_doubles.begin(); itArg != m_doubles.end(); itArg++)
		{
			qDebug("%c=%lf\n", itArg->first, itArg->second);
		}
	}

	void SetLineNumber( const int value ) { line_number = value; }
	void StatementType( const LinuxCNC::eStatement_t type )
	{
		// Only override the statement type if this integer is larger than the
		// current value.  This allows the order of the statement type
		// enumeration to determine the precendence used for statement
		// types (for colour etc.)

		if (int(type) > int(statement_type)) statement_type = type;
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

	linuxcnc_grammar(GCode::Geometry_t *tool_movement_geometry) : linuxcnc_grammar::base_type(Start)
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
		LineNumberRule = (ascii::no_case[qi::lit("N")] >> qi::int_)
			[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::SetLineNumber, phx::ref(*this), qi::_1) ] // call this->SetLineNumber(_2);
			;

		// X 1.1 etc.
			MotionArgument = (ascii::no_case[qi::repeat(1,1)[qi::char_("XYZABCUVWLPRQIJK")]] >> MathematicalExpression )
			[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::Add, phx::ref(*this), qi::_1, qi::_2) ] // call this->Add(_1, _2);
			;

		G00 = qi::lexeme[qi::repeat(1,1)[qi::char_("gG")] >> qi::repeat(1,2)[qi::char_("0")]];
		G01 = qi::lexeme[ascii::no_case[qi::lit("G")] >> qi::repeat(0,1)[qi::char_("0")] >> qi::lit("1")];

		// EndOfBlock = (qi::no_skip[*qi::space >> qi::eol])
		EndOfBlock = qi::eol
			[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::ProcessBlock, phx::ref(*this) ) ]	// call this->EndOfBlock()
			;

		Motion =	 
					(G01 >> +(MotionArgument))
				|	(G00 >> +(MotionArgument))
				|	(ascii::no_case[qi::lit("G80")])
				|	((ascii::no_case[qi::lit("G83")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stDrilling ) ]
				;

		Comment =	(qi::lit("(") >> +(qi::char_ - qi::lit(")")) >> qi::lit(")"))
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

			| Functions [ qi::_val = qi::_1 ]

			| (qi::lit("[") >> MathematicalExpression >> qi::lit("]")) [ qi::_val = qi::_1 ]
			;

		Functions = 
				(ascii::no_case[qi::lit("ABS")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::ABS, phx::ref(*this), qi::_val, qi::_1) ] // call this->ABS(_val, _1);

			|	(ascii::no_case[qi::lit("ACOS")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::ACOS, phx::ref(*this), qi::_val, qi::_1) ] // call this->ACOS(_val, _1);

			|	(ascii::no_case[qi::lit("ASIN")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::ASIN, phx::ref(*this), qi::_val, qi::_1) ] // call this->ASIN(_val, _1);

			|	(ascii::no_case[qi::lit("COS")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::COS, phx::ref(*this), qi::_val, qi::_1) ] // call this->COS(_val, _1);

			|	(ascii::no_case[qi::lit("EXP")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::EXP, phx::ref(*this), qi::_val, qi::_1) ] // call this->EXP(_val, _1);

			|	(ascii::no_case[qi::lit("FIX")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::FIX, phx::ref(*this), qi::_val, qi::_1) ] // call this->FIX(_val, _1);

			|	(ascii::no_case[qi::lit("FUP")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::FUP, phx::ref(*this), qi::_val, qi::_1) ] // call this->FUP(_val, _1);

			|	(ascii::no_case[qi::lit("ROUND")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::ROUND, phx::ref(*this), qi::_val, qi::_1) ] // call this->ROUND(_val, _1);

			|	(ascii::no_case[qi::lit("LN")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::LN, phx::ref(*this), qi::_val, qi::_1) ] // call this->LN(_val, _1);

			|	(ascii::no_case[qi::lit("SIN")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::SIN, phx::ref(*this), qi::_val, qi::_1) ] // call this->SIN(_val, _1);

			|	(ascii::no_case[qi::lit("SQRT")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::SQRT, phx::ref(*this), qi::_val, qi::_1) ] // call this->SQRT(_val, _1);

			|	(ascii::no_case[qi::lit("TAN")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
					[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::TAN, phx::ref(*this), qi::_val, qi::_1) ] // call this->TAN(_val, _1);

			|	(ascii::no_case[qi::lit("ATAN")] >> qi::lit("[") >> MathematicalExpression >> qi::lit("]") >> qi::lit("/") >> qi::lit("[") >> MathematicalExpression >> qi::lit("]"))
				[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::ATAN, phx::ref(*this), qi::_val, qi::_1, qi::_2) ] // call this->ATAN(_val, _1, _2);
			;

		Spindle =	(ascii::no_case[qi::lit("S")] >> MathematicalExpression)
			;

		FeedRate =	(ascii::no_case[qi::lit("F")] >> MathematicalExpression)
			;

		MCodes =	(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("0")]])	// M00 - Pause Program
			|		(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("1")]])	// M01
			// Add M02 (end of program) as its own rule as its location is quite specific.
			|		(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("3")]])	// M03 - turn spindle clockwise
			|		(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("4")]])	// M04 - turn spindle counter-clockwise
			|		(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("5")]])	// M05 - stop spindle
			|		(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("6")]])	// M06 - enable selected tool
			|		(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("7")]])	// M07 - mist coolant on
			|		(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("8")]])	// M08 - flood coolant on
			|		(qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("9")]])	// M09 - coolant off
			|		(qi::lexeme[ascii::no_case[qi::lit("M")]                                  >> qi::repeat(1,1)[qi::lit("60")]])	// M60
			;

		EndOfProgram = (qi::lexeme[ascii::no_case[qi::lit("M")] >> qi::repeat(0,1)[qi::lit("0")] >> qi::repeat(1,1)[qi::lit("2")]])	// M02 - end of program
			;

		RS274 = (LineNumberRule)
			|	(Motion)
			|	(Comment)
			|	(Spindle >> *(MCodes))
			|	(Spindle)
			|	(MCodes)
			|	(FeedRate)
			|	(EndOfProgram)
			;

		Expression = (+(RS274) >> EndOfBlock)
			;

		Start = +(Expression)
			;

		BOOST_SPIRIT_DEBUG_NODE(Start);
		BOOST_SPIRIT_DEBUG_NODE(G00);
		BOOST_SPIRIT_DEBUG_NODE(G01);		
		BOOST_SPIRIT_DEBUG_NODE(Motion);
		BOOST_SPIRIT_DEBUG_NODE(MotionArgument);
		BOOST_SPIRIT_DEBUG_NODE(LineNumberRule);
		BOOST_SPIRIT_DEBUG_NODE(MathematicalExpression);
		BOOST_SPIRIT_DEBUG_NODE(Functions);
		BOOST_SPIRIT_DEBUG_NODE(Comment);
		BOOST_SPIRIT_DEBUG_NODE(RS274);
		BOOST_SPIRIT_DEBUG_NODE(Expression);
		BOOST_SPIRIT_DEBUG_NODE(Spindle);
		BOOST_SPIRIT_DEBUG_NODE(MCodes);
		BOOST_SPIRIT_DEBUG_NODE(EndOfBlock);
		BOOST_SPIRIT_DEBUG_NODE(EndOfProgram);
		BOOST_SPIRIT_DEBUG_NODE(FeedRate);
	}

	public:
		qi::rule<Iter, Skipper> Start;
		qi::rule<Iter, Skipper> G00;
		qi::rule<Iter, Skipper> G01;
		qi::rule<Iter, Skipper> G80;
		qi::rule<Iter, Skipper> G83;
		qi::rule<Iter, Skipper> Motion;
		qi::rule<Iter, Skipper> MotionArgument;
		qi::rule<Iter, int(), Skipper> LineNumberRule;
		qi::rule<Iter, double(), Skipper> MathematicalExpression;
		qi::rule<Iter, double(), Skipper> Functions;
		qi::rule<Iter, Skipper> Comment;
		qi::rule<Iter, Skipper> RS274;
		qi::rule<Iter, Skipper> Expression;
		qi::rule<Iter, Skipper> Spindle;
		qi::rule<Iter, Skipper> MCodes;
		qi::rule<Iter, Skipper> EndOfBlock;
		qi::rule<Iter, Skipper> EndOfProgram;
		qi::rule<Iter, Skipper> FeedRate;

		double previous[9]; // in parse_units

		LinuxCNC::eCoordinateSystems_t current_coordinate_system;
		LinuxCNC::eCoordinateSystems_t modal_coordinate_system;

		double feed_rate;	// invalid.
		double spindle_speed;
		double units;
		double tool_length_offset;

		int	 line_offset;			// Which line in the GCode file are we processing?  i.e. index into g_svLines
		int  line_number;			// GCode line number.  i.e. N30, N40 etc.
		char comment[1024];

		LinuxCNC::ePlane_t plane;

		int	tool_slot_number;

		// The statement type defines both the colour of the GCode text and the type of
		// motion used.
		LinuxCNC::eStatement_t statement_type;
		LinuxCNC::eStatement_t previous_statement_type;	// from previous block.

		typedef std::map<char, double>	DoubleMap_t;
		DoubleMap_t	m_doubles;

		typedef std::map<char, int>	IntegerMap_t;
		IntegerMap_t	m_integers; 
};

GCode::Geometry_t LinuxCNC::Parse(const char *program)
{
	// from http://stackoverflow.com/questions/12208705/add-to-a-spirit-qi-symbol-table-in-a-semantic-action
	// and http://stackoverflow.com/questions/9139015/parsing-mixed-values-and-key-value-pairs-with-boost-spirit

	Geometry_t tool_movement_graphics;
	linuxcnc_grammar<std::string::const_iterator> parser(&tool_movement_graphics);

	// const std::string gcode = "N220 g0 X [1.0 + 0.1] Y [2.2 - 0.1] Z[[3.3 * 2]/7.0] \n"
	// 						  "N230 g01 X abs[-4.4] Y ATAN[4]/[3]\n";
	// const std::string gcode = "N220 G0 \n";
	const std::string gcode(program);

	std::string::const_iterator begin = gcode.begin();
	
	// Parse the GCode using the linuxcnc grammar.  The qi::blank skipper will skip all whitespace
	// except newline characters.  i.e. it allows newline characters to be included in the grammar
	// (which they need to be as they represent an 'end of block' marker)

	if ((qi::phrase_parse(begin, gcode.end(), parser, qi::blank)) && (begin == gcode.end()))
	{
		qDebug("last line number %d\n", parser.line_number );
		qDebug("Generated %d graphical elements representing the tool movements\n", tool_movement_graphics.size());
	}
	else
	{
		// qDebug("Parsing failed at %c%d\n", linuxcnc.line_number[0].first, linuxcnc.line_number[0].second);
		qDebug("%s\n", GCode_GrammarDebugOutputBuffer.str().c_str());
		if (begin != gcode.end())
		{
			qDebug("%s\n", std::string(begin, gcode.end()).c_str());
		}
	}

	return(tool_movement_graphics);
}