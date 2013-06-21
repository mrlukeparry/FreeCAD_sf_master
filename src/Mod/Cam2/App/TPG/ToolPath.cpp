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

/* static */ unsigned int ToolPath::required_decimal_places = 5;

ToolPath::ToolPath(TPG* source) {
    this->source = source;
    this->toolpath = new QStringList();
    refcnt = 1;
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

	result = QString::fromAscii("\'") + _value + QString::fromAscii("\'");
	return(result);
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


QString ToolPath::PythonString( const double value ) const
{
	double rounded_value = Round(value,ToolPath::RequiredDecimalPlaces());
	int field_width = 0;
	char format = 'g';
	int precision = ToolPath::required_decimal_places;

	return(QString().arg(rounded_value, field_width, format, precision));
}

ToolPath & ToolPath::operator<<( const double value )
{
	if (this->toolpath == NULL)
        this->toolpath = new QStringList();
	this->toolpath->push_back(PythonString(value));
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
	if (this->toolpath == NULL)
        this->toolpath = new QStringList();
	this->toolpath->push_back(value);
	return(*this);
}


ToolPath & ToolPath::operator<< ( const int value )
{
	if (this->toolpath == NULL)
        this->toolpath = new QStringList();
	this->toolpath->push_back(QString().arg(value));

	return(*this);

}






} /* namespace Cam */
