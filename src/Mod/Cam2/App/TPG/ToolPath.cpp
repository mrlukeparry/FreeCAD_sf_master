/***************************************************************************
 *   Copyright (c) 2012 Andrew Robinson <andrewjrobinson@gmail.com>        *
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
#include <cstring>
#endif

#include "ToolPath.h"
#include "Area.h"


namespace Cam {


ToolPath::ToolPath(TPG* source) {
    this->source = source;
    this->toolpath = new QStringList();
    refcnt = 1;
	required_decimal_places = 3;	// TODO: Implement this properly.  Assume metric for now.
}

ToolPath::~ToolPath() {
    if (this->toolpath != NULL)
        delete this->toolpath;
}

/**
 * Add a single toolpath command to the ToolPath
 */
void ToolPath::addToolPath(QString tp) {
    if (this->toolpath == NULL)
        this->toolpath = new QStringList();
    this->toolpath->push_back(tp);
}

/**
 * Clear out the toolpath.
 */
void ToolPath::clear() {
    if (this->toolpath != NULL)
        this->toolpath->clear();
    else
        this->toolpath = new QStringList();
}

/**
 * Get the TPG that created this toolpath
 */
TPG *ToolPath::getSource() {
    return this->source;
}

/**
 * Get the Toolpath as strings
 */
QStringList *ToolPath::getToolPath() {
    return this->toolpath;
}





/**
	When a string is passed into a Python routine, it needs to be surrounded by
	single (or double) quote characters.  If the contents of the string contain
	backslash characters then these may be interpreted by Python as part of
	a special character representation.  eg: '\n' represents a single newline
	character.  If we pass a Windows path in then the backslash characters that
	separate directory names may be interpreted rather than be taken literaly.
	This routine also adds the single quotes to the beginning and end of the
	string passed in.

	eg: if value = "abcde" then the returned string would be 'abcde'
	    if value = "c:\temp\myfile.txt" then the returned string would be 'c:\\temp\\myfile.txt'
		if value = "abc'de" then the returned string would be 'abc\'de'.
 */
QString ToolPath::PythonString( const QString value ) const
{
	QString _value(value);
	QString result;
	if (_value.startsWith(QString::fromAscii("\'")) || (_value.startsWith(QString::fromAscii("\""))))
	{
		_value.remove(0, 1);
	}

	if ((_value.endsWith(QString::fromAscii("\'"))) || (_value.endsWith(QString::fromAscii("\""))))
	{
		_value.remove(_value.length()-1, 1);
	}

	_value.replace(QString::fromAscii("\\"), QString::fromAscii("\\\\") );
	_value.replace(QString::fromAscii("\'"), QString::fromAscii("\\\'") );
	_value.replace(QString::fromAscii("\""), QString::fromAscii("\\\"") );
	_value.replace(QString::fromAscii("("), QString::fromAscii("{") );
	_value.replace(QString::fromAscii(")"), QString::fromAscii("}") );

	result = QString::fromAscii("\'") + _value + QString::fromAscii("\'");
	return(result);
}

QString ToolPath::PythonString( const char *value ) const
{
	return(PythonString(QString::fromUtf8(value)));
}

QString ToolPath::PythonString( const double value ) const
{
	double rounded_value = Round(value,RequiredDecimalPlaces());
	int field_width = 0;
	char format = 'g';
	int precision = Precision(rounded_value);

	return(QString(QString::fromAscii("%1")).arg(rounded_value, field_width, format, precision));
}


double ToolPath::round(double r) const {
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

double ToolPath::Round(double number,int place) const
{
	/*place = decimal point. putting in 0 will make it round to whole number. putting in 1 will round to the tenths digit.*/

	number *= pow(double(10.0),place);
	int istack = (int)floor(number);
	double out = number-istack;

	if (out < 0.5)
	{
		number = floor(number);
		number /= pow(double(10.0),place);
		return number;
	}
	if (out > 0.4) 
	{
		number = ceil(number);
		number /= pow(double(10.0),place);
		return number;
	}
	return(number);
}

// How many characters required for both the digits left of the decimal point, the decimal
// point, the +/- sign character and those digits required after the decimal point.
// It's used for rounding floating point numbers during the conversion to string format.

unsigned int ToolPath::Precision( const double value ) const
{
    unsigned int lhs = 0;
    unsigned int one_for_plus_minus_sign = 1;

    lhs = 0;
    double temp = fabs(value);
    while (temp >= 1.0)
    {
        lhs++;
        temp /= 10.0;
    }

    return( lhs + RequiredDecimalPlaces() + one_for_plus_minus_sign );
}





ToolPath & ToolPath::operator<<( const double value )
{
	this->line_buffer.append(PythonString(value));
	if (this->line_buffer.endsWith(QString::fromAscii("\n")))
	{
		if (this->toolpath == NULL)
			this->toolpath = new QStringList();

		this->toolpath->push_back(this->line_buffer);
		this->line_buffer.clear();
	}

	return(*this);
}

ToolPath & ToolPath::operator<< ( const ToolPath & value )
{
	if (this->toolpath == NULL)
        this->toolpath = new QStringList();

	if (value.toolpath)
	{
		std::copy( value.toolpath->begin(), value.toolpath->end(), std::inserter( *(this->toolpath), this->toolpath->begin() ) );
	}

	return(*this);
}


ToolPath & ToolPath::operator<< ( const QString value )
{
	this->line_buffer.append(value);
	if (this->line_buffer.endsWith(QString::fromAscii("\n")))
	{
		if (this->toolpath == NULL)
			this->toolpath = new QStringList();

		this->toolpath->push_back(this->line_buffer);
		this->line_buffer.clear();
	}
	return(*this);
}

ToolPath & ToolPath::operator<< ( const char *value )
{
	return( (*this) << QString::fromAscii(value) );
}


ToolPath & ToolPath::operator<< ( const int value )
{
	this->line_buffer.append(QString::fromAscii("%1").arg(value));
	if (this->line_buffer.endsWith(QString::fromAscii("\n")))
	{
		if (this->toolpath == NULL)
			this->toolpath = new QStringList();

		this->toolpath->push_back(this->line_buffer);
		this->line_buffer.clear();
	}
	return(*this);
}


/* friend */ QString operator<< ( QString & buf, const ToolPath & tool_path )
{
	if (tool_path.toolpath)
	{
		for (QStringList::const_iterator itString = tool_path.toolpath->begin(); itString != tool_path.toolpath->end(); itString++)
		{
			buf.append(*itString);
			buf.append(QString::fromAscii("\n"));
		}
	}
	return(buf);
}


const unsigned int ToolPath::RequiredDecimalPlaces() const
{
	// Look for this as one of the TPG settings.  If it's not
	// found then look for the units used by this TPG.  Finally, just
	// assume metric and use 3+1 decimal places.
	// TODO - implement this functionality.

	return(required_decimal_places);
}

void ToolPath::RequiredDecimalPlaces(const unsigned int value)
{
	this->required_decimal_places = value;
	area::CArea::m_accuracy = 1.0 / pow(10.0, double(value));
	area::Point::tolerance = 1.0 / pow(10.0, double(value));
}


} /* namespace Cam */
