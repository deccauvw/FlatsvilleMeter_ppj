//
// Created by Orstphone on 2024-04-06.
//

#pragma once

//juce essentials
#include<juce_audio_basics/juce_audio_basics.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include "juce_audio_processors/juce_audio_processors.h"

//meter "needle point" line
#include "GuiComponents/BarMeterComponent.h"
#include "GuiComponents/TinyStripComponent.h"
#include "GuiComponents//BarMeterHelper.h"

#include "GuiModules/FacePlateGui.h"
//#include <stdio.h>

//FlatsDsp
//#include "DspModules/DspModulesHelper.h"
#include "DspModules/AnalogVuMeterProcessor.h"

#include <vector>
#include <unordered_map>
#include <algorithm>