//
// Created by Decca on 2024-04-19.
//

#ifndef FLATSVILLEMETER_BARMETERCOMPONENT_H
#define FLATSVILLEMETER_BARMETERCOMPONENT_H
#include "juce_gui_basics/juce_gui_basics.h"
#include "BarMeterHelper.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"

namespace Gui
{

    class BarMeterComponent: public juce::Component
    {
    public:
        BarMeterComponent():m_levelValue(0.0f)
        {}
        ~BarMeterComponent() override;

        void paint (juce::Graphics& g)override;
        void setLevel (float value);

    private:
        float m_levelValue = 0.0f;

    };

} // Gui

#endif //FLATSVILLEMETER_BARMETERCOMPONENT_H
