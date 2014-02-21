/***************************************************************************
 *   Copyright (c) 2014 Luke Parry <l.parry@warwick.ac.uk>                 *
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
#include <Base/Interpreter.h>
#include <Base/FileInfo.h>

#include <App/Application.h>

#include <boost/regex.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <iostream>
#include <iterator>

#include "FeatureSVGTemplate.h"

//#include "FeatureSVGTemplatePy.h"

using namespace Drawing;
using namespace std;

PROPERTY_SOURCE(Drawing::FeatureSVGTemplate, Drawing::FeatureTemplate)

FeatureSVGTemplate::FeatureSVGTemplate()
{
    static const char *group = "Drawing view";

    ADD_PROPERTY_TYPE(PageResult ,(0),group,App::Prop_Output,"Resulting SVG document of that page");
    ADD_PROPERTY_TYPE(Template   ,(""),group,App::Prop_None  ,"Template for the page");
    ADD_PROPERTY_TYPE(EditableTexts,(""),group,App::Prop_None,"Substitution values for the editable strings in the template");
}

FeatureSVGTemplate::~FeatureSVGTemplate()
{
}

#if 0
PyObject *FeatureSVGTemplate::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new FeatureSVGTemplatePy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}

#endif

unsigned int FeatureSVGTemplate::getMemSize(void) const
{
    return 0;
}

short FeatureSVGTemplate::mustExecute() const
{
    return Drawing::FeatureTemplate::mustExecute();
}

/// get called by the container when a Property was changed
void FeatureSVGTemplate::onChanged(const App::Property* prop)
{
    if (prop == &PageResult) {
        if (this->isRestoring()) {
            // When loading a document the included file
            // doesn't need to exist at this point.
            Base::FileInfo fi(PageResult.getValue());
            if (!fi.exists())
                return;
        }
    }

    if (prop == &Template) {
        if (!this->isRestoring()) {
            EditableTexts.setValues(getEditableTextsFromTemplate());
            this->touch();
        }
    }

    Drawing::FeatureTemplate::onChanged(prop);
}

App::DocumentObjectExecReturn *FeatureSVGTemplate::execute(void)
{
    std::string temp = Template.getValue();
    if (temp.empty())
        return App::DocumentObject::StdReturn;

    Base::FileInfo fi(temp);
    if (!fi.isReadable()) {
        // if there is a old absolute template file set use a redirect
        fi.setFile(App::Application::getResourceDir() + "Mod/Drawing/Templates/" + fi.fileName());
        // try the redirect
        if (!fi.isReadable()) {
            Base::Console().Log("FeaturePage::execute() not able to open %s!\n",Template.getValue());
            std::string error = std::string("Cannot open file ") + Template.getValue();
            return new App::DocumentObjectExecReturn(error);
        }
    }

    if (std::string(PageResult.getValue()).empty())
        PageResult.setValue(fi.filePath().c_str());

    // open Template file
    string line;
    ifstream file (fi.filePath().c_str());

    // make a temp file for FileIncluded Property
    string tempName = PageResult.getExchangeTempFile();
    ostringstream ofile;
    string tempendl = "--endOfLine--";

    while (!file.eof())
    {
        getline (file,line);
        // check if the marker in the template is found
        if(line.find("<!-- DrawingContent -->") == string::npos)
            // if not -  write through
            ofile << line << tempendl;
    }
    file.close();

    // checking for freecad editable texts
    string outfragment(ofile.str());

    if (EditableTexts.getSize() > 0) {
        boost::regex e1 ("<text.*?freecad:editable=\"(.*?)\".*?<tspan.*?>(.*?)</tspan>");
        string::const_iterator begin, end;
        begin = outfragment.begin();
        end = outfragment.end();
        boost::match_results<std::string::const_iterator> what;
        int count = 0;

        while (boost::regex_search(begin, end, what, e1)) {
            if (count < EditableTexts.getSize()) {
                // change values of editable texts
                boost::regex e2 ("(<text.*?freecad:editable=\""+what[1].str()+"\".*?<tspan.*?)>(.*?)(</tspan>)");
                outfragment = boost::regex_replace(outfragment, e2, "$1>"+EditableTexts.getValues()[count]+"$3");
            }
            count ++;
            begin = what[0].second;
        }
    }

    // restoring linebreaks and saving the file
    boost::regex e3 ("--endOfLine--");
    string fmt = "\\n";
    outfragment = boost::regex_replace(outfragment, e3, fmt);
    ofstream outfinal(tempName.c_str());
    outfinal << outfragment;
    outfinal.close();

    PageResult.setValue(tempName.c_str());
    this->touch();

    //const char* text = "lskdfjlsd";
    //const char* regex = "lskdflds";
    //boost::regex e(regex);
    //boost::smatch what;
    //if(boost::regex_match(string(text), what, e))
    //{
    //}
    return App::DocumentObject::StdReturn;
}

std::vector<std::string> FeatureSVGTemplate::getEditableTextsFromTemplate() const {
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

// Python Template feature ---------------------------------------------------------

#if 0
namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Drawing::FeatureSVGTemplatePython, Drawing::FeatureSVGTemplate)
template<> const char* Drawing::FeatureSVGTemplatePython::getViewProviderName(void) const {
    return "DrawingGui::ViewProviderPython";
}
/// @endcond

// explicit template instantiation
template class DrawingExport FeaturePythonT<Drawing::FeatureSVGTemplate>;
}

#endif