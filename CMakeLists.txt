cmake_minimum_required(VERSION 2.8)
project( ArUcoPoseEstimation )
find_package( OpenCV REQUIRED )
add_executable( output/ArUcoPoseEstimation main.cpp )
target_link_libraries( output/ArUcoPoseEstimation ${OpenCV_LIBS} )

# Add executable for the calibration utility
add_executable( output/CalibrateCamera calibrate_camera.cpp )
target_link_libraries( output/CalibrateCamera ${OpenCV_LIBS} )