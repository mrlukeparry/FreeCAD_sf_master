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

#include <Base/Factory.h>
#include <Base/Exception.h>
//#include <QStringList>
#include <boost/iterator/iterator_concepts.hpp>

#include "TPG.h"
//#include "../Support.h"

namespace Cam
{

//class TPG;

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
    this->id          = id;
    this->name        = name;
    this->description = description;
    this->type        = type;
  }
  /// Convenience method for using plain ascii string
  TPGDescriptor(const char *id, const char * name, const char * description, const char * type) {
    this->id          = QString::fromAscii(id);
    this->name        = QString::fromAscii(name);
    this->description = QString::fromAscii(description);
    this->type        = QString::fromAscii(type);
  }
  TPGDescriptor(const TPGDescriptor &copy) {
      this->id = copy.id;
      this->name = copy.name;
      this->description = copy.description;
      this->type = copy.type;
  }
  virtual ~TPGDescriptor() {}

  /**
   * Creates a new instance of this TPG.  Sub-classes need to implement this
   */
  virtual Cam::TPG* make() = 0;
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
  * The actual static initialisation of the TPGFactory that stores the list of tpgLists
  */
class TPGFactoryInstP
{
public:
  std::vector<TPGDescriptor*> tpgList;
};

class CamExport TPGFactoryInst // : public Base::Factory
{
//    TYPESYSTEM_HEADER();

public:

  static TPGFactoryInst& instance(void);

  static void destruct (void);
  TPG * getPlugin(QString id);

  const std::vector<TPGDescriptor*> & getPluginList() { return d->tpgList; }

//  template <class CLASS>
//  void registerPlugin(TPGDescriptor *);

private:
  TPGFactoryInst(void);
  ~TPGFactoryInst(void);
  static TPGFactoryInst* _pcSingleton;
  
  /// Used for clearing the registered descriptors for describing a TPG plugin
  void clearDescriptors();

  TPGFactoryInstP* d;
};

/// Get the global instance
inline TPGFactoryInst& TPGFactory(void)
{
    return TPGFactoryInst::instance();
}

///**
// * Producer is used to register C++ TPG Plugins and produce the objects
// * */
//template <class CLASS>
//class CamExport TPGProducer : public Base::AbstractProducer
//{
//public:
//    TPGProducer (const char* name)
//    {
//        TPGFactory().AddProducer(name, this);
//    }
//
//    virtual ~TPGProducer(){}
//    void setValue(int v)
//    {
//        this->value = v;
//    }
//    virtual void* Produce () const
//    {
//        CLASS inst;
//        return (void*)inst.makeTPG(); // Function Pointer
//    }
//
//    int value;
//};
//
//// Unfortunatly we have to implement this after the TPGProducer and in the header file.
//template <class CLASS>
//void TPGFactoryInst::registerPlugin(TPGDescriptor *descriptor)
//{
//    // We need to check if plugin of name has been registered and throw exception
//    std::map<const std::string, Base::AbstractProducer*>::const_iterator it;
//    it = _mpcProducers.find(descriptor->id.toStdString());
//
//    if (it != _mpcProducers.end())
//        throw new Base::Exception("A plugin with this ID has already been registered");
//
//    //We must register the producer
//    new Cam::TPGProducer<CLASS>(descriptor->id.toStdString().c_str());
//
//    //Store some useful information (the descriptor) related to the plugin
//    d->tpgList.push_back(descriptor);
//
//
//};
}

#endif //_CAM_TPGFACTORYINST_h_
