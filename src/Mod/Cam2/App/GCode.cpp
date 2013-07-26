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

// #include <boost/spirit/include/qi.hpp>
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


// double mysub(boost::spirit::_1_type a, boost::spirit::_2_type b)
double mysub(char a, double b)
{
	qDebug("%c %lf\n", a, b);
	return(b);
}

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

typedef qi::symbols<char, double> arguments_dictionary;

qi::rule<std::string::const_iterator, qi::space_type> GetRapid()
{
	return(qi::rule<std::string::const_iterator, qi::space_type>(qi::lexeme [+qi::char_("gG") >> *qi::char_("0") >> qi::char_("0")] >> (qi::lexeme [+qi::char_("xXyYzZ")] >> qi::double_)));
}

struct Arguments_t
{
	Arguments_t()
	{
	}

	~Arguments_t()
	{
		
	}

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

	void EndOfBlock()
	{
		int j=3;
	}

private:
	typedef std::map<char, double>	DoubleMap_t;
	DoubleMap_t	m_doubles;
};

template <typename Iter, typename Skipper = qi::space_type> 
	struct rs274 : qi::grammar<Iter, Skipper> 
{
	rs274(arguments_dictionary &dict) : rs274::base_type(Start)
	{
		// Variables declared here are created and destroyed for each rule parsed.
		// Use member variables of the structure for long-lived variables instead.

		// arguments_dictionary arguments;

		line_number = 0;

		// N110 - i.e. use qi::lexeme to avoid qi::space_type skipper which allows the possibility of interpreting 'N 110'.  We don't want spaces here.
		LineNumberRule = (qi::lexeme [qi::char_("nN") ] >> qi::int_ )
			[ phx::ref(line_number) = qi::_2 ];

		MotionArguments = (qi::lexeme [+qi::char_("xXyYzZ")] >> qi::double_)
			[ phx::bind(&Arguments_t::Add, phx::ref(arguments), qi::_1, qi::_2) ] // call arguments.Add(_1, _2);
			// [ phx::bind(dict.add, qi::_1, qi::_2) ]
			;

		// g00 X <float> Y <float> etc.
		Rapid = qi::lexeme [+qi::char_("gG") >> *qi::char_("0") >> qi::char_("0")] >> +(MotionArguments)
			[ phx::bind(&Arguments_t::Print, phx::ref(arguments) ) ]	// call arguments.Print()
			;

		// g01 X <float> Y <float> etc.
		Feed = qi::lexeme [+qi::char_("gG") >> *qi::char_("0") >> qi::char_("1")] >> +(MotionArguments)
			[ phx::bind(&Arguments_t::Print, phx::ref(arguments) ) ]	// call arguments.Print()
			// [ phx::bind(&Arguments_t::Print, arguments) ]
			;

		EndOfBlock = qi::lit("\n")
			[ phx::bind(&Arguments_t::EndOfBlock, phx::ref(arguments) ) ]	// call arguments.EndOfBlock()
			;

		MotionCommand =	
					LineNumberRule >> Feed
				|	LineNumberRule >> Rapid
					;

		Start = MotionCommand	// [ phx::bind(&Arguments_t::Print, phx::ref(arguments) ) ]
					;
	}

	public:
		qi::rule<Iter, Skipper> Start;
		qi::rule<Iter, Skipper> MotionArguments;
		qi::rule<Iter, Skipper> Feed;
		qi::rule<Iter, Skipper> Rapid;		
		qi::rule<Iter, Skipper> MotionCommand;
		qi::rule<Iter, Skipper> LineNumberRule;
		qi::rule<Iter, Skipper> EndOfBlock;

		Arguments_t	arguments;
		int			line_number;
};

int CamExport wilma()
{

	// from http://stackoverflow.com/questions/12208705/add-to-a-spirit-qi-symbol-table-in-a-semantic-action
	// and http://stackoverflow.com/questions/9139015/parsing-mixed-values-and-key-value-pairs-with-boost-spirit



	arguments_dictionary arguments;
	rs274<std::string::const_iterator> linuxcnc(arguments);
	const std::string test = "N220 g01 X 1.1 Y 2.2 Z3.3   ";
	std::string::const_iterator begin = test.begin();
	if (qi::phrase_parse(begin, test.end(), linuxcnc, qi::space))
	{
		if (arguments.find("X")) qDebug("%lf\n", arguments.at("X"));
		if (arguments.find("Y")) qDebug("%lf\n", arguments.at("Y"));
		if (arguments.find("Z")) qDebug("%lf\n", arguments.at("Z"));
	}
	else
	{
		qDebug("Parsing failed at N%d %d, %s\n", linuxcnc.line_number, std::distance(test.begin(), begin), begin);
	}
/*
	// from http://stackoverflow.com/questions/3066701/boost-spirit-semantic-action-parameters

	std::string input("X1.1");
   std::string::const_iterator begin = input.begin(), end = input.end();

   typedef double ArgumentRuleReturn_t;
   qi::rule<
      std::string::const_iterator,
      ArgumentRuleReturn_t(double),                    //output (_val) and input (_r1)
      qi::locals<double>,             //local int (_a)
      ascii::space_type
   >
      ArgumentRule =
            (qi::char_("xXyYzZ") >> qi::double_)
			[
				qi::_val = mysub(qi::as<char>(qi::_1), qi::as<double>(qi::_2))
			];

   double ruleValue, x0 = 10;
   qi::phrase_parse(begin, end, ArgumentRule(x0), ascii::space, ruleValue);
   std::cout << "rule value: " << ruleValue << std::endl;
	*/


   /*
   std::string input("1234, 6543, 42");
   std::string::const_iterator begin = input.begin(), end = input.end();

   qi::rule<
      std::string::const_iterator,
      int(int),                    //output (_val) and input (_r1)
      qi::locals<int>,             //local int (_a)
      ascii::space_type
   >
      intRule =
            (qi::int_ >> "," >> qi::int_ >> "," >> qi::int_)
			[
				qi::_val = (qi::_1 + qi::_2) * qi::_3 + qi::_r1
			];

   int ruleValue, x0 = 10;
   qi::phrase_parse(begin, end, intRule(x0), ascii::space, ruleValue);
   std::cout << "rule value: " << ruleValue << std::endl;
   */


   /*
	std::string input("x1.1");

	point_double_grammar<std::string::iterator> point_grammar;
	// argument_grammar<std::string::iterator> arguments;

    bool result = false;
    double d = 0.0;
    std::string p1 = "575040.3400";
    std::string p2 = "117380.1200";
    std::string p3 = "-001.22916765";
    std::string p4 = "063.39171738";
    std::string p5 = "2.5";

    std::string::iterator it;
    std::string::iterator last;

    it = p1.begin();
    last = p1.end();
    result = (boost::spirit::qi::parse(it, last, point_grammar, d) && it ==
            last);
    if(result)
    {
        std::cout << p1 << " == " << d << std::endl;
    }
    else
    {
        std::cout << "Parsing failed!" << std::endl;
    }

    it = p2.begin();
    last = p2.end();
    result = (boost::spirit::qi::parse(it, last, point_grammar, d) && it ==
            last);
    if(result)
    {
        std::cout << p2 << " == " << d << std::endl;
    }
    else
    {
        std::cout << "Parsing failed!" << std::endl;
    }

    it = p3.begin();
    last = p3.end();
    result = (boost::spirit::qi::parse(it, last, point_grammar, d) && it == last);
    if(result)
    {
        std::cout << p3 << " == " << d << std::endl;
    }
    else
    {
        std::cout << "Parsing failed!" << std::endl;
    }

    it = p4.begin();
    last = p4.end();
    result = (boost::spirit::qi::parse(it, last, point_grammar, d) && it == last);
    if(result)
    {
        std::cout << p4 << " == " << d << std::endl;
    }
    else
    {
        std::cout << "Parsing failed!" << std::endl;
    }

    it = p5.begin();
    last = p5.end();
    result = (boost::spirit::qi::parse(it, last, point_grammar, d) && it == last);
    if(result)
    {
        std::cout << p5 << " == " << d << std::endl;
    }
    else
    {
        std::cout << "Parsing failed!" << std::endl;
    }
	*/


	// http://climbing-the-hill.blogspot.com.au/2010/05/boost-your-spirits.html


	// from http://4thmouse.com/index.php/2008/07/20/boost-spirit-part-ii-attaching-actions-to-your-grammar/


	// from http://www.codeproject.com/Articles/8516/An-Introduction-to-the-Boost-Spirit-Parser-framewo

	/*
	struct Syntax :
		public boost::spirit::grammar<Syntax>
	{
	public:
		Syntax( CParser &parser );
		virtual ~Syntax();
		template <typename ScannerT>
		struct definition
		{
		public:
			definition( Syntax const &self )
			{
				integer =
					lexeme_d[ (+digit_p) ]
					;
				factor =
					integer |
					vars |
					'(' >> expression >> ')' |
					( '-' >> factor ) |
					( '+' >> factor )
					;
				term =
					factor >> *( ( '*' >> factor) | ( '/' >> factor ) )
					;
				expression =
					term >> *( ( '+' >> term ) | ( '-' >> term ) )
					;
				assignment =
					vars
					>> '=' >> expression
					;
				var_decl =
					lexeme_d
					[
						( ( alpha_p >> *( alnum_p | '_' ) )
						- vars )[vars.add]
					]
					;
				declaration =
					"int" >> var_decl >> *( ',' >> var_decl )
					;
				baseExpression =
					str_p( "exit" )[*self.m_finishedProcessing] |
					str_p( "mod" ) >> integer |
					declaration |
					assignment |
					'?' >> expression
					;
			}
			boost::spirit::symbols<int> vars;
			boost::spirit::rule<ScannerT> integer, factor, term,
				expression, assignment, var_decl, declaration,
				baseExpression;
			const boost::spirit::rule<ScannerT> &start() const { return baseExpression; }
		};
		friend struct definition;
	private:
		Private::FinishedProcessing *m_finishedProcessing;
	};
	*/

	return(0);
}