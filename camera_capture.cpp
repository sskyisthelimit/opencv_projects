#include <opencv2/opencv.hpp>
#include <iostream>

int main(){
    cv::VideoCapture cap(2);
    if (!cap.isOpened()) {
        std::cerr << "Error: couldn't open cam" << std::endl;
        return -1;
    }

    cv::namedWindow("Camera Prewiew", cv::WINDOW_AUTOSIZE);

    int imgCounter = 0;
    std::string outputFolder = "./";

    while(true){
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Could not read frame from camera." << std::endl;
            break;
        }

        cv::imshow("Camera Preview", frame);
        char key = (char)cv::waitKey(1);

        if (key == 'q') {
            break;
        }
        else if (key == 's') {
            std::string filename = outputFolder + "image_" + std::to_string(imgCounter) + ".jpg";
            cv::imwrite(filename, frame);
            std::cout << "Saved: " << filename << std::endl;
            imgCounter++;
        }
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}