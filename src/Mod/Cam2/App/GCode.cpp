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

typedef std::string::iterator Iterator;

template <typename Iterator>
struct argument_grammar : boost::spirit::qi::grammar<Iterator, std::pair<char, double>()>
{
    argument_grammar() : argument_grammar::base_type(d)
    {
        d = +boost::spirit::qi::lit('x') >> boost::spirit::qi::double_ >> *boost::spirit::qi::lit(' ');
    }
    boost::spirit::qi::rule<Iterator, std::pair<char, double>()> d;
};


template <typename Iterator>
struct point_double_grammar : boost::spirit::qi::grammar<Iterator, double()>
{
    point_double_grammar() : point_double_grammar::base_type(d)
    {
        d = *boost::spirit::qi::lit(' ') >> boost::spirit::qi::double_ >> *boost::spirit::qi::lit(' ');
    }
    boost::spirit::qi::rule<Iterator, double()> d;
};

int wilma()
{
	std::string input("x1.1");

	point_double_grammar<std::string::iterator> point_grammar;
	argument_grammar<std::string::iterator> arguments;

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