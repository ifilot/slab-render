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

#include "bond.h"

Bond::Bond(const Atom& _atom1, const Atom& _atom2, uint16_t i, uint16_t j) :
atom1(_atom1),
atom2(_atom2),
atom_id_1(i),
atom_id_2(j) {
    Vec3d v = this->atom2.get_vector_pos() - this->atom1.get_vector_pos();

    this->length = v.norm();
    this->direction = v.normalized();

    // avoid gimball locking
    if (fabs(this->direction[2]) > .999) {
        if(this->direction[2] < 0.0) {
            this->axis = Vec3d(0.0, 1.0, 0.0);
            this->angle = -M_PI;
        } else {
            this->axis = Vec3d(0.0, 0.0, 1.0);
            this->angle = 0.0;
        }
    } else {
        this->axis = Vec3d(0.0, 0.0, 1.0).cross(this->direction);
        this->angle = std::acos(this->direction[2]);
    }
}