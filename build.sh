# clang++ main.cpp -I/usr/local/include/opencv4 -L/usr/local/lib/ -lopencv_video -lopencv_core -lopencv_videoio -lopencv_highgui
clang++ webcam_head_pose.cpp -I/usr/local/include/opencv4/ -I/usr/local/include/ -L/usr/local/lib/ -lpthread -lopencv_video -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_calib3d -lopencv_imgcodecs -ldlib -llapack -lblas -lgif -lX11
