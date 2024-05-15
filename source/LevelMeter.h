//
// Created by Orstphone on 2024-04-06.
//

#pragma once

//juce essentials
#include<juce_audio_basics/juce_audio_basics.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>


//meter "needle point" line
#include "GuiComponents/BarMeterComponent.h"
#include "GuiComponents/TinyStripComponent.h"

#include "GuiModules/FacePlateGui.h"
#include <stdio.h>
//#include "Components/HorizontalBarMeterHelpers.h"
//#include "Components/HorizontalBarMeterChannel.h"
//#include "Components/HorizontalBarMeterLevel.h"
//#include "Components/HorizontalBarMeterHeader.h"
//#include "Components/HorizontalBarMeterSegment.h"
//#include "Components/HorizontalBarMetersComponent.h"

//dsp line
#include "DspModules/DspModulesHelper.h"
#include "DspModules/AnalogVuMeterProcessor.h"