# Define project
cmake_minimum_required(VERSION 2.8.11)
project(CG1Exercise4)

if(${CMAKE_VERSION} VERSION_EQUAL "3.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.0")
    cmake_policy(SET CMP0043 NEW)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set target name
set(target42 Viewer)


# Include QT5 libraries
set(CMAKE_AUTOMOC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)

if(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=gnu++11)
elseif(APPLE)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
elseif(UNIX)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=gnu++11)
endif()

find_package(Qt5Widgets REQUIRED)

# Define source files
set(util_sources
    src/util/objio.cpp
    src/util/objio.h
    src/util/polygonaldrawable.cpp
    src/util/polygonaldrawable.h
    src/util/abstractexercise.cpp
    src/util/abstractexercise.h
    src/util/abstractpainter.h
    src/util/abstractpainter.cpp
    src/util/cachedvalue.h
    src/util/cachedvalue.hpp
    src/util/camera.h
    src/util/camera.cpp
    src/util/fileassociatedasset.h
    src/util/fileassociatedasset.cpp
    src/util/fileassociatedshader.h
    src/util/fileassociatedshader.cpp
    src/util/glviewer.h
    src/util/glviewer.cpp
    src/util/icosahedron.cpp
    src/util/icosahedron.h
    src/util/openglfunctions.h
    src/util/paintermode.h
    src/util/unitcube.cpp
    src/util/unitcube.h
    src/util/wire.h
    src/util/wire.cpp
    src/util/wirecreator.h
    src/util/wirecreator.cpp
)



set(target42_sources
    src/viewer.cpp
)





# Create util library
add_library(util STATIC ${util_sources})

# Create executable
add_executable(${target42} ${target42_sources})


# Create executable
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# Use the Widgets and OpenGL module from Qt 5
target_link_libraries(util Qt5::Widgets)
target_link_libraries(${target42} util Qt5::Widgets)
