/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <sstream>
#endif

#include <Base/Exception.h>
#include <Base/Console.h>
#include <Base/FileInfo.h>
#include <App/Application.h>

#include <boost/regex.hpp>
#include <iostream>
#include <iterator>

#include "FeaturePage.h"
#include "FeatureView.h"
#include "FeatureClip.h"

using namespace Drawing;
using namespace std;


//===========================================================================
// FeaturePage
//===========================================================================

PROPERTY_SOURCE(Drawing::FeaturePage, App::DocumentObjectGroup)

const char *group = "Drawing view";

FeaturePage::FeaturePage(void) 
{
    static const char *group = "Drawing view";
    ADD_PROPERTY_TYPE(Template ,(""),group, (App::PropertyType) App::Prop_None,"Template for the page");
    ADD_PROPERTY_TYPE(Views    ,(0), group, (App::PropertyType)(App::Prop_None),"Attached Views");
}

FeaturePage::~FeaturePage()
{
}

short FeaturePage::mustExecute() const
{
    // If Tolerance Property is touched
    if(Template.isTouched())
        return 1;
    
    // Check if within the selection, any Document Object have been touched
    bool ViewsTouched = false;
    const std::vector<App::DocumentObject*> &vals = Views.getValues();
    for(std::vector<App::DocumentObject *>::const_iterator it = vals.begin(); it < vals.end(); ++it) {
        if((*it)->isTouched()) {
            ViewsTouched = true;            
        }
    }

    return (ViewsTouched) ? 1 : App::DocumentObjectGroup::mustExecute();
}

/// get called by the container when a Property was changed
void FeaturePage::onChanged(const App::Property* prop)
{
    if (prop == &Template) {
        if (!this->isRestoring()) {
        }
    } else if (prop == &Views) {
        Base::Console().Log("views changed -> feature page \n");
    }
    App::DocumentObjectGroup::onChanged(prop);
}

int FeaturePage::addView(App::DocumentObject *docObj)
{
    if(!docObj->isDerivedFrom(Drawing::FeatureView::getClassTypeId()))
        return -1; //Doc Object mst be derived from a Part Feature

    const std::vector<App::DocumentObject *> vals = Views.getValues();

    std::vector<App::DocumentObject *> newVals(vals);
    newVals.push_back(docObj);
    Views.setValues(newVals);

    return Views.getSize();
}

App::DocumentObjectExecReturn *FeaturePage::execute(void)
{
    // get through the children and collect all the views
    const std::vector<App::DocumentObject*> &Grp = Views.getValues();
    for (std::vector<App::DocumentObject*>::const_iterator it = Grp.begin();it!=Grp.end();++it) {
        if ( (*it)->getTypeId().isDerivedFrom(Drawing::FeatureView::getClassTypeId()) ) {
            Drawing::FeatureView *View = dynamic_cast<Drawing::FeatureView *>(*it);
        } else if ( (*it)->getTypeId().isDerivedFrom(Drawing::FeatureClip::getClassTypeId()) ) {
            Drawing::FeatureClip *Clip = dynamic_cast<Drawing::FeatureClip *>(*it);        
        }
    }
    Views.touch();
    return App::DocumentObject::StdReturn;
}
//     std::string temp = Template.getValue();
//     if (temp.empty())
//         return App::DocumentObject::StdReturn;
// 
//     Base::FileInfo fi(temp);
//     if (!fi.isReadable()) {
//         // if there is a old absolute template file set use a redirect
//         fi.setFile(App::Application::getResourceDir() + "Mod/Drawing/Templates/" + fi.fileName());
//         // try the redirect
//         if (!fi.isReadable()) {
//             Base::Console().Log("FeaturePage::execute() not able to open %s!\n",Template.getValue());
//             std::string error = std::string("Cannot open file ") + Template.getValue();
//             return new App::DocumentObjectExecReturn(error);
//         }
//     }
// 
//     if (std::string(PageResult.getValue()).empty())
//         PageResult.setValue(fi.filePath().c_str());
// 
//     // open Template file
//     string line;
//     ifstream file (fi.filePath().c_str());
// 
//     // make a temp file for FileIncluded Property
//     string tempName = PageResult.getExchangeTempFile();
//     ostringstream ofile;
//     string tempendl = "--endOfLine--";
// 
//     while (!file.eof())
//     {
//         getline (file,line);
//         // check if the marker in the template is found
//         if(line.find("<!-- DrawingContent -->") == string::npos)
//             // if not -  write through
//             ofile << line << tempendl;
//         else
//         {
//      
//         }
//     }
// 
//     file.close();
// 
//     // checking for freecad editable texts
//     string outfragment(ofile.str());
//     if (EditableTexts.getSize() > 0) {
//         boost::regex e1 ("<text.*?freecad:editable=\"(.*?)\".*?<tspan.*?>(.*?)</tspan>");
//         string::const_iterator begin, end;
//         begin = outfragment.begin();
//         end = outfragment.end();
//         boost::match_results<std::string::const_iterator> what;
//         int count = 0;
// 
//         while (boost::regex_search(begin, end, what, e1)) {
//             if (count < EditableTexts.getSize()) {
//                 // change values of editable texts
//                 boost::regex e2 ("(<text.*?freecad:editable=\""+what[1].str()+"\".*?<tspan.*?)>(.*?)(</tspan>)");
//                 outfragment = boost::regex_replace(outfragment, e2, "$1>"+EditableTexts.getValues()[count]+"$3");
//             }
//             count ++;
//             begin = what[0].second;
//         }
//     }
// 
//     // restoring linebreaks and saving the file
//     boost::regex e3 ("--endOfLine--");
//     string fmt = "\\n";
//     outfragment = boost::regex_replace(outfragment, e3, fmt);
//     ofstream outfinal(tempName.c_str());
//     outfinal << outfragment;
//     outfinal.close();
// 
//     PageResult.setValue(tempName.c_str());
// 
//     //const char* text = "lskdfjlsd";
//     //const char* regex = "lskdflds";
//     //boost::regex e(regex);
//     //boost::smatch what;
//     //if(boost::regex_match(string(text), what, e))
//     //{
//     //}


#if 0
std::vector<std::string> FeaturePage::getEditableTextsFromTemplate(void) const {
    //getting editable texts from "freecad:editable" attributes in SVG template

    std::vector<string> eds;

    std::string temp = Template.getValue();
    if (!temp.empty()) {
        Base::FileInfo tfi(temp);
        if (!tfi.isReadable()) {
            // if there is a old absolute template file set use a redirect
            tfi.setFile(App::Application::getResourceDir() + "Mod/Drawing/Templates/" + tfi.fileName());
            // try the redirect
            if (!tfi.isReadable()) {
                return eds;
            }
        }
        string tline, tfrag;
        ifstream tfile (tfi.filePath().c_str());
        while (!tfile.eof()) {
            getline (tfile,tline);
            tfrag += tline;
            tfrag += "--endOfLine--";
        }
        tfile.close();
        boost::regex e ("<text.*?freecad:editable=\"(.*?)\".*?<tspan.*?>(.*?)</tspan>");
        string::const_iterator tbegin, tend;
        tbegin = tfrag.begin();
        tend = tfrag.end();
        boost::match_results<std::string::const_iterator> twhat;
        while (boost::regex_search(tbegin, tend, twhat, e)) {
            eds.push_back(twhat[2]);
            tbegin = twhat[0].second;
        }
    }
    return eds;
}
#endif