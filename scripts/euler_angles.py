# -*- coding: utf-8 -*-
import numpy as np
from scipy.spatial.transform import Rotation as R

def main():
    angles = [-42.06, -59.88, 3.27]
    r = R.from_euler('xyz', angles, degrees=True)
    p = np.array([-5.20307423, 0.86032313, 5.38248363])
    print(r.apply(p))
    
    rotmat = matrix_euler_angles(angles[0], angles[1], angles[2])
    print(rotmat.dot(p))

def Rx(theta):
    """
    @brief      Create a rotational matrix for a rotation around the x-axis
    """
    return np.array([[ 1, 0           , 0           ],
                     [ 0, np.cos(theta),-np.sin(theta)],
                     [ 0, np.sin(theta), np.cos(theta)]])

def Ry(theta):
    """
    @brief      Create a rotational matrix for a rotation around the x-axis
    """
    return np.array([[ np.cos(theta), 0, np.sin(theta)],
                     [ 0           , 1, 0           ],
                     [-np.sin(theta), 0, np.cos(theta)]])

def Rz(theta):
    """
    @brief      Create a rotational matrix for a rotation around the x-axis
    """
    return np.array([[ np.cos(theta), -np.sin(theta), 0 ],
                     [ np.sin(theta), np.cos(theta) , 0 ],
                     [ 0           , 0            , 1 ]])

def matrix_euler_angles(x,y,z):
    """
    Build rotational matrix from Euler angles

    Assumes that Euler angles are given in degrees
    """
    ax = np.radians(x)
    ay = np.radians(y)
    az = np.radians(z)
    return Rz(az).dot(Ry(ay).dot(Rx(ax)))

if __name__ == '__main__':
    main()