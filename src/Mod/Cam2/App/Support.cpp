/***************************************************************************
 *   Copyright (c) 2012 Andrew Robinson  (andrewjrobinson@gmail.com)       *
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


#include "Support.h"

/**
 * Convert a QString to a Python Object
 */
PyObject *QStringToPythonUC(const QString &str)
{
  //TODO: check this works properly, it appears to work with the UCS4 characters I tested
  return PyString_FromString((const char*) str.toStdString().c_str());
}

/**
 * Convert a Python Object (string/unicode) to a QString
 */
QString PythonUCToQString(PyObject *obj)
{
  Py_UNICODE *pid = PyUnicode_AsUnicode(obj);

  if (pid != NULL)
    return QString::fromUcs4((const uint *) pid); //TODO: this should have a check for older pythons that use Ucs2)
  if (PyString_Check(obj))
    return QString::fromLatin1(PyString_AS_STRING(obj));
  return QString();
}

///**
// * Convert a QString to a const char *
// */
//inline const char* ts(QString str)
//{
//  return str.toAscii().constData();
//}
//inline const char* ts(QString *str)
//{
//  if (str != NULL)
//    return str->toAscii().constData();
//  return "NULL";
//}



