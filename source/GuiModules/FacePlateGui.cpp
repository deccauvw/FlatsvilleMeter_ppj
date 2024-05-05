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
}



void FacePlateGui::paint (juce::Graphics& g)
{
    //empty
}

void FacePlateGui::paintOverChildren(juce::Graphics& g)
{
    g.drawImage(
        imageFaceplateBody, getLocalBounds().toFloat());
}


void FacePlateGui::resized()
{
}

juce::Image FacePlateGui::getFaceplateFrameBody() const
{
    return imageFaceplateBody;
}

