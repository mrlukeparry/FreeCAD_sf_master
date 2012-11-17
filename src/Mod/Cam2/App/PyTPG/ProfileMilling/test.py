import AreaClipper as area

import simple_nc

import profile_funcs as kurve_funcs

Pocket20 = area.Curve()
#closed path
Pocket20.append(area.Point( 0.0, 10.0))
Pocket20.append(area.Point( 10.0, 10.0))
Pocket20.append(area.Point( 10.0, 0.0))
Pocket20.append(area.Point( 2.0, 0.0))
Pocket20.append(area.Vertex(1 , area.Point( -3.49148336111e-15, 2.0), area.Point(0.0, 0.0)))
Pocket20.append(area.Point(0.0,10.0))
curve = Pocket20
#pocket
Pocket21 = area.Curve()
#closed path
Pocket21.append(area.Point( 6.91869606042, 2.51235014759))
Pocket21.append(area.Vertex(1 , area.Point( 8.66869606042, 4.26235014759), area.Point(6.91869606042, 4.26235014759)))
Pocket21.append(area.Point( 8.66869606042, 6.87148713278))
Pocket21.append(area.Vertex(1 , area.Point( 6.91869606042, 8.62148713278), area.Point(6.91869606042, 6.87148713278)))
Pocket21.append(area.Point( 4.74373522448, 8.62148713278))
Pocket21.append(area.Vertex(1 , area.Point( 2.99373522448, 6.87148713278), area.Point(4.74373522448, 6.87148713278)))
Pocket21.append(area.Point( 2.99373522448, 4.26235014759))
Pocket21.append(area.Vertex(1 , area.Point( 4.74373522448, 2.51235014759), area.Point(4.74373522448, 4.26235014759)))
Pocket21.append(area.Point(6.91869606042,2.51235014759))

curve2 = Pocket21

clearance = float(5)
rapid_safety_space = float(2)
start_depth = float(0)
step_down = float(1)
final_depth = float(-1)
tool_diameter = float(.25)
cutting_edge_angle = float(0)

roll_radius = float(2)
offset_extra = 0


roll_on = 'auto'
roll_off = 'auto'
extend_at_start= 0
extend_at_end= 0
lead_in_line_len= 0
lead_out_line_len= 0
kurve_funcs.profile(curve, 'left', tool_diameter/2, offset_extra, roll_radius, roll_on, roll_off, rapid_safety_space, clearance, start_depth, step_down, final_depth,extend_at_start,extend_at_end,lead_in_line_len,lead_out_line_len )

roll_radius = float(.12)
roll_on = 'auto'
roll_off = 'auto'
extend_at_start= 0
extend_at_end= 0
lead_in_line_len= 0
lead_out_line_len= 0
kurve_funcs.profile(curve2, 'left', tool_diameter/2, offset_extra, roll_radius, roll_on, roll_off, rapid_safety_space, clearance, start_depth, step_down, final_depth,extend_at_start,extend_at_end,lead_in_line_len,lead_out_line_len )


# use the following script from the FreeCAD python console to test this:
#import sys
#sys.path.insert(0,FreeCAD.ConfigGet("AppHomePath")+'/Mod/PyCam/PyTPG/ProfileMilling')
#import test
