#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <sstream>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <filesystem> 

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

const float calibrationSquareLen = 0.015f;
const Size chessboardDimensions = Size(7, 7);

bool saveCameraSettings(const string& filename, Mat& distCoeffs, Mat& cameraMatrix) {
    FileStorage fs(filename, FileStorage::WRITE);
    if (!fs.isOpened()){
        cout << "couldn't open file" << endl;
        return false;
    }

    fs << "cameraMatrix" << cameraMatrix;
    fs << "distCoeffs" << distCoeffs;
    cout << "Camera settings saved to " << filename << endl;
    return true;
}

bool loadCameraSettings(const string& filename, Mat& distCoeffs, Mat& cameraMatrix) {
    FileStorage fs(filename, FileStorage::READ);
    if (!fs.isOpened()){
        cout << "couldn't open file" << endl;
        return false;
    }
    fs["cameraMatrix"] >> cameraMatrix;
    fs["distCoeffs"] >> distCoeffs;
    cout << "Camera settings loaded from " << filename << endl;
    return true;


void undistortAndSave(const vector<Mat>& images, Mat& distCoeffs,
    Mat& cameraMatrix, const string& outputFolder) {
    int count = 0;
    for (const auto& image : images) {
        Mat undistorted;

        undistort(image, undistorted, cameraMatrix, distCoeffs);

        string outputFilename = outputFolder + "/undistorted_" + to_string(count++) + ".jpg";
        if (imwrite(outputFilename, undistorted)) {
            cout << "Saved undistorted image: " << outputFilename << endl;
        } else {
            cerr << "Failed to save undistorted image: " << outputFilename << endl;
        }

        if (count >= 5) break;
    }

}


void createKnownBoardPositions(Size boardSize, float squareLen, vector<Point3f>& corners) {
    corners.clear();
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            corners.push_back(Point3f(j * squareLen, i * squareLen, 0.0f));
        }
    }
}

bool getChessboardCorners(const vector<Mat>& images, vector<vector<Point2f>>& allFoundCorners, Size boardSize) {
    for (const auto& image : images) {
        vector<Point2f> pointBuf;
        bool found = findChessboardCorners(image, boardSize, pointBuf,
            CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
        if (found) {
            Mat gray;
            cvtColor(image, gray, COLOR_BGR2GRAY);
            cornerSubPix(gray, pointBuf, Size(11, 11), Size(-1, -1),
            TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001 ));
            allFoundCorners.push_back(pointBuf);
        } else {
            cout << "Chessboard not found in an image." << endl;
        }
    }
    return !allFoundCorners.empty();
}

void calibrateCameraByBoard(const vector<Mat>& images, Size boardSize, float squareLen,
    Mat& cameraMatrix, Mat& distCoeffs) {

    vector<vector<Point2f>> allFoundCorners;
    if (!getChessboardCorners(images, allFoundCorners, boardSize)) {
        cout << "no corners found in all images." << endl;
        return;
    }
    vector<vector<Point3f>> knownCornersPositions(allFoundCorners.size());
    for (size_t i = 0; i < allFoundCorners.size(); i++) {
        createKnownBoardPositions(boardSize, squareLen, knownCornersPositions[i]);
    }

    vector<Mat> rvecs, tvecs;
    double reprojectionError = calibrateCamera(knownCornersPositions, allFoundCorners,
        images[0].size(), cameraMatrix, distCoeffs, rvecs, tvecs);
    
    cout << "Calibration successful. Reprojection error: " << reprojectionError << endl;
    cout << "Camera matrix:\n" << cameraMatrix << endl;
    cout << "Distortion coefficients:\n" << distCoeffs << endl;
}

int main() {
    string samplesFolder = "../chessboard_samples/";
    string outputFolder = "../undistorted_samples/";
    
    fs::create_directories(outputFolder);

    vector<Mat> calibrationImages;
    for (const auto& entry : fs::directory_iterator(samplesFolder)) {
        if (entry.path().extension() == ".jpg") {
            Mat img = imread(entry.path().string());
            if (!img.empty()) {
                calibrationImages.push_back(img);
            } else {
                cerr << "Failed to load image: " << entry.path() << endl;
            }
        }
    }

    if (calibrationImages.empty()) {
        cerr << "No valid images found in the samples folder." << endl;
        return -1;
    }

    Mat cameraMatrix, distCoeffs;
    calibrateCameraByBoard(calibrationImages, chessboardDimensions, calibrationSquareLen, cameraMatrix, distCoeffs);
    undistortAndSave(calibrationImages, distCoeffs, cameraMatrix, outputFolder);

    return 0;
}
