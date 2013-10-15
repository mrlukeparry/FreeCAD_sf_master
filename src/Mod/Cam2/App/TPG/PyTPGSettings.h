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

#ifndef PYTPGSETTINGS_H_
#define PYTPGSETTINGS_H_

#include <Python.h>
#include <QString>

#include "CXX/Objects.hxx"
#include "CXX/Extensions.hxx"

#include "TPGSettings.h"


#define QString_toPyString(__str__) PyString_FromString(((const char*)(__str__)).toStdString().c_str());
#define QStringPtr_toPyString(__str__) PyString_FromString(((const char*)(__str__))->toStdString().c_str());

extern PyTypeObject *PyTPGSettings_Type();
extern PyTypeObject *PyTPGSettingDefinition_Type();


#define PyCamTPGSettings_Check(__obj__) PyObject_TypeCheck((__obj__), PyTPGSettings_Type())
#define PyTPGSettingDefinition_Check(__obj__) PyObject_TypeCheck((__obj__), PyTPGSettingDefinition_Type())


// ----------------------------------------------------------------------------
// ----- TPGSettingDefinition -------------------------------------------------
// ----------------------------------------------------------------------------

/**
 * A wrapper for the TPGSettingDefinition class to allow access from PyTPGs
 */
typedef struct  {
    PyObject_HEAD
    Cam::Settings::Definition *setting;
} cam_PyTPGSettingDefinition;



// ----------------------------------------------------------------------------
// ----- TPGSettings ----------------------------------------------------------
// ----------------------------------------------------------------------------

/**
 * A wrapper for the TPGSettings class to allow access from PyTPGs
 */
typedef struct  {
    PyObject_HEAD
    Cam::Settings::TPGSettings *settings;
} cam_PyTPGSettings;

/**
 * Wrapper function to create new PyTPGSettings objects from c++
 */
extern PyObject* PyTPGSettings_New(Cam::Settings::TPGSettings* settings);


namespace Cam
{

class PySettingsOption: public Py::PythonClass< PySettingsOption >
{
public:
    PySettingsOption( Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds )
    : Py::PythonClass< PySettingsOption >::PythonClass( self, args, kwds )
    {
		QString id, label;

		Py::String _id = kwds.getItem("id");
		if (! _id.isNone())
		{
			id = QString::fromStdString(_id.as_std_string());
		}

		Py::String _label = kwds.getItem("label");
		if (! _label.isNone())
		{
			label = QString::fromStdString(_label.as_std_string());
		}

		this->pOption = std::auto_ptr<Cam::Settings::Option>(new Cam::Settings::Option(id, label));
		
        std::cout << "PySettingsOption c'tor Called with " << args.length() << " normal arguments." << std::endl;
        Py::List names( kwds.keys() );
        std::cout << "and with " << names.length() << " keyword arguments:" << std::endl;
        for( Py::List::size_type i=0; i< names.length(); i++ )
        {
            Py::String name( names[i] );
            std::cout << "    " << name << std::endl;
        }
    }

    virtual ~PySettingsOption()
    {
        std::cout << "~PySettingsOption." << std::endl;
		pOption.reset(NULL);
    }

    static void init_type(void)
    {
        behaviors().name( "PySettingsOption" );
		behaviors().doc( "Python wrapper for the Cam::Settings::Option class." );
        behaviors().supportGetattro();
        behaviors().supportSetattro();

        // Call to make the type ready for use
        behaviors().readyType();
    }

    Py::Object getattro( const Py::String &name_ )
    {
		if (pOption.get() == NULL) return genericGetAttro( name_ );

        std::string name( name_.as_std_string( "utf-8" ) );

		if( name == "id" )
        {
			return(Py::String(pOption->id.toStdString()));
        }
		else if( name == "label" )
        {
			return(Py::String(pOption->label.toStdString()));
        }
        else
        {
            return genericGetAttro( name_ );
        }
    }

    int setattro( const Py::String &name_, const Py::Object &value )
    {
        std::string name( name_.as_std_string( "utf-8" ) );

		if (pOption.get() == NULL) return(genericSetAttro(name_, value));

        if( name == "id" )
        {
			pOption->id = QString::fromStdString(Py::String(value).as_std_string());
            return 0;
        }
		else if( name == "label" )
        {
			pOption->label = QString::fromStdString(Py::String(value).as_std_string());
            return 0;
        }
        else
        {
            return genericSetAttro( name_, value );
        }
    }

private:
	std::auto_ptr<Cam::Settings::Option>	pOption;
};

class PySettingsDefinition: public Py::PythonClass< PySettingsDefinition >, Cam::Settings::Definition
{
public:
    PySettingsDefinition( Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds )
		: Py::PythonClass< PySettingsDefinition >::PythonClass( self, args, kwds ), Cam::Settings::Definition()
    {
        std::cout << "PySettingsDefinition c'tor Called with " << args.length() << " normal arguments." << std::endl;
        Py::List names( kwds.keys() );
        std::cout << "and with " << names.length() << " keyword arguments:" << std::endl;
        for( Py::List::size_type i=0; i< names.length(); i++ )
        {
            Py::String name( names[i] );
            std::cout << "    " << name << std::endl;
			this->setattro( Py::String(names[i]), kwds[names[i]] );
        }
    }

    virtual ~PySettingsDefinition()
    {
        std::cout << "~PySettingsDefinition." << std::endl;
    }

    static void init_type(void)
    {
        behaviors().name( "PySettingsDefinition" );
		behaviors().doc( "Python wrapper for the Cam::Settings::Definition class." );
        behaviors().supportGetattro();
        behaviors().supportSetattro();

        // Call to make the type ready for use
        behaviors().readyType();
    }



	





    Py::Object getattro( const Py::String &name_ )
    {
        std::string name( name_.as_std_string( "utf-8" ) );

        if( name == "name" )		return Py::String(Cam::Settings::Definition::name.toStdString());
		else if (name == "label") 	return Py::String(Cam::Settings::Definition::label.toStdString());
		else if (name == "type") 	return Py::Int(int(Cam::Settings::Definition::type));
		else if (name == "defaultvalue") 	return Py::String(Cam::Settings::Definition::defaultvalue.toStdString());
		else if (name == "units") 	return Py::String(Cam::Settings::Definition::units.toStdString());
		else if (name == "helptext") 	return Py::String(Cam::Settings::Definition::helptext.toStdString());
		else if (name == "visible") 	return Py::Boolean(Cam::Settings::Definition::visible);
		/*
		else if (name == "options")
		{
			Py::List pyOptions;
			for (QList<Cam::Settings::Option *>::const_iterator itOption = options.begin(); itOption != options.end(); itOption++)
			{
				PySettingsOption pyOption;
				pyOption->setattro( Py::String(std::string("id")), Py::String(itOption->id.toStdString()) );
				pyOption->setattro( Py::String(std::string("label")), Py::String(itOption->label.toStdString()) );
				pyOptions.append(pyOption);
			}
		}
		*/
        else
        {
            return genericGetAttro( name_ );
        }
    }

    int setattro( const Py::String &name_, const Py::Object &value )
    {
        std::string name( name_.as_std_string( "utf-8" ) );

        if( name == "name" )
		{
			Cam::Settings::Definition::name = QString::fromStdString(Py::String(value).as_std_string());
			return 0;
		}
		else if (name == "label")
		{
			Cam::Settings::Definition::label = QString::fromStdString(Py::String(value).as_std_string());
			return 0;
		}
		else if (name == "type")
		{
			Cam::Settings::Definition::type = SettingType(int(Py::Int(value).asLongLong()));
			return 0;
		}
		else if (name == "defaultvalue")
		{
			Cam::Settings::Definition::defaultvalue = QString::fromStdString(Py::String(value).as_std_string());
			return 0;
		}
		else if (name == "units")
		{
			Cam::Settings::Definition::units = QString::fromStdString(Py::String(value).as_std_string());
			return 0;
		}
		else if (name == "helptext")
		{
			Cam::Settings::Definition::helptext = QString::fromStdString(Py::String(value).as_std_string());
			return 0;
		}
		else if (name == "visible")
		{
			this->visible = (bool) Py::Boolean(value);
			return 0;
		}
		/*
		else if (name == "options")
		{
			for (QList<Option *>::iterator itOption = options.begin(); itOption != options.end(); itOption++)
			{
				delete *itOption;
			}
			options.clear();

			Py::List pyOptions(value);
			for (int i=0; i<pyOptions.size(); i++)
			{
				tingsOption pyOption(pyOptions[i]);
				
			}
		}
		*/
        else
        {
            return genericSetAttro( name_, value );
        }
    }

    Py::String id;
	Py::String label;
};


} // End namespace cam

#endif /* PYTPGSETTINGS_H_ */
