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

		current_coordinate_system = LinuxCNC::csG53;
		modal_coordinate_system = LinuxCNC::csG54;
		units = 1.0;	// Metric by default.
		plane = LinuxCNC::eXYPlane;
		tool_length_offset = 0.0;

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
		G02 = qi::lexeme[ascii::no_case[qi::lit("G")] >> qi::repeat(0,1)[qi::char_("0")] >> qi::lit("2")];
		G03 = qi::lexeme[ascii::no_case[qi::lit("G")] >> qi::repeat(0,1)[qi::char_("0")] >> qi::lit("3")];
		G04 = qi::lexeme[ascii::no_case[qi::lit("G")] >> qi::repeat(0,1)[qi::char_("0")] >> qi::lit("4")];

		// EndOfBlock = (qi::no_skip[*qi::space >> qi::eol])
		EndOfBlock = qi::eol
			[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::ProcessBlock, phx::ref(*this) ) ]	// call this->EndOfBlock()
			;

		Motion =
					(G00 >> +(MotionArgument)) [ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stRapid ) ]
				|	(G01 >> +(MotionArgument)) [ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stFeed ) ]
				|	(G02 >> +(MotionArgument)) [ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stArcClockwise ) ]
				|	(G03 >> +(MotionArgument)) [ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stArcCounterClockwise ) ]
				|	(G04 >> +(MotionArgument)) [ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stFeed ) ]
				|	(ascii::no_case[qi::lit("G80")])

				|	((ascii::no_case[qi::lit("G33_1")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stFeed ) ]

				|	((ascii::no_case[qi::lit("G33_2")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stFeed ) ]

				|	((ascii::no_case[qi::lit("G33_3")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stFeed ) ]

				|	((ascii::no_case[qi::lit("G33_4")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stFeed ) ]

				|	((ascii::no_case[qi::lit("G33_5")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stFeed ) ]

				|	((ascii::no_case[qi::lit("G33")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stFeed ) ]
				
				|	((ascii::no_case[qi::lit("G81")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stDrilling ) ]

				|	((ascii::no_case[qi::lit("G82")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stDrilling ) ]

				|	((ascii::no_case[qi::lit("G83")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stDrilling ) ]

				|	((ascii::no_case[qi::lit("G84")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stTapping ) ]

				|	((ascii::no_case[qi::lit("G85")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stBoring ) ]

				|	((ascii::no_case[qi::lit("G86")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stBoring ) ]

				|	((ascii::no_case[qi::lit("G89")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stBoring ) ]
				;

		NonModalCodes =	
					((ascii::no_case[qi::lit("G28_1")]) >> +(MotionArgument)) // Save current position.
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stG28 ) ]

				|	((ascii::no_case[qi::lit("G28")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stG28 ) ]

				|	((ascii::no_case[qi::lit("G30_1")]) >> +(MotionArgument)) // Save current position.
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stG30 ) ]

				|	((ascii::no_case[qi::lit("G30")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stG30 ) ]

				|	((ascii::no_case[qi::lit("G92_1")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stG92_1 ) ]

				|	((ascii::no_case[qi::lit("G92_2")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stG92_2 ) ]

				|	((ascii::no_case[qi::lit("G92_3")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stG92_3 ) ]

				|	((ascii::no_case[qi::lit("G92")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stG92 ) ]

				|	((ascii::no_case[qi::lit("G10")]) >> +(MotionArgument))
						[ phx::bind(&linuxcnc_grammar<Iter, Skipper>::StatementType, phx::ref(*this),  LinuxCNC::stDataSetting ) ]
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
		BOOST_SPIRIT_DEBUG_NODE(G02);
		BOOST_SPIRIT_DEBUG_NODE(G03);
		BOOST_SPIRIT_DEBUG_NODE(G04);
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
		BOOST_SPIRIT_DEBUG_NODE(NonModalCodes);
		
	}

	public:
		qi::rule<Iter, Skipper> Start;
		qi::rule<Iter, Skipper> G00;
		qi::rule<Iter, Skipper> G01, G02, G03, G04;
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
		qi::rule<Iter, Skipper> Spindle, FeedRate;
		qi::rule<Iter, Skipper> MCodes, NonModalCodes;
		qi::rule<Iter, Skipper> EndOfBlock, EndOfProgram;

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

	private:
			double Emc2Units(const double value_in_parse_units)
			{
				double value_in_mm = value_in_parse_units * this->units;

				switch (PROGRAM->m_emc2_variables_units)
				{
					case CProgram::eImperial:
						return(value_in_mm / 25.4); // imperial - inches

					case CProgram::eMetric:
					default:
						return(value_in_mm);
				}
			}

			// Heeks uses millimeters
			double HeeksUnits( const double value_in_emc2_units )
			{
				switch (PROGRAM->m_emc2_variables_units)
				{
					case CProgram::eImperial:
						return(value_in_emc2_units * 25.4);


					case CProgram::eMetric:
					default:
						return(value_in_emc2_units);
				}
			}

			double ParseUnits( const double value_in_emc2_units )
			{
				double value_in_mm = HeeksUnits( value_in_emc2_units );
				return( value_in_mm / this->units );
			}

			double ParseUnitsFromHeeksUnits( const double value_in_heeks_units )
			{
				double value_in_mm = value_in_heeks_units;	// always.
				return( value_in_mm / this->units );
			}


			/**
				When the GCode switches from metric to imperial (or vice-versa) we need
				to change the values in out ParseState_t structure to the new standards
				so that we're comparing 'apples with apples' when interpreting
				subsequent coordinates.
			 */
			void SwitchParseUnits( const int metric )
			{
				if (metric)
				{
					if (this->units == 25.4)
					{
						// Need to adjust the existing settings from imperial to metric.

						this->x *= 25.4;
						this->y *= 25.4;
						this->z *= 25.4;

						/*
						Do not adjust a,b or c values as these are rotational units (degrees) rather than linear units
						this->a *= 25.4;
						this->b *= 25.4;
						this->c *= 25.4;
						*/

						this->u *= 25.4;
						this->v *= 25.4;
						this->w *= 25.4;

						this->i *= 25.4;
						this->j *= 25.4;
						this->k *= 25.4;

						this->l *= 25.4;
						this->p *= 25.4;
						this->q *= 25.4;
						this->r *= 25.4;

						for (::size_t i=0; i<sizeof(this->previous)/sizeof(this->previous[0]); i++)
						{
							this->previous[i] *= 25.4;
						}

						this->feed_rate *= 25.4;
					}

					this->units = 1.0;  // Switch to metric
				}
				else
				{
					if (this->units == 1.0)
					{
						// Need to adjust the existing settings from metric to imperial.

						this->x /= 25.4;
						this->y /= 25.4;
						this->z /= 25.4;

						/*
						Do not adjust a,b or c values as these are rotational units (degrees) rather than linear units
						this->a /= 25.4;
						this->b /= 25.4;
						this->c /= 25.4;
						*/

						this->u /= 25.4;
						this->v /= 25.4;
						this->w /= 25.4;

						this->i /= 25.4;
						this->j /= 25.4;
						this->k /= 25.4;

						this->l /= 25.4;
						this->p /= 25.4;
						this->q /= 25.4;
						this->r /= 25.4;

						for (::size_t i=0; i<sizeof(this->previous)/sizeof(this->previous[0]); i++)
						{
							this->previous[i] /= 25.4;
						}

						this->feed_rate /= 25.4;
					}

					this->units = 25.4;  // Switch to imperial
				}
			}


			int AddSymbol( const char *name, const double value )
			{
				if ((name == NULL) || (*name == '\0'))
				{
					int id = emc_variables.new_id();
					emc_variables[id] = value;
					return(id);
				}
				else
				{
					double unused = emc_variables[name];	// This either finds or creates an entry.
					return(emc_variables.hash(name));
				}
			}


			// Either find an existing symbol with this name and return it's ID or
			// add a new symbol with this name.
			int Symbol( const char *name )
			{
				if ((name == NULL) || (*name == '\0'))
				{
					int id = emc_variables.new_id();
					emc_variables[id] = 0.0;
					return(id);
				}
				else
				{
					double unused = emc_variables[name];	// This either finds or creates an entry.
					return(emc_variables.hash(name));
				}
			}


			int LHSequivalenttoRHS(const int lhs, const int rhs)
			{
				return(( emc_variables[lhs] == emc_variables[rhs] )?1:0);
			}

			int LHSnotequaltoRHS(const int lhs, const int rhs)
			{
				return(( emc_variables[lhs] != emc_variables[rhs] )?1:0);
			}

			int LHSgreaterthanRHS(const int lhs, const int rhs)
			{
				return(( emc_variables[lhs] > emc_variables[rhs] )?1:0);
			}

			int LHSlessthanRHS(const int lhs, const int rhs)
			{
				return(( emc_variables[lhs] < emc_variables[rhs] )?1:0);
			}


			int LHSplusRHS(const int lhs, const int rhs)
			{
				int id = emc_variables.new_id();
				emc_variables[id] = emc_variables[lhs] + emc_variables[rhs];
				return(id);
			}

			int LHSminusRHS(const int lhs, const int rhs)
			{
				int id = emc_variables.new_id();
				emc_variables[id] = emc_variables[lhs] - emc_variables[rhs];
				return(id);
			}

			int LHStimesRHS(const int lhs, const int rhs)
			{
				int id = emc_variables.new_id();
				emc_variables[id] = emc_variables[lhs] * emc_variables[rhs];
				return(id);
			}

			int LHSdividedbyRHS(const int lhs, const int rhs)
			{
				int id = emc_variables.new_id();
				if (emc_variables[rhs] == 0.0)
				{
					// We don't want to get into too much trouble just for backplotting.
					emc_variables[id] = DBL_MAX;
				}
				else
				{
					emc_variables[id] = emc_variables[lhs] / emc_variables[rhs];
				}

				return(id);
			}


			int LHSassignmentfromRHS( const int lhs, const int rhs )
			{
				emc_variables[lhs] = emc_variables[rhs];
				return(lhs);
			}


			double Value(const int name)
			{
				return( emc_variables[name] );
			}


			const char * StringFromDouble(const double name)
			{
				std::ostringstream ossName;
				ossName << name;

				string_tokens.push_back(ossName.str());
				return(string_tokens.back().c_str());
			}


			const char *StringDuplication( const char *value )
			{
				string_tokens.push_back(value);
				return(string_tokens.back().c_str());
			}

			double radians_to_degrees( const double radians )
			{
				double a = radians;
				return( (radians / (2.0 * M_PI)) * 360.0 );
			}

			double degrees_to_radians( const double degrees )
			{
				double a = degrees;
				return( (degrees / 360.0) * (2.0 * M_PI) );
			}


			int ASin(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = radians_to_degrees( asin(degrees_to_radians(emc_variables[symbol_id] )) );
				return(id);
			}

			int ACos(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = radians_to_degrees( acos(degrees_to_radians(emc_variables[symbol_id] )) );
				return(id);
			}

			int ATan(const int symbol_id)
			{
				int id=emc_variables.new_id();
				double degrees = emc_variables[symbol_id];
				double radians = atan(degrees_to_radians(degrees));
				emc_variables[id] = radians_to_degrees( radians );
				return(id);
			}

			int Sin(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = radians_to_degrees( sin(degrees_to_radians(emc_variables[symbol_id] )) );
				return(id);
			}

			int Cos(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = radians_to_degrees( cos(degrees_to_radians(emc_variables[symbol_id] )) );
				return(id);
			}

			int Tan(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = radians_to_degrees( tan(degrees_to_radians(emc_variables[symbol_id] )) );
				return(id);
			}

			int AbsoluteValue(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = abs(emc_variables[symbol_id]);
				return(id);
			}

			int	Sqrt(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = sqrt(emc_variables[symbol_id]);
				return(id);
			}

			int	Exp(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = exp((emc_variables[symbol_id]));
				return(id);
			}

			int	Fix(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = int(floor(((emc_variables[symbol_id]))));
				return(id);
			}

			int	Fup(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = int(ceil(((emc_variables[symbol_id]))));
				return(id);
			}

			int	Round(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = int(((emc_variables[symbol_id])));
				return(id);
			}

			int	Ln(const int symbol_id)
			{
				int id=emc_variables.new_id();
				emc_variables[id] = log(emc_variables[symbol_id]);
				return(id);
			}

			int	Exists(const int symbol_id)
			{
				return(emc_variables.exists(symbol_id));
			}


			/**
				Adjust the value for the active coordinate system.  The parameter_offset is 0 for x, 1 for y etc.
				We want to arrange the GCode so that the G54 (the first relative coordinate system) lines up
				with the HeeksCAD drawing coordinate system.  All other coordinates are moved around that
				relative position.  We're using the fact that the gcode_variables map was initialized
				from the emc.var file to set the offsets of the various coordinate systems.

				The parameter_offset is added to the modal_coordinate_system base number (eg: 5221 for G54)
				to indicate which variable within that coordinate system we're referring to.  eg: 0 = x,
				1 = y etc.
			 */
			double adjust( const int parameter_offset, const double value_in_parse_units )
			{
				double value_in_emc2_units = Emc2Units(value_in_parse_units);
				double tool_length_offset = 0.0;

				if (parameter_offset == 2)
				{
					// We're adjusting a Z coordinate so add the tool length offset
					tool_length_offset = this->tool_length_offset;
				}

				if (parameter_offset >= 9)
				{
					printf("Parameter offset must be less than 9.  It is %d instead\n", parameter_offset);
					return( HeeksUnits(value_in_emc2_units) );
				}

				double g54_offset = emc_variables[eG54VariableBase + parameter_offset];
				double g92_offset = emc_variables[eG92VariableBase + parameter_offset];

				if (emc_variables[eG92Enabled] > 0.0)
				{
					// Copy the values from the gcode_variables into the local cache.
					return(HeeksUnits(value_in_emc2_units - g54_offset + g92_offset + tool_length_offset));
				}

				if (this->current_coordinate_system == csG53)
				{
					// We need to move the point so that it's relavitve to the eG54 coordinate system.
					return(HeeksUnits(value_in_emc2_units - g54_offset + tool_length_offset));
				}

				int coordinate_system_offset = eG54VariableBase + ((this->modal_coordinate_system - 1) * 20);
				int name = coordinate_system_offset + parameter_offset;

				return(HeeksUnits(value_in_emc2_units - g54_offset + emc_variables[name] + tool_length_offset));
			}


			/**
				We have received an 'end of block' character (a newline character).  Take all the settings we've found for
				this block and add the XML describing them. This includes both a verbatim copy of the original GCode line
				(taken from the g_svLines cache) and the various line/arc (etc.) elements that will allow Heeks to draw
				the path's meaning.
			 */
			void AddToHeeks()
			{

				if (this->statement_type == stUndefined)
				{
					switch (this->previous_statement_type)
					{
						// If it's one of the 'modal' commands then we don't actually NEED to see
						// that command again in the next block.  We could just be given the next
						// set of coordinates for the same command.  In this case, keep the
						// statement type as it is.  If another is seen, it will be overridden.
						case stProbe:
						case stFeed:
						case stArcClockwise:
						case stArcCounterClockwise:
						case stDrilling:
						case stBoring:
						case stTapping:
							this->statement_type = this->previous_statement_type;	// Reinstate the previous statement type.
						break;
					}
				} // End if - then


				if (::size_t(this->line_offset) < g_svLines.size())
				{
					xml << _T("<ncblock>\n");

					// See if the line number is the first part of the line.  If so, colour it.
					if ((strlen(this->line_number) > 0) && (g_svLines[this->line_offset].find(this->line_number) == 0))
					{
						xml << _T("<text col=\"blocknum\">") << Ctt(this->line_number) << _T("</text>\n");
						xml << _T("<text><![CDATA[ ]]></text>\n");
						std::string value = XmlData(g_svLines[this->line_offset].c_str());
						value.erase(0, strlen(this->line_number) );
						xml << _T("<text col=\"") << ColourForStatementType(this->statement_type) << _T("\">") << Ctt(value.c_str()) << _T("</text>\n");
					}
					else
					{
						xml << _T("<text col=\"") << ColourForStatementType(this->statement_type) << _T("\">") << Ctt(XmlData(g_svLines[this->line_offset].c_str()).c_str()) << _T("</text>\n");
					}

					switch (this->statement_type)
					{
					case stUndefined:
						break;

					case stDataSetting:
						// The G10 statement has been specified.  Look at the L argument to see what needs to be set.
						if ((this->l_specified) && (this->p_specified))
						{
							if (this->l == 20)
							{
								// We need to make the current coordinate be whatever the arguments say they are.  i.e.
								// adjust the appropriate coordinate system offset accordingly.

								int coordinate_system = this->p;

								if (this->x_specified)
								{
									int parameter_offset = 0;	// x
									int coordinate_system_offset = eG54VariableBase + ((this->p - 1) * 20);
									int name = coordinate_system_offset + parameter_offset;
									double offset_in_heeks_units = HeeksUnits( emc_variables[name] );
									double offset_in_emc2_units = Emc2Units( this->x ) - emc_variables[name];
									emc_variables[name] = emc_variables[name] + offset_in_emc2_units;
								}

								if (this->y_specified)
								{
									int parameter_offset = 1;	// y
									int coordinate_system_offset = eG54VariableBase + ((this->p - 1) * 20);
									int name = coordinate_system_offset + parameter_offset;
									double offset_in_heeks_units = HeeksUnits( emc_variables[name] );
									double offset_in_emc2_units = Emc2Units( this->y ) - emc_variables[name];
									emc_variables[name] = emc_variables[name] + offset_in_emc2_units;
								}

								if (this->z_specified)
								{
									int parameter_offset = 2;	// z
									int coordinate_system_offset = eG54VariableBase + ((this->p - 1) * 20);
									int name = coordinate_system_offset + parameter_offset;
									double offset_in_heeks_units = HeeksUnits( emc_variables[name] );
									double offset_in_emc2_units = Emc2Units( this->z ) - emc_variables[name];
									emc_variables[name] = emc_variables[name] + offset_in_emc2_units;
								}
							}
						}
						break;

					case stToolLengthEnabled:
						// The Z parameters given determine where we should think
						// we are right now.
						// this->tool_length_offset = this->k - ParseUnits(emc_variables[eG54VariableBase+2]);
						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line z=\"") << adjust(2,this->z) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						break;

					case stToolLengthDisabled:
						// The Z parameters given determine where we should think
						// we are right now.
						this->tool_length_offset = 0.0;
						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line z=\"") << adjust(2,this->previous[2]) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						break;

					case stRapid:
						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line ");
						if (this->x_specified) xml << _T("x=\"") << adjust(0,this->x) << _T("\" ");
						if (this->y_specified) xml << _T("y=\"") << adjust(1,this->y) << _T("\" ");
						if (this->z_specified) xml << _T("z=\"") << adjust(2,this->z) << _T("\" ");
						xml << _T("/>\n")
							<< _T("</path>\n");
						break;

					case stFeed:
						xml << _T("<path col=\"feed\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line ");
						if (this->x_specified) xml << _T("x=\"") << adjust(0,this->x) << _T("\" ");
						if (this->y_specified) xml << _T("y=\"") << adjust(1,this->y) << _T("\" ");
						if (this->z_specified) xml << _T("z=\"") << adjust(2,this->z) << _T("\" ");
						xml << _T("/>\n")
							<< _T("</path>\n");
						if (this->feed_rate <= 0.0) 
						{
							wxString warning;
							warning << _("Zero feed rate found for feed movement - line ") << Ctt(this->line_number) << _(" GCode ") << Ctt(g_svLines[this->line_offset].c_str());
							popup_warnings.insert(warning);
						}
						break;

					case stProbe:
						xml << _T("<path col=\"feed\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line ");
						if (this->x_specified) xml << _T("x=\"") << adjust(0,this->x) << _T("\" ");
						if (this->y_specified) xml << _T("y=\"") << adjust(1,this->y) << _T("\" ");
						if (this->z_specified) xml << _T("z=\"") << adjust(2,this->z) << _T("\" ");
						xml << _T("/>\n")
							<< _T("</path>\n");

						// Assume that the furthest point of probing tripped the switch.  Store this location
						// as though we found our probed object here.
						emc_variables[eG38_2VariableBase + 0] = ParseUnitsFromHeeksUnits(adjust(0,this->x));
						emc_variables[eG38_2VariableBase + 1] = ParseUnitsFromHeeksUnits(adjust(1,this->y));
						emc_variables[eG38_2VariableBase + 2] = ParseUnitsFromHeeksUnits(adjust(2,this->z));

						emc_variables[eG38_2VariableBase + 3] = adjust(3,this->a);
						emc_variables[eG38_2VariableBase + 4] = adjust(4,this->b);
						emc_variables[eG38_2VariableBase + 5] = adjust(5,this->c);

						emc_variables[eG38_2VariableBase + 6] = ParseUnitsFromHeeksUnits(adjust(6,this->u));
						emc_variables[eG38_2VariableBase + 7] = ParseUnitsFromHeeksUnits(adjust(7,this->v));
						emc_variables[eG38_2VariableBase + 8] = ParseUnitsFromHeeksUnits(adjust(8,this->w));

						if (this->feed_rate <= 0.0) popup_warnings.insert(_("Zero feed rate found for probe movement"));
						break;

					case stArcClockwise:
						xml << _T("<path col=\"feed\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<arc x=\"") << adjust(0,this->x) << _T("\" ")
							<< _T("y=\"") << adjust(1,this->y) << _T("\" ")
							<< _T("z=\"") << adjust(1,this->z) << _T("\" ")
							<< _T("i=\"") << HeeksUnits(Emc2Units(this->i)) << _T("\" ")
							<< _T("j=\"") << HeeksUnits(Emc2Units(this->j)) << _T("\" ")
							<< _T("k=\"") << HeeksUnits(Emc2Units(this->k)) << _T("\" ")
							<< _T("d=\"-1\" ")
							<< _T("/>\n")
							<< _T("</path>\n");

							{
								// Confirm that the previous location, the center-point and the final location all make
								// sense for an arc movement.  If we have a rounding error then we want to know it.
								gp_Pnt start(this->previous[0], this->previous[1], this->previous[2]);
								gp_Pnt end(this->x, this->y, this->z);
								gp_Pnt centre(this->i + this->previous[0], this->j + this->previous[1], this->k + this->previous[2]);

								switch (this->plane)
								{
								case eXYPlane:
									start  = gp_Pnt(this->previous[0],                  this->previous[1],                  0.0);
									end    = gp_Pnt(this->x,                            this->y,                            0.0);
									centre = gp_Pnt(this->i + this->previous[0], this->j + this->previous[1], 0.0);
									break;

								case eXZPlane:
									start  = gp_Pnt(this->previous[0],                  0.0, this->previous[2]);
									end    = gp_Pnt(this->x,                            0.0, this->y);
									centre = gp_Pnt(this->i + this->previous[0], 0.0, this->k + this->previous[2]);
									break;

								case eYZPlane:
									start  = gp_Pnt(0.0, this->previous[1],                  this->previous[2]);
									end    = gp_Pnt(0.0, this->y,                            this->y);
									centre = gp_Pnt(0.0, this->j + this->previous[1], this->k + this->previous[2]);
									break;
								} // End switch

								double error = fabs(fabs(centre.Distance(end)) - fabs(start.Distance(centre)));
								double tolerance = 1.0 / pow(10, double(Python::RequiredDecimalPlaces()));
								if (error > tolerance)
								{
									double magnitude = (fabs(error) - fabs(tolerance)) * pow(10, double(Python::RequiredDecimalPlaces()-1));
									if (magnitude > 1.0)
									{
										wxString msg;
										msg << _T("Clockwise arc offset between start and centre does not match that between centre and end.");
										// msg << _T(" error is ") << error << _T(" line ") << this->line_number;
										popup_warnings.insert(msg.c_str());
									}
								}
							}

							if (this->feed_rate <= 0.0) popup_warnings.insert(_("Zero feed rate found for arc movement"));
						break;

					case stArcCounterClockwise:
						xml << _T("<path col=\"feed\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<arc x=\"") << adjust(0,this->x) << _T("\" ")
							<< _T("y=\"") << adjust(1,this->y) << _T("\" ")
							<< _T("z=\"") << adjust(1,this->z) << _T("\" ")
							<< _T("i=\"") << HeeksUnits(Emc2Units(this->i)) << _T("\" ")
							<< _T("j=\"") << HeeksUnits(Emc2Units(this->j)) << _T("\" ")
							<< _T("k=\"") << HeeksUnits(Emc2Units(this->k)) << _T("\" ")
							<< _T("d=\"1\" ")
							<< _T("/>\n")
							<< _T("</path>\n");

							{
								// Confirm that the previous location, the center-point and the final location all make
								// sense for an arc movement.  If we have a rounding error then we want to know it.
								gp_Pnt start(this->previous[0], this->previous[1], this->previous[2]);
								gp_Pnt end(this->x, this->y, this->z);
								gp_Pnt centre(this->i + this->previous[0], this->j + this->previous[1], this->k + this->previous[2]);

								switch (this->plane)
								{
								case eXYPlane:
									start  = gp_Pnt(this->previous[0],                  this->previous[1],                  0.0);
									end    = gp_Pnt(this->x,                            this->y,                            0.0);
									centre = gp_Pnt(this->i + this->previous[0], this->j + this->previous[1], 0.0);
									break;

								case eXZPlane:
									start  = gp_Pnt(this->previous[0],                  0.0, this->previous[2]);
									end    = gp_Pnt(this->x,                            0.0, this->y);
									centre = gp_Pnt(this->i + this->previous[0], 0.0, this->k + this->previous[2]);
									break;

								case eYZPlane:
									start  = gp_Pnt(0.0, this->previous[1],                  this->previous[2]);
									end    = gp_Pnt(0.0, this->y,                            this->y);
									centre = gp_Pnt(0.0, this->j + this->previous[1], this->k + this->previous[2]);
									break;
								} // End switch

								double error = fabs(fabs(centre.Distance(end)) - fabs(start.Distance(centre)));
								double tolerance = 1.0 / pow(10, double(Python::RequiredDecimalPlaces()));
								if (error > tolerance)
								{
									double magnitude = (fabs(error) - fabs(tolerance)) * pow(10, double(Python::RequiredDecimalPlaces()-1));
									if (magnitude > 1.0)
									{
										wxString msg;
										msg << _T("Counter-Clockwise arc offset between start and centre does not match that between centre and end.");
										// msg << _T(" error is ") << error << _T(" line ") << this->line_number;
										popup_warnings.insert(msg.c_str());
									}
								}
							}
							if (this->feed_rate <= 0.0) popup_warnings.insert(_("Zero feed rate found for arc movement"));
						break;

					case stBoring:
					case stDrilling:
						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line x=\"") << adjust(0,this->x) << _T("\" ")
							<< _T("y=\"") << adjust(1,this->y) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line z=\"") << adjust(2,this->r) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						xml << _T("<path col=\"feed\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line z=\"") << adjust(2,this->z) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line z=\"") << adjust(2,this->r) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						this->z = this->r;	// We end up at the clearance (r) position.
						if (this->feed_rate <= 0.01) popup_warnings.insert(_("Zero feed rate found for drilling movement"));
						break;

					case stTapping:
						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line x=\"") << adjust(0,this->x) << _T("\" ")
							<< _T("y=\"") << adjust(1,this->y) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line z=\"") << adjust(2,this->r) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						xml << _T("<path col=\"feed\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line z=\"") << adjust(2,this->z) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						xml << _T("<path col=\"feed\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line z=\"") << adjust(2,this->r) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						this->z = this->r;	// We end up at the clearance (r) position.
						if (this->feed_rate <= 0.0) popup_warnings.insert(_("Zero feed rate found for tapping movement"));
						break;


					case stG28:
						// The saved position can be found in variables 5161 to 5169.
						this->x = ParseUnits(emc_variables[ eG28VariableBase + 0 ] - emc_variables[ eG54VariableBase + 0 ]);
						this->y = ParseUnits(emc_variables[ eG28VariableBase + 1 ] - emc_variables[ eG54VariableBase + 1 ]);
						this->z = ParseUnits(emc_variables[ eG28VariableBase + 2 ] - emc_variables[ eG54VariableBase + 2 ]);
						this->a = ParseUnits(emc_variables[ eG28VariableBase + 3 ] - emc_variables[ eG54VariableBase + 3 ]);
						this->b = ParseUnits(emc_variables[ eG28VariableBase + 4 ] - emc_variables[ eG54VariableBase + 4 ]);
						this->c = ParseUnits(emc_variables[ eG28VariableBase + 5 ] - emc_variables[ eG54VariableBase + 5 ]);
						this->u = ParseUnits(emc_variables[ eG28VariableBase + 6 ] - emc_variables[ eG54VariableBase + 6 ]);
						this->v = ParseUnits(emc_variables[ eG28VariableBase + 7 ] - emc_variables[ eG54VariableBase + 7 ]);
						this->w = ParseUnits(emc_variables[ eG28VariableBase + 8 ] - emc_variables[ eG54VariableBase + 8 ]);

						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line x=\"") << adjust(0,this->x) << _T("\" ")
							<< _T("y=\"") << adjust(1,this->y) << _T("\" ")
							<< _T("z=\"") << adjust(2,this->z) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						break;

					case stG30:
						// The saved position can be found in variables 5181 to 5189.
						this->x = ParseUnits(emc_variables[ eG30VariableBase + 0 ] - emc_variables[ eG54VariableBase + 0 ]);
						this->y = ParseUnits(emc_variables[ eG30VariableBase + 1 ] - emc_variables[ eG54VariableBase + 1 ]);
						this->z = ParseUnits(emc_variables[ eG30VariableBase + 2 ] - emc_variables[ eG54VariableBase + 2 ]);
						this->a = ParseUnits(emc_variables[ eG30VariableBase + 3 ] - emc_variables[ eG54VariableBase + 3 ]);
						this->b = ParseUnits(emc_variables[ eG30VariableBase + 4 ] - emc_variables[ eG54VariableBase + 4 ]);
						this->c = ParseUnits(emc_variables[ eG30VariableBase + 5 ] - emc_variables[ eG54VariableBase + 5 ]);
						this->u = ParseUnits(emc_variables[ eG30VariableBase + 6 ] - emc_variables[ eG54VariableBase + 6 ]);
						this->v = ParseUnits(emc_variables[ eG30VariableBase + 7 ] - emc_variables[ eG54VariableBase + 7 ]);
						this->w = ParseUnits(emc_variables[ eG30VariableBase + 8 ] - emc_variables[ eG54VariableBase + 8 ]);

						xml << _T("<path col=\"rapid\" fixture=\"") << int(this->modal_coordinate_system) << _T("\">\n")
							<< _T("<line x=\"") << adjust(0,this->x) << _T("\" ")
							<< _T("y=\"") << adjust(1,this->y) << _T("\" ")
							<< _T("z=\"") << adjust(2,this->z) << _T("\" ")
							<< _T("/>\n")
							<< _T("</path>\n");
						break;

					case stG92:
						// Make the axis values just read in become the current machine position by
						// figuring out what these values are (in machine coordinates) and adding
						// the offsets into a new set of coordinate system variables.  That's so
						// we can use the adjust() routine to allow for them in subsequent
						// commands.  This must occur until we find a G92.1 command which turns
						// this 'temporary coordinate system' functionality off.
						this->current_coordinate_system = csG92;
						emc_variables[eG92Enabled] = 1.0;

						emc_variables[eG92VariableBase + 0] = Emc2Units(this->previous[0]) - Emc2Units(this->x);
						emc_variables[eG92VariableBase + 1] = Emc2Units(this->previous[1]) - Emc2Units(this->y);
						emc_variables[eG92VariableBase + 2] = Emc2Units(this->previous[2]) - Emc2Units(this->z);

						emc_variables[eG92VariableBase + 3] = Emc2Units(this->previous[3]) - Emc2Units(this->a);
						emc_variables[eG92VariableBase + 4] = Emc2Units(this->previous[4]) - Emc2Units(this->b);
						emc_variables[eG92VariableBase + 5] = Emc2Units(this->previous[5]) - Emc2Units(this->c);

						emc_variables[eG92VariableBase + 6] = Emc2Units(this->previous[6]) - Emc2Units(this->u);
						emc_variables[eG92VariableBase + 7] = Emc2Units(this->previous[7]) - Emc2Units(this->v);
						emc_variables[eG92VariableBase + 8] = Emc2Units(this->previous[8]) - Emc2Units(this->w);
						break;

					case stG92_1:
						// Turn off the 'temporary coordinate system' functionality.
						this->current_coordinate_system = csUndefined;

						// Disable the G92 offset function.
						emc_variables[eG92Enabled] = 0.0;

						// Reset the G92 offsets to all zero.
						for (int i=eG92VariableBase; i<eG92VariableBase+9; i++)
						{
							emc_variables[i] = 0.0;
						}

						break;

					case stG92_2:
						// Disable the G92 offset function but don't reset the offsets in the memory locations.
						emc_variables[eG92Enabled] = 0.0;
						break;

					case stG92_3:
						// Re-Enable the G92 offset function and don't change the offsets in the memory locations.
						emc_variables[eG92Enabled] = 1.0;
						break;

					case stAxis:
						// Nothing extra special to do here.
						break;

					} // End switch

					xml << _T("</ncblock>\n");
				}

			} // End AddToHeeks() routine


			void ResetForEndOfBlock()
			{
				/*
				FILE *fp = fopen("c:\\temp\\david.log","a+t");
				fprintf(fp,"%s\n", this->line_number);
				fprintf(fp,"%s\n", emc_variables.log().c_str());
				fclose(fp);
				*/

				this->previous_statement_type = this->statement_type;
				this->statement_type = stUndefined;

				memset( this->comment, '\0', sizeof(this->comment) );
				memset( this->line_number, '\0', sizeof(this->line_number) );

				this->previous[0] = this->x;
				this->previous[1] = this->y;
				this->previous[2] = this->z;
				this->previous[3] = this->a;
				this->previous[4] = this->b;
				this->previous[5] = this->c;
				this->previous[6] = this->u;
				this->previous[7] = this->v;
				this->previous[8] = this->w;

				this->l = 0.0;
				this->p = 0.0;
				this->r = 0.0;
				this->q = 0.0;

				this->i = 0.0;
				this->j = 0.0;
				this->k = 0.0;

				this->x_specified = 0;
				this->y_specified = 0;
				this->z_specified = 0;

				this->a_specified = 0;
				this->b_specified = 0;
				this->c_specified = 0;

				this->u_specified = 0;
				this->v_specified = 0;
				this->w_specified = 0;

				this->i_specified = 0;
				this->j_specified = 0;
				this->k_specified = 0;

				this->l_specified = 0;
				this->p_specified = 0;
				this->q_specified = 0;
				this->r_specified = 0;

			} // End ResetForEndOfBlock() routine




			/**
				Initialize all nine variables from this base number to the value given.
			 */
			void InitOneCoordinateSystem( const int base, const double value )
			{
				for (int var=base; var<=base + 8; var++)
				{
					emc_variables[var] = value;
				}
			}

			void InitializeGCodeVariables()
			{
				InitOneCoordinateSystem(eG38_2VariableBase, 0.0);
				InitOneCoordinateSystem(eG28VariableBase, 0.0);
				InitOneCoordinateSystem(eG30VariableBase, 0.0);
				InitOneCoordinateSystem(eG92VariableBase, 0.0);

				// Coordinate system number (1 = G54, 2=G55 etc.)
				{
					emc_variables[eCoordinateSystemInUse] = 1.0;
				}


				const wxChar *not_used = _("Not Used");
				if (PROGRAM->m_emc2_variables_file_name != not_used)
				{
					FILE *fp = fopen(PROGRAM->m_emc2_variables_file_name.utf8_str(),"r");
					if (fp == NULL)
					{
						wxString error;
						error << _("Could not open ") << PROGRAM->m_emc2_variables_file_name << _(" for reading. Would you like to turn off the use of the EMC2 variables file?");
						int answer = wxMessageBox( error, _("EMC2 Variables File"), wxYES_NO );
						if (answer == wxYES)
						{
							PROGRAM->SetEmc2VariablesFileName( not_used );
						} // End if - then
					}
					else
					{
						char buf[1024];
						memset( buf, '\0', sizeof(buf) );
						while (fgets(buf, sizeof(buf)-1, fp) != NULL)
						{
							std::string line(buf);
							std::string::size_type offset=line.find_first_of("\t ");
							if (offset != std::string::npos)
							{
								std::string name = line.substr(0, offset);
								while (isspace(line[offset])) offset++;
								std::string value = line.substr(offset);

								emc_variables[name.c_str()] = atof(value.c_str());
							} // End if - then
						} // End while
						fclose(fp);
						fp = NULL;
					} // End if - then
				} // End if - then
			}

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