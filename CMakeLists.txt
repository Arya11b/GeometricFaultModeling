
cmake_minimum_required(VERSION 3.16)
project(SketchBasedHRBF)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Libigl
option(LIBIGL_COPYLEFT_CORE    "Build target igl_copyleft::core"    ON)
option(LIBIGL_COPYLEFT_CGAL    "Build target igl_copyleft::cgal"    ON)
option(LIBIGL_EMBREE           "Build target igl::embree"           ON)
option(LIBIGL_GLFW             "Build target igl::glfw"             ON)
option(LIBIGL_IMGUI            "Build target igl::imgui"            ON)
option(LIBIGL_OPENGL           "Build target igl::opengl"           ON)
include(libigl)
find_package(OpenMP)


# Add your project files
set(SRCFILES
        src/main.cpp
        src/fault.cpp src/fault.hpp
        src/fcurve.hpp src/fcurve.cpp
        src/faultdb.cpp src/faultdb.hpp
        src/faultcwidget.hpp src/faultcwidget.cpp
        src/viewer.cpp src/viewer.hpp
        src/hrbf.cpp src/hrbf.hpp
        src/menu.hpp src/menu.cpp
        src/drawable.hpp src/drawable.cpp
        src/implicit.hpp src/implicit.cpp
        src/object.hpp src/object.cpp
        src/sketchcwidget.hpp src/sketchcwidget.cpp
        src/cwidget.hpp src/cwidget.cpp
        src/boundingbox.hpp src/boundingbox.cpp
        src/sketchplane.hpp src/sketchplane.cpp
        src/boundingboxcwidget.hpp src/boundingboxcwidget.cpp
        src/defs/rawparser.hpp
        src/defs/imvec2misc.hpp
        src/defs/eigen_curveN_conversions.hpp
        src/geo/extract_non_manifold_edge_curves.hpp
        src/geo/separate.hpp
        src/geo/reorder_curve_edgelist.hpp
        src/geo/marching_squares.hpp
        src/functions/calculate_intersection_curves.hpp src/functions/separate_object_by_interpolant.hpp
        src/tree.hpp src/tree.cpp
        )

add_library(sketchlib INTERFACE) #interface is for header only (doesn't compile stuff beforehand)
target_include_directories(sketchlib INTERFACE libs/SketchLib/Include/)

add_library(nanoflann INTERFACE) #interface is for header only (doesn't compile stuff beforehand)
target_include_directories(nanoflann INTERFACE libs/nanoflann/Include/)

add_library(houghnormal INTERFACE) #interface is for header only (doesn't compile stuff beforehand)
target_include_directories(houghnormal INTERFACE libs/HoughNormalLib/Include/)

# Add your project files
add_executable(${PROJECT_NAME} ${SRCFILES})

target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX igl::embree igl::core igl_copyleft::core igl_copyleft::cgal igl::glfw igl::imgui sketchlib)