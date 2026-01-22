#!/usr/bin/env python3

#
# Copyright (C) Inorganic Materials & Catalysis - All Rights Reserved
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

import bpy

def main():
    cuda_devices, opencl_devices = bpy.context.preferences.addons['cycles'].preferences.get_devices()
    for device in cuda_devices:
        print(device)

if __name__ == '__main__':
    main()
