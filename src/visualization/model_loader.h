/********************************************************************************
 * This file is part of SlabRender                                              *
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

#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>
#include <QFile>
#include <QDataStream>

#include "model.h"

class ModelLoader {

private:

public:
   /**
    * @brief ModelLoader.
    */
    ModelLoader();

    /**
     * @brief load_model.
     */
    std::unique_ptr<Model> load_model(const std::string& path);

private:
    /**
     * @brief      Load object data from obj file
     *
     * @param[in]  path   Path to file
     */
    std::unique_ptr<Model> load_data_obj(const std::string& path);

    /**
     * @brief      Load object data from ply file
     *
     * @param[in]  path   Path to file
     */
    std::unique_ptr<Model> load_data_ply(const std::string& path);

    /**
     * @brief      Loads a ply file from hard drive stored as little endian binary
     *
     * @param[in]  path   Path to file
     */
    std::unique_ptr<Model> load_data_ply_binary(const std::string& path);

    /**
     * @brief      Loads a ply file from hard drive stored as little endian binary
     *
     * @param[in]  path   Path to file
     */
    std::unique_ptr<Model> load_data_ply_ascii(const std::string& path);
};
