/*
  ==============================================================================

    FacePlateGui.h
    Created: 19 Mar 2024 2:04:36am
    Author:  Decca

  ==============================================================================
*/

#pragma once
#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "FacePlateHelper.h"

//==============================================================================

namespace Face {
    class FacePlateGui : public juce::Component
    {
    public:
        FacePlateGui();
        ~FacePlateGui() override;
        void loadAssets();
        void paint(juce::Graphics& g) override;
        void paintOverChildren(juce::Graphics& g) override;
        void resized() override;

        ///assets getters     >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        [[nodiscard]] juce::Image getFaceplateFrameBody() const;


    private:
        //image instances
        juce::Image imageFaceplateBody;
        juce::Image imageLabelNumericStrip;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FacePlateGui)
    };

}