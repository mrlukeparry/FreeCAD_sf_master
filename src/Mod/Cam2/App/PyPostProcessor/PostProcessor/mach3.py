################################################################################
# mach3.py
#
#  Directly copied from HeeksCNC with modifications by Dan Falck intended to 
#  use std output via print functions
#  License header also added to be consistant with HeeksCNC

# work derived from HeeksCNC and Hirutso Enni, 2009-01-13
# altered by Dan Falck 2010-08-04
# added tap() arguments Michael Haberler 2010-10-07
#    New BSD License
#    Copyright (c) <2013>, <Dan Heeks>
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#        * Redistributions of source code must retain the above copyright
#          notice, this list of conditions and the following disclaimer.
#        * Redistributions in binary form must reproduce the above copyright
#          notice, this list of conditions and the following disclaimer in the
#          documentation and/or other materials provided with the distribution.
#        * Neither the name of the <organization> nor the
#          names of its contributors may be used to endorse or promote products
#          derived from this software without specific prior written permission.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#    DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
#    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
###############################################################################

import nc
import iso
import math

class Creator(iso.Creator):
	def init(self): 
		iso.Creator.init(self) 

	def SPACE(self): return(' ')
	def WORK_OFFSET(self): return('G10 L2' + self.SPACE())
        
	def program_begin(self, id, comment):
		self.write( ('(' + 'GCode created using the HeeksCNC Mach3 post processor' + ')' + '\n') )
		self.write( ('(' + comment + ')' + '\n') )

	# def TAP(self): return('G33.1')
	# def TAP_DEPTH(self, depth): return(self.SPACE() + 'K' + (self.fmt.string(depth)))
	def BORE_FEED_OUT(self): return('G85')
	def BORE_SPINDLE_STOP_RAPID_OUT(self): return('G86')
	def BORE_DWELL_FEED_OUT(self, format, dwell): return('G89') + self.SPACE() + (format % dwell)
	def FEEDRATE(self): return((self.SPACE() + ' F'))

	def COMMENT(self,comment):
		# Replace any embedded round brackets with curly braces so that the EMC2 GCode
		# interpreter will not have trouble with the nested comment format.
		_comment = comment.replace('(','{')
		_comment = _comment.replace(')','}')
		return( ('(%s)' % _comment ) )


	def program_begin(self, id, comment):
		self.write( ('(' + comment + ')' + '\n') )
		self.write_blocknum()
		self.write( self.DISABLE_TOOL_LENGTH_COMPENSATION() + '\t(Ensure tool length compensation is OFF)\n' )
		self.tool_length_compenstation_enabled = False
		self.remove_temporary_origin()
		for _coordinate_system_number in range(1,10):
			self.work_offset(workplane=_coordinate_system_number, xy_plane_rotation=0.0)

 ############################################################################
    ##  Settings
    
	def imperial(self):
            self.write_blocknum()
            self.write( self.IMPERIAL() + '\t(Imperial Values)\n')
            self.fmt.number_of_decimal_places = 4
	    self.gcode_is_metric = False

	def metric(self):
            self.write_blocknum()
            self.fmt.number_of_decimal_places = 3
            self.write( self.METRIC() + '\t(Metric Values)\n' )
	    self.gcode_is_metric = True

	def absolute(self):
		self.write_blocknum()
		self.write( self.ABSOLUTE() + '\t(Absolute Coordinates)\n')

	def polar(self, on=True):
		if (on) :
			self.write_blocknum()
			self.write(self.POLAR_ON() + '\t(Polar ON)\n' )
		else : 
			self.write_blocknum()
			self.write(self.POLAR_OFF() + '\t(Polar OFF)\n' )

	def set_plane(self, plane):
		if (plane == 0) : 
			self.write_blocknum()
			self.write(self.PLANE_XY() + '\t(Select XY Plane)\n')
		elif (plane == 1) :
			self.write_blocknum()
			self.write(self.PLANE_XZ() + '\t(Select XZ Plane)\n')
		elif (plane == 2) : 
			self.write_blocknum()
			self.write(self.PLANE_YZ() + '\t(Select YZ Plane)\n')

	def comment(self, text):
		self.write_blocknum()
		self.write((self.COMMENT(text) + '\n'))

	# This is the coordinate system we're using.  G54->G59, G59.1, G59.2, G59.3
	# These are selected by values from 1 to 9 inclusive.
	def workplane(self, id):
		self.current_workplane = id
		if ((id >= 1) and (id <= 6)):
			self.write_blocknum()
			self.write( (self.WORKPLANE() % (id + self.WORKPLANE_BASE())) + '\t(Select Relative Coordinate System)\n')
		if ((id >= 7) and (id <= 9)):
			self.write_blocknum()
			self.write( ((self.WORKPLANE() % (6 + self.WORKPLANE_BASE())) + ('.%i' % (id - 6))) + '\t(Select Relative Coordinate System)\n')


	def work_offset(self, workplane, x=None, y=None, z=None, a=None, b=None, c=None, xy_plane_rotation=None ):
		if x != None or y != None or z != None or a != None or b != None or c != None:
			self.write_blocknum()
			self.write(self.WORK_OFFSET() + ' P' + str(workplane))
			if x != None:
				self.write(self.SPACE() + self.X() + self.SPACE() + self.fmt.string(x))
			if y != None:
				self.write(self.SPACE() + self.Y() + self.SPACE() + self.fmt.string(y))
			if z != None:
				self.write(self.SPACE() + self.Z() + self.SPACE() + self.fmt.string(z))
			if a != None:
				self.write(self.SPACE() + self.A() + self.SPACE() + self.fmt.string(a))
			if b != None:
				self.write(self.SPACE() + self.B() + self.SPACE() + self.fmt.string(b))
			if c != None:
				self.write(self.SPACE() + self.C() + self.SPACE() + self.fmt.string(c))
			self.write('\n')

		if xy_plane_rotation != None:
			self.write_blocknum()
			self.write('G10 L2 P' + str(workplane))
			self.write(self.SPACE() + 'R' + self.SPACE() + self.fmt.string(xy_plane_rotation))
			self.write(self.SPACE() + '(set the XY plane rotation)');
			self.write('\n')

	def debug_message(self, text=None ):
		self.message(text)
		
	def log_message(self, text=None ):
		self.message(text)
		
	def message(self, text=None ):
		# Replace any embedded round brackets with curly braces so that the EMC2 GCode
		# interpreter will not have trouble with the nested comment format.
		_message = text.replace('(','{')
		_message = _message.replace(')','}')
		self.write_blocknum()
		self.write('(MSG,' + _message + ')\n')
		
	def start_CRC(self, left = True, radius = 0.0):
		if self.t == None:
			raise "No tool specified for start_CRC()"
		self.write_blocknum()
		if left:
			self.write(('G41' + self.SPACE() + 'D%i') % self.t  + '\t(start left cutter radius compensation)\n' )
		else:
			self.write(('G42' + self.SPACE() + 'D%i') % self.t  + '\t(start right cutter radius compensation)\n' )

	def end_CRC(self):
		self.g = 'G40'
		self.write_blocknum()
		self.write_preps()
		self.write_misc()
		self.write('\t(end cutter radius compensation)\n')
		
	def tool_defn(self, id, name='', radius=None, length=None, gradient=None):
		pass

nc.creator = Creator()

