#!/usr/bin/python3
from flask import Flask, json, request, Response
from adafruit_servokit import ServoKit
import threading
import math

class ServoHandler(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.event = threading.Event()
        
        self.goal_pan = 90
        self.goal_tilt = 25

        self.MAX_PAN = 180
        self.MAX_TILT = 180
 
        self.MIN_PAN = 0
        self.MIN_TILT = 0

        self.SNAP_ANGLE = 2

        self.kit = ServoKit(channels=16)

    def set_goals(self, pan, tilt):
        # Make sure have reasonable values
        self.goal_pan = min(self.MAX_PAN, max(self.MIN_PAN, pan))
        self.goal_tilt = min(self.MAX_TILT, max(self.MIN_TILT, tilt))

        # Wake up our thread
        self.event.set()

    def run(self):
        self.kit.servo[0].angle = self.goal_pan
        self.kit.servo[1].angle = self.goal_tilt

        while True:
            current_pan = math.ceil(self.kit.servo[0].angle)
            current_tilt = math.ceil(self.kit.servo[1].angle)

            print(f'current_pan={current_pan} current_tilt={current_tilt}')

            if current_pan == self.goal_pan and current_tilt == self.goal_tilt:
                self.event.wait(1)
                self.event.clear()
            else:
                print("moving")

                # If exponential is required: take dist from current to goal divide it by 2 and then add it to current pan/tilt:q
                # self.kit.servo[0].angle = self.goal_pan
                # self.kit.servo[1].angle = self.goal_tilt
                distance_pan = abs(self.goal_pan - current_pan) 

                if distance_pan < self.SNAP_ANGLE:
                    self.kit.servo[0].angle = self.goal_pan 
                else:
                    self.kit.servo[0].angle = ((self.goal_pan - current_pan) / 2) + current_pan 

                distance_tilt = abs(self.goal_tilt - current_tilt) 

                if distance_tilt < self.SNAP_ANGLE:
                    self.kit.servo[1].angle = self.goal_tilt 
                else:
                    self.kit.servo[1].angle = ((self.goal_tilt - current_tilt) / 2) + current_tilt 

api = Flask(__name__)

@api.route('/aim_camera', methods=['GET'])
def get_aim_camera():

    # We don't check if x and y are garbage!
    pan = int(request.args.get('pan'))
    tilt = int(request.args.get('tilt'))

    # print(type(x))
    # print(type(y))

    print(f'pan={pan} tilt={tilt}')
    servo_handler.set_goals(pan, tilt)

    return Response(response="I GOT THIS", status=200)

@api.route('/clench', methods=['GET'])
def get_clench():

    return Response(response="CLENCH!", status=200)

if __name__ == '__main__':
    servo_handler = ServoHandler()
    servo_handler.start()

    api.run()

