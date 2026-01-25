cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0167 NEW)

project(slabrender LANGUAGES CXX)

# --------------------
# C++ standard
# --------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# --------------------
# Qt5
# --------------------
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    OpenGL
)

# --------------------
# Dependencies
# --------------------
find_package(Boost REQUIRED COMPONENTS
    filesystem
    regex
    iostreams
)

find_package(Eigen3 REQUIRED)
find_package(glm REQUIRED)

# --------------------
# Sources
# --------------------
add_executable(slabrender WIN32
    src/atom.cpp
    src/atom_settings.cpp
    src/bond.cpp
    src/jobinfowidget.cpp
    src/logwindow.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/structure.cpp
    src/structure_loader.cpp
    src/threadrenderimage.cpp
    src/vendor/simpleson/json.cpp
    src/visualization/anaglyph_widget.cpp
    src/visualization/model.cpp
    src/visualization/model_loader.cpp
    src/visualization/primitivebuilder.cpp
    src/visualization/shader_program.cpp
    src/visualization/shader_program_manager.cpp

    src/atom.h
    src/atom_settings.h
    src/bond.h
    src/config.h
    src/jobinfowidget.h
    src/logwindow.h
    src/mainwindow.h
    src/matrixmath.h
    src/structure.h
    src/structure_loader.h
    src/threadrenderimage.h
    src/vendor/simpleson/json.h
    src/visualization/anaglyph_widget.h
    src/visualization/model.h
    src/visualization/model_loader.h
    src/visualization/primitivebuilder.h
    src/visualization/shader_program.h
    src/visualization/shader_program_manager.h
    src/visualization/shader_program_types.h
)

# --------------------
# Qt resources
# --------------------
qt5_add_resources(RESOURCES resources.qrc)
target_sources(slabrender PRIVATE ${RESOURCES})

# --------------------
# Compile definitions
# --------------------
target_compile_definitions(slabrender PRIVATE
    BOOST_BIND_GLOBAL_PLACEHOLDERS
)

# --------------------
# Includes
# --------------------
target_include_directories(slabrender PRIVATE src)

# --------------------
# Link libraries
# --------------------
target_link_libraries(slabrender PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    Boost::filesystem
    Boost::regex
    Boost::iostreams
    Eigen3::Eigen
    glm::glm
)

# --------------------
# Windows-specific
# --------------------
if (MINGW)
    target_link_libraries(slabrender PRIVATE ws2_32 bcrypt)
endif()

if (WIN32)
    target_sources(slabrender PRIVATE slabrender.rc)
endif()

# --------------------
# Install
# --------------------
install(TARGETS slabrender
    RUNTIME DESTINATION bin
)
