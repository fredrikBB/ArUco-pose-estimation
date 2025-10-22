#pragma once

#include <opencv2/opencv.hpp>
using namespace cv;

/* Camera parameters for OnePlus 11 caluculated using calibrate_camera.cpp
 * Date: 2025-10-22
 */
Mat OnePlus11CameraMatrix = (Mat_<double>(3, 3) << 
    3014.044254828271, 0, 1528.275218587665,
    0, 2997.408167857721, 1925.62623776718,
    0, 0, 1);
Mat OnePlus11DistCoeffs = (Mat_<double>(1, 5) <<
    -0.1941553080712784, -1.392641662440838, -0.005725443843629925, -0.003499506162759533, 2.776044852295584);