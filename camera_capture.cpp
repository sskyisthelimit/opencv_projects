#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <vector>
#include <filesystem> 

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

const float calibrationSquareLen = 0.015f;
const Size chessboardDimensions = Size(7, 7);

void processChessboardImages(const string& samplesDir, vector<vector<Point2f>>& foundCorners, const string& outputFolder) {
    fs::create_directories(outputFolder);
    int imgCounter = 0;

    for (const auto& entry : fs::directory_iterator(samplesDir)) {
        string filepath = entry.path().string();
        Mat image = imread(filepath);
        if (image.empty()) {
            cerr << "Could not load image: " << filepath << endl;
            continue;
        }
        vector<Point2f> pointBuffer;
        bool found = findChessboardCorners(image, chessboardDimensions, pointBuffer, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
        if (found) {
            foundCorners.push_back(pointBuffer);
            drawChessboardCorners(image, chessboardDimensions, pointBuffer, found);
        } else {
            cerr << "Chessboard not found in: " << filepath << endl;
        }

        string outputFilename = outputFolder + "image_" + to_string(imgCounter) + ".jpg";
        imwrite(outputFilename, image);
        cout << "Processed and saved: " << outputFilename << endl;

        imgCounter++;
    }
}


int main(){
    string samplesFolder = "../chessboard_samples";
    string outputFolder = "../processed_samples/";

    vector<vector<Point2f>> imagePoints;

    processChessboardImages(samplesFolder, imagePoints, outputFolder);
    
    cout << "Finished processing images. Found corners in " << imagePoints.size() << " images." << endl;

    return 0;


}