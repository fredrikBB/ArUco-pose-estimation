#include <stdio.h>
#include <opencv2/opencv.hpp>

#include "camera_parameters.h"

using namespace cv;

int main(int argc, char** argv )
{
    Mat InputImage;
    bool SHOW_IMAGE = false;
    bool PRINT_DATA = false;

    if ( argc < 2 )
    {
        printf("usage: ArUcoDetection.out <Image_Path> <Show_Image?> <Print_Data?>\n");
        return -1;
    }

    InputImage = imread( argv[1], 1 );
    if ( !InputImage.data ) {
        printf("No image data \n");
        return -1;
    }

    if (argc >= 3) {
        SHOW_IMAGE = (std::string(argv[2]) == "true");
    }
    if (argc >= 4) {
        PRINT_DATA = (std::string(argv[3]) == "true");
    }

    /**************************************************************************************/
    /************************************ArUco Detection***********************************/
    /**************************************************************************************/

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    // ArUco detection
    detector.detectMarkers(InputImage, markerCorners, markerIds, rejectedCandidates);

    // Print out detected marker ids sorted
    if (PRINT_DATA) {
        std::sort(markerIds.begin(), markerIds.end());
        std::cout << "Detected marker ids: ";
        for (auto i : markerIds)
            std::cout << i << " ";
        std::cout << std::endl;
        std::cout << "Number of detected markers: " << markerIds.size() << std::endl;
    }

    /**************************************************************************************/
    /************************************Pose Estimation***********************************/
    /**************************************************************************************/

    float markerLength = 26.41f; // Marker side length in mm
    Mat cameraMatrix = OnePlus11CameraMatrix;
    Mat distCoeffs = OnePlus11DistCoeffs;
    std::vector<Vec3d> rvecs(markerIds.size()); // Rotation vectors
    std::vector<Vec3d> tvecs(markerIds.size()); // Translation vectors

    // set coordinate system
    cv::Mat objPoints(4, 1, CV_32FC3);
    objPoints.ptr<Vec3f>(0)[0] = Vec3f(-markerLength/2.f, markerLength/2.f, 0);
    objPoints.ptr<Vec3f>(0)[1] = Vec3f(markerLength/2.f, markerLength/2.f, 0);
    objPoints.ptr<Vec3f>(0)[2] = Vec3f(markerLength/2.f, -markerLength/2.f, 0);
    objPoints.ptr<Vec3f>(0)[3] = Vec3f(-markerLength/2.f, -markerLength/2.f, 0);

    // Estimate pose for each detected marker
    for (size_t i = 0; i < markerIds.size(); i++) {
        cv::solvePnP(objPoints, markerCorners.at(i), cameraMatrix, distCoeffs, rvecs[i], tvecs[i]);
    }

    // Print out pose estimation results
    if (PRINT_DATA) {
        for (size_t i = 0; i < markerIds.size(); i++) {
            std::cout << "Rotation Vector: [" << rvecs[i][0] << ", " << rvecs[i][1] << ", " << rvecs[i][2] << "]" << std::endl;
            std::cout << "Translation Vector: [" << tvecs[i][0] << ", " << tvecs[i][1] << ", " << tvecs[i][2] << "]" << std::endl;
        }
    }

    // Display the image with detected markers and axes
    if (SHOW_IMAGE) {
        cv::aruco::drawDetectedMarkers(InputImage, markerCorners, markerIds);
        for (size_t i = 0; i < markerIds.size(); i++) {
            cv::drawFrameAxes(InputImage, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], markerLength * 1.5f, 2);
        }
        cv::namedWindow("Detected ArUco markers and pose", cv::WINDOW_NORMAL);
        cv::imshow("Detected ArUco markers and pose", InputImage);
        waitKey(0);
    }
    
    return 0;
}