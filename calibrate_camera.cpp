/* This is a utility program that calculates the camera matrix and distortion
 * coefficients given a set of chessboard images. This is useful for
 * undistorting images and for more accurate pose estimation.
 * 
 * Arguments:
 *   <folder_with_chessboard_images> named chess01.jpg, chess02.jpg, ...  
 */
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>

int main(int argc, char** argv ) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <folder_with_chessboard_images>" << std::endl;
        return -1;
    }

    // Collect files named chess01.jpg, chess02.jpg, ...
    std::string folder = argv[1];
    std::vector<std::string> imageFiles;

    for (int i = 1; ; ++i) {
        char filename[256];
        snprintf(filename, sizeof(filename), "chess%02d.jpg", i);
        std::filesystem::path imagePath = std::filesystem::path(folder) / filename;
        if (!std::filesystem::exists(imagePath))
            break;
        imageFiles.push_back(imagePath.string());
    }

    if (imageFiles.empty()) {
        std::cout << "No chessboard images found in folder: " << folder << std::endl;
        std::cout << "Make sure images are named chess01.jpg, chess02.jpg, etc." << std::endl;
        return -1;
    }

    // Chessboard settings
    cv::Size boardSize(7, 9); // Number of inner corners per chessboard row and column
    float squareSize = 20.0f; // Square size in mm

    // Prepare object points and image points
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;
    std::vector<cv::Point3f> obj;
    for (int i = 0; i < boardSize.height; ++i) {
        for (int j = 0; j < boardSize.width; ++j) {
            obj.push_back(cv::Point3f(j * squareSize, i * squareSize, 0));
        }
    }

    for (const auto& file : imageFiles) {
        cv::Mat img = cv::imread(file, cv::IMREAD_GRAYSCALE);

        if (img.empty()) {
            std::cout << "Could not open or find the image: " << file << std::endl;
            continue;
        }

        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(img, boardSize, corners);

        if (found) {
            cv::cornerSubPix(img, corners, cv::Size(11, 11), cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
        } else {
            std::cout << "Chessboard corners not found in image: " << file << std::endl;
        }
    }

    if (imagePoints.size() < 1) {
        std::cout << "Not enough valid images for calibration." << std::endl;
        return -1;
    }

    // Calculate camera matrix and distortion coefficients
    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    cv::Size imageSize = cv::imread(imageFiles[0], cv::IMREAD_GRAYSCALE).size();

    double rms = cv::calibrateCamera(objectPoints, imagePoints, imageSize,
                                cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "RMS error reported by calibrateCamera: " << rms << std::endl;
    std::cout << "Camera matrix:" << std::endl << cameraMatrix << std::endl;
    std::cout << "Distortion coefficients:" << std::endl << distCoeffs << std::endl;

    return 0;
}