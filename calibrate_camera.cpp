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

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char** argv ) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <folder_with_chessboard_images>" << endl;
        return -1;
    }

    // Collect files named chess01.jpg, chess02.jpg, ...
    string folder = argv[1];
    vector<string> imageFiles;

    for (int i = 1; ; ++i) {
        char filename[256];
        snprintf(filename, sizeof(filename), "chess%02d.jpg", i);
        fs::path imagePath = fs::path(folder) / filename;
        if (!fs::exists(imagePath))
            break;
        imageFiles.push_back(imagePath.string());
    }

    if (imageFiles.empty()) {
        cout << "No chessboard images found in folder: " << folder << endl;
        cout << "Make sure images are named chess01.jpg, chess02.jpg, etc." << endl;
        return -1;
    }

    // Chessboard settings
    Size boardSize(7, 9); // Number of inner corners per chessboard row and column
    float squareSize = 20.0f; // Square size in mm

    // Prepare object points and image points
    vector<vector<Point3f>> objectPoints;
    vector<vector<Point2f>> imagePoints;
    vector<Point3f> obj;
    for (int i = 0; i < boardSize.height; ++i) {
        for (int j = 0; j < boardSize.width; ++j) {
            obj.push_back(Point3f(j * squareSize, i * squareSize, 0));
        }
    }

    for (const auto& file : imageFiles) {
        Mat img = imread(file, IMREAD_GRAYSCALE);

        if (img.empty()) {
            cout << "Could not open or find the image: " << file << endl;
            continue;
        }

        vector<Point2f> corners;
        bool found = findChessboardCorners(img, boardSize, corners);

        if (found) {
            cornerSubPix(img, corners, Size(11, 11), Size(-1, -1),
                         TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
        } else {
            cout << "Chessboard corners not found in image: " << file << endl;
        }
    }

    if (imagePoints.size() < 1) {
        cout << "Not enough valid images for calibration." << endl;
        return -1;
    }

    // Calculate camera matrix and distortion coefficients
    Mat cameraMatrix, distCoeffs;
    vector<Mat> rvecs, tvecs;
    Size imageSize = imread(imageFiles[0], IMREAD_GRAYSCALE).size();

    double rms = calibrateCamera(objectPoints, imagePoints, imageSize,
                                cameraMatrix, distCoeffs, rvecs, tvecs);

    cout << "RMS error reported by calibrateCamera: " << rms << endl;
    cout << "Camera matrix:" << endl << cameraMatrix << endl;
    cout << "Distortion coefficients:" << endl << distCoeffs << endl;

    return 0;
}