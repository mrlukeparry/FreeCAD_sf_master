/***************************************************************************
* Copyright (c) Luke Parry (l.parry@warwick.ac.uk) 2012 *
* *
* This file is part of the FreeCAD CAx development system. *
* *
* This library is free software; you can redistribute it and/or *
* modify it under the terms of the GNU Library General Public *
* License as published by the Free Software Foundation; either *
* version 2 of the License, or (at your option) any later version. *
* *
* This library is distributed in the hope that it will be useful, *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
* GNU Library General Public License for more details. *
* *
* You should have received a copy of the GNU Library General Public *
* License along with this library; see the file COPYING.LIB. If not, *
* write to the Free Software Foundation, Inc., 59 Temple Place, *
* Suite 330, Boston, MA 02111-1307, USA *
* *
***************************************************************************/

#ifndef _CAM_TPGFACTORYINST_h_
#define _CAM_TPGFACTORYINST_h_

#include <Base/Exception.h>
#include <Base/Factory.h>
#include <QStringList>
#include <boost/iterator/iterator_concepts.hpp>

#include "TPG.h"
//#include "../Support.h"

namespace Cam
{

/**
  * Basic container structure for storing the TPG basic information for the plugins library
  * Intended to be lightweight
  */
struct TPGStruc
{
public:
    const char *id;
    std::string name;
    std::string desc;
    TPG::Type type;
};

/**
 * A superclass for TPG Descriptors.  These describe the basic information
 * about a TPG and contain a method to create a new instance of this TPG.
 */
class TPGDescriptor
{
public:
  QString id;
  QString name;
  QString description;

  QString type; // subclasses should set this to their classname in the constructor

  TPGDescriptor(QString id, QString name, QString description, QString type) {
    this->id = id;
    this->name = name;
    this->description = description;
    this->type = type;
  }
  virtual ~TPGDescriptor() {}

  /**
   * Creates a new instance of this TPG.  Sub-classes need to implement this
   */
  virtual TPG* make() = 0;
  virtual void print()
  {
    printf("- ('%s', '%s', '%s', '%s')\n",
        id.toAscii().constData(),
        name.toAscii().constData(),
        description.toAscii().constData(),
        type.toAscii().constData());
  }
};

/**
  * The actual static initialisation of the TPGFactory
  */
class TPGFactoryInstP
{
public:
//  std::vector<TPGStruc> tpgList;
  std::vector<TPGDescriptor*> tpgList;
  QString userPluginsPath;
};

class CamExport TPGFactoryInst : public Base::Factory
{
public:

  static TPGFactoryInst& instance(void);

  static void destruct (void);
  TPG * getPluginById(const char *id);

  const std::vector<TPGDescriptor*> & getPluginList() { return d->tpgList; }

  void scanPlugins();
  void setUserPluginPath(const char *);

  template <class CLASS>
  void registerPlugin(const char *id, const char *name, const char *desc, TPG::Type type);

private:
  TPGFactoryInst(void);
  ~TPGFactoryInst(void);
  static TPGFactoryInst* _pcSingleton;

  QStringList scanDirectory(const QString &path);
  void clearPlugins() {}

  TPGFactoryInstP* d;
};

/// Get the global instance
inline TPGFactoryInst& TPGFactory(void)
{
    return TPGFactoryInst::instance();
}

/**
 * Producer is used to register C++ TPG Plugins and produce the objects
 * */
template <class CLASS>
class CamExport TPGProducer : public Base::AbstractProducer
{
public:
    TPGProducer (const char* name)
    {
        TPGFactory().AddProducer(name, this);
    }

    virtual ~TPGProducer(){}
    void setValue(int v)
    {
        this->value = v;
    }
    virtual void* Produce () const
    {
        CLASS inst;
        return (void*)inst.makeTPG(); // Function Pointer
    }

    int value;
};

// Unfortunatly we have to implement this after the TPGProducer and in the header file.
template <class CLASS>
void TPGFactoryInst::registerPlugin(const char *id, const char *name, const char *desc, TPG::Type type)
{
    // We need to check if plugin of name has been registered and throw exception
    std::map<const std::string, Base::AbstractProducer*>::const_iterator it;
    it = _mpcProducers.find(name);

    if (it != _mpcProducers.end())
        throw new Base::Exception("A plugin with this ID has already been registered");

    //We must register the producer
    new Cam::TPGProducer<CLASS>(name);

    // We now create a light TPG Description to add to the library
//    TPGStruc tpgDesc;
//    tpgDesc.id = id;
//    tpgDesc.name = name;
//    tpgDesc.desc = desc;
//    tpgDesc.type = type;
//    d->tpgList.push_back(tpgDesc);

    //Store some useful information related to the plugin
};
}

#endif //_CAM_TPGFACTORYINST_h_
