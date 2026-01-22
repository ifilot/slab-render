/********************************************************************************
 * This file is part of Saucepan                                                *
 *                                                                              *
 * Author: Ivo Filot <i.a.w.filot@tue.nl>                                       *
 *                                                                              *
 * This program is free software; you can redistribute it and/or                *
 * modify it under the terms of the GNU Lesser General Public                   *
 * License as published by the Free Software Foundation; either                 *
 * version 3 of the License, or (at your option) any later version.             *
 *                                                                              *
 * This program is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 * Lesser General Public License for more details.                              *
 *                                                                              *
 * You should have received a copy of the GNU Lesser General Public License     *
 * along with this program; if not, write to the Free Software Foundation,      *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ********************************************************************************/

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "atom.h"

class Bond {
public:
    // atom 1
    Atom atom1;
    Atom atom2;

    uint16_t atom_id_1;
    uint16_t atom_id_2;

    // length of the bond
    double length;

    // bond orientation
    Vec3d direction;
    Vec3d axis;
    double angle;

    Bond(const Atom& _atom1, const Atom& _atom2, uint16_t i, uint16_t j);

private:
};