FILE="webcam_head_pose.cpp"

FILE="main.cpp"

# clang++ main.cpp -I/usr/local/include/opencv4 -L/usr/local/lib/ -lopencv_video -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc
clang++ $FILE -O3 -I/usr/local/include/opencv4/ -I/usr/local/include/ -I/usr/local/lib/JetsonGPIO/lib -L/usr/local/lib/ -lpthread -lopencv_video -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_calib3d -lJetsonGPIO -ldlib -llapack -lblas -lgif -o FaceposeEstimation.exe

echo $FILE
