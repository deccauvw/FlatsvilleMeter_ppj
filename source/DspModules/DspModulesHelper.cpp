//
// Created by prmir on 2024-04-07.
//

#include "DspModulesHelper.h"

namespace DspLine
{
    initializeMatrices::initializeMatrices():
        systemDim(Constants::kSystemOrder),
                                               ssm_v2i_x(0,0),
                                               ssm_v2i_x0(0,0),
                                               ssm_v2i_A(0,0),
                                               ssm_v2i_B(0,0),
                                               ssm_v2i_C(0,0),
                                               ssm_v2i_D(0,0),
                                               ssm_i2a_x(0,0),
                                               ssm_i2a_x0(0,0),
                                               ssm_i2a_A(0,0),
                                               ssm_i2a_B(0,0),
                                               ssm_i2a_C(0,0),
                                               ssm_i2a_D(0,0)
    {
        ssm_v2i_x (systemDim, 1);
        ssm_v2i_x0 (systemDim, 1);
        ssm_v2i_A (systemDim, systemDim);
        ssm_v2i_B (1, systemDim);
        ssm_v2i_C (systemDim, 1);
        ssm_v2i_D (1, 1);

        ssm_i2a_x (systemDim, 1);
        ssm_i2a_x0 (systemDim, 1);
        ssm_i2a_A (systemDim, systemDim);
        ssm_i2a_B (1, systemDim);
        ssm_i2a_C (systemDim, 1);
        ssm_i2a_D (1, 1);

    }

    initializeMatrices::~initializeMatrices()
    {}
} // DspLine