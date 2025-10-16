#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: ArUcoDetection.out <Image_Path>\n");
        return -1;
    }

    Mat InputImage;
    InputImage = imread( argv[1], 1 );

    if ( !InputImage.data )
    {
        printf("No image data \n");
        return -1;
    }

    // ArUco detection
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);
    detector.detectMarkers(InputImage, markerCorners, markerIds, rejectedCandidates);

    // Draw results
    if (markerIds.size() > 0)
        cv::aruco::drawDetectedMarkers(InputImage, markerCorners, markerIds);
    else
        std::cout << "No markers detected" << std::endl;

    // Show image
    cv::namedWindow("Detected markers", cv::WINDOW_NORMAL);
    imshow("Detected markers", InputImage);

    // Print out detected marker ids sorted
    std::sort(markerIds.begin(), markerIds.end());
    std::cout << "Detected marker ids: ";
    for (auto i : markerIds)
        std::cout << i << " ";
    std::cout << std::endl;
    std::cout << "Number of detected markers: " << markerIds.size() << std::endl;

    waitKey(0);

    return 0;
}