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

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <QDebug>

#include "atom_settings.h"
#include "structure.h"

enum OutcarReadStatus {
    VASP_OUTCAR_READ_STATE_UNDEFINED,
    VASP_OUTCAR_READ_STATE_ELEMENTS,
    VASP_OUTCAR_READ_STATE_IONS_PER_ELEMENT,
    VASP_OUTCAR_READ_STATE_LATTICE_VECTORS,
    VASP_OUTCAR_READ_STATE_ATOMS,
    VASP_OUTCAR_READ_STATE_OPEN,
    VASP_OUTCAR_READ_STATE_FINISHED
};

class StructureLoader {
private:

public:
    /**
     * @brief      Constructs a new instance.
     */
    StructureLoader();

    std::vector<std::shared_ptr<Structure>> load_file(const QString& path);

    /**
     * @brief      Load structure from OUTCAR file
     *
     * @param[in]  filename  The filename
     *
     * @return     Structures
     */
    std::vector<std::shared_ptr<Structure>> load_outcar(const std::string& filename);

    /**
     * @brief      Loads a structure from a binary structure pack file
     *
     * @param[in]  filename  The filename
     *
     * @return     Structure
     */
    std::vector<std::shared_ptr<Structure>> load_structurepack(const std::string& filename);

    /**
     * @brief      Load structure from POSCAR file
     *
     * @param[in]  filename  The filename
     *
     * @return     Structure
     */
    std::vector<std::shared_ptr<Structure>> load_poscar(const std::string& filename);

    /**
     * @brief      Load structure from ADF logfile
     *
     * @param[in]  filename  The filename
     *
     * @return     Structure
     */
    std::vector<std::shared_ptr<Structure>> load_adf_logfile(const std::string& filename);

    /**
     * @brief      Load structure from Gaussian logfile
     *
     * @param[in]  filename  The filename
     *
     * @return     Structure
     */
    std::vector<std::shared_ptr<Structure>> load_gaussian_logfile(const std::string& filename);

    /**
     * @brief      Load structure from ANNP DATA file
     *
     * @param[in]  filename  The filename
     *
     * @return     Structure
     */
    std::vector<std::shared_ptr<Structure>> load_data(const std::string& filename);

private:
};
