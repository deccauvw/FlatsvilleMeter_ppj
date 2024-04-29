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
using namespace Face;
using ImageFileFormat = juce::ImageFileFormat;
using File = juce::File;
//==============================================================================
Face::FacePlateGui::FacePlateGui() = default;


void FacePlateGui::loadAssets()
{
    imageFaceplateBody = juce::ImageCache::getFromFile(filedirFaceplateBody);
    imageGlassSheenReflection = juce::ImageCache::getFromFile(filedirGlassSheenReflection);
    imageNumericPrintedValuesLrVu = juce::ImageCache::getFromFile(filedirNumericPrintedValuesLrVu);
    //convert these into binary data
    extern const char imageFaceplateBody[];
}



void FacePlateGui::paint (juce::Graphics& g)
{
    g.drawImage(
        imageFaceplateBody,
        Face::Constants::kFacePositionX,
        Face::Constants::kFacePositionY,
        Face::Constants::kFaceSizeWidth,
        Face::Constants::kFaceSizeHeight,
        Face::Constants::kFacePositionX,
        Face::Constants::kFacePositionY,
        imageFaceplateBody.getWidth(),
        imageFaceplateBody.getHeight());
}

void FacePlateGui::resized()
{
}

juce::Image FacePlateGui::getFaceplateFrameBody() const
{
    return imageFaceplateBody;
}


juce::Image FacePlateGui::getImageGlassSheenReflection() const
{
    return imageGlassSheenReflection;
}



juce::Image FacePlateGui::getImageNumericPrintedValuesLrVu() const
{
    return imageNumericPrintedValuesLrVu;
}

