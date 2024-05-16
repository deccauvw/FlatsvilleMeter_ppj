//
// Created by Orstphone on 2024-04-07.
//

#include "DspModulesHelper.h"

namespace DspLine
{
    using mat = juce::dsp::Matrix<float>;
    SystemMatrices::SystemMatrices() : systemDim (Constants::kSystemOrder),
                                       ssm_v2i_x (systemDim, 1),
                                       ssm_v2i_x0 (systemDim, 1),
                                       ssm_v2i_A (systemDim, systemDim),
                                       ssm_v2i_B (systemDim,1),
                                       ssm_v2i_C (1, systemDim),
                                       ssm_v2i_D (1, 1),
                                       ssm_i2a_x (systemDim, 1),
                                       ssm_i2a_x0 (systemDim, 1),
                                       ssm_i2a_A (systemDim, systemDim),
                                       ssm_i2a_B (systemDim, 1),
                                       ssm_i2a_C (1, systemDim),
                                       ssm_i2a_D (1, 1)
    {
        ssm_v2i_x.clear();
        ssm_v2i_x0.clear();
        ssm_v2i_A.clear();
        ssm_v2i_B.clear();
        ssm_v2i_C.clear();
        ssm_v2i_D.clear();

        ssm_i2a_x.clear();
        ssm_i2a_x0.clear();
        ssm_i2a_A.clear();
        ssm_i2a_B.clear();
        ssm_i2a_C.clear();
        ssm_i2a_D.clear();
        setMatrices();
    }

    SystemMatrices::~SystemMatrices() = default;

    void SystemMatrices::setMatrices()
    {
        auto& d = Constants::kSystemOrder;
        ssm_v2i_x.clear();
        ssm_v2i_x0.clear();
        fillElementsWithArray (ssm_v2i_A, Constants::v2i_A, d * d);
        fillElementsWithArray (ssm_v2i_B, Constants::v2i_B, d);
        fillElementsWithArray (ssm_v2i_C, Constants::v2i_C, d);
        fillElementsWithArray (ssm_v2i_D, Constants::v2i_D, 1);
        ssm_i2a_x.clear();
        ssm_i2a_x0.clear();
        fillElementsWithArray (ssm_i2a_A, Constants::i2a_A, d * d);
        fillElementsWithArray (ssm_i2a_B, Constants::i2a_B, d);
        fillElementsWithArray (ssm_i2a_C, Constants::i2a_C, d);
        fillElementsWithArray (ssm_i2a_D, Constants::i2a_D, 1);
    }

    void SystemMatrices::fillElementsWithArray (mat &matrix, float arr[], int numOfElements)
    {
        auto dim = matrix.getSize();
        auto row = dim[0];
        auto col = dim[1];
//        jassert(row * col == numOfElements);

        auto _idx =
            [&row, &col](unsigned long long r, unsigned long long c) {
                return row * c + r;
            };

        jassert(numOfElements == row * col);


        for(auto r = 0; r < row; r++)
        {
            for (auto c = 0; c < col; c++)
            {
                matrix(r, c) = arr[_idx(r, c)];
            }
        }
    }

    std::vector<mat> SystemMatrices::returnAllSystemMatrices(int systemNumber)
    {
        jassert(systemNumber == 1 || systemNumber == 2);
        std::vector<mat> v;
        switch(systemNumber)
        {
            case 1:
                v = { ssm_v2i_A, ssm_v2i_B, ssm_v2i_C, ssm_v2i_D };
                break;
            case 2:
                v = { ssm_i2a_A, ssm_i2a_B, ssm_i2a_C, ssm_i2a_D };
                break;
            default:
                jassert("invalid system number.");
                break;
        }
        return v;
    }

} // DspLine