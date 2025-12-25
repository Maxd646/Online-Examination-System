cmake_minimum_required(VERSION 3.16)
project(OnlineExaminationSystem VERSION 2.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Find SQLite3
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Include directories
include_directories(src)

# Source files for Qt GUI application
set(SOURCES
    src/gui/main_gui.cpp
    src/gui/windows/LoginWindow.cpp
    src/gui/windows/TeacherWindow.cpp
    src/gui/windows/StudentWindow.cpp
    src/authentication/user.cpp
    src/authentication/simple_auth.cpp
    src/database/database.cpp
    src/database/exam_types.cpp
    src/structure/utils.cpp
)

# Main executable (Qt GUI)
add_executable(exam_system ${SOURCES})
target_link_libraries(exam_system 
    Qt6::Core 
    Qt6::Widgets 
    ${SQLITE3_LIBRARIES}
)
target_include_directories(exam_system PRIVATE ${SQLITE3_INCLUDE_DIRS})

# Set output directory
set_target_properties(exam_system
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build
)

# Copy database directory to build output directory
file(COPY database DESTINATION ${CMAKE_BINARY_DIR}/build)