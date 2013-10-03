/***************************************************************************
 *   Copyright (c) 2013 Andrew Robinson <andrewjrobinson@gmail.com>        *
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

#ifndef CAMPROJECTDOCKWINDOWCOMPONENTS_H_
#define CAMPROJECTDOCKWINDOWCOMPONENTS_H_

#include <PreCompiled.h>

#include <QLineEdit>
#include <QFormLayout>
#include <QList>
#include <QWidget>
#include <QRadioButton>
#include <QMap>
#include <QPushButton>
#include <QToolButton>

#include "../App/TPG/TPGSettings.h"

namespace CamGui {

class CamGuiExport CamComponent {
protected:
    Cam::Settings::Definition *tpgsetting;
    QFormLayout* form;
    QList<QWidget*> rootComponents;

	/**
		The Validator class allows the QValidator mechanisms supported by the Qt library to ask
		the Cam::Settings::Definition class whether the value entered into the user interface
		is valid.  This allows the Cam::Settings::Definition::SettingType values to each implement
		their own validation rules without the need to link with the Qt library.
	 */
	class Validator : public QValidator
	{
	public:
		Validator( Cam::Settings::Definition *setting_definition, QObject *widget ) : QValidator(widget)
		{
			this->setting_definition = setting_definition->grab();
		}

		~Validator()
		{
			if (this->setting_definition)
			{
				this->setting_definition->release();
			}
		}

	virtual QValidator::State validate(QString &input, int &position) const;

	private:
		Cam::Settings::Definition *setting_definition;
	}; // End Validator class definition

	Validator *validator;

public:
    CamComponent();
    virtual ~CamComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

    /**
     * Get the name of this component's setting
     */
    const char* name() {
    	return tpgsetting->name.toStdString().c_str();
    }

	static QString FractionalRepresentation( const double original_value, const int max_denominator = 64 );
};

class CamLineEdit : public QLineEdit
{
public:
	CamLineEdit(QWidget *parent, Cam::Settings::Definition *tpgsetting) : QLineEdit(parent) 
	{
		this->tpgSetting = tpgsetting->grab();
	}

	~CamLineEdit()
	{
		if (this->tpgSetting)
		{
			this->tpgSetting->release();
		}
	}

	virtual void focusOutEvent ( QFocusEvent * e );

private:
	Cam::Settings::Definition *tpgSetting;
};

// ----- CamTextBoxComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::TextBox setting
 */
class CamGuiExport CamTextBoxComponent: public QObject, public CamComponent {

	Q_OBJECT

protected:
    CamLineEdit *widget;

public:

    CamTextBoxComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

public Q_SLOTS:

    /**
     * Slot to receive messages when the user changes the text value
     */
    void editingFinished();
	void handleButton();
	void handleAddObjectNameButton();
	void handleOKButton();

};



// ----- CamLengthComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::Length setting
 */
class CamGuiExport CamLengthComponent: public QObject, public CamComponent {

	Q_OBJECT

protected:
    CamLineEdit *camLineEdit;

public:

    CamLengthComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

public Q_SLOTS:

    /**
     * Slot to receive messages when the user changes the text value
     */
    void editingFinished();
	void currentIndexChanged(int index);

private:
	typedef QString Id_t;
	typedef QString Label_t;
	std::vector< std::pair< Id_t, Label_t > > values;

};



// ----- CamRateComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::Rate setting
 */
class CamGuiExport CamRateComponent: public QObject, public CamComponent {

	Q_OBJECT

protected:
    CamLineEdit *camLineEdit;

public:

    CamRateComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

public Q_SLOTS:

    /**
     * Slot to receive messages when the user changes the text value
     */
    void editingFinished();
	void currentIndexChanged(int index);

private:
	typedef QString Id_t;
	typedef QString Label_t;
	std::vector< std::pair< Id_t, Label_t > > values;

};



// ----- CamRadioComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::Radio setting
 */
class CamGuiExport CamRadioComponent: public CamComponent {
protected:
//    QList<QRadioButton *> radios;
    QMap<QString, QRadioButton*> radios;

public:

    CamRadioComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

};


// ----- CamRadioComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::Settings::Definition::SettingType_Enumeration setting
 */
class CamGuiExport CamComboBoxComponent: public QObject, public CamComponent {

	Q_OBJECT

public:

    CamComboBoxComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

public Q_SLOTS:

    /**
     * Slot to receive messages when the user changes the text value
     */
    void currentIndexChanged(int index);

private:
	typedef QString Id_t;
	typedef QString Label_t;
	std::vector< std::pair< Id_t, Label_t > > values;
};


// ----- CamFilenameComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::Filename setting
 */
class CamGuiExport CamFilenameComponent: public QObject, public CamComponent {

	Q_OBJECT

protected:
	QToolButton	*button;
    CamLineEdit *camLineEdit;

public:

    CamFilenameComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

private Q_SLOTS:

    /**
     * Slot to receive messages when the user changes the text value
     */
    void editingFinished();
	void handleButton();

};



// ----- CamDirectoryComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::Directory setting
 */
class CamGuiExport CamDirectoryComponent: public QObject, public CamComponent {

	Q_OBJECT

protected:
	QToolButton	*button;
    CamLineEdit *camLineEdit;

public:

    CamDirectoryComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

private Q_SLOTS:

    /**
     * Slot to receive messages when the user changes the text value
     */
    void editingFinished();
	void handleButton();

};




// ----- CamColorComponent ---------------------------------------------------------
/**
 * Object that manages a Cam::Directory setting
 */
class CamGuiExport CamColorComponent: public QObject, public CamComponent {

	Q_OBJECT

protected:
	QPushButton	*button;

public:

    CamColorComponent();

    /**
     * Creates the UI for this component and loads the initial value
     */
    virtual bool makeUI(Cam::Settings::Definition *tpgsetting, QFormLayout* form);

    /**
     * Saves the values on the UI to the TPGSetting instance
     */
    virtual bool close();

private Q_SLOTS:

    /**
     * Slot to receive messages when the user changes the text value
     */
	void handleButton();

};

} /* namespace Cam */
#endif /* CAMPROJECTDOCKWINDOWCOMPONENTS_H_ */
