#!/usr/bin/python3
#/usr/bin/python3.11
from flask import Flask, json, request
from adafruit_servokit import ServoKit

kit = ServoKit(channels=16)

current_pan = 90
current_tilt = 25

kit.servo[0].angle = current_pan
kit.servo[1].angle = current_tilt

MAX_PAN = 180
MAX_TILT = 180

MIN_PAN = 0
MIN_TILT = 0

companies = [{"id": 1, "name": "Company One"}, {"id": 2, "name": "Company Two"}]

api = Flask(__name__)

@api.route('/aim_camera', methods=['GET'])
def get_companies():

    # We don't check if x and y are garbage!
    pan = int(request.args.get('pan'))
    tilt = int(request.args.get('tilt'))

    # print(type(x))
    # print(type(y))

    if pan > MAX_PAN:
        pan = MAX_PAN
    elif pan < MIN_PAN:
        pan = MIN_PAN

    if tilt > MAX_PAN:
        tilt = MAX_PAN
    elif tilt < MIN_PAN:
        tilt = MIN_PAN

    print(f'pan={pan} tilt={tilt}')

    kit.servo[0].angle = pan
    kit.servo[1].angle = tilt

    current_pan = pan

    current_tilt = tilt

    return json.dumps(companies)

@api.route('/clench', methods=['GET'])
def get_clench():

    return json.dumps(companies)

if __name__ == '__main__':
    api.run()

