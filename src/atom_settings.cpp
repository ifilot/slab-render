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
#include "atom_settings.h"

/**
 * @brief      Constructs a new instance.
 */
AtomSettings::AtomSettings() {
    // grab settings file from assets and read it
    QFile file(":/assets/configuration/atoms.json");
    if(!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Could not read atoms.json file from assets");
    }

    QTemporaryDir dir;
    QString spath = dir.path() + "/atoms.json";
    file.copy(spath);
    this->settings_file = spath.toStdString();
    this->reset();
}

void AtomSettings::reset() {
    this->load();

    // set all bonds by default to 3.0
    this->bond_distances.resize(121);
    for(unsigned int i=0; i<121; i++) {
        this->bond_distances[i].resize(121, 2.5);
    }

    // loop over all atoms
    for(unsigned int i=0; i<121; i++) {
        if(i > 20) {
            for(unsigned int j=2; j<=20; j++) {
                // bonds for hydrogen
                this->bond_distances[i][1] = 2.0;
                this->bond_distances[1][i] = 2.0;

                // other atoms
                this->bond_distances[i][j] = 2.2;
                this->bond_distances[j][i] = 2.2;
            }
        } else {
            for(unsigned int j=2; j<=20; j++) {
                // bonds for hydrogen
                this->bond_distances[i][1] = 1.2;
                this->bond_distances[1][i] = 1.2;

                // other atoms
                this->bond_distances[i][j] = 2.0;
                this->bond_distances[j][i] = 2.0;
            }
        }
    }

    // add some special cases on the basis of user input
    this->bond_distances[6][13] = 3.5; // Al-C
    this->bond_distances[13][6] = 3.5;

    this->radii.resize(119);
    this->colors.resize(119);
    for(unsigned int i=1; i<=118; i++) {
        this->radii[i] = this->get_atom_radius(this->get_name_from_elnr(i));
        this->colors[i] = this->hexcode_to_vector3d(this->get_atom_color(this->get_name_from_elnr(i)).substr(1,6));
    }
}

/**
 * @brief overwrite data using json
 * @param data
 */
void AtomSettings::overwrite(const std::string& data) {
    qDebug() << "Reconfiguring AtomSettings data";
    boost::property_tree::ptree troot;
    try {
        std::stringstream ss;
        ss << "{";

        // do not read trailing comma if present
        if(data.back() == ',') {
            ss << data.substr(0, data.size()-1);
        } else {
            ss << data;
        }

        ss << "}";
        boost::property_tree::read_json(ss, troot);
        auto pt = troot.get_child("bond_distances");
        for(boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); iter++) {
            //qDebug() << iter->second.get_value<std::string>().c_str();
            std::string item = iter->second.get_value<std::string>();
            std::vector<std::string> pieces;
            boost::split(pieces, item, boost::is_any_of("/"));

            std::string atom0 = pieces[0];
            std::string atom1 = pieces[1];
            float dist = boost::lexical_cast<float>(pieces[2]);

            unsigned int atom_id0 = this->get_atom_elnr(atom0);
            unsigned int atom_id1 = this->get_atom_elnr(atom1);

            this->bond_distances[atom_id0][atom_id1] = dist;
            this->bond_distances[atom_id1][atom_id0] = dist;

            qDebug() << "Overwring bond distances " << atom0.c_str() << "-"
                     << atom1.c_str() << ": " << pieces[2].c_str() << " angstrom.";
        }
    }  catch (const std::exception& e) {
        qDebug() << "Error encountered in parsing JSON string: " << e.what();
    }
}

/**
 * @brief      Load the JSON file and parse its contents
 */
void AtomSettings::load() {
    try {
        qDebug() << "Reading " << this->settings_file.c_str();
        boost::property_tree::read_json(this->settings_file, this->root);
    } catch(std::exception const& ex) {
        std::cerr << "[ERROR] There was an error parsing the JSON tree" << std::endl;
        std::cerr << ex.what() << std::endl;
        std::cerr << "[ERROR] Terminating program" << std::endl;
        exit(-1);
    }
}

/**
 * @brief      Get the atomic radius of an element
 *
 * @param[in]  elname  Element name
 *
 * @return     atomic radius
 */
float AtomSettings::get_atom_radius(const std::string& elname){
    std::string radius = this->root.get<std::string>("atoms.radii." + elname);
    return boost::lexical_cast<float>(radius);
}

/**
 * @brief      Get the color of an element
 *
 * @param[in]  elname  Element name
 *
 * @return     atomic radius
 */
std::string AtomSettings::get_atom_color(const std::string& elname){
    return this->root.get<std::string>("atoms.colors." + elname);
}

/**
 * @brief      Get the atomic radius of an element
 *
 * @param[in]  elname  Element name
 *
 * @return     atomic radius
 */
float AtomSettings::get_atom_radius_from_elnr(unsigned int elnr) {
    return this->radii[elnr];
}

/**
 * @brief      Get element number of an element
 *
 * @param[in]  elname  Element name
 *
 * @return     The atom elnr.
 */
unsigned int AtomSettings::get_atom_elnr(const std::string& elname){
    std::string elnr = this->root.get<std::string>("atoms.elnr." + elname);
    return boost::lexical_cast<unsigned int>(elnr);
}

/**
 * @brief      Get the maximum bond distance between two atoms
 *
 * @param[in]  atoma  The atoma
 * @param[in]  atomb  The atomb
 *
 * @return     The bond distance.
 */
double AtomSettings::get_bond_distance(int atoma, int atomb) {
    return this->bond_distances[atoma][atomb];
}

/**
 * @brief      Gets the name from element number.
 *
 * @param[in]  elnr  The elnr
 *
 * @return     The name from elnr.
 */
std::string AtomSettings::get_name_from_elnr(unsigned int elnr) const {
    return this->root.get<std::string>("atoms.nr2element." + boost::lexical_cast<std::string>(elnr));
}

const QVector3D& AtomSettings::get_atom_color_from_elnr(unsigned int elnr) const {
    return this->colors[elnr];
}

QVector3D AtomSettings::hexcode_to_vector3d(const std::string& hexcode) const {
    if(hexcode.size() != 6) {
        throw std::runtime_error("Invalid hexcode received: " + hexcode);
    }

    float r = strtoul(hexcode.substr(0, 2).c_str(), NULL, 16) / 255.f;
    float g = strtoul(hexcode.substr(2, 2).c_str(), NULL, 16) / 255.f;
    float b = strtoul(hexcode.substr(4, 2).c_str(), NULL, 16) / 255.f;

    return QVector3D(r,g,b);
}
