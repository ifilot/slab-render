#!/usr/bin/env python3

#
# Copyright (C) Inorganic Materials & Catalysis - All Rights Reserved
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

import bpy
import sys
import os
import numpy as np
from os.path import dirname
import json
import struct
import bmesh
import json

def main():
    # read input and output file
    
    argv = sys.argv
    argv = argv[argv.index("--") + 1:]
    inputfile = argv[0]
    binfile = argv[1]
    outfile = argv[2]
    print("Reading: %s" % inputfile)

    print("Searching for GPUs")
    enable_cycles_gpu()

    with open(inputfile) as f:
        data = json.load(f)
    print('Render settings:')
    print(data)

    if 'hide_axes' in data.keys():
        if data['hide_axes'] == True:
            bpy.data.collections['Coordinate axes'].hide_render = True
            print('Disable rendering of coordinate axes')
        else:
            print('Enable rendering of coordinate axes')

    # build molecule
    matrix = build_molecule(binfile, data)
    autoscale = max(np.linalg.norm(matrix[:,0]), np.linalg.norm(matrix[:,1]))

    # show the unitcell dimensions using dashed lines
    if data['show_unitcell'] == True:
        show_unitcell(matrix, data)

    # add a camera
    build_camera(data, autoscale)

    # run single image with just the geometry
    run_render(outfile, data)

def run_render(filename, data):
    scene = bpy.context.scene
    scene.render.engine = 'CYCLES'
    scene.cycles.device = 'GPU'

    bpy.context.scene.render.resolution_x = data['resolution_x']
    bpy.context.scene.render.resolution_y = data['resolution_y']
    bpy.context.scene.render.resolution_percentage = 100
    bpy.context.scene.cycles.samples = data['samples']

    bpy.data.scenes['Scene'].render.filepath = filename
    bpy.ops.render.render(write_still=True)

def read_binfile(binfile, lib, data):
    """
    Read atom coordinates from .bin file
    """

    if "object_euler" in data.keys():
        rotate_structure = True
        angles = [float(angle) for angle in data["object_euler"].split("/")]
        print("Building rotation matrix from Euler angles: %s" % angles)
        rotation_matrix = matrix_euler_angles(angles[0], angles[1], angles[2])

    # read binfile
    f = open(binfile, 'rb')

    # unit cell
    matrix = np.zeros((3,3))
    for i in range(0,3):
        for j in range(0,3):
            matrix[i,j] = struct.unpack('d', f.read(8))[0]

    # atoms
    nr_atoms = struct.unpack('I', f.read(4))[0]
    atoms = []
    for i in range(0, nr_atoms):
        el = lib.get_element(struct.unpack('B', f.read(1))[0])
        x = struct.unpack('d', f.read(8))[0]
        y = struct.unpack('d', f.read(8))[0]
        z = struct.unpack('d', f.read(8))[0]
        if "object_euler" in data.keys():
            p = np.array([x,y,z])
            pr = rotation_matrix.dot(p)
            atoms.append([el, pr[0], pr[1], pr[2]])
        else:
            atoms.append([el, x, y, z])

    # once the atoms are known, a billboarding alignment matrix
    # can be generated, but we still need to retroactively apply
    # this rotation matrix to the atoms
    if "atom_align" in data.keys() and not "object_euler" in data.keys():
        rotate_structure = True
        atomlist = [int(idx)-1 for idx in data["atom_align"].split(",")]
        rotation_matrix = matrix_billboard_atoms(atomlist, atoms)

        for i in range(0, len(atoms)):
            p = np.array([atoms[i][1],atoms[i][2],atoms[i][3]])
            pr = rotation_matrix.dot(p)
            for j in range(0,3):
                atoms[i][j+1] = pr[j]
    elif "object_euler" in data.keys():
        rotate_structure = True
    else:
        rotate_structure = False

    # bonds
    nr_bonds = struct.unpack('I', f.read(4))[0]
    bonds = []
    for i in range(0, nr_bonds):
        el1 = lib.get_element(struct.unpack('B', f.read(1))[0])
        el2 = lib.get_element(struct.unpack('B', f.read(1))[0])
        id1 = struct.unpack('H', f.read(2))[0]
        id2 = struct.unpack('H', f.read(2))[0]
        ax = struct.unpack('d', f.read(8))[0]
        ay = struct.unpack('d', f.read(8))[0]
        az = struct.unpack('d', f.read(8))[0]
        angle = struct.unpack('d', f.read(8))[0]
        length = struct.unpack('d', f.read(8))[0]

        if rotate_structure:
            bondrotation = build_rotation_matrix(np.array([ax,ay,az]), angle)
            newrotation = np.dot(rotation_matrix, bondrotation)
            axis, angle = get_axis_angle_from_matrix(newrotation)
            bonds.append([el1, el2, id1, id2, axis[0], axis[1], axis[2], angle, length])
        else:
            bonds.append([el1, el2, id1, id2, ax, ay, az, angle, length])

    # atoms expansion
    nr_atoms_expansion = struct.unpack('I', f.read(4))[0]
    atoms_expansion = []
    for i in range(0, nr_atoms_expansion):
        el = lib.get_element(struct.unpack('B', f.read(1))[0])
        x = struct.unpack('d', f.read(8))[0]
        y = struct.unpack('d', f.read(8))[0]
        z = struct.unpack('d', f.read(8))[0]
        if rotate_structure:
            p = np.array([x,y,z])
            pr = rotation_matrix.dot(p)
            atoms_expansion.append([el, pr[0], pr[1], pr[2]])
        else:
            atoms_expansion.append([el, x, y, z])

    # bonds expansion
    nr_bonds_expansion = struct.unpack('I', f.read(4))[0]
    bonds_expansion = []
    for i in range(0, nr_bonds_expansion):
        el1 = lib.get_element(struct.unpack('B', f.read(1))[0])
        el2 = lib.get_element(struct.unpack('B', f.read(1))[0])
        id1 = struct.unpack('H', f.read(2))[0]
        id2 = struct.unpack('H', f.read(2))[0]
        ax = struct.unpack('d', f.read(8))[0]
        ay = struct.unpack('d', f.read(8))[0]
        az = struct.unpack('d', f.read(8))[0]
        angle = struct.unpack('d', f.read(8))[0]
        length = struct.unpack('d', f.read(8))[0]

        if rotate_structure:
            bondrotation = build_rotation_matrix(np.array([ax,ay,az]), angle)
            newrotation = rotation_matrix.dot(bondrotation)
            axis, angle = get_axis_angle_from_matrix(newrotation)
            bonds_expansion.append([el1, el2, id1, id2, axis[0], axis[1], axis[2], angle, length])
        else:
            bonds_expansion.append([el1, el2, id1, id2, ax, ay, az, angle, length])

    return atoms, bonds, atoms_expansion, bonds_expansion, matrix

def build_molecule(xyzfile, data):
    """
    Import atoms based on .xyz file
    """
    lib = AtomSettings()

    atoms, bonds, atoms_expansion, bonds_expansion, matrix = read_binfile(xyzfile, lib, data)
    build_atoms(atoms, lib, data)
    build_bonds(atoms, bonds, lib, data)

    if data['expansion'] == True:
        build_atoms(atoms_expansion, lib, data)
        build_bonds(atoms + atoms_expansion, bonds_expansion, lib, data)

    return matrix

def show_unitcell(matrix, data):
    """
    Build a primitive cube that represents the unit cell

    Automatically multiplies cube vertex positions with the unit cell matrix to
    obtain the vertex positions in world space. The cube edges are marked to
    obtain dashed freestyle lines.
    """

    # check if unit cell needs to be rotated
    if "object_euler" in data.keys():
        rotate_structure = True
        angles = [float(angle) for angle in data["object_euler"].split("/")]
        print("Building rotation matrix from Euler angles: %s" % angles)
        rotation_matrix = matrix_euler_angles(angles[0], angles[1], angles[2])
    else:
        rotation_matrix = np.identity(3)

    # build unitcell
    bpy.ops.mesh.primitive_cube_add(size=1, enter_editmode=True, align='WORLD', location=(0,0,0))
    ob = bpy.context.object
    mesh = bmesh.from_edit_mesh(ob.data)
    mesh.verts.ensure_lookup_table()
    for i,v in enumerate(mesh.verts):
        newcoord = rotation_matrix.dot(np.array([v.co[0]+0.5, v.co[1]+0.5, v.co[2]+0.5]).dot(matrix))
        v.co = newcoord
    ob.location = rotation_matrix.dot(np.array([-0.5, -0.5, -0.5]).dot(matrix))
    material = bpy.data.materials.get('transparent')
    ob.data.materials.append(material)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.mesh.mark_freestyle_edge(clear=False)
    bpy.ops.object.mode_set(mode='OBJECT')

    # enable freestyle settings
    bpy.context.scene.render.use_freestyle = True
    freestyle_settings = bpy.context.window.view_layer.freestyle_settings
    lineset = freestyle_settings.linesets.active
    lineset.select_silhouette = False
    lineset.select_edge_mark = True
    lineset.select_border = False
    lineset.select_crease = False
    bpy.data.linestyles["LineStyle"].use_split_pattern = True
    bpy.data.linestyles["LineStyle"].split_dash1 = 3
    bpy.data.linestyles["LineStyle"].split_dash2 = 3
    bpy.data.linestyles["LineStyle"].split_dash3 = 3
    bpy.data.linestyles["LineStyle"].split_gap1 = 3
    bpy.data.linestyles["LineStyle"].split_gap2 = 3
    bpy.data.linestyles["LineStyle"].split_gap3 = 3
    bpy.data.linestyles["LineStyle"].caps = 'ROUND'

def build_rotation_matrix(rotation_data):
    """
    Build a 3x3 rotation matrix from rotation data
    """
    data = rotation_data.split(',')
    axis = [float(data[1]), float(data[2]), float(data[3])]
    angle = np.radians(float(data[0]))

    # Trig factors.
    ca = np.cos(angle)
    sa = np.sin(angle)
    C = 1.0 - ca

    # Depack the axis.
    x, y, z = axis

    # Multiplications (to remove duplicate calculations).
    xs = x*sa
    ys = y*sa
    zs = z*sa
    xC = x*C
    yC = y*C
    zC = z*C
    xyC = x*yC
    yzC = y*zC
    zxC = z*xC

    # Update the rotation matrix.
    matrix = np.zeros((3,3))
    matrix[0,0] = x*xC + ca
    matrix[0,1] = xyC - zs
    matrix[0,2] = zxC + ys
    matrix[1,0] = xyC + zs
    matrix[1,1] = y*yC + ca
    matrix[1,2] = yzC - xs
    matrix[2,0] = zxC - ys
    matrix[2,1] = yzC + xs
    matrix[2,2] = z*zC + ca

    return matrix

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
        rotation = [-np.pi/2.0, np.pi, 0.0]
    elif data['camera_direction'] == 'Y-':
        location = [0.0, -100.0, 0.0]
        rotation = [np.pi/2.0, 0.0, 0.0]

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

def build_atoms(atoms, lib, data):
    """
    Build atoms based on list of atoms
    """
    # build a high-poly sphere
    if 'nsubdiv' in data.keys():
        nsubdiv = data['nsubdiv']
    else:
        nsubdiv = 4
    bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=nsubdiv, location=(0,0,0))
    ob = bpy.context.object
    bpy.ops.object.shade_smooth()
    material = bpy.data.materials.get('specular')
    ob.data.materials.append(material)

    # construct atoms
    counter = 0
    atomlist = []
    for counter,at in enumerate(atoms):

        # copy materials
        bpy.data.materials[data['atmat']].copy().name = "atom%4i" % counter

        x = at[1]
        y = at[2]
        z = at[3]

        # change atom scale
        scale = lib.get_scale(at[0])
        if 'atom_radii' in data.keys():
            for mod in data['atom_radii']:
                pieces = mod.split('/')
                if pieces[0] == at[0] and (counter+1) >= int(pieces[1]) and (counter+1) <= int(pieces[2]):
                    scale = float(pieces[3])
                if pieces[0] == at[0] and int(pieces[1])==0 and int(pieces[2]) == 0:
                    scale = float(pieces[3])

        copy = ob.copy()
        copy.data = ob.data.copy()
        copy.location.x = x
        copy.location.y = y
        copy.location.z = z
        copy.scale.x = scale
        copy.scale.y = scale
        copy.scale.z = scale

        material = bpy.data.materials.get("atom%4i" % counter)
        copy.data.materials[0] = material
        color = lib.get_color(at[0])

        # change color based on specific directives
        scale = lib.get_scale(at[0])
        if 'atom_colors' in data.keys():
            for mod in data['atom_colors']:
                pieces = mod.split('/')
                if pieces[0] == at[0] and (counter+1) >= int(pieces[1]) and (counter+1) <= int(pieces[2]):
                    color = pieces[3]
                if pieces[0] == at[0] and int(pieces[1])==0 and int(pieces[2]) == 0:
                    color = pieces[3]

        atomlist.append(at[0])
        bpy.data.materials["atom%4i" % counter].node_tree.nodes["RGB"].outputs[0].default_value = hex2rgb(color, _tuple=True)

        bpy.context.collection.objects.link(copy)

    # remove high-poly sphere
    bpy.ops.object.select_all(action='DESELECT')
    bpy.data.objects['Icosphere'].select_set(True)
    bpy.ops.object.delete()

def build_bonds(atoms, bonds, lib, data):
    """
    Build bonds between atoms based on list of atoms
    """
    # build a high-poly cylinder
    bpy.ops.mesh.primitive_cylinder_add(vertices=128, location=(0,0,0))
    ob = bpy.context.object
    bpy.ops.object.shade_flat()
    material = bpy.data.materials.get('specular')
    ob.data.materials.append(material)

    for i,bond in enumerate(bonds):

        # establish diameter
        scale1 = lib.get_scale(bond[0])
        scale2 = lib.get_scale(bond[1])

        # overrule scales
        if 'atom_radii' in data.keys():
            for mod in data['atom_radii']:
                pieces = mod.split('/')
                if pieces[0] == bond[0] and (bond[2]+1) >= int(pieces[1]) and (bond[2]+1) <= int(pieces[2]):
                    scale1 = float(pieces[3])
                if pieces[0] == bond[1] and (bond[3]+1) >= int(pieces[1]) and (bond[3]+1) <= int(pieces[2]):
                    scale2 = float(pieces[3])
                if pieces[0] == bond[0] and int(pieces[1])==0 and int(pieces[2])==0:
                    scale1 = float(pieces[3])
                if pieces[0] == bond[1] and int(pieces[1])==0 and int(pieces[2])==0:
                    scale2 = float(pieces[3])

        diam = np.min([scale1, scale2]) / 2.0
        diam = min(diam, 0.4)

        # copy materials
        bpy.data.materials[data['bondmat']].copy().name = "bondu%4i" % i

        # upper tube
        copy = ob.copy()
        copy.data = ob.data.copy()

        if bond[0] == bond[1]:
            copy.location.x = (atoms[bond[2]][1] + atoms[bond[3]][1]) / 2.0
            copy.location.y = (atoms[bond[2]][2] + atoms[bond[3]][2]) / 2.0
            copy.location.z = (atoms[bond[2]][3] + atoms[bond[3]][3]) / 2.0
        else:
            copy.location.x = (3.0 * atoms[bond[2]][1] + atoms[bond[3]][1]) / 4.0
            copy.location.y = (3.0 * atoms[bond[2]][2] + atoms[bond[3]][2]) / 4.0
            copy.location.z = (3.0 * atoms[bond[2]][3] + atoms[bond[3]][3]) / 4.0

        copy.scale.x = diam
        copy.scale.y = diam

        if bond[0] == bond[1]:
            copy.scale.z = bond[8] / 2.0
        else:
            copy.scale.z = bond[8] / 4.0

        copy.rotation_mode = 'AXIS_ANGLE'
        copy.rotation_axis_angle[0] = bond[7]
        copy.rotation_axis_angle[1] = bond[4]
        copy.rotation_axis_angle[2] = bond[5]
        copy.rotation_axis_angle[3] = bond[6]
        material = bpy.data.materials.get("bondu%4i" % i)
        copy.data.materials[0] = material

        # get default color from library
        color = lib.get_color(atoms[bond[2]][0])

        # overrule color
        if 'atom_colors' in data.keys():
            for mod in data['atom_colors']:
                pieces = mod.split('/')
                if pieces[0] == atoms[bond[2]][0] and (bond[2]+1) >= int(pieces[1]) and (bond[2]+1) <= int(pieces[2]):
                    color = pieces[3]
                if pieces[0] == atoms[bond[2]][0] and int(pieces[1])==0 and int(pieces[2])==0:
                    color = pieces[3]

        bpy.data.materials["bondu%4i" % i].node_tree.nodes["RGB"].outputs[0].default_value = darken(hex2rgb(color, _tuple=True), 0.5)
        bpy.context.collection.objects.link(copy)

        # lower tube
        if bond[0] != bond[1]:
            copy = ob.copy()
            copy.data = ob.data.copy()

            copy.location.x = (atoms[bond[2]][1] + 3.0 * atoms[bond[3]][1]) / 4.0
            copy.location.y = (atoms[bond[2]][2] + 3.0 * atoms[bond[3]][2]) / 4.0
            copy.location.z = (atoms[bond[2]][3] + 3.0 * atoms[bond[3]][3]) / 4.0

            copy.scale.x = diam
            copy.scale.y = diam
            copy.scale.z = bond[8] / 4.0

            copy.rotation_mode = 'AXIS_ANGLE'
            copy.rotation_axis_angle[0] = bond[7]
            copy.rotation_axis_angle[1] = bond[4]
            copy.rotation_axis_angle[2] = bond[5]
            copy.rotation_axis_angle[3] = bond[6]
            material = bpy.data.materials.get("bondu%4i" % i)
            copy.data.materials[0] = material

            # get default color from library
            color = lib.get_color(atoms[bond[3]][0])

            # overrule color
            if 'atom_colors' in data.keys():
                for mod in data['atom_colors']:
                    pieces = mod.split('/')
                    if pieces[0] == atoms[bond[3]][0] and (bond[3]+1) >= int(pieces[1]) and (bond[3]+1) <= int(pieces[2]):
                        color = pieces[3]
                    if pieces[0] == atoms[bond[3]][0] and int(pieces[1])==0 and int(pieces[2])==0:
                        color = pieces[3]

            bpy.data.materials["bondu%4i" % i].node_tree.nodes["RGB"].outputs[0].default_value = darken(hex2rgb(color, _tuple=True), 0.5)
            bpy.context.collection.objects.link(copy)

    # remove cylinder
    bpy.ops.object.select_all(action='DESELECT')
    bpy.data.objects['Cylinder'].select_set(True)
    bpy.ops.object.delete()

def hex2rgb(_hex,_tuple=False):
    """
    @brief      Converts RGB code to numeric values

    @param      angle  the hex code
    @param      angle  whether to create a tuple

    @return     Numeric color values
    """
    string = _hex.lstrip('#') # if hex color starts with #, remove # char
    rgb255 = list(int(string[i:i+2], 16) for i in (0, 2 ,4))
    rgb_r = rgb255[0]/255.0;
    rgb_g = rgb255[1]/255.0;
    rgb_b = rgb255[2]/255.0;
    if _tuple == False:
        return [rgb_r, rgb_g, rgb_b]
    elif _tuple == True:
        return (rgb_r, rgb_g, rgb_b, 1.0)

def darken(rgb, value):
    rgb = list(rgb)
    value = 1.0 - value
    for i in range(0, len(rgb)):
        rgb[i] = rgb[i] * value
    return tuple(rgb)

def Rx(theta):
    """
    @brief      Create a rotational matrix for a rotation around the x-axis
    """
    return np.array([[ 1, 0           , 0           ],
                     [ 0, np.cos(theta),-np.sin(theta)],
                     [ 0, np.sin(theta), np.cos(theta)]])

def Ry(theta):
    """
    @brief      Create a rotational matrix for a rotation around the y-axis
    """
    return np.array([[ np.cos(theta), 0, np.sin(theta)],
                     [ 0           , 1, 0           ],
                     [-np.sin(theta), 0, np.cos(theta)]])

def Rz(theta):
    """
    @brief      Create a rotational matrix for a rotation around the z-axis
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
    return Rx(ax).dot(Ry(ay).dot(Rz(az)))

def matrix_billboard_atoms(atomlist, atoms):
    """
    @brief Build a matrix based on the normal vector spanned by
           a series of atoms and their center point.
    """
    ctr = np.array([0.0,0.0,0.0])
    for atnr in atomlist:
        p = np.array([atoms[atnr][1],atoms[atnr][2],atoms[atnr][3]])
        ctr += p
    ctr /= float(len(atomlist))

    avgnorm = np.array([0.0,0.0,0.0])
    for i in range(0, len(atomlist)):
        atnr = atomlist[i]
        v1 = ctr - np.array([atoms[atnr][1],atoms[atnr][2],atoms[atnr][3]])
        atnr = atomlist[(i+1)%len(atomlist)]
        v2 = ctr - np.array([atoms[atnr][1],atoms[atnr][2],atoms[atnr][3]])
        v3 = np.cross(v1, v2)
        v3 = v3 / np.linalg.norm(v3)
        avgnorm += v3

    avgnorm = avgnorm / np.linalg.norm(avgnorm)
    axis, angle = align_vector(avgnorm, np.array([0.0,0.0,1.0]))

    return build_rotation_matrix(axis, angle)

def align_vector(origin, target):
    """
    @brief Produce axis angle for rotation

    Assumes that input vectors are already normalized
    """
    angle = np.arccos(origin.dot(target))
    axis = np.cross(origin, target)
    axis /= np.linalg.norm(axis)

    return axis, angle


def build_rotation_matrix(axis, angle):
    """
    @brief Build a rotation matrix from an axis-angle combination

    @param axis     np array axis (normalized!)
    @param angle    angle in radians

    Returns np array rotation matrix
    """
    axis = axis / np.linalg.norm(axis)
    a = np.cos(angle / 2.0)
    b, c, d = -axis * np.sin(angle / 2.0)
    aa, bb, cc, dd = a * a, b * b, c * c, d * d
    bc, ad, ac, ab, bd, cd = b * c, a * d, a * c, a * b, b * d, c * d
    return np.array([[aa + bb - cc - dd, 2 * (bc + ad), 2 * (bd - ac)],
                     [2 * (bc - ad), aa + cc - bb - dd, 2 * (cd + ab)],
                     [2 * (bd + ac), 2 * (cd - ab), aa + dd - bb - cc]])

def get_axis_angle_from_matrix(rotmat):
    """
    Calculate axis and angle from rotation matrix
    """
    # calculate axis
    axis = np.zeros(3, np.float64)
    axis[0] = rotmat[2,1] - rotmat[1,2]
    axis[1] = rotmat[0,2] - rotmat[2,0]
    axis[2] = rotmat[1,0] - rotmat[0,1]

    # calculate angle
    r = np.hypot(axis[0], np.hypot(axis[1], axis[2]))
    t = rotmat[0,0] + rotmat[1,1] + rotmat[2,2]
    theta = np.arctan2(r, t-1)

    # normalize the axis
    axis = axis / r

    return axis, theta

class AtomSettings:
    """
    Specialty class that loads properties for atoms
    """
    def __init__(self):
        # load colores and indices
        dir_path = os.path.dirname(os.path.realpath(__file__))
        with open(os.path.join(dir_path, 'atoms.json')) as f:
            self.data = json.load(f)['atoms']

        # load distances
        self.distances = np.full((86,86), 3.0)
        for i in range(0, 86):
            self.distances[i][1] = 1.4
            self.distances[1][i] = 1.4

            if i > 20:
                for j in range(2,20):
                    self.distances[i][j] = 2.8
                    self.distances[j][i] = 2.8
            else:
                for j in range(2,20):
                    self.distances[i][j] = 2.0
                    self.distances[j][i] = 2.0

    def get_color(self, at):
        return self.data['colors'][at]

    def get_element(self, atnr):
        return self.data['nr2element'][str(atnr)]

    def get_atom_id(self, at):
        return self.data['atomid'][at]

    def get_bond_distance(self, at1, at2):
        atid1 = self.get_atom_id(at1)
        atid2 = self.get_atom_id(at2)

        if atid1 > atid2:
            atid1, atid2 = atid2, atid1

        return self.distances[atid1, atid2]

    def get_scale(self, el):
        if el in self.data['radii']:
            return float(self.data['radii'][el])
        else:
            raise Exception('Could not find element: %s' % el)

def enable_cycles_gpu():
    prefs = bpy.context.preferences
    cycles_prefs = prefs.addons['cycles'].preferences

    # Force device discovery
    cycles_prefs.get_devices()

    # Prefer OPTIX, fallback to CUDA
    if 'OPTIX' in cycles_prefs.compute_device_type:
        cycles_prefs.compute_device_type = 'OPTIX'
    else:
        cycles_prefs.compute_device_type = 'CUDA'

    for device in cycles_prefs.devices:
        device.use = True
        print(f"Using device: {device.name} ({device.type})")

if __name__ == '__main__':
    main()
