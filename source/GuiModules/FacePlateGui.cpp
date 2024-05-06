/*
  ==============================================================================

    FacePlateGui.cpp
    Created: 19 Mar 2024 2:04:36am
    Author:  Decca

  ==============================================================================
*/

//#include "juce_graphics/juce_graphics.h"
#include "FacePlateGui.h"
#include "BinaryData.h"
#include <vector>
using namespace Face;
using ImageFileFormat = juce::ImageFileFormat;
using File = juce::File;
//==============================================================================
Face::FacePlateGui::FacePlateGui() = default;
Face::FacePlateGui::~FacePlateGui() = default;

void FacePlateGui::loadAssets()
{
    imageFaceplateBody = juce::ImageCache::getFromMemory(BinaryData::FaceplateCanvas_png, BinaryData::FaceplateCanvas_pngSize);
    imageLabelNumericStrip = juce::ImageCache::getFromMemory(BinaryData::label)
}



void FacePlateGui::paint (juce::Graphics& g)
{
    loadAssets();
    g.drawImage(
        imageFaceplateBody,0, 0, Face::Constants::kFaceSizeWidth,Face::Constants::kFaceSizeHeight,
        0, 0, imageFaceplateBody.getWidth(), imageFaceplateBody.getHeight());

}

void FacePlateGui::paintOverChildren(juce::Graphics& g)
{
    juce::ignoreUnused(g);
 }


void FacePlateGui::resized()
{
}

juce::Image FacePlateGui::getFaceplateFrameBody() const
{
    return imageFaceplateBody;
}

