#include <iostream>
#include <sstream>
#include <new>
#include <string>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#define DESKTOP_WIDTH 480
#define DESKTOP_HEIGHT 480

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 480

#define CAMERA_FRAMERATE 21/1
#define FLIP 2

void DisplayVersion()
{
    std::cout << "OpenCV version: " << cv::getVersionMajor() << "." << cv::getVersionMinor() << "." << cv::getVersionRevision() << std::endl;
}

int main(int argc, const char** argv)
{
    DisplayVersion();

    //return 0;

    std::stringstream ss;

    ss << "nvarguscamerasrc !  video/x-raw(memory:NVMM), width=" << DESKTOP_WIDTH <<
    ", height=" << DESKTOP_HEIGHT <<
    ", format=NV12, framerate=" << CAMERA_FRAMERATE <<
    " ! nvvidconv flip-method=" << FLIP <<
    " ! video/x-raw, width=" << DISPLAY_WIDTH <<
    ", height=" << DISPLAY_HEIGHT <<
    ", format=BGRx ! videoconvert ! video/x-raw, format=BGR ! appsink";

    //cv::VideoCapture video(ss.str());
    
    cv::VideoCapture video;

    video.open("http://10.102.66.208:8000/");


    if (!video.isOpened())
    {
        std::cout << "Unable to get video from the camera!" << std::endl;

        return -1;
    }

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

