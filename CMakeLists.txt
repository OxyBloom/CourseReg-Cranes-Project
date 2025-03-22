cmake_minimum_required(VERSION 3.10)
project(CourseReg C)

# Find GTK3
find_package(PkgConfig REQUIRED)
find_package(SQLite3 REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Add sources
add_executable(course_reg 
    src/main.c 
    src/gui.c 
    src/database.c 
    # src/utils.c
)

# Include directories
target_include_directories(course_reg PRIVATE 
    ${GTK3_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
    include
)

# Link libraries
target_link_libraries(course_reg PRIVATE ${GTK3_LIBRARIES} SQLite::SQLite3)

# Compiler flags
target_compile_options(course_reg PRIVATE ${GTK3_CFLAGS_OTHER})
