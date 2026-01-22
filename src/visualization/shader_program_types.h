/****************************************************************************
 *                                                                          *
 *   Rubriks Cube                                                           *
 *   Copyright (C) 2022 Ivo Filot <ivo@ivofilot.nl>                         *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as         *
 *   published by the Free Software Foundation, either version 3 of the     *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public license      *
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>. *
 *                                                                          *
 ****************************************************************************/

#ifndef _SHADER_PROGRAM_TYPES_H
#define _SHADER_PROGRAM_TYPES_H

// Define different shader program types so that each type can be linked to a
// set of uniforms
enum class ShaderProgramType {
    ModelShader,
    StereoscopicShader,
    AxesShader,
    UnitcellShader
};

#endif
