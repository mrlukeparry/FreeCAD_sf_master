/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
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

#include <PreCompiled.h>
#ifndef _PreComp_
#endif

#include <boost/bind.hpp>

#include <App/Application.h>
#include <App/Document.h>
#include <App/PropertyContainer.h>

#include <Base/Console.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "ToolFeature.h"

using namespace Cam;

ToolFeature::tap_sizes_t metric_tap_sizes[] = {
      {QString::fromAscii("M1 x 0.25 mm coarse"), 1.0, 0.25},
      {QString::fromAscii("M1.2 x 0.25 mm coarse"), 1.2, 0.25},
      {QString::fromAscii("M1.4 x 0.3 mm coarse"), 1.3, 0.3},
      {QString::fromAscii("M1.6 x 0.35mm coarse"), 1.6, 0.35},
      {QString::fromAscii("M1.8 x 0.35mm coarse"), 1.8, 0.35},
      {QString::fromAscii("M2 x 0.4 mm coarse"), 2.0, 0.4},
      {QString::fromAscii("M2.5 x 0.45 mm coarse"), 2.5, 0.45},
      {QString::fromAscii("M3 x 0.5 mm coarse"), 3.0, 0.5},
      {QString::fromAscii("M3.5 x 0.6 mm coarse"), 3.5, 0.6},
      {QString::fromAscii("M4 x 0.7 mm coarse"), 4.0, 0.7},
      {QString::fromAscii("M5 x 0.8 mm coarse"), 5.0, 0.8},
      {QString::fromAscii("M6 x 1 mm coarse"), 6.0, 1.0},
      {QString::fromAscii("M7 x 1 mm coarse"), 7.0, 1.0},
      {QString::fromAscii("M8 x 1.25mm coarse"), 8.0, 1.25},
      {QString::fromAscii("M8 x 1 mm fine"), 8.0, 1.0},
      {QString::fromAscii("M10 x 1.5 mm coarse"), 10.0, 1.5},
      {QString::fromAscii("M10 x 1 mm fine"), 10.0, 1.0},
      {QString::fromAscii("M10 x 1.25mm fine"), 10.0, 1.25},
      {QString::fromAscii("M12 x 1.75mm coarse"), 12.0, 1.75},
      {QString::fromAscii("M12 x 1.5 mm fine"), 12.0, 1.5},
      {QString::fromAscii("M12 x 1.25mm fine"), 12.0, 1.25},
      {QString::fromAscii("M14 x 1.5 mm fine"), 14.0, 1.5},
      {QString::fromAscii("M14 x 2 mm coarse"), 14.0, 2.0},
      {QString::fromAscii("M16 x 2 mm coarse"), 16.0, 2.0},
      {QString::fromAscii("M16 x 1.5 mm fine"), 16.0, 1.5},
      {QString::fromAscii("M18 x 2.5 mm coarse"), 18.0, 2.5},
      {QString::fromAscii("M18 x 2 mm fine"), 18.0, 2.0},
      {QString::fromAscii("M18 x 1.5 mm fine"), 18.0, 1.5},
      {QString::fromAscii("M20 x 2.5 mm coarse"), 20.0, 2.5},
      {QString::fromAscii("M20 x 2 mm fine"), 20.0, 2.0},
      {QString::fromAscii("M20 x 1.5 mm fine"), 20.0, 1.5},
      {QString::fromAscii("M30 x 3.5 mm coarse"), 30.0, 3.5},
      {QString::fromAscii("M30 x 2 mm fine"), 30.0, 2.0},
      {QString::fromAscii("M33 x 3.5 mm coarse"), 33.0, 3.5},
      {QString::fromAscii("M33 x 2 mm fine"), 33.0, 2.0},
      {QString::fromAscii("M36 x 4 mm coarse"), 36.0, 4.0},
      {QString::fromAscii("M36 x 3 mm fine"), 36.0, 3.0},
      {QString::fromAscii("M39 x 4 mm coarse"), 39.0, 4.0},
      {QString::fromAscii("M39 x 3 mm fine"), 39.0, 3.0},
      {QString::fromAscii("M42 x 4.5 mm coarse"), 42.0, 4.5},
      {QString::fromAscii("M42 x 3 mm fine"), 42.0, 3.0},
      {QString::fromAscii("M45 x 4.5 mm coarse"), 45.0, 4.5},
      {QString::fromAscii("M45 x 3 mm fine"), 45.0, 3.0},
      {QString::fromAscii("M48 x 5 mm coarse"), 48.0, 5.0},
      {QString::fromAscii("M48 x 3 mm fine"), 48.0, 3.0},
      {QString::fromAscii("M52 x 5 mm coarse"), 52.0, 5.0},
      {QString::fromAscii("M52 x 4 mm fine"), 52.0, 4.0},
      {QString::fromAscii("M56 x 5.5 mm coarse"), 56.0, 5.5},
      {QString::fromAscii("M56 x 4 mm fine"), 56.0, 4.0},
      {QString::fromAscii("M60 x 5.5 mm coarse"), 60.0, 5.5},
      {QString::fromAscii("M60 x 4 mm fine"), 60.0, 4.0},
      {QString::fromAscii("M64 x 6 mm coarse"), 64.0, 6.0},
      {QString::fromAscii("M64 x 4 mm fine"), 64.0, 4.0}
    };

ToolFeature::tap_sizes_t unified_thread_standard_tap_sizes[] = {
      {QString::fromAscii("#0 = 0.060 x 80 UNF"), 0.06 * 25.4, 25.4 / 80},
      {QString::fromAscii("#1 = 0.073 x 64 UNC"), 0.073 * 25.4, 25.4 / 64},
      {QString::fromAscii("#1 = 0.073 x 72 UNF"), 0.073 * 25.4, 25.4 / 72},
      {QString::fromAscii("#2 = 0.086 x 56 UNC"), 0.086 * 25.4, 25.4 / 56},
      {QString::fromAscii("#2 = 0.086 x 64 UNF"), 0.086 * 25.4, 25.4 / 64},
      {QString::fromAscii("#3 = 0.099 x 48 UNC"), 0.099 * 25.4, 25.4 / 48},
      {QString::fromAscii("#3 = 0.099 x 56 UNF"), 0.099 * 25.4, 25.4 / 56},
      {QString::fromAscii("#4 = 0.112 x 40 UNC"), 0.112 * 25.4, 25.4 / 40},
      {QString::fromAscii("#4 = 0.112 x 48 UNF"), 0.112 * 25.4, 25.4 / 48},
      {QString::fromAscii("#5 = 0.125 x 40 UNC"), 0.125 * 25.4, 25.4 / 40},
      {QString::fromAscii("#5 = 0.125 x 44 UNF"), 0.125 * 25.4, 25.4 / 44},
      {QString::fromAscii("#6 = 0.138 x 32 UNC"), 0.138 * 25.4, 25.4 / 32},
      {QString::fromAscii("#6 = 0.138 x 40 UNF"), 0.138 * 25.4, 25.4 / 40},
      {QString::fromAscii("#8 = 0.164 x 32 UNC"), 0.164 * 25.4, 25.4 / 32},
      {QString::fromAscii("#8 = 0.164 x 36 UNF"), 0.164 * 25.4, 25.4 / 36},
      {QString::fromAscii("#10= 0.190 x 24 UNC"), 0.190 * 25.4, 25.4 / 24},
      {QString::fromAscii("#10= 0.190 x 32 UNF"), 0.190 * 25.4, 25.4 / 32},
      {QString::fromAscii("#12= 0.216 x 24 UNC"), 0.216 * 25.4, 25.4 / 24},
      {QString::fromAscii("#12= 0.216 x 28 UNC"), 0.216 * 25.4, 25.4 / 28},
      {QString::fromAscii("#12= 0.216 x 32 UNEF"), 0.216 * 25.4, 25.4 / 32},
      {QString::fromAscii("1/4  x 20 UNC"), 0.25 * 25.4, 25.4 / 20},
      {QString::fromAscii("1/4  x 28 UNF"), 0.25 * 25.4, 25.4 / 28},
      {QString::fromAscii("1/4  x 32 UNEF"), 0.25 * 25.4, 25.4 / 32},
      {QString::fromAscii("5/16 x 18 UNC"), (5.0/16.0) * 25.4, 25.4 / 18},
      {QString::fromAscii("5/16 x 24 UNF"), (5.0/16.0) * 25.4, 25.4 / 24},
      {QString::fromAscii("5/16 x 32 UNEF"), (5.0/16.0) * 25.4, 25.4 / 32},
      {QString::fromAscii("3/8 x 16 UNC"), (3.0/8.0) * 25.4, 25.4 / 16},
      {QString::fromAscii("3/8 x 24 UNF"), (3.0/8.0) * 25.4, 25.4 / 24},
      {QString::fromAscii("3/8 x 32 UNEF"), (3.0/8.0) * 25.4, 25.4 / 32},
      {QString::fromAscii("7/16 x 14 UNC"), (7.0/16.0) * 25.4, 25.4 / 14},
      {QString::fromAscii("7/16 x 20 UNF"), (7.0/16.0) * 25.4, 25.4 / 20},
      {QString::fromAscii("7/16 x 28 UNEF"), (7.0/16.0) * 25.4, 25.4 / 28},
      {QString::fromAscii("1/2 x 13 UNC"), (1.0/2.0) * 25.4, 25.4 / 13},
      {QString::fromAscii("1/2 x 20 UNF"), (1.0/2.0) * 25.4, 25.4 / 20},
      {QString::fromAscii("1/2 x 28 UNEF"), (1.0/2.0) * 25.4, 25.4 / 28},
      {QString::fromAscii("9/16 x 12 UNC"), (9.0/16.0) * 25.4, 25.4 / 12},
      {QString::fromAscii("9/16 x 18 UNF"), (9.0/16.0) * 25.4, 25.4 / 18},
      {QString::fromAscii("9/16 x 24 UNEF"), (9.0/16.0) * 25.4, 25.4 / 24},
      {QString::fromAscii("5/8 x 11 UNC"), (5.0/8.0) * 25.4, 25.4 / 11},
      {QString::fromAscii("5/8 x 18 UNF"), (5.0/8.0) * 25.4, 25.4 / 18},
      {QString::fromAscii("5/8 x 24 UNEF"), (5.0/8.0) * 25.4, 25.4 / 24},
      {QString::fromAscii("3/4 x 10 UNC"), (3.0/4.0) * 25.4, 25.4 / 10},
      {QString::fromAscii("3/4 x 16 UNF"), (3.0/4.0) * 25.4, 25.4 / 16},
      {QString::fromAscii("3/4 x 20 UNEF"), (3.0/4.0) * 25.4, 25.4 / 20},
      {QString::fromAscii("7/8 x 9 UNC"), (7.0/8.0) * 25.4, 25.4 / 9},
      {QString::fromAscii("7/8 x 14 UNF"), (7.0/8.0) * 25.4, 25.4 / 14},
      {QString::fromAscii("7/8 x 20 UNEF"), (7.0/8.0) * 25.4, 25.4 / 20},
      {QString::fromAscii("1 x 8 UNC"), 1.0 * 25.4, 25.4 / 8},
      {QString::fromAscii("1 x 14 UNF"), 1.0 * 25.4, 25.4 / 14},
      {QString::fromAscii("1 x 20 UNEF"), 1.0 * 25.4, 25.4 / 20}
    };

ToolFeature::tap_sizes_t british_standard_whitworth_tap_sizes[] = {
      {QString::fromAscii("1/16 x 60 BSW"), (1.0/16.0) * 25.4, 25.4 / 60},
      {QString::fromAscii("3/32 x 48 BSW"), (3.0/32.0) * 25.4, 25.4 / 48},
      {QString::fromAscii("1/8  x 48 BSW"), (1.0/8.0) * 25.4, 25.4 / 40},
      {QString::fromAscii("5/32 x 32 BSW"), (5.0/32.0) * 25.4, 25.4 / 32},
      {QString::fromAscii("3/16 x 24 BSW"), (3.0/16.0) * 25.4, 25.4 / 24},
      {QString::fromAscii("7/32 x 24 BSW"), (7.0/32.0) * 25.4, 25.4 / 24},
      {QString::fromAscii("1/4 x 20 BSW"), (1.0/4.0) * 25.4, 25.4 / 20},
      {QString::fromAscii("5/16 x 18 BSW"), (5.0/16.0) * 25.4, 25.4 / 18},
      {QString::fromAscii("3/8 x 16 BSW"), (3.0/8.0) * 25.4, 25.4 / 16},
      {QString::fromAscii("7/16 x 14 BSW"), (7.0/16.0) * 25.4, 25.4 / 14},
      {QString::fromAscii("1/2 x 12 BSW"), (1.0/2.0) * 25.4, 25.4 / 12},
      {QString::fromAscii("9/16 x 12 BSW"), (9.0/16.0) * 25.4, 25.4 / 12},
      {QString::fromAscii("5/8 x 11 BSW"), (5.0/8.0) * 25.4, 25.4 / 11},
      {QString::fromAscii("11/16 x 11 BSW"), (11.0/16.0) * 25.4, 25.4 / 11},
      {QString::fromAscii("3/4 x 10 BSW"), (3.0/4.0) * 25.4, 25.4 / 10},
      {QString::fromAscii("13/16 x 10 BSW"), (13.0/16.0) * 25.4, 25.4 / 10},
      {QString::fromAscii("7/8 x 9 BSW"), (7.0/8.0) * 25.4, 25.4 / 9},
      {QString::fromAscii("15/16 x 9 BSW"), (15.0/16.0) * 25.4, 25.4 / 9},
      {QString::fromAscii("1 x 8 BSW"), 1.0 * 25.4, 25.4 / 8},
      {QString::fromAscii("1 1/8 x 7 BSW"), (1.0 + (1.0/8.0)) * 25.4, 25.4 / 7},
      {QString::fromAscii("1 1/4 x 7 BSW"), (1.0 + (1.0/4.0)) * 25.4, 25.4 / 7},
      {QString::fromAscii("1 1/2 x 6 BSW"), (1.0 + (1.0/2.0)) * 25.4, 25.4 / 6},
      {QString::fromAscii("1 3/4 x 5 BSW"), (1.0 + (3.0/4.0)) * 25.4, 25.4 / 5},
      {QString::fromAscii("2 x 4.5 BSW"), 2.0 * 25.4, 25.4 / 4.5}
    };


// Standard sizes (by name)
ToolFeature::centre_drill_t centre_drill_sizes[] =
{
	{ QString::fromAscii("#00"), (1.0 / 8.0) * 25.4,   0.025 * 25.4,        0.030 * 25.4,        (25.4 * 1.0) + ((1.0 / 8.0) * 25.4) },
	{ QString::fromAscii("#0"),  (1.0 / 8.0) * 25.4,   (1.0 / 32.0) * 25.4, 0.038 * 25.4,        (25.4 * 1.0) + ((1.0 / 8.0) * 25.4) },
	{ QString::fromAscii("#1"),  (1.0 / 8.0) * 25.4,   (3.0 / 64.0) * 25.4, (3.0 / 64.0) * 25.4, (25.4 * 1.0) + ((1.0 / 4.0) * 25.4) },
	{ QString::fromAscii("#2"),  (3.0 / 16.0) * 25.4,  (5.0 / 64.0) * 25.4, (5.0 / 64.0) * 25.4, (25.4 * 1.0) + ((7.0 / 8.0) * 25.4) },
	{ QString::fromAscii("#3"),  (1.0 / 4.0) * 25.4,   (7.0 / 64.0) * 25.4, (7.0 / 64.0) * 25.4, (25.4 * 2) },
	{ QString::fromAscii("#4"),  (5.0 / 16.0) * 25.4,  (1.0 / 8.0) * 25.4,  (1.0 / 8.0) * 25.4,  (25.4 * 2) + ((1.0 / 8.0) * 25.4) },
	{ QString::fromAscii("#5"),  (7.0 / 16.0) * 25.4,  (3.0 / 16.0) * 25.4, (3.0 / 16.0) * 25.4, (25.4 * 2) + ((3.0 / 4.0) * 25.4) },
	{ QString::fromAscii("#6"),  (1.0 / 2.0) * 25.4,   (7.0 / 32.0) * 25.4, (7.0 / 32.0) * 25.4, (25.4 * 3)},
	{ QString::fromAscii("#7"),  (5.0 / 8.0) * 25.4,   (1.0 / 4.0) * 25.4,  (1.0 / 4.0) * 25.4,  (25.4 * 3) + ((1.0 / 4.0) * 25.4) },
	{ QString::fromAscii("#8"),  (3.0 / 4.0) * 25.4,   (5.0 / 16.0) * 25.4, (5.0 / 16.0) * 25.4, (25.4 * 3) + ((1.0 / 2.0) * 25.4) },
	{ QString::fromAscii("#9"),  (0.875) * 25.4,       (0.3437) * 25.4,     (0.3437) * 25.4,     (25.4 * 3.625) },
	{ QString::fromAscii("#10"), (1.0) * 25.4,         (0.375) * 25.4,      (0.375) * 25.4,      (25.4 * 3.75) }
};


PROPERTY_SOURCE(Cam::ToolFeature, Cam::Settings::Feature)

ToolFeature::ToolFeature()
{
	//ADD_PROPERTY_TYPE(_prop_, _defaultval_, _group_,_type_,_Docu_)
    ADD_PROPERTY_TYPE(ToolId, (""), "Tool Feature", (App::PropertyType)(App::Prop_ReadOnly),
            "Unique Tool Identifier");

	this->settings = new Settings::TPGSettings;
	this->settings->setFeature(this);

	diameter = NULL;
	tool_length_offset = NULL;
	material = NULL;
	tool_type = NULL;

	m_corner_radius = NULL;
	m_flat_radius = NULL;
	m_cutting_edge_angle = NULL;
	m_cutting_edge_height = NULL;
	m_max_advance_per_revolution = NULL;
	m_automatically_generate_title = NULL;
	m_probe_offset_x = NULL;
	m_probe_offset_y = NULL;
	m_gradient = NULL;
	m_direction = NULL;
	m_pitch = NULL;
	centre_drill_size = NULL;
	m_setup_instructions = NULL;
	title = NULL;
}

ToolFeature::~ToolFeature()
{
	delObjConnection.disconnect();

	if (diameter) diameter->release();
	if (tool_length_offset) tool_length_offset->release();
	if (material) material->release();
	if (tool_type) tool_type->release();

	if (m_corner_radius) m_corner_radius->release();
	if (m_flat_radius) m_flat_radius->release();
	if (m_cutting_edge_angle) m_cutting_edge_angle->release();
	if (m_cutting_edge_height) m_cutting_edge_height->release();
	if (m_max_advance_per_revolution) m_max_advance_per_revolution->release();
	if (m_automatically_generate_title) m_automatically_generate_title->release();
	if (m_probe_offset_x) m_probe_offset_x->release();
	if (m_probe_offset_y) m_probe_offset_y->release();

	if (m_gradient) m_gradient->release();
	if (m_direction) m_direction->release();
	if (m_pitch) m_pitch->release();
	if (centre_drill_size) centre_drill_size->release();
	if (m_setup_instructions) m_setup_instructions->release();
	if (title) title->release();
}

App::DocumentObjectExecReturn *ToolFeature::execute(void)
{
    return App::DocumentObject::StdReturn;
}

void ToolFeature::onSettingDocument()
{
    //Create a signal to observe slot if this item is deleted
    delObjConnection = getDocument()->signalDeletedObject.connect(boost::bind(&Cam::ToolFeature::onDelete, this, _1));
}

// From Cam::Settings::Feature
/* virtual */ Cam::Settings::TPGSettings *ToolFeature::getTPGSettings()
{
	this->initialise();

	if (this->settings != NULL)
	{
		return(this->settings->grab());
	}
	else
	{
		return(NULL);
	}
}


// From Cam::Settings::Feature
/* virtual */ void ToolFeature::onSettingChanged(const std::string key, const std::string previous_value, const std::string new_value)
{
	if (this->settings != NULL)
	{
		Cam::Settings::Definition *definition = this->settings->getDefinition( QString::fromStdString(key) );
		if (definition != NULL)
		{
			if (definition == this->diameter)
			{
				switch (ToolType())
				{
				case eEndmill:
				case eSlotCutter:
				case eBoringHead:
					this->m_flat_radius->set( this->diameter->get(this->m_flat_radius->getUnits()) / 2.0, this->m_flat_radius->getUnits() );
					break;

				default:
					break;
				} // End switch
			}
			else if (definition == this->title)
			{
				this->Label.setValue(this->title->getValue().toAscii().constData());
			}
			else if (definition == m_standard_tap_sizes)
			{
				// The operator has chosen a new tap size from the standard sizes available.  We need to lookup
				// the pre-loaded data and populate the other settings from these definitions.

				::size_t num_metric = (sizeof(metric_tap_sizes)/sizeof(metric_tap_sizes[0]));
				::size_t num_un = (sizeof(unified_thread_standard_tap_sizes)/sizeof(unified_thread_standard_tap_sizes[0]));
				::size_t num_whitworth = (sizeof(british_standard_whitworth_tap_sizes)/sizeof(british_standard_whitworth_tap_sizes[0]));

				::size_t offset = this->m_standard_tap_sizes->get().first;
				if ((offset >= 0) && (offset < num_metric))
				{
					// The diameters in the tables are ALL defined in metric.
					this->diameter->set( metric_tap_sizes[offset].diameter, Cam::Settings::Length::Metric );
					this->m_pitch->set( metric_tap_sizes[offset].pitch, Cam::Settings::Length::Metric );
					this->diameter->setUnits( Cam::Settings::Length::Metric );
					this->m_pitch->setUnits( Cam::Settings::Length::Metric );

				}
				else if ((offset >= num_metric) && (offset < (num_metric + num_un)))
				{
					offset -= num_metric;	// Adjust the offset so that it's relevant to the unified sizes list.

					// The diameters in the tables are ALL defined in metric.
					this->diameter->set( unified_thread_standard_tap_sizes[offset].diameter, Cam::Settings::Length::Metric );
					this->m_pitch->set( unified_thread_standard_tap_sizes[offset].pitch, Cam::Settings::Length::Metric );
					this->diameter->setUnits( Cam::Settings::Length::Imperial );
					this->m_pitch->setUnits( Cam::Settings::Length::Imperial );
				}
				else if ((offset >= (num_metric + num_un)) && (offset < (num_metric + num_un + num_whitworth)))
				{
					offset -= (num_metric + num_un);	// Adjust the offset so that it's relevant to the whitworth sizes list.

					// The diameters in the tables are ALL defined in metric.
					this->diameter->set( british_standard_whitworth_tap_sizes[offset].diameter, Cam::Settings::Length::Metric );
					this->m_pitch->set( british_standard_whitworth_tap_sizes[offset].pitch, Cam::Settings::Length::Metric );
					this->diameter->setUnits( Cam::Settings::Length::Imperial );
					this->m_pitch->setUnits( Cam::Settings::Length::Imperial );
				}
			}
		}
	}
}


void ToolFeature::initialise()
{
	QString qaction = QString::fromAscii("default");

	if ((settings != NULL) && (this->diameter == NULL))
	{
		this->title = new Settings::Text(	"Title", 
											 "Title",
											 this->Label.getValue(),
											 "",
											 "This reflects the object's name.  It can be automatically or manually generated" );		
		settings->addSettingDefinition(qaction, this->title);


		this->m_automatically_generate_title = new Settings::Enumeration(	 "Title Generation", 
												 "Title Generation",
												 int(true),
												 "Title Generation",
												 "Set to true by default but reset to false when the user edits the title");

		this->m_automatically_generate_title->Add(int(false), QString::fromAscii("Leave manually assigned title"));
		this->m_automatically_generate_title->Add(int(true), QString::fromAscii("Automatically generate title"));
		settings->addSettingDefinition(qaction, this->m_automatically_generate_title);


		this->diameter = new Settings::Length(	"Diameter", 
											 "Cutting diameter",
											 "Cutting diameter",
											 5.0,
											 Settings::Definition::Metric );
		this->diameter->Minimum(0.0);	// must be positive.  No maximum.
		settings->addSettingDefinition(qaction, this->diameter);

		this->tool_length_offset = new Settings::Length( "Tool Length Offset", 
											 "Tool Length Offset",
											 "Full length of the tool.  Includes both cutting and non-cutting areas of the tool.  Used for rendering of the tool solid.",
											 5.0,
											 Settings::Definition::Metric );
		this->tool_length_offset->Minimum(0.0);	// must be positive.  No maximum.
		settings->addSettingDefinition(qaction, this->tool_length_offset);

		this->material = new Settings::Enumeration(	 "Material", 
													 "Material",
													 int(eHighSpeedSteel),
													 "HSS or Carbide",
													 "Material with which the tool is made.  eg: High Speed Steel or Carbide.  Used for spindle speed estimation.");

		// Enumerated types MUST have one option for each different value.  For each option, the Id must be the integer form and the Label must
		// be the string (verbose) form.  Only the verbose forms are used on the user interface but the values used in the TPGSettingDefinition.value will
		// always be the integer form.
		// The integer forms need not start from zero or be sequential.  The values will appear in the combo-box in the order that
		// they're defined in the options list.  Their position in the list will be used by the combo-box.

		for (eMaterial_t mat = eHighSpeedSteel; mat < eUndefinedMaterialType; mat = eMaterial_t(int(mat)+1))
		{
			QString label;
			label << mat;		// use the operator<< override to convert from the enum to the string form.

			this->material->Add(int(mat), label);
		}

		settings->addSettingDefinition(qaction, this->material);


		this->tool_type = new Settings::Enumeration(	 "Type", 
												 "Type",
												 int(eEndmill),
												 "Type of tool",
												 "Type of tool");

		// Enumerated types MUST have one option for each different value.  For each option, the Id must be the integer form and the Label must
		// be the string (verbose) form.  Only the verbose forms are used on the user interface but the values used in the TPGSettingDefinition.value will
		// always be the integer form.
		// The integer forms need not start from zero or be sequential.  The values will appear in the combo-box in the order that
		// they're defined in the options list.  Their position in the list will be used by the combo-box.

		for (eToolType tool_type = eDrill; tool_type < eUndefinedToolType; tool_type = eToolType(int(tool_type)+1))
		{
			QString label;
			label << tool_type;		// use the operator<< override to convert from the enum to the string form.

			this->tool_type->Add(int(tool_type), label);
		}
		settings->addSettingDefinition(qaction, this->tool_type);


		this->m_corner_radius = new Settings::Length(	"Corner Radius", 
											 "Corner radius",
											 "Corner radius",
											 0.0,
											 Settings::Definition::Metric );
		this->m_corner_radius->Minimum(0.0);	// must be positive.  No maximum.
		settings->addSettingDefinition(qaction, this->m_corner_radius);

		this->m_flat_radius = new Settings::Length(	"Flat Radius", 
											 "Flat radius",
											 "Flat radius",
											 0.0,
											 Settings::Definition::Metric );
		this->m_flat_radius->Minimum(0.0);	// must be positive.  No maximum.
		settings->addSettingDefinition(qaction, this->m_flat_radius);
		this->m_flat_radius->set( this->diameter->get(this->m_flat_radius->getUnits()) / 2.0 );

		this->m_cutting_edge_height = new Settings::Length(	"Cutting Edge Height", 
											 "Cutting Edge Height",
											 "Cutting Edge Height",
											 0.0,
											 Settings::Definition::Metric );
		this->m_cutting_edge_height->Minimum(0.0);	// must be positive.  No maximum.
		settings->addSettingDefinition(qaction, this->m_cutting_edge_height);

		this->m_cutting_edge_angle = new Settings::Double(	"Cutting Edge Angle", 
											 "Cutting Edge Angle",
											 "Cutting Edge Angle",
											 0.0,
											 "degrees" );
		this->m_cutting_edge_angle->Minimum(0.0);	// must be positive.
		this->m_cutting_edge_angle->Minimum(90.0);
		settings->addSettingDefinition(qaction, this->m_cutting_edge_angle);

		this->m_max_advance_per_revolution = new Settings::Length(	"Max advance per revolution", 
											 "Max advance per revolution",
											 "This is the maximum distance a tool should advance during a single "
											"revolution.  This value is often defined by the manufacturer in "
											"terms of an advance no a per-tooth basis.  This value, however, "
											"must be expressed on a per-revolution basis.  i.e. we don't want "
											"to maintain the number of cutting teeth so a per-revolution "
											"value is easier to use.",
											 0.0,
											 Settings::Definition::Metric );
		this->m_max_advance_per_revolution->Minimum(0.0);	// must be positive.  No maximum.
		settings->addSettingDefinition(qaction, this->m_max_advance_per_revolution);



		this->m_probe_offset_x = new Settings::Length(	"Probe Offset X", 
											 "Probe Offset X",
											 "The following coordinates relate ONLY to touch probe tools.  They describe "
											 "the error the probe tool has in locating an X,Y point.  These values are "
											"added to a probed point's location to find the actual point.  The values "
											"should come from calibrating the touch probe.  i.e. set machine position "
											"to (0,0,0), drill a hole and then probe for the centre of the hole.  The "
											"coordinates found by the centre finding operation should be entered into "
											"these values verbatim.  These will represent how far off concentric the "
											"touch probe's tip is with respect to the quil.  Of course, these only "
											"make sense if the probe's body is aligned consistently each time.  I will "
											"ASSUME this is correct.",
											 0.0,
											 Settings::Definition::Metric );
		settings->addSettingDefinition(qaction, this->m_probe_offset_x);

		this->m_probe_offset_y = new Settings::Length(	"Probe Offset Y", 
											 "Probe Offset Y",
											 "The following coordinates relate ONLY to touch probe tools.  They describe "
											 "the error the probe tool has in locating an X,Y point.  These values are "
											"added to a probed point's location to find the actual point.  The values "
											"should come from calibrating the touch probe.  i.e. set machine position "
											"to (0,0,0), drill a hole and then probe for the centre of the hole.  The "
											"coordinates found by the centre finding operation should be entered into "
											"these values verbatim.  These will represent how far off concentric the "
											"touch probe's tip is with respect to the quil.  Of course, these only "
											"make sense if the probe's body is aligned consistently each time.  I will "
											"ASSUME this is correct.",
											 0.0,
											 Settings::Definition::Metric );	
		settings->addSettingDefinition(qaction, this->m_probe_offset_y);




		this->m_gradient = new Settings::Length(	"Ramp entry gradient", 
											 "Ramp entry gradient",
											 "The gradient is the steepest angle at which this tool can plunge into the material.  Many "
											 "tools behave better if they are slowly ramped down into the material.  This gradient "
											 "specifies the steepest angle of decsent.  This is expected to be a negative number indicating "
											 "the 'rise / run' ratio.  Since the 'rise' will be downward, it will be negative. "
											 "By this measurement, a drill bit's straight plunge would have an infinite gradient (all rise, no run). "
											 "To cater for this, a value of zero will indicate a straight plunge.",
											 0.0,
											 Settings::Definition::Metric );	
		settings->addSettingDefinition(qaction, this->m_gradient);

		
		this->m_direction = new Settings::Enumeration(	 "Tapping direction", 
												 "Tapping direction",
												 int(eRightHandThread),
												 "Tapping direction",
												 "Tapping direction");

		for (eTappingDirection_t direction = eRightHandThread; direction <= eLeftHandThread; direction = eTappingDirection_t(int(direction)+1))
		{
			QString label;
			label << direction;		// use the operator<< override to convert from the enum to the string form.

			this->m_direction->Add(int(direction), label);
		}
		settings->addSettingDefinition(qaction, this->m_direction);


		this->m_pitch = new Settings::Length(	"Thread Pitch", 
											 "Thread Pitch",
											 "Thread Pitch",
											 0.0,
											 Settings::Definition::Metric );	
		settings->addSettingDefinition(qaction, this->m_pitch);

		

		this->m_standard_tap_sizes = new Settings::Enumeration(	 "Standard Tap Sizes", 
												 "Standard Tap Sizes",
												 int(0),
												 "Standard Tap Sizes",
												 "Standard Tap Sizes");

		::size_t num_metric = (sizeof(metric_tap_sizes)/sizeof(metric_tap_sizes[0]));
		::size_t num_un = (sizeof(unified_thread_standard_tap_sizes)/sizeof(unified_thread_standard_tap_sizes[0]));
		::size_t num_whitworth = (sizeof(british_standard_whitworth_tap_sizes)/sizeof(british_standard_whitworth_tap_sizes[0]));

		for (::size_t i=0; i<num_metric; i++)
		{
			this->m_standard_tap_sizes->Add(int(i), metric_tap_sizes[i].description);
		}
		for (::size_t i=num_metric; i<(num_un + num_metric); i++)
		{
			this->m_standard_tap_sizes->Add(int(i), unified_thread_standard_tap_sizes[i-(num_metric)].description);
		}
		for (::size_t i=(num_un + num_metric); i<(num_un + num_metric + num_whitworth); i++)
		{
			this->m_standard_tap_sizes->Add(int(i), british_standard_whitworth_tap_sizes[i-(num_metric+num_un)].description);
		}
		settings->addSettingDefinition(qaction, this->m_standard_tap_sizes);



		this->centre_drill_size = new Settings::Enumeration(	"Centre-Drill size", 
											 "Centre-Drill size",
											 0,	// Value.
											 "",
											 "Centre-Drill size" );
		for (::size_t i=0; i<(sizeof(centre_drill_sizes)/sizeof(centre_drill_sizes[0])); i++)
		{
			this->centre_drill_size->Add(int(i), centre_drill_sizes[i].size);
		}
		settings->addSettingDefinition(qaction, this->centre_drill_size);

		this->m_setup_instructions = new Settings::Text(	"Setup Instructions", 
											 "Setup Instructions",
											 "",	// Value.
											 "",
											 "Note to add to GCode file to make sure the operator sets up the tool correctly at the start." );		
		settings->addSettingDefinition(qaction, this->m_setup_instructions);
	}
}


void ToolFeature::ResetSettingsToReasonableValues(const bool suppress_warnings /* = false */ )
{
	// Force all settings to be invisible and then turn on just those settings that make sense.
	std::vector<Settings::Definition *> all_settings = this->settings->getSettings();
	for (std::vector<Settings::Definition *>::iterator itSetting = all_settings.begin(); itSetting != all_settings.end(); itSetting++)
	{
		itSetting->visible = false;
	}
	
	// These settings are true for all tool types.
	this->tool_type->visible = true;
	this->title->visible = true;
	this->m_automatically_generate_title->visible = true;

	switch (this->tool_type->get().first)
	{
		case eDrill:
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			break;

		case eCentreDrill:
			this->material->visible = true;
			this->centre_drill_size->visible = true;
			break;

		case eEndmill:
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = true;
			this->m_flat_radius->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eSlotCutter:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eBallEndMill:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eChamfer:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eTurningTool:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eTouchProbe:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eToolLengthSwitch:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eExtrusion:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eTapTool:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eEngravingTool:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eBoringHead:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;

	case eDragKnife:
					this->diameter->visible = true;
			this->diameter->visible = true;
			this->tool_length_offset = true;
			this->material->visible = true;
			this->m_corner_radius->visible = false;
			this->m_flat_radius->visible = false;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = false;
			this->m_probe_offset_x = false;
			this->m_probe_offset_y->visible = false;
			this->m_gradient->visible = false;
			this->m_direction->visible = false;
			this->m_pitch->visible = false;
			this->m_standard_tap_sizes->visible = false;
			this->centre_drill_size->visible = false;
			this->m_setup_instructions->visible = false;
		break;
	}
}

const ToolFeature::eToolType ToolFeature::ToolType() const
{
	if (this->tool_type == NULL) return(this->eUndefinedToolType);
	return(eToolType(this->tool_type->get().first));
}


void ToolFeature::onDelete(const App::DocumentObject &docObj) {

//    // If deleted object me, proceed to delete my children
//    const char *myName = getNameInDocument();
//    if(myName != 0 && std::strcmp(docObj.getNameInDocument(), myName) == 0) {
//        App::Document *pcDoc = getDocument();
//
//        // remove the toolpath object if needed
//        if (this->ToolPath.getValue() != NULL) {
//            pcDoc->remObject(this->ToolPath.getValue()->getNameInDocument());
//        }
//    }
}

//void ToolFeature::Save(Base::Writer &writer) const
//{
//    //save the father classes
//    App::DocumentObject::Save(writer);
//}

//void TPGFeature::Restore(Base::XMLReader &reader)
//{
//    //read the father classes
//    App::DocumentObject::Restore(reader);
//}


//void ToolFeature::onDocumentRestored()
//{
//}
