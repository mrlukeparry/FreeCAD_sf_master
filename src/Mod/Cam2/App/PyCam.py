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

import types, inspect

import Cam

PyTPGManagerInst = None

def scanPlugins():
    '''Short-cut to 'PyTPGManager.instance().scanPlugins()' '''
    return PyTPGManager.instance().scanPlugins()

def printPlugins():
    '''Short-cut to 'PyTPGManager.instance().printPlugins()' '''
    return PyTPGManager.instance().printPlugins()

def init():
    '''Initialises the PyTPG connection between Python and C++'''
    man = PyTPGManager.instance()
    if PyTPGManagerInst and PyTPGManagerInst != man:
        Cam._registerPyTPGManager_(man)

class PyTPGBase(object):
    '''The Base class for all Python TPG's'''
    ## Class attributes ##
    id = None # Generate once by running 'import uuid; str(uuid.uuid1())' in python e.g. 'b7b4ada0-23e8-11e2-a9d9-08002734b94f'
    name = "Un-named" # Should match Class name
    description = "This PyTPG forgot to override the description" # to help the user choose your TPG
    
    def __init__(self):
        '''The constructor.  Only called when the PyTPG is added to the FreeCAD project'''
    
    ## Support methods ##
    
    ## C++ to Python API ##
    # Class methods for TPG list #
    @classmethod
    def getId(cls):
        '''Get the Unique identifier of this PyTPG'''
        return cls.id
    
    @classmethod
    def getName(cls):
        '''Get the user-friendly name of this PyTPG'''
        return cls.name
    
    @classmethod
    def getDescription(cls):
        '''Get the description of this PyTPG'''
        return cls.description
    
    # Methods for once added to Project #
    def getActions(self):
        '''Returns a list of actions that this PyTPG offers'''
        return []

    def getSettings(self, action=None):
        '''Returns a list of settings that the PyTPG 'Action' will take.  If 
        action is None, then this will return a dictionary of all actions 
        settings.  Each setting will be in format of (<name>, <label>, <type>, <defaultvalue>, <units>, <helptext>)'''
        if action:
            return []
        return {}
        
    def run(self, action, settings=[]):
        '''Runs the selected action and returns the resulting TP'''
        raise UnimplementedTPError(action) # The run() method of this TP hasn't been implemented
    
    ## Python to C++ API ##
    # Call these methods to obtain data from the C++ side
    
    #TODO: define Python to C++ API
    
# End PyTPGBase

class PyTPGManager(object):
    '''The Manager class on the Python side of the API.'''
    
    _instance_ = None
    @classmethod
    def instance(cls):
        '''Obtain a reference to the PyTPGManager singleton'''
        if not cls._instance_:
            cls._instance_ = PyTPGManager()
        return cls._instance_
    
    def __init__(self, searchpath=['PyTPG']):
        '''Use PyTPGManager.instance() to get the singleton instance'''
        self._searchpath = searchpath
        self._tpgs = {} # elements [<class>, <instance>]

    ## C++ to Python API ##
    def scanPlugins(self):
        '''Search for Python TPGs'''
        try:
            self._scanDirs()
        except:
            pass
        plugins = self._tpgs.keys()
        return plugins
    
    def getPlugin(self, pid):
        '''Gets the Python TPG Class (as opposed to Instance) for the selected id'''
        if pid in self._tpgs:
            return self._tpgs[pid][0]
        return None
    
    def printPlugins(self):
        '''Print out a user-friendly list of plugins'''
        print "Known python plugins (TPGs):"
        for key in self._tpgs:
            tpgcls = self._tpgs[key][0]
            print "> %s [%s] '%s'" % (tpgcls.getName(), tpgcls.getId(), tpgcls.getDescription())
    
    def pytest(self, val):
        print "pytest(%s) [%s]" % (val, type(val))
        self.val = val
    
    ## Support Methods ##
    def _scanDirs(self, reload = False):
        '''Scans the entire searchpath for TPGs'''
        for path in self._searchpath:
            self._scanDir(path, reload)
        
    def _scanDir(self, path, reload = False):
        '''Scans <path> package for TPGs'''
        package = __import__(path, globals(), locals(), [], -1)
        # loop through all modules (or special packages @see examples in tpg folder)
        for modname in dir(package):
            mod = getattr(package, modname)
            if type(mod) == types.ModuleType:
                # loop through all the definitions in the module
                for classname in dir(mod):
                    cls = getattr(mod, classname)
                    # filter out only Classes that extend the PyTPGBase superclass
                    if inspect.isclass(cls) and issubclass(cls, PyTPGBase) and cls != PyTPGBase:
                        if reload or classname not in self._tpgs:
                            self._tpgs[cls.getID()] = [cls, None]
            # check if they included a class rather than a module
            elif inspect.isclass(mod) and issubclass(mod, PyTPGBase) and mod != PyTPGBase:
                if reload or modname not in self._tpgs:
                    self._tpgs[mod.getId()] = [mod, None]
# End PyTPGManager


class TPError(Exception):
    '''A super class for all Tool Path Errors'''
    

class UnimplementedTPError(TPError):
    def __init__(self, name):
        self.name = name
    def __str__(self):
        return "Unimplemented Tool Path Runner: '%s'" % self.name
    
    
