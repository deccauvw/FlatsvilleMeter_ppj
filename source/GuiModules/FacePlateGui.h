/*
  ==============================================================================

    FacePlateGui.h
    Created: 19 Mar 2024 2:04:36am
    Author:  Decca

  ==============================================================================
*/

#pragma once

#include "juce_graphics/juce_graphics.h"

//==============================================================================

namespace Gui {
    class FacePlateGui : public juce::Component
    {
    public:
        FacePlateGui();
        //~FacePlateGui() = default;

        void loadAssets();

        void paint(juce::Graphics& g) override;

        //void paintOverChildren(juce::Graphics& g) override;

        void resized() override;


        ///assets getters     >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        [[nodiscard]] juce::Image getFaceplateFrameBody() const;
        [[nodiscard]] juce::Image getImageGlassSheenReflection() const;
        [[nodiscard]] juce::Image getImageNumericPrintedValuesLrVu() const;




    private:
        //directories
        juce::File filedirFaceplateBody = juce::String("Assets\\FaceplateCanvas.png");
        juce::File filedirGlassSheenReflection = juce::String("Assets\\glassSheen.png");
        juce::File filedirNumericPrintedValuesLrVu = juce::String("Assets\\Values.png");

        //image instances
        juce::Image imageFaceplateBody;
        juce::Image imageGlassSheenReflection;
        juce::Image imageNumericPrintedValuesLrVu;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FacePlateGui)
    };

}