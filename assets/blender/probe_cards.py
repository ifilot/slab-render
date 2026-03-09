#!/usr/bin/env python3

import bpy

def main():
    # Get Cycles addon preferences
    cycles_prefs = bpy.context.preferences.addons['cycles'].preferences

    # Force device discovery (important!)
    cycles_prefs.get_devices()

    print("Compute device type:", cycles_prefs.compute_device_type)

    for device in cycles_prefs.devices:
        print(
            f"Name: {device.name}, "
            f"Type: {device.type}, "
            f"Use: {device.use}"
        )

if __name__ == "__main__":
    main()
