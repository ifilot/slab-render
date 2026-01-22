#!/usr/bin/env python3

#
# Copyright (C) Inorganic Materials & Catalysis - All Rights Reserved
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

import bpy
from math import pi
import sys
import os
import numpy as np
from os.path import dirname
import json
import struct
import bmesh

# enable GPUs
known_devices = [
    r'GeForce RTX 2070 with Max-Q Design',
    r'GeForce GTX 1080 Ti',
    r'GeForce RTX 2070',
    r'NVIDIA GeForce RTX 2070',
    r'GeForce GTX 970',
    r'GeForce RTX 3090',
]
cuda_devices, opencl_devices = bpy.context.preferences.addons['cycles'].preferences.get_devices()
print(cuda_devices)
bpy.context.preferences.addons['cycles'].preferences.compute_device_type = 'CUDA'
for device in bpy.context.preferences.addons['cycles'].preferences.devices:
    if device.name in known_devices:
        device.use = True
        print('Enabling: %s' % device.name)
    else:
        device.use = False

def main():
    # read input and output file
    argv = sys.argv
    argv = argv[argv.index("--") + 1:]
    outfile = argv[0]

    # set render parameters
    data = {}
    data['ortho_scale'] = 'auto'
    data['resolution_x'] = 512
    data['resolution_y'] = 512
    data['samples'] = 128
    data['tile_x'] = 128
    data['tile_y'] = 128

    # build camera
    build_camera(data, 10.0)

    # run single image with just the geometry
    run_render(outfile, data)

def run_render(filename, data):
    # start render
    for scene in bpy.data.scenes:
        scene.cycles.device = 'GPU'
    bpy.context.scene.render.resolution_x = data['resolution_x']
    bpy.context.scene.render.resolution_y = data['resolution_y']
    bpy.context.scene.render.resolution_percentage = 100
    bpy.context.scene.cycles.samples = data['samples']
    bpy.context.scene.render.tile_x = data['tile_x']
    bpy.context.scene.render.tile_y = data['tile_y']

    bpy.data.scenes['Scene'].render.filepath = filename
    bpy.ops.render.render(write_still=True)

def build_camera(data, autoscale):
    """
    Build an orthogonal camera object
    """

    # set the position of the camera
    # and the rotation to align it towards
    # the center of the unit cell
    if 'camera_direction' not in data.keys():
        location = [0.0, 0.0, 100.0]
        rotation = [0.0, 0.0, 0.0]
    elif data['camera_direction'] == 'Z+':
        location = [0.0, 0.0, 100.0]
        rotation = [0.0, 0.0, 0.0]
    elif data['camera_direction'] == 'Z-':
        location = [0.0, 0.0, -100.0]
        rotation = [np.pi, 0.0, 0.0]
    elif data['camera_direction'] == 'X+':
        location = [100.0, 0.0, 0.0]
        rotation = [np.pi/2.0, 0.0, np.pi/2.0]
    elif data['camera_direction'] == 'X-':
        location = [-100.0, 0.0, 0.0]
        rotation = [np.pi/2.0, 0.0, -np.pi/2.0]
    elif data['camera_direction'] == 'Y+':
        location = [0.0, 100.0, 0.0]
        rotation = [np.pi/2.0, 0.0, np.pi]
    elif data['camera_direction'] == 'Y-':
        location = [0.0, -100.0, 0.0]
        rotation = [-np.pi/2.0, 0.0, 0.0]

    # if a camera position is specified, overwrite the
    # default position from the direction
    if "camera_position" in data.keys():
        location = [float(v) for v in data['camera_position'].split(",")]

    camera_data = bpy.data.cameras.new(name='Camera')
    camera_object = bpy.data.objects.new('Camera', camera_data)
    camera_object.location = location
    print("Setting camera to location: %s" % location)
    camera_object.rotation_euler = rotation
    camera_object.data.type = 'ORTHO'
    bpy.context.scene.collection.objects.link(camera_object)
    bpy.context.scene.camera = camera_object
    if data['ortho_scale'] == 'auto':
        camera_object.data.ortho_scale = autoscale + 5.0
    else:
        camera_object.data.ortho_scale = float(data['ortho_scale'])
    print("Setting camera ortho scale to %f" % camera_object.data.ortho_scale)
    camera_object.data.clip_end = 1000

if __name__ == '__main__':
    main()
