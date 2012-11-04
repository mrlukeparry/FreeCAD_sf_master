#/***************************************************************************
# *   Copyright (c) 2012 Andrew Robinson  (andrewjrobinson@gmail.com)       *
# *                                                                         *
# *   This file is part of the FreeCAD CAx development system.              *
# *                                                                         *
# *   This library is free software; you can redistribute it and/or         *
# *   modify it under the terms of the GNU Library General Public           *
# *   License as published by the Free Software Foundation; either          *
# *   version 2 of the License, or (at your option) any later version.      *
# *                                                                         *
# *   This library  is distributed in the hope that it will be useful,      *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this library; see the file COPYING.LIB. If not,    *
# *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
# *   Suite 330, Boston, MA  02111-1307, USA                                *
# *                                                                         *
# ***************************************************************************/
 
from PyCam.PyCam import PyTPGBase, UnimplementedTPError


class ExampleTPG(PyTPGBase):
    '''This is an example TPG that demonstrates how to create a TPG for the 
    CAM2 module.'''
    
    '''
    Each settings is made up of a 6-tuple (<name>, <label>, <type>, 
    <defaultvalue>, <unit>, <helptext>)
    
    NOTE: All values must be a string, unicode or None
    <name>
        A unique (within the action) identifier for the given setting.  It may 
        be preceded by one or more group names (separated by ':') which are 
        used by the UI to construct a setting Tree.
        
    <label>
        The text to display to the user for this setting.
    
    <type>
        May be 'Cam::Group' for a group header (i.e. not a real setting) OR one
        of the built in types
        @see: http://sourceforge.net/apps/mediawiki/free-cad/index.php?title=Scripted_objects#Available_properties
        
    <defaultvalue>
        The default value that will appear on screen before the user makes 
        changes.
        
    <unit>
        Optional help text that identifies the units that this setting expects
        
    <helptext>
        Optional description of this setting; it is useful to include examples 
        of desirable options
    '''
    settings = {
                'rough': [('tolerance','Tolerance', 'App::PropertyInteger', '1', 'mm', 'How close to run tool to final depth'),
                          ],
                'finish': [('tolerance','Tolerance', 'Cam::Group', None, '', ''), # this is optional way to specify the label of the group
                           # these are the sub elements of tolerance group
                           ('tolerance:min','Minimum', 'App::PropertyInteger', '0.0', 'mm', 'How close to run tool to final depth'),
                           ('tolerance:max','Maximum', 'App::PropertyInteger', '0.1', 'mm', 'How close to run tool to final depth'),
                           ],
                }
    
    # The static identification and descriptive information 
    id = '10bf335e-2491-11e2-8f39-08002734b94f' #this NEEDS to be unique!!!  @see PyTPGBase.id
    name = 'ExampleTPG' # should match classname
    description = 'This is a simple Example Python TPG'
        
    def getActions(self):
        '''Returns a list of actions that this TPG offers'''
        return self.settings.keys()

    def getSettings(self, action=None):
        '''Returns a list of settings that the TPG 'Action' will take.  If 
        action is None, then this will return a dictionary of all actions 
        settings.  Each setting will be in format of (<name>, <label>, <type>, <defaultvalue>, <units>, <helptext>)
        NOTE: the resulting dictionary or list should be considered read-only!'''
        if action:
            if action in self.settings:
                return self.settings[action]
            raise UnimplementedTPError(action)
        return self.settings
        
    def run(self, action, settings=[]):
        '''Runs the selected action and returns the resulting TP'''
        #TODO: implement an example (Note: need to define the output interface i.e. what it will return)
        raise UnimplementedTPError(action)
    
    