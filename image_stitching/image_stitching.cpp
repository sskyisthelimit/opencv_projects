#include <opencv2/core/core.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"
 
#include <iostream>

#include <filesystem>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    string samplesFolder = "../samples/1";
    bool divide_images = false;
    Stitcher::Mode mode = Stitcher::PANORAMA;
    vector<Mat> imgs;
    string result_name = "../result.jpg";
 
    for (const auto& entry : fs::directory_iterator(samplesFolder)) {
        if (entry.path().extension() == ".jpg") {
            Mat img = imread(entry.path().string());
            if (!img.empty()) {
                imgs.push_back(img);
            } else {
                cerr << "Failed to load image: " << entry.path() << endl;
            }
        }
    }

    if (imgs.size() < 2) {
        cerr << "Need at least two images to stitch." << endl;
        return EXIT_FAILURE;
    }

    Mat pano;
    Ptr<Stitcher> stitcher = Stitcher::create(mode);
    Stitcher::Status status = stitcher->stitch(imgs, pano);
 
    if (status != Stitcher::OK)
    {
        cout << "Can't stitch images, error code = " << int(status) << endl;
        return EXIT_FAILURE;
    }
 
    imwrite(result_name, pano);
    cout << "stitching completed successfully\n" << result_name << " saved!";
    return EXIT_SUCCESS;
}
 