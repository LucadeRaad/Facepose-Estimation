# Facepose-Estimation
There are two components:
ServoServer.py: web server that talks to the servos aiming the camera
build/Pinocchio: head pose estimation which can display output to a debug video, can take video input from an mjpeg server or the camera attached to the gizmo
##How to build build/Pinocchio:
mkdir build; cd build; cmake ..; cmake --build .

The 'build' directory will have the following programs:
Pinocchio: head pose estimation
Maia: a test program

