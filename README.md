# Facepose-Estimation
There are two components:

ServoServer.py: web server that talks to the servos aiming the camera

build/Pinocchio: head pose estimation which can display output to a debug video, can take video input from an mjpeg server or the camera attached to the gizmo

## How to setup:
Run the build scripts in the installer. The bashrc in the original jetson nano was also modified so you just need to copy my modifications there.

## How to build:

`./build.sh`

Now `FaceposeEstimation.exe` has compiled

## How to launch:

`./launcher.sh`

Something to note:
Bad idea to run the programs independently. Both `FaceposeEstimation.exe` and `ServoServer.py` must be in sync and know the current pan and tilt of the camera. 
This means that since `FaceposeEstimation.exe` launches first when launching `launcher.sh` it will calculate and update the pan and tilt values on its end and send the information to `ServoServer.py`.
If `ServoServer.py` has not launched yet it will not recieve these updates and therefore have the default pan and tilt values, be out of sync and the only way to fix this is to either restart or move the servos all the way into the edges to reset the values.

## How it works:

`FaceposeEstimation.exe`:
- Gets images from either an ffmpeg server or from the camera based on command line arguments.
- Solves for a face using [solvepnp](https://docs.opencv.org/4.x/d5/d1f/calib3d_solvePnP.html) based on a dataset of 68 landmarks on someone's face.
  - The landmarks can include a couple of points for each eye, a point for the nose, and points for the face, jawline, and ears.
  - When a face is found a message is sent to `ServoServer.py` in a thread.
- You can get a point that shows in 3d space what direction a person is facing.
  - Akin to if someone has a pinocchio nose.
- Calculate the distance of that point to the person's actual nose in 2d space.
  - A heuristic is used to determine if they are looking at or away from the camera.
- If the person is looking away you can calculate if they are looking left or right.
  - Too much of the landmarks used to determine the person's face is obscured to figure out if they are looking up down. If that person is looking up or down.
  
`ServoServer.py`:
- When `FaceposeEstimation.exe` finds an image with a face, it sends the new pan and tilt to `ServoServer.py`.
- A thread is spawned and the server attempts to move the servos so that the camera can pan and tilt towards the new pan and tilt values it has recieved.
  - The movement of the servos is done via exponential interpolation, the distance from the current pan and tilt and the new pan and tilt is halved until the servos "snap" to the correct angle.

