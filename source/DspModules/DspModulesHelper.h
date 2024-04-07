//
// Created by prmir on 2024-04-07.
//

#pragma once
#include "juce_audio_processors/juce_audio_processors.h"  //juceheader
#include "juce_dsp/juce_dsp.h"
#include <vector>


namespace DspLine
{
    //@brief constant Matrix contents.
    namespace Constants{
        //System Dimension (Order 4) =====================================================================
        static constexpr auto kSystemOrder = 4;

        //Matrix element data for System Voltage to Current (System I) ==================================
        float v2i_A[kSystemOrder * kSystemOrder] = {
            -10.0,
            0.0,
            0.0,
            -10.0,

            -9.671e+6,
            -45.45,
            -9.671e+6,
            9.671,

            -21.28,
            0.0,
            -21.28,
            -2.128e-5,

            -0.01,
            0.0,
            1.0,
            -0.21
        };

        float v2i_B[kSystemOrder] = {
            10.0,
            9.671e+6,
            21.28,
            0.01
        };

        float v2i_C[kSystemOrder] = {
            8.674e-19,
            -8.674e-19,
            8.078e-28,
            -1.654e-24
        };

        float v2i_D[kSystemOrder] = {
            -8.674e-19
        };

        //state space equation matrices for CUR -> ANGLE : system II=====================================
        float i2a_A[kSystemOrder * kSystemOrder] = {
            -19.84,
            8.746,
            -39.940,
            -1232.0,

            169.6,
            -100.0,
            456.7,
            14080.0,

            26.18,
            0.0,
            -24.41,
            -150.6,

            0.0,
            0.0,
            1.0,
            0.0
        };

        float i2a_B[kSystemOrder] = {
            0.2572,
            0,
            0,
            0
        };

        float i2a_C[kSystemOrder] = {
            1.696,
            0,
            4.567,
            140.8
        };

        float i2a_D[kSystemOrder] = {
            0
        };

        //Matrix constructing ==============================

    }; //struct Constant

//Matrix initializing=============================================================================
    using namespace Constants;
    class initializeMatrices
    {
    private:
        //initializeMatrices();
    public:
        using mat = juce::dsp::Matrix<float>;
        explicit initializeMatrices();
        ~initializeMatrices();
        //==================================================
        //for system I
        size_t systemDim;
        mat ssm_v2i_x;
        mat ssm_v2i_x0;
        mat ssm_v2i_A;
        mat ssm_v2i_B;
        mat ssm_v2i_C;
        mat ssm_v2i_D;
        //for system II
        mat ssm_i2a_x;
        mat ssm_i2a_x0;
        mat ssm_i2a_A;
        mat ssm_i2a_B;
        mat ssm_i2a_C;
        mat ssm_i2a_D;
    };

} // DspLine
