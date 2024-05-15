//
// Created by orstphone@github on 2024-04-07.
//

#pragma once
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"
#include <vector>


namespace FlatsDsp
{
    //@brief constant Matrix contents.
    namespace Constants
    {
        //System Dimension (Order 4) =====================================================================
        static constexpr auto kSystemOrder = 4;
        static constexpr auto kMinimalReturnValue = -20.0f;
        //Matrix element data for System Voltage to Current (System I) ==================================
        static float v2i_A[kSystemOrder * kSystemOrder] = {
            -10.0f,
            0.0f,
            0.0f,
            -10.0f,

            -9.671e+6f,
            -45.45f,
            -9.671e+6f,
            9.671f,

            -21.28f,
            0.0f,
            -21.28f,
            -2.128e-5f,

            -0.01f,
            0.0f,
            1.0f,
            -0.21f
        };

        static float v2i_B[kSystemOrder] = {
            10.0f,
            9.671e+6f,
            21.28f,
            0.01f
        };

        static float v2i_C[kSystemOrder] = {
            8.674e-19f,
            -8.674e-19f,
            8.078e-28f,
            -1.654e-24f
        };

        static float v2i_D[kSystemOrder] = {
            -8.674e-19f
        };

        //state space equation matrices for CUR -> ANGLE : system II=====================================
        static float i2a_A[kSystemOrder * kSystemOrder] = {
            -19.84f,
            8.746f,
            -39.940f,
            -1232.0f,

            169.6f,
            -100.0f,
            456.7f,
            14080.0f,

            26.18f,
            0.0f,
            -24.41f,
            -150.6f,

            0.0f,
            0.0f,
            1.0f,
            0.0f
        };

        static float i2a_B[kSystemOrder] = {
            0.2572f,
            0.f,
            0.f,
            0.f,
        };

        static float i2a_C[kSystemOrder] = {
            1.696f,
            0.0f,
            4.567f,
            140.8f
        };

        static float i2a_D[kSystemOrder] = {
            0.0f
        };

    } //struct Constant

//Matrix initializing=============================================================================
    //using namespace Constants;
    struct SystemMatrices
    {
        explicit SystemMatrices() : systemDim(Constants::kSystemOrder){};
        SystemMatrices(int  d) : systemDim(d){};

        using mat = juce::dsp::Matrix<float>;
        size_t systemDim;

        mat ssm_v2i_x = mat(systemDim, 1);
        mat ssm_v2i_x0 = mat(systemDim, 1);

        mat ssm_v2i_A = mat(systemDim, systemDim, Constants::v2i_A);
        mat ssm_v2i_B = mat(systemDim, 1, Constants::v2i_B);
        mat ssm_v2i_C = mat(1, systemDim, Constants::v2i_C);
        mat ssm_v2i_D = mat(1, 1, Constants::v2i_D);

        mat ssm_i2a_x = mat(systemDim, 1);
        mat ssm_i2a_x0 = mat(systemDim, 1);

        mat ssm_i2a_A = mat(systemDim, systemDim, Constants::i2a_A);
        mat ssm_i2a_B = mat(systemDim, 1, Constants::i2a_B);
        mat ssm_i2a_C = mat(1, systemDim, Constants::i2a_C);
        mat ssm_i2a_D = mat(1, 1, Constants::i2a_D);

    };
//    class SystemMatrices
//    {
//    public:
//        using mat = juce::dsp::Matrix<float>;
//        explicit SystemMatrices();
//        ~SystemMatrices();
//        //member functions ====
//        void setMatrices();
//
//        //member variables ====
//        //for system I
//        size_t systemDim;
//
//        mat ssm_v2i_x;
//        mat ssm_v2i_x0;
//        mat ssm_v2i_A;
//        mat ssm_v2i_B;
//        mat ssm_v2i_C;
//        mat ssm_v2i_D;
//        //for system II
//        mat ssm_i2a_x;
//        mat ssm_i2a_x0;
//        mat ssm_i2a_A;
//        mat ssm_i2a_B;
//        mat ssm_i2a_C;
//        mat ssm_i2a_D;
//    private:
//    };
    enum MeterBallisticsTypeEnumerator
    {
        TYPE_BLANK = 0,
        TYPE_PEAK = 1,
        TYPE_RMS = 2,
        TYPE_VU = 3,
        TYPE_LAST
    };

    struct MeterBallisticsValueSupplierMemberFnEncoder
    {
        MeterBallisticsTypeEnumerator MBTenum;
        int mbtEncodedValue;
        int channelNumber;
        std::function<float()> fn;
        MeterBallisticsValueSupplierMemberFnEncoder():  MBTenum(TYPE_BLANK),
                                                        mbtEncodedValue(TYPE_BLANK),
                                                        channelNumber(0),
                                                        fn([]()->float{return 0.0f;})
                                                            {};

        MeterBallisticsValueSupplierMemberFnEncoder(
            MeterBallisticsTypeEnumerator MBTenumInput,
            int channelNumberInput,
            std::function<float()>& valueSupplierInput):
                                                           MBTenum(MBTenumInput),
                                                           mbtEncodedValue(MBTenumInput),
                                                           channelNumber(channelNumberInput),
                                                           fn(std::move(valueSupplierInput))
                                                               {};
    };

} // FlatsDsp
