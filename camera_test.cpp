#include <iostream>
#include <sstream>
#include <new>
#include <string>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <JetsonGPIO.h>

#define INPUT_WIDTH 3264
#define INPUT_HEIGHT 2464

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 640

#define CAMERA_FRAMERATE 21/1
#define FLIP 2

void DisplayVersion()
{
    std::cout << "OpenCV version: " << cv::getVersionMajor() << "." << cv::getVersionMinor() << "." << cv::getVersionRevision() << std::endl;
}

int main(int argc, const char** argv)
{
    DisplayVersion();

    std::stringstream ss;

    std::cout << GPIO::JETSON_INFO() << std::endl;

    return 0;

    ss << "nvarguscamerasrc !  video/x-raw(memory:NVMM), width=3264, height=2464, format=NV12, framerate=21/1 ! nvvidconv flip-method=2 ! video/x-raw, width=480, height=680, format=BGRx ! videoconvert ! video/x-raw, format=BGR ! appsink";

    //ss << "nvarguscamerasrc !  video/x-raw(memory:NVMM), width=" << INPUT_WIDTH <<
    //", height=" << INPUT_HEIGHT <<
    //", format=NV12, framerate=" << CAMERA_FRAMERATE <<
    //" ! nvvidconv flip-method=" << FLIP <<
    //" ! video/x-raw, width=" << DISPLAY_WIDTH <<
    //", height=" << DISPLAY_HEIGHT <<
    //", format=BGRx ! videoconvert ! video/x-raw, format=BGR ! appsink";

    cv::VideoCapture video;

    video.open(ss.str());

    if (!video.isOpened())
    {
        std::cout << "Unable to get video from the camera!" << std::endl;

        return -1;
    }

    std::cout << "Got here!" << std::endl;

    cv::Mat frame;

    while (video.read(frame))
    {
        cv::imshow("Video feed", frame);

        if (cv::waitKey(25) >= 0)
        {
            break;
        }
   }

    std::cout << "Finished!" << std::endl;

    return 0;
}

