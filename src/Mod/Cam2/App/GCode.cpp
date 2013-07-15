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



void callback(char c, double f)
{
	int = 3;
}

std::vector<double> doubles;
void add_argument(double const &value)
{
	doubles.push_back(value);
}

int wilma()
{
	std::string input("x1.1");

	typedef boost::spirit::function2<void, char, double> Callback_t;
	typedef std::string::iterator Iterator;

	boost::spirit::qi::rule<Iterator, char, double>()> arguments;
	arguments = boost::spirit::qi::char_('x') >> boost::spirit::qi::double_;

	boost::spirit::qi::parse(input.begin(), input.end(), arguments );


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