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
	extrusion_material = NULL;
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
	if (extrusion_material) extrusion_material->release();
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

				case eChamfer:
				case eEngravingTool:
				case eCentreDrill:
					{
						// Recalculate the cutting edge length based on this new diameter
						// and the cutting angle.

						double opposite = (this->diameter->get(this->diameter->getUnits()) / 2.0) - this->m_flat_radius->get(this->diameter->getUnits());
						double angle = this->m_cutting_edge_angle->get() / 360.0 * 2 * M_PI;

						this->m_cutting_edge_height->set( opposite / tan(angle) );
					}
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

		ResetSettingsToReasonableValues(true);
	}
}


void ToolFeature::initialise()
{
	QString qaction = QString::fromAscii("default");

	if ((settings != NULL) && (this->diameter == NULL))
	{
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



		this->extrusion_material = new Settings::Enumeration(	 "Extrusion Material", 
												 "Extrusion Material",
												 int(eABS),
												 "Extrusion Material",
												 "Extrusion Material");

		for (eExtrusionMaterial_t eMaterial = eABS; eMaterial <= eUndefinedExtrusionMaterialType; eMaterial = eExtrusionMaterial_t(int(eMaterial)+1))
		{
			QString label;
			label << eMaterial;		// use the operator<< override to convert from the enum to the string form.

			this->extrusion_material->Add(int(eMaterial), label);
		}
		settings->addSettingDefinition(qaction, this->extrusion_material);





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

		ResetSettingsToReasonableValues(true);
	}
}


void ToolFeature::ResetSettingsToReasonableValues(const bool suppress_warnings /* = false */ )
{
	// Force all settings to be invisible and then turn on just those settings that make sense.
	std::vector<Settings::Definition *> all_settings = this->settings->getSettings();
	for (std::vector<Settings::Definition *>::iterator itSetting = all_settings.begin(); itSetting != all_settings.end(); itSetting++)
	{
		(*itSetting)->visible = false;
	}
	
	// These settings are true for all tool types.
	this->tool_type->visible = true;
	this->title->visible = true;
	this->m_automatically_generate_title->visible = true;

	// Turn settings on and off based on the tool's type.
	switch (this->tool_type->get().first)
	{
		case eDrill:
			this->diameter->visible = true;
			this->tool_length_offset->visible = true;
			this->material->visible = true;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_gradient->set(0.0);
			this->m_cutting_edge_angle->set(59.0);
			this->m_cutting_edge_height->set( this->diameter->get(this->m_cutting_edge_height->getUnits()) * 3.0 );
			break;

		case eCentreDrill:
			this->material->visible = true;
			this->centre_drill_size->visible = true;
			this->m_gradient->set(0.0);
			break;

		case eSlotCutter:
		case eEndmill:
			this->diameter->visible = true;
			this->tool_length_offset->visible = true;
			this->material->visible = true;
			this->m_corner_radius->visible = true;
			this->m_flat_radius->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = true;
			this->m_gradient->visible = true;
			this->m_gradient->set(-1.0 / 50.0);
		break;

	case eBallEndMill:
			this->diameter->visible = true;
			this->tool_length_offset->visible = true;
			this->material->visible = true;
			this->m_corner_radius->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = true;
			this->m_gradient->visible = true;
			this->m_gradient->set(-1.0 / 50.0);
		break;

	case eChamfer:
			this->diameter->visible = true;
			this->tool_length_offset->visible = true;
			this->material->visible = true;
			this->m_flat_radius->visible = true;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = true;
			this->m_gradient->set(0.0);
		break;

	case eTurningTool:
			this->material->visible = true;
			this->m_cutting_edge_angle->visible = true;
			this->m_max_advance_per_revolution->visible = true;
			this->m_gradient->set(0.0);
		break;

	case eTouchProbe:
			this->tool_length_offset->visible = true;
			this->m_probe_offset_x->visible = true;
			this->m_probe_offset_y->visible = true;
			this->m_gradient->set(0.0);
		break;

	case eToolLengthSwitch:
			this->tool_length_offset->visible = true;
			this->m_gradient->set(0.0);
		break;

	case eExtrusion:
			this->diameter->visible = true;
			this->tool_length_offset->visible = true;
			this->m_setup_instructions->visible = true;
			this->m_gradient->set(0.0);
		break;

	case eTapTool:
			this->diameter->visible = true;
			this->tool_length_offset->visible = true;
			this->material->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_direction->visible = true;
			this->m_pitch->visible = true;
			this->m_standard_tap_sizes->visible = true;
			this->m_gradient->set(0.0);
		break;

	case eEngravingTool:
			this->diameter->visible = true;
			this->tool_length_offset->visible = true;
			this->material->visible = true;
			this->m_corner_radius->set(0.0);
			this->m_flat_radius->visible = true;
			this->m_cutting_edge_angle->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = true;
			this->m_gradient->set(0.0);
		break;

	case eBoringHead:
			this->diameter->visible = true;
			this->tool_length_offset->visible = true;
			this->material->visible = true;
			this->m_cutting_edge_height->visible = true;
			this->m_max_advance_per_revolution->visible = true;
			this->m_gradient->set(0.0);
		break;

	case eDragKnife:
			this->tool_length_offset->visible = true;
		break;
	}

	if (this->m_automatically_generate_title->get().first != 0)
	{
		this->ResetTitle();
	}
}

/**
	Find a fraction that represents this floating point number.  We use this
	purely for readability purposes.  It only looks accurate to the nearest 1/64th

	eg: 0.125 -> "1/8"
	    1.125 -> "1 1/8"
	    0.109375 -> "7/64"
 */
/* static */ QString ToolFeature::FractionalRepresentation( const Cam::Settings::Length *original_value, const int max_denominator /* = 64 */ )
{

	std::ostringstream result;
	double _value(original_value->get(Cam::Settings::Length::Imperial));
	double near_enough = 0.00001;

	if (floor(_value) > 0)
	{
		result << floor(_value);
		_value -= floor(_value);
	} // End if - then

	double fraction = 0.0;
	if ( ((_value > fraction) && ((_value - fraction) < near_enough)) ||
	     ((_value < fraction) && ((fraction - _value) < near_enough)) ||
	     (_value == fraction) )
	{
		return(QString::fromStdString(result.str()));
	} // End if - then

	if (result.str().length() > 0)
	{
	    result << " ";
	}

	// We only want even numbers between 2 and 64 for the denominator.  The others just look 'wierd'.
	for (int denominator = 2; denominator <= max_denominator; denominator *= 2)
	{
		for (int numerator = 1; numerator < denominator; numerator++)
		{
			double fraction = double( double(numerator) / double(denominator) );
			if ( ((_value > fraction) && ((_value - fraction) < near_enough)) ||
			     ((_value < fraction) && ((fraction - _value) < near_enough)) ||
			     (_value == fraction) )
			{
				result << numerator << "/" << denominator;
				return(QString::fromStdString(result.str()));
			} // End if - then
		} // End for
	} // End for

	return(QString::null);	// It's not a recognisable fraction.  Return nothing to indicate such.
} // End FractionalRepresentation() method


/* static */ QString ToolFeature::PrintedCircuitBoardRepresentation( const Cam::Settings::Length *pDiameter )
{
	// Only look up to 200 thousanths of an inch
	Cam::Settings::Length TwoHundredThousanths( 200.0 / 1000.0, Cam::Settings::Length::Imperial );
	if (*pDiameter < TwoHundredThousanths)
	{
		for (int mil=1; mil<=200; mil++)
		{
			Cam::Settings::Length size_in_mil((mil / 1000.0), Cam::Settings::Length::Imperial);
			if (*pDiameter == size_in_mil)
			{
				std::ostringstream result;
				result << mil << " thou";
				return(QString::fromStdString(result.str()));
			}
		}
	}

	return(QString::null);
} // End PrintedCircuitBoardRepresentation() method



/* static */ QString ToolFeature::GuageNumberRepresentation( const Cam::Settings::Length *pDiameter )
{

    typedef struct Guages {
        const QString guage;
        double imperial;
        double metric;
    } Guages_t;

    static Guages_t guages[] = {{QString::fromAscii("80"),0.0135,0.343},{QString::fromAscii("79"),0.0145,0.368},{QString::fromAscii("78"),0.016,0.406},{QString::fromAscii("77"),0.018,0.457},{QString::fromAscii("76"),0.020,0.508},
                         {QString::fromAscii("75"),0.021,0.533},{QString::fromAscii("74"),0.0225,0.572},{QString::fromAscii("73"),0.024,0.610},{QString::fromAscii("72"),0.025,0.635},{QString::fromAscii("71"),0.026,0.660},
                         {QString::fromAscii("70"),0.028,0.711},{QString::fromAscii("69"),0.0292,0.742},{QString::fromAscii("68"),0.031,0.787},{QString::fromAscii("67"),0.032,0.813},{QString::fromAscii("66"),0.033,0.838},
                         {QString::fromAscii("65"),0.035,0.889},{QString::fromAscii("64"),0.036,0.914},{QString::fromAscii("63"),0.037,0.940},{QString::fromAscii("62"),0.038,0.965},{QString::fromAscii("61"),0.039,0.991},
                         {QString::fromAscii("60"),0.040,1.016},{QString::fromAscii("59"),0.041,1.041},{QString::fromAscii("58"),0.042,1.067},{QString::fromAscii("57"),0.043,1.092},{QString::fromAscii("56"),0.0465,1.181},
                         {QString::fromAscii("55"),0.052,1.321},{QString::fromAscii("54"),0.055,1.397},{QString::fromAscii("53"),0.0595,1.511},{QString::fromAscii("52"),0.0635,1.613},{QString::fromAscii("51"),0.067,1.702},
                         {QString::fromAscii("50"),0.070,1.778},{QString::fromAscii("49"),0.073,1.854},{QString::fromAscii("48"),0.076,1.930},{QString::fromAscii("47"),0.0785,1.994},{QString::fromAscii("46"),0.081,2.057},
                         {QString::fromAscii("45"),0.082,2.083},{QString::fromAscii("44"),0.086,2.184},{QString::fromAscii("43"),0.089,2.261},{QString::fromAscii("42"),0.0935,2.375},{QString::fromAscii("41"),0.096,2.438},
                         {QString::fromAscii("40"),0.098,2.489},{QString::fromAscii("39"),0.0995,2.527},{QString::fromAscii("38"),0.1015,2.578},{QString::fromAscii("37"),0.104,2.642},{QString::fromAscii("36"),0.1065,2.705},
                         {QString::fromAscii("35"),0.110,2.794},{QString::fromAscii("34"),0.111,2.819},{QString::fromAscii("33"),0.113,2.870},{QString::fromAscii("32"),0.116,2.946},{QString::fromAscii("31"),0.120,3.048},
                         {QString::fromAscii("30"),0.1285,3.264},{QString::fromAscii("29"),0.136,3.454},{QString::fromAscii("28"),0.1405,3.569},{QString::fromAscii("27"),0.144,3.658},{QString::fromAscii("26"),0.147,3.734},
                         {QString::fromAscii("25"),0.1495,3.797},{QString::fromAscii("24"),0.152,3.861},{QString::fromAscii("23"),0.154,3.912},{QString::fromAscii("22"),0.157,3.988},{QString::fromAscii("21"),0.159,4.039},
                         {QString::fromAscii("20"),0.161,4.089},{QString::fromAscii("19"),0.166,4.216},{QString::fromAscii("18"),0.1695,4.305},{QString::fromAscii("17"),0.173,4.394},{QString::fromAscii("16"),0.177,4.496},
                         {QString::fromAscii("15"),0.180,4.572},{QString::fromAscii("14"),0.182,4.623},{QString::fromAscii("13"),0.185,4.699},{QString::fromAscii("12"),0.189,4.801},{QString::fromAscii("11"),0.191,4.851},
                         {QString::fromAscii("10"),0.1935,4.915},{QString::fromAscii("9"),0.196,4.978},{QString::fromAscii("8"),0.199,5.055},{QString::fromAscii("7"),0.201,5.105},{QString::fromAscii("6"),0.204,5.182},
                         {QString::fromAscii("5"),0.2055,5.220},{QString::fromAscii("4"),0.209,5.309},{QString::fromAscii("3"),0.213,5.410},{QString::fromAscii("2"),0.221,5.613},{QString::fromAscii("1"),0.228,5.791},
                         {QString::fromAscii("A"),0.234,5.944},{QString::fromAscii("B"),0.238,6.045},{QString::fromAscii("C"),0.242,6.147},{QString::fromAscii("D"),0.246,6.248},{QString::fromAscii("E"),0.250,6.350},
                         {QString::fromAscii("F"),0.257,6.528},{QString::fromAscii("G"),0.261,6.629},{QString::fromAscii("H"),0.266,6.756},{QString::fromAscii("I"),0.272,6.909},{QString::fromAscii("J"),0.277,7.036},
                         {QString::fromAscii("K"),0.281,7.137},{QString::fromAscii("L"),0.290,7.366},{QString::fromAscii("M"),0.295,7.493},{QString::fromAscii("N"),0.302,7.671},{QString::fromAscii("O"),0.316,8.026},
                         {QString::fromAscii("P"),0.323,8.204},{QString::fromAscii("Q"),0.332,8.433},{QString::fromAscii("R"),0.339,8.611},{QString::fromAscii("S"),0.348,8.839},{QString::fromAscii("T"),0.358,9.093},
                         {QString::fromAscii("U"),0.368,9.347},{QString::fromAscii("V"),0.377,9.576},{QString::fromAscii("W"),0.386,9.804},{QString::fromAscii("X"),0.397,10.08},{QString::fromAscii("Y"),0.404,10.26},
                         {QString::fromAscii("Z"),0.413,10.49}};

    double tolerance = 1e-7;	// Todo handle tolerance properly.
    for (::size_t offset=0; offset < (sizeof(guages)/sizeof(guages[0])); offset++)
    {
		if (pDiameter->getUnits() == Cam::Settings::Length::Imperial)
        {
			if (fabs(pDiameter->get(Cam::Settings::Length::Imperial) - guages[offset].imperial) < tolerance)
            {
				std::ostringstream result;
				result << "#" << guages[offset].guage.toAscii().constData();
				return(QString::fromStdString(result.str()));
            }
        }
        else
        {
            if (fabs(pDiameter->get(Cam::Settings::Length::Metric) - guages[offset].metric) < tolerance)
            {
				std::ostringstream result;
                result << "#" << guages[offset].guage.toAscii().constData();
                return(QString::fromStdString(result.str()));
            }
        }
    } // End for

	return(QString::null);
} // End GuageNumberRepresentation() method


void ToolFeature::ResetTitle()
{
	std::ostringstream name;

	eToolType ToolType(eToolType(this->tool_type->get().first));
	name << ToolType;
	
	if (ToolType == eCentreDrill)
	{
		centre_drill_t *pCentreDrill = this->CentreDrillDefinition(this->centre_drill_size->get().second);
		if (pCentreDrill != NULL)
		{
			name << "Size " << this->centre_drill_size->get().second.toAscii().constData() << " ";
		}

		QString fraction = FractionalRepresentation(diameter);
		if (fraction.length() > 0)
		{
			name << "(" << fraction.toAscii().constData() << "\"=";
		}
		else
		{
			name << "(";
		}

		QString mm = QString::number(diameter->get(Cam::Settings::Length::Metric), 'f', 2);
		name << mm.toAscii().constData() << "mm) ";
	}

	if ((ToolType != eTurningTool) &&
		(ToolType != eTouchProbe) &&
		(ToolType != eToolLengthSwitch) &&
		(ToolType != eCentreDrill))
	{
		if (diameter->getUnits() == Cam::Settings::Length::Metric)
		{
			// We're using metric.  Leave the diameter as a floating point number.  It just looks more natural.
			name << diameter->get(Cam::Settings::Length::Metric) << " mm ";

			QString fraction = FractionalRepresentation( diameter );
			if (fraction.toStdString().length() > 0)
			{
				name << "(" << fraction.toAscii().constData() << "\") ";
			}
			else
			{
				QString pcb = PrintedCircuitBoardRepresentation( diameter );
				if (pcb.length() > 0)
				{
					name << "(" << pcb.toAscii().constData() << ") ";
				}
			}
		} // End if - then
		else
		{
			// We're using inches.  Find a fractional representation if one matches.
			QString fraction = FractionalRepresentation(diameter);
			if (fraction.length() > 0)
			{
                name << fraction.toAscii().constData() << " inch ";
			}
			else
			{
				QString guage = GuageNumberRepresentation( diameter );
			    if (guage.length() > 0)
			    {
                    name << guage.toAscii().constData() << " ";

					/*
					TODO Add the title format setting when we get somewhere to store long-term settings.
                    if (TOOLS)
                    {
                        if (TOOLS->m_title_format == CTools::eIncludeGuageAndSize)
                        {
						*/
							name << "(" << diameter->get(Cam::Settings::Length::Imperial) << " inch) ";
						/*
                        }
                    }
					*/
			    }
			    else
			    {
					QString pcb = PrintedCircuitBoardRepresentation( diameter );
					if (pcb.length() > 0)
					{
						name << pcb.toAscii().constData() << " ";
					}
					else
					{
						name << diameter->get(Cam::Settings::Length::Imperial) << " inch ";
					}
			    }
			}
		} // End if - else
	} // End if - then

	if ((ToolType != eTouchProbe) && (ToolType != eToolLengthSwitch) && (ToolType != eExtrusion))
	{
		name << eMaterial_t(this->material->get().first) << " ";
	} // End if - then

	if ((ToolType == eExtrusion))
	{
		name << eExtrusionMaterial_t(extrusion_material->get().first);
	} // End if - then

	switch (ToolType)
	{
        case eTapTool:
        {
            // See if we can find a name for it in the standard TAP sizes tables.
            bool found = false;

            for (::size_t i=0; ((metric_tap_sizes[i].diameter > 0.0) && (! found)); i++)
            {
				if ((*diameter == Cam::Settings::Length(metric_tap_sizes[i].diameter, Cam::Settings::Length::Metric)) &&
					(*m_pitch == Cam::Settings::Length(metric_tap_sizes[i].pitch, Cam::Settings::Length::Metric)))
                    {
                        name.clear();  // Replace what came before.
                        QString description = metric_tap_sizes[i].description;
						description.replace(QString::fromAscii("  "), QString::fromAscii(" "));
						name << metric_tap_sizes[i].description.toAscii().constData();
                        found = true;
                    }
            }

            for (::size_t i=0; ((unified_thread_standard_tap_sizes[i].diameter > 0.0) && (! found)); i++)
            {
				if ((*diameter == Cam::Settings::Length(unified_thread_standard_tap_sizes[i].diameter, Cam::Settings::Length::Metric)) &&
					(*m_pitch == Cam::Settings::Length(unified_thread_standard_tap_sizes[i].pitch, Cam::Settings::Length::Metric)))
                    {
						name.clear();  // Replace what came before.
                        QString description = unified_thread_standard_tap_sizes[i].description;
						description.replace(QString::fromAscii("  "), QString::fromAscii(" "));
						name << metric_tap_sizes[i].description.toAscii().constData();
                        found = true;
                    }
            }

            for (::size_t i=0; ((british_standard_whitworth_tap_sizes[i].diameter > 0.0) && (! found)); i++)
            {
				if ((*diameter == Cam::Settings::Length(british_standard_whitworth_tap_sizes[i].diameter, Cam::Settings::Length::Metric)) &&
					(*m_pitch == Cam::Settings::Length(british_standard_whitworth_tap_sizes[i].pitch, Cam::Settings::Length::Metric)))
                    {
						name.clear();  // Replace what came before.
                        QString description = british_standard_whitworth_tap_sizes[i].description;
						description.replace(QString::fromAscii("  "), QString::fromAscii(" "));
						name << metric_tap_sizes[i].description.toAscii().constData();
                        found = true;
                    }
            }

            name << " Tap Tool";
            if (eTappingDirection_t(m_direction->get().first) == eLeftHandThread) {
                name << ", " << eTappingDirection_t(m_direction->get().first);
            }
        }
		break;

        case eEngravingTool:	name.clear();	// Remove all that we've already prepared.
			name << m_cutting_edge_angle->get() << " degreee ";
    				name << "Engraving Bit";
		break;

		default:
			break;
	} // End switch

	this->title->setValue( QString::fromAscii(name.str().c_str()) );
}


/**
	The CuttingRadius is almost always the same as half the tool's diameter.
	The exception to this is if it's a chamfering bit.  In this case we
	want to use the flat_radius plus a little bit.  i.e. if we're chamfering the edge
	then we want to use the part of the cutting surface just a little way from
	the flat radius.  If it has a flat radius of zero (i.e. it has a pointed end)
	then it will be a small number.  If it is a carbide tipped bit then the
	flat radius will allow for the area below the bit that doesn't cut.  In this
	case we want to cut around the middle of the carbide tip.  In this case
	the carbide tip should represent the full cutting edge height.  We can
	use this method to make all these adjustments based on the tool's
	geometry and return a reasonable value.

	If express_in_drawing_units is true then we need to divide by the drawing
	units value.  We use metric (mm) internally and convert to inches only
	if we need to and only as the last step in the process.  By default, return
	the value in internal (metric) units.

	If the depth value is passed in as a positive number then the radius is given
	for the corresponding depth (from the bottom-most tip of the tool).  This is
	only relevant for chamfering (angled) bits.
 */
double ToolFeature::CuttingRadius( const Cam::Settings::Length::Units_t units, const Cam::Settings::Length depth /* = -1 */ ) const
{
	double radius;
	double _depth = fabs(depth.get(units));
	switch (eToolType(this->tool_type->get().first))
	{
		case eChamfer:
		case eEngravingTool:
			{
				radius = this->m_flat_radius->get(units) + (_depth * tan((m_cutting_edge_angle->get() / 360.0 * 2 * M_PI)));
		        if (radius > (diameter->get(units) / 2.0))
		        {
		            // The angle and depth would have us cutting larger than our largest diameter.
		            radius = (diameter->get(units) / 2.0);
		        }
		        if (radius <= 0.0)
		        {
		            radius = diameter->get(units) / 2.0;
		        }
			}
			break;

        case eCentreDrill:
            {
				centre_drill_t *pCentreDrill = CentreDrillDefinition(this->centre_drill_size->get().second);

                if (pCentreDrill != NULL)
                {
					Cam::Settings::Length drill_diameter(pCentreDrill->drill_diameter, Cam::Settings::Length::Metric);
                    radius = drill_diameter.get(units) / 2.0;
                }
                else
                {
                    radius = diameter->get(units) / 2.0;
                }
            }
			break;

		case eDragKnife:
			radius = 0.0;
			break;

		case eDrill:
		case eEndmill:
		case eSlotCutter:
		case eBallEndMill:
		case eTurningTool:
		case eTouchProbe:
		case eExtrusion:
		case eToolLengthSwitch:
		case eTapTool:
		default:
			radius = diameter->get(units) / 2.0;
	} // End switch

	return(radius);

} // End CuttingRadius() method



ToolFeature::centre_drill_t *ToolFeature::CentreDrillDefinition( const QString size ) const
{
	for (::size_t i=0; i<sizeof(centre_drill_sizes)/sizeof(centre_drill_sizes[0]); i++)
	{
		if (centre_drill_sizes[i].size == size)
		{
			return(&(centre_drill_sizes[i]));
		}
	}

	return(NULL);
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
