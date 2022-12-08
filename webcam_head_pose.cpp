#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include "httplib.h"

#include <string>
#include <sstream>
#include <thread>
#include <algorithm>

#define FACE_DOWNSAMPLE_RATIO 4
#define SKIP_FRAMES 2

#define FACE_RADIUS 270

#define OPENCV_PIXELS_MAP_TO_PAN 40
#define OPENCV_PIXELS_MAP_TO_TILT 30

#define START_PAN 90
#define START_TILT 25

#define MIN_ANGLE 0
#define MAX_ANGLE 180

int current_tilt = START_TILT;
int current_pan = START_PAN;

int recent_change = 0;

// Eventually remove this!
using namespace std;

enum FaceDirection { FORWARD, LEFT, RIGHT, UP, DOWN, NONE };
static const char *DirectionStrings[] = {"Forward", "Left", "Right", "Up", "Down", "None"};

enum ServoAngle { PAN, TILT };

const char *GetDirectionString(int val)
{
    return DirectionStrings[val];
}

std::vector<cv::Point3d> get_3d_model_points()
{
    std::vector<cv::Point3d> modelPoints;

    modelPoints.push_back(cv::Point3d(0.0f, 0.0f, 0.0f)); //The first must be (0,0,0) while using POSIT
    modelPoints.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));
    modelPoints.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));
    modelPoints.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));
    modelPoints.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));
    modelPoints.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));
    
    return modelPoints;
}

std::vector<cv::Point2d> get_2d_image_points(dlib::full_object_detection &d)
{
    std::vector<cv::Point2d> image_points;
    image_points.push_back( cv::Point2d( d.part(30).x(), d.part(30).y() ) );    // Nose tip
    image_points.push_back( cv::Point2d( d.part(8).x(), d.part(8).y() ) );      // Chin
    image_points.push_back( cv::Point2d( d.part(36).x(), d.part(36).y() ) );    // Left eye left corner
    image_points.push_back( cv::Point2d( d.part(45).x(), d.part(45).y() ) );    // Right eye right corner
    image_points.push_back( cv::Point2d( d.part(48).x(), d.part(48).y() ) );    // Left Mouth corner
    image_points.push_back( cv::Point2d( d.part(54).x(), d.part(54).y() ) );    // Right mouth corner
    return image_points;
}

cv::Mat get_camera_matrix(float focal_length, cv::Point2d center)
{
    cv::Mat camera_matrix = (cv::Mat_<double>(3,3) << focal_length, 0, center.x, 0 , focal_length, center.y, 0, 0, 1);
    return camera_matrix;
}

void DisplayVersion()
{
    std::cout << "OpenCV version: " 
                     << cv::getVersionMajor() << 
                 "." << cv::getVersionMinor() << 
                 "." << cv::getVersionRevision() 
                     << std::endl;
}

string ParseCLI(int argc, char** argv)
{
    bool useIP = false;

    std::stringstream ss;

    if (2 > argc)
    {
        std::cout << "No arguments, will default to camera!" << std::endl;
        useIP = false;
    }
    else if (3 == argc)
    {
        if (strncmp("-ip", argv[1], 3) == 0)
        {
            std::cout << "server input specified" << std::endl;
            useIP = true;
        }
        else if (strncmp("-c", argv[1], 2) == 0)
        {
            std::cout << "camera input specified" << std::endl;
            useIP = false;
        } 
    }
    else if (3 < argc)
    {
        std::cout << "Too many arguments, will default to camera!" << std::endl;
    }

    if (useIP)
    {
        ss << "http://" << argv[2] << "/";
    }
    else
    {
        ss << "nvarguscamerasrc !  video/x-raw(memory:NVMM), width=1280, height=720, format=NV12, framerate=21/1 ! nvvidconv flip-method=2 ! video/x-raw, width=48    0, height=680, format=BGRx ! videoconvert ! video/x-raw, format=BGR ! appsink";
    }

    std::cout << "Reading input from: " << (useIP ? "a server" : "the camera") << ". Settings: " << ss.str() << std::endl;

    return ss.str();
}

void SetAngleRotation(int distance, ServoAngle angle)
{
    float rotation = distance;

    if (ServoAngle::PAN == angle)
    {
        rotation /= OPENCV_PIXELS_MAP_TO_PAN;

        current_pan += rotation;
    }
    else if (ServoAngle::TILT == angle)
    {
        rotation /= OPENCV_PIXELS_MAP_TO_TILT;

        current_tilt -= rotation;
    }
    else
    {
        std::cout << "Unknown angle: " << angle << "!"<< std::endl;
    }
}

void do_http_get(std::string host, int port, int x, int y)
{
    SetAngleRotation(x, ServoAngle::PAN);
    SetAngleRotation(y, ServoAngle::TILT);

    std::clamp(current_pan, 0, 180);

    std::clamp(current_tilt, 0, 180);

    int pan = current_pan;
    int tilt = current_tilt;

    std::cout << "PAN: " << pan << " TILT: " << tilt << std::endl;
 
    httplib::Client cli(host, port);
 
    std::stringstream uri;
    uri << "/aim_camera?pan=" << pan << "&tilt=" << tilt;
 
    if (auto res = cli.Get(uri.str()))
    {
        if (res->status == 200)
        {
            std::cout << res->body << std::endl;
        }
    }
    else
    {
        auto err = res.error();

        std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
    }
}

int main(int argc, char** argv)
{
    DisplayVersion();

    try
    {
        cv::VideoCapture cap;

        cap.open(ParseCLI(argc, argv));

        if (!cap.isOpened())
        {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }

        double fps = 30.0; // Just a place holder. Actual value calculated after 100 frames.
        cv::Mat im;
        
        // Get first frame and allocate memory.
        cap >> im;
        cv::Mat im_small, im_display;
        cv::resize(im, im_small, cv::Size(), 1.0/FACE_DOWNSAMPLE_RATIO, 1.0/FACE_DOWNSAMPLE_RATIO);
        cv::resize(im, im_display, cv::Size(), 0.5, 0.5);
        
        cv::Size size = im.size(); 
        
        // Load face detection and pose estimation models.
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
        dlib::shape_predictor pose_model;
        dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model; //try 5 face landmarks aswell

        int count = 0;
        std::vector<dlib::rectangle> faces;
        // Grab and process frames until the main window is closed by the user.
        double t = (double)cv::getTickCount();
        while (1)
        {
            // std::cout << count << std::endl;

            if ( count == 0 )
            {
                t = cv::getTickCount();
            }

            cap >> im;
            
            // Resize image for face detection
            cv::resize(im, im_small, cv::Size(), 1.0/FACE_DOWNSAMPLE_RATIO, 1.0/FACE_DOWNSAMPLE_RATIO);
            
            // Change to dlib's image format. No memory is copied.
            dlib::cv_image<dlib::bgr_pixel> cimg_small(im_small);
            dlib::cv_image<dlib::bgr_pixel> cimg(im);
            
            // Detect faces 
            if ( count % SKIP_FRAMES == 0 )
            {
                faces = detector(cimg_small);
                // cout << "faces " << faces.size() << endl;
            }
            
            // Pose estimation
            std::vector<cv::Point3d> model_points = get_3d_model_points();
            
            // Find the pose of each face.
            std::vector<dlib::full_object_detection> shapes;
            for (unsigned long i = 0; i < faces.size(); ++i)
            {
                dlib::rectangle r(
                            (long)(faces[i].left() * FACE_DOWNSAMPLE_RATIO),
                            (long)(faces[i].top() * FACE_DOWNSAMPLE_RATIO),
                            (long)(faces[i].right() * FACE_DOWNSAMPLE_RATIO),
                            (long)(faces[i].bottom() * FACE_DOWNSAMPLE_RATIO)
                            );

                dlib::full_object_detection shape = pose_model(cimg, r);
                shapes.push_back(shape);
                std::vector<cv::Point2d> image_points = get_2d_image_points(shape);

                double focal_length = im.cols;

                cv::Mat camera_matrix = get_camera_matrix(focal_length, cv::Point2d(im.cols/2,im.rows/2));
                cv::Mat rotation_vector;
                cv::Mat rotation_matrix;
                cv::Mat translation_vector;
                
                cv::Mat dist_coeffs = cv::Mat::zeros(4,1,cv::DataType<double>::type);
                
                cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

                std::vector<cv::Point3d> nose_end_point3D;
                std::vector<cv::Point2d> nose_end_point2D;
                nose_end_point3D.push_back(cv::Point3d(0, 0, 1000.0));

                cv::projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);     
                cv::line(im, image_points[0], nose_end_point2D[0], cv::Scalar(255, 0, 255), 10);

                double dist = cv::norm(image_points[0] - nose_end_point2D[0]);

                CvPoint middle;

                cv::Size sz = im.size();

                middle.x = sz.width / 2;
                middle.y = sz.height / 2;

                int dist_from_middle = image_points[0].x - middle.x;

                if (0 == (count % 4))
                {
                    std::thread http_thread(do_http_get, "localhost", 5000, image_points[0].x - middle.x, image_points[0].y - middle.y);

                    http_thread.detach();
                }

                //std::cout << "distance from the center: " << dist << std::endl;

                //std::cout << "Nose X: " << nose_end_point2D[0].x << " Nose Y: " << nose_end_point2D[0].y << std::endl;
 
                bool isFacingCamera = (dist < FACE_RADIUS);

                FaceDirection direction = NONE;

                if (!isFacingCamera)
                {
                    if (image_points[0].x > nose_end_point2D[0].x)
                    {
                        direction = LEFT;
                    }
                    else
                    {
                        direction = RIGHT;
                    }
                }
                else
                {
                    direction = FORWARD;
                }
            

                cv::Scalar radiusColor = (isFacingCamera) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 250);

                cv::putText(im, cv::format("Facing %s", GetDirectionString(direction)), cv::Point(50, size.height - 50), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 255), 5);

                cv::circle(im, image_points[0], FACE_RADIUS, radiusColor, 1);
            }

            //cv::putText(im, cv::format("fps %.2f",fps), cv::Point(50, size.height - 50), cv::FONT_HERSHEY_COMPLEX, 1.5, cv::Scalar(0, 0, 255), 3);
            
            // Resize image for display
            im_display = im;
            cv::resize(im, im_display, cv::Size(), 0.5, 0.5);
            cv::imshow("Fast Facial Landmark Detector", im_display);

            count++;

            cv::pollKey();

            if (count == 100)
            {
                t = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
                fps = 100.0 / t;
                count = 0;
            }
        }
    }
    catch(dlib::serialization_error& e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}

