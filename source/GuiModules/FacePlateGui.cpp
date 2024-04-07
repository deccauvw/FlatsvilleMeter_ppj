/*
  ==============================================================================

    FacePlateGui.cpp
    Created: 19 Mar 2024 2:04:36am
    Author:  Decca

  ==============================================================================
*/

//#include "juce_graphics/juce_graphics.h"
#include "FacePlateGui.h"
#include <vector>

using ImageFileFormat = juce::ImageFileFormat;
using File = juce::File;
//==============================================================================
Gui::FacePlateGui::FacePlateGui() = default;
//{
//    imageFaceplateDisplay(ImageFileFormat::loadFrom(filedirFaceplateDisplay)),
//    imageFaceplateFrameBody(ImageFileFormat::loadFrom(filedirFaceplateFrameBody)),
//    imageFrameGuardRail(ImageFileFormat::loadFrom(filedirFrameGuardRail)),
//    imageGlassSheenReflection(ImageFileFormat::loadFrom(filedirGlassSheenReflection)),
//    imageNumericPrintedValuesLrVu(ImageFileFormat::loadFrom(filedirNumericPrintedValuesLrVu)),
//    imageHorizontalSmallDisplay(ImageFileFormat::loadFrom(filedirHorizontalSmallDisplay)),
//    imageTickpointAtZero(ImageFileFormat::loadFrom(filedirTickpointAtZero))
//}


void Gui::FacePlateGui::loadAssets()
{
    imageFaceplateBody = juce::ImageCache::getFromFile(filedirFaceplateBody);
    imageGlassSheenReflection = juce::ImageCache::getFromFile(filedirGlassSheenReflection);
    imageNumericPrintedValuesLrVu = juce::ImageCache::getFromFile(filedirNumericPrintedValuesLrVu);
    //juce::Image can be sized with rectangle
}



void Gui::FacePlateGui::paint (juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

//void Gui::FacePlateGui::paintOverChildren(juce::Graphics& g){}

void Gui::FacePlateGui::resized()
{
}

juce::Image Gui::FacePlateGui::getFaceplateFrameBody() const
{
    return imageFaceplateBody;
}


juce::Image Gui::FacePlateGui::getImageGlassSheenReflection() const
{
    return imageGlassSheenReflection;
}



juce::Image Gui::FacePlateGui::getImageNumericPrintedValuesLrVu() const
{
    return imageNumericPrintedValuesLrVu;
}

